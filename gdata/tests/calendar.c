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
#include <unistd.h>

#include "gdata.h"
#include "common.h"

/* TODO: probably a better way to do this; some kind of data associated with the test suite? */
static GDataService *service = NULL;
static GMainLoop *main_loop = NULL;

static void
test_authentication (void)
{
	gboolean retval;
	GError *error = NULL;

	/* Create a service */
	service = GDATA_SERVICE (gdata_calendar_service_new (CLIENT_ID));

	g_assert (service != NULL);
	g_assert (GDATA_IS_SERVICE (service));
	g_assert_cmpstr (gdata_service_get_client_id (service), ==, CLIENT_ID);

	/* Log in */
	retval = gdata_service_authenticate (service, USERNAME, PASSWORD, NULL, &error);
	g_assert_no_error (error);
	g_assert (retval == TRUE);
	g_clear_error (&error);

	/* Check all is as it should be */
	g_assert (gdata_service_is_authenticated (service) == TRUE);
	g_assert_cmpstr (gdata_service_get_username (service), ==, USERNAME);
	g_assert_cmpstr (gdata_service_get_password (service), ==, PASSWORD);
}

static void
test_authentication_async_cb (GDataService *service, GAsyncResult *async_result, gpointer user_data)
{
	gboolean retval;
	GError *error = NULL;

	retval = gdata_service_authenticate_finish (service, async_result, &error);
	g_assert_no_error (error);
	g_assert (retval == TRUE);
	g_clear_error (&error);

	g_main_loop_quit (main_loop);

	/* Check all is as it should be */
	g_assert (gdata_service_is_authenticated (service) == TRUE);
	g_assert_cmpstr (gdata_service_get_username (service), ==, USERNAME);
	g_assert_cmpstr (gdata_service_get_password (service), ==, PASSWORD);
}

static void
test_authentication_async (void)
{
	/* Create a service */
	service = GDATA_SERVICE (gdata_calendar_service_new (CLIENT_ID));

	g_assert (service != NULL);
	g_assert (GDATA_IS_SERVICE (service));

	gdata_service_authenticate_async (service, USERNAME, PASSWORD, NULL, (GAsyncReadyCallback) test_authentication_async_cb, NULL);

	main_loop = g_main_loop_new (NULL, TRUE);
	g_main_loop_run (main_loop);
	g_main_loop_unref (main_loop);
}

static void
test_query_all_calendars (void)
{
	GDataFeed *feed;
	GError *error = NULL;

	g_assert (service != NULL);

	feed = gdata_calendar_service_query_all_calendars (GDATA_CALENDAR_SERVICE (service), NULL, NULL, NULL, NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (feed));
	g_clear_error (&error);

	/* TODO: check entries and feed properties */

	g_object_unref (feed);
}

static void
test_query_all_calendars_async_cb (GDataService *service, GAsyncResult *async_result, gpointer user_data)
{
	GDataFeed *feed;
	GError *error = NULL;

	feed = gdata_service_query_finish (service, async_result, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (feed));
	g_clear_error (&error);

	/* TODO: Tests? */
	g_main_loop_quit (main_loop);

	g_object_unref (feed);
}

static void
test_query_all_calendars_async (void)
{
	g_assert (service != NULL);

	gdata_calendar_service_query_all_calendars_async (GDATA_CALENDAR_SERVICE (service), NULL, NULL, NULL,
							  NULL, (GAsyncReadyCallback) test_query_all_calendars_async_cb, NULL);

	main_loop = g_main_loop_new (NULL, TRUE);
	g_main_loop_run (main_loop);
	g_main_loop_unref (main_loop);
}

static void
test_query_own_calendars (void)
{
	GDataFeed *feed;
	GError *error = NULL;

	g_assert (service != NULL);

	feed = gdata_calendar_service_query_own_calendars (GDATA_CALENDAR_SERVICE (service), NULL, NULL, NULL, NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (feed));
	g_clear_error (&error);

	/* TODO: check entries and feed properties */

	g_object_unref (feed);
}

