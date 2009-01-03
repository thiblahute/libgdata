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
#include <libsoup/soup.h>
#include <string.h>

#include "gdata-youtube-service.h"
#include "gdata-service.h"
#include "gdata-private.h"

static void gdata_youtube_service_finalize (GObject *object);
static void gdata_youtube_service_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_youtube_service_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static gboolean parse_authentication_response (GDataService *self, const gchar *response_body, GError **error);
static void append_query_headers (GDataService *self, SoupMessage *message);

struct _GDataYouTubeServicePrivate {
	gchar *youtube_user;
	gchar *developer_key;
};

enum {
	PROP_DEVELOPER_KEY = 1,
	PROP_YOUTUBE_USER
};

G_DEFINE_TYPE (GDataYouTubeService, gdata_youtube_service, GDATA_TYPE_SERVICE)
#define GDATA_YOUTUBE_SERVICE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_YOUTUBE_SERVICE, GDataYouTubeServicePrivate))

static void
gdata_youtube_service_class_init (GDataYouTubeServiceClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataServiceClass *service_class = GDATA_SERVICE_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataYouTubeServicePrivate));

	gobject_class->set_property = gdata_youtube_service_set_property;
	gobject_class->get_property = gdata_youtube_service_get_property;
	gobject_class->finalize = gdata_youtube_service_finalize;

	service_class->service_name = "youtube";
	service_class->authentication_uri = "https://www.google.com/youtube/accounts/ClientLogin";
	service_class->parse_authentication_response = parse_authentication_response;
	service_class->append_query_headers = append_query_headers;

	g_object_class_install_property (gobject_class, PROP_DEVELOPER_KEY,
				g_param_spec_string ("developer-key",
					"Developer key", "Your YouTube developer API key.",
					NULL,
					G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_YOUTUBE_USER,
				g_param_spec_string ("youtube-user",
					"YouTube username", "The YouTube account username.",
					NULL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_youtube_service_init (GDataYouTubeService *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_YOUTUBE_SERVICE, GDataYouTubeServicePrivate);
}

static void
gdata_youtube_service_finalize (GObject *object)
{
	GDataYouTubeServicePrivate *priv = GDATA_YOUTUBE_SERVICE_GET_PRIVATE (object);

	g_free (priv->youtube_user);
	g_free (priv->developer_key);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_youtube_service_parent_class)->finalize (object);
}

static void
gdata_youtube_service_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataYouTubeServicePrivate *priv = GDATA_YOUTUBE_SERVICE_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_DEVELOPER_KEY:
			g_value_set_string (value, priv->developer_key);
			break;
		case PROP_YOUTUBE_USER:
			g_value_set_string (value, priv->youtube_user);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gdata_youtube_service_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataYouTubeServicePrivate *priv = GDATA_YOUTUBE_SERVICE_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_DEVELOPER_KEY:
			priv->developer_key = g_value_dup_string (value);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static gboolean
parse_authentication_response (GDataService *self, const gchar *response_body, GError **error)
{
	GDataYouTubeServicePrivate *priv = GDATA_YOUTUBE_SERVICE (self)->priv;
	gchar *user_start, *user_end;

	/* Chain up to the parent method first */
	if (GDATA_SERVICE_CLASS (gdata_youtube_service_parent_class)->parse_authentication_response (self, response_body, error) == FALSE)
		return FALSE;

	/* Parse the response */
	user_start = strstr (response_body, "YouTubeUser=");
	if (user_start == NULL)
		goto protocol_error;
	user_start += strlen ("YouTubeUser=");

	user_end = strstr (user_start, "\n");
	if (user_end == NULL)
		goto protocol_error;

	priv->youtube_user = g_strndup (user_start, user_end - user_start);
	if (priv->youtube_user == NULL || strlen (priv->youtube_user) == 0)
		goto protocol_error;

	return TRUE;

protocol_error:
	g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
		     _("The server returned a malformed response."));
	return FALSE;
}

