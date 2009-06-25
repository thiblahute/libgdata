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

#ifndef GDATA_YOUTUBE_GROUP_H
#define GDATA_YOUTUBE_GROUP_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-parsable.h>
#include <gdata/media/gdata-media-group.h>

G_BEGIN_DECLS

#define GDATA_TYPE_YOUTUBE_GROUP		(gdata_youtube_group_get_type ())
#define GDATA_YOUTUBE_GROUP(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_YOUTUBE_GROUP, GDataYouTubeGroup))
#define GDATA_YOUTUBE_GROUP_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_YOUTUBE_GROUP, GDataYouTubeGroupClass))
#define GDATA_IS_YOUTUBE_GROUP(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_YOUTUBE_GROUP))
#define GDATA_IS_YOUTUBE_GROUP_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_YOUTUBE_GROUP))
#define GDATA_YOUTUBE_GROUP_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_YOUTUBE_GROUP, GDataYouTubeGroupClass))

typedef struct _GDataYouTubeGroupPrivate	GDataYouTubeGroupPrivate;

/**
 * GDataYouTubeGroup:
 *
 * All the fields in the #GDataYouTubeGroup structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataMediaGroup parent;
	GDataYouTubeGroupPrivate *priv;
} GDataYouTubeGroup;

/**
 * GDataYouTubeGroupClass:
 *
 * All the fields in the #GDataYouTubeGroupClass structure are private and should never be accessed directly.
 *
 * Since: 0.4.0
 **/
typedef struct {
	/*< private >*/
	GDataMediaGroupClass parent;
} GDataYouTubeGroupClass;

GType gdata_youtube_group_get_type (void) G_GNUC_CONST;

guint gdata_youtube_group_get_duration (GDataYouTubeGroup *self);
gboolean gdata_youtube_group_is_private (GDataYouTubeGroup *self);
void gdata_youtube_group_set_is_private (GDataYouTubeGroup *self, gboolean is_private);
void gdata_youtube_group_get_uploaded (GDataYouTubeGroup *self, GTimeVal *uploaded);
const gchar *gdata_youtube_group_get_video_id (GDataYouTubeGroup *self);

G_END_DECLS

#endif /* !GDATA_YOUTUBE_GROUP_H */
