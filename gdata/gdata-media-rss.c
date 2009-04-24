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

/**
 * SECTION:gdata-media
 * @short_description: Media RSS namespace API
 * @stability: Unstable
 * @include: gdata/gdata-media-rss.h
 *
 * The structures here represent several core elements in the
 * <ulink type="http" url="http://search.yahoo.com/mrss/">Media RSS specification</ulink> which are used
 * in some media-related GData APIs, in particular the YouTube API.
 **/

#include <stdlib.h>
#include <string.h>

#include "gdata-media-rss.h"

/**
 * gdata_media_rating_new:
 * @country: a comma-delimited list of ISO 3166 country codes where the content is restricted
 * @scheme: a URI identifying the rating scheme, or %NULL
 *
 * Creates a new #GDataMediaRating. More information is available in the <ulink type="http"
 * url="http://search.yahoo.com/mrss/">Media RSS specification</ulink>.
 *
 * Return value: a new #GDataMediaRating, or %NULL; free with gdata_media_rating_free()
 **/
GDataMediaRating *
gdata_media_rating_new (const gchar *scheme, const gchar *country)
{
	GDataMediaRating *self;

	g_return_val_if_fail (country != NULL, NULL);

	self = g_slice_new (GDataMediaRating);
	self->scheme = g_strdup (scheme);
	self->country = g_strdup (country);
	return self;
}

/**
 * gdata_media_rating_free:
 * @self: a #GDataMediaRating
 *
 * Frees a #GDataMediaRating.
 **/
void
gdata_media_rating_free (GDataMediaRating *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->scheme);
	g_free (self->country);
	g_slice_free (GDataMediaRating, self);
}

/**
 * gdata_media_restriction_new:
 * @countries: a space-delimited list of ISO 3166 country codes
 * @relationship: %TRUE if the given @countries are not restricted regarding the content, %FALSE otherwise
 *
 * Creates a new #GDataMediaRestriction. More information is available in the <ulink type="http"
 * url="http://search.yahoo.com/mrss/">Media RSS specification</ulink>.
 *
 * Return value: a new #GDataMediaRestriction, or %NULL; free with gdata_media_restriction_free()
 **/
GDataMediaRestriction *
gdata_media_restriction_new (const gchar *countries, gboolean relationship)
{
	GDataMediaRestriction *self;

	g_return_val_if_fail (countries != NULL, NULL);

	self = g_slice_new (GDataMediaRestriction);
	self->countries = g_strdup (countries);
	self->relationship = relationship;
	return self;
}

/**
 * gdata_media_restriction_free:
 * @self: a #GDataMediaRestriction
 *
 * Frees a #GDataMediaRestriction.
 **/
void
gdata_media_restriction_free (GDataMediaRestriction *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->countries);
	g_slice_free (GDataMediaRestriction, self);
}

/**
 * gdata_media_category_new:
 * @category: a category describing the content
 * @scheme: a URI identifying the categorisation scheme, or %NULL
 * @label: a human-readable name for the category, or %NULL
 *
 * Creates a new #GDataMediaCategory. More information is available in the <ulink type="http"
 * url="http://search.yahoo.com/mrss/">Media RSS specification</ulink>.
 *
 * Return value: a new #GDataMediaCategory, or %NULL; free with gdata_media_category_free()
 **/
GDataMediaCategory *
gdata_media_category_new (const gchar *category, const gchar *label, const gchar *scheme)
{
	GDataMediaCategory *self;

	g_return_val_if_fail (category != NULL, NULL);

	self = g_slice_new (GDataMediaCategory);
	self->category = g_strdup (category);
	self->label = g_strdup (label);
	self->scheme = g_strdup (scheme);
	return self;
}

/**
 * gdata_media_category_free:
 * @self: a #GDataMediaCategory
 *
 * Frees a #GDataMediaCategory.
 **/
void
gdata_media_category_free (GDataMediaCategory *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->category);
	g_free (self->label);
	g_free (self->scheme);
	g_slice_free (GDataMediaCategory, self);
}

/**
 * gdata_media_credit_new:
 * @credit: the username of someone who contributed towards the media
 * @partner: %TRUE if the video was uploaded by a YouTube partner, %FALSE otherwise
 *
 * Creates a new #GDataMediaCredit. More information is available in the <ulink type="http"
 * url="http://search.yahoo.com/mrss/">Media RSS specification</ulink>.
 *
 * Return value: a new #GDataMediaCredit, or %NULL; free with gdata_media_credit_free()
 **/
GDataMediaCredit *
gdata_media_credit_new (const gchar *credit, gboolean partner)
{
	GDataMediaCredit *self;

	g_return_val_if_fail (credit != NULL, NULL);

	self = g_slice_new (GDataMediaCredit);
	self->credit = g_strdup (credit);
	self->partner = partner;
	return self;
}

/**
 * gdata_media_credit_free:
 * @self: a #GDataMediaCredit
 *
 * Frees a #GDataMediaCredit.
 **/
