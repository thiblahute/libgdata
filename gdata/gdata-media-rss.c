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

#include <stdlib.h>
#include <string.h>

#include "gdata-media-rss.h"

GDataMediaRating *
gdata_media_rating_new (const gchar *scheme, const gchar *country)
{
	GDataMediaRating *self = g_slice_new (GDataMediaRating);
	self->scheme = g_strdup (scheme);
	self->country = g_strdup (country);
	return self;
}

void
gdata_media_rating_free (GDataMediaRating *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->scheme);
	g_free (self->country);
	g_slice_free (GDataMediaRating, self);
}

GDataMediaRestriction *
gdata_media_restriction_new (const gchar *countries, gboolean relationship)
{
	GDataMediaRestriction *self = g_slice_new (GDataMediaRestriction);
	self->countries = g_strdup (countries);
	self->relationship = relationship;
	return self;
}

void
gdata_media_restriction_free (GDataMediaRestriction *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->countries);
	g_slice_free (GDataMediaRestriction, self);
}

GDataMediaCategory *
gdata_media_category_new (const gchar *category, const gchar *label, const gchar *scheme)
{
	GDataMediaCategory *self = g_slice_new (GDataMediaCategory);
	self->category = g_strdup (category);
	self->label = g_strdup (label);
	self->scheme = g_strdup (scheme);
	return self;
}

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

GDataMediaCredit *
gdata_media_credit_new (const gchar *credit, gboolean partner)
{
	GDataMediaCredit *self = g_slice_new (GDataMediaCredit);
	self->credit = g_strdup (credit);
	self->partner = partner;
	return self;
}

void
gdata_media_credit_free (GDataMediaCredit *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->credit);
	g_slice_free (GDataMediaCredit, self);
}

GDataMediaContent *
gdata_media_content_new (const gchar *uri, const gchar *type, gboolean is_default, GDataMediaExpression expression, gint duration, gint format)
{
	GDataMediaContent *self = g_slice_new (GDataMediaContent);
	self->uri = g_strdup (uri);
	self->type = g_strdup (type);
	self->is_default = is_default;
	self->expression = expression;
	self->duration = duration;
	self->format = format;
	return self;
}

void
gdata_media_content_free (GDataMediaContent *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->uri);
	g_free (self->type);
	g_slice_free (GDataMediaContent, self);
}

GDataMediaThumbnail *
gdata_media_thumbnail_new (const gchar *uri, guint width, guint height, gint64 time)
{
	GDataMediaThumbnail *self = g_slice_new (GDataMediaThumbnail);
	self->uri = g_strdup (uri);
	self->width = width;
	self->height = height;
	self->time = time;
	return self;
}

/* Parses a reduced version of NTP from http://www.ietf.org/rfc/rfc2326.txt.
 * See http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_media:thumbnail */
gint64
gdata_media_thumbnail_parse_time (const gchar *time_string)
{
	guint hours, minutes;
	gfloat seconds;
	gchar *end_pointer;

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

gchar *
gdata_media_thumbnail_build_time (gint64 time)
{
	guint hours, minutes;
	gfloat seconds;

	hours = time % 3600000;
	time -= hours * 3600000;

	minutes = time % 60000;
	time -= minutes * 60000;

	seconds = time / 1000.0;

	return g_strdup_printf ("%02u:%02u:%02f", hours, minutes, seconds);
}

void
gdata_media_thumbnail_free (GDataMediaThumbnail *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->uri);
	g_slice_free (GDataMediaThumbnail, self);
}
