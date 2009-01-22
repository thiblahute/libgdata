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

#include <glib.h>

#ifndef GDATA_ATOM_H
#define GDATA_ATOM_H

G_BEGIN_DECLS

typedef struct {
	gchar *scheme;
	gchar *term;
	gchar *label;
} GDataCategory;

GDataCategory *gdata_category_new (const gchar *scheme, const gchar *term, const gchar *label);
void gdata_category_free (GDataCategory *self);

typedef struct {
	gchar *href;
	gchar *rel;
	gchar *type;
	gchar *hreflang;
	gchar *title;
	gint length; /* TODO: guint? */
} GDataLink;

GDataLink *gdata_link_new (const gchar *href, const gchar *rel, const gchar *type, const gchar *hreflang, const gchar *title, gint length);
void gdata_link_free (GDataLink *self);

typedef struct {
	gchar *name;
	gchar *uri;
	gchar *email;
} GDataAuthor;

GDataAuthor *gdata_author_new (const gchar *name, const gchar *uri, const gchar *email);
void gdata_author_free (GDataAuthor *self);

typedef struct {
	gchar *uri;
	gchar *version;
	gchar *name;
} GDataGenerator;

GDataGenerator *gdata_generator_new (const gchar *name, const gchar *uri, const gchar *version);
void gdata_generator_free (GDataGenerator *self);

G_END_DECLS

#endif /* !GDATA_ATOM_H */