static void
append_query_headers (GDataService *self, SoupMessage *message)
{
	GDataYouTubeServicePrivate *priv = GDATA_YOUTUBE_SERVICE (self)->priv;
	gchar *key_header;

	/* Dev key and client headers */
	key_header = g_strdup_printf ("key=%s", priv->developer_key);
	soup_message_headers_append (message->request_headers, "X-GData-Key", key_header);
	g_free (key_header);

	soup_message_headers_append (message->request_headers, "X-GData-Client", gdata_service_get_client_id (self));
}

GDataYouTubeService *
gdata_youtube_service_new (const gchar *developer_key, const gchar *client_id)
{
	g_return_val_if_fail (developer_key != NULL, NULL);
	g_return_val_if_fail (client_id != NULL, NULL);

	return g_object_new (GDATA_TYPE_YOUTUBE_SERVICE,
			     "developer-key", developer_key,
			     "client-id", client_id,
			     NULL);
}

GDataYouTubeVideoFeed *
gdata_youtube_service_query_standard_feed (GDataYouTubeService *self, GDataYouTubeStandardFeedType feed_type, GError **error)
{
	const gchar *query_uri;

	/* TODO: Support the "time" parameter, as well as category- and region-specific feeds */

	switch (feed_type) {
	case GDATA_YOUTUBE_TOP_RATED_FEED:
		query_uri = "http://gdata.youtube.com/feeds/api/standardfeeds/top_rated";
		break;
	case GDATA_YOUTUBE_TOP_FAVORITES_FEED:
		query_uri = "http://gdata.youtube.com/feeds/api/standardfeeds/top_favorites";
		break;
	case GDATA_YOUTUBE_MOST_VIEWED_FEED:
		query_uri = "http://gdata.youtube.com/feeds/api/standardfeeds/most_viewed";
		break;
	case GDATA_YOUTUBE_MOST_POPULAR_FEED:
		query_uri = "http://gdata.youtube.com/feeds/api/standardfeeds/most_popular";
		break;
	case GDATA_YOUTUBE_MOST_RECENT_FEED:
		query_uri = "http://gdata.youtube.com/feeds/api/standardfeeds/most_recent";
		break;
	case GDATA_YOUTUBE_MOST_DISCUSSED_FEED:
		query_uri = "http://gdata.youtube.com/feeds/api/standardfeeds/most_discussed";
		break;
	case GDATA_YOUTUBE_MOST_LINKED_FEED:
		query_uri = "http://gdata.youtube.com/feeds/api/standardfeeds/most_linked";
		break;
	case GDATA_YOUTUBE_MOST_RESPONDED_FEED:
		query_uri = "http://gdata.youtube.com/feeds/api/standardfeeds/most_responded";
		break;
	case GDATA_YOUTUBE_RECENTLY_FEATURED_FEED:
		query_uri = "http://gdata.youtube.com/feeds/api/standardfeeds/recently_featured";
		break;
	case GDATA_YOUTUBE_WATCH_ON_MOBILE_FEED:
		query_uri = "http://gdata.youtube.com/feeds/api/standardfeeds/watch_on_mobile";
		break;
	default:
		g_assert_not_reached ();
	}

	return GDATA_YOUTUBE_VIDEO_FEED (gdata_service_query_full (GDATA_SERVICE (self), query_uri, _gdata_youtube_video_feed_new_from_xml, error));
}

GDataYouTubeVideoFeed *
gdata_youtube_service_query_videos (GDataYouTubeService *self, const gchar *query_terms, GError **error)
{
	GDataFeed *feed;
	gchar *query_uri, *escaped_terms;

	/* Make sure to escape the parameters first */
	escaped_terms = g_uri_escape_string (query_terms, NULL, TRUE);
	query_uri = g_strdup_printf ("http://gdata.youtube.com/feeds/api/videos?q=%s", escaped_terms);
	g_free (escaped_terms);

	/* Perform the query */
	feed = gdata_service_query_full (GDATA_SERVICE (self), query_uri, _gdata_youtube_video_feed_new_from_xml, error);
	g_free (query_uri);

	return GDATA_YOUTUBE_VIDEO_FEED (feed);
}

const gchar *
gdata_youtube_service_get_developer_key (GDataYouTubeService *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_SERVICE (self), NULL);
	return self->priv->developer_key;
}

const gchar *
gdata_youtube_service_get_youtube_user (GDataYouTubeService *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_SERVICE (self), NULL);
	return self->priv->youtube_user;
}
