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

#include "gdata-youtube-media-group.h"
#include "gdata-private.h"
#include "gdata-service.h"
#include "gdata-types.h"
#include "gdata-parser.h"

static void gdata_youtube_media_group_finalize (GObject *object);
static void gdata_youtube_media_group_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_youtube_media_group_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

struct _GDataYouTubeMediaGroupPrivate {
	gint duration;
	gboolean private;
	GTimeVal uploaded;
	gchar *video_id;
	gboolean no_embed;
};

enum {
	PROP_DURATION = 1,
	PROP_PRIVATE,
	PROP_UPLOADED,
	PROP_VIDEO_ID,
	PROP_NO_EMBED
};

G_DEFINE_TYPE (GDataYouTubeMediaGroup, gdata_youtube_media_group, GDATA_TYPE_MEDIA_GROUP)
#define GDATA_YOUTUBE_MEDIA_GROUP_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_YOUTUBE_MEDIA_GROUP, GDataYouTubeMediaGroupPrivate))

static void
gdata_youtube_media_group_class_init (GDataYouTubeMediaGroupClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataYouTubeMediaGroupPrivate));

	gobject_class->set_property = gdata_youtube_media_group_set_property;
	gobject_class->get_property = gdata_youtube_media_group_get_property;
	gobject_class->finalize = gdata_youtube_media_group_finalize;

	g_object_class_install_property (gobject_class, PROP_DURATION,
				g_param_spec_int ("duration",
					"Duration", "The video duration, in seconds.",
					0, G_MAXINT, 0,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_PRIVATE,
				g_param_spec_boolean ("private",
					"Private", "Whether the video is private.",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_UPLOADED,
				g_param_spec_boxed ("uploaded",
					"Uploaded", "The time the video was uploaded.",
					G_TYPE_TIME_VAL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_VIDEO_ID,
				g_param_spec_string ("video-id",
					"Video ID", "The video's unique ID.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_NO_EMBED,
				g_param_spec_boolean ("no-embed",
					"No embed", "Whether embedding the video is allowed.",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_youtube_media_group_init (GDataYouTubeMediaGroup *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_YOUTUBE_MEDIA_GROUP, GDataYouTubeMediaGroupPrivate);
}

static void
gdata_youtube_media_group_finalize (GObject *object)
{
	GDataYouTubeMediaGroupPrivate *priv = GDATA_YOUTUBE_MEDIA_GROUP_GET_PRIVATE (object);

	g_free (priv->video_id);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_youtube_media_group_parent_class)->finalize (object);
}

static void
gdata_youtube_media_group_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataYouTubeMediaGroupPrivate *priv = GDATA_YOUTUBE_MEDIA_GROUP_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_DURATION:
			g_value_set_int (value, priv->duration);
			break;
		case PROP_PRIVATE:
			g_value_set_boolean (value, priv->private);
			break;
		case PROP_UPLOADED:
			g_value_set_boxed (value, &(priv->uploaded));
			break;
		case PROP_VIDEO_ID:
			g_value_set_string (value, priv->video_id);
			break;
		case PROP_NO_EMBED:
			g_value_set_boolean (value, priv->no_embed);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gdata_youtube_media_group_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataYouTubeMediaGroup *self = GDATA_YOUTUBE_MEDIA_GROUP (object);

	switch (property_id) {
		case PROP_DURATION:
			gdata_youtube_media_group_set_duration (self, g_value_get_int (value));
			break;
		case PROP_PRIVATE:
			gdata_youtube_media_group_set_private (self, g_value_get_boolean (value));
			break;
		case PROP_UPLOADED:
			gdata_youtube_media_group_set_uploaded (self, g_value_get_boxed (value));
			break;
		case PROP_VIDEO_ID:
			gdata_youtube_media_group_set_video_id (self, g_value_get_string (value));
			break;
		case PROP_NO_EMBED:
			gdata_youtube_media_group_set_no_embed (self, g_value_get_boolean (value));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

GDataYouTubeMediaGroup *
gdata_youtube_media_group_new (void)
{
	return g_object_new (GDATA_TYPE_YOUTUBE_MEDIA_GROUP, NULL);
}

GDataYouTubeMediaGroup *
_gdata_youtube_media_group_new_from_xml_node (xmlDoc *doc, xmlNode *node, GError **error)
{
	GDataYouTubeMediaGroup *group;

	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);
	g_return_val_if_fail (xmlStrcmp (node->name, (xmlChar*) "group") == 0, FALSE);

	group = gdata_youtube_media_group_new ();

	node = node->xmlChildrenNode;
	while (node != NULL) {
		if (_gdata_youtube_media_group_parse_xml_node (group, doc, node, error) == FALSE) {
			g_object_unref (group);
			return NULL;
		}
		node = node->next;
	}

	return group;
}

gboolean
_gdata_youtube_media_group_parse_xml_node (GDataYouTubeMediaGroup *self, xmlDoc *doc, xmlNode *node, GError **error)
{
	GError *child_error = NULL;

	g_return_val_if_fail (GDATA_IS_YOUTUBE_MEDIA_GROUP (self), FALSE);
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);

	if (xmlStrcmp (node->name, (xmlChar*) "duration") == 0) {
		/* yt:duration */
		xmlChar *duration;
		gint duration_int;

		duration = xmlGetProp (node, (xmlChar*) "seconds");
		if (duration == NULL)
			duration_int = -1;
		else
			duration_int = strtoul ((gchar*) duration, NULL, 10);
		xmlFree (duration);

		gdata_youtube_media_group_set_duration (self, duration_int);
	} else if (xmlStrcmp (node->name, (xmlChar*) "private") == 0) {
		/* yt:private */
		gdata_youtube_media_group_set_private (self, TRUE);
	} else if (xmlStrcmp (node->name, (xmlChar*) "uploaded") == 0) {
		/* yt:uploaded */
		xmlChar *uploaded;
		GTimeVal uploaded_timeval;

		uploaded = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		if (g_time_val_from_iso8601 ((gchar*) uploaded, &uploaded_timeval) == FALSE) {
			/* Error */
			gdata_parser_error_not_iso8601_format ("media:group", "uploaded", (gchar*) uploaded, error);
			xmlFree (uploaded);
			return FALSE;
		}

		gdata_youtube_media_group_set_uploaded (self, &uploaded_timeval);
		xmlFree (uploaded);
	} else if (xmlStrcmp (node->name, (xmlChar*) "videoid") == 0) {
		/* yt:videoid */
		xmlChar *video_id = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		gdata_youtube_media_group_set_video_id (self, (gchar*) video_id);
		xmlFree (video_id);
	} else if (xmlStrcmp (node->name, (xmlChar*) "noembed") == 0) {
		/* yt:noembed */
		gdata_youtube_media_group_set_no_embed (self, TRUE);
	} else if (_gdata_media_group_parse_xml_node (GDATA_MEDIA_GROUP (self), doc, node, &child_error) == FALSE) {
		if (g_error_matches (child_error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_UNHANDLED_XML_ELEMENT) == TRUE) {
			g_error_free (child_error);
			gdata_parser_error_unhandled_element ((gchar*) node->ns->prefix, (gchar*) node->name, "media:group", error);
		} else {
			g_propagate_error (error, child_error);
		}

		return FALSE;
	}

	return TRUE;
}

gint
gdata_youtube_media_group_get_duration (GDataYouTubeMediaGroup *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_MEDIA_GROUP (self), -1);
	return self->priv->duration;
}

void
gdata_youtube_media_group_set_duration (GDataYouTubeMediaGroup *self, gint duration)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_MEDIA_GROUP (self));
	self->priv->duration = duration;
	g_object_notify (G_OBJECT (self), "duration");
}