void
gdata_media_credit_free (GDataMediaCredit *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->credit);
	g_slice_free (GDataMediaCredit, self);
}

/**
 * gdata_media_content_new:
 * @uri: the media object's URI
 * @type: the media object's MIME type, or %NULL
 * @is_default: %TRUE if this is the default object for the media group, %FALSE otherwise
 * @expression: whether this media is a full version or just a sample
 * @duration: the length of the media, in seconds, or %-1
 * @format: the video format of the media (see the <ulink type="http"
 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_media:content">YouTube specification</ulink>)
 *
 * Creates a new #GDataMediaContent. More information is available in the <ulink type="http"
 * url="http://search.yahoo.com/mrss/">Media RSS specification</ulink>.
 *
 * Return value: a new #GDataMediaContent, or %NULL; free with gdata_media_content_free()
 **/
GDataMediaContent *
gdata_media_content_new (const gchar *uri, const gchar *type, gboolean is_default, GDataMediaExpression expression, gint duration, gint format)
{
	GDataMediaContent *self;

	g_return_val_if_fail (uri != NULL, NULL);

	self = g_slice_new (GDataMediaContent);
	self->uri = g_strdup (uri);
	self->type = g_strdup (type);
	self->is_default = is_default;
	self->expression = expression;
	self->duration = duration;
	self->format = format;
	return self;
}

/**
 * gdata_media_content_free:
 * @self: a #GDataMediaContent
 *
 * Frees a #GDataMediaContent.
 **/
void
gdata_media_content_free (GDataMediaContent *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->uri);
	g_free (self->type);
	g_slice_free (GDataMediaContent, self);
}

/**
 * gdata_media_thumbnail_new:
 * @uri: the thumbnail's URI
 * @width: the thumbnail's width, in pixels, or %0
 * @height: the thumbnail's height, in pixels, or %0
 * @_time: the number of milliseconds into the media the thumbnail was taken, or %0
 *
 * Creates a new #GDataMediaThumbnail. More information is available in the <ulink type="http"
 * url="http://search.yahoo.com/mrss/">Media RSS specification</ulink>.
 *
 * Return value: a new #GDataMediaThumbnail, or %NULL; free with gdata_media_thumbnail_free()
 **/
GDataMediaThumbnail *
gdata_media_thumbnail_new (const gchar *uri, guint width, guint height, gint64 _time)
{
	GDataMediaThumbnail *self;

	g_return_val_if_fail (uri != NULL, NULL);

	self = g_slice_new (GDataMediaThumbnail);
	self->uri = g_strdup (uri);
	self->width = width;
	self->height = height;
	self->time = _time;
	return self;
}

/**
 * gdata_media_thumbnail_parse_time:
 * @time_string: a time string to parse
 *
 * Parses a time string in NTP format into a number of milliseconds since the
 * start of a media stream.
 *
 * For more information about NTP format, see <ulink type="http" url="http://www.ietf.org/rfc/rfc2326.txt">RFC 2326 3.6 Normal Play Time</ulink>.
 *
 * To build an NTP-format string, see gdata_media_thumbnail_build_time().
 *
 * Return value: number of milliseconds since the start of a media stream
 **/
gint64
gdata_media_thumbnail_parse_time (const gchar *time_string)
{
	guint hours, minutes;
	gfloat seconds;
	gchar *end_pointer;

	g_return_val_if_fail (time_string != NULL, 0);

	hours = strtoul (time_string, &end_pointer, 10);
	if (end_pointer != time_string + 2)
		return -1;

	minutes = strtoul (time_string + 3, &end_pointer, 10);
	if (end_pointer != time_string + 5)
		return -1;

	seconds = strtod (time_string + 6, &end_pointer);
	if (end_pointer != time_string + strlen (time_string))
		return -1;

	return (gint64) ((seconds + minutes * 60 + hours * 3600) * 1000);
}

/**
 * gdata_media_thumbnail_build_time:
 * @_time: a number of milliseconds since the start of a media stream
 *
 * Builds an NTP-format time string describing @_time milliseconds since the start
 * of a media stream.
 *
 * Return value: an NTP-format string describing @_time; free with g_free()
 **/
gchar *
gdata_media_thumbnail_build_time (gint64 _time)
{
	guint hours, minutes;
	gfloat seconds;

	hours = _time % 3600000;
	_time -= hours * 3600000;

	minutes = _time % 60000;
	_time -= minutes * 60000;

	seconds = _time / 1000.0;

	return g_strdup_printf ("%02u:%02u:%02f", hours, minutes, seconds);
}

/**
 * gdata_media_thumbnail_free:
 * @self: a #GDataMediaThumbnail
 *
 * Frees a #GDataMediaThumbnail.
 **/
void
gdata_media_thumbnail_free (GDataMediaThumbnail *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->uri);
	g_slice_free (GDataMediaThumbnail, self);
}
