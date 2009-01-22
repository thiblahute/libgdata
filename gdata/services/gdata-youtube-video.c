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
#include "gdata-youtube-media-group.h"
#include "gdata-gdata.h"
#include "gdata-parser.h"

static void gdata_youtube_video_dispose (GObject *object);
static void gdata_youtube_video_finalize (GObject *object);
static void gdata_youtube_video_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_youtube_video_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

struct _GDataYouTubeVideoPrivate {
	GDataMediaGroup *media_group;
	guint view_count;
	guint favorite_count;
	gchar *location;
	gboolean no_embed;
	GDataGDRating *rating;
};

enum {
	PROP_MEDIA_GROUP = 1,
	PROP_VIEW_COUNT,
	PROP_FAVORITE_COUNT,
	PROP_LOCATION,
	PROP_NO_EMBED,
	PROP_RATING
};

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
	gobject_class->finalize = gdata_youtube_video_finalize;

	g_object_class_install_property (gobject_class, PROP_MEDIA_GROUP,
				g_param_spec_object ("media-group",
					"Media group", "The media group describing this video.",
					GDATA_TYPE_MEDIA_GROUP,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_VIEW_COUNT,
				g_param_spec_uint ("view-count",
					"View count", "TODO",
					0, G_MAXUINT, 0,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_FAVORITE_COUNT,
				g_param_spec_uint ("favorite-count",
					"Favorite count", "TODO",
					0, G_MAXUINT, 0,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_LOCATION,
				g_param_spec_string ("location",
					"Location", "TODO",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_NO_EMBED,
				g_param_spec_boolean ("no-embed",
					"No embed", "TODO",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_RATING,
				g_param_spec_pointer ("rating",
					"Rating", "TODO",
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_youtube_video_init (GDataYouTubeVideo *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_YOUTUBE_VIDEO, GDataYouTubeVideoPrivate);
}

static void
gdata_youtube_video_dispose (GObject *object)
{
	GDataYouTubeVideoPrivate *priv = GDATA_YOUTUBE_VIDEO_GET_PRIVATE (object);

	if (priv->media_group != NULL)
		g_object_unref (priv->media_group);
	priv->media_group = NULL;

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_youtube_video_parent_class)->dispose (object);
}

static void
gdata_youtube_video_finalize (GObject *object)
{
	GDataYouTubeVideoPrivate *priv = GDATA_YOUTUBE_VIDEO_GET_PRIVATE (object);

	g_free (priv->location);
	gdata_gd_rating_free (priv->rating);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_youtube_video_parent_class)->finalize (object);
}

static void
gdata_youtube_video_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataYouTubeVideoPrivate *priv = GDATA_YOUTUBE_VIDEO_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_MEDIA_GROUP:
			g_value_set_object (value, priv->media_group);
			break;
		case PROP_VIEW_COUNT:
			g_value_set_uint (value, priv->view_count);
			break;
		case PROP_FAVORITE_COUNT:
			g_value_set_uint (value, priv->favorite_count);
			break;
		case PROP_LOCATION:
			g_value_set_string (value, priv->location);
			break;
		case PROP_NO_EMBED:
			g_value_set_boolean (value, priv->no_embed);
			break;
		case PROP_RATING:
			g_value_set_pointer (value, priv->rating);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gdata_youtube_video_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataYouTubeVideo *self = GDATA_YOUTUBE_VIDEO (object);

	switch (property_id) {
		case PROP_MEDIA_GROUP:
			gdata_youtube_video_set_media_group (self, g_value_get_object (value));
			break;
		case PROP_VIEW_COUNT:
			gdata_youtube_video_set_view_count (self, g_value_get_uint (value));
			break;
		case PROP_FAVORITE_COUNT:
			gdata_youtube_video_set_favorite_count (self, g_value_get_uint (value));
			break;
		case PROP_LOCATION:
			gdata_youtube_video_set_location (self, g_value_get_string (value));
			break;
		case PROP_NO_EMBED:
			gdata_youtube_video_set_no_embed (self, g_value_get_boolean (value));
			break;
		case PROP_RATING:
			gdata_youtube_video_set_rating (self, g_value_get_pointer (value));
			break;
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

	if (xmlStrcmp (node->name, (xmlChar*) "group") == 0) {
		/* media:group */
		GDataYouTubeMediaGroup *media_group = _gdata_youtube_media_group_new_from_xml_node (doc, node, error);
		if (media_group == NULL)
			return FALSE;

		gdata_youtube_video_set_media_group (self, GDATA_MEDIA_GROUP (media_group));
		g_object_unref (media_group);
	} else if (xmlStrcmp (node->name, (xmlChar*) "rating") == 0) {
		/* gd:rating */
		xmlChar *min, *max, *num_raters, *average;
		guint num_raters_uint;
		gdouble average_double;
		GDataGDRating *rating;

		min = xmlGetProp (node, (xmlChar*) "min");
		if (min == NULL)
			return gdata_parser_error_required_property_missing ("gd:rating", "min", error);

		max = xmlGetProp (node, (xmlChar*) "max");
		if (max == NULL) {
			gdata_parser_error_required_property_missing ("gd:rating", "max", error);
			xmlFree (min);
			return FALSE;
		}

		num_raters = xmlGetProp (node, (xmlChar*) "numRaters");
		if (num_raters == NULL)
			num_raters_uint = 0;
		else
			num_raters_uint = strtoul ((gchar*) num_raters, NULL, 10);
		xmlFree (num_raters);

		average = xmlGetProp (node, (xmlChar*) "average");
		if (average == NULL)
			average_double = 0;
		else
			average_double = strtod ((gchar*) average, NULL);
		xmlFree (average);

		rating = gdata_gd_rating_new (strtoul ((gchar*) min, NULL, 10),
					      strtoul ((gchar*) max, NULL, 10),
					      num_raters_uint, average_double);
		gdata_youtube_video_set_rating (self, rating);
	} else if (xmlStrcmp (node->name, (xmlChar*) "comments") == 0) {
		/* gd:comments */
		g_message ("TODO: comments unimplemented");
	} else if (xmlStrcmp (node->name, (xmlChar*) "statistics") == 0) {
		/* yt:statistics */
		xmlChar *view_count, *favorite_count;

		/* View count */
		view_count = xmlGetProp (node, (xmlChar*) "viewCount");
		if (view_count == NULL)
			return gdata_parser_error_required_property_missing ("yt:statistics", "viewCount", error);
		gdata_youtube_video_set_view_count (self, strtoul ((gchar*) view_count, NULL, 10));
		xmlFree (view_count);

		/* Favourite count */
		favorite_count = xmlGetProp (node, (xmlChar*) "favoriteCount");
		if (favorite_count == NULL)
			return gdata_parser_error_required_property_missing ("yt:statistics", "favoriteCount", error);
		gdata_youtube_video_set_favorite_count (self, strtoul ((gchar*) favorite_count, NULL, 10));
		xmlFree (favorite_count);
	} else if (xmlStrcmp (node->name, (xmlChar*) "location") == 0) {
		/* yt:location */
		xmlChar *location = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		gdata_youtube_video_set_location (self, (gchar*) location);
		xmlFree (location);
	} else if (xmlStrcmp (node->name, (xmlChar*) "where") == 0) {
		/* georss:where */
		g_message ("TODO: where unimplemented");
	} else if (xmlStrcmp (node->name, (xmlChar*) "noembed") == 0) {
		/* yt:noembed */
		gdata_youtube_video_set_no_embed (self, TRUE);
	} else if (xmlStrcmp (node->name, (xmlChar*) "recorded") == 0) {
		/* yt:recorded */
		g_message ("TODO: recorded unimplemented");
	} else if (_gdata_entry_parse_xml_node (GDATA_ENTRY (self), doc, node, &child_error) == FALSE) {
		if (g_error_matches (child_error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_UNHANDLED_XML_ELEMENT) == TRUE) {
			g_error_free (child_error);
			gdata_parser_error_unhandled_element ((gchar*) node->ns->prefix, (gchar*) node->name, "entry", error);
		} else {
			g_propagate_error (error, child_error);
		}

		return FALSE;
	}

	return TRUE;
}

GDataMediaGroup *
gdata_youtube_video_get_media_group (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->media_group;
}

void
gdata_youtube_video_set_media_group (GDataYouTubeVideo *self, GDataMediaGroup *media_group)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));
	g_return_if_fail (GDATA_IS_MEDIA_GROUP (media_group));

	if (self->priv->media_group != NULL)
		g_object_unref (self->priv->media_group);
	self->priv->media_group = g_object_ref (media_group);
	g_object_notify (G_OBJECT (self), "media-group");
}

guint
gdata_youtube_video_get_view_count (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), 0);
	return self->priv->view_count;
}

