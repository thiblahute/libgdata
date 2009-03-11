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

#ifndef GDATA_CALENDAR_CALENDAR_H
#define GDATA_CALENDAR_CALENDAR_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-entry.h>
#include <gdata/gdata-gdata.h>
#include <gdata/gdata-types.h>

G_BEGIN_DECLS

#define GDATA_TYPE_CALENDAR_CALENDAR		(gdata_calendar_calendar_get_type ())
#define GDATA_CALENDAR_CALENDAR(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_CALENDAR_CALENDAR, GDataCalendarCalendar))
#define GDATA_CALENDAR_CALENDAR_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_CALENDAR_CALENDAR, GDataCalendarCalendarClass))
#define GDATA_IS_CALENDAR_CALENDAR(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_CALENDAR_CALENDAR))
#define GDATA_IS_CALENDAR_CALENDAR_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_CALENDAR_CALENDAR))
#define GDATA_CALENDAR_CALENDAR_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_CALENDAR_CALENDAR, GDataCalendarCalendarClass))

typedef struct _GDataCalendarCalendarPrivate	GDataCalendarCalendarPrivate;

typedef struct {
	GDataEntry parent;
	GDataCalendarCalendarPrivate *priv;
} GDataCalendarCalendar;

typedef struct {
	GDataEntryClass parent;
} GDataCalendarCalendarClass;

GType gdata_calendar_calendar_get_type (void);

GDataCalendarCalendar *gdata_calendar_calendar_new (void);
GDataCalendarCalendar *gdata_calendar_calendar_new_from_xml (const gchar *xml, gint length, GError **error);

const gchar *gdata_calendar_calendar_get_timezone (GDataCalendarCalendar *self);
void gdata_calendar_calendar_set_timezone (GDataCalendarCalendar *self, const gchar *timezone);
guint gdata_calendar_calendar_get_times_cleaned (GDataCalendarCalendar *self);
void gdata_calendar_calendar_set_times_cleaned (GDataCalendarCalendar *self, guint times_cleaned);
gboolean gdata_calendar_calendar_get_hidden (GDataCalendarCalendar *self);
void gdata_calendar_calendar_set_hidden (GDataCalendarCalendar *self, gboolean hidden);
void gdata_calendar_calendar_get_color (GDataCalendarCalendar *self, GDataColor *color);
void gdata_calendar_calendar_set_color (GDataCalendarCalendar *self, GDataColor *color);
gboolean gdata_calendar_calendar_get_selected (GDataCalendarCalendar *self);
void gdata_calendar_calendar_set_selected (GDataCalendarCalendar *self, gboolean selected);
const gchar *gdata_calendar_calendar_get_access_level (GDataCalendarCalendar *self);
void gdata_calendar_calendar_set_access_level (GDataCalendarCalendar *self, const gchar *access_level);
void gdata_calendar_calendar_get_edited (GDataCalendarCalendar *self, GTimeVal *edited);
void gdata_calendar_calendar_set_edited (GDataCalendarCalendar *self, GTimeVal *edited);

G_END_DECLS

#endif /* !GDATA_CALENDAR_CALENDAR_H */