static void
test_query_own_calendars_async_cb (GDataService *service, GAsyncResult *async_result, gpointer user_data)
{
	GDataFeed *feed;
	GError *error = NULL;

	feed = gdata_service_query_finish (service, async_result, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (feed));
	g_clear_error (&error);

	/* TODO: Tests? */
	g_main_loop_quit (main_loop);

	g_object_unref (feed);
}

static void
test_query_own_calendars_async (void)
{
	g_assert (service != NULL);

	gdata_calendar_service_query_own_calendars_async (GDATA_CALENDAR_SERVICE (service), NULL, NULL, NULL,
							  NULL, (GAsyncReadyCallback) test_query_own_calendars_async_cb, NULL);

	main_loop = g_main_loop_new (NULL, TRUE);
	g_main_loop_run (main_loop);
	g_main_loop_unref (main_loop);
}

static void
test_query_events (void)
{
	GDataFeed *feed, *calendar_feed;
	GDataCalendarCalendar *calendar;
	GList *calendars;
	GError *error = NULL;

	g_assert (service != NULL);

	/* Get a calendar */
	calendar_feed = gdata_calendar_service_query_own_calendars (GDATA_CALENDAR_SERVICE (service), NULL, NULL, NULL, NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (calendar_feed));
	g_clear_error (&error);

	calendars = gdata_feed_get_entries (calendar_feed);
	g_assert (calendars != NULL);
	calendar = calendars->data;
	g_assert (GDATA_IS_CALENDAR_CALENDAR (calendar));

	g_object_ref (calendar);
	g_object_unref (calendar_feed);

	/* Get the entry feed */
	feed = gdata_calendar_service_query_events (GDATA_CALENDAR_SERVICE (service), calendar, NULL, NULL, NULL, NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (feed));
	g_clear_error (&error);

	/* TODO: check entries and feed properties */

	g_object_unref (feed);
	g_object_unref (calendar);
}

static void
test_insert_simple (void)
{
	GDataCalendarEvent *event, *new_event;
	GDataCategory *category;
	GDataGDWhere *where;
	GDataGDWho *who;
	GDataGDWhen *when;
	GTimeVal start_time, end_time;
	gchar *xml;
	GError *error = NULL;

	g_assert (service != NULL);

	event = gdata_calendar_event_new (NULL);

	gdata_entry_set_title (GDATA_ENTRY (event), "Tennis with Beth");
	gdata_entry_set_content (GDATA_ENTRY (event), "Meet for a quick lesson.");
	category = gdata_category_new ("http://schemas.google.com/g/2005#event", "http://schemas.google.com/g/2005#kind", NULL);
	gdata_entry_add_category (GDATA_ENTRY (event), category);
	gdata_calendar_event_set_transparency (event, "http://schemas.google.com/g/2005#event.opaque");
	gdata_calendar_event_set_status (event, "http://schemas.google.com/g/2005#event.confirmed");
	where = gdata_gd_where_new (NULL, "Rolling Lawn Courts", NULL);
	gdata_calendar_event_add_place (event, where);
	who = gdata_gd_who_new ("http://schemas.google.com/g/2005#event.organizer", "John Smith‽", "john.smith@example.com");
	gdata_calendar_event_add_person (event, who);
	g_time_val_from_iso8601 ("2009-04-17T15:00:00.000Z", &start_time);
	g_time_val_from_iso8601 ("2009-04-17T17:00:00.000Z", &end_time);
	when = gdata_gd_when_new (&start_time, &end_time, FALSE, NULL, NULL);
	gdata_calendar_event_add_time (event, when);

	/* Check the XML */
	xml = gdata_entry_get_xml (GDATA_ENTRY (event));
	g_assert_cmpstr (xml, ==,
			 "<entry xmlns='http://www.w3.org/2005/Atom' "
			 	"xmlns:gd='http://schemas.google.com/g/2005' "
			 	"xmlns:gCal='http://schemas.google.com/gCal/2005' "
			 	"xmlns:app='http://www.w3.org/2007/app'>"
			 	"<title type='text'>Tennis with Beth</title>"
			 	"<content type='text'>Meet for a quick lesson.</content>"
			 	"<category term='http://schemas.google.com/g/2005#event' scheme='http://schemas.google.com/g/2005#kind'/>"
			 	"<gd:eventStatus value='http://schemas.google.com/g/2005#event.confirmed'/>"
			 	"<gd:transparency value='http://schemas.google.com/g/2005#event.opaque'/>"
			 	"<gCal:guestsCanModify value='false'/>"
			 	"<gCal:guestsCanInviteOthers value='false'/>"
			 	"<gCal:guestsCanSeeGuests value='false'/>"
			 	"<gCal:anyoneCanAddSelf value='false'/>"
				"<gd:when startTime='2009-04-17T15:00:00Z' endTime='2009-04-17T17:00:00Z'/>"
			 	"<gd:who email='john.smith@example.com' "
			 		"rel='http://schemas.google.com/g/2005#event.organizer' "
			 		"valueString='John Smith\342\200\275'/>"
			 	"<gd:where valueString='Rolling Lawn Courts'/>"
			 "</entry>");
	g_free (xml);

	/* Insert the event */
	new_event = gdata_calendar_service_insert_event (GDATA_CALENDAR_SERVICE (service), event, NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_CALENDAR_EVENT (new_event));
	g_clear_error (&error);

	/* TODO: check entries and feed properties */

	g_object_unref (event);
	g_object_unref (new_event);
}