void
gdata_youtube_video_set_view_count (GDataYouTubeVideo *self, guint view_count)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));
	self->priv->view_count = view_count;
	g_object_notify (G_OBJECT (self), "view-count");
}

guint
gdata_youtube_video_get_favorite_count (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), 0);
	return self->priv->favorite_count;
}

void
gdata_youtube_video_set_favorite_count (GDataYouTubeVideo *self, guint favorite_count)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));
	self->priv->favorite_count = favorite_count;
	g_object_notify (G_OBJECT (self), "favorite-count");
}

const gchar *
gdata_youtube_video_get_location (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->location;
}

void
gdata_youtube_video_set_location (GDataYouTubeVideo *self, const gchar *location)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));

	g_free (self->priv->location);
	self->priv->location = g_strdup (location);
	g_object_notify (G_OBJECT (self), "location");
}

gboolean
gdata_youtube_video_get_no_embed (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), FALSE);
	return self->priv->no_embed;
}

void
gdata_youtube_video_set_no_embed (GDataYouTubeVideo *self, gboolean no_embed)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));
	self->priv->no_embed = no_embed;
	g_object_notify (G_OBJECT (self), "no-embed");
}

GDataGDRating *
gdata_youtube_video_get_rating (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->rating;
}

void
gdata_youtube_video_set_rating (GDataYouTubeVideo *self, GDataGDRating *rating)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));

	gdata_gd_rating_free (self->priv->rating); /* TODO: Not so happy about this memory management */
	self->priv->rating = rating;
	g_object_notify (G_OBJECT (self), "rating");
}
