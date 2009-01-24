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
#include "gdata-gdata.h"
#include "gdata-parser.h"
#include "gdata-media-rss.h"
#include "gdata-types.h"

static void gdata_youtube_video_finalize (GObject *object);
static void gdata_youtube_video_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_youtube_video_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

struct _GDataYouTubeVideoPrivate {
	guint view_count;
	guint favorite_count;
	gchar *location;
	gboolean no_embed;
	GDataGDRating *rating;

	/* media:group properties */
	gchar *keywords;
	gchar *player_uri;
	GDataMediaRating *media_rating;
	GDataMediaRestriction *restriction;
	GList *thumbnails; /* GDataMediaThumbnail */
	gchar *title;
	GDataMediaCategory *category;
	GList *contents; /* GDataMediaContent */
	GDataMediaCredit *credit;
	gchar *description;

	/* YouTube-specific media:group properties */
	gint duration;
	gboolean private;
	GTimeVal uploaded;
	gchar *video_id;
};

enum {
	PROP_VIEW_COUNT = 1,
	PROP_FAVORITE_COUNT,
	PROP_LOCATION,
	PROP_NO_EMBED,
	PROP_RATING,
	PROP_KEYWORDS,
	PROP_PLAYER_URI,
	PROP_MEDIA_RATING,
	PROP_RESTRICTION,
	PROP_TITLE,
	PROP_CATEGORY,
	PROP_CREDIT,
	PROP_DESCRIPTION,
	PROP_DURATION,
	PROP_PRIVATE,
	PROP_UPLOADED,
	PROP_VIDEO_ID
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
	gobject_class->finalize = gdata_youtube_video_finalize;

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
	g_object_class_install_property (gobject_class, PROP_KEYWORDS,
				g_param_spec_string ("keywords",
					"Keywords", "TODO",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_PLAYER_URI,
				g_param_spec_string ("player-uri",
					"Player URI", "TODO",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_MEDIA_RATING,
				g_param_spec_pointer ("media-rating",
					"Media rating", "TODO",
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_RESTRICTION,
				g_param_spec_pointer ("restriction",
					"Restriction", "TODO",
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_TITLE,
				g_param_spec_string ("title",
					"Title", "TODO",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_CATEGORY,
				g_param_spec_pointer ("category",
					"Category", "TODO",
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_CREDIT,
				g_param_spec_pointer ("credit",
					"Credit", "TODO",
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_DESCRIPTION,
				g_param_spec_string ("description",
					"Description", "TODO",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
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
}

static void
gdata_youtube_video_init (GDataYouTubeVideo *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_YOUTUBE_VIDEO, GDataYouTubeVideoPrivate);
}

static void
gdata_youtube_video_finalize (GObject *object)
{
	GDataYouTubeVideoPrivate *priv = GDATA_YOUTUBE_VIDEO_GET_PRIVATE (object);

	g_free (priv->location);
	gdata_gd_rating_free (priv->rating);

	g_free (priv->keywords);
	g_free (priv->player_uri);
	gdata_media_rating_free (priv->media_rating);
	gdata_media_restriction_free (priv->restriction);
	g_list_foreach (priv->thumbnails, (GFunc) gdata_media_thumbnail_free, NULL);
	g_list_free (priv->thumbnails);
	g_free (priv->title);
	gdata_media_category_free (priv->category);
	g_list_foreach (priv->contents, (GFunc) gdata_media_content_free, NULL);
	g_list_free (priv->contents);
	gdata_media_credit_free (priv->credit);
	g_free (priv->description);

	g_free (priv->video_id);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_youtube_video_parent_class)->finalize (object);
}

static void
gdata_youtube_video_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataYouTubeVideoPrivate *priv = GDATA_YOUTUBE_VIDEO_GET_PRIVATE (object);

	switch (property_id) {
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
		case PROP_KEYWORDS:
			g_value_set_string (value, priv->keywords);
			break;
		case PROP_PLAYER_URI:
			g_value_set_string (value, priv->player_uri);
			break;
		case PROP_MEDIA_RATING:
			g_value_set_pointer (value, priv->media_rating);
			break;
		case PROP_RESTRICTION:
			g_value_set_pointer (value, priv->restriction);
			break;
		case PROP_TITLE:
			g_value_set_string (value, priv->title);
			break;
		case PROP_CATEGORY:
			g_value_set_pointer (value, priv->category);
			break;
		case PROP_CREDIT:
			g_value_set_pointer (value, priv->credit);
			break;
		case PROP_DESCRIPTION:
			g_value_set_string (value, priv->description);
			break;
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
		case PROP_KEYWORDS:
			gdata_youtube_video_set_keywords (self, g_value_get_string (value));
			break;
		case PROP_PLAYER_URI:
			gdata_youtube_video_set_player_uri (self, g_value_get_string (value));
			break;
		case PROP_MEDIA_RATING:
			gdata_youtube_video_set_media_rating (self, g_value_get_pointer (value));
			break;
		case PROP_RESTRICTION:
			gdata_youtube_video_set_restriction (self, g_value_get_pointer (value));
			break;
		case PROP_TITLE:
			gdata_youtube_video_set_title (self, g_value_get_string (value));
			break;
		case PROP_CATEGORY:
			gdata_youtube_video_set_category (self, g_value_get_pointer (value));
			break;
		case PROP_CREDIT:
			gdata_youtube_video_set_credit (self, g_value_get_pointer (value));
			break;
		case PROP_DESCRIPTION:
			gdata_youtube_video_set_description (self, g_value_get_string (value));
			break;
		case PROP_DURATION:
			gdata_youtube_video_set_duration (self, g_value_get_int (value));
			break;
		case PROP_PRIVATE:
			gdata_youtube_video_set_private (self, g_value_get_boolean (value));
			break;
		case PROP_UPLOADED:
			gdata_youtube_video_set_uploaded (self, g_value_get_boxed (value));
			break;
		case PROP_VIDEO_ID:
			gdata_youtube_video_set_video_id (self, g_value_get_string (value));
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

static gboolean
parse_media_group_xml_node (GDataYouTubeVideo *self, xmlDoc *doc, xmlNode *node, GError **error)
{
	if (xmlStrcmp (node->name, (xmlChar*) "title") == 0) {
		/* media:title */
		xmlChar *title = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		gdata_youtube_video_set_title (self, (gchar*) title);
		xmlFree (title);
	} else if (xmlStrcmp (node->name, (xmlChar*) "description") == 0) {
		/* media:description */
		xmlChar *description = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		gdata_youtube_video_set_description (self, (gchar*) description);
		xmlFree (description);
	} else if (xmlStrcmp (node->name, (xmlChar*) "keywords") == 0) {
		/* media:keywords */
		xmlChar *keywords = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		gdata_youtube_video_set_keywords (self, (gchar*) keywords);
		xmlFree (keywords);
	} else if (xmlStrcmp (node->name, (xmlChar*) "category") == 0) {
		/* media:category */
		xmlChar *scheme, *label, *content;
		GDataMediaCategory *category;

		scheme = xmlGetProp (node, (xmlChar*) "scheme");
		label = xmlGetProp (node, (xmlChar*) "label");
		content = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);

		category = gdata_media_category_new ((gchar*) content, (gchar*) scheme, (gchar*) label);
		gdata_youtube_video_set_category (self, category);
	} else if (xmlStrcmp (node->name, (xmlChar*) "content") == 0) {
		/* media:content */
		xmlChar *uri, *type, *is_default, *expression, *duration, *format;
		gboolean is_default_bool;
		GDataMediaExpression expression_enum;
		gint duration_int, format_int;
		GDataMediaContent *content;

		/* Parse isDefault */
		is_default = xmlGetProp (node, (xmlChar*) "isDefault");
		if (is_default == NULL || xmlStrcmp (is_default, (xmlChar*) "false") == 0)
			is_default_bool = FALSE;
		else if (xmlStrcmp (is_default, (xmlChar*) "true") == 0)
			is_default_bool = TRUE;
		else {
			gdata_parser_error_unknown_property_value ("media:content", "isDefault", (gchar*) is_default, error);
			xmlFree (is_default);
			return FALSE;
		}
		xmlFree (is_default);

		/* Parse expression */
		expression = xmlGetProp (node, (xmlChar*) "expression");
		if (xmlStrcmp (expression, (xmlChar*) "sample") == 0)
			expression_enum = GDATA_MEDIA_EXPRESSION_SAMPLE;
		else if (xmlStrcmp (expression, (xmlChar*) "full") == 0)
			expression_enum = GDATA_MEDIA_EXPRESSION_FULL;
		else if (xmlStrcmp (expression, (xmlChar*) "nonstop") == 0)
			expression_enum = GDATA_MEDIA_EXPRESSION_NONSTOP;
		else {
			gdata_parser_error_unknown_property_value ("media:content", "expression", (gchar*) expression, error);
			xmlFree (expression);
			return FALSE;
		}
		xmlFree (expression);

		/* Parse duration */
		duration = xmlGetProp (node, (xmlChar*) "duration");
		if (duration == NULL)
			duration_int = -1;
		else
			duration_int = strtoul ((gchar*) duration, NULL, 10);
		xmlFree (duration);

		format = xmlGetProp (node, (xmlChar*) "format");
		if (format == NULL)
			format_int = -1;
		else
			format_int = strtoul ((gchar*) format, NULL, 10);
		xmlFree (format);

		uri = xmlGetProp (node, (xmlChar*) "url");
		type = xmlGetProp (node, (xmlChar*) "type");

		content = gdata_media_content_new ((gchar*) uri, (gchar*) type, is_default_bool, expression_enum, duration_int, format_int);
		gdata_youtube_video_add_content (self, content);
	} else if (xmlStrcmp (node->name, (xmlChar*) "credit") == 0) {
		/* media:credit */
		xmlChar *role, *type, *content;
		GDataMediaCredit *credit;

		/* Check the role property is "uploader" */
		role = xmlGetProp (node, (xmlChar*) "role");
		if (xmlStrcmp (role, (xmlChar*) "uploader") != 0) {
			gdata_parser_error_unknown_property_value ("media:credit", "role", (gchar*) role, error);
			xmlFree (role);
			return FALSE;
		}
		xmlFree (role);

		/* Check the type property */
		type = xmlGetProp (node, (xmlChar*) "type");
		if (type != NULL && xmlStrcmp (type, (xmlChar*) "partner") != 0) {
			gdata_parser_error_unknown_property_value ("media:credit", "type", (gchar*) type, error);
			xmlFree (type);
			return FALSE;
		}

		content = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);

		credit = gdata_media_credit_new ((gchar*) content, (type != NULL) ? TRUE : FALSE);
		gdata_youtube_video_set_credit (self, credit);

		xmlFree (type);
		xmlFree (content);
	} else if (xmlStrcmp (node->name, (xmlChar*) "player") == 0) {
		/* media:player */
		xmlChar *player_uri = xmlGetProp (node, (xmlChar*) "url");
		gdata_youtube_video_set_player_uri (self, (gchar*) player_uri);
		xmlFree (player_uri);
	} else if (xmlStrcmp (node->name, (xmlChar*) "rating") == 0) {
		/* media:rating */
		xmlChar *scheme, *country;
		GDataMediaRating *rating;

		scheme = xmlGetProp (node, (xmlChar*) "scheme");
		country = xmlGetProp (node, (xmlChar*) "country");

		rating = gdata_media_rating_new ((gchar*) scheme, (gchar*) country);
		gdata_youtube_video_set_media_rating (self, rating);

		xmlFree (scheme);
		xmlFree (country);
	} else if (xmlStrcmp (node->name, (xmlChar*) "restriction") == 0) {
		/* media:restriction */
		xmlChar *type, *countries, *relationship;
		gboolean relationship_bool;
		GDataMediaRestriction *restriction;

		/* Check the type property is "country" */
		type = xmlGetProp (node, (xmlChar*) "type");
		if (xmlStrcmp (type, (xmlChar*) "country") != 0) {
			gdata_parser_error_unknown_property_value ("media:restriction", "type", (gchar*) type, error);
			xmlFree (type);
			return FALSE;
		}
		xmlFree (type);

		countries = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		relationship = xmlGetProp (node, (xmlChar*) "relationship");

		if (xmlStrcmp (relationship, (xmlChar*) "allow") == 0)
			relationship_bool = TRUE;
		else if (xmlStrcmp (relationship, (xmlChar*) "deny") == 0)
			relationship_bool = FALSE;
		else {
			gdata_parser_error_unknown_property_value ("media:restriction", "relationship", (gchar*) relationship, error);
			xmlFree (relationship);
			return FALSE;
		}
		xmlFree (relationship);

		restriction = gdata_media_restriction_new ((gchar*) countries, relationship_bool);
		gdata_youtube_video_set_restriction (self, restriction);
	} else if (xmlStrcmp (node->name, (xmlChar*) "thumbnail") == 0) {
		/* media:thumbnail */
		xmlChar *uri, *width, *height, *time;
		guint width_uint, height_uint;
		gint time_int;
		GDataMediaThumbnail *thumbnail;

		/* Get the width and height */
		width = xmlGetProp (node, (xmlChar*) "width");
		if (width == NULL)
			return gdata_parser_error_required_property_missing ("media:thumbnail", "width", error);
		width_uint = strtoul ((gchar*) width, NULL, 10);
		xmlFree (width);

		height = xmlGetProp (node, (xmlChar*) "height");
		if (height == NULL)
			return gdata_parser_error_required_property_missing ("media:thumbnail", "height", error);
		height_uint = strtoul ((gchar*) height, NULL, 10);
		xmlFree (height);

		/* Get and parse the time */
		time = xmlGetProp (node, (xmlChar*) "time");
		if (time == NULL) {
			time_int = -1;
		} else {
			time_int = gdata_media_thumbnail_parse_time ((gchar*) time);
			if (time_int == -1) {
				g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
					     _("The @time property (\"%s\") of a <media:thumbnail> could not be parsed."),
					     (gchar*) time);
				xmlFree (time);
				return FALSE;
			}
			xmlFree (time);
		}

		uri = xmlGetProp (node, (xmlChar*) "url");

		thumbnail = gdata_media_thumbnail_new ((gchar*) uri, width_uint, height_uint, time_int);
		gdata_youtube_video_add_thumbnail (self, thumbnail);
	} else if (xmlStrcmp (node->name, (xmlChar*) "duration") == 0) {
		/* yt:duration */
		xmlChar *duration;
		gint duration_int;

		duration = xmlGetProp (node, (xmlChar*) "seconds");
		if (duration == NULL)
			duration_int = -1;
		else
			duration_int = strtoul ((gchar*) duration, NULL, 10);
		xmlFree (duration);

		gdata_youtube_video_set_duration (self, duration_int);
	} else if (xmlStrcmp (node->name, (xmlChar*) "private") == 0) {
		/* yt:private */
		gdata_youtube_video_set_private (self, TRUE);
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

		gdata_youtube_video_set_uploaded (self, &uploaded_timeval);
		xmlFree (uploaded);
	} else if (xmlStrcmp (node->name, (xmlChar*) "videoid") == 0) {
		/* yt:videoid */
		xmlChar *video_id = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		gdata_youtube_video_set_video_id (self, (gchar*) video_id);
		xmlFree (video_id);
	} else {
		return gdata_parser_error_unhandled_element ((gchar*) node->ns->prefix, (gchar*) node->name, "media:group", error);
	}

	return TRUE;
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
		xmlNode *child_node;

		child_node = node->xmlChildrenNode;
		while (child_node != NULL) {
			if (parse_media_group_xml_node (self, doc, child_node, error) == FALSE)
				return FALSE;
			child_node = child_node->next;
		}
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
		xmlChar *rel, *href, *count_hint;
		guint count_hint_uint;
		GDataGDFeedLink *feed_link;

		/* TODO: This is actually the child of the <comments> element */

		count_hint = xmlGetProp (node, (xmlChar*) "countHint");
		if (count_hint == NULL)
			count_hint_uint = 0;
		else
			count_hint_uint = strtoul ((gchar*) count_hint, NULL, 10);
		xmlFree (count_hint);

		rel = xmlGetProp (node, (xmlChar*) "rel");
		href = xmlGetProp (node, (xmlChar*) "href");

		feed_link = gdata_gd_feed_link_new ((gchar*) href, (gchar*) rel, count_hint_uint);
		/*gdata_youtube_video_set_comments_feed_link (self, feed_link);*/

		xmlFree (rel);
		xmlFree (href);
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
		if (g_error_matches (child_error, GDATA_PARSER_ERROR, GDATA_PARSER_ERROR_UNHANDLED_XML_ELEMENT) == TRUE) {
			g_error_free (child_error);
			gdata_parser_error_unhandled_element ((gchar*) node->ns->prefix, (gchar*) node->name, "entry", error);
		} else {
			g_propagate_error (error, child_error);
		}

		return FALSE;
	}

	return TRUE;
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

const gchar *
gdata_youtube_video_get_keywords (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->keywords;
}

void
gdata_youtube_video_set_keywords (GDataYouTubeVideo *self, const gchar *keywords)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));

	g_free (self->priv->keywords);
	self->priv->keywords = g_strdup (keywords);
	g_object_notify (G_OBJECT (self), "keywords");
}

const gchar *
gdata_youtube_video_get_player_uri (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->player_uri;
}

void
gdata_youtube_video_set_player_uri (GDataYouTubeVideo *self, const gchar *player_uri)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));

	g_free (self->priv->player_uri);
	self->priv->player_uri = g_strdup (player_uri);
	g_object_notify (G_OBJECT (self), "player-uri");
}

