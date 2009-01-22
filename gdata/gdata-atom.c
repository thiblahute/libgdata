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

#include "gdata-atom.h"

GDataCategory *
gdata_category_new (const gchar *scheme, const gchar *term, const gchar *label)
{
	GDataCategory *self = g_slice_new (GDataCategory);
	self->scheme = g_strdup (scheme);
	self->term = g_strdup (term);
	self->label = g_strdup (label);
	return self;
}

void
gdata_category_free (GDataCategory *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->scheme);
	g_free (self->term);
	g_free (self->label);
	g_slice_free (GDataCategory, self);
}

GDataLink *
gdata_link_new (const gchar *href, const gchar *rel, const gchar *type, const gchar *hreflang, const gchar *title, gint length)
{
	GDataLink *self = g_slice_new (GDataLink);
	self->href = g_strdup (href);
	self->rel = g_strdup (rel);
	self->type = g_strdup (type);
	self->hreflang = g_strdup (hreflang);
	self->title = g_strdup (title);
	self->length = length;
	return self;
}

void
gdata_link_free (GDataLink *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->href);
	g_free (self->rel);
	g_free (self->type);
	g_free (self->hreflang);
	g_free (self->title);
	g_slice_free (GDataLink, self);
}

GDataAuthor *
gdata_author_new (const gchar *name, const gchar *uri, const gchar *email)
{
	GDataAuthor *self = g_slice_new (GDataAuthor);
	self->name = g_strdup (name);
	self->uri = g_strdup (uri);
	self->email = g_strdup (email);
	return self;
}

void
gdata_author_free (GDataAuthor *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->name);
	g_free (self->uri);
	g_free (self->email);
	g_slice_free (GDataAuthor, self);
}

GDataGenerator *
gdata_generator_new (const gchar *name, const gchar *uri, const gchar *version)
{
	GDataGenerator *self = g_slice_new (GDataGenerator);
	self->name = g_strdup (name);
	self->uri = g_strdup (uri);
	self->version = g_strdup (version);
	return self;
}

void
gdata_generator_free (GDataGenerator *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->name);
	g_free (self->uri);
	g_free (self->version);
	g_slice_free (GDataGenerator, self);
}