static void
test_xml_dates (void)
{
	GDataCalendarEvent *event;
	GList *times, *i;
	GDataGDWhen *when;
	gchar *xml;
	GError *error = NULL;

	event = gdata_calendar_event_new_from_xml (
		"<entry xmlns='http://www.w3.org/2005/Atom' "
		 	"xmlns:gd='http://schemas.google.com/g/2005' "
		 	"xmlns:gCal='http://schemas.google.com/gCal/2005' "
		 	"xmlns:app='http://www.w3.org/2007/app'>"
		 	"<title type='text'>Tennis with Beth</title>"
		 	"<content type='text'>Meet for a quick lesson.</content>"
		 	"<category term='http://schemas.google.com/g/2005#event' scheme='http://schemas.google.com/g/2005#kind'/>"
		 	"<gd:when startTime='2009-04-17'/>"
		 	"<gd:when startTime='2009-04-17T15:00:00Z'/>"
		 	"<gd:when startTime='2009-04-27' endTime='20090506'/>"
		 "</entry>", -1, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_ENTRY (event));
	g_clear_error (&error);

	/* Check the times */
	times = i = gdata_calendar_event_get_times (event);

	/* First time */
	when = (GDataGDWhen*) i->data;
	g_assert (i->next != NULL);
	g_assert (when->is_date == TRUE);
	g_assert_cmpint (when->start_time.tv_sec, ==, 1239926400);
	g_assert_cmpint (when->start_time.tv_usec, ==, 0);
	g_assert_cmpint (when->end_time.tv_sec, ==, 0);
	g_assert_cmpint (when->end_time.tv_usec, ==, 0);
	g_assert (when->value_string == NULL);
	g_assert (when->reminders == NULL);

	/* Second time */
	i = i->next;
	when = (GDataGDWhen*) i->data;
	g_assert (i->next != NULL);
	g_assert (when->is_date == FALSE);
	g_assert_cmpint (when->start_time.tv_sec, ==, 1239926400 + 54000);
	g_assert_cmpint (when->start_time.tv_usec, ==, 0);
	g_assert_cmpint (when->end_time.tv_sec, ==, 0);
	g_assert_cmpint (when->end_time.tv_usec, ==, 0);
	g_assert (when->value_string == NULL);
	g_assert (when->reminders == NULL);

	/* Third time */
	i = i->next;
	when = (GDataGDWhen*) i->data;
	g_assert (i->next == NULL);
	g_assert (when->is_date == TRUE);
	g_assert_cmpint (when->start_time.tv_sec, ==, 1239926400 + 864000);
	g_assert_cmpint (when->start_time.tv_usec, ==, 0);
	g_assert_cmpint (when->end_time.tv_sec, ==, 1241568000);
	g_assert_cmpint (when->end_time.tv_usec, ==, 0);
	g_assert (when->value_string == NULL);
	g_assert (when->reminders == NULL);

	/* Check the XML */
	xml = gdata_entry_get_xml (GDATA_ENTRY (event));
	g_assert_cmpstr (xml, ==,
			 "<entry xmlns='http://www.w3.org/2005/Atom' "
			 	"xmlns:gd='http://schemas.google.com/g/2005' "
			 	"xmlns:gCal='http://schemas.google.com/gCal/2005' "
			 	"xmlns:app='http://www.w3.org/2007/app'>"
			 	"<title type='text'>Tennis with Beth</title>"
			 	"<content type='text'>Meet for a quick lesson.</content>"
			 	"<category term='http://schemas.google.com/g/2005#event' scheme='http://schemas.google.com/g/2005#kind'/>"
			 	"<gCal:guestsCanModify value='false'/>"
				"<gCal:guestsCanInviteOthers value='false'/>"
				"<gCal:guestsCanSeeGuests value='false'/>"
				"<gCal:anyoneCanAddSelf value='false'/>"
			 	"<gd:when startTime='2009-04-17'/>"
			 	"<gd:when startTime='2009-04-17T15:00:00Z'/>"
			 	"<gd:when startTime='2009-04-27' endTime='2009-05-06'/>"
			 "</entry>");
	g_free (xml);

	g_object_unref (event);
}

