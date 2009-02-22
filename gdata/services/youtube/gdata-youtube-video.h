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

#ifndef GDATA_YOUTUBE_VIDEO_H
#define GDATA_YOUTUBE_VIDEO_H

#include <glib.h>
#include <glib-object.h>

#include "gdata-entry.h"
#include "gdata-gdata.h"
#include "gdata-media-rss.h"

G_BEGIN_DECLS

#define GDATA_TYPE_YOUTUBE_VIDEO		(gdata_youtube_video_get_type ())
#define GDATA_YOUTUBE_VIDEO(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_YOUTUBE_VIDEO, GDataYouTubeVideo))
#define GDATA_YOUTUBE_VIDEO_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_YOUTUBE_VIDEO, GDataYouTubeVideoClass))
#define GDATA_IS_YOUTUBE_VIDEO(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_YOUTUBE_VIDEO))
#define GDATA_IS_YOUTUBE_VIDEO_CLASS(k)		(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_YOUTUBE_VIDEO))
#define GDATA_YOUTUBE_VIDEO_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_YOUTUBE_VIDEO, GDataYouTubeVideoClass))

typedef struct _GDataYouTubeVideoPrivate	GDataYouTubeVideoPrivate;

typedef struct {
	GDataEntry parent;
	GDataYouTubeVideoPrivate *priv;
} GDataYouTubeVideo;

typedef struct {
	GDataEntryClass parent;
} GDataYouTubeVideoClass;

GType gdata_youtube_video_get_type (void);

GDataYouTubeVideo *gdata_youtube_video_new (void);
GDataYouTubeVideo *gdata_youtube_video_new_from_xml (const gchar *xml, gint length, GError **error);

guint gdata_youtube_video_get_view_count (GDataYouTubeVideo *self);
void gdata_youtube_video_set_view_count (GDataYouTubeVideo *self, guint view_count);
guint gdata_youtube_video_get_favorite_count (GDataYouTubeVideo *self);
void gdata_youtube_video_set_favorite_count (GDataYouTubeVideo *self, guint favorite_count);
const gchar *gdata_youtube_video_get_location (GDataYouTubeVideo *self);
void gdata_youtube_video_set_location (GDataYouTubeVideo *self, const gchar *location);
gboolean gdata_youtube_video_get_no_embed (GDataYouTubeVideo *self);
void gdata_youtube_video_set_no_embed (GDataYouTubeVideo *self, gboolean no_embed);
GDataGDRating *gdata_youtube_video_get_rating (GDataYouTubeVideo *self);
void gdata_youtube_video_set_rating (GDataYouTubeVideo *self, GDataGDRating *rating);
const gchar *gdata_youtube_video_get_keywords (GDataYouTubeVideo *self);
void gdata_youtube_video_set_keywords (GDataYouTubeVideo *self, const gchar *keywords);
const gchar *gdata_youtube_video_get_player_uri (GDataYouTubeVideo *self);
void gdata_youtube_video_set_player_uri (GDataYouTubeVideo *self, const gchar *player_uri);
GDataMediaRating *gdata_youtube_video_get_media_rating (GDataYouTubeVideo *self);
void gdata_youtube_video_set_media_rating (GDataYouTubeVideo *self, GDataMediaRating *rating);
GDataMediaRestriction *gdata_youtube_video_get_restriction (GDataYouTubeVideo *self);
void gdata_youtube_video_set_restriction (GDataYouTubeVideo *self, GDataMediaRestriction *restriction);
const gchar *gdata_youtube_video_get_title (GDataYouTubeVideo *self);
void gdata_youtube_video_set_title (GDataYouTubeVideo *self, const gchar *title);
GDataMediaCategory *gdata_youtube_video_get_category (GDataYouTubeVideo *self);
void gdata_youtube_video_set_category (GDataYouTubeVideo *self, GDataMediaCategory *category);
GDataMediaCredit *gdata_youtube_video_get_credit (GDataYouTubeVideo *self);
void gdata_youtube_video_set_credit (GDataYouTubeVideo *self, GDataMediaCredit *credit);
const gchar *gdata_youtube_video_get_description (GDataYouTubeVideo *self);
void gdata_youtube_video_set_description (GDataYouTubeVideo *self, const gchar *description);
void gdata_youtube_video_add_content (GDataYouTubeVideo *self, GDataMediaContent *content);
void gdata_youtube_video_add_thumbnail (GDataYouTubeVideo *self, GDataMediaThumbnail *thumbnail);
gint gdata_youtube_video_get_duration (GDataYouTubeVideo *self);
void gdata_youtube_video_set_duration (GDataYouTubeVideo *self, gint duration);
gboolean gdata_youtube_video_get_private (GDataYouTubeVideo *self);
void gdata_youtube_video_set_private (GDataYouTubeVideo *self, gboolean private);
void gdata_youtube_video_get_uploaded (GDataYouTubeVideo *self, GTimeVal *uploaded);
void gdata_youtube_video_set_uploaded (GDataYouTubeVideo *self, GTimeVal *uploaded);
const gchar *gdata_youtube_video_get_video_id (GDataYouTubeVideo *self);
void gdata_youtube_video_set_video_id (GDataYouTubeVideo *self, const gchar *video_id);

G_END_DECLS

#endif /* !GDATA_YOUTUBE_VIDEO_H */
