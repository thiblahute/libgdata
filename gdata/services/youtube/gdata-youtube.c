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

#include "gdata-youtube.h"

GDataYouTubeState *
gdata_youtube_state_new (const gchar *name, const gchar *message, const gchar *reason_code, const gchar *help_uri)
{
	GDataYouTubeState *self = g_slice_new (GDataYouTubeState);
	self->name = g_strdup (name);
	self->message = g_strdup (message);
	self->reason_code = g_strdup (reason_code);
	self->help_uri = g_strdup (help_uri);
	return self;
}

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
