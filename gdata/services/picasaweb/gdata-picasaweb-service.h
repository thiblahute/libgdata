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

#ifndef GDATA_PICASAWEB_SERVICE_H
#define GDATA_PICASAWEB_SERVICE_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-service.h>
#include <gdata/services/picasaweb/gdata-picasaweb-album.h>

G_BEGIN_DECLS

#define GDATA_TYPE_PICASAWEB_SERVICE		(gdata_picasaweb_service_get_type ())
#define GDATA_PICASAWEB_SERVICE(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_PICASAWEB_SERVICE, GDataPicasaWebService))
#define GDATA_PICASAWEB_SERVICE_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_PICASAWEB_SERVICE, GDataPicasaWebServiceClass))
#define GDATA_IS_PICASAWEB_SERVICE(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_PICASAWEB_SERVICE))
#define GDATA_IS_PICASAWEB_SERVICE_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_PICASAWEB_SERVICE))
#define GDATA_PICASAWEB_SERVICE_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_PICASAWEB_SERVICE, GDataPicasaWebServiceClass))

/**
 * GDataPicasaWebService:
 *
 * All the fields in the #GDataPicasaWebService structure are private and should never be accessed directly.
 *
 * Since: 0.4.0
 **/
typedef struct {
	GDataService parent;
} GDataPicasaWebService;

/**
 * GDataPicasaWebServiceClass:
 *
 * All the fields in the #GDataPicasaWebServiceClass structure are private and should never be accessed directly.
 *
 * Since: 0.4.0
 **/
typedef struct {
	/*< private >*/
	GDataServiceClass parent;
} GDataPicasaWebServiceClass;

GType gdata_picasaweb_service_get_type (void) G_GNUC_CONST;

GDataPicasaWebService *gdata_picasaweb_service_new (const gchar *client_id) G_GNUC_WARN_UNUSED_RESULT;

#include <gdata/services/picasaweb/gdata-picasaweb-query.h>

GDataFeed *gdata_picasaweb_service_query_all_albums (GDataPicasaWebService *self, GDataQuery *query, const gchar *username,
						     GCancellable *cancellable, GDataQueryProgressCallback progress_callback, gpointer progress_user_data,
						     GError **error) G_GNUC_WARN_UNUSED_RESULT;
void gdata_picasaweb_service_query_all_albums_async (GDataPicasaWebService *self, GDataQuery *query, const gchar *username,
						     GCancellable *cancellable, GDataQueryProgressCallback progress_callback, gpointer progress_user_data,
						     GAsyncReadyCallback callback, gpointer user_data);

GDataFeed *gdata_picasaweb_service_query_files (GDataPicasaWebService *self, GDataPicasaWebAlbum *album, GDataQuery *query,
						GCancellable *cancellable, GDataQueryProgressCallback progress_callback, gpointer progress_user_data,
						GError **error) G_GNUC_WARN_UNUSED_RESULT;

#include <gdata/services/picasaweb/gdata-picasaweb-file.h>

GDataPicasaWebFile *gdata_picasaweb_service_upload_file (GDataPicasaWebService *self, GDataPicasaWebAlbum *album, GDataPicasaWebFile *file,
							 GFile *actual_file, GCancellable *cancellable, GError **error) G_GNUC_WARN_UNUSED_RESULT;
/* TODO: async version */

G_END_DECLS

#endif /* !GDATA_PICASAWEB_SERVICE_H */