static void
test_xml_recurrence (void)
{
	GDataCalendarEvent *event;
	gchar *xml;
	GError *error = NULL;
	gchar *id, *uri;

	event = gdata_calendar_event_new_from_xml (
		"<entry xmlns='http://www.w3.org/2005/Atom' "
		 	"xmlns:gd='http://schemas.google.com/g/2005' "
		 	"xmlns:gCal='http://schemas.google.com/gCal/2005' "
		 	"xmlns:app='http://www.w3.org/2007/app'>"
			"<id>http://www.google.com/calendar/feeds/libgdata.test@googlemail.com/events/g5928e82rrch95b25f8ud0dlsg_20090429T153000Z</id>"
			"<published>2009-04-25T15:22:47.000Z</published>"
			"<updated>2009-04-27T17:54:10.000Z</updated>"
			"<app:edited xmlns:app='http://www.w3.org/2007/app'>2009-04-27T17:54:10.000Z</app:edited>"
			"<category scheme='http://schemas.google.com/g/2005#kind' term='http://schemas.google.com/g/2005#event'/>"
			"<title>Test daily instance event</title>"
			"<content></content>"
			"<link rel='alternate' type='text/html' href='http://www.google.com/calendar/event?eid=ZzU5MjhlODJycmNoOTViMjVmOHVkMGRsc2dfMjAwOTA0MjlUMTUzMDAwWiBsaWJnZGF0YS50ZXN0QGdvb2dsZW1haWwuY29t' title='alternate'/>"
			"<link rel='self' type='application/atom+xml' href='http://www.google.com/calendar/feeds/libgdata.test@googlemail.com/private/full/g5928e82rrch95b25f8ud0dlsg_20090429T153000Z'/>"
			"<link rel='edit' type='application/atom+xml' href='http://www.google.com/calendar/feeds/libgdata.test@googlemail.com/private/full/g5928e82rrch95b25f8ud0dlsg_20090429T153000Z'/>"
			"<author>"
				"<name>GData Test</name>"
				"<email>libgdata.test@googlemail.com</email>"
			"</author>"
			"<gd:originalEvent id='g5928e82rrch95b25f8ud0dlsg' href='http://www.google.com/calendar/feeds/libgdata.test@googlemail.com/private/full/g5928e82rrch95b25f8ud0dlsg'>"
				"<gd:when startTime='2009-04-29T16:30:00.000+01:00'/>"
			"</gd:originalEvent>"
			"<gCal:guestsCanModify value='false'/>"
			"<gCal:guestsCanInviteOthers value='false'/>"
			"<gCal:guestsCanSeeGuests value='false'/>"
			"<gCal:anyoneCanAddSelf value='false'/>"
			"<gd:comments>"
				"<gd:feedLink href='http://www.google.com/calendar/feeds/libgdata.test@googlemail.com/private/full/g5928e82rrch95b25f8ud0dlsg_20090429T153000Z/comments'/>"
			"</gd:comments>"
			"<gd:eventStatus value='http://schemas.google.com/g/2005#event.confirmed'/>"
			"<gd:visibility value='http://schemas.google.com/g/2005#event.private'/>"
			"<gd:transparency value='http://schemas.google.com/g/2005#event.opaque'/>"
			"<gCal:uid value='g5928e82rrch95b25f8ud0dlsg@google.com'/>"
			"<gCal:sequence value='0'/>"
			"<gd:when startTime='2009-04-29T17:30:00.000+01:00' endTime='2009-04-29T17:30:00.000+01:00'>"
				"<gd:reminder minutes='10' method='email'/>"
				"<gd:reminder minutes='10' method='alert'/>"
			"</gd:when>"
			"<gd:who rel='http://schemas.google.com/g/2005#event.organizer' valueString='GData Test' email='libgdata.test@googlemail.com'/>"
			"<gd:where valueString=''/>"
		"</entry>", -1, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_ENTRY (event));
	g_clear_error (&error);

	/* Check the original event */
	g_assert (gdata_calendar_event_is_exception (event) == TRUE);

	gdata_calendar_event_get_original_event_details (event, &id, &uri);
	g_assert_cmpstr (id, ==, "g5928e82rrch95b25f8ud0dlsg");
	g_assert_cmpstr (uri, ==, "http://www.google.com/calendar/feeds/libgdata.test@googlemail.com/private/full/g5928e82rrch95b25f8ud0dlsg");

	g_object_unref (event);
}

