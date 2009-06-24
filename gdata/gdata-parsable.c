/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Philip Withnall 2009 <philip@tecnocode.co.uk>
 * 
 * GData Client is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GData Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GData Client.  If not, see <http://www.gnu.org/licenses/>.
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

GDataParsable *
_gdata_parsable_new_from_xml (GType parsable_type, const gchar *first_element, const gchar *xml, gint length, gpointer user_data, GError **error)
{
	xmlDoc *doc;
	xmlNode *node;

	g_return_val_if_fail (g_type_is_a (parsable_type, GDATA_TYPE_PARSABLE) == TRUE, FALSE);
	g_return_val_if_fail (first_element != NULL, NULL);
	g_return_val_if_fail (xml != NULL, NULL);

/*	g_print ("\nXML\n\n %s \n\n", xml);*/
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

	if (xmlStrcmp (node->name, (xmlChar*) first_element) != 0) {
		/* No <entry> element (required) */
		xmlFreeDoc (doc);
		gdata_parser_error_required_element_missing (first_element, "root", error);
		return NULL;
	}

	return _gdata_parsable_new_from_xml_node (parsable_type, first_element, doc, node, user_data, error);
}

GDataParsable *
_gdata_parsable_new_from_xml_node (GType parsable_type, const gchar *first_element, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error)
{
	GDataParsable *parsable;
	GDataParsableClass *klass;

	g_return_val_if_fail (g_type_is_a (parsable_type, GDATA_TYPE_PARSABLE) == TRUE, FALSE);
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);
	g_return_val_if_fail (xmlStrcmp (node->name, (xmlChar*) first_element) == 0, FALSE);

	parsable = g_object_new (parsable_type, NULL);

	klass = GDATA_PARSABLE_GET_CLASS (parsable);
	if (klass->parse_xml == NULL)
		return FALSE;

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

const gchar *
_gdata_parsable_get_extra_xml (GDataParsable *self)
{
	g_return_val_if_fail (GDATA_IS_PARSABLE (self), NULL);
	return self->priv->extra_xml->str;
}

GHashTable *
_gdata_parsable_get_extra_namespaces (GDataParsable *self)
{
	g_return_val_if_fail (GDATA_IS_PARSABLE (self), NULL);
	return self->priv->extra_namespaces;
}
