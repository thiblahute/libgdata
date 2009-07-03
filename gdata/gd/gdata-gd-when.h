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

#ifndef GDATA_GD_WHEN_H
#define GDATA_GD_WHEN_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-parsable.h>

G_BEGIN_DECLS

#define GDATA_TYPE_GD_WHEN		(gdata_gd_when_get_type ())
#define GDATA_GD_WHEN(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_GD_WHEN, GDataGDWhen))
#define GDATA_GD_WHEN_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_GD_WHEN, GDataGDWhenClass))
#define GDATA_IS_GD_WHEN(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_GD_WHEN))
#define GDATA_IS_GD_WHEN_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_GD_WHEN))
#define GDATA_GD_WHEN_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_GD_WHEN, GDataGDWhenClass))

typedef struct _GDataGDWhenPrivate	GDataGDWhenPrivate;

/**
 * GDataGDWhen:
 *
 * All the fields in the #GDataGDWhen structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataParsable parent;
	GDataGDWhenPrivate *priv;
} GDataGDWhen;

/**
 * GDataGDWhenClass:
 *
 * All the fields in the #GDataGDWhenClass structure are private and should never be accessed directly.
 *
 * Since: 0.4.0
 **/
typedef struct {
	/*< private >*/
	GDataParsableClass parent;
} GDataGDWhenClass;

GType gdata_gd_when_get_type (void) G_GNUC_CONST;

GDataGDWhen *gdata_gd_when_new (GTimeVal *start_time, GTimeVal *end_time, gboolean is_date) G_GNUC_WARN_UNUSED_RESULT;
gint gdata_gd_when_compare (const GDataGDWhen *a, const GDataGDWhen *b);

void gdata_gd_when_get_start_time (GDataGDWhen *self, GTimeVal *start_time);
void gdata_gd_when_set_start_time (GDataGDWhen *self, GTimeVal *start_time);

void gdata_gd_when_get_end_time (GDataGDWhen *self, GTimeVal *end_time);
void gdata_gd_when_set_end_time (GDataGDWhen *self, GTimeVal *end_time);

gboolean gdata_gd_when_is_date (GDataGDWhen *self);
void gdata_gd_when_set_is_date (GDataGDWhen *self, gboolean is_date);

const gchar *gdata_gd_when_get_value_string (GDataGDWhen *self);
void gdata_gd_when_set_value_string (GDataGDWhen *self, const gchar *value_string);

GList *gdata_gd_when_get_reminders (GDataGDWhen *self);
/* TODO: More reminder API */

G_END_DECLS

#endif /* !GDATA_GD_WHEN_H */
