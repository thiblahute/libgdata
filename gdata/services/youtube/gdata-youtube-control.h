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

#ifndef GDATA_YOUTUBE_CONTROL_H
#define GDATA_YOUTUBE_CONTROL_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-parsable.h>
#include <gdata/services/youtube/gdata-youtube-state.h>

G_BEGIN_DECLS

#define GDATA_TYPE_YOUTUBE_CONTROL		(gdata_youtube_control_get_type ())
#define GDATA_YOUTUBE_CONTROL(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_YOUTUBE_CONTROL, GDataYouTubeControl))
#define GDATA_YOUTUBE_CONTROL_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_YOUTUBE_CONTROL, GDataYouTubeControlClass))
#define GDATA_IS_YOUTUBE_CONTROL(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_YOUTUBE_CONTROL))
#define GDATA_IS_YOUTUBE_CONTROL_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_YOUTUBE_CONTROL))
#define GDATA_YOUTUBE_CONTROL_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_YOUTUBE_CONTROL, GDataYouTubeControlClass))

typedef struct _GDataYouTubeControlPrivate	GDataYouTubeControlPrivate;

/**
 * GDataYouTubeControl:
 *
 * All the fields in the #GDataYouTubeControl structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataParsable parent;
	GDataYouTubeControlPrivate *priv;
} GDataYouTubeControl;

/**
 * GDataYouTubeControlClass:
 *
 * All the fields in the #GDataYouTubeControlClass structure are private and should never be accessed directly.
 *
 * Since: 0.4.0
 **/
typedef struct {
	/*< private >*/
	GDataParsableClass parent;
} GDataYouTubeControlClass;

GType gdata_youtube_control_get_type (void) G_GNUC_CONST;

gboolean gdata_youtube_control_is_draft (GDataYouTubeControl *self);
void gdata_youtube_control_set_is_draft (GDataYouTubeControl *self, gboolean is_draft);
GDataYouTubeState *gdata_youtube_control_get_state (GDataYouTubeControl *self);

G_END_DECLS

#endif /* !GDATA_YOUTUBE_CONTROL_H */