static void
test_query_uri (void)
{
	GTimeVal time_val, time_val2;
	gchar *query_uri;
	GDataCalendarQuery *query = gdata_calendar_query_new ("q");

	gdata_calendar_query_set_future_events (query, TRUE);
	g_assert (gdata_calendar_query_get_future_events (query) == TRUE);

	gdata_calendar_query_set_order_by (query, "starttime");
	g_assert_cmpstr (gdata_calendar_query_get_order_by (query), ==, "starttime");

	g_time_val_from_iso8601 ("2009-04-17T15:00:00.000Z", &time_val);
	gdata_calendar_query_set_recurrence_expansion_start (query, &time_val);
	gdata_calendar_query_get_recurrence_expansion_start (query, &time_val2);
	g_assert_cmpint (time_val.tv_sec, ==, time_val2.tv_sec);
	g_assert_cmpint (time_val.tv_usec, ==, time_val2.tv_usec);

	g_time_val_from_iso8601 ("2010-04-17T15:00:00.000Z", &time_val);
	gdata_calendar_query_set_recurrence_expansion_end (query, &time_val);
	gdata_calendar_query_get_recurrence_expansion_end (query, &time_val2);
	g_assert_cmpint (time_val.tv_sec, ==, time_val2.tv_sec);
	g_assert_cmpint (time_val.tv_usec, ==, time_val2.tv_usec);

	gdata_calendar_query_set_single_events (query, TRUE);
	g_assert (gdata_calendar_query_get_single_events (query) == TRUE);

	gdata_calendar_query_set_sort_order (query, "descending");
	g_assert_cmpstr (gdata_calendar_query_get_sort_order (query), ==, "descending");

	g_time_val_from_iso8601 ("2009-04-17T15:00:00.000Z", &time_val);
	gdata_calendar_query_set_start_min (query, &time_val);
	gdata_calendar_query_get_start_min (query, &time_val2);
	g_assert_cmpint (time_val.tv_sec, ==, time_val2.tv_sec);
	g_assert_cmpint (time_val.tv_usec, ==, time_val2.tv_usec);

	g_time_val_from_iso8601 ("2010-04-17T15:00:00.000Z", &time_val);
	gdata_calendar_query_set_start_max (query, &time_val);
	gdata_calendar_query_get_start_max (query, &time_val2);
	g_assert_cmpint (time_val.tv_sec, ==, time_val2.tv_sec);
	g_assert_cmpint (time_val.tv_usec, ==, time_val2.tv_usec);

	gdata_calendar_query_set_timezone (query, "America/Los Angeles");
	g_assert_cmpstr (gdata_calendar_query_get_timezone (query), ==, "America/Los_Angeles");

	/* Check the built query URI with a normal feed URI */
	query_uri = gdata_query_get_query_uri (GDATA_QUERY (query), "http://example.com");
	g_assert_cmpstr (query_uri, ==, "http://example.com?q=q&futureevents=true&orderby=starttime&recurrence-expansion-start=2009-04-17T15:00:00Z"
					"&recurrence-expansion-end=2010-04-17T15:00:00Z&singleevents=true&sortorder=descending"
					"&start-min=2009-04-17T15:00:00Z&start-max=2010-04-17T15:00:00Z&ctz=America%2FLos_Angeles");
	g_free (query_uri);

	/* …with a feed URI with a trailing slash */
	query_uri = gdata_query_get_query_uri (GDATA_QUERY (query), "http://example.com/");
	g_assert_cmpstr (query_uri, ==, "http://example.com/?q=q&futureevents=true&orderby=starttime&recurrence-expansion-start=2009-04-17T15:00:00Z"
					"&recurrence-expansion-end=2010-04-17T15:00:00Z&singleevents=true&sortorder=descending"
					"&start-min=2009-04-17T15:00:00Z&start-max=2010-04-17T15:00:00Z&ctz=America%2FLos_Angeles");
	g_free (query_uri);

	/* …with a feed URI with pre-existing arguments */
	query_uri = gdata_query_get_query_uri (GDATA_QUERY (query), "http://example.com/bar/?test=test&this=that");
	g_assert_cmpstr (query_uri, ==, "http://example.com/bar/?test=test&this=that&q=q&futureevents=true&orderby=starttime"
					"&recurrence-expansion-start=2009-04-17T15:00:00Z&recurrence-expansion-end=2010-04-17T15:00:00Z"
					"&singleevents=true&sortorder=descending&start-min=2009-04-17T15:00:00Z&start-max=2010-04-17T15:00:00Z"
					"&ctz=America%2FLos_Angeles");
	g_free (query_uri);

	g_object_unref (query);
}

