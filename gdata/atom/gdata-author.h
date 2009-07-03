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

#ifndef GDATA_AUTHOR_H
#define GDATA_AUTHOR_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-parsable.h>

G_BEGIN_DECLS

#define GDATA_TYPE_AUTHOR		(gdata_author_get_type ())
#define GDATA_AUTHOR(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_AUTHOR, GDataAuthor))
#define GDATA_AUTHOR_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_AUTHOR, GDataAuthorClass))
#define GDATA_IS_AUTHOR(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_AUTHOR))
#define GDATA_IS_AUTHOR_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_AUTHOR))
#define GDATA_AUTHOR_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_AUTHOR, GDataAuthorClass))

typedef struct _GDataAuthorPrivate	GDataAuthorPrivate;

/**
 * GDataAuthor:
 *
 * All the fields in the #GDataAuthor structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataParsable parent;
	GDataAuthorPrivate *priv;
} GDataAuthor;

/**
 * GDataAuthorClass:
 *
 * All the fields in the #GDataAuthorClass structure are private and should never be accessed directly.
 *
 * Since: 0.4.0
 **/
typedef struct {
	/*< private >*/
	GDataParsableClass parent;
} GDataAuthorClass;

GType gdata_author_get_type (void) G_GNUC_CONST;

GDataAuthor *gdata_author_new (const gchar *name, const gchar *uri, const gchar *email_address) G_GNUC_WARN_UNUSED_RESULT;
gint gdata_author_compare (const GDataAuthor *a, const GDataAuthor *b);

const gchar *gdata_author_get_name (GDataAuthor *self);
void gdata_author_set_name (GDataAuthor *self, const gchar *name);

const gchar *gdata_author_get_uri (GDataAuthor *self);
void gdata_author_set_uri (GDataAuthor *self, const gchar *uri);

const gchar *gdata_author_get_email_address (GDataAuthor *self);
void gdata_author_set_email_address (GDataAuthor *self, const gchar *email_address);

G_END_DECLS

#endif /* !GDATA_AUTHOR_H */
