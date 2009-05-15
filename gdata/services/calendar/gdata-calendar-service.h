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

#ifndef GDATA_CALENDAR_SERVICE_H
#define GDATA_CALENDAR_SERVICE_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-service.h>
#include <gdata/gdata-query.h>
#include <gdata/services/calendar/gdata-calendar-calendar.h>

G_BEGIN_DECLS

#define GDATA_TYPE_CALENDAR_SERVICE		(gdata_calendar_service_get_type ())
#define GDATA_CALENDAR_SERVICE(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_CALENDAR_SERVICE, GDataCalendarService))
#define GDATA_CALENDAR_SERVICE_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_CALENDAR_SERVICE, GDataCalendarServiceClass))
#define GDATA_IS_CALENDAR_SERVICE(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_CALENDAR_SERVICE))
#define GDATA_IS_CALENDAR_SERVICE_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_CALENDAR_SERVICE))
#define GDATA_CALENDAR_SERVICE_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_CALENDAR_SERVICE, GDataCalendarServiceClass))

typedef struct _GDataCalendarServicePrivate	GDataCalendarServicePrivate;

/**
 * GDataCalendarService:
 *
 * All the fields in the #GDataCalendarService structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataService parent;
} GDataCalendarService;

/**
 * GDataCalendarServiceClass:
 *
 * All the fields in the #GDataCalendarServiceClass structure are private and should never be accessed directly.
 **/
typedef struct {
	/*< private >*/
	GDataServiceClass parent;
} GDataCalendarServiceClass;

GType gdata_calendar_service_get_type (void) G_GNUC_CONST;

GDataCalendarService *gdata_calendar_service_new (const gchar *client_id) G_GNUC_WARN_UNUSED_RESULT;

GDataFeed *gdata_calendar_service_query_all_calendars (GDataCalendarService *self, GDataQuery *query, GCancellable *cancellable,
						       GDataQueryProgressCallback progress_callback, gpointer progress_user_data,
						       GError **error) G_GNUC_WARN_UNUSED_RESULT;
void gdata_calendar_service_query_all_calendars_async (GDataCalendarService *self, GDataQuery *query, GCancellable *cancellable,
						       GDataQueryProgressCallback progress_callback, gpointer progress_user_data,
						       GAsyncReadyCallback callback, gpointer user_data);

GDataFeed *gdata_calendar_service_query_own_calendars (GDataCalendarService *self, GDataQuery *query, GCancellable *cancellable,
						       GDataQueryProgressCallback progress_callback, gpointer progress_user_data,
						       GError **error) G_GNUC_WARN_UNUSED_RESULT;
void gdata_calendar_service_query_own_calendars_async (GDataCalendarService *self, GDataQuery *query, GCancellable *cancellable,
						       GDataQueryProgressCallback progress_callback, gpointer progress_user_data,
						       GAsyncReadyCallback callback, gpointer user_data);

GDataFeed *gdata_calendar_service_query_events (GDataCalendarService *self, GDataCalendarCalendar *calendar, GDataQuery *query,
						GCancellable *cancellable, GDataQueryProgressCallback progress_callback, gpointer progress_user_data,
						GError **error) G_GNUC_WARN_UNUSED_RESULT;

#include <gdata/services/calendar/gdata-calendar-event.h>

GDataCalendarEvent *gdata_calendar_service_insert_event (GDataCalendarService *self, GDataCalendarEvent *event,
							 GCancellable *cancellable, GError **error) G_GNUC_WARN_UNUSED_RESULT;

G_END_DECLS

#endif /* !GDATA_CALENDAR_SERVICE_H */
