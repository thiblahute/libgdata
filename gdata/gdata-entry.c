/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Philip Withnall 2008-2009 <philip@tecnocode.co.uk>
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

static void gdata_entry_finalize (GObject *object);
static void gdata_entry_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_entry_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static gboolean pre_parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *root_node, gpointer user_data, GError **error);
static gboolean parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error);
static gboolean post_parse_xml (GDataParsable *parsable, gpointer user_data, GError **error);
static void real_get_xml (GDataEntry *self, GString *xml_string);
static void real_get_namespaces (GDataEntry *self, GHashTable *namespaces);

struct _GDataEntryPrivate {
	gchar *title;
	gchar *id;
	gchar *etag;
	GTimeVal updated;
	GTimeVal published;
	GList *categories;
	gchar *content;
	GList *links;
	GList *authors;
};

enum {
	PROP_TITLE = 1,
	PROP_ETAG,
	PROP_ID,
	PROP_UPDATED,
	PROP_PUBLISHED,
	PROP_CONTENT,
	PROP_IS_INSERTED
};

G_DEFINE_TYPE (GDataEntry, gdata_entry, GDATA_TYPE_PARSABLE)
#define GDATA_ENTRY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_ENTRY, GDataEntryPrivate))

static void
gdata_entry_class_init (GDataEntryClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataEntryPrivate));

	gobject_class->set_property = gdata_entry_set_property;
	gobject_class->get_property = gdata_entry_get_property;
	gobject_class->finalize = gdata_entry_finalize;

	parsable_class->pre_parse_xml = pre_parse_xml;
	parsable_class->parse_xml = parse_xml;
	parsable_class->post_parse_xml = post_parse_xml;

	klass->get_xml = real_get_xml;
	klass->get_namespaces = real_get_namespaces;

	g_object_class_install_property (gobject_class, PROP_TITLE,
				g_param_spec_string ("title",
					"Title", "The title for this entry.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_ID,
				g_param_spec_string ("id",
					"ID", "The ID for this entry.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));
	/* Since: 0.2.0 */
	g_object_class_install_property (gobject_class, PROP_ETAG,
				g_param_spec_string ("etag",
					"ETag", "The ETag for this entry.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_UPDATED,
				g_param_spec_boxed ("updated",
					"Updated", "The last update time for this entry.",
					GDATA_TYPE_G_TIME_VAL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_PUBLISHED,
				g_param_spec_boxed ("published",
					"Published", "The time this entry was published.",
					GDATA_TYPE_G_TIME_VAL,
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
	xmlFree (priv->etag);
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
		case PROP_ETAG:
			g_value_set_string (value, priv->etag);
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
		case PROP_ID:
			/* Construct only */
			self->priv->id = g_value_dup_string (value);
			break;
		case PROP_ETAG:
			/* Construct only */
			self->priv->etag = g_value_dup_string (value);
			break;
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

static gboolean
pre_parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *root_node, gpointer user_data, GError **error)
{
	g_return_val_if_fail (GDATA_IS_ENTRY (parsable), FALSE);
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (root_node != NULL, FALSE);

	/* Extract the ETag */
	GDATA_ENTRY (parsable)->priv->etag = (gchar*) xmlGetProp (root_node, (xmlChar*) "etag");

	return TRUE;
}

static gboolean
parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error)
{
	GDataEntry *self;

	g_return_val_if_fail (GDATA_IS_ENTRY (parsable), FALSE);
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);

	self = GDATA_ENTRY (parsable);

	if (xmlStrcmp (node->name, (xmlChar*) "title") == 0) {
		/* atom:title */
		xmlChar *title = xmlNodeListGetString (doc, node->children, TRUE);

		/* Title can be empty */
		if (title == NULL)
			gdata_entry_set_title (self, "");
		else
			gdata_entry_set_title (self, (gchar*) title);
		xmlFree (title);
	} else if (xmlStrcmp (node->name, (xmlChar*) "id") == 0) {
		/* atom:id */
		xmlFree (self->priv->id);
		self->priv->id = (gchar*) xmlNodeListGetString (doc, node->children, TRUE);
	} else if (xmlStrcmp (node->name, (xmlChar*) "updated") == 0) {
		/* atom:updated */
		xmlChar *updated;

		updated = xmlNodeListGetString (doc, node->children, TRUE);
		if (g_time_val_from_iso8601 ((gchar*) updated, &(self->priv->updated)) == FALSE) {
			/* Error */
			gdata_parser_error_not_iso8601_format (node, (gchar*) updated, error);
			xmlFree (updated);
			return FALSE;
		}
		xmlFree (updated);
	} else if (xmlStrcmp (node->name, (xmlChar*) "published") == 0) {
		/* atom:published */
		xmlChar *published;

		published = xmlNodeListGetString (doc, node->children, TRUE);
		if (g_time_val_from_iso8601 ((gchar*) published, &(self->priv->published)) == FALSE) {
			/* Error */
			gdata_parser_error_not_iso8601_format (node, (gchar*) published, error);
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
		self->priv->categories = g_list_prepend (self->priv->categories, category);

		xmlFree (scheme);
		xmlFree (term);
		xmlFree (label);
	} else if (xmlStrcmp (node->name, (xmlChar*) "content") == 0) {
		/* atom:content */
		xmlChar *content = xmlNodeListGetString (doc, node->children, TRUE);
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
		self->priv->links = g_list_prepend (self->priv->links, link);

		xmlFree (href);
		xmlFree (rel);
		xmlFree (type);
		xmlFree (hreflang);
		xmlFree (title);
		xmlFree (length);
	} else if (xmlStrcmp (node->name, (xmlChar*) "author") == 0) {
		/* atom:author */
		GDataAuthor *author;
		xmlNode *author_node;
		xmlChar *name = NULL, *uri = NULL, *email = NULL;

		author_node = node->children;
		while (author_node != NULL) {
			if (xmlStrcmp (author_node->name, (xmlChar*) "name") == 0) {
				name = xmlNodeListGetString (doc, author_node->children, TRUE);
			} else if (xmlStrcmp (author_node->name, (xmlChar*) "uri") == 0) {
				uri = xmlNodeListGetString (doc, author_node->children, TRUE);
			} else if (xmlStrcmp (author_node->name, (xmlChar*) "email") == 0) {
				email = xmlNodeListGetString (doc, author_node->children, TRUE);
			} else {
				gdata_parser_error_unhandled_element (author_node, error);
				xmlFree (name);
				xmlFree (uri);
				xmlFree (email);
				return FALSE;
			}

			author_node = author_node->next;
		}

		author = gdata_author_new ((gchar*) name, (gchar*) uri, (gchar*) email);
		self->priv->authors = g_list_prepend (self->priv->authors, author);

		xmlFree (name);
		xmlFree (uri);
		xmlFree (email);
	} else if (GDATA_PARSABLE_CLASS (gdata_entry_parent_class)->parse_xml (parsable, doc, node, user_data, error) == FALSE) {
		/* Error! */
		return FALSE;
	}

	return TRUE;
}

static gboolean
post_parse_xml (GDataParsable *parsable, gpointer user_data, GError **error)
{
	GDataEntryPrivate *priv = GDATA_ENTRY (parsable)->priv;

	/* Check for missing required elements */
	/* Can't uncomment it, as things like access rules break the Atom standard */
	/*if (priv->title == NULL)
		return gdata_parser_error_required_element_missing ("title", "entry", error);
	if (priv->id == NULL)
		return gdata_parser_error_required_element_missing ("id", "entry", error);
	if (priv->updated.tv_sec == 0 && priv->updated.tv_usec == 0)
		return gdata_parser_error_required_element_missing ("updated", "entry", error);*/

	/* Reverse our lists of stuff */
	priv->categories = g_list_reverse (priv->categories);
	priv->links = g_list_reverse (priv->links);
	priv->authors = g_list_reverse (priv->authors);

	return TRUE;
}

GDataEntry *
_gdata_entry_new_from_xml (GType entry_type, const gchar *xml, gint length, GError **error)
{
	g_return_val_if_fail (xml != NULL, NULL);
	g_return_val_if_fail (g_type_is_a (entry_type, GDATA_TYPE_ENTRY) == TRUE, FALSE);

	return GDATA_ENTRY (_gdata_parsable_new_from_xml (entry_type, "entry", xml, length, NULL, error));
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
			gchar *link_title = g_markup_escape_text (link->title, -1);
			g_string_append_printf (xml_string, " title='%s'", link_title);
			g_free (link_title);
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

	if (_gdata_parsable_get_extra_xml (GDATA_PARSABLE (self)) != NULL)
		g_string_append (xml_string, _gdata_parsable_get_extra_xml (GDATA_PARSABLE (self)));
}

static void
real_get_namespaces (GDataEntry *self, GHashTable *namespaces)
{
	g_hash_table_insert (namespaces, (gchar*) "gd", (gchar*) "http://schemas.google.com/g/2005");
}

/**
 * gdata_entry_new:
 * @id: the entry's ID, or %NULL
 *
 * Creates a new #GDataEntry with the given ID and default properties.
 *
 * Return value: a new #GDataEntry; unref with g_object_unref()
 **/
GDataEntry *
gdata_entry_new (const gchar *id)
{
	return g_object_new (GDATA_TYPE_ENTRY, "id", id, NULL);
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
	return GDATA_ENTRY (_gdata_parsable_new_from_xml (GDATA_TYPE_ENTRY, "entry", xml, length, NULL, error));
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
 * @title: the new entry title, or %NULL
 *
 * Sets the title of the entry.
 **/
void
gdata_entry_set_title (GDataEntry *self, const gchar *title)
{
	g_return_if_fail (GDATA_IS_ENTRY (self));
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
 * gdata_entry_get_etag:
 * @self: a #GDataEntry
 *
 * Returns the ETag of the entry; a unique identifier for each version of the entry. For more information, see the
 * <ulink type="http" url="http://code.google.com/apis/gdata/docs/2.0/reference.html#ResourceVersioning">online documentation</ulink>.
 *
 * Return value: the entry's ETag
 *
 * Since: 0.2.0
 **/
const gchar *
gdata_entry_get_etag (GDataEntry *self)
{
	g_return_val_if_fail (GDATA_IS_ENTRY (self), NULL);
	return self->priv->etag;
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
 * Adds @category to the list of categories in the given #GDataEntry. The #GDataEntry takes
 * ownership of @category, so it must not be freed after being added.
 *
 * Duplicate categories will not be added to the list.
 **/
void
gdata_entry_add_category (GDataEntry *self, GDataCategory *category)
{
	g_return_if_fail (GDATA_IS_ENTRY (self));
	g_return_if_fail (category != NULL);

	if (g_list_find_custom (self->priv->categories, category, (GCompareFunc) gdata_category_compare) == NULL)
		self->priv->categories = g_list_prepend (self->priv->categories, category);
	else
		gdata_category_free (category);
}

/**
 * gdata_entry_get_categories:
 * @self: a #GDataEntry
 *
 * Gets a list of the #GDataCategory<!-- -->s containing this entry.
 *
 * Return value: a #GList of #GDataCategory<!-- -->s
 *
 * Since: 0.2.0
 **/
GList *
gdata_entry_get_categories (GDataEntry *self)
{
	g_return_val_if_fail (GDATA_IS_ENTRY (self), NULL);
	return self->priv->categories;
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
 * Adds @link to the list of links in the given #GDataEntry. The #GDataEntry takes
 * ownership of @link, so it must not be freed after being added.
 *
 * Duplicate links will not be added to the list.
 **/
void
gdata_entry_add_link (GDataEntry *self, GDataLink *link)
{
	/* TODO: More link API */
	g_return_if_fail (GDATA_IS_ENTRY (self));
	g_return_if_fail (link != NULL);

	if (g_list_find_custom (self->priv->links, link, (GCompareFunc) gdata_link_compare) == NULL)
		self->priv->links = g_list_prepend (self->priv->links, link);
	else
		gdata_link_free (link);
}

static gint
link_compare_cb (const GDataLink *link, const gchar *rel)
{
	return strcmp (link->rel, rel);
}

/**
 * gdata_entry_look_up_link:
 * @self: a #GDataEntry
 * @rel: the value of the <structfield>rel</structfield> attribute of the desired link
 *
 * Looks up a link by <structfield>rel</structfield> value from the list of links in the entry.
 *
 * Return value: a #GDataLink, or %NULL if one was not found
 *
 * Since: 0.1.1
 **/
GDataLink *
gdata_entry_look_up_link (GDataEntry *self, const gchar *rel)
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
 * Adds @author to the list of authors in the given #GDataEntry. The #GDataEntry takes
 * ownership of @author, so it must not be freed after being added.
 *
 * Duplicate authors will not be added to the list.
 **/
void
gdata_entry_add_author (GDataEntry *self, GDataAuthor *author)
{
	/* TODO: More author API */
	g_return_if_fail (GDATA_IS_ENTRY (self));
	g_return_if_fail (author != NULL);

	if (g_list_find_custom (self->priv->authors, author, (GCompareFunc) gdata_author_compare) == NULL)
		self->priv->authors = g_list_prepend (self->priv->authors, author);
	else
		gdata_author_free (author);
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
	GHashTable *namespaces, *extra_namespaces;

	klass = GDATA_ENTRY_GET_CLASS (self);
	g_assert (klass->get_xml != NULL);
	g_assert (klass->get_namespaces != NULL);

	/* Get the namespaces the class uses */
	namespaces = g_hash_table_new (g_str_hash, g_str_equal);
	klass->get_namespaces (self, namespaces);
	extra_namespaces = _gdata_parsable_get_extra_namespaces (GDATA_PARSABLE (self));

	/* Remove any duplicate extra namespaces */
	g_hash_table_foreach_remove (extra_namespaces, (GHRFunc) filter_namespaces_cb, namespaces);

	/* Build up the namespace list */
	xml_string = g_string_new ("<entry xmlns='http://www.w3.org/2005/Atom'");
	g_hash_table_foreach (namespaces, (GHFunc) build_namespaces_cb, xml_string);
	g_hash_table_foreach (extra_namespaces, (GHFunc) build_namespaces_cb, xml_string);

	/* Add the entry's ETag, if available */
	if (self->priv->etag != NULL)
		g_string_append_printf (xml_string, " gd:etag='%s'>", self->priv->etag);
	else
		g_string_append_c (xml_string, '>');

	g_hash_table_destroy (namespaces);

	/* Add the rest of the XML */
	klass->get_xml (self, xml_string);
	g_string_append (xml_string, "</entry>");

	return g_string_free (xml_string, FALSE);
}
