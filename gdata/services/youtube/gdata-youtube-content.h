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

#ifndef GDATA_YOUTUBE_CONTENT_H
#define GDATA_YOUTUBE_CONTENT_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-parsable.h>
#include <gdata/media/gdata-media-content.h>

G_BEGIN_DECLS

/**
 * GDataYouTubeFormat:
 * @GDATA_YOUTUBE_FORMAT_UNKNOWN: retrieve videos in all formats when querying the service
 * @GDATA_YOUTUBE_FORMAT_RTSP_H263_AMR: RTSP streaming URI for mobile video playback; H.263 video (up to 176×144) and AMR audio
 * @GDATA_YOUTUBE_FORMAT_HTTP_SWF: HTTP URI to the embeddable player (SWF) for this video
 * @GDATA_YOUTUBE_FORMAT_RTSP_MPEG4_AAC: RTSP streaming URI for mobile video playback; MPEG-4 SP video (up to 176×144) and AAC audio
 *
 * Video formats available on YouTube. For more information, see the
 * <ulink type="http" url="http://code.google.com/apis/youtube/2.0/reference.html#formatsp">online documentation</ulink>.
 *
 * Since: 0.3.0
 **/
typedef enum {
	GDATA_YOUTUBE_FORMAT_UNKNOWN = 0,
	GDATA_YOUTUBE_FORMAT_RTSP_H263_AMR = 1,
	GDATA_YOUTUBE_FORMAT_HTTP_SWF = 5,
	GDATA_YOUTUBE_FORMAT_RTSP_MPEG4_AAC = 6
} GDataYouTubeFormat;

#define GDATA_TYPE_YOUTUBE_CONTENT		(gdata_youtube_content_get_type ())
#define GDATA_YOUTUBE_CONTENT(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_YOUTUBE_CONTENT, GDataYouTubeContent))
#define GDATA_YOUTUBE_CONTENT_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_YOUTUBE_CONTENT, GDataYouTubeContentClass))
#define GDATA_IS_YOUTUBE_CONTENT(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_YOUTUBE_CONTENT))
#define GDATA_IS_YOUTUBE_CONTENT_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_YOUTUBE_CONTENT))
#define GDATA_YOUTUBE_CONTENT_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_YOUTUBE_CONTENT, GDataYouTubeContentClass))

typedef struct _GDataYouTubeContentPrivate	GDataYouTubeContentPrivate;

/**
 * GDataYouTubeContent:
 *
 * All the fields in the #GDataYouTubeContent structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataMediaContent parent;
	GDataYouTubeContentPrivate *priv;
} GDataYouTubeContent;

/**
 * GDataYouTubeContentClass:
 *
 * All the fields in the #GDataYouTubeContentClass structure are private and should never be accessed directly.
 *
 * Since: 0.4.0
 **/
typedef struct {
	/*< private >*/
	GDataMediaContentClass parent;
} GDataYouTubeContentClass;

GType gdata_youtube_content_get_type (void) G_GNUC_CONST;
GDataYouTubeFormat gdata_youtube_content_get_format (GDataYouTubeContent *self);

G_END_DECLS

#endif /* !GDATA_YOUTUBE_CONTENT_H */