GDataMediaRating *
gdata_youtube_video_get_media_rating (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->media_rating;
}

void
gdata_youtube_video_set_media_rating (GDataYouTubeVideo *self, GDataMediaRating *rating)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));

	gdata_media_rating_free (self->priv->media_rating); /* TODO: not so happy about this memory management */
	self->priv->media_rating = rating;
	g_object_notify (G_OBJECT (self), "media-rating");
}

GDataMediaRestriction *
gdata_youtube_video_get_restriction (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->restriction;
}

void
gdata_youtube_video_set_restriction (GDataYouTubeVideo *self, GDataMediaRestriction *restriction)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));

	gdata_media_restriction_free (self->priv->restriction); /* TODO: not so happy about this memory management */
	self->priv->restriction = restriction;
	g_object_notify (G_OBJECT (self), "restriction");
}

const gchar *
gdata_youtube_video_get_title (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->title;
}

void
gdata_youtube_video_set_title (GDataYouTubeVideo *self, const gchar *title)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));

	g_free (self->priv->title);
	self->priv->title = g_strdup (title);
	g_object_notify (G_OBJECT (self), "title");
}

GDataMediaCategory *
gdata_youtube_video_get_category (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->category;
}

void
gdata_youtube_video_set_category (GDataYouTubeVideo *self, GDataMediaCategory *category)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));

	gdata_media_category_free (self->priv->category); /* TODO: not so happy about this memory management */
	self->priv->category = category;
	g_object_notify (G_OBJECT (self), "category");
}

