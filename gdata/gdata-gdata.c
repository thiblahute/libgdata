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

#include "gdata-gdata.h"

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

void
gdata_gd_rating_free (GDataGDRating *self)
{
	g_slice_free (GDataGDRating, self);
}
