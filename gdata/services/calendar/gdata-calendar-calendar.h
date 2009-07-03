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

#ifndef GDATA_CALENDAR_CALENDAR_H
#define GDATA_CALENDAR_CALENDAR_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-entry.h>
#include <gdata/gdata-types.h>

G_BEGIN_DECLS

#define GDATA_TYPE_CALENDAR_CALENDAR		(gdata_calendar_calendar_get_type ())
#define GDATA_CALENDAR_CALENDAR(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_CALENDAR_CALENDAR, GDataCalendarCalendar))
#define GDATA_CALENDAR_CALENDAR_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_CALENDAR_CALENDAR, GDataCalendarCalendarClass))
#define GDATA_IS_CALENDAR_CALENDAR(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_CALENDAR_CALENDAR))
#define GDATA_IS_CALENDAR_CALENDAR_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_CALENDAR_CALENDAR))
#define GDATA_CALENDAR_CALENDAR_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_CALENDAR_CALENDAR, GDataCalendarCalendarClass))

typedef struct _GDataCalendarCalendarPrivate	GDataCalendarCalendarPrivate;

/**
 * GDataCalendarCalendar:
 *
 * All the fields in the #GDataCalendarCalendar structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataEntry parent;
	GDataCalendarCalendarPrivate *priv;
} GDataCalendarCalendar;

/**
 * GDataCalendarCalendarClass:
 *
 * All the fields in the #GDataCalendarCalendarClass structure are private and should never be accessed directly.
 **/
typedef struct {
	/*< private >*/
	GDataEntryClass parent;
} GDataCalendarCalendarClass;

GType gdata_calendar_calendar_get_type (void) G_GNUC_CONST;

GDataCalendarCalendar *gdata_calendar_calendar_new (const gchar *id) G_GNUC_WARN_UNUSED_RESULT;

const gchar *gdata_calendar_calendar_get_timezone (GDataCalendarCalendar *self);
void gdata_calendar_calendar_set_timezone (GDataCalendarCalendar *self, const gchar *_timezone);
guint gdata_calendar_calendar_get_times_cleaned (GDataCalendarCalendar *self);
gboolean gdata_calendar_calendar_is_hidden (GDataCalendarCalendar *self);
void gdata_calendar_calendar_set_is_hidden (GDataCalendarCalendar *self, gboolean is_hidden);
void gdata_calendar_calendar_get_color (GDataCalendarCalendar *self, GDataColor *color);
void gdata_calendar_calendar_set_color (GDataCalendarCalendar *self, GDataColor *color);
gboolean gdata_calendar_calendar_is_selected (GDataCalendarCalendar *self);
void gdata_calendar_calendar_set_is_selected (GDataCalendarCalendar *self, gboolean is_selected);
const gchar *gdata_calendar_calendar_get_access_level (GDataCalendarCalendar *self);
void gdata_calendar_calendar_get_edited (GDataCalendarCalendar *self, GTimeVal *edited);

G_END_DECLS

#endif /* !GDATA_CALENDAR_CALENDAR_H */
