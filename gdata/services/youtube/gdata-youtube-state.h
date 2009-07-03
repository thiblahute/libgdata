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

#ifndef GDATA_YOUTUBE_STATE_H
#define GDATA_YOUTUBE_STATE_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-parsable.h>

G_BEGIN_DECLS

#define GDATA_TYPE_YOUTUBE_STATE		(gdata_youtube_state_get_type ())
#define GDATA_YOUTUBE_STATE(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_YOUTUBE_STATE, GDataYouTubeState))
#define GDATA_YOUTUBE_STATE_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_YOUTUBE_STATE, GDataYouTubeStateClass))
#define GDATA_IS_YOUTUBE_STATE(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_YOUTUBE_STATE))
#define GDATA_IS_YOUTUBE_STATE_CLASS(k)		(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_YOUTUBE_STATE))
#define GDATA_YOUTUBE_STATE_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_YOUTUBE_STATE, GDataYouTubeStateClass))

typedef struct _GDataYouTubeStatePrivate	GDataYouTubeStatePrivate;

/**
 * GDataYouTubeState:
 *
 * All the fields in the #GDataYouTubeState structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataParsable parent;
	GDataYouTubeStatePrivate *priv;
} GDataYouTubeState;

/**
 * GDataYouTubeStateClass:
 *
 * All the fields in the #GDataYouTubeStateClass structure are private and should never be accessed directly.
 *
 * Since: 0.4.0
 **/
typedef struct {
	/*< private >*/
	GDataParsableClass parent;
} GDataYouTubeStateClass;

GType gdata_youtube_state_get_type (void) G_GNUC_CONST;

const gchar *gdata_youtube_state_get_name (GDataYouTubeState *self);
const gchar *gdata_youtube_state_get_reason_code (GDataYouTubeState *self);
const gchar *gdata_youtube_state_get_help_uri (GDataYouTubeState *self);
const gchar *gdata_youtube_state_get_message (GDataYouTubeState *self);

G_END_DECLS

#endif /* !GDATA_YOUTUBE_STATE_H */
