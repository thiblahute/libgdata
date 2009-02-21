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

	feed = gdata_youtube_service_query_standard_feed (GDATA_YOUTUBE_SERVICE (service), GDATA_YOUTUBE_TOP_RATED_FEED, -1, -1, NULL, &error);
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

	gdata_youtube_service_query_standard_feed_async (GDATA_YOUTUBE_SERVICE (service), GDATA_YOUTUBE_TOP_RATED_FEED, -1, -1,
							 NULL, (GAsyncReadyCallback) test_query_standard_feed_async_cb, NULL);

	main_loop = g_main_loop_new (NULL, TRUE);
	g_main_loop_run (main_loop);
	g_main_loop_unref (main_loop);
}

static GDataYouTubeVideo *
get_video_for_related (void)
{
	GDataYouTubeVideo *video;
	GError *error = NULL;

	video = gdata_youtube_video_new_from_xml (
		"<entry xmlns='http://www.w3.org/2005/Atom' "
			"xmlns:media='http://search.yahoo.com/mrss/' "
			"xmlns:yt='http://gdata.youtube.com/schemas/2007' "
			"xmlns:georss='http://www.georss.org/georss' "
			"xmlns:gd='http://schemas.google.com/g/2005' "
			"xmlns:gml='http://www.opengis.net/gml'>"
			"<id>http://gdata.youtube.com/feeds/api/videos/q1UPMEmCqZo</id>"
			"<published>2009-02-12T20:34:08.000Z</published>"
			"<updated>2009-02-21T13:00:13.000Z</updated>"
			"<category scheme='http://gdata.youtube.com/schemas/2007/keywords.cat' term='part one'/>"
			"<category scheme='http://gdata.youtube.com/schemas/2007/categories.cat' term='Film' label='Film &amp; Animation'/>"
			"<category scheme='http://schemas.google.com/g/2005#kind' term='http://gdata.youtube.com/schemas/2007#video'/>"
			"<category scheme='http://gdata.youtube.com/schemas/2007/keywords.cat' term='ian purchase'/>"
			"<category scheme='http://gdata.youtube.com/schemas/2007/keywords.cat' term='purchase brothers'/>"
			"<category scheme='http://gdata.youtube.com/schemas/2007/keywords.cat' term='half life 2'/>"
			"<category scheme='http://gdata.youtube.com/schemas/2007/keywords.cat' term='escape from city 17'/>"
			"<category scheme='http://gdata.youtube.com/schemas/2007/keywords.cat' term='Half Life'/>"
			"<category scheme='http://gdata.youtube.com/schemas/2007/keywords.cat' term='david purchase'/>"
			"<category scheme='http://gdata.youtube.com/schemas/2007/keywords.cat' term='half-life'/>"
			"<title type='text'>Escape From City 17 - Part One</title>"
			"<content type='text'>Directed by The Purchase Brothers. *snip*</content>"
			"<link rel='alternate' type='text/html' href='http://www.youtube.com/watch?v=q1UPMEmCqZo'/>"
			"<link rel='http://gdata.youtube.com/schemas/2007#video.related' type='application/atom+xml' href='http://gdata.youtube.com/feeds/api/videos/q1UPMEmCqZo/related'/>"
			"<link rel='http://gdata.youtube.com/schemas/2007#mobile' type='text/html' href='http://m.youtube.com/details?v=q1UPMEmCqZo'/>"
			"<link rel='self' type='application/atom+xml' href='http://gdata.youtube.com/feeds/api/standardfeeds/top_rated/v/q1UPMEmCqZo'/>"
			"<author>"
				"<name>PurchaseBrothers</name>"
				"<uri>http://gdata.youtube.com/feeds/api/users/purchasebrothers</uri>"
			"</author>"
			"<media:group>"
				"<media:title type='plain'>Escape From City 17 - Part One</media:title>"
				"<media:description type='plain'>Directed by The Purchase Brothers. *snip*</media:description>"
				"<media:keywords>Half Life, escape from city 17, half-life, half life 2, part one, purchase brothers, david purchase, ian purchase</media:keywords>"
				"<yt:duration seconds='330'/>"
				"<media:category label='Film &amp; Animation' scheme='http://gdata.youtube.com/schemas/2007/categories.cat'>Film</media:category>"
				"<media:content url='http://www.youtube.com/v/q1UPMEmCqZo&amp;f=standard&amp;app=youtube_gdata' type='application/x-shockwave-flash' medium='video' isDefault='true' expression='full' duration='330' yt:format='5'/>"
				"<media:content url='rtsp://rtsp2.youtube.com/CiQLENy73wIaGwmaqYJJMA9VqxMYDSANFEgGUghzdGFuZGFyZAw=/0/0/0/video.3gp' type='video/3gpp' medium='video' expression='full' duration='330' yt:format='1'/>"
				"<media:content url='rtsp://rtsp2.youtube.com/CiQLENy73wIaGwmaqYJJMA9VqxMYESARFEgGUghzdGFuZGFyZAw=/0/0/0/video.3gp' type='video/3gpp' medium='video' expression='full' duration='330' yt:format='6'/>"
				"<media:thumbnail url='http://i.ytimg.com/vi/q1UPMEmCqZo/2.jpg' height='97' width='130' time='00:02:45'/>"
				"<media:thumbnail url='http://i.ytimg.com/vi/q1UPMEmCqZo/1.jpg' height='97' width='130' time='00:01:22.500'/>"
				"<media:thumbnail url='http://i.ytimg.com/vi/q1UPMEmCqZo/3.jpg' height='97' width='130' time='00:04:07.500'/>"
				"<media:thumbnail url='http://i.ytimg.com/vi/q1UPMEmCqZo/0.jpg' height='240' width='320' time='00:02:45'/>"
				"<media:player url='http://www.youtube.com/watch?v=q1UPMEmCqZo'/>"
			"</media:group>"
			"<yt:statistics viewCount='1683289' favoriteCount='29963'/>"
			"<gd:rating min='1' max='5' numRaters='24550' average='4.95'/>"
			"<georss:where>"
				"<gml:Point>"
					"<gml:pos>43.661911057260674 -79.37759399414062</gml:pos>"
				"</gml:Point>"
			"</georss:where>"
			"<gd:comments>"
				"<gd:feedLink href='http://gdata.youtube.com/feeds/api/videos/q1UPMEmCqZo/comments' countHint='13021'/>"
			"</gd:comments>"
		"</entry>", -1, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_YOUTUBE_VIDEO (video));
	g_clear_error (&error);

	return video;
}

