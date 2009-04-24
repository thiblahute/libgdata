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

/**
 * SECTION:gdata-youtube
 * @short_description: YouTube namespace API
 * @stability: Unstable
 * @include: gdata/services/youtube/gdata-youtube.h
 *
 * The structures here represent several core elements in the
 * <ulink type="http" url="http://code.google.com/apis/youtube/2.0/reference.html#YouTube_elements_reference">YouTube specification</ulink>
 * which are used exclusively in the YouTube API.
 **/

#include "gdata-youtube.h"

/**
 * gdata_youtube_state_new:
 * @name: the publishing status of the video
 * @reason_code: code providing information about why a video failed to upload or was rejected during processing
 * @help_uri: a URI for a page explaining the problem, or %NULL
 * @message: a human-readable message explaining the problem, or %NULL
 *
 * Creates a new #GDataYouTubeState. More information is available in the <ulink type="http"
 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_yt:state">YouTube specification</ulink>.
 *
 * Return value: a new #GDataYouTubeState, or %NULL; free with gdata_youtube_state_free()
 **/
GDataYouTubeState *
gdata_youtube_state_new (const gchar *name, const gchar *message, const gchar *reason_code, const gchar *help_uri)
{
	GDataYouTubeState *self;

	g_return_val_if_fail (name != NULL, NULL);
	g_return_val_if_fail (message != NULL, NULL);

	self = g_slice_new (GDataYouTubeState);
	self->name = g_strdup (name);
	self->message = g_strdup (message);
	self->reason_code = g_strdup (reason_code);
	self->help_uri = g_strdup (help_uri);
	return self;
}

/**
 * gdata_youtube_state_free:
 * @self: a #GDataYouTubeState
 *
 * Frees a #GDataYouTubeState.
 **/
void
gdata_youtube_state_free (GDataYouTubeState *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->name);
	g_free (self->message);
	g_free (self->reason_code);
	g_free (self->help_uri);
	g_slice_free (GDataYouTubeState, self);
}
