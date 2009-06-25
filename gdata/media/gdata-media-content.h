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

#ifndef GDATA_MEDIA_CONTENT_H
#define GDATA_MEDIA_CONTENT_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-parsable.h>

G_BEGIN_DECLS

/**
 * GDataMediaExpression:
 * @GDATA_MEDIA_EXPRESSION_SAMPLE: the media is a sample of a larger video
 * @GDATA_MEDIA_EXPRESSION_FULL: the media is the full version
 * @GDATA_MEDIA_EXPRESSION_NONSTOP: the media is a continuous stream
 *
 * An enum representing the possible values of #GDataMediaContent:expression.
 **/
typedef enum {
	GDATA_MEDIA_EXPRESSION_SAMPLE,
	GDATA_MEDIA_EXPRESSION_FULL,
	GDATA_MEDIA_EXPRESSION_NONSTOP
} GDataMediaExpression;

/**
 * GDataMediaMedium:
 * @GDATA_MEDIA_UNKNOWN: the type of the media is unknown
 * @GDATA_MEDIA_IMAGE: the media is an image
 * @GDATA_MEDIA_AUDIO: the media is an audio stream
 * @GDATA_MEDIA_VIDEO: the media is a video
 * @GDATA_MEDIA_DOCUMENT: the media is another type of document
 * @GDATA_MEDIA_EXECUTABLE: the media is an executable file
 *
 * An enum representing the possible values of #GDataMediaContent:medium.
 **/
typedef enum {
	GDATA_MEDIA_UNKNOWN,
	GDATA_MEDIA_IMAGE,
	GDATA_MEDIA_AUDIO,
	GDATA_MEDIA_VIDEO,
	GDATA_MEDIA_DOCUMENT,
	GDATA_MEDIA_EXECUTABLE
} GDataMediaMedium;

#define GDATA_TYPE_MEDIA_CONTENT		(gdata_media_content_get_type ())
#define GDATA_MEDIA_CONTENT(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_MEDIA_CONTENT, GDataMediaContent))
#define GDATA_MEDIA_CONTENT_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_MEDIA_CONTENT, GDataMediaContentClass))
#define GDATA_IS_MEDIA_CONTENT(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_MEDIA_CONTENT))
#define GDATA_IS_MEDIA_CONTENT_CLASS(k)		(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_MEDIA_CONTENT))
#define GDATA_MEDIA_CONTENT_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_MEDIA_CONTENT, GDataMediaContentClass))

typedef struct _GDataMediaContentPrivate	GDataMediaContentPrivate;

/**
 * GDataMediaContent:
 *
 * All the fields in the #GDataMediaContent structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataParsable parent;
	GDataMediaContentPrivate *priv;
} GDataMediaContent;

/**
 * GDataMediaContentClass:
 *
 * All the fields in the #GDataMediaContentClass structure are private and should never be accessed directly.
 *
 * Since: 0.4.0
 **/
typedef struct {
	/*< private >*/
	GDataParsableClass parent;
} GDataMediaContentClass;

GType gdata_media_content_get_type (void) G_GNUC_CONST;

const gchar *gdata_media_content_get_uri (GDataMediaContent *self);
gsize gdata_media_content_get_filesize (GDataMediaContent *self);
const gchar *gdata_media_content_get_content_type (GDataMediaContent *self);
GDataMediaMedium gdata_media_content_get_medium (GDataMediaContent *self);
gboolean gdata_media_content_is_default (GDataMediaContent *self);
GDataMediaExpression gdata_media_content_get_expression (GDataMediaContent *self);
gint64 gdata_media_content_get_duration (GDataMediaContent *self);
guint gdata_media_content_get_height (GDataMediaContent *self);
guint gdata_media_content_get_width (GDataMediaContent *self);

G_END_DECLS

#endif /* !GDATA_MEDIA_CONTENT_H */