gboolean
gdata_youtube_media_group_get_private (GDataYouTubeMediaGroup *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_MEDIA_GROUP (self), FALSE);
	return self->priv->private;
}

void
gdata_youtube_media_group_set_private (GDataYouTubeMediaGroup *self, gboolean private)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_MEDIA_GROUP (self));
	self->priv->private = private;
	g_object_notify (G_OBJECT (self), "private");
}

void
gdata_youtube_media_group_get_uploaded (GDataYouTubeMediaGroup *self, GTimeVal *uploaded)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_MEDIA_GROUP (self));

	uploaded->tv_sec = self->priv->uploaded.tv_sec;
	uploaded->tv_usec = self->priv->uploaded.tv_usec;
}

void
gdata_youtube_media_group_set_uploaded (GDataYouTubeMediaGroup *self, GTimeVal *uploaded)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_MEDIA_GROUP (self));
	self->priv->uploaded.tv_sec = uploaded->tv_sec;
	self->priv->uploaded.tv_usec = uploaded->tv_usec;
	g_object_notify (G_OBJECT (self), "uploaded");
}

const gchar *
gdata_youtube_media_group_get_video_id (GDataYouTubeMediaGroup *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_MEDIA_GROUP (self), NULL);
	return self->priv->video_id;
}

void
gdata_youtube_media_group_set_video_id (GDataYouTubeMediaGroup *self, const gchar *video_id)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_MEDIA_GROUP (self));

	g_free (self->priv->video_id);
	self->priv->video_id = g_strdup (video_id);
	g_object_notify (G_OBJECT (self), "video-id");
}

gboolean
gdata_youtube_media_group_get_no_embed (GDataYouTubeMediaGroup *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_MEDIA_GROUP (self), FALSE);
	return self->priv->no_embed;
}

void
gdata_youtube_media_group_set_no_embed (GDataYouTubeMediaGroup *self, gboolean no_embed)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_MEDIA_GROUP (self));
	self->priv->no_embed = no_embed;
	g_object_notify (G_OBJECT (self), "no-embed");
}
