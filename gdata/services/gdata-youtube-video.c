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

#include "gdata-youtube-video.h"
#include "gdata-private.h"
#include "gdata-service.h"

static void gdata_youtube_video_dispose (GObject *object);
static void gdata_youtube_video_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_youtube_video_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

struct _GDataYouTubeVideoPrivate {
	gboolean dispose_has_run;
	/* TODO */
};

/*
TODO
enum {
	PROP_DEV_PATH = 1
};
*/

G_DEFINE_TYPE (GDataYouTubeVideo, gdata_youtube_video, GDATA_TYPE_ENTRY)
#define GDATA_YOUTUBE_VIDEO_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_YOUTUBE_VIDEO, GDataYouTubeVideoPrivate))

static void
gdata_youtube_video_class_init (GDataYouTubeVideoClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataYouTubeVideoPrivate));

	gobject_class->set_property = gdata_youtube_video_set_property;
	gobject_class->get_property = gdata_youtube_video_get_property;
	gobject_class->dispose = gdata_youtube_video_dispose;

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
gdata_youtube_video_init (GDataYouTubeVideo *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_YOUTUBE_VIDEO, GDataYouTubeVideoPrivate);
	self->priv->dispose_has_run = FALSE;
	/*
	TODO
	self->priv->dev_path = NULL;
	*/
}

static void
gdata_youtube_video_dispose (GObject *object)
{
	GDataYouTubeVideoPrivate *priv = GDATA_YOUTUBE_VIDEO_GET_PRIVATE (object);

	/* Make sure we only run once */
	if (priv->dispose_has_run)
		return;
	priv->dispose_has_run = TRUE;

	/*
	TODO
	g_free (priv->dev_path);
	*/

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_youtube_video_parent_class)->dispose (object);
}

static void
gdata_youtube_video_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataYouTubeVideoPrivate *priv = GDATA_YOUTUBE_VIDEO_GET_PRIVATE (object);

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
gdata_youtube_video_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataYouTubeVideoPrivate *priv = GDATA_YOUTUBE_VIDEO_GET_PRIVATE (object);

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

GDataYouTubeVideo *
gdata_youtube_video_new (void)
{
	return g_object_new (GDATA_TYPE_YOUTUBE_VIDEO, NULL);
}

GDataYouTubeVideo *
_gdata_youtube_video_new_from_xml_node (xmlDoc *doc, xmlNode *node, GError **error)
{
	GDataYouTubeVideo *video;

	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);
	g_return_val_if_fail (xmlStrcmp (node->name, (xmlChar*) "entry") == 0, FALSE);

	video = gdata_youtube_video_new ();

	node = node->xmlChildrenNode;
	while (node != NULL) {
		if (_gdata_youtube_video_parse_xml_node (video, doc, node, error) == FALSE) {
			g_object_unref (video);
			return NULL;
		}
		node = node->next;
	}

	return video;
}

gboolean
_gdata_youtube_video_parse_xml_node (GDataYouTubeVideo *self, xmlDoc *doc, xmlNode *node, GError **error)
{
	GError *child_error = NULL;

	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), FALSE);
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);

	if (xmlStrcmp (node->name, (xmlChar*) "group") == 0) { /* media:group */
		/*GDataMediaGroup *media_group = gdata_media_group_new ();
		if (_gdata_media_group_parse_xml_node (media_group, doc, node, error) == FALSE) {
			/* Error *
			g_object_unref (media_group);
			return FALSE;
		}
		video->priv->media_group = media_group;*/
		g_warning ("TODO: unimplemented");
	} else if (xmlStrcmp (node->name, (xmlChar*) "statistics") == 0) /* yt:statistics */
		g_warning ("TODO: unimplemented");
	else if (xmlStrcmp (node->name, (xmlChar*) "location") == 0) /* yt:location */
		g_warning ("TODO: unimplemented");
	else if (xmlStrcmp (node->name, (xmlChar*) "where") == 0) /* georss:where */
		g_warning ("TODO: unimplemented");
	else if (xmlStrcmp (node->name, (xmlChar*) "noembed") == 0) /* yt:noembed */
		g_warning ("TODO: unimplemented");
	else if (xmlStrcmp (node->name, (xmlChar*) "recorded") == 0) /* yt:recorded */
		g_warning ("TODO: unimplemented");
	else if (_gdata_entry_parse_xml_node (GDATA_ENTRY (self), doc, node, &child_error) == FALSE) {
		if (g_error_matches (child_error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_UNHANDLED_XML_ELEMENT) == TRUE) {
			g_error_free (child_error);
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_UNHANDLED_XML_ELEMENT,
				     _("Unhandled <%s:%s> element as a child of a YouTube video <entry>."),
				     node->ns->prefix, node->name);
		} else {
			g_propagate_error (error, child_error);
		}

		return FALSE;
	}

	return TRUE;
}
