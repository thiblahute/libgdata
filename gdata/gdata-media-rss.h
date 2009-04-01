/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Philip Withnall 2009 <philip@tecnocode.co.uk>
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

#include <glib.h>

#ifndef GDATA_MEDIA_RSS_H
#define GDATA_MEDIA_RSS_H

G_BEGIN_DECLS

typedef struct {
	gchar *scheme;
	gchar *country;
} GDataMediaRating;

GDataMediaRating *gdata_media_rating_new (const gchar *scheme, const gchar *country);
void gdata_media_rating_free (GDataMediaRating *self);

typedef struct {
	gboolean relationship;
	gchar *countries;
} GDataMediaRestriction;

GDataMediaRestriction *gdata_media_restriction_new (const gchar *countries, gboolean relationship);
void gdata_media_restriction_free (GDataMediaRestriction *self);

typedef struct {
	gchar *label;
	gchar *scheme;
	gchar *category;
} GDataMediaCategory;

GDataMediaCategory *gdata_media_category_new (const gchar *category, const gchar *label, const gchar *scheme);
void gdata_media_category_free (GDataMediaCategory *self);

typedef struct {
	gchar *credit;
	gboolean partner;
} GDataMediaCredit;

GDataMediaCredit *gdata_media_credit_new (const gchar *credit, gboolean partner);
void gdata_media_credit_free (GDataMediaCredit *self);

typedef enum {
	GDATA_MEDIA_EXPRESSION_SAMPLE,
	GDATA_MEDIA_EXPRESSION_FULL,
	GDATA_MEDIA_EXPRESSION_NONSTOP
} GDataMediaExpression;

typedef struct {
	gchar *uri;
	gchar *type;
	gboolean is_default;
	GDataMediaExpression expression;
	gint duration; /* TODO: Should be guint? */
	gint format; /* TODO: YouTube-specific */
} GDataMediaContent;

GDataMediaContent *gdata_media_content_new (const gchar *uri, const gchar *type, gboolean is_default, GDataMediaExpression expression, gint duration, gint format);
void gdata_media_content_free (GDataMediaContent *self);

typedef struct {
	gchar *uri;
	guint width;
	guint height;
	gint64 time;
} GDataMediaThumbnail;

GDataMediaThumbnail *gdata_media_thumbnail_new (const gchar *uri, guint width, guint height, gint64 time);
gint64 gdata_media_thumbnail_parse_time (const gchar *time_string);
gchar *gdata_media_thumbnail_build_time (gint64 time);
void gdata_media_thumbnail_free (GDataMediaThumbnail *self);

G_END_DECLS

#endif /* !GDATA_MEDIA_RSS_H */
