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

#ifndef GDATA_PICASWEB_ALBUM_H
#define GDATA_PICASWEB_ALBUM_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-entry.h>

G_BEGIN_DECLS

/**
 * GDataPicasaWebVisibility:
 * @GDATA_PICASAWEB_PUBLIC: the album is visible to everyone, regardless of whether they're authenticated
 * @GDATA_PICASAWEB_PRIVATE: the album is visible only to authenticated users in a whitelist
 *
 * Visibility statuses available for albums on PicasaWeb. For more information, see the <ulink type="http"
 * url="http://code.google.com/apis/picasaweb/reference.html#Visibility">online documentation</ulink>.
 *
 * Since: 0.4.0
 **/
typedef enum {
	GDATA_PICASAWEB_PUBLIC = 1,
	GDATA_PICASAWEB_PRIVATE
} GDataPicasaWebVisibility;

#define GDATA_TYPE_PICASAWEB_ALBUM		(gdata_picasaweb_album_get_type ())
#define GDATA_PICASAWEB_ALBUM(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_PICASAWEB_ALBUM, GDataPicasaWebAlbum))
#define GDATA_PICASAWEB_ALBUM_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_PICASAWEB_ALBUM, GDataPicasaWebAlbumClass))
#define GDATA_IS_PICASAWEB_ALBUM(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_PICASAWEB_ALBUM))
#define GDATA_IS_PICASAWEB_ALBUM_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_PICASAWEB_ALBUM))
#define GDATA_PICASAWEB_ALBUM_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_PICASAWEB_ALBUM, GDataPicasaWebAlbumClass))

typedef struct _GDataPicasaWebAlbumPrivate	GDataPicasaWebAlbumPrivate;

/**
 * GDataPicasaWebAlbum:
 *
 * All the fields in the #GDataPicasaWebAlbum structure are private and should never be accessed directly.
 *
 * Since: 0.4.0
 **/
typedef struct {
	GDataEntry parent;
	GDataPicasaWebAlbumPrivate *priv;
} GDataPicasaWebAlbum;

/**
 * GDataPicasaWebAlbumClass:
 *
 * All the fields in the #GDataPicasaWebAlbumClass structure are private and should never be accessed directly.
 *
 * Since: 0.4.0
 **/
typedef struct {
	/*< private >*/
	GDataEntryClass parent;
} GDataPicasaWebAlbumClass;

GType gdata_picasaweb_album_get_type (void) G_GNUC_CONST;

GDataPicasaWebAlbum *gdata_picasaweb_album_new (const gchar *id) G_GNUC_WARN_UNUSED_RESULT;
GDataPicasaWebAlbum *gdata_picasaweb_album_new_from_xml (const gchar *xml, gint length, GError **error) G_GNUC_WARN_UNUSED_RESULT;

const gchar *gdata_picasaweb_album_get_user (GDataPicasaWebAlbum *self);
const gchar *gdata_picasaweb_album_get_nickname (GDataPicasaWebAlbum *self);
void gdata_picasaweb_album_get_edited (GDataPicasaWebAlbum *self, GTimeVal *edited);
const gchar *gdata_picasaweb_album_get_name (GDataPicasaWebAlbum *self);
const gchar *gdata_picasaweb_album_get_location (GDataPicasaWebAlbum *self);
void gdata_picasaweb_album_set_location (GDataPicasaWebAlbum *self, const gchar *location);
GDataPicasaWebVisibility gdata_picasaweb_album_get_visibility (GDataPicasaWebAlbum *self);
void gdata_picasaweb_album_set_visibility (GDataPicasaWebAlbum *self, GDataPicasaWebVisibility visibility);
void gdata_picasaweb_album_get_timestamp (GDataPicasaWebAlbum *self, GTimeVal *timestamp);
void gdata_picasaweb_album_set_timestamp (GDataPicasaWebAlbum *self, GTimeVal *timestamp);
guint gdata_picasaweb_album_get_num_photos (GDataPicasaWebAlbum *self);
guint gdata_picasaweb_album_get_num_photos_remaining (GDataPicasaWebAlbum *self);
glong gdata_picasaweb_album_get_bytes_used (GDataPicasaWebAlbum *self);
gboolean gdata_picasaweb_album_is_commenting_enabled (GDataPicasaWebAlbum *self);
void gdata_picasaweb_album_set_is_commenting_enabled (GDataPicasaWebAlbum *self, gboolean is_commenting_enabled);
guint gdata_picasaweb_album_get_comment_count (GDataPicasaWebAlbum *self);
const gchar *gdata_picasaweb_album_get_tags (GDataPicasaWebAlbum *self);
void gdata_picasaweb_album_set_tags (GDataPicasaWebAlbum *self, const gchar *tags);
const gchar *gdata_picasaweb_album_get_description (GDataPicasaWebAlbum *self);
void gdata_picasaweb_album_set_description (GDataPicasaWebAlbum *self, const gchar *description);
GList *gdata_picasaweb_album_get_contents (GDataPicasaWebAlbum *self);
GList *gdata_picasaweb_album_get_thumbnails (GDataPicasaWebAlbum *self);

G_END_DECLS

#endif /* !GDATA_PICASAWEB_ALBUM_H */
