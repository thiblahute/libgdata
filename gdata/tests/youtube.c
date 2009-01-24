/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Philip Withnall 2008 <philip@tecnocode.co.uk>
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

#include <glib.h>
#include <unistd.h>

#include "gdata.h"

#define DEVELOPER_KEY	"AI39si7Me3Q7zYs6hmkFvpRBD2nrkVjYYsUO5lh_3HdOkGRc9g6Z4nzxZatk_aAo2EsA21k7vrda0OO6oFg2rnhMedZXPyXoEw"
#define CLIENT_ID	"ytapi-GNOME-libgdata-444fubtt-0"

/* TODO: probably a better way to do this; some kind of data associated with the test suite? */
static GDataService *service = NULL;
static GMainLoop *main_loop = NULL;

static void
test_authentication (void)
{
	gboolean retval;
	GError *error = NULL;

	/* Create a service */
	service = GDATA_SERVICE (gdata_youtube_service_new (DEVELOPER_KEY, CLIENT_ID));

	g_assert (service != NULL);
	g_assert (GDATA_IS_SERVICE (service));
	g_assert_cmpstr (gdata_service_get_client_id (service), ==, CLIENT_ID);
	g_assert_cmpstr (gdata_youtube_service_get_developer_key (GDATA_YOUTUBE_SERVICE (service)), ==, DEVELOPER_KEY);

	/* Log in */
	retval = gdata_service_authenticate (service, "GDataTest", "gdata", NULL, &error);
	g_assert_no_error (error);
	g_assert (retval == TRUE);
	g_clear_error (&error);

	/* Check all is as it should be */
	g_assert (gdata_service_is_logged_in (service) == TRUE);
	g_assert_cmpstr (gdata_youtube_service_get_youtube_user (GDATA_YOUTUBE_SERVICE (service)), ==, "GDataTest");
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
	g_assert (gdata_service_is_logged_in (service) == TRUE);
	g_assert_cmpstr (gdata_youtube_service_get_youtube_user (GDATA_YOUTUBE_SERVICE (service)), ==, "GDataTest");
}

static void
test_authentication_async (void)
{
	/* Create a service */
	service = GDATA_SERVICE (gdata_youtube_service_new (DEVELOPER_KEY, CLIENT_ID));

	g_assert (service != NULL);
	g_assert (GDATA_IS_SERVICE (service));

	gdata_service_authenticate_async (service, "GDataTest", "gdata", NULL, (GAsyncReadyCallback) test_authentication_async_cb, NULL);

	main_loop = g_main_loop_new (NULL, TRUE);
	g_main_loop_run (main_loop);
	g_main_loop_unref (main_loop);
}

static void
test_query_standard_feed (void)
{
	GDataFeed *feed;
	GError *error = NULL;

	g_assert (service != NULL);

	feed = gdata_youtube_service_query_standard_feed (GDATA_YOUTUBE_SERVICE (service), GDATA_YOUTUBE_TOP_RATED_FEED, NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (feed));
	g_clear_error (&error);

	/* TODO: check entries and feed properties */

	g_object_unref (feed);
}

static void
test_query_standard_feed_async_cb (GDataService *service, GAsyncResult *async_result, gpointer user_data)
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
test_query_standard_feed_async (void)
{
	g_assert (service != NULL);

	gdata_youtube_service_query_standard_feed_async (GDATA_YOUTUBE_SERVICE (service), GDATA_YOUTUBE_TOP_RATED_FEED,
							 NULL, (GAsyncReadyCallback) test_query_standard_feed_async_cb, NULL);

	main_loop = g_main_loop_new (NULL, TRUE);
	g_main_loop_run (main_loop);
	g_main_loop_unref (main_loop);
}

int
main (int argc, char *argv[])
{
	gint retval;

	g_type_init ();
	g_thread_init (NULL);
	g_test_init (&argc, &argv, NULL);
	g_test_bug_base ("http://bugzilla.gnome.org/show_bug.cgi?id=");

	g_test_add_func ("/youtube/authentication", test_authentication);
	if (g_test_thorough () == TRUE)
		g_test_add_func ("/youtube/authentication_async", test_authentication_async);
	g_test_add_func ("/youtube/query/standard_feed", test_query_standard_feed);
	if (g_test_thorough () == TRUE)
		g_test_add_func ("/youtube/query/standard_feed_async", test_query_standard_feed_async);

	retval = g_test_run ();
	if (service != NULL)
		g_object_unref (service);

	return retval;
}
