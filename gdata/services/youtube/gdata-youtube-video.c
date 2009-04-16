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
 * SECTION:gdata-youtube-video
 * @short_description: GData YouTube video object
 * @stability: Unstable
 * @include: gdata/services/youtube/gdata-youtube-video.h
 *
 * #GDataYouTubeVideo is a subclass of #GDataEntry to represent a single video on YouTube, either when uploading or querying.
 *
 * For more details of YouTube's GData API, see the <ulink type="http" url="http://code.google.com/apis/youtube/2.0/reference.html">
 * online documentation</ulink>.
 **/

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
static void get_xml (GDataEntry *entry, GString *xml_string);
static gboolean parse_xml (GDataEntry *entry, xmlDoc *doc, xmlNode *node, GError **error);
static void get_namespaces (GDataEntry *entry, GHashTable *namespaces);

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
	guint duration;
	gboolean is_private;
	GTimeVal uploaded;
	gchar *video_id;

	/* Other properties */
	gboolean is_draft;
	GDataYouTubeState *state;
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
	PROP_IS_PRIVATE,
	PROP_UPLOADED,
	PROP_VIDEO_ID,
	PROP_IS_DRAFT,
	PROP_STATE
};

G_DEFINE_TYPE (GDataYouTubeVideo, gdata_youtube_video, GDATA_TYPE_ENTRY)
#define GDATA_YOUTUBE_VIDEO_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_YOUTUBE_VIDEO, GDataYouTubeVideoPrivate))

