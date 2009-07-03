/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Philip Withnall 2009 <philip@tecnocode.co.uk>
 *
 * GData Client is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * GData Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GData Client.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * SECTION:gdata-parsable
 * @short_description: GData parsable object
 * @stability: Unstable
 * @include: gdata/gdata-parsable.h
 *
 * #GDataParsable is an abstract class allowing easy implementation of an extensible parser. It is primarily extended by #GDataFeed and #GDataEntry,
 * both of which require XML parsing which can be extended by subclassing.
 *
 * It allows methods to be defined for handling the root XML node, each of its child nodes, and a method to be called after parsing is complete.
 **/

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <string.h>
#include <libxml/parser.h>

#include "gdata-parsable.h"
#include "gdata-private.h"
#include "gdata-parser.h"

GQuark
gdata_parser_error_quark (void)
{
	return g_quark_from_static_string ("gdata-parser-error-quark");
}

static void gdata_parsable_finalize (GObject *object);
static gboolean real_parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error);

struct _GDataParsablePrivate {
	GString *extra_xml;
	GHashTable *extra_namespaces;
};

G_DEFINE_ABSTRACT_TYPE (GDataParsable, gdata_parsable, G_TYPE_OBJECT)
#define GDATA_PARSABLE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_PARSABLE, GDataParsablePrivate))

static void
gdata_parsable_class_init (GDataParsableClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataParsablePrivate));
	gobject_class->finalize = gdata_parsable_finalize;
	klass->parse_xml = real_parse_xml;
}

static void
gdata_parsable_init (GDataParsable *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_PARSABLE, GDataParsablePrivate);
	self->priv->extra_xml = g_string_new ("");
	self->priv->extra_namespaces = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
}

static void
gdata_parsable_finalize (GObject *object)
{
	GDataParsablePrivate *priv = GDATA_PARSABLE_GET_PRIVATE (object);

	g_string_free (priv->extra_xml, TRUE);
	g_hash_table_destroy (priv->extra_namespaces);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_parsable_parent_class)->finalize (object);
}

static gboolean
real_parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error)
{
	xmlBuffer *buffer;
	xmlNs **namespaces, **namespace;

	/* Unhandled XML */
	buffer = xmlBufferCreate ();
	xmlNodeDump (buffer, doc, node, 0, 0);
	g_string_append (parsable->priv->extra_xml, (gchar*) xmlBufferContent (buffer));
	g_message ("Unhandled XML in %s: %s", G_OBJECT_TYPE_NAME (parsable), (gchar*) xmlBufferContent (buffer));
	xmlBufferFree (buffer);

	/* Get the namespaces */
	namespaces = xmlGetNsList (doc, node);
	if (namespaces == NULL)
		return TRUE;

	for (namespace = namespaces; *namespace != NULL; namespace++) {
		if ((*namespace)->prefix != NULL) {
			g_hash_table_insert (parsable->priv->extra_namespaces,
					     g_strdup ((gchar*) ((*namespace)->prefix)),
					     g_strdup ((gchar*) ((*namespace)->href)));
		}
	}
	xmlFree (namespaces);

	return TRUE;
}

/**
 * gdata_parsable_new_from_xml:
 * @parsable_type: the type of the class represented by the XML
 * @xml: the XML for just the parsable object, with full namespace declarations
 * @length: the length of @xml, or -1
 * @error: a #GError, or %NULL
 *
 * Creates a new #GDataParsable subclass (of the given @parsable_type) from the given @xml.
 *
 * An object of the given @parsable_type is created, and its <function>pre_parse_xml</function>, <function>parse_xml</function> and
 * <function>post_parse_xml</function> class functions called on the XML tree obtained from @xml. <function>pre_parse_xml</function> and
 * <function>post_parse_xml</function> are called once each on the root node of the tree, while <function>parse_xml</function> is called for each of the
 * child nodes of the root node.
 *
 * If @length is -1, @xml will be assumed to be null-terminated.
 *
 * If an error occurs during parsing, a suitable error from #GDataParserError will be returned.
 *
 * Return value: a new #GDataParsable, or %NULL; unref with g_object_unref()
 **/