static void
test_query_related (void)
{
	GDataFeed *feed;
	GDataYouTubeVideo *video;
	GError *error = NULL;

	g_assert (service != NULL);

	video = get_video_for_related ();
	feed = gdata_youtube_service_query_related (GDATA_YOUTUBE_SERVICE (service), video, -1, -1, NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (feed));
	g_clear_error (&error);

	/* TODO: check entries and feed properties */

	g_object_unref (video);
	g_object_unref (feed);
}

static void
test_query_related_async_cb (GDataService *service, GAsyncResult *async_result, gpointer user_data)
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
test_query_related_async (void)
{
	GDataYouTubeVideo *video;

	g_assert (service != NULL);

	video = get_video_for_related ();
	gdata_youtube_service_query_related_async (GDATA_YOUTUBE_SERVICE (service), video, -1, -1,
						   NULL, (GAsyncReadyCallback) test_query_related_async_cb, NULL);
	g_object_unref (video);

	main_loop = g_main_loop_new (NULL, TRUE);
	g_main_loop_run (main_loop);
	g_main_loop_unref (main_loop);
}

static void
test_upload_simple (void)
{
	GDataYouTubeVideo *video, *new_video;
	GDataMediaCategory *category;
	GFile *video_file;
	gchar *xml;
	GError *error = NULL;

	g_assert (service != NULL);

	video = gdata_youtube_video_new ();

	gdata_entry_set_title (GDATA_ENTRY (video), "Bad Wedding Toast");
	gdata_youtube_video_set_title (video, "Bad Wedding Toast");
	gdata_youtube_video_set_description (video, "I gave a bad toast at my friend's wedding.");
	category = gdata_media_category_new ("People", NULL, "http://gdata.youtube.com/schemas/2007/categories.cat");
	gdata_youtube_video_set_category (video, category);
	gdata_youtube_video_set_keywords (video, "toast, wedding");

	/* Check the XML */
	xml = gdata_entry_get_xml (GDATA_ENTRY (video));
	g_assert_cmpstr (xml, ==,
			 "<entry xmlns='http://www.w3.org/2005/Atom' "
				"xmlns:media='http://search.yahoo.com/mrss/' "
				"xmlns:yt='http://gdata.youtube.com/schemas/2007'>"
			 	"<title type='text'>Bad Wedding Toast</title>"
			 	"<media:group>"
			 		"<media:category scheme='http://gdata.youtube.com/schemas/2007/categories.cat'>People</media:category>"
			 		"<media:title type='plain'>Bad Wedding Toast</media:title>"
			 		"<media:description type='plain'>I gave a bad toast at my friend&apos;s wedding.</media:description>"
			 		"<media:keywords>toast, wedding</media:keywords>"
			 		"<yt:duration seconds='0'/>"
			 	"</media:group>"
			 "</entry>");
	g_free (xml);

	/* TODO: fix the path */
	video_file = g_file_new_for_path ("/home/philip/Development/libgdata/gdata/tests/sample.ogg");

	/* Upload the video */
	new_video = gdata_youtube_service_upload_video (GDATA_YOUTUBE_SERVICE (service), video, video_file, NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_YOUTUBE_VIDEO (new_video));
	g_clear_error (&error);

	/* TODO: check entries and feed properties */

	g_object_unref (video);
	g_object_unref (new_video);
	g_object_unref (video_file);
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
	g_test_add_func ("/youtube/query/related", test_query_related);
	if (g_test_thorough () == TRUE)
		g_test_add_func ("/youtube/query/related_async", test_query_related_async);
	g_test_add_func ("/youtube/upload/simple", test_upload_simple);

	retval = g_test_run ();
	if (service != NULL)
		g_object_unref (service);

	return retval;
}