GDataMediaCredit *
gdata_youtube_video_get_credit (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->credit;
}

void
gdata_youtube_video_set_credit (GDataYouTubeVideo *self, GDataMediaCredit *credit)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));

	gdata_media_credit_free (self->priv->credit); /* TODO: not so happy about this memory management */
	self->priv->credit = credit;
	g_object_notify (G_OBJECT (self), "credit");
}

const gchar *
gdata_youtube_video_get_description (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->description;
}

void
gdata_youtube_video_set_description (GDataYouTubeVideo *self, const gchar *description)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));

	g_free (self->priv->description);
	self->priv->description = g_strdup (description);
	g_object_notify (G_OBJECT (self), "keywords");
}

/* TODO: More content API */
void
gdata_youtube_video_add_content (GDataYouTubeVideo *self, GDataMediaContent *content)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));
	g_return_if_fail (content != NULL);

	self->priv->contents = g_list_prepend (self->priv->contents, content);
}

/* TODO: More thumbnail API */
void
gdata_youtube_video_add_thumbnail (GDataYouTubeVideo *self, GDataMediaThumbnail *thumbnail)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));
	g_return_if_fail (thumbnail != NULL);

	self->priv->thumbnails = g_list_prepend (self->priv->thumbnails, thumbnail);
}

