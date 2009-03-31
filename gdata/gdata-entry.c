/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Philip Withnall 2008-2009 <philip@tecnocode.co.uk>
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
 * SECTION:gdata-entry
 * @short_description: GData entry object
 * @stability: Unstable
 * @include: gdata/gdata-entry.h
 *
 * #GDataEntry represents a single object on the online service, such as a playlist, video or calendar event. It is a snapshot of the
 * state of that object at the time of querying the service, so modifications made to a #GDataEntry will not be automatically or
 * magically propagated to the server.
 **/

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <libxml/parser.h>
#include <string.h>

#include "gdata-entry.h"
#include "gdata-types.h"
#include "gdata-service.h"
#include "gdata-private.h"
#include "gdata-atom.h"
#include "gdata-parser.h"

static void gdata_entry_finalize (GObject *object);
static void gdata_entry_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_entry_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void real_get_xml (GDataEntry *self, GString *xml_string);

struct _GDataEntryPrivate {
	gchar *title;
	gchar *id;
	GTimeVal updated;
	GTimeVal published;
	GList *categories;
	gchar *content;
	GList *links;
	GList *authors;
};

enum {
	PROP_TITLE = 1,
	PROP_ID,
	PROP_UPDATED,
	PROP_PUBLISHED,
	PROP_CONTENT,
	PROP_IS_INSERTED
};

G_DEFINE_TYPE (GDataEntry, gdata_entry, G_TYPE_OBJECT)
#define GDATA_ENTRY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_ENTRY, GDataEntryPrivate))

