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

#include "gdata-feed.h"
#include "gdata-entry.h"
#include "gdata-types.h"
#include "gdata-private.h"
#include "gdata-service.h"

static void gdata_feed_finalize (GObject *object);
static void gdata_feed_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_feed_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

struct _GDataFeedPrivate {
	GList *entries;
	gchar *title;
	gchar *id;
	GTimeVal updated;
	GList *categories;
	gchar *logo;
	GList *links;
	GList *authors;
	GDataGenerator *generator;
	guint items_per_page;
	guint start_index;
	guint total_results;
};

enum {
	PROP_ID = 1,
	PROP_UPDATED,
	PROP_TITLE,
	PROP_LOGO,
	PROP_GENERATOR,
	PROP_ITEMS_PER_PAGE,
	PROP_START_INDEX,
	PROP_TOTAL_RESULTS
};

G_DEFINE_TYPE (GDataFeed, gdata_feed, G_TYPE_OBJECT)
#define GDATA_FEED_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_FEED, GDataFeedPrivate))

static void
gdata_feed_class_init (GDataFeedClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataFeedPrivate));

	gobject_class->set_property = gdata_feed_set_property;
	gobject_class->get_property = gdata_feed_get_property;
	gobject_class->finalize = gdata_feed_finalize;

	g_object_class_install_property (gobject_class, PROP_TITLE,
				g_param_spec_string ("title",
					"Title", "The title for this feed.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_ID,
				g_param_spec_string ("id",
					"ID", "The ID for this feed.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_UPDATED,
				g_param_spec_boxed ("updated",
					"Updated", "The last update time for this feed.",
					G_TYPE_TIME_VAL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_LOGO,
				g_param_spec_string ("logo",
					"Logo", "The logo image URI for this feed.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_GENERATOR,
				g_param_spec_pointer ("generator",
					"Generator", "Details of the application to generate the feed.",
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_ITEMS_PER_PAGE,
				g_param_spec_uint ("items-per-page",
					"Items per page", "The number of items per results page.",
					0, G_MAXINT, 0,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_START_INDEX,
				g_param_spec_uint ("start-index",
					"Start index", "The one-based start index.",
					1, G_MAXINT, 1,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_TOTAL_RESULTS,
				g_param_spec_uint ("total-results",
					"Total results", "The total number of results in the feed.",
					0, 1000000, 0,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_feed_init (GDataFeed *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_FEED, GDataFeedPrivate);
}

static void
gdata_feed_finalize (GObject *object)
{
	GDataFeedPrivate *priv = GDATA_FEED_GET_PRIVATE (object);

	g_list_foreach (priv->entries, (GFunc) g_object_unref, NULL);
	g_list_free (priv->entries);
	g_free (priv->title);
	g_free (priv->id);
	g_list_foreach (priv->categories, (GFunc) gdata_category_free, NULL);
	g_list_free (priv->categories);
	g_free (priv->logo);
	g_list_foreach (priv->links, (GFunc) gdata_link_free, NULL);
	g_list_free (priv->links);
	g_list_foreach (priv->authors, (GFunc) gdata_author_free, NULL);
	g_list_free (priv->authors);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_feed_parent_class)->finalize (object);
}

static void
gdata_feed_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataFeedPrivate *priv = GDATA_FEED_GET_PRIVATE (object);

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
		case PROP_LOGO:
			g_value_set_string (value, priv->logo);
			break;
		case PROP_GENERATOR:
			g_value_set_pointer (value, priv->generator);
			break;
		case PROP_ITEMS_PER_PAGE:
			g_value_set_uint (value, priv->items_per_page);
			break;
		case PROP_START_INDEX:
			g_value_set_uint (value, priv->start_index);
			break;
		case PROP_TOTAL_RESULTS:
			g_value_set_uint (value, priv->total_results);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gdata_feed_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataFeedPrivate *priv = GDATA_FEED_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_TITLE:
			gdata_feed_set_title (GDATA_FEED (object), g_value_get_string (value));
			break;
		case PROP_ID:
			gdata_feed_set_id (GDATA_FEED (object), g_value_get_string (value));
			break;
		case PROP_UPDATED:
			gdata_feed_set_updated (GDATA_FEED (object), g_value_get_boxed (value));
			break;
		case PROP_LOGO:
			gdata_feed_set_logo (GDATA_FEED (object), g_value_get_string (value));
			break;
		case PROP_GENERATOR:
			gdata_feed_set_generator (GDATA_FEED (object), g_value_get_pointer (value));
			break;
		case PROP_ITEMS_PER_PAGE:
			gdata_feed_set_items_per_page (GDATA_FEED (object), g_value_get_uint (value));
			break;
		case PROP_START_INDEX:
			gdata_feed_set_start_index (GDATA_FEED (object), g_value_get_uint (value));
			break;
		case PROP_TOTAL_RESULTS:
			gdata_feed_set_total_results (GDATA_FEED (object), g_value_get_uint (value));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

GDataFeed *
_gdata_feed_new_from_xml (const gchar *xml, gint length, GError **error)
{
	GDataFeed *feed = NULL;
	xmlDoc *doc;
	xmlNode *node;

	g_return_val_if_fail (xml != NULL, NULL);

	if (length == -1)
		length = strlen (xml);

	/* Parse the XML */
	doc = xmlReadMemory (xml, length, "feed.xml", NULL, 0);
	if (doc == NULL) /* TODO: error */
		return NULL;

	/* Get the root element */
	node = xmlDocGetRootElement (doc);
	if (node == NULL) {
		/* XML document's empty */
		xmlFreeDoc (doc);
		/* TODO: error */
		return NULL;
	}

	if (xmlStrcmp (node->name, (xmlChar*) "feed") != 0) {
		/* No <feed> element (required) */
		xmlFreeDoc (doc);
		/* TODO: error */
		return NULL;
	}

	feed = g_object_new (GDATA_TYPE_FEED, NULL);
	node = node->xmlChildrenNode;
	while (node != NULL) {
		if (_gdata_feed_parse_xml_node (feed, doc, node, error) == FALSE) {
			g_object_unref (feed);
			feed = NULL;
			goto error;
		}
		node = node->next;
	}

error:
	xmlFreeDoc (doc);

	return feed;
}

gboolean
_gdata_feed_parse_xml_node (GDataFeed *self, xmlDoc *doc, xmlNode *node, GError **error)
{
	g_return_val_if_fail (GDATA_IS_FEED (self), FALSE);
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);

	if (xmlStrcmp (node->name, (xmlChar*) "entry") == 0) {
		/* atom:entry */
		GDataEntry *entry = _gdata_entry_new_from_xml_node (doc, node, error);
		if (entry == NULL)
			return FALSE;

		gdata_feed_append_entry (self, entry); /* TODO: any way we can cleanly _prepend_ entries, then reverse the list once we're done? */
		g_object_unref (entry);
	} else if (xmlStrcmp (node->name, (xmlChar*) "title") == 0) {
		/* atom:title */
		xmlChar *title = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		gdata_feed_set_title (self, (gchar*) title);
		xmlFree (title);
	} else if (xmlStrcmp (node->name, (xmlChar*) "id") == 0) {
		/* atom:id */
		xmlChar *id = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		gdata_feed_set_id (self, (gchar*) id);
		xmlFree (id);
	} else if (xmlStrcmp (node->name, (xmlChar*) "updated") == 0) {
		/* atom:updated */
		xmlChar *updated;
		GTimeVal updated_timeval;

		updated = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		if (g_time_val_from_iso8601 ((gchar*) updated, &updated_timeval) == FALSE) {
			/* Error */
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
				     _("A <feed>'s <updated> element (\"%s\") was not in ISO8601 format."), updated);
			xmlFree (updated);
			return FALSE;
		}

		gdata_feed_set_updated (self, &updated_timeval);
		xmlFree (updated);
	} else if (xmlStrcmp (node->name, (xmlChar*) "category") == 0) {
		/* atom:category */
		xmlChar *scheme, *term, *label;
		GDataCategory *category;

		scheme = xmlGetProp (node, (xmlChar*) "scheme");
		term = xmlGetProp (node, (xmlChar*) "term");
		label = xmlGetProp (node, (xmlChar*) "label");

		category = gdata_category_new ((gchar*) scheme, (gchar*) term, (gchar*) label);
		gdata_feed_add_category (self, category);

		xmlFree (scheme);
		xmlFree (term);
		xmlFree (label);
	} else if (xmlStrcmp (node->name, (xmlChar*) "logo") == 0) {
		/* atom:logo */
		xmlChar *logo = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		gdata_feed_set_logo (self, (gchar*) logo);
		xmlFree (logo);
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
			length_int = MAX (atoi ((gchar*) length), -1);

		link = gdata_link_new ((gchar*) href, (gchar*) rel, (gchar*) type, (gchar*) hreflang, (gchar*) title, length_int);
		gdata_feed_add_link (self, link);

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

		author_node = node->xmlChildrenNode;
		while (author_node != NULL) {
			if (xmlStrcmp (author_node->name, (xmlChar*) "name") == 0) {
				name = xmlNodeListGetString (doc, author_node->xmlChildrenNode, TRUE);
			} else if (xmlStrcmp (author_node->name, (xmlChar*) "uri") == 0) {
				uri = xmlNodeListGetString (doc, author_node->xmlChildrenNode, TRUE);
			} else if (xmlStrcmp (author_node->name, (xmlChar*) "email") == 0) {
				email = xmlNodeListGetString (doc, author_node->xmlChildrenNode, TRUE);
			} else {
				g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
					     _("Unexpected <%s:%s> element as a child of <author>."),
					     author_node->ns->prefix, author_node->name);
				xmlFree (name);
				xmlFree (uri);
				xmlFree (email);
				return FALSE;
			}

			author_node = author_node->next;
		}

		author = gdata_author_new ((gchar*) name, (gchar*) uri, (gchar*) email);
		gdata_feed_add_author (self, author);

		xmlFree (name);
		xmlFree (uri);
		xmlFree (email);
	} else if (xmlStrcmp (node->name, (xmlChar*) "generator") == 0) {
		/* atom:generator */
		xmlChar *name, *uri, *version;
		GDataGenerator *generator;

		name = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		uri = xmlGetProp (node, (xmlChar*) "uri");
		version = xmlGetProp (node, (xmlChar*) "version");

		generator = gdata_generator_new ((gchar*) name, (gchar*) uri, (gchar*) version);
		gdata_feed_set_generator (self, generator);

		xmlFree (name);
		xmlFree (uri);
		xmlFree (version);
	} else if (xmlStrcmp (node->name, (xmlChar*) "totalResults") == 0) {
		/* openSearch:totalResults */
		xmlChar *total_results;
		guint total_results_uint;

		total_results = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		if (total_results == NULL)
			total_results_uint = -1; /* TODO: error? */
		else
			total_results_uint = MAX (atoi ((gchar*) total_results), 0);
		xmlFree (total_results);

		gdata_feed_set_total_results (self, total_results_uint);
	} else if (xmlStrcmp (node->name, (xmlChar*) "startIndex") == 0) {
		/* openSearch:startIndex */
		xmlChar *start_index;
		guint start_index_uint;

		start_index = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		if (start_index == NULL)
			start_index_uint = -1; /* TODO: error? */
		else
			start_index_uint = MAX (atoi ((gchar*) start_index), 0);
		xmlFree (start_index);

		gdata_feed_set_start_index (self, start_index_uint);
	} else if (xmlStrcmp (node->name, (xmlChar*) "itemsPerPage") == 0) {
		/* openSearch:itemsPerPage */
		xmlChar *items_per_page;
		guint items_per_page_uint;

		items_per_page = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		if (items_per_page == NULL)
			items_per_page_uint = -1; /* TODO: error? */
		else
			items_per_page_uint = MAX (atoi ((gchar*) items_per_page), 0);
		xmlFree (items_per_page);

		gdata_feed_set_items_per_page (self, items_per_page_uint);
	} else {
		g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_UNHANDLED_XML_ELEMENT,
			     _("Unhandled <%s:%s> element as a child of <feed>."),
			     node->ns->prefix, node->name);
		return FALSE;
	}

	return TRUE;
}

const GList *
gdata_feed_get_entries (GDataFeed *self)
{
	g_return_val_if_fail (GDATA_IS_FEED (self), NULL);
	return self->priv->entries;
}

void
gdata_feed_append_entry (GDataFeed *self, GDataEntry *entry)
{
	self->priv->entries = g_list_append (self->priv->entries, g_object_ref (entry));
}

const gchar *
gdata_feed_get_title (GDataFeed *self)
{
	g_return_val_if_fail (GDATA_IS_FEED (self), NULL);
	return self->priv->title;
}

void
gdata_feed_set_title (GDataFeed *self, const gchar *title)
{
	g_return_if_fail (GDATA_IS_FEED (self));
	g_return_if_fail (title != NULL);

	g_free (self->priv->title);
	self->priv->title = g_strdup (title);
	g_object_notify (G_OBJECT (self), "title");
}

const gchar *
gdata_feed_get_id (GDataFeed *self)
{
	g_return_val_if_fail (GDATA_IS_FEED (self), NULL);
	return self->priv->id;
}

void
gdata_feed_set_id (GDataFeed *self, const gchar *id)
{
	g_return_if_fail (GDATA_IS_FEED (self));
	g_return_if_fail (id != NULL);

	g_free (self->priv->id);
	self->priv->id = g_strdup (id);
	g_object_notify (G_OBJECT (self), "id");
}

void
gdata_feed_get_updated (GDataFeed *self, GTimeVal *updated)
{
	g_return_if_fail (GDATA_IS_FEED (self));
	g_return_if_fail (updated != NULL);

	updated->tv_sec = self->priv->updated.tv_sec;
	updated->tv_usec = self->priv->updated.tv_usec;
}

void
gdata_feed_set_updated (GDataFeed *self, GTimeVal *updated)
{
	g_return_if_fail (GDATA_IS_FEED (self));
	g_return_if_fail (updated != NULL);

	self->priv->updated.tv_sec = updated->tv_sec;
	self->priv->updated.tv_usec = updated->tv_usec;
	g_object_notify (G_OBJECT (self), "updated");
}

/* TODO: More category API */
void
gdata_feed_add_category (GDataFeed *self, GDataCategory *category)
{
	g_return_if_fail (GDATA_IS_FEED (self));
	g_return_if_fail (category != NULL);

	self->priv->categories = g_list_prepend (self->priv->categories, category);
}

const gchar *
gdata_feed_get_logo (GDataFeed *self)
{
	g_return_val_if_fail (GDATA_IS_FEED (self), NULL);
	return self->priv->logo;
}

void
gdata_feed_set_logo (GDataFeed *self, const gchar *logo)
{
	g_return_if_fail (GDATA_IS_FEED (self));
	g_return_if_fail (logo != NULL);

	g_free (self->priv->logo);
	self->priv->logo = g_strdup (logo);
	g_object_notify (G_OBJECT (self), "logo");
}

/* TODO: More link API */
void
gdata_feed_add_link (GDataFeed *self, GDataLink *link)
{
	g_return_if_fail (GDATA_IS_FEED (self));
	g_return_if_fail (link != NULL);

	self->priv->links = g_list_prepend (self->priv->links, link);
}

/* TODO: More author API */
void
gdata_feed_add_author (GDataFeed *self, GDataAuthor *author)
{
	g_return_if_fail (GDATA_IS_FEED (self));
	g_return_if_fail (author != NULL);

	self->priv->authors = g_list_prepend (self->priv->authors, author);
}

GDataGenerator *
gdata_feed_get_generator (GDataFeed *self)
{
	g_return_val_if_fail (GDATA_IS_FEED (self), NULL);
	return self->priv->generator;
}

void
gdata_feed_set_generator (GDataFeed *self, GDataGenerator *generator)
{
	g_return_if_fail (GDATA_IS_FEED (self));
	g_return_if_fail (generator != NULL);

	if (self->priv->generator != NULL)
		gdata_generator_free (self->priv->generator); /* TODO: don't like this memory "management" */
	self->priv->generator = generator;
	g_object_notify (G_OBJECT (self), "generator");
}

guint
gdata_feed_get_items_per_page (GDataFeed *self)
{
	g_return_val_if_fail (GDATA_IS_FEED (self), 0);
	return self->priv->items_per_page;
}

void
gdata_feed_set_items_per_page (GDataFeed *self, guint items_per_page)
{
	g_return_if_fail (GDATA_IS_FEED (self));
	self->priv->items_per_page = items_per_page;
	g_object_notify (G_OBJECT (self), "items-per-page");
}

guint
gdata_feed_get_start_index (GDataFeed *self)
{
	g_return_val_if_fail (GDATA_IS_FEED (self), 0);
	return self->priv->start_index;
}

void
gdata_feed_set_start_index (GDataFeed *self, guint start_index)
{
	g_return_if_fail (GDATA_IS_FEED (self));
	g_return_if_fail (start_index > 0);

	self->priv->start_index = start_index;
	g_object_notify (G_OBJECT (self), "start-index");
}

guint
gdata_feed_get_total_results (GDataFeed *self)
{
	g_return_val_if_fail (GDATA_IS_FEED (self), 0);
	return self->priv->total_results;
}

void
gdata_feed_set_total_results (GDataFeed *self, guint total_results)
{
	g_return_if_fail (GDATA_IS_FEED (self));
	self->priv->total_results = total_results;
	g_object_notify (G_OBJECT (self), "total-results");
}
