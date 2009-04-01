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

#include <glib.h>

#include "gdata.h"
#include "common.h"

static void
test_query_events (void)
{
	GDataFeed *feed, *calendar_feed;
	GDataCalendarCalendar *calendar;
	GDataCalendarService *service;
	GList *calendars;
	GError *error = NULL;

	service = gdata_calendar_service_new (CLIENT_ID);

	/* Log in */
	gdata_service_authenticate (GDATA_SERVICE (service), USERNAME, PASSWORD, NULL, &error);
	g_assert_no_error (error);

	/* Get a calendar */
	calendar_feed = gdata_calendar_service_query_own_calendars (service, NULL, NULL, NULL, NULL, &error);
	g_assert_no_error (error);

	calendars = gdata_feed_get_entries (calendar_feed);
	calendar = calendars->data;

	g_object_ref (calendar);
	g_object_unref (calendar_feed);

	/* Get the entry feed */
	feed = gdata_calendar_service_query_events (service, calendar, NULL, NULL, NULL, NULL, &error);
	g_assert_no_error (error);

	g_object_unref (feed);
	g_object_unref (calendar);
	g_object_unref (service);
}

int
main (int argc, char *argv[])
{
	g_mem_set_vtable (glib_mem_profiler_table);

	g_type_init ();
	g_thread_init (NULL);

	test_query_events ();

	/* Print memory usage data */
	g_mem_profile ();

	return 0;
}