static void
gdata_entry_class_init (GDataEntryClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataEntryPrivate));

	gobject_class->set_property = gdata_entry_set_property;
	gobject_class->get_property = gdata_entry_get_property;
	gobject_class->finalize = gdata_entry_finalize;

	klass->get_xml = real_get_xml;

	g_object_class_install_property (gobject_class, PROP_TITLE,
				g_param_spec_string ("title",
					"Title", "The title for this entry.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_ID,
				g_param_spec_string ("id",
					"ID", "The ID for this entry.",
					NULL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_UPDATED,
				g_param_spec_boxed ("updated",
					"Updated", "The last update time for this entry.",
					G_TYPE_TIME_VAL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_PUBLISHED,
				g_param_spec_boxed ("published",
					"Published", "The time this entry was published.",
					G_TYPE_TIME_VAL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_CONTENT,
				g_param_spec_string ("content",
					"Content", "The textual content of this entry.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_IS_INSERTED,
				g_param_spec_boolean ("is-inserted",
					"Inserted?", "Whether the entry has been inserted on the server.",
					FALSE,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_entry_init (GDataEntry *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_ENTRY, GDataEntryPrivate);
}

static void
gdata_entry_finalize (GObject *object)
{
	GDataEntryPrivate *priv = GDATA_ENTRY_GET_PRIVATE (object);

	g_free (priv->title);
	xmlFree (priv->id);
	g_list_foreach (priv->categories, (GFunc) gdata_category_free, NULL);
	g_list_free (priv->categories);
	g_free (priv->content);
	g_list_foreach (priv->links, (GFunc) gdata_link_free, NULL);
	g_list_free (priv->links);
	g_list_foreach (priv->authors, (GFunc) gdata_author_free, NULL);
	g_list_free (priv->authors);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_entry_parent_class)->finalize (object);
}

static void
gdata_entry_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataEntryPrivate *priv = GDATA_ENTRY_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_TITLE:
			g_value_set_string (value, priv->title);
			break;
		case PROP_ID:
			g_value_set_string (value, priv->id);
			break;
		case PROP_UPDATED:
			g_value_set_boxed (value, &(priv->updated));
			break;
		case PROP_PUBLISHED:
			g_value_set_boxed (value, &(priv->published));
			break;
		case PROP_CONTENT:
			g_value_set_string (value, priv->content);
			break;
		case PROP_IS_INSERTED:
			g_value_set_boolean (value, gdata_entry_is_inserted (GDATA_ENTRY (object)));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gdata_entry_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataEntry *self = GDATA_ENTRY (object);

	switch (property_id) {
		case PROP_TITLE:
			gdata_entry_set_title (self, g_value_get_string (value));
			break;
		case PROP_CONTENT:
			gdata_entry_set_content (self, g_value_get_string (value));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
real_get_xml (GDataEntry *self, GString *xml_string)
{
	GDataEntryPrivate *priv = self->priv;
	gchar *title;
	GList *categories, *links, *authors;

	title = g_markup_escape_text (priv->title, -1);
	g_string_append_printf (xml_string, "<title type='text'>%s</title>", title);
	g_free (title);

	if (priv->id != NULL)
		g_string_append_printf (xml_string, "<id>%s</id>", priv->id);

	if (priv->updated.tv_sec != 0 || priv->updated.tv_usec != 0) {
		gchar *updated = g_time_val_to_iso8601 (&(priv->updated));
		g_string_append_printf (xml_string, "<updated>%s</updated>", updated);
		g_free (updated);
	}

	if (priv->published.tv_sec != 0 || priv->published.tv_usec != 0) {
		gchar *published = g_time_val_to_iso8601 (&(priv->published));
		g_string_append_printf (xml_string, "<published>%s</published>", published);
		g_free (published);
	}

	if (priv->content != NULL) {
		gchar *content = g_markup_escape_text (priv->content, -1);
		g_string_append_printf (xml_string, "<content type='text'>%s</content>", content);
		g_free (content);
	}

	for (categories = priv->categories; categories != NULL; categories = categories->next) {
		GDataCategory *category = (GDataCategory*) categories->data;

		g_string_append_printf (xml_string, "<category term='%s'", category->term);

		if (G_LIKELY (category->scheme != NULL))
			g_string_append_printf (xml_string, " scheme='%s'", category->scheme);

		if (G_UNLIKELY (category->label != NULL)) {
			gchar *label = g_markup_escape_text (category->label, -1);
			g_string_append_printf (xml_string, " label='%s'", label);
			g_free (label);
		}

		g_string_append (xml_string, "/>");
	}

	for (links = priv->links; links != NULL; links = links->next) {
		GDataLink *link = (GDataLink*) links->data;

		g_string_append_printf (xml_string, "<link href='%s'", link->href);

		if (G_UNLIKELY (link->title != NULL)) {
			gchar *title = g_markup_escape_text (link->title, -1);
			g_string_append_printf (xml_string, " title='%s'", title);
			g_free (title);
		}

		if (G_LIKELY (link->rel != NULL))
			g_string_append_printf (xml_string, " rel='%s'", link->rel);
		if (G_LIKELY (link->type != NULL))
			g_string_append_printf (xml_string, " type='%s'", link->type);
		if (G_UNLIKELY (link->hreflang != NULL))
			g_string_append_printf (xml_string, " hreflang='%s'", link->hreflang);
		if (G_UNLIKELY (link->length != -1))
			g_string_append_printf (xml_string, " length='%i'", link->length);
		g_string_append (xml_string, "/>");
	}

	for (authors = priv->authors; authors != NULL; authors = authors->next) {
		GDataAuthor *author = (GDataAuthor*) authors->data;

		gchar *name = g_markup_escape_text (author->name, -1);
		g_string_append_printf (xml_string, "<author><name>%s</name>", name);
		g_free (name);

		if (G_LIKELY (author->uri != NULL)) {
			gchar *uri = g_markup_escape_text (author->uri, -1);
			g_string_append_printf (xml_string, "<uri>%s</uri>", uri);
			g_free (uri);
		}

		if (G_UNLIKELY (author->email != NULL)) {
			gchar *email = g_markup_escape_text (author->email, -1);
			g_string_append_printf (xml_string, "<email>%s</email>", email);
			g_free (email);
		}

		g_string_append (xml_string, "</author>");
	}
}

/**
 * gdata_entry_new:
 *
 * Creates a new #GDataEntry with default properties.
 *
 * Return value: a new #GDataEntry
 **/
GDataEntry *
gdata_entry_new (void)
{
	return g_object_new (GDATA_TYPE_ENTRY, NULL);
}

/**
 * gdata_entry_new_from_xml:
 * @xml: the XML for just the entry, with full namespace declarations
 * @length: the length of @xml, or -1
 * @error: a #GError, or %NULL
 *
 * Creates a new #GDataEntry from the provided @xml.
 *
 * If @length is -1, @xml will be assumed to be nul-terminated.
 *
 * If an error occurs during parsing, a suitable error from #GDataParserError will be returned.
 *
 * Return value: a new #GDataEntry, or %NULL
 **/
GDataEntry *
gdata_entry_new_from_xml (const gchar *xml, gint length, GError **error)
{
	/* TODO: Shouldn't this be private? */
	xmlDoc *doc;
	xmlNode *node;

	g_return_val_if_fail (xml != NULL, NULL);

	if (length == -1)
		length = strlen (xml);

	/* Parse the XML */
	doc = xmlReadMemory (xml, length, "entry.xml", NULL, 0);
	if (doc == NULL) {
		xmlError *xml_error = xmlGetLastError ();
		g_set_error (error, GDATA_PARSER_ERROR, GDATA_PARSER_ERROR_PARSING_STRING,
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
			     _("Empty document."));
		return NULL;
	}

	if (xmlStrcmp (node->name, (xmlChar*) "entry") != 0) {
		/* No <entry> element (required) */
		xmlFreeDoc (doc);
		gdata_parser_error_required_element_missing ("entry", "root", error);
		return NULL;
	}

	return _gdata_entry_new_from_xml_node (doc, node, error);
}

GDataEntry *
_gdata_entry_new_from_xml_node (xmlDoc *doc, xmlNode *node, GError **error)
{
	GDataEntry *entry;

	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);
	g_return_val_if_fail (xmlStrcmp (node->name, (xmlChar*) "entry") == 0, FALSE);

	entry = gdata_entry_new ();

	node = node->xmlChildrenNode;
	while (node != NULL) {
		if (_gdata_entry_parse_xml_node (entry, doc, node, error) == FALSE) {
			g_object_unref (entry);
			return NULL;
		}
		node = node->next;
	}

	return entry;
}

gboolean
_gdata_entry_parse_xml_node (GDataEntry *self, xmlDoc *doc, xmlNode *node, GError **error)
{
	g_return_val_if_fail (GDATA_IS_ENTRY (self), FALSE);
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);

	if (xmlStrcmp (node->name, (xmlChar*) "title") == 0) {
		/* atom:title */
		xmlChar *title = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		gdata_entry_set_title (self, (gchar*) title);
		xmlFree (title);
	} else if (xmlStrcmp (node->name, (xmlChar*) "id") == 0) {
		/* atom:id */
		xmlFree (self->priv->id);
		self->priv->id = (gchar*) xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
	} else if (xmlStrcmp (node->name, (xmlChar*) "updated") == 0) {
		/* atom:updated */
		xmlChar *updated;

		updated = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		if (g_time_val_from_iso8601 ((gchar*) updated, &(self->priv->updated)) == FALSE) {
			/* Error */
			gdata_parser_error_not_iso8601_format ("updated", "entry", (gchar*) updated, error);
			xmlFree (updated);
			return FALSE;
		}
		xmlFree (updated);
	} else if (xmlStrcmp (node->name, (xmlChar*) "published") == 0) {
		/* atom:published */
		xmlChar *published;

		published = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		if (g_time_val_from_iso8601 ((gchar*) published, &(self->priv->published)) == FALSE) {
			/* Error */
			gdata_parser_error_not_iso8601_format ("published", "entry", (gchar*) published, error);
			xmlFree (published);
			return FALSE;
		}
		xmlFree (published);
	} else if (xmlStrcmp (node->name, (xmlChar*) "category") == 0) {
		/* atom:category */
		xmlChar *scheme, *term, *label;
		GDataCategory *category;

		scheme = xmlGetProp (node, (xmlChar*) "scheme");
		term = xmlGetProp (node, (xmlChar*) "term");
		label = xmlGetProp (node, (xmlChar*) "label");

		category = gdata_category_new ((gchar*) term, (gchar*) scheme, (gchar*) label);
		gdata_entry_add_category (self, category);

		xmlFree (scheme);
		xmlFree (term);
		xmlFree (label);
	} else if (xmlStrcmp (node->name, (xmlChar*) "content") == 0) {
		/* atom:content */
		xmlChar *content = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		if (content == NULL)
			content = xmlGetProp (node, (xmlChar*) "src");
		gdata_entry_set_content (self, (gchar*) content);
		xmlFree (content);
	} else if (xmlStrcmp (node->name, (xmlChar*) "link") == 0) {
		/* atom:link */
		xmlChar *href, *rel, *type, *hreflang, *title, *length;
		gint length_int;
		GDataLink *link;

		href = xmlGetProp (node, (xmlChar*) "href");
		rel = xmlGetProp (node, (xmlChar*) "rel");
		type = xmlGetProp (node, (xmlChar*) "type");
		hreflang = xmlGetProp (node, (xmlChar*) "hreflang");
		title = xmlGetProp (node, (xmlChar*) "title");
		length = xmlGetProp (node, (xmlChar*) "length");

		if (length == NULL)
			length_int = -1;
		else
			length_int = strtoul ((gchar*) length, NULL, 10);

		link = gdata_link_new ((gchar*) href, (gchar*) rel, (gchar*) type, (gchar*) hreflang, (gchar*) title, length_int);
		gdata_entry_add_link (self, link);

		xmlFree (href);
		xmlFree (rel);
		xmlFree (type);
		xmlFree (hreflang);
		xmlFree (title);
		xmlFree (length);
	} else if (xmlStrcmp (node->name, (xmlChar*) "author") == 0) {
		/* atom:author */
		/* TODO: move this and similar functions (link, category) to gdata-atom.c */
		GDataAuthor *author;
		xmlNode *author_node;
		xmlChar *name = NULL, *uri = NULL, *email = NULL;

		author_node = node->xmlChildrenNode;
		while (author_node != NULL) {
			if (xmlStrcmp (author_node->name, (xmlChar*) "name") == 0) {
				name = xmlNodeListGetString (doc, author_node->xmlChildrenNode, TRUE);
			} else if (xmlStrcmp (author_node->name, (xmlChar*) "uri") == 0) {
				uri = xmlNodeListGetString (doc, author_node->xmlChildrenNode, TRUE);
			} else if (xmlStrcmp (author_node->name, (xmlChar*) "email") == 0) {
				email = xmlNodeListGetString (doc, author_node->xmlChildrenNode, TRUE);
			} else {
				gdata_parser_error_unhandled_element ((gchar*) author_node->ns->prefix, (gchar*) author_node->name, "author", error);
				xmlFree (name);
				xmlFree (uri);
				xmlFree (email);
				return FALSE;
			}

			author_node = author_node->next;
		}

		author = gdata_author_new ((gchar*) name, (gchar*) uri, (gchar*) email);
		gdata_entry_add_author (self, author);

		xmlFree (name);
		xmlFree (uri);
		xmlFree (email);
	} else {
		return gdata_parser_error_unhandled_element ((gchar*) node->ns->prefix, (gchar*) node->name, "entry", error);
	}

	return TRUE;
}

/**
 * gdata_entry_get_title:
 * @self: a #GDataEntry
 *
 * Returns the title of the entry.
 *
 * Return value: the entry's title
 **/
const gchar *
gdata_entry_get_title (GDataEntry *self)
{
	g_return_val_if_fail (GDATA_IS_ENTRY (self), NULL);
	return self->priv->title;
}

/**
 * gdata_entry_set_title:
 * @self: a #GDataEntry
 * @title: the new entry title
 *
 * Sets the title of the entry.
 **/
void
gdata_entry_set_title (GDataEntry *self, const gchar *title)
{
	g_return_if_fail (GDATA_IS_ENTRY (self));
	g_return_if_fail (title != NULL);

	g_free (self->priv->title);
	self->priv->title = g_strdup (title);
	g_object_notify (G_OBJECT (self), "title");
}

/**
 * gdata_entry_get_id:
 * @self: a #GDataEntry
 *
 * Returns the URN ID of the entry; a unique and permanent identifier for the object the entry represents.
 *
 * Return value: the entry's ID
 **/
const gchar *
gdata_entry_get_id (GDataEntry *self)
{
	g_return_val_if_fail (GDATA_IS_ENTRY (self), NULL);
	return self->priv->id;
}

/**
 * gdata_entry_get_updated:
 * @self: a #GDataEntry
 * @updated: a #GTimeVal
 *
 * Puts the time the entry was last updated into @updated.
 **/
void
gdata_entry_get_updated (GDataEntry *self, GTimeVal *updated)
{
	g_return_if_fail (GDATA_IS_ENTRY (self));
	g_return_if_fail (updated != NULL);
	*updated = self->priv->updated;
}

/**
 * gdata_entry_get_published:
 * @self: a #GDataEntry
 * @published: a #GTimeVal
 *
 * Puts the time the entry was originally published into @published.
 **/
void
gdata_entry_get_published (GDataEntry *self, GTimeVal *published)
{
	g_return_if_fail (GDATA_IS_ENTRY (self));
	g_return_if_fail (published != NULL);
	*published = self->priv->published;
}

/**
 * gdata_entry_add_category:
 * @self: a #GDataEntry
 * @category: a #GDataCategory to add
 *
 * Adds @category to the list of categories in the given #GDataEntry.
 **/
void
gdata_entry_add_category (GDataEntry *self, GDataCategory *category)
{
	/* TODO: More category API */
	g_return_if_fail (GDATA_IS_ENTRY (self));
	g_return_if_fail (category != NULL);

	self->priv->categories = g_list_prepend (self->priv->categories, category);
}

/**
 * gdata_entry_get_content:
 * @self: a #GDataEntry
 *
 * Returns the textual content in this entry.
 *
 * Return value: the entry's content, or %NULL
 **/
const gchar *
gdata_entry_get_content (GDataEntry *self)
{
	g_return_val_if_fail (GDATA_IS_ENTRY (self), NULL);
	return self->priv->content;
}

/**
 * gdata_entry_set_content:
 * @self: a #GDataEntry
 * @content: the new content for the entry
 *
 * Sets the entry's content to @content.
 **/
void
gdata_entry_set_content (GDataEntry *self, const gchar *content)
{
	g_return_if_fail (GDATA_IS_ENTRY (self));

	g_free (self->priv->content);
	self->priv->content = g_strdup (content);
	g_object_notify (G_OBJECT (self), "content");
}

/**
 * gdata_entry_add_link:
 * @self: a #GDataEntry
 * @link: a #GDataLink to add
 *
 * Adds @link to the list of links in the given #GDataEntry.
 **/
void
gdata_entry_add_link (GDataEntry *self, GDataLink *link)
{
	/* TODO: More link API */
	g_return_if_fail (GDATA_IS_ENTRY (self));
	g_return_if_fail (link != NULL);

	self->priv->links = g_list_prepend (self->priv->links, link);
}

static gint
link_compare_cb (const GDataLink *link, const gchar *rel)
{
	return strcmp (link->rel, rel);
}

/**
 * gdata_entry_lookup_link:
 * @self: a #GDataEntry
 * @rel: the value of the <structfield>rel</structfield> attribute of the desired link
 *
 * Looks up a link by <structfield>rel</structfield> value from the list of links in the entry.
 *
 * Return value: a #GDataLink, or %NULL if one was not found
 **/
GDataLink *
gdata_entry_lookup_link (GDataEntry *self, const gchar *rel)
{
	GList *element;

	g_return_val_if_fail (GDATA_IS_ENTRY (self), NULL);
	g_return_val_if_fail (rel != NULL, NULL);

	element = g_list_find_custom (self->priv->links, rel, (GCompareFunc) link_compare_cb);
	if (element == NULL)
		return NULL;
	return (GDataLink*) (element->data);
}

/**
 * gdata_entry_add_author:
 * @self: a #GDataEntry
 * @author: a #GDataAuthor to add
 *
 * Adds @author to the list of authors in the given #GDataEntry.
 **/
void
gdata_entry_add_author (GDataEntry *self, GDataAuthor *author)
{
	/* TODO: More author API */
	g_return_if_fail (GDATA_IS_ENTRY (self));
	g_return_if_fail (author != NULL);

	self->priv->authors = g_list_prepend (self->priv->authors, author);
}

/**
 * gdata_entry_is_inserted:
 * @self: a #GDataEntry
 *
 * Returns whether the entry is marked as having been inserted on (uploaded to) the server already.
 *
 * Return value: %TRUE if the entry has been inserted already, %FALSE otherwise
 **/
gboolean
gdata_entry_is_inserted (GDataEntry *self)
{
	g_return_val_if_fail (GDATA_IS_ENTRY (self), FALSE);

	if (self->priv->id != NULL &&
	    self->priv->links != NULL &&
	    (self->priv->updated.tv_sec != 0 || self->priv->updated.tv_usec != 0))
		return TRUE;
	return FALSE;
}

/**
 * gdata_entry_get_xml:
 * @self: a #GDataEntry
 *
 * Builds an XML representation of the #GDataEntry in its current state, such that it could be inserted on the server.
 * The XML is guaranteed to have all its namespaces declared properly in a self-contained fashion. The root element is
 * guaranteed to be <literal>&lt;entry&gt;</literal>.
 *
 * Return value: the entry's XML; free with g_free()
 **/
gchar *
gdata_entry_get_xml (GDataEntry *self)
{
	GDataEntryClass *klass;
	GString *xml_string;
	const gchar *namespaces;

	klass = GDATA_ENTRY_GET_CLASS (self);
	if (klass->get_xml == NULL)
		return NULL;

	/* Get the namespaces the class uses */
	if (klass->get_namespaces == NULL)
		namespaces = "";
	else
		namespaces = klass->get_namespaces (self);

	/* Allocate enough space for 300 characters, which is a decent average entry length */
	xml_string = g_string_sized_new (300);
	g_string_append_printf (xml_string, "<entry xmlns='http://www.w3.org/2005/Atom' %s>", namespaces);
	klass->get_xml (self, xml_string);
	g_string_append (xml_string, "</entry>");

	return g_string_free (xml_string, FALSE);
}
