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
 * SECTION:gdata-atom
 * @short_description: Atom namespace API
 * @stability: Unstable
 * @include: gdata/gdata-atom.h
 *
 * The structures here represent several core elements in the
 * <ulink type="http" url="http://www.atomenabled.org/developers/syndication/atom-format-spec.php">Atom specification</ulink> which are used in
 * GData APIs, as the GData API is based on Atom.
 **/

#include "gdata-atom.h"

/**
 * gdata_category_new:
 * @term: a category identifier
 * @scheme: an IRI to define the categorisation scheme, or %NULL
 * @label: a human-readable label for the category, or %NULL
 *
 * Creates a new #GDataCategory. More information is available in the <ulink type="http"
 * url="http://www.atomenabled.org/developers/syndication/atom-format-spec.php#element.category">Atom specification</ulink>.
 *
 * All the parameters are copied, so can be freed after calling this function.
 *
 * Return value: a new #GDataCategory, or %NULL on error
 **/
GDataCategory *
gdata_category_new (const gchar *term, const gchar *scheme, const gchar *label)
{
	GDataCategory *self;

	g_return_val_if_fail (term != NULL, NULL);

	self = g_slice_new (GDataCategory);
	self->term = g_strdup (term);
	self->scheme = g_strdup (scheme);
	self->label = g_strdup (label);
	return self;
}

/**
 * gdata_category_free
 * @self: a #GDataCategory
 *
 * Frees a #GDataCategory and its fields.
 **/
void
gdata_category_free (GDataCategory *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->term);
	g_free (self->scheme);
	g_free (self->label);
	g_slice_free (GDataCategory, self);
}

/**
 * gdata_link_new:
 * @href: the link's IRI
 * @rel: the relationship of the link to the current document, or %NULL
 * @type: an advisory media type for the @href page, or %NULL
 * @hreflang: the language of the @href page, or %NULL
 * @title: human-readable information about the link, or %NULL
 * @length: an advisory content length, in octets, for the @href page, or %-1
 *
 * Creates a new #GDataLink. More information is available in the <ulink type="http"
 * url="http://www.atomenabled.org/developers/syndication/atom-format-spec.php#element.link">Atom specification</ulink>.
 *
 * All the parameters are copied, so can be freed after calling this function.
 *
 * Return value: a new #GDataLink, or %NULL on error
 **/
GDataLink *
gdata_link_new (const gchar *href, const gchar *rel, const gchar *type, const gchar *hreflang, const gchar *title, gint length)
{
	GDataLink *self;

	g_return_val_if_fail (href != NULL, NULL);
	if (rel == NULL)
		rel = "alternate";

	self = g_slice_new (GDataLink);
	self->href = g_strdup (href);
	self->rel = g_strdup (rel);
	self->type = g_strdup (type);
	self->hreflang = g_strdup (hreflang);
	self->title = g_strdup (title);
	self->length = length;
	return self;
}

/**
 * gdata_link_free
 * @self: a #GDataLink
 *
 * Frees a #GDataLink and its fields.
 **/
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

/**
 * gdata_author_new:
 * @name: the author's name
 * @uri: an IRI describing the author, or %NULL
 * @email: the author's e-mail address, or %NULL
 *
 * Creates a new #GDataAuthor. More information is available in the <ulink type="http"
 * url="http://www.atomenabled.org/developers/syndication/atom-format-spec.php#element.author">Atom specification</ulink>.
 *
 * All the parameters are copied, so can be freed after calling this function.
 *
 * Return value: a new #GDataAuthor, or %NULL on error
 **/
GDataAuthor *
gdata_author_new (const gchar *name, const gchar *uri, const gchar *email)
{
	GDataAuthor *self;

	g_return_val_if_fail (name != NULL, NULL);

	self = g_slice_new (GDataAuthor);
	self->name = g_strdup (name);
	self->uri = g_strdup (uri);
	self->email = g_strdup (email);
	return self;
}

/**
 * gdata_author_free
 * @self: a #GDataAuthor
 *
 * Frees a #GDataAuthor and its fields.
 **/
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

/**
 * gdata_generator_new:
 * @name: the human-readable generator's name
 * @uri: an IRI describing the generator, or %NULL
 * @version: the version of the generator, or %NULL
 *
 * Creates a new #GDataGenerator. More information is available in the <ulink type="http"
 * url="http://www.atomenabled.org/developers/syndication/atom-format-spec.php#element.generator">Atom specification</ulink>.
 *
 * All the parameters are copied, so can be freed after calling this function.
 *
 * Return value: a new #GDataGenerator, or %NULL on error
 **/
GDataGenerator *
gdata_generator_new (const gchar *name, const gchar *uri, const gchar *version)
{
	GDataGenerator *self;

	g_return_val_if_fail (name != NULL, NULL);

	self = g_slice_new (GDataGenerator);
	self->name = g_strdup (name);
	self->uri = g_strdup (uri);
	self->version = g_strdup (version);
	return self;
}

/**
 * gdata_generator_free
 * @self: a #GDataGenerator
 *
 * Frees a #GDataGenerator and its fields.
 **/
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
