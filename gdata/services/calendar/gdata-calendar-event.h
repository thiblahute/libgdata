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

#ifndef GDATA_CALENDAR_EVENT_H
#define GDATA_CALENDAR_EVENT_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-entry.h>
#include <gdata/gdata-gdata.h>
#include <gdata/gdata-types.h>

G_BEGIN_DECLS

#define GDATA_TYPE_CALENDAR_EVENT		(gdata_calendar_event_get_type ())
#define GDATA_CALENDAR_EVENT(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_CALENDAR_EVENT, GDataCalendarEvent))
#define GDATA_CALENDAR_EVENT_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_CALENDAR_EVENT, GDataCalendarEventClass))
#define GDATA_IS_CALENDAR_EVENT(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_CALENDAR_EVENT))
#define GDATA_IS_CALENDAR_EVENT_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_CALENDAR_EVENT))
#define GDATA_CALENDAR_EVENT_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_CALENDAR_EVENT, GDataCalendarEventClass))

typedef struct _GDataCalendarEventPrivate	GDataCalendarEventPrivate;

typedef struct {
	GDataEntry parent;
	GDataCalendarEventPrivate *priv;
} GDataCalendarEvent;

typedef struct {
	GDataEntryClass parent;
} GDataCalendarEventClass;

GType gdata_calendar_event_get_type (void);

GDataCalendarEvent *gdata_calendar_event_new (void);
GDataCalendarEvent *gdata_calendar_event_new_from_xml (const gchar *xml, gint length, GError **error);

void gdata_calendar_event_get_edited (GDataCalendarEvent *self, GTimeVal *edited);
void gdata_calendar_event_set_edited (GDataCalendarEvent *self, GTimeVal *edited);
const gchar *gdata_calendar_event_get_event_status (GDataCalendarEvent *self);
void gdata_calendar_event_set_event_status (GDataCalendarEvent *self, const gchar *event_status);
const gchar *gdata_calendar_event_get_visibility (GDataCalendarEvent *self);
void gdata_calendar_event_set_visibility (GDataCalendarEvent *self, const gchar *visibility);
const gchar *gdata_calendar_event_get_transparency (GDataCalendarEvent *self);
void gdata_calendar_event_set_transparency (GDataCalendarEvent *self, const gchar *transparency);
const gchar *gdata_calendar_event_get_uid (GDataCalendarEvent *self);
void gdata_calendar_event_set_uid (GDataCalendarEvent *self, const gchar *uid);
guint gdata_calendar_event_get_sequence (GDataCalendarEvent *self);
void gdata_calendar_event_set_sequence (GDataCalendarEvent *self, guint sequence);
void gdata_calendar_event_get_start_time (GDataCalendarEvent *self, GTimeVal *start_time);
void gdata_calendar_event_set_start_time (GDataCalendarEvent *self, GTimeVal *start_time);
void gdata_calendar_event_get_end_time (GDataCalendarEvent *self, GTimeVal *end_time);
void gdata_calendar_event_set_end_time (GDataCalendarEvent *self, GTimeVal *end_time);
const gchar *gdata_calendar_event_get_when_value (GDataCalendarEvent *self);
void gdata_calendar_event_set_when_value (GDataCalendarEvent *self, const gchar *when_value);
gboolean gdata_calendar_event_get_guests_can_modify (GDataCalendarEvent *self);
void gdata_calendar_event_set_guests_can_modify (GDataCalendarEvent *self, gboolean guests_can_modify);
gboolean gdata_calendar_event_get_guests_can_invite_others (GDataCalendarEvent *self);
void gdata_calendar_event_set_guests_can_invite_others (GDataCalendarEvent *self, gboolean guests_can_invite_others);
gboolean gdata_calendar_event_get_guests_can_see_guests (GDataCalendarEvent *self);
void gdata_calendar_event_set_guests_can_see_guests (GDataCalendarEvent *self, gboolean guests_can_see_guests);
gboolean gdata_calendar_event_get_anyone_can_add_self (GDataCalendarEvent *self);
void gdata_calendar_event_set_anyone_can_add_self (GDataCalendarEvent *self, gboolean anyone_can_add_self);
void gdata_calendar_event_add_person (GDataCalendarEvent *self, GDataGDWho *who);
void gdata_calendar_event_add_place (GDataCalendarEvent *self, GDataGDWhere *where);

G_END_DECLS

#endif /* !GDATA_CALENDAR_EVENT_H */