GDataParsable *
gdata_parsable_new_from_xml (GType parsable_type, const gchar *xml, gint length, GError **error)
{
	return _gdata_parsable_new_from_xml (parsable_type, xml, length, NULL, error);
}

GDataParsable *
_gdata_parsable_new_from_xml (GType parsable_type, const gchar *xml, gint length, gpointer user_data, GError **error)
{
	xmlDoc *doc;
	xmlNode *node;

	g_return_val_if_fail (g_type_is_a (parsable_type, GDATA_TYPE_PARSABLE) == TRUE, FALSE);
	g_return_val_if_fail (xml != NULL, NULL);
	g_return_val_if_fail (length >= -1, NULL);

	if (length == -1)
		length = strlen (xml);

	/* Parse the XML */
	doc = xmlReadMemory (xml, length, "/dev/null", NULL, 0);
	if (doc == NULL) {
		xmlError *xml_error = xmlGetLastError ();
		g_set_error (error, GDATA_PARSER_ERROR, GDATA_PARSER_ERROR_PARSING_STRING,
			     /* Translators: the parameter is an error message */
			     _("Error parsing XML: %s"),
			     xml_error->message);
		return NULL;
	}

	/* Get the root element */
	node = xmlDocGetRootElement (doc);
	if (node == NULL) {
		/* XML document's empty */
		xmlFreeDoc (doc);
		g_set_error (error, GDATA_PARSER_ERROR, GDATA_PARSER_ERROR_EMPTY_DOCUMENT,
			     _("Error parsing XML: %s"),
			     /* Translators: this is a dummy error message to be substituted into "Error parsing XML: %s". */
			     _("Empty document."));
		return NULL;
	}

	return _gdata_parsable_new_from_xml_node (parsable_type, doc, node, user_data, error);
}

GDataParsable *
_gdata_parsable_new_from_xml_node (GType parsable_type, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error)
{
	GDataParsable *parsable;
	GDataParsableClass *klass;

	g_return_val_if_fail (g_type_is_a (parsable_type, GDATA_TYPE_PARSABLE) == TRUE, FALSE);
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);

	parsable = g_object_new (parsable_type, NULL);

	klass = GDATA_PARSABLE_GET_CLASS (parsable);
	if (klass->parse_xml == NULL)
		return FALSE;

	/*if (xmlStrcmp (node->name, (xmlChar*) klass->element_name) != 0 ||
	    (node->ns != NULL && xmlStrcmp (node->ns->prefix, (xmlChar*) klass->element_namespace) != 0)) {
		 /*No <entry> element (required) 
		xmlFreeDoc (doc);
		gdata_parser_error_required_element_missing (klass->element_name, "root", error);
		return NULL;
	}*/

	/* Call the pre-parse function first */
	if (klass->pre_parse_xml != NULL &&
	    klass->pre_parse_xml (parsable, doc, node, user_data, error) == FALSE) {
		g_object_unref (parsable);
		return NULL;
	}	

	/* Parse each child element */
	node = node->children;
	while (node != NULL) {
		if (klass->parse_xml (parsable, doc, node, user_data, error) == FALSE) {
			g_object_unref (parsable);
			return NULL;
		}
		node = node->next;
	}

	/* Call the post-parse function */
	if (klass->post_parse_xml != NULL &&
	    klass->post_parse_xml (parsable, user_data, error) == FALSE) {
		g_object_unref (parsable);
		return NULL;
	}

	return parsable;
}

static void
build_namespaces_cb (gchar *prefix, gchar *href, GString *output)
{
	g_string_append_printf (output, " xmlns:%s='%s'", prefix, href);
}

static gboolean
filter_namespaces_cb (gchar *prefix, gchar *href, GHashTable *canonical_namespaces)
{
	if (g_hash_table_lookup (canonical_namespaces, prefix) != NULL)
		return TRUE;
	return FALSE;
}

