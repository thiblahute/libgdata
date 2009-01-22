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

#ifndef GDATA_YOUTUBE_MEDIA_GROUP_H
#define GDATA_YOUTUBE_MEDIA_GROUP_H

#include <glib.h>
#include <glib-object.h>

#include "gdata-media-group.h"

G_BEGIN_DECLS

#define GDATA_TYPE_YOUTUBE_MEDIA_GROUP		(gdata_youtube_media_group_get_type ())
#define GDATA_YOUTUBE_MEDIA_GROUP(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_YOUTUBE_MEDIA_GROUP, GDataYouTubeMediaGroup))
#define GDATA_YOUTUBE_MEDIA_GROUP_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_YOUTUBE_MEDIA_GROUP, GDataYouTubeMediaGroupClass))
#define GDATA_IS_YOUTUBE_MEDIA_GROUP(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_YOUTUBE_MEDIA_GROUP))
#define GDATA_IS_YOUTUBE_MEDIA_GROUP_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_YOUTUBE_MEDIA_GROUP))
#define GDATA_YOUTUBE_MEDIA_GROUP_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_YOUTUBE_MEDIA_GROUP, GDataYouTubeMediaGroupClass))

typedef struct _GDataYouTubeMediaGroupPrivate	GDataYouTubeMediaGroupPrivate;

typedef struct {
	GDataMediaGroup parent;
	GDataYouTubeMediaGroupPrivate *priv;
} GDataYouTubeMediaGroup;

typedef struct {
	GDataMediaGroupClass parent;
} GDataYouTubeMediaGroupClass;

GType gdata_youtube_media_group_get_type (void);

GDataYouTubeMediaGroup *gdata_youtube_media_group_new (void);

gint gdata_youtube_media_group_get_duration (GDataYouTubeMediaGroup *self);
void gdata_youtube_media_group_set_duration (GDataYouTubeMediaGroup *self, gint duration);
gboolean gdata_youtube_media_group_get_private (GDataYouTubeMediaGroup *self);
void gdata_youtube_media_group_set_private (GDataYouTubeMediaGroup *self, gboolean private);
void gdata_youtube_media_group_get_uploaded (GDataYouTubeMediaGroup *self, GTimeVal *uploaded);
void gdata_youtube_media_group_set_uploaded (GDataYouTubeMediaGroup *self, GTimeVal *uploaded);
const gchar *gdata_youtube_media_group_get_video_id (GDataYouTubeMediaGroup *self);
void gdata_youtube_media_group_set_video_id (GDataYouTubeMediaGroup *self, const gchar *video_id);
gboolean gdata_youtube_media_group_get_no_embed (GDataYouTubeMediaGroup *self);
void gdata_youtube_media_group_set_no_embed (GDataYouTubeMediaGroup *self, gboolean no_embed);

G_END_DECLS

#endif /* !GDATA_YOUTUBE_MEDIA_GROUP_H */
