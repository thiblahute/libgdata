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

typedef struct {
	gchar *name;
	gchar *reason_code;
	gchar *help_uri;
	gchar *message;
} GDataYouTubeState;

GDataYouTubeState *gdata_youtube_state_new (const gchar *name, const gchar *message, const gchar *reason_code, const gchar *help_uri);
void gdata_youtube_state_free (GDataYouTubeState *self);

G_END_DECLS

#endif /* !GDATA_YOUTUBE_H */
