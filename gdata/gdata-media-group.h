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

#ifndef GDATA_MEDIA_GROUP_H
#define GDATA_MEDIA_GROUP_H

#include <glib.h>
#include <glib-object.h>

#include "gdata-media-rss.h"

G_BEGIN_DECLS

#define GDATA_TYPE_MEDIA_GROUP		(gdata_media_group_get_type ())
#define GDATA_MEDIA_GROUP(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_MEDIA_GROUP, GDataMediaGroup))
#define GDATA_MEDIA_GROUP_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_MEDIA_GROUP, GDataMediaGroupClass))
#define GDATA_IS_MEDIA_GROUP(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_MEDIA_GROUP))
#define GDATA_IS_MEDIA_GROUP_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_MEDIA_GROUP))
#define GDATA_MEDIA_GROUP_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_MEDIA_GROUP, GDataMediaGroupClass))

typedef struct _GDataMediaGroupPrivate	GDataMediaGroupPrivate;

typedef struct {
	GObject parent;
	GDataMediaGroupPrivate *priv;
} GDataMediaGroup;

typedef struct {
	GObjectClass parent;
} GDataMediaGroupClass;

GType gdata_media_group_get_type (void);

GDataMediaGroup *gdata_media_group_new (void);

const gchar *gdata_media_group_get_keywords (GDataMediaGroup *self);
void gdata_media_group_set_keywords (GDataMediaGroup *self, const gchar *keywords);
const gchar *gdata_media_group_get_player_uri (GDataMediaGroup *self);
void gdata_media_group_set_player_uri (GDataMediaGroup *self, const gchar *player_uri);
GDataMediaRating *gdata_media_group_get_rating (GDataMediaGroup *self);
void gdata_media_group_set_rating (GDataMediaGroup *self, GDataMediaRating *rating);
GDataMediaRestriction *gdata_media_group_get_restriction (GDataMediaGroup *self);
void gdata_media_group_set_restriction (GDataMediaGroup *self, GDataMediaRestriction *restriction);
const gchar *gdata_media_group_get_title (GDataMediaGroup *self);
void gdata_media_group_set_title (GDataMediaGroup *self, const gchar *title);
GDataMediaCategory *gdata_media_group_get_category (GDataMediaGroup *self);
void gdata_media_group_set_category (GDataMediaGroup *self, GDataMediaCategory *category);
GDataMediaCredit *gdata_media_group_get_credit (GDataMediaGroup *self);
void gdata_media_group_set_credit (GDataMediaGroup *self, GDataMediaCredit *credit);
const gchar *gdata_media_group_get_description (GDataMediaGroup *self);
void gdata_media_group_set_description (GDataMediaGroup *self, const gchar *description);
void gdata_media_group_add_content (GDataMediaGroup *self, GDataMediaContent *content);
void gdata_media_group_add_thumbnail (GDataMediaGroup *self, GDataMediaThumbnail *thumbnail);

G_END_DECLS

#endif /* !GDATA_MEDIA_GROUP_H */