static void
gdata_youtube_video_class_init (GDataYouTubeVideoClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataEntryClass *entry_class = GDATA_ENTRY_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataYouTubeVideoPrivate));

	gobject_class->set_property = gdata_youtube_video_set_property;
	gobject_class->get_property = gdata_youtube_video_get_property;
	gobject_class->finalize = gdata_youtube_video_finalize;

	entry_class->get_xml = get_xml;
	entry_class->parse_xml = parse_xml;
	entry_class->get_namespaces = get_namespaces;

	/**
	 * GDataYouTubeVideo:view-count:
	 *
	 * The number of times the video has been viewed.
	 *
	 * For more information, see the <ulink type="http"
	 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_yt:statistics">online documentation</ulink>.
	 **/
	g_object_class_install_property (gobject_class, PROP_VIEW_COUNT,
				g_param_spec_uint ("view-count",
					"View count", "The number of times the video has been viewed.",
					0, G_MAXUINT, 0,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataYouTubeVideo:favorite-count:
	 *
	 * The number of YouTube users who have added the video to their list of favorite videos.
	 *
	 * For more information, see the <ulink type="http"
	 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_yt:statistics">online documentation</ulink>.
	 **/
	g_object_class_install_property (gobject_class, PROP_FAVORITE_COUNT,
				g_param_spec_uint ("favorite-count",
					"Favorite count", "The number of YouTube users who have added the video to their list of favorite videos.",
					0, G_MAXUINT, 0,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataYouTubeVideo:location:
	 *
	 * Descriptive text about the location where the video was taken.
	 *
	 * For more information, see the <ulink type="http"
	 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_yt:location">online documentation</ulink>.
	 **/
	g_object_class_install_property (gobject_class, PROP_LOCATION,
				g_param_spec_string ("location",
					"Location", "Descriptive text about the location where the video was taken.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataYouTubeVideo:no-embed:
	 *
	 * Specifies whether the video may not be embedded on other websites.
	 *
	 * For more information, see the <ulink type="http"
	 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_yt:noembed">online documentation</ulink>.
	 **/
	g_object_class_install_property (gobject_class, PROP_NO_EMBED,
				g_param_spec_boolean ("no-embed",
					"No embed?", "Specifies whether the video may not be embedded on other websites.",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataYouTubeVideo:rating:
	 *
	 * Specifies the current average rating of the video based on aggregated YouTube user ratings.
	 *
	 * It is a pointer to a #GDataGDRating.
	 *
	 * For more information, see the <ulink type="http"
	 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_gd:rating">online documentation</ulink>.
	 **/
	g_object_class_install_property (gobject_class, PROP_RATING,
				g_param_spec_pointer ("rating",
					"Rating", "Specifies the current average rating of the video based on aggregated YouTube user ratings.",
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataYouTubeVideo:keywords:
	 *
	 * A comma-separated list of words associated with the video.
	 *
	 * For more information, see the <ulink type="http"
	 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_media:keywords">online documentation</ulink>.
	 **/
	g_object_class_install_property (gobject_class, PROP_KEYWORDS,
				g_param_spec_string ("keywords",
					"Keywords", "A comma-separated list of words associated with the video.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataYouTubeVideo:player-uri:
	 *
	 * Specifies a URI where the full-length video is available through a media player that runs inside a web browser
	 * (i.e. the video's page on YouTube).
	 *
	 * For more information, see the <ulink type="http"
	 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_media:player">online documentation</ulink>.
	 **/
	g_object_class_install_property (gobject_class, PROP_PLAYER_URI,
				g_param_spec_string ("player-uri",
					"Player URI", "Specifies a URI where the full-length video is available through a media player"
					"that runs inside a web browser.",
					NULL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataYouTubeVideo:media-rating:
	 *
	 * Indicates that the video contains restricted content, although such restrictions might not apply in your country.
	 *
	 * It is a pointer to a #GDataMediaRating.
	 *
	 * For more information, see the <ulink type="http"
	 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_media:rating">online documentation</ulink>.
	 **/
	g_object_class_install_property (gobject_class, PROP_MEDIA_RATING,
				g_param_spec_pointer ("media-rating",
					"Media rating", "Indicates that the video contains restricted content, although such restrictions"
					"might not apply in your country.",
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataYouTubeVideo:restriction:
	 *
	 * Identifies the country or countries where the video may or may not be played.
	 *
	 * It is a pointer to a #GDataMediaRestriction.
	 *
	 * For more information, see the <ulink type="http"
	 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_media:restriction">online documentation</ulink>.
	 **/
	g_object_class_install_property (gobject_class, PROP_RESTRICTION,
				g_param_spec_pointer ("restriction",
					"Restriction", "Identifies the country or countries where the video may or may not be played.",
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataYouTubeVideo:title:
	 *
	 * Identifies the title of the video. This field has a maximum length of 60 characters or 100 bytes, whichever is reached first.
	 *
	 * For more information, see the <ulink type="http"
	 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_media:title">online documentation</ulink>.
	 **/
	g_object_class_install_property (gobject_class, PROP_TITLE,
				g_param_spec_string ("title",
					"Title", "Identifies the title of the video.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataYouTubeVideo:category:
	 *
	 * Specifies a genre or developer tag that describes the video.
	 *
	 * For more information, see the <ulink type="http"
	 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_media:category">online documentation</ulink>.
	 **/
	g_object_class_install_property (gobject_class, PROP_CATEGORY,
				g_param_spec_pointer ("category",
					"Category", "Specifies a genre or developer tag that describes the video.",
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataYouTubeVideo:credit:
	 *
	 * Identifies the owner of the video.
	 *
	 * For more information, see the <ulink type="http"
	 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_media:credit">online documentation</ulink>.
	 **/
	g_object_class_install_property (gobject_class, PROP_CREDIT,
				g_param_spec_pointer ("credit",
					"Credit", "Identifies the owner of the video.",
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataYouTubeVideo:description:
	 *
	 * A summary or description of the video.
	 *
	 * For more information, see the <ulink type="http"
	 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_media:description">online documentation</ulink>.
	 **/
	g_object_class_install_property (gobject_class, PROP_DESCRIPTION,
				g_param_spec_string ("description",
					"Description", "A summary or description of the video.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataYouTubeVideo:duration:
	 *
	 * The duration of the video in seconds.
	 *
	 * For more information, see the <ulink type="http"
	 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_yt:duration">online documentation</ulink>.
	 **/
	g_object_class_install_property (gobject_class, PROP_DURATION,
				g_param_spec_uint ("duration",
					"Duration", "The duration of the video in seconds.",
					0, G_MAXINT, 0,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataYouTubeVideo:private:
	 *
	 * Indicates whether the video is private, meaning that it will not be publicly visible on YouTube's website.
	 *
	 * For more information, see the <ulink type="http"
	 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_yt:private">online documentation</ulink>.
	 **/
	g_object_class_install_property (gobject_class, PROP_IS_PRIVATE,
				g_param_spec_boolean ("is-private",
					"Private?", "Indicates whether the video is private.",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataYouTubeVideo:uploaded:
	 *
	 * Specifies the time the video was originally uploaded to YouTube.
	 *
	 * For more information, see the <ulink type="http"
	 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_yt:uploaded">online documentation</ulink>.
	 **/
	g_object_class_install_property (gobject_class, PROP_UPLOADED,
				g_param_spec_boxed ("uploaded",
					"Uploaded", "Specifies the time the video was originally uploaded to YouTube.",
					G_TYPE_TIME_VAL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataYouTubeVideo:video-id:
	 *
	 * Specifies a unique ID which YouTube uses to identify the video. For example: <literal>qz8EfkS4KK0</literal>.
	 *
	 * For more information, see the <ulink type="http"
	 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_yt:videoid">online documentation</ulink>.
	 **/
	g_object_class_install_property (gobject_class, PROP_VIDEO_ID,
				g_param_spec_string ("video-id",
					"Video ID", "Specifies a unique ID which YouTube uses to identify the video.",
					NULL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataYouTubeVideo:is-draft:
	 *
	 * Indicates whether the video is in draft, or unpublished, status.
	 *
	 * For more information, see the <ulink type="http"
	 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_app:draft">online documentation</ulink>.
	 **/
	g_object_class_install_property (gobject_class, PROP_IS_DRAFT,
				g_param_spec_boolean ("is-draft",
					"Draft?", "Indicates whether the video is in draft, or unpublished, status.",
					FALSE, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataYouTubeVideo:state:
	 *
	 * Information describing the state of the video. If this is non-%NULL, the video is not playable.
	 * It points to a #GDataYouTubeState.
	 *
	 * For more information, see the <ulink type="http"
	 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_yt:state">online documentation</ulink>.
	 **/
	g_object_class_install_property (gobject_class, PROP_STATE,
				g_param_spec_pointer ("state",
					"State", "Information describing the state of the video.",
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
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
	gdata_youtube_state_free (priv->state);

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
			g_value_set_uint (value, priv->duration);
			break;
		case PROP_IS_PRIVATE:
			g_value_set_boolean (value, priv->is_private);
			break;
		case PROP_UPLOADED:
			g_value_set_boxed (value, &(priv->uploaded));
			break;
		case PROP_VIDEO_ID:
			g_value_set_string (value, priv->video_id);
			break;
		case PROP_IS_DRAFT:
			g_value_set_boolean (value, priv->is_draft);
			break;
		case PROP_STATE:
			g_value_set_pointer (value, priv->state);
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
		case PROP_LOCATION:
			gdata_youtube_video_set_location (self, g_value_get_string (value));
			break;
		case PROP_NO_EMBED:
			gdata_youtube_video_set_no_embed (self, g_value_get_boolean (value));
			break;
		/*case PROP_RATING:
			gdata_youtube_video_set_rating (self, g_value_get_pointer (value));
			break;*/
		case PROP_KEYWORDS:
			gdata_youtube_video_set_keywords (self, g_value_get_string (value));
			break;
		case PROP_TITLE:
			gdata_youtube_video_set_title (self, g_value_get_string (value));
			break;
		case PROP_CATEGORY:
			gdata_youtube_video_set_category (self, g_value_get_pointer (value));
			break;
		case PROP_DESCRIPTION:
			gdata_youtube_video_set_description (self, g_value_get_string (value));
			break;
		case PROP_IS_PRIVATE:
			gdata_youtube_video_set_is_private (self, g_value_get_boolean (value));
			break;
		case PROP_IS_DRAFT:
			gdata_youtube_video_set_is_draft (self, g_value_get_boolean (value));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

/**
 * gdata_youtube_video_new:
 * @id: the video's ID, or %NULL
 *
 * Creates a new #GDataYouTubeVideo with the given ID and default properties.
 *
 * Return value: a new #GDataYouTubeVideo; unref with g_object_unref()
 **/
GDataYouTubeVideo *
gdata_youtube_video_new (const gchar *id)
{
	return g_object_new (GDATA_TYPE_YOUTUBE_VIDEO, "id", id, NULL);
}

/**
 * gdata_youtube_video_new_from_xml:
 * @xml: an XML string
 * @length: the length in characters of @xml, or %-1
 * @error: a #GError, or %NULL
 *
 * Creates a new #GDataYouTubeVideo from an XML string. If @length is %-1, the length of
 * the string will be calculated.
 *
 * Errors from #GDataParserError can be returned if problems are found in the XML.
 *
 * Return value: a new #GDataYouTubeVideo, or %NULL; unref with g_object_unref()
 **/
GDataYouTubeVideo *
gdata_youtube_video_new_from_xml (const gchar *xml, gint length, GError **error)
{
	return GDATA_YOUTUBE_VIDEO (_gdata_entry_new_from_xml (GDATA_TYPE_YOUTUBE_VIDEO, xml, length, error));
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
		self->priv->contents = g_list_prepend (self->priv->contents, content);
	} else if (xmlStrcmp (node->name, (xmlChar*) "credit") == 0) {
		/* media:credit */
		xmlChar *role, *type, *content;

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

		gdata_media_credit_free (self->priv->credit);
		self->priv->credit = gdata_media_credit_new ((gchar*) content, (type != NULL) ? TRUE : FALSE);
		g_object_notify (G_OBJECT (self), "credit");

		xmlFree (type);
		xmlFree (content);
	} else if (xmlStrcmp (node->name, (xmlChar*) "player") == 0) {
		/* media:player */
		xmlChar *player_uri = xmlGetProp (node, (xmlChar*) "url");

		g_free (self->priv->player_uri);
		self->priv->player_uri = g_strdup ((gchar*) player_uri);
		g_object_notify (G_OBJECT (self), "player-uri");

		xmlFree (player_uri);
	} else if (xmlStrcmp (node->name, (xmlChar*) "rating") == 0) {
		/* media:rating */
		xmlChar *scheme, *country;

		scheme = xmlGetProp (node, (xmlChar*) "scheme");
		country = xmlGetProp (node, (xmlChar*) "country");

		gdata_media_rating_free (self->priv->media_rating);
		self->priv->media_rating = gdata_media_rating_new ((gchar*) scheme, (gchar*) country);
		g_object_notify (G_OBJECT (self), "media-rating");

		xmlFree (scheme);
		xmlFree (country);
	} else if (xmlStrcmp (node->name, (xmlChar*) "restriction") == 0) {
		/* media:restriction */
		xmlChar *type, *countries, *relationship;
		gboolean relationship_bool;

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

		gdata_media_restriction_free (self->priv->restriction);
		self->priv->restriction = gdata_media_restriction_new ((gchar*) countries, relationship_bool);
		g_object_notify (G_OBJECT (self), "restriction");
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
		self->priv->thumbnails = g_list_prepend (self->priv->thumbnails, thumbnail);
	} else if (xmlStrcmp (node->name, (xmlChar*) "duration") == 0) {
		/* yt:duration */
		xmlChar *duration = xmlGetProp (node, (xmlChar*) "seconds");
		if (duration == NULL)
			return gdata_parser_error_required_property_missing ("yt:duration", "seconds", error);

		self->priv->duration = strtoul ((gchar*) duration, NULL, 10);
		g_object_notify (G_OBJECT (self), "duration");
		xmlFree (duration);
	} else if (xmlStrcmp (node->name, (xmlChar*) "private") == 0) {
		/* yt:private */
		gdata_youtube_video_set_is_private (self, TRUE);
	} else if (xmlStrcmp (node->name, (xmlChar*) "uploaded") == 0) {
		/* yt:uploaded */
		xmlChar *uploaded;
		GTimeVal uploaded_timeval;

		uploaded = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		if (g_time_val_from_iso8601 ((gchar*) uploaded, &uploaded_timeval) == FALSE) {
			/* Error */
			gdata_parser_error_not_iso8601_format ("uploaded", "media:group", (gchar*) uploaded, error);
			xmlFree (uploaded);
			return FALSE;
		}
		xmlFree (uploaded);

		self->priv->uploaded = uploaded_timeval;
		g_object_notify (G_OBJECT (self), "uploaded");
	} else if (xmlStrcmp (node->name, (xmlChar*) "videoid") == 0) {
		/* yt:videoid */
		xmlChar *video_id = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		g_free (self->priv->video_id);
		self->priv->video_id = g_strdup ((gchar*) video_id);
		g_object_notify (G_OBJECT (self), "video-id");
		xmlFree (video_id);
	} else {
		return gdata_parser_error_unhandled_element ((gchar*) node->ns->prefix, (gchar*) node->name, "media:group", error);
	}

	return TRUE;
}

static gboolean
parse_xml (GDataEntry *entry, xmlDoc *doc, xmlNode *node, GError **error)
{
	GDataYouTubeVideo *self = GDATA_YOUTUBE_VIDEO (entry);

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
		/*GDataGDRating *rating;*/

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

		gdata_gd_rating_free (self->priv->rating);
		self->priv->rating = gdata_gd_rating_new (strtoul ((gchar*) min, NULL, 10),
							  strtoul ((gchar*) max, NULL, 10),
							  num_raters_uint, average_double);
		g_object_notify (G_OBJECT (self), "rating");
		/*gdata_youtube_video_set_rating (self, rating);*/
	} else if (xmlStrcmp (node->name, (xmlChar*) "comments") == 0) {
		/* gd:comments */
		xmlChar *rel, *href, *count_hint;
		xmlNode *child_node;
		guint count_hint_uint;
		/*GDataGDFeedLink *feed_link;*/

		/* This is actually the child of the <comments> element */
		child_node = node->xmlChildrenNode;

		count_hint = xmlGetProp (child_node, (xmlChar*) "countHint");
		if (count_hint == NULL)
			count_hint_uint = 0;
		else
			count_hint_uint = strtoul ((gchar*) count_hint, NULL, 10);
		xmlFree (count_hint);

		rel = xmlGetProp (child_node, (xmlChar*) "rel");
		href = xmlGetProp (child_node, (xmlChar*) "href");

		/* TODO */
		/*feed_link = gdata_gd_feed_link_new ((gchar*) href, (gchar*) rel, count_hint_uint);*/
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
		self->priv->view_count = strtoul ((gchar*) view_count, NULL, 10);
		g_object_notify (G_OBJECT (self), "view-count");
		xmlFree (view_count);

		/* Favourite count */
		favorite_count = xmlGetProp (node, (xmlChar*) "favoriteCount");
		if (favorite_count == NULL)
			self->priv->favorite_count = 0;
		else
			self->priv->favorite_count = strtoul ((gchar*) favorite_count, NULL, 10);
		g_object_notify (G_OBJECT (self), "favorite-count");
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
	} else if (xmlStrcmp (node->name, (xmlChar*) "control") == 0) {
		/* app:control */
		xmlNode *child_node;

		child_node = node->xmlChildrenNode;
		while (child_node != NULL) {
			if (xmlStrcmp (child_node->name, (xmlChar*) "draft") == 0) {
				/* app:draft */
				gdata_youtube_video_set_is_draft (self, TRUE);
			} else if (xmlStrcmp (child_node->name, (xmlChar*) "state") == 0) {
				/* yt:state */
				xmlChar *name, *message, *reason_code, *help_uri;

				name = xmlGetProp (child_node, (xmlChar*) "name");
				if (name == NULL)
					return gdata_parser_error_required_property_missing ("yt:state", "name", error);

				message = xmlNodeListGetString (doc, child_node->xmlChildrenNode, TRUE);
				reason_code = xmlGetProp (child_node, (xmlChar*) "reasonCode");
				help_uri = xmlGetProp (child_node, (xmlChar*) "helpUrl");

				gdata_youtube_state_free (self->priv->state);
				self->priv->state = gdata_youtube_state_new ((gchar*) name, (gchar*) message, (gchar*) reason_code, (gchar*) help_uri);
				g_object_notify (G_OBJECT (self), "state");

				xmlFree (name);
				xmlFree (message);
				xmlFree (reason_code);
				xmlFree (help_uri);
			} else {
				/* Unhandled element */
				return gdata_parser_error_unhandled_element ((gchar*) child_node->ns->prefix,
									     (gchar*) child_node->name, "app:control", error);
			}

			child_node = child_node->next;
		}
	} else if (GDATA_ENTRY_CLASS (gdata_youtube_video_parent_class)->parse_xml (entry, doc, node, error) == FALSE) {
		/* Error! */
		return FALSE;
	}

	return TRUE;
}

static void
get_xml (GDataEntry *entry, GString *xml_string)
{
	GDataYouTubeVideoPrivate *priv = GDATA_YOUTUBE_VIDEO (entry)->priv;
	gchar *category;
	GList *contents, *thumbnails;

	/* TODO: Make sure read-only properties aren't represented here */
	/* Chain up to the parent class */
	GDATA_ENTRY_CLASS (gdata_youtube_video_parent_class)->get_xml (entry, xml_string);

	/* Add all the YouTube-specific XML */
	g_string_append (xml_string, "<media:group><media:category");

	if (priv->category->label != NULL)
		g_string_append_printf (xml_string, " label='%s'", priv->category->label);
	if (priv->category->scheme != NULL)
		g_string_append_printf (xml_string, " scheme='%s'", priv->category->scheme);

	category = g_markup_escape_text (priv->category->category, -1);
	g_string_append_printf (xml_string, ">%s</media:category>", category);
	g_free (category);

	if (priv->title != NULL) {
		gchar *title = g_markup_escape_text (priv->title, -1);
		g_string_append_printf (xml_string, "<media:title type='plain'>%s</media:title>", title);
		g_free (title);
	}

	if (priv->description != NULL) {
		gchar *description = g_markup_escape_text (priv->description, -1);
		g_string_append_printf (xml_string, "<media:description type='plain'>%s</media:description>", description);
		g_free (description);
	}

	if (priv->keywords != NULL) {
		gchar *keywords = g_markup_escape_text (priv->keywords, -1);
		g_string_append_printf (xml_string, "<media:keywords>%s</media:keywords>", keywords);
		g_free (keywords);
	}

	for (contents = priv->contents; contents != NULL; contents = contents->next) {
		GDataMediaContent *content = (GDataMediaContent*) contents->data;
		gchar *uri, *type;

		uri = g_markup_escape_text (content->uri, -1);
		type = g_markup_escape_text (content->type, -1);
		g_string_append_printf (xml_string, "<media:content url='%s' type='%s'", uri, type);
		g_free (uri);
		g_free (type);

		if (content->is_default == TRUE)
			g_string_append (xml_string, " isDefault='true'");
		else
			g_string_append (xml_string, " isDefault='false'");

		switch (content->expression) {
		case GDATA_MEDIA_EXPRESSION_SAMPLE:
			g_string_append (xml_string, " expression='sample'");
			break;
		case GDATA_MEDIA_EXPRESSION_FULL:
			g_string_append (xml_string, " expression='full'");
			break;
		case GDATA_MEDIA_EXPRESSION_NONSTOP:
			g_string_append (xml_string, " expression='nonstop'");
			break;
		default:
			g_assert_not_reached ();
		}

		if (content->duration != -1)
			g_string_append_printf (xml_string, " duration='%i'", content->duration);
		if (content->format != -1)
			g_string_append_printf (xml_string, " yt:format='%i'", content->format);

		g_string_append (xml_string, "/>");
	}

	if (priv->player_uri != NULL) {
		gchar *player_uri = g_markup_escape_text (priv->player_uri, -1);
		g_string_append_printf (xml_string, "<media:player url='%s'/>", player_uri);
		g_free (player_uri);
	}

	for (thumbnails = priv->thumbnails; thumbnails != NULL; thumbnails = thumbnails->next) {
		GDataMediaThumbnail *thumbnail = (GDataMediaThumbnail*) thumbnails->data;
		gchar *uri, *time;

		uri = g_markup_escape_text (thumbnail->uri, -1);
		time = gdata_media_thumbnail_build_time (thumbnail->time);
		g_string_append_printf (xml_string, "<media:thumbnail url='%s' height='%u' width='%u' time='%s'/>",
					uri, thumbnail->height, thumbnail->width, time);
		g_free (uri);
		g_free (time);
	}

	if (priv->is_private == TRUE)
		g_string_append (xml_string, "<yt:private/>");

	if (priv->no_embed == TRUE)
		g_string_append (xml_string, "<yt:noembed/>");

	g_string_append (xml_string, "</media:group>");

	if (priv->location != NULL) {
		gchar *location = g_markup_escape_text (priv->location, -1);
		g_string_append_printf (xml_string, "<yt:location>%s</yt:location>", location);
		g_free (location);
	}

	/* TODO:
	 * - yt:recorded
	 * - georss:where
	 * - Check all tags here are valid for insertions and updates
	 * - Check things are escaped (or not) as appropriate
	 * - Write a function to encapsulate g_markup_escape_text and
	 *   g_string_append_printf to reduce the number of allocations
	 */
}

static void
get_namespaces (GDataEntry *entry, GHashTable *namespaces)
{
	g_hash_table_insert (namespaces, "media", "http://search.yahoo.com/mrss/");
	g_hash_table_insert (namespaces, "yt", "http://gdata.youtube.com/schemas/2007");
}

/**
 * gdata_youtube_video_get_view_count:
 * @self: a #GDataYouTubeVideo
 *
 * Gets the #GDataYouTubeVideo:view-count property.
 *
 * Return value: the number of times the video has been viewed
 **/
guint
gdata_youtube_video_get_view_count (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), 0);
	return self->priv->view_count;
}

/**
 * gdata_youtube_video_get_favorite_count:
 * @self: a #GDataYouTubeVideo
 *
 * Gets the #GDataYouTubeVideo:favorite-count property.
 *
 * Return value: the number of users who have added the video to their favorites list
 **/
guint
gdata_youtube_video_get_favorite_count (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), 0);
	return self->priv->favorite_count;
}

/**
 * gdata_youtube_video_get_location:
 * @self: a #GDataYouTubeVideo
 *
 * Gets the #GDataYouTubeVideo:location property.
 *
 * Return value: a string describing the video's location, or %NULL
 **/
const gchar *
gdata_youtube_video_get_location (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->location;
}

/**
 * gdata_youtube_video_set_location:
 * @self: a #GDataYouTubeVideo
 * @location: a new location, or %NULL
 *
 * Sets the #GDataYouTubeVideo:location property to the new location string, @location.
 *
 * Set @location to %NULL to unset the property in the video.
 **/
void
gdata_youtube_video_set_location (GDataYouTubeVideo *self, const gchar *location)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));

	g_free (self->priv->location);
	self->priv->location = g_strdup (location);
	g_object_notify (G_OBJECT (self), "location");
}

/**
 * gdata_youtube_video_get_no_embed:
 * @self: a #GDataYouTubeVideo
 *
 * Gets the #GDataYouTubeVideo:no-embed property.
 *
 * Return value: %TRUE if the video cannot be embedded on web pages, %FALSE otherwise
 **/
gboolean
gdata_youtube_video_get_no_embed (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), FALSE);
	return self->priv->no_embed;
}

/**
 * gdata_youtube_video_set_no_embed:
 * @self: a #GDataYouTubeVideo
 * @no_embed: whether the video can be embedded 
 *
 * Sets the #GDataYouTubeVideo:no-embed property to @no_embed.
 **/
void
gdata_youtube_video_set_no_embed (GDataYouTubeVideo *self, gboolean no_embed)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));
	self->priv->no_embed = no_embed;
	g_object_notify (G_OBJECT (self), "no-embed");
}

/**
 * gdata_youtube_video_get_rating:
 * @self: a #GDataYouTubeVideo
 *
 * Gets the #GDataYouTubeVideo:rating property.
 *
 * Return value: a #GDataGDRating describing the popularity of the video, or %NULL
 **/
GDataGDRating *
gdata_youtube_video_get_rating (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->rating;
}

/* TODO: The only use for this is for adding a new rating, but there must be a better interface for that */
/*void
gdata_youtube_video_set_rating (GDataYouTubeVideo *self, GDataGDRating *rating)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));

	gdata_gd_rating_free (self->priv->rating);  TODO: Not so happy about this memory management 
	self->priv->rating = rating;
	g_object_notify (G_OBJECT (self), "rating");
}*/

/**
 * gdata_youtube_video_get_keywords:
 * @self: a #GDataYouTubeVideo
 *
 * Gets the #GDataYouTubeVideo:keywords property.
 *
 * Return value: a comma-separated list of words associated with the video
 **/
const gchar *
gdata_youtube_video_get_keywords (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->keywords;
}

/**
 * gdata_youtube_video_set_keywords:
 * @self: a #GDataYouTubeVideo
 * @keywords: a new comma-separated list of keywords
 *
 * Sets the #GDataYouTubeVideo:keywords property to the new keyword list, @keywords.
 *
 * @keywords must not be %NULL. For more information, see the <ulink type="http"
 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_media:keywords">online documentation</ulink>.
 **/
void
gdata_youtube_video_set_keywords (GDataYouTubeVideo *self, const gchar *keywords)
{
	g_return_if_fail (keywords != NULL);
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));

	g_free (self->priv->keywords);
	self->priv->keywords = g_strdup (keywords);
	g_object_notify (G_OBJECT (self), "keywords");
}

/**
 * gdata_youtube_video_get_player_uri:
 * @self: a #GDataYouTubeVideo
 *
 * Gets the #GDataYouTubeVideo:player-uri property.
 *
 * Return value: a URI where the video is playable in a web browser, or %NULL
 **/
const gchar *
gdata_youtube_video_get_player_uri (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->player_uri;
}

/**
 * gdata_youtube_video_get_media_rating:
 * @self: a #GDataYouTubeVideo
 *
 * Gets the #GDataYouTubeVideo:media-rating property.
 *
 * Return value: a #GDataMediaRating giving information about restrictions on the video, or %NULL if there are none
 **/
GDataMediaRating *
gdata_youtube_video_get_media_rating (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->media_rating;
}

/**
 * gdata_youtube_video_get_restriction:
 * @self: a #GDataYouTubeVideo
 *
 * Gets the #GDataYouTubeVideo:restriction property.
 *
 * Return value: a #GDataMediaRestriction giving information on countries where the video is restricted, or %NULL if there are none
 **/
GDataMediaRestriction *
gdata_youtube_video_get_restriction (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->restriction;
}

/**
 * gdata_youtube_video_get_title:
 * @self: a #GDataYouTubeVideo
 *
 * Gets the #GDataYouTubeVideo:title property.
 *
 * Return value: the video's title, or %NULL
 **/
const gchar *
gdata_youtube_video_get_title (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->title;
}

/**
 * gdata_youtube_video_set_title:
 * @self: a #GDataYouTubeVideo
 * @title: the new video title
 *
 * Sets the #GDataYouTubeVideo:title property to the new title, @title.
 *
 * Set @title to %NULL to unset the video's title.
 **/
void
gdata_youtube_video_set_title (GDataYouTubeVideo *self, const gchar *title)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));

	g_free (self->priv->title);
	self->priv->title = g_strdup (title);
	g_object_notify (G_OBJECT (self), "title");
}

/**
 * gdata_youtube_video_get_category:
 * @self: a #GDataYouTubeVideo
 *
 * Gets the #GDataYouTubeVideo:category property.
 *
 * Return value: a #GDataMediaCategory giving the video's single and mandatory category
 **/
GDataMediaCategory *
gdata_youtube_video_get_category (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->category;
}

/**
 * gdata_youtube_video_set_category:
 * @self: a #GDataYouTubeVideo
 * @category: a new #GDataMediaCategory
 *
 * Sets the #GDataYouTubeVideo:category property to the new category, @category.
 *
 * @category must not be %NULL. For more information, see the <ulink type="http"
 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_media:category">online documentation</ulink>.
 **/
void
gdata_youtube_video_set_category (GDataYouTubeVideo *self, GDataMediaCategory *category)
{
	g_return_if_fail (category != NULL);
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));

	gdata_media_category_free (self->priv->category); /* TODO: not so happy about this memory management */
	self->priv->category = category;
	g_object_notify (G_OBJECT (self), "category");
}

/**
 * gdata_youtube_video_get_credit:
 * @self: a #GDataYouTubeVideo
 *
 * Gets the #GDataYouTubeVideo:credit property.
 *
 * Return value: a #GDataMediaCredit giving information on who to credit for the video, or %NULL
 **/
GDataMediaCredit *
gdata_youtube_video_get_credit (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->credit;
}

/**
 * gdata_youtube_video_get_description:
 * @self: a #GDataYouTubeVideo
 *
 * Gets the #GDataYouTubeVideo:description property.
 *
 * Return value: the video's long text description, or %NULL
 **/
const gchar *
gdata_youtube_video_get_description (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->description;
}

/**
 * gdata_youtube_video_set_description:
 * @self: a #GDataYouTubeVideo
 * @description: the video's new description
 *
 * Sets the #GDataYouTubeVideo:description property to the new description, @description.
 *
 * Set @description to %NULL to unset the video's description.
 **/
void
gdata_youtube_video_set_description (GDataYouTubeVideo *self, const gchar *description)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));

	g_free (self->priv->description);
	self->priv->description = g_strdup (description);
	g_object_notify (G_OBJECT (self), "keywords");
}

static gint
content_compare_cb (const GDataMediaContent *content, const gchar *type)
{
	return strcmp (content->type, type);
}

/**
 * gdata_youtube_video_look_up_content:
 * @self: a #GDataYouTubeVideo
 * @type: the MIME type of the content desired
 *
 * Looks up a #GDataMediaContent from the video with the given MIME type. The video's list of contents is
 * a list of URIs to various formats of the video itself, such as its SWF URI or RTSP stream.
 *
 * Return value: a #GDataMediaContent matching @type, or %NULL
 **/
GDataMediaContent *
gdata_youtube_video_look_up_content (GDataYouTubeVideo *self, const gchar *type)
{
	GList *element;

	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	g_return_val_if_fail (type != NULL, NULL);

	/* TODO: If type is required, and is unique, the contents can be stored in a hash table rather than a linked list */
	element = g_list_find_custom (self->priv->contents, type, (GCompareFunc) content_compare_cb);
	if (element == NULL)
		return NULL;
	return (GDataMediaContent*) (element->data);
}

/**
 * gdata_youtube_video_get_thumbnails:
 * @self: a #GDataYouTubeVideo
 *
 * Gets a list of the thumbnails available for the video.
 *
 * Return value: a #GList of #GDataMediaThumbnail<!-- -->s, or %NULL
 **/
GList *
gdata_youtube_video_get_thumbnails (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->thumbnails;
}

/**
 * gdata_youtube_video_get_duration:
 * @self: a #GDataYouTubeVideo
 *
 * Gets the #GDataYouTubeVideo:duration property.
 *
 * Return value: the video duration in seconds, or %0 if unknown
 **/
guint
gdata_youtube_video_get_duration (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), 0);
	return self->priv->duration;
}

/**
 * gdata_youtube_video_is_private:
 * @self: a #GDataYouTubeVideo
 *
 * Gets the #GDataYouTubeVideo:is-private property.
 *
 * Return value: %TRUE if the video is private, %FALSE otherwise
 **/
gboolean
gdata_youtube_video_is_private (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), FALSE);
	return self->priv->is_private;
}

/**
 * gdata_youtube_video_set_is_private:
 * @self: a #GDataYouTubeVideo
 * @is_private: whether the video is private
 *
 * Sets the #GDataYouTubeVideo:is-private property to decide whether the video is publicly viewable.
 **/
void
gdata_youtube_video_set_is_private (GDataYouTubeVideo *self, gboolean is_private)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));
	self->priv->is_private = is_private;
	g_object_notify (G_OBJECT (self), "is-private");
}

/**
 * gdata_youtube_video_get_uploaded:
 * @self: a #GDataYouTubeVideo
 * @uploaded: a #GTimeVal
 *
 * Gets the #GDataYouTubeVideo:uploaded property and puts it in @uploaded. If the property is unset,
 * both fields in the #GTimeVal will be set to 0.
 **/
void
gdata_youtube_video_get_uploaded (GDataYouTubeVideo *self, GTimeVal *uploaded)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));
	*uploaded = self->priv->uploaded;
}

/**
 * gdata_youtube_video_get_video_id:
 * @self: a #GDataYouTubeVideo
 *
 * Gets the #GDataYouTubeVideo:video-id property.
 *
 * Return value: the video's unique and permanent ID
 **/
const gchar *
gdata_youtube_video_get_video_id (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->video_id;
}

/**
 * gdata_youtube_video_is_draft:
 * @self: a #GDataYouTubeVideo
 *
 * Gets the #GDataYouTubeVideo:is-draft property.
 *
 * Return value: %TRUE if the video is a draft, %FALSE otherwise
 **/
gboolean
gdata_youtube_video_is_draft (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), FALSE);
	return self->priv->is_draft;
}

/**
 * gdata_youtube_video_set_is_draft:
 * @self: a #GDataYouTubeVideo
 * @is_draft: whether the video is a draft
 *
 * Sets the #GDataYouTubeVideo:is-draft property to decide whether the video is a draft.
 **/
void
gdata_youtube_video_set_is_draft (GDataYouTubeVideo *self, gboolean is_draft)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_VIDEO (self));
	self->priv->is_draft = is_draft;
	g_object_notify (G_OBJECT (self), "is-draft");
}

/**
 * gdata_youtube_video_get_state:
 * @self: a #GDataYouTubeVideo
 *
 * Gets the #GDataYouTubeVideo:state property.
 *
 * For more information, see the <ulink type="http"
 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_yt:state">online documentation</ulink>.
 *
 * Return value: a #GDataYouTubeState showing the state of the video, or %NULL
 **/
GDataYouTubeState *
gdata_youtube_video_get_state (GDataYouTubeVideo *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_VIDEO (self), NULL);
	return self->priv->state;
}
