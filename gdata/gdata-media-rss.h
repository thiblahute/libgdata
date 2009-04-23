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

/**
 * GDataMediaRating:
 * @country: a comma-delimited list of ISO 3166 country codes where the content is restricted
 * @scheme: a URI identifying the rating scheme, or %NULL
 *
 * A structure fully representing a Media RSS "rating" element. The @country field is required,
 * but the other is optional.
 *
 * See the <literal>media:rating</literal> element in the
 * <ulink type="http" url="http://search.yahoo.com/mrss/">Media RSS specification</ulink> for more information.
 **/
typedef struct {
	gchar *country;
	gchar *scheme;
} GDataMediaRating;

/* TODO: Reverse the parameters */
GDataMediaRating *gdata_media_rating_new (const gchar *scheme, const gchar *country) G_GNUC_WARN_UNUSED_RESULT;
void gdata_media_rating_free (GDataMediaRating *self);

/**
 * GDataMediaRestriction:
 * @countries: a space-delimited list of ISO 3166 country codes
 * @relationship: %TRUE if the given @countries are not restricted regarding the content, %FALSE otherwise
 *
 * A structure fully representing a Media RSS "restriction" element. All fields are required.
 *
 * See the <literal>media:restriction</literal> element in the
 * <ulink type="http" url="http://search.yahoo.com/mrss/">Media RSS specification</ulink> for more information.
 **/
typedef struct {
	gchar *countries;
	gboolean relationship;
	/* TODO: Support uri and type */
} GDataMediaRestriction;

GDataMediaRestriction *gdata_media_restriction_new (const gchar *countries, gboolean relationship) G_GNUC_WARN_UNUSED_RESULT;
void gdata_media_restriction_free (GDataMediaRestriction *self);

/**
 * GDataMediaCategory:
 * @category: a category describing the content
 * @label: a human-readable name for the category, or %NULL
 * @scheme: a URI identifying the categorisation scheme, or %NULL
 *
 * A structure fully representing a Media RSS "category" element. The @category field is required,
 * but the others are optional.
 *
 * See the <literal>media:category</literal> element in the
 * <ulink type="http" url="http://search.yahoo.com/mrss/">Media RSS specification</ulink> for more information.
 **/
typedef struct {
	gchar *category;
	gchar *label;
	gchar *scheme;
} GDataMediaCategory;

GDataMediaCategory *gdata_media_category_new (const gchar *category, const gchar *label, const gchar *scheme) G_GNUC_WARN_UNUSED_RESULT;
void gdata_media_category_free (GDataMediaCategory *self);

/**
 * GDataMediaCredit:
 * @credit: the username of someone who contributed towards the media
 * @partner: %TRUE if the video was uploaded by a YouTube partner, %FALSE otherwise
 *
 * A structure fully representing a Media RSS "credit" element. All fields are required.
 *
 * See the <literal>media:credit</literal> element in the
 * <ulink type="http" url="http://search.yahoo.com/mrss/">Media RSS specification</ulink> for more information.
 **/
typedef struct {
	gchar *credit;
	gboolean partner;
	/* TODO: add scheme and role */
	/* TODO: partner is YouTube-specific */
} GDataMediaCredit;

GDataMediaCredit *gdata_media_credit_new (const gchar *credit, gboolean partner) G_GNUC_WARN_UNUSED_RESULT;
void gdata_media_credit_free (GDataMediaCredit *self);

/**
 * GDataMediaExpression:
 * @GDATA_MEDIA_EXPRESSION_SAMPLE: the media is a sample of a larger video
 * @GDATA_MEDIA_EXPRESSION_FULL: the media is the full version
 * @GDATA_MEDIA_EXPRESSION_NONSTOP: the media is a continuous stream
 *
 * An enum representing the possible values of #GDataMediaContent:expression.
 **/
typedef enum {
	GDATA_MEDIA_EXPRESSION_SAMPLE,
	GDATA_MEDIA_EXPRESSION_FULL,
	GDATA_MEDIA_EXPRESSION_NONSTOP
} GDataMediaExpression;

/**
 * GDataMediaContent:
 * @uri: the media object's URI
 * @type: the media object's MIME type, or %NULL
 * @is_default: %TRUE if this is the default object for the media group, %FALSE otherwise
 * @expression: whether this media is a full version or just a sample
 * @duration: the length of the media, in seconds, or %-1
 * @format: the video format of the media (see the <ulink type="http"
 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_media:content">YouTube specification</ulink>)
 *
 * A structure fully representing a Media RSS "content" element. The @uri field is required,
 * but the others are optional.
 *
 * See the <literal>media:content</literal> element in the
 * <ulink type="http" url="http://search.yahoo.com/mrss/">Media RSS specification</ulink> for more information.
 **/
typedef struct {
	gchar *uri;
	gchar *type;
	gboolean is_default;
	GDataMediaExpression expression;
	gint duration; /* TODO: Should be guint? */
	gint format; /* TODO: YouTube-specific */
	/* TODO: add support for other standard properties */
} GDataMediaContent;

GDataMediaContent *gdata_media_content_new (const gchar *uri, const gchar *type, gboolean is_default, GDataMediaExpression expression,
					    gint duration, gint format) G_GNUC_WARN_UNUSED_RESULT;
void gdata_media_content_free (GDataMediaContent *self);

/**
 * GDataMediaThumbnail:
 * @uri: the thumbnail's URI
 * @width: the thumbnail's width, in pixels, or %0
 * @height: the thumbnail's height, in pixels, or %0
 * @time: the number of milliseconds into the media the thumbnail was taken, or %0
 *
 * A structure fully representing a Media RSS "thumbnail" element. The @uri field is required,
 * but the others are optional.
 *
 * See the <literal>media:thumbnail</literal> element in the
 * <ulink type="http" url="http://search.yahoo.com/mrss/">Media RSS specification</ulink> for more information.
 **/
typedef struct {
	gchar *uri;
	guint width;
	guint height;
	gint64 time;
} GDataMediaThumbnail;

GDataMediaThumbnail *gdata_media_thumbnail_new (const gchar *uri, guint width, guint height, gint64 _time) G_GNUC_WARN_UNUSED_RESULT;
gint64 gdata_media_thumbnail_parse_time (const gchar *time_string) G_GNUC_PURE;
gchar *gdata_media_thumbnail_build_time (gint64 _time) G_GNUC_WARN_UNUSED_RESULT;
void gdata_media_thumbnail_free (GDataMediaThumbnail *self);

G_END_DECLS

#endif /* !GDATA_MEDIA_RSS_H */
