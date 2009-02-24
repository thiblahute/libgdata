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

#ifndef GDATA_CALENDAR_SERVICE_H
#define GDATA_CALENDAR_SERVICE_H

#include <glib.h>
#include <glib-object.h>
#include <libsoup/soup.h>

#include "gdata-service.h"

G_BEGIN_DECLS

#define GDATA_TYPE_CALENDAR_SERVICE		(gdata_calendar_service_get_type ())
#define GDATA_CALENDAR_SERVICE(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_CALENDAR_SERVICE, GDataCalendarService))
#define GDATA_CALENDAR_SERVICE_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_CALENDAR_SERVICE, GDataCalendarServiceClass))
#define GDATA_IS_CALENDAR_SERVICE(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_CALENDAR_SERVICE))
#define GDATA_IS_CALENDAR_SERVICE_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_CALENDAR_SERVICE))
#define GDATA_CALENDAR_SERVICE_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_CALENDAR_SERVICE, GDataCalendarServiceClass))

typedef struct _GDataCalendarServicePrivate	GDataCalendarServicePrivate;

typedef struct {
	GDataService parent;
} GDataCalendarService;

typedef struct {
	GDataServiceClass parent;
} GDataCalendarServiceClass;

GType gdata_calendar_service_get_type (void);

GDataCalendarService *gdata_calendar_service_new (const gchar *client_id);

GDataFeed *gdata_calendar_service_query_all_calendars (GDataCalendarService *self, gint start_index, gint max_results,
						       GCancellable *cancellable, GError **error);
void gdata_calendar_service_query_all_calendars_async (GDataCalendarService *self, gint start_index, gint max_results,
						       GCancellable *cancellable, GAsyncReadyCallback callback, gpointer user_data);

GDataFeed *gdata_calendar_service_query_own_calendars (GDataCalendarService *self, gint start_index, gint max_results,
						       GCancellable *cancellable, GError **error);
void gdata_calendar_service_query_own_calendars_async (GDataCalendarService *self, gint start_index, gint max_results,
						       GCancellable *cancellable, GAsyncReadyCallback callback, gpointer user_data);

G_END_DECLS

#endif /* !GDATA_CALENDAR_SERVICE_H */
