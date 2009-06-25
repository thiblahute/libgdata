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
#include <gdata/gdata-types.h>
#include <gdata/gd/gdata-gd-when.h>
#include <gdata/gd/gdata-gd-where.h>
#include <gdata/gd/gdata-gd-who.h>

G_BEGIN_DECLS

#define GDATA_TYPE_CALENDAR_EVENT		(gdata_calendar_event_get_type ())
#define GDATA_CALENDAR_EVENT(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_CALENDAR_EVENT, GDataCalendarEvent))
#define GDATA_CALENDAR_EVENT_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_CALENDAR_EVENT, GDataCalendarEventClass))
#define GDATA_IS_CALENDAR_EVENT(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_CALENDAR_EVENT))
#define GDATA_IS_CALENDAR_EVENT_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_CALENDAR_EVENT))
#define GDATA_CALENDAR_EVENT_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_CALENDAR_EVENT, GDataCalendarEventClass))

typedef struct _GDataCalendarEventPrivate	GDataCalendarEventPrivate;

/**
 * GDataCalendarEvent:
 *
 * All the fields in the #GDataCalendarEvent structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataEntry parent;
	GDataCalendarEventPrivate *priv;
} GDataCalendarEvent;

/**
 * GDataCalendarEventClass:
 *
 * All the fields in the #GDataCalendarEventClass structure are private and should never be accessed directly.
 **/
typedef struct {
	/*< private >*/
	GDataEntryClass parent;
} GDataCalendarEventClass;

GType gdata_calendar_event_get_type (void) G_GNUC_CONST;

GDataCalendarEvent *gdata_calendar_event_new (const gchar *id) G_GNUC_WARN_UNUSED_RESULT;
GDataCalendarEvent *gdata_calendar_event_new_from_xml (const gchar *xml, gint length, GError **error) G_GNUC_WARN_UNUSED_RESULT;

void gdata_calendar_event_get_edited (GDataCalendarEvent *self, GTimeVal *edited);
const gchar *gdata_calendar_event_get_status (GDataCalendarEvent *self);
void gdata_calendar_event_set_status (GDataCalendarEvent *self, const gchar *status);
const gchar *gdata_calendar_event_get_visibility (GDataCalendarEvent *self);
void gdata_calendar_event_set_visibility (GDataCalendarEvent *self, const gchar *visibility);
const gchar *gdata_calendar_event_get_transparency (GDataCalendarEvent *self);
void gdata_calendar_event_set_transparency (GDataCalendarEvent *self, const gchar *transparency);
const gchar *gdata_calendar_event_get_uid (GDataCalendarEvent *self);
void gdata_calendar_event_set_uid (GDataCalendarEvent *self, const gchar *uid);
guint gdata_calendar_event_get_sequence (GDataCalendarEvent *self);
void gdata_calendar_event_set_sequence (GDataCalendarEvent *self, guint sequence);
gboolean gdata_calendar_event_get_guests_can_modify (GDataCalendarEvent *self);
void gdata_calendar_event_set_guests_can_modify (GDataCalendarEvent *self, gboolean guests_can_modify);
gboolean gdata_calendar_event_get_guests_can_invite_others (GDataCalendarEvent *self);
void gdata_calendar_event_set_guests_can_invite_others (GDataCalendarEvent *self, gboolean guests_can_invite_others);
gboolean gdata_calendar_event_get_guests_can_see_guests (GDataCalendarEvent *self);
void gdata_calendar_event_set_guests_can_see_guests (GDataCalendarEvent *self, gboolean guests_can_see_guests);
gboolean gdata_calendar_event_get_anyone_can_add_self (GDataCalendarEvent *self);
void gdata_calendar_event_set_anyone_can_add_self (GDataCalendarEvent *self, gboolean anyone_can_add_self);
void gdata_calendar_event_add_person (GDataCalendarEvent *self, GDataGDWho *who);
GList *gdata_calendar_event_get_people (GDataCalendarEvent *self);
void gdata_calendar_event_add_place (GDataCalendarEvent *self, GDataGDWhere *where);
GList *gdata_calendar_event_get_places (GDataCalendarEvent *self);
void gdata_calendar_event_add_time (GDataCalendarEvent *self, GDataGDWhen *when);
GList *gdata_calendar_event_get_times (GDataCalendarEvent *self);
gboolean gdata_calendar_event_get_primary_time (GDataCalendarEvent *self, GTimeVal *start_time, GTimeVal *end_time, GDataGDWhen **when);
const gchar *gdata_calendar_event_get_recurrence (GDataCalendarEvent *self);
void gdata_calendar_event_set_recurrence (GDataCalendarEvent *self, const gchar *recurrence);
void gdata_calendar_event_get_original_event_details (GDataCalendarEvent *self, gchar **event_id, gchar **event_uri);
gboolean gdata_calendar_event_is_exception (GDataCalendarEvent *self);

G_END_DECLS

#endif /* !GDATA_CALENDAR_EVENT_H */
