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

/**
 * SECTION:gdata-gdata
 * @short_description: GData namespace API
 * @stability: Unstable
 * @include: gdata/gdata-gdata.h
 *
 * The structures here represent several core GData-specific elements used by various different GData-based services, from the "gdata" namespace.
 *
 * For more information on the common GData elements, see the <ulink type="http" url="http://code.google.com/apis/gdata/elements.html#gdReference">
 * GData specification</ulink>.
 **/

#include "gdata-gdata.h"

/**
 * gdata_gd_rating_new:
 * @min: the minimum rating which can be chosen (typically %1)
 * @max: the maximum rating which can be chosen (typically %5)
 * @num_raters: the number of people who have rated the item
 * @average: the average rating for the item
 *
 * Creates a new #GDataGDRating. More information is available in the <ulink type="http"
 * url="http://code.google.com/apis/gdata/elements.html#gdRating">GData specification</ulink>.
 *
 * Currently, rel and value functionality is not implemented in #GDataGDRating.
 *
 * Return value: a new #GDataGDRating
 **/
GDataGDRating *
gdata_gd_rating_new (guint min, guint max, guint num_raters, gdouble average)
{
	GDataGDRating *self = g_slice_new (GDataGDRating);
	self->min = min;
	self->max = max;
	self->num_raters = num_raters;
	self->average = average;
	return self;
}

/**
 * gdata_gd_rating_free:
 * @self: a #GDataGDRating
 *
 * Frees a #GDataGDRating.
 **/
void
gdata_gd_rating_free (GDataGDRating *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_slice_free (GDataGDRating, self);
}

/**
 * gdata_gd_feed_link_new:
 * @href: the URI of the related feed
 * @rel: the relationship between the related feed and the current item, or %NULL
 * @count_hint: a hint about the number of items in the related feed
 *
 * Creates a new #GDataGDFeedLink. More information is available in the <ulink type="http"
 * url="http://code.google.com/apis/gdata/elements.html#gdFeedLink">GData specification</ulink>.
 *
 * Currently, readOnly and feed functionality is not implemented in #GDataGDFeedLink.
 *
 * Return value: a new #GDataGDFeedLink, or %NULL on error
 **/
GDataGDFeedLink *
gdata_gd_feed_link_new (const gchar *href, const gchar *rel, guint count_hint)
{
	GDataGDFeedLink *self;

	g_return_val_if_fail (href != NULL, NULL);

	self = g_slice_new (GDataGDFeedLink);
	self->href = g_strdup (href);
	self->rel = g_strdup (rel);
	self->count_hint = count_hint;
	return self;
}

/**
 * gdata_gd_feed_link_free:
 * @self: a #GDataGDFeedLink
 *
 * Frees a #GDataGDFeedLink.
 **/
void
gdata_gd_feed_link_free (GDataGDFeedLink *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->href);
	g_free (self->rel);
	g_slice_free (GDataGDFeedLink, self);
}

/**
 * gdata_gd_who_new:
 * @rel: the relationship between the item and this person, or %NULL
 * @value_string: a string to represent the person, or %NULL
 * @email: the person's e-mail address, or %NULL
 *
 * Creates a new #GDataGDWho. More information is available in the <ulink type="http"
 * url="http://code.google.com/apis/gdata/elements.html#gdWho">GData specification</ulink>.
 *
 * Currently, entryLink functionality is not implemented in #GDataGDWho.
 *
 * Return value: a new #GDataGDWho
 **/
GDataGDWho *
gdata_gd_who_new (const gchar *rel, const gchar *value_string, const gchar *email)
{
	GDataGDWho *self = g_slice_new (GDataGDWho);
	self->rel = g_strdup (rel);
	self->email = g_strdup (email);
	self->value_string = g_strdup (value_string);
	return self;
}

/**
 * gdata_gd_who_free:
 * @self: a #GDataGDWho
 *
 * Frees a #GDataGDWho.
 **/
void
gdata_gd_who_free (GDataGDWho *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->rel);
	g_free (self->email);
	g_free (self->value_string);
	g_slice_free (GDataGDWho, self);
}

/**
 * gdata_gd_where_new:
 * @rel: the relationship between the item and this place, or %NULL
 * @value_string: a string to represent the place, or %NULL
 * @label: a human-readable label for the place, or %NULL
 *
 * Creates a new #GDataGDWhere. More information is available in the <ulink type="http"
 * url="http://code.google.com/apis/gdata/elements.html#gdWhere">GData specification</ulink>.
 *
 * Currently, entryLink functionality is not implemented in #GDataGDWhere.
 *
 * Return value: a new #GDataGDWhere
 **/
GDataGDWhere *
gdata_gd_where_new (const gchar *rel, const gchar *value_string, const gchar *label)
{
	GDataGDWhere *self = g_slice_new (GDataGDWhere);
	self->rel = g_strdup (rel);
	self->label = g_strdup (label);
	self->value_string = g_strdup (value_string);
	return self;
}

/**
 * gdata_gd_where_free:
 * @self: a #GDataGDWhere
 *
 * Frees a #GDataGDWhere.
 **/
void
gdata_gd_where_free (GDataGDWhere *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->rel);
	g_free (self->label);
	g_free (self->value_string);
	g_slice_free (GDataGDWhere, self);
}
