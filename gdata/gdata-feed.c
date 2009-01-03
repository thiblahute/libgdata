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

static void gdata_feed_dispose (GObject *object);
static void gdata_feed_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_feed_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

struct _GDataFeedPrivate {
	GList *entries;
	gchar *title;
	gchar *id;
	GTimeVal updated;
};

enum {
	PROP_ID = 1,
	PROP_UPDATED,
	PROP_TITLE
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
	gobject_class->dispose = gdata_feed_dispose;

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
}

static void
gdata_feed_init (GDataFeed *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_FEED, GDataFeedPrivate);
}

static void
gdata_feed_dispose (GObject *object)
{
	GDataFeedPrivate *priv = GDATA_FEED_GET_PRIVATE (object);

	g_list_foreach (priv->entries, (GFunc) g_object_unref, NULL);
	g_list_free (priv->entries);
	g_free (priv->title);
	g_free (priv->id);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_feed_parent_class)->dispose (object);
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

	if (xmlStrcmp (node->name, (xmlChar*) "entry") == 0) { /* atom:entry */
		GDataEntry *entry = _gdata_entry_new_from_xml_node (doc, node, error);
		if (entry == NULL)
			return FALSE;
		gdata_feed_append_entry (self, entry); /* TODO: any way we can cleanly _prepend_ entries, then reverse the list once we're done? */
	} else if (xmlStrcmp (node->name, (xmlChar*) "title") == 0) { /* atom:title */
		xmlChar *title = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		gdata_feed_set_title (self, (gchar*) title);
		xmlFree (title);
	} else if (xmlStrcmp (node->name, (xmlChar*) "id") == 0) { /* atom:id */
		xmlChar *id = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		gdata_feed_set_id (self, (gchar*) id);
		xmlFree (id);
	} else if (xmlStrcmp (node->name, (xmlChar*) "updated") == 0) { /* atom:updated */
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
	} else if (xmlStrcmp (node->name, (xmlChar*) "category") == 0) /* atom:category */
		/* TODO */
		doc = doc;
	else if (xmlStrcmp (node->name, (xmlChar*) "logo") == 0) /* atom:logo */
		/* TODO */
		doc = doc;
	else if (xmlStrcmp (node->name, (xmlChar*) "link") == 0) /* atom:link */
		/* TODO */
		doc = doc;
	else if (xmlStrcmp (node->name, (xmlChar*) "author") == 0) /* atom:author */
		/* TODO */
		doc = doc;
	else if (xmlStrcmp (node->name, (xmlChar*) "generator") == 0) /* atom:generator */
		/* TODO */
		doc = doc;
	else if (xmlStrcmp (node->name, (xmlChar*) "totalResults") == 0) /* openSearch:totalResults */
		/* TODO */
		doc = doc;
	else if (xmlStrcmp (node->name, (xmlChar*) "startIndex") == 0) /* openSearch:startIndex */
		/* TODO */
		doc = doc;
	else if (xmlStrcmp (node->name, (xmlChar*) "itemsPerPage") == 0) /* openSearch:itemsPerPage */
		/* TODO */
		doc = doc;
	else {
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
