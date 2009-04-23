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

#ifndef GDATA_YOUTUBE_H
#define GDATA_YOUTUBE_H

G_BEGIN_DECLS

/**
 * GDataYouTubeState:
 * @name: the publishing status of the video
 * @reason_code: code providing information about why a video failed to upload or was rejected during processing
 * @help_uri: a URI for a page explaining the problem
 * @message: a human-readable message explaining the problem
 *
 * A structure fully representing a YouTube "state" element. All fields are required.
 *
 * See the <ulink type="http" url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_yt:state">YouTube specification</ulink>
 * for more information.
 **/
typedef struct {
	gchar *name;
	gchar *reason_code;
	gchar *help_uri;
	gchar *message;
} GDataYouTubeState;

GDataYouTubeState *gdata_youtube_state_new (const gchar *name, const gchar *message, const gchar *reason_code,
					    const gchar *help_uri) G_GNUC_WARN_UNUSED_RESULT;
void gdata_youtube_state_free (GDataYouTubeState *self);

G_END_DECLS

#endif /* !GDATA_YOUTUBE_H */
