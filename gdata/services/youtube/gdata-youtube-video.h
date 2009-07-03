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

#ifndef GDATA_YOUTUBE_VIDEO_H
#define GDATA_YOUTUBE_VIDEO_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-entry.h>
#include <gdata/media/gdata-media-category.h>
#include <gdata/services/youtube/gdata-youtube-content.h>
#include <gdata/services/youtube/gdata-youtube-credit.h>
#include <gdata/services/youtube/gdata-youtube-state.h>

G_BEGIN_DECLS

#define GDATA_TYPE_YOUTUBE_VIDEO		(gdata_youtube_video_get_type ())
#define GDATA_YOUTUBE_VIDEO(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_YOUTUBE_VIDEO, GDataYouTubeVideo))
#define GDATA_YOUTUBE_VIDEO_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_YOUTUBE_VIDEO, GDataYouTubeVideoClass))
#define GDATA_IS_YOUTUBE_VIDEO(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_YOUTUBE_VIDEO))
#define GDATA_IS_YOUTUBE_VIDEO_CLASS(k)		(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_YOUTUBE_VIDEO))
#define GDATA_YOUTUBE_VIDEO_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_YOUTUBE_VIDEO, GDataYouTubeVideoClass))

typedef struct _GDataYouTubeVideoPrivate	GDataYouTubeVideoPrivate;

/**
 * GDataYouTubeVideo:
 *
 * All the fields in the #GDataYouTubeVideo structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataEntry parent;
	GDataYouTubeVideoPrivate *priv;
} GDataYouTubeVideo;

/**
 * GDataYouTubeVideoClass:
 *
 * All the fields in the #GDataYouTubeVideoClass structure are private and should never be accessed directly.
 **/
typedef struct {
	/*< private >*/
	GDataEntryClass parent;
} GDataYouTubeVideoClass;

GType gdata_youtube_video_get_type (void) G_GNUC_CONST;

GDataYouTubeVideo *gdata_youtube_video_new (const gchar *id) G_GNUC_WARN_UNUSED_RESULT;

guint gdata_youtube_video_get_view_count (GDataYouTubeVideo *self);
guint gdata_youtube_video_get_favorite_count (GDataYouTubeVideo *self);
const gchar *gdata_youtube_video_get_location (GDataYouTubeVideo *self);
void gdata_youtube_video_set_location (GDataYouTubeVideo *self, const gchar *location);
gboolean gdata_youtube_video_get_no_embed (GDataYouTubeVideo *self);
void gdata_youtube_video_set_no_embed (GDataYouTubeVideo *self, gboolean no_embed);
void gdata_youtube_video_get_rating (GDataYouTubeVideo *self, guint *min, guint *max, guint *count, gdouble *average);
const gchar *gdata_youtube_video_get_keywords (GDataYouTubeVideo *self);
void gdata_youtube_video_set_keywords (GDataYouTubeVideo *self, const gchar *keywords);
const gchar *gdata_youtube_video_get_player_uri (GDataYouTubeVideo *self);
gboolean gdata_youtube_video_is_restricted_in_country (GDataYouTubeVideo *self, const gchar *country);
const gchar *gdata_youtube_video_get_title (GDataYouTubeVideo *self);
void gdata_youtube_video_set_title (GDataYouTubeVideo *self, const gchar *title);
GDataMediaCategory *gdata_youtube_video_get_category (GDataYouTubeVideo *self);
void gdata_youtube_video_set_category (GDataYouTubeVideo *self, GDataMediaCategory *category);
GDataYouTubeCredit *gdata_youtube_video_get_credit (GDataYouTubeVideo *self);
const gchar *gdata_youtube_video_get_description (GDataYouTubeVideo *self);
void gdata_youtube_video_set_description (GDataYouTubeVideo *self, const gchar *description);
GDataYouTubeContent *gdata_youtube_video_look_up_content (GDataYouTubeVideo *self, const gchar *type);
GList *gdata_youtube_video_get_thumbnails (GDataYouTubeVideo *self);
guint gdata_youtube_video_get_duration (GDataYouTubeVideo *self);
gboolean gdata_youtube_video_is_private (GDataYouTubeVideo *self);
void gdata_youtube_video_set_is_private (GDataYouTubeVideo *self, gboolean is_private);
void gdata_youtube_video_get_uploaded (GDataYouTubeVideo *self, GTimeVal *uploaded);
const gchar *gdata_youtube_video_get_video_id (GDataYouTubeVideo *self);
gboolean gdata_youtube_video_is_draft (GDataYouTubeVideo *self);
void gdata_youtube_video_set_is_draft (GDataYouTubeVideo *self, gboolean is_draft);
GDataYouTubeState *gdata_youtube_video_get_state (GDataYouTubeVideo *self);
void gdata_youtube_video_get_recorded (GDataYouTubeVideo *self, GTimeVal *recorded);
void gdata_youtube_video_set_recorded (GDataYouTubeVideo *self, GTimeVal *recorded);

G_END_DECLS

#endif /* !GDATA_YOUTUBE_VIDEO_H */
