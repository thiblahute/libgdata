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

#ifndef GDATA_ATOM_H
#define GDATA_ATOM_H

G_BEGIN_DECLS

/**
 * GDataCategory:
 * @term: a category identifier
 * @scheme: an IRI to define the categorisation scheme, or %NULL
 * @label: a human-readable label for the category, or %NULL
 *
 * A structure fully representing an Atom "category" element. The @term field is required, but the others are optional.
 *
 * See the <ulink type="http" url="http://www.atomenabled.org/developers/syndication/atom-format-spec.php#element.category">Atom specification</ulink>
 * for more information.
 **/
typedef struct {
	gchar *term;
	gchar *scheme;
	gchar *label;
} GDataCategory;

GDataCategory *gdata_category_new (const gchar *term, const gchar *scheme, const gchar *label) G_GNUC_WARN_UNUSED_RESULT;
void gdata_category_free (GDataCategory *self);

/**
 * GDataLink:
 * @href: the link's IRI
 * @rel: the relationship of the link to the current document, or %NULL
 * @type: an advisory media type for the @href page, or %NULL
 * @hreflang: the language of the @href page, or %NULL
 * @title: human-readable information about the link, or %NULL
 * @length: an advisory content length, in octets, for the @href page, or %-1
 *
 * A structure fully representing an Atom "link" element. The @href field is required, but the others are optional.
 *
 * See the <ulink type="http" url="http://www.atomenabled.org/developers/syndication/atom-format-spec.php#element.link">Atom specification</ulink>
 * for more information.
 **/
typedef struct {
	gchar *href;
	gchar *rel;
	gchar *type;
	gchar *hreflang;
	gchar *title;
	gint length;
} GDataLink;

GDataLink *gdata_link_new (const gchar *href, const gchar *rel, const gchar *type, const gchar *hreflang,
			   const gchar *title, gint length) G_GNUC_WARN_UNUSED_RESULT;
void gdata_link_free (GDataLink *self);

/**
 * GDataAuthor:
 * @name: the author's name
 * @uri: an IRI describing the author, or %NULL
 * @email: the author's e-mail address, or %NULL
 *
 * A structure fully representing an Atom "author" element. The @name field is required, but the others are optional.
 *
 * See the <ulink type="http" url="http://www.atomenabled.org/developers/syndication/atom-format-spec.php#element.author">Atom specification</ulink>
 * for more information.
 **/
typedef struct {
	gchar *name;
	gchar *uri;
	gchar *email;
} GDataAuthor;

GDataAuthor *gdata_author_new (const gchar *name, const gchar *uri, const gchar *email) G_GNUC_WARN_UNUSED_RESULT;
void gdata_author_free (GDataAuthor *self);

/**
 * GDataGenerator:
 * @name: the human-readable generator's name
 * @uri: an IRI describing the generator, or %NULL
 * @version: the version of the generator, or %NULL
 *
 * A structure fully representing an Atom "generator" element. The @name field is required, but the others are optional.
 *
 * See the <ulink type="http" url="http://www.atomenabled.org/developers/syndication/atom-format-spec.php#element.generator">Atom specification</ulink>
 * for more information.
 **/
typedef struct {
	gchar *uri;
	gchar *version;
	gchar *name;
} GDataGenerator;

GDataGenerator *gdata_generator_new (const gchar *name, const gchar *uri, const gchar *version) G_GNUC_WARN_UNUSED_RESULT;
void gdata_generator_free (GDataGenerator *self);

G_END_DECLS

#endif /* !GDATA_ATOM_H */
