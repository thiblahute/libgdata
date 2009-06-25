/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Richard Schwarting 2009 <aquarichy@gmail.com>
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

#ifndef GDATA_PICASAWEB_QUERY_H
#define GDATA_PICASAWEB_QUERY_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-query.h>
#include <gdata/services/picasaweb/gdata-picasaweb-album.h>

G_BEGIN_DECLS

#define GDATA_TYPE_PICASAWEB_QUERY		(gdata_picasaweb_query_get_type ())
#define GDATA_PICASAWEB_QUERY(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_PICASAWEB_QUERY, GDataPicasaWebQuery))
#define GDATA_PICASAWEB_QUERY_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_PICASAWEB_QUERY, GDataPicasawebQueryClass))
#define GDATA_IS_PICASAWEB_QUERY(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_PICASAWEB_QUERY))
#define GDATA_IS_PICASAWEB_QUERY_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_PICASAWEB_QUERY))
#define GDATA_PICASAWEB_QUERY_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_PICASAWEB_QUERY, GDataPicasawebQueryClass))

typedef struct _GDataPicasaWebQueryPrivate	GDataPicasaWebQueryPrivate;

/**
 * GDataPicasaWebQuery:
 *
 * All the fields in the #GDataPicasaWebQuery structure are private and should never be accessed directly.
 *
 * Since: 0.4.0
 **/
typedef struct {
	GDataQuery parent;
	GDataPicasaWebQueryPrivate *priv;
} GDataPicasaWebQuery;

/**
 * GDataPicasaWebQueryClass:
 *
 * All the fields in the #GDataPicasaWebQueryClass structure are private and should never be accessed directly.
 *
 * Since: 0.4.0
 **/
typedef struct {
	/*< private >*/
	GDataQueryClass parent;
} GDataPicasaWebQueryClass;

GType gdata_picasaweb_query_get_type (void) G_GNUC_CONST;

GDataPicasaWebQuery *gdata_picasaweb_query_new (const gchar *q) G_GNUC_WARN_UNUSED_RESULT;

GDataPicasaWebVisibility gdata_picasaweb_query_get_visibility (GDataPicasaWebQuery *self);
void gdata_picasaweb_query_set_visibility (GDataPicasaWebQuery *self, GDataPicasaWebVisibility visibility);
const gchar *gdata_picasaweb_query_get_thumbnail_size (GDataPicasaWebQuery *self);
void gdata_picasaweb_query_set_thumbnail_size (GDataPicasaWebQuery *self, const gchar *thumbnail_size);
const gchar *gdata_picasaweb_query_get_image_size (GDataPicasaWebQuery *self);
void gdata_picasaweb_query_set_image_size (GDataPicasaWebQuery *self, const gchar *image_size);
const gchar *gdata_picasaweb_query_get_tag (GDataPicasaWebQuery *self);
void gdata_picasaweb_query_set_tag (GDataPicasaWebQuery *self, const gchar *tag);
void gdata_picasaweb_query_get_bounding_box (GDataPicasaWebQuery *self, gdouble *north, gdouble *east, gdouble *south, gdouble *west);
void gdata_picasaweb_query_set_bounding_box (GDataPicasaWebQuery *self, gdouble north, gdouble east, gdouble south, gdouble west);
const gchar *gdata_picasaweb_query_get_location (GDataPicasaWebQuery *self);
void gdata_picasaweb_query_set_location (GDataPicasaWebQuery *self, const gchar *location);

G_END_DECLS

#endif /* !GDATA_PICASAWEB_QUERY_H */