/**
 * gdata_parsable_get_xml:
 * @self: a #GDataParsable
 *
 * Builds an XML representation of the #GDataParsable in its current state, such that it could be inserted on the server. The XML is guaranteed to have
 * all its namespaces declared properly in a self-contained fashion, and is valid for stand-alone use.
 *
 * Return value: the object's XML; free with g_free()
 **/
gchar *
gdata_parsable_get_xml (GDataParsable *self)
{
	return _gdata_parsable_get_xml (self, TRUE);
}

/*
 * _gdata_parsable_get_xml:
 * @self: a #GDataParsable
 * @declare_namespaces: %TRUE if all the namespaces used in the outputted XML should be declared in the opening tag of the root element, %FALSE otherwise
 *
 * Builds an XML representation of the #GDataParsable in its current state, such that it could be inserted on the server. If @declare_namespaces is
 * %TRUE, the XML is guaranteed to have all its namespaces declared properly in a self-contained fashion, and is valid for stand-alone use. If
 * @declare_namespaces is %FALSE, none of the used namespaces are declared, and the XML is suitable for insertion into a larger XML tree.
 *
 * Return value: the object's XML; free with g_free()
 */
gchar *
_gdata_parsable_get_xml (GDataParsable *self, gboolean declare_namespaces)
{
	GDataParsableClass *klass;
	GString *xml_string;
	guint length;
	GHashTable *namespaces = NULL; /* shut up, gcc */

	klass = GDATA_PARSABLE_GET_CLASS (self);
	g_assert (klass->element_name != NULL);

	/* Get the namespaces the class uses */
	if (declare_namespaces == TRUE && klass->get_namespaces != NULL) {
		namespaces = g_hash_table_new (g_str_hash, g_str_equal);
		klass->get_namespaces (self, namespaces);

		/* Remove any duplicate extra namespaces */
		g_hash_table_foreach_remove (self->priv->extra_namespaces, (GHRFunc) filter_namespaces_cb, namespaces);
	}

	/* Build up the namespace list */
	xml_string = g_string_sized_new (100);
	if (klass->element_namespace != NULL)
		g_string_append_printf (xml_string, "<%s:%s", klass->element_namespace, klass->element_name);
	else
		g_string_append_printf (xml_string, "<%s", klass->element_name);

	/* We only include the normal namespaces if we're not at the top level of XML building */
	if (declare_namespaces == TRUE) {
		g_string_append (xml_string, " xmlns='http://www.w3.org/2005/Atom'");
		if (namespaces != NULL) {
			g_hash_table_foreach (namespaces, (GHFunc) build_namespaces_cb, xml_string);
			g_hash_table_destroy (namespaces);
		}
	}

	g_hash_table_foreach (self->priv->extra_namespaces, (GHFunc) build_namespaces_cb, xml_string);

	/* Add anything the class thinks is suitable */
	if (klass->pre_get_xml != NULL)
		klass->pre_get_xml (self, xml_string);
	g_string_append_c (xml_string, '>');

	/* Store the length before we close the opening tag, so we can determine whether to self-close later on */
	length = xml_string->len;

	/* Add the rest of the XML */
	if (klass->get_xml != NULL)
		klass->get_xml (self, xml_string);

	/* Any extra XML? */
	if (self->priv->extra_xml != NULL && self->priv->extra_xml->str != NULL)
		g_string_append (xml_string, self->priv->extra_xml->str);

	/* Close the element; either by self-closing the opening tag, or by writing out a closing tag */
	if (xml_string->len == length)
		g_string_overwrite (xml_string, length - 1, "/>");
	else if (klass->element_namespace != NULL)
		g_string_append_printf (xml_string, "</%s:%s>", klass->element_namespace, klass->element_name);
	else
		g_string_append_printf (xml_string, "</%s>", klass->element_name);

	return g_string_free (xml_string, FALSE);
}