int
main (int argc, char *argv[])
{
	gint retval;

	g_type_init ();
	g_thread_init (NULL);
	g_test_init (&argc, &argv, NULL);
	g_test_bug_base ("http://bugzilla.gnome.org/show_bug.cgi?id=");

	g_test_add_func ("/calendar/authentication", test_authentication);
	if (g_test_thorough () == TRUE)
		g_test_add_func ("/calendar/authentication_async", test_authentication_async);
	g_test_add_func ("/calendar/query/all_calendars", test_query_all_calendars);
	if (g_test_thorough () == TRUE)
		g_test_add_func ("/calendar/query/all_calendars_async", test_query_all_calendars_async);
	g_test_add_func ("/calendar/query/own_calendars", test_query_own_calendars);
	if (g_test_thorough () == TRUE)
		g_test_add_func ("/calendar/query/own_calendars_async", test_query_own_calendars_async);
	g_test_add_func ("/calendar/query/events", test_query_events);
	if (g_test_slow () == TRUE)
		g_test_add_func ("/calendar/insert/simple", test_insert_simple);
	g_test_add_func ("/calendar/xml/dates", test_xml_dates);
	g_test_add_func ("/calendar/xml/recurrence", test_xml_recurrence);
	g_test_add_func ("/calendar/query/uri", test_query_uri);

	retval = g_test_run ();
	if (service != NULL)
		g_object_unref (service);

	return retval;
}
