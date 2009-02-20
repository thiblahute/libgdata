/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Philip Withnall 2008 <philip@tecnocode.co.uk>
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
	PROP_CONTENT
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
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_UPDATED,
				g_param_spec_boxed ("updated",
					"Updated", "The last update time for this entry.",
					G_TYPE_TIME_VAL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_PUBLISHED,
				g_param_spec_boxed ("published",
					"Published", "The time this entry was published.",
					G_TYPE_TIME_VAL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_CONTENT,
				g_param_spec_string ("content",
					"Content", "The textual content of this entry.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
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
	g_free (priv->id);
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
		case PROP_ID:
			gdata_entry_set_id (self, g_value_get_string (value));
			break;
		case PROP_UPDATED:
			gdata_entry_set_updated (self, g_value_get_boxed (value));
			break;
		case PROP_PUBLISHED:
			gdata_entry_set_published (self, g_value_get_boxed (value));
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

GDataEntry *
gdata_entry_new (void)
{
	return g_object_new (GDATA_TYPE_ENTRY, NULL);
}

GDataEntry *
gdata_entry_new_from_xml (const gchar *xml, gint length, GError **error)
{
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
		xmlChar *id = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		gdata_entry_set_id (self, (gchar*) id);
		xmlFree (id);
	} else if (xmlStrcmp (node->name, (xmlChar*) "updated") == 0) {
		/* atom:updated */
		xmlChar *updated;
		GTimeVal updated_timeval;

		updated = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		if (g_time_val_from_iso8601 ((gchar*) updated, &updated_timeval) == FALSE) {
			/* Error */
			gdata_parser_error_not_iso8601_format ("entry", "updated", (gchar*) updated, error);
			xmlFree (updated);
			return FALSE;
		}

		gdata_entry_set_updated (self, &updated_timeval);
		xmlFree (updated);
	} else if (xmlStrcmp (node->name, (xmlChar*) "published") == 0) {
		/* atom:published */
		xmlChar *published;
		GTimeVal published_timeval;

		published = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		if (g_time_val_from_iso8601 ((gchar*) published, &published_timeval) == FALSE) {
			/* Error */
			gdata_parser_error_not_iso8601_format ("entry", "published", (gchar*) published, error);
			xmlFree (published);
			return FALSE;
		}

		gdata_entry_set_published (self, &published_timeval);
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

const gchar *
gdata_entry_get_title (GDataEntry *self)
{
	g_return_val_if_fail (GDATA_IS_ENTRY (self), NULL);
	return self->priv->title;
}

void
gdata_entry_set_title (GDataEntry *self, const gchar *title)
{
	g_return_if_fail (GDATA_IS_ENTRY (self));
	g_return_if_fail (title != NULL);

	g_free (self->priv->title);
	self->priv->title = g_strdup (title);
	g_object_notify (G_OBJECT (self), "title");
}

const gchar *
gdata_entry_get_id (GDataEntry *self)
{
	g_return_val_if_fail (GDATA_IS_ENTRY (self), NULL);
	return self->priv->id;
}

void
gdata_entry_set_id (GDataEntry *self, const gchar *id)
{
	g_return_if_fail (GDATA_IS_ENTRY (self));
	g_return_if_fail (id != NULL);

	g_free (self->priv->id);
	self->priv->id = g_strdup (id);
	g_object_notify (G_OBJECT (self), "id");
}

void
gdata_entry_get_updated (GDataEntry *self, GTimeVal *updated)
{
	g_return_if_fail (GDATA_IS_ENTRY (self));
	g_return_if_fail (updated != NULL);

	updated->tv_sec = self->priv->updated.tv_sec;
	updated->tv_usec = self->priv->updated.tv_usec;
}

void
gdata_entry_set_updated (GDataEntry *self, GTimeVal *updated)
{
	g_return_if_fail (GDATA_IS_ENTRY (self));
	g_return_if_fail (updated != NULL);

	self->priv->updated.tv_sec = updated->tv_sec;
	self->priv->updated.tv_usec = updated->tv_usec;
	g_object_notify (G_OBJECT (self), "updated");
}

void
gdata_entry_get_published (GDataEntry *self, GTimeVal *published)
{
	g_return_if_fail (GDATA_IS_ENTRY (self));
	g_return_if_fail (published != NULL);

	published->tv_sec = self->priv->published.tv_sec;
	published->tv_usec = self->priv->published.tv_usec;
}

void
gdata_entry_set_published (GDataEntry *self, GTimeVal *published)
{
	g_return_if_fail (GDATA_IS_ENTRY (self));
	g_return_if_fail (published != NULL);

	self->priv->published.tv_sec = published->tv_sec;
	self->priv->published.tv_usec = published->tv_usec;
	g_object_notify (G_OBJECT (self), "published");
}

/* TODO: More category API */
void
gdata_entry_add_category (GDataEntry *self, GDataCategory *category)
{
	g_return_if_fail (GDATA_IS_ENTRY (self));
	g_return_if_fail (category != NULL);

	self->priv->categories = g_list_prepend (self->priv->categories, category);
}

const gchar *
gdata_entry_get_content (GDataEntry *self)
{
	g_return_val_if_fail (GDATA_IS_ENTRY (self), NULL);
	return self->priv->content;
}

void
gdata_entry_set_content (GDataEntry *self, const gchar *content)
{
	g_return_if_fail (GDATA_IS_ENTRY (self));

	g_free (self->priv->content);
	self->priv->content = g_strdup (content);
	g_object_notify (G_OBJECT (self), "content");
}

/* TODO: More link API */
void
gdata_entry_add_link (GDataEntry *self, GDataLink *link)
{
	g_return_if_fail (GDATA_IS_ENTRY (self));
	g_return_if_fail (link != NULL);

	self->priv->links = g_list_prepend (self->priv->links, link);
}

/* TODO: More author API */
void
gdata_entry_add_author (GDataEntry *self, GDataAuthor *author)
{
	g_return_if_fail (GDATA_IS_ENTRY (self));
	g_return_if_fail (author != NULL);

	self->priv->authors = g_list_prepend (self->priv->authors, author);
}

gboolean
gdata_entry_inserted (GDataEntry *self)
{
	g_return_val_if_fail (GDATA_IS_ENTRY (self), FALSE);

	if (self->priv->id != NULL &&
	    self->priv->links != NULL &&
	    (self->priv->updated.tv_sec != 0 || self->priv->updated.tv_usec != 0))
		return TRUE;
	return FALSE;
}

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
	g_string_append_printf (xml_string, "<entry xmlns='http://www.w3.org/2005/Atom' %s>", klass->get_namespaces (self));
	klass->get_xml (self, xml_string);
	g_string_append (xml_string, "</entry>");

	return g_string_free (xml_string, FALSE);
}
