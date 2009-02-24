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

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <libsoup/soup.h>
#include <string.h>

#include "gdata-calendar-service.h"
#include "gdata-service.h"
#include "gdata-private.h"

/* Standards reference here: http://code.google.com/apis/calendar/docs/2.0/reference.html */

G_DEFINE_TYPE (GDataCalendarService, gdata_calendar_service, GDATA_TYPE_SERVICE)
#define GDATA_CALENDAR_SERVICE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_CALENDAR_SERVICE, GDataCalendarServicePrivate))

static void
gdata_calendar_service_class_init (GDataCalendarServiceClass *klass)
{
	GDataServiceClass *service_class = GDATA_SERVICE_CLASS (klass);
	service_class->service_name = "cl";
}

static void
gdata_calendar_service_init (GDataCalendarService *self)
{
	/* Nothing to see here */
}

GDataCalendarService *
gdata_calendar_service_new (const gchar *client_id)
{
	g_return_val_if_fail (client_id != NULL, NULL);

	return g_object_new (GDATA_TYPE_CALENDAR_SERVICE,
			     "client-id", client_id,
			     NULL);
}

GDataFeed *
gdata_calendar_service_query_all_calendars (GDataCalendarService *self, gint start_index, gint max_results,
					    GCancellable *cancellable, GError **error)
{
	GDataQuery *query;
	GDataFeed *feed;

	/* Ensure we're authenticated first */
	if (gdata_service_is_authenticated (GDATA_SERVICE (self)) == FALSE) {
		g_set_error_literal (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_AUTHENTICATION_REQUIRED,
				     _("You must be authenticated to query all calendars."));
		return NULL;
	}

	query = gdata_query_new_with_limits (GDATA_SERVICE (self), NULL, start_index, max_results);
	feed = gdata_service_query (GDATA_SERVICE (self), "http://www.google.com/calendar/feeds/default/allcalendars/full", query,
				    (GDataEntryParserFunc) _gdata_calendar_calendar_new_from_xml_node, cancellable, error);
	g_object_unref (query);

	return feed;
}

void
gdata_calendar_service_query_all_calendars_async (GDataCalendarService *self, gint start_index, gint max_results,
						  GCancellable *cancellable, GAsyncReadyCallback callback, gpointer user_data)
{
	GDataQuery *query;

	/* Ensure we're authenticated first */
	if (gdata_service_is_authenticated (GDATA_SERVICE (self)) == FALSE) {
		g_simple_async_report_error_in_idle (G_OBJECT (self), callback, user_data,
						     GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_AUTHENTICATION_REQUIRED,
						     _("You must be authenticated to query all calendars."));
		return;
	}

	query = gdata_query_new_with_limits (GDATA_SERVICE (self), NULL, start_index, max_results);
	gdata_service_query_async (GDATA_SERVICE (self), "http://www.google.com/calendar/feeds/default/allcalendars/full", query,
				   (GDataEntryParserFunc) _gdata_calendar_calendar_new_from_xml_node,
				   cancellable, callback, user_data);
	g_object_unref (query);
}

GDataFeed *
gdata_calendar_service_query_own_calendars (GDataCalendarService *self, gint start_index, gint max_results,
					    GCancellable *cancellable, GError **error)
{
	GDataQuery *query;
	GDataFeed *feed;

	/* Ensure we're authenticated first */
	if (gdata_service_is_authenticated (GDATA_SERVICE (self)) == FALSE) {
		g_set_error_literal (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_AUTHENTICATION_REQUIRED,
				     _("You must be authenticated to query your own calendars."));
		return NULL;
	}

	query = gdata_query_new_with_limits (GDATA_SERVICE (self), NULL, start_index, max_results);
	feed = gdata_service_query (GDATA_SERVICE (self), "http://www.google.com/calendar/feeds/default/owncalendars/full", query,
				    (GDataEntryParserFunc) _gdata_calendar_calendar_new_from_xml_node, cancellable, error);
	g_object_unref (query);

	return feed;
}

void
gdata_calendar_service_query_own_calendars_async (GDataCalendarService *self, gint start_index, gint max_results,
						  GCancellable *cancellable, GAsyncReadyCallback callback, gpointer user_data)
{
	GDataQuery *query;

	/* Ensure we're authenticated first */
	if (gdata_service_is_authenticated (GDATA_SERVICE (self)) == FALSE) {
		g_simple_async_report_error_in_idle (G_OBJECT (self), callback, user_data,
						     GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_AUTHENTICATION_REQUIRED,
						     _("You must be authenticated to query your own calendars."));
		return;
	}

	query = gdata_query_new_with_limits (GDATA_SERVICE (self), NULL, start_index, max_results);
	gdata_service_query_async (GDATA_SERVICE (self), "http://www.google.com/calendar/feeds/default/owncalendars/full", query,
				   (GDataEntryParserFunc) _gdata_calendar_calendar_new_from_xml_node,
				   cancellable, callback, user_data);
	g_object_unref (query);
}
