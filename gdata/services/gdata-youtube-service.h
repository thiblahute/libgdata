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

#ifndef GDATA_YOUTUBE_SERVICE_H
#define GDATA_YOUTUBE_SERVICE_H

#include <glib.h>
#include <glib-object.h>
#include <libsoup/soup.h>

#include "gdata-service.h"

G_BEGIN_DECLS

/* TODO: Make this a proper GObject enum */
typedef enum {
	GDATA_YOUTUBE_TOP_RATED_FEED,
	GDATA_YOUTUBE_TOP_FAVORITES_FEED,
	GDATA_YOUTUBE_MOST_VIEWED_FEED,
	GDATA_YOUTUBE_MOST_POPULAR_FEED,
	GDATA_YOUTUBE_MOST_RECENT_FEED,
	GDATA_YOUTUBE_MOST_DISCUSSED_FEED,
	GDATA_YOUTUBE_MOST_LINKED_FEED,
	GDATA_YOUTUBE_MOST_RESPONDED_FEED,
	GDATA_YOUTUBE_RECENTLY_FEATURED_FEED,
	GDATA_YOUTUBE_WATCH_ON_MOBILE_FEED
} GDataYouTubeStandardFeedType;

#define GDATA_TYPE_YOUTUBE_SERVICE		(gdata_youtube_service_get_type ())
#define GDATA_YOUTUBE_SERVICE(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_YOUTUBE_SERVICE, GDataYouTubeService))
#define GDATA_YOUTUBE_SERVICE_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_YOUTUBE_SERVICE, GDataYouTubeServiceClass))
#define GDATA_IS_YOUTUBE_SERVICE(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_YOUTUBE_SERVICE))
#define GDATA_IS_YOUTUBE_SERVICE_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_YOUTUBE_SERVICE))
#define GDATA_YOUTUBE_SERVICE_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_YOUTUBE_SERVICE, GDataYouTubeServiceClass))

typedef struct _GDataYouTubeServicePrivate	GDataYouTubeServicePrivate;

typedef struct {
	GDataService parent;
	GDataYouTubeServicePrivate *priv;
} GDataYouTubeService;

typedef struct {
	GDataServiceClass parent;
} GDataYouTubeServiceClass;

GType gdata_youtube_service_get_type (void);

GDataYouTubeService *gdata_youtube_service_new (const gchar *developer_key, const gchar *client_id);

GDataFeed *gdata_youtube_service_query_standard_feed (GDataYouTubeService *self, GDataYouTubeStandardFeedType feed_type, GError **error);
GDataFeed *gdata_youtube_service_query_videos (GDataYouTubeService *self, const gchar *query_terms, GError **error);

const gchar *gdata_youtube_service_get_developer_key (GDataYouTubeService *self);
const gchar *gdata_youtube_service_get_youtube_user (GDataYouTubeService *self);

G_END_DECLS

#endif /* !GDATA_YOUTUBE_SERVICE_H */
