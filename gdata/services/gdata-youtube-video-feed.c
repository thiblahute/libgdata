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

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <libxml/parser.h>
#include <string.h>

#include "gdata-youtube-video-feed.h"
#include "gdata-feed.h"
#include "gdata-private.h"
#include "gdata-service.h"

static void gdata_youtube_video_feed_dispose (GObject *object);
static void gdata_youtube_video_feed_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_youtube_video_feed_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

struct _GDataYouTubeVideoFeedPrivate {
	gboolean dispose_has_run;
	/* TODO */
};

/*
TODO
enum {
	PROP_DEV_PATH = 1
};
*/

G_DEFINE_TYPE (GDataYouTubeVideoFeed, gdata_youtube_video_feed, GDATA_TYPE_FEED)
#define GDATA_YOUTUBE_VIDEO_FEED_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_YOUTUBE_VIDEO_FEED, GDataYouTubeVideoFeedPrivate))

static void
gdata_youtube_video_feed_class_init (GDataYouTubeVideoFeedClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataYouTubeVideoFeedPrivate));

	gobject_class->set_property = gdata_youtube_video_feed_set_property;
	gobject_class->get_property = gdata_youtube_video_feed_get_property;
	gobject_class->dispose = gdata_youtube_video_feed_dispose;

	/*
	TODO
	g_object_class_install_property (gobject_class, PROP_DEV_PATH,
				g_param_spec_string ("dev-path",
					"Device path", "The path to this connection's device node.",
					NULL,
					G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	*/
}

static void
gdata_youtube_video_feed_init (GDataYouTubeVideoFeed *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_YOUTUBE_VIDEO_FEED, GDataYouTubeVideoFeedPrivate);
	self->priv->dispose_has_run = FALSE;
	/*
	TODO
	self->priv->dev_path = NULL;
	*/
}

static void
gdata_youtube_video_feed_dispose (GObject *object)
{
	GDataYouTubeVideoFeedPrivate *priv = GDATA_YOUTUBE_VIDEO_FEED_GET_PRIVATE (object);

	/* Make sure we only run once */
	if (priv->dispose_has_run)
		return;
	priv->dispose_has_run = TRUE;

	/*
	TODO
	g_free (priv->dev_path);
	*/

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_youtube_video_feed_parent_class)->dispose (object);
}

static void
gdata_youtube_video_feed_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataYouTubeVideoFeedPrivate *priv = GDATA_YOUTUBE_VIDEO_FEED_GET_PRIVATE (object);

	switch (property_id) {
		/*
		TODO
		case PROP_DEV_PATH:
			g_value_set_string (value, g_strdup (priv->dev_path));
			break;
		*/
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gdata_youtube_video_feed_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataYouTubeVideoFeedPrivate *priv = GDATA_YOUTUBE_VIDEO_FEED_GET_PRIVATE (object);

	switch (property_id) {
		/*
		TODO
		case PROP_DEV_PATH:
			g_free (priv->dev_path);
			priv->dev_path = g_strdup (g_value_get_string (value));
			break;
		*/
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

/* TODO: see if it's possible to merge this with _gdata_feed_new_from_xml */
GDataFeed *
_gdata_youtube_video_feed_new_from_xml (const gchar *xml, gint length, GError **error)
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

	feed = g_object_new (GDATA_TYPE_YOUTUBE_VIDEO_FEED, NULL);
	node = node->xmlChildrenNode;
	while (node != NULL) {
		if (_gdata_youtube_video_feed_parse_xml_node (GDATA_YOUTUBE_VIDEO_FEED (feed), doc, node, error) == FALSE) {
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
_gdata_youtube_video_feed_parse_xml_node (GDataYouTubeVideoFeed *self, xmlDoc *doc, xmlNode *node, GError **error)
{
	GError *child_error = NULL;

	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO_FEED (self), FALSE);
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);

	if (xmlStrcmp (node->name, (xmlChar*) "entry") == 0) { /* atom:entry */
		GDataYouTubeVideo *video = _gdata_youtube_video_new_from_xml_node (doc, node, error);
		if (video == NULL)
			return FALSE;
		gdata_feed_append_entry (GDATA_FEED (self), GDATA_ENTRY (video)); /* TODO: any way we can cleanly _prepend_ entries, then reverse the list once we're done? */
		g_object_unref (video);
	} else if (_gdata_feed_parse_xml_node (GDATA_FEED (self), doc, node, &child_error) == FALSE) {
		if (g_error_matches (child_error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_UNHANDLED_XML_ELEMENT) == TRUE) {
			g_error_free (child_error);
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_UNHANDLED_XML_ELEMENT,
				     _("Unhandled <%s:%s> element as a child of a YouTube video <feed>."),
				     node->ns->prefix, node->name);
		} else {
			g_propagate_error (error, child_error);
		}

		return FALSE;
	}

	return TRUE;
}