gint
gdata_youtube_video_get_duration (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), -1);
	return self->priv->duration;
}

void
gdata_youtube_video_set_duration (GDataYouTubeVideo *self, gint duration)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));
	self->priv->duration = duration;
	g_object_notify (G_OBJECT (self), "duration");
}

gboolean
gdata_youtube_video_get_private (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), FALSE);
	return self->priv->private;
}

void
gdata_youtube_video_set_private (GDataYouTubeVideo *self, gboolean private)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));
	self->priv->private = private;
	g_object_notify (G_OBJECT (self), "private");
}

void
gdata_youtube_video_get_uploaded (GDataYouTubeVideo *self, GTimeVal *uploaded)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));

	uploaded->tv_sec = self->priv->uploaded.tv_sec;
	uploaded->tv_usec = self->priv->uploaded.tv_usec;
}

void
gdata_youtube_video_set_uploaded (GDataYouTubeVideo *self, GTimeVal *uploaded)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));
	self->priv->uploaded.tv_sec = uploaded->tv_sec;
	self->priv->uploaded.tv_usec = uploaded->tv_usec;
	g_object_notify (G_OBJECT (self), "uploaded");
}

const gchar *
gdata_youtube_video_get_video_id (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->video_id;
}

void
gdata_youtube_video_set_video_id (GDataYouTubeVideo *self, const gchar *video_id)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));

	g_free (self->priv->video_id);
	self->priv->video_id = g_strdup (video_id);
	g_object_notify (G_OBJECT (self), "video-id");
}
