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

#ifndef GDATA_LINK_H
#define GDATA_LINK_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-parsable.h>

G_BEGIN_DECLS

#define GDATA_TYPE_LINK			(gdata_link_get_type ())
#define GDATA_LINK(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_LINK, GDataLink))
#define GDATA_LINK_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_LINK, GDataLinkClass))
#define GDATA_IS_LINK(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_LINK))
#define GDATA_IS_LINK_CLASS(k)		(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_LINK))
#define GDATA_LINK_GET_CLASS(o)		(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_LINK, GDataLinkClass))

typedef struct _GDataLinkPrivate	GDataLinkPrivate;

/**
 * GDataLink:
 *
 * All the fields in the #GDataLink structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataParsable parent;
	GDataLinkPrivate *priv;
} GDataLink;

/**
 * GDataLinkClass:
 *
 * All the fields in the #GDataLinkClass structure are private and should never be accessed directly.
 *
 * Since: 0.4.0
 **/
typedef struct {
	/*< private >*/
	GDataParsableClass parent;
} GDataLinkClass;

GType gdata_link_get_type (void) G_GNUC_CONST;

GDataLink *gdata_link_new (const gchar *uri, const gchar *relation_type) G_GNUC_WARN_UNUSED_RESULT;
gint gdata_link_compare (const GDataLink *a, const GDataLink *b);

const gchar *gdata_link_get_uri (GDataLink *self);
void gdata_link_set_uri (GDataLink *self, const gchar *uri);

const gchar *gdata_link_get_relation_type (GDataLink *self);
void gdata_link_set_relation_type (GDataLink *self, const gchar *relation_type);

const gchar *gdata_link_get_content_type (GDataLink *self);
void gdata_link_set_content_type (GDataLink *self, const gchar *content_type);

const gchar *gdata_link_get_language (GDataLink *self);
void gdata_link_set_language (GDataLink *self, const gchar *language);

const gchar *gdata_link_get_title (GDataLink *self);
void gdata_link_set_title (GDataLink *self, const gchar *title);

gint gdata_link_get_length (GDataLink *self);
void gdata_link_set_length (GDataLink *self, gint length);

G_END_DECLS

#endif /* !GDATA_LINK_H */
