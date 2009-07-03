/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Philip Withnall 2008-2009 <philip@tecnocode.co.uk>
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

#define DEVELOPER_KEY "AI39si7Me3Q7zYs6hmkFvpRBD2nrkVjYYsUO5lh_3HdOkGRc9g6Z4nzxZatk_aAo2EsA21k7vrda0OO6oFg2rnhMedZXPyXoEw"
#define YT_USERNAME "GDataTest"
#define YT_PASSWORD "gdata"

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
	retval = gdata_service_authenticate (service, YT_USERNAME, YT_PASSWORD, NULL, &error);
	g_assert_no_error (error);
	g_assert (retval == TRUE);
	g_clear_error (&error);

	/* Check all is as it should be */
	g_assert (gdata_service_is_authenticated (service) == TRUE);
	g_assert_cmpstr (gdata_service_get_username (service), ==, YT_USERNAME "@gmail.com");
	g_assert_cmpstr (gdata_service_get_password (service), ==, YT_PASSWORD);
	g_assert_cmpstr (gdata_youtube_service_get_youtube_user (GDATA_YOUTUBE_SERVICE (service)), ==, YT_USERNAME);
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
	g_assert_cmpstr (gdata_service_get_username (service), ==, YT_USERNAME "@gmail.com");
	g_assert_cmpstr (gdata_service_get_password (service), ==, YT_PASSWORD);
	g_assert_cmpstr (gdata_youtube_service_get_youtube_user (GDATA_YOUTUBE_SERVICE (service)), ==, YT_USERNAME);
}

static void
test_authentication_async (void)
{
	/* Create a service */
	service = GDATA_SERVICE (gdata_youtube_service_new (DEVELOPER_KEY, CLIENT_ID));

	g_assert (service != NULL);
	g_assert (GDATA_IS_SERVICE (service));

	gdata_service_authenticate_async (service, YT_USERNAME, YT_PASSWORD, NULL, (GAsyncReadyCallback) test_authentication_async_cb, NULL);

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

	feed = gdata_youtube_service_query_standard_feed (GDATA_YOUTUBE_SERVICE (service), GDATA_YOUTUBE_TOP_RATED_FEED, NULL, NULL, NULL, NULL, &error);
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

	gdata_youtube_service_query_standard_feed_async (GDATA_YOUTUBE_SERVICE (service), GDATA_YOUTUBE_TOP_RATED_FEED, NULL,
							 NULL, NULL, NULL, (GAsyncReadyCallback) test_query_standard_feed_async_cb, NULL);

	main_loop = g_main_loop_new (NULL, TRUE);
	g_main_loop_run (main_loop);
	g_main_loop_unref (main_loop);
}

static GDataYouTubeVideo *
get_video_for_related (void)
{
	GDataYouTubeVideo *video;
	GError *error = NULL;

	video = GDATA_YOUTUBE_VIDEO (gdata_parsable_new_from_xml (GDATA_TYPE_YOUTUBE_VIDEO,
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
			"<link rel='http://www.iana.org/assignments/relation/alternate' type='text/html' href='http://www.youtube.com/watch?v=q1UPMEmCqZo'/>"
			"<link rel='http://gdata.youtube.com/schemas/2007#video.related' type='application/atom+xml' href='http://gdata.youtube.com/feeds/api/videos/q1UPMEmCqZo/related'/>"
			"<link rel='http://gdata.youtube.com/schemas/2007#mobile' type='text/html' href='http://m.youtube.com/details?v=q1UPMEmCqZo'/>"
			"<link rel='http://www.iana.org/assignments/relation/self' type='application/atom+xml' href='http://gdata.youtube.com/feeds/api/standardfeeds/top_rated/v/q1UPMEmCqZo'/>"
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
		"</entry>", -1, &error));
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
	feed = gdata_youtube_service_query_related (GDATA_YOUTUBE_SERVICE (service), video, NULL, NULL, NULL, NULL, &error);
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
	gdata_youtube_service_query_related_async (GDATA_YOUTUBE_SERVICE (service), video, NULL, NULL, NULL,
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

	video = gdata_youtube_video_new (NULL);

	gdata_entry_set_title (GDATA_ENTRY (video), "Bad Wedding Toast");
	gdata_youtube_video_set_title (video, "Bad Wedding Toast");
	gdata_youtube_video_set_description (video, "I gave a bad toast at my friend's wedding.");
	category = gdata_media_category_new ("People", NULL, "http://gdata.youtube.com/schemas/2007/categories.cat");
	gdata_youtube_video_set_category (video, category);
	g_object_unref (category);
	gdata_youtube_video_set_keywords (video, "toast, wedding");

	/* Check the XML */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (video));
	g_assert_cmpstr (xml, ==,
			 "<entry xmlns='http://www.w3.org/2005/Atom' "
				"xmlns:media='http://search.yahoo.com/mrss/' "
				"xmlns:gd='http://schemas.google.com/g/2005' "
				"xmlns:yt='http://gdata.youtube.com/schemas/2007'>"
			 	"<title type='text'>Bad Wedding Toast</title>"
			 	"<media:group>"
			 		"<media:category scheme='http://gdata.youtube.com/schemas/2007/categories.cat'>People</media:category>"
			 		"<media:title type='plain'>Bad Wedding Toast</media:title>"
			 		"<media:description type='plain'>I gave a bad toast at my friend&apos;s wedding.</media:description>"
			 		"<media:keywords>toast, wedding</media:keywords>"
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

static void
test_parsing_app_control (void)
{
	GDataYouTubeVideo *video;
	GDataYouTubeState *state;
	GError *error = NULL;

	video = GDATA_YOUTUBE_VIDEO (gdata_parsable_new_from_xml (GDATA_TYPE_YOUTUBE_VIDEO,
		"<entry xmlns='http://www.w3.org/2005/Atom' "
			"xmlns:media='http://search.yahoo.com/mrss/' "
			"xmlns:yt='http://gdata.youtube.com/schemas/2007' "
			"xmlns:gd='http://schemas.google.com/g/2005' "
			"gd:etag='W/\"CEMFSX47eCp7ImA9WxVUGEw.\"'>"
			"<id>tag:youtube.com,2008:video:JAagedeKdcQ</id>"
			"<published>2006-05-16T14:06:37.000Z</published>"
			"<updated>2009-03-23T12:46:58.000Z</updated>"
			"<app:control xmlns:app='http://www.w3.org/2007/app'>"
				"<app:draft>yes</app:draft>"
				"<yt:state name='blacklisted'>This video is not available in your country</yt:state>"
			"</app:control>"
			"<category scheme='http://schemas.google.com/g/2005#kind' term='http://gdata.youtube.com/schemas/2007#video'/>"
			"<title>Judas Priest - Painkiller</title>"
			"<link rel='http://www.iana.org/assignments/relation/alternate' type='text/html' href='http://www.youtube.com/watch?v=JAagedeKdcQ'/>"
			"<link rel='http://www.iana.org/assignments/relation/self' type='application/atom+xml' href='http://gdata.youtube.com/feeds/api/videos/JAagedeKdcQ?client=ytapi-google-jsdemo'/>"
			"<author>"
				"<name>eluves</name>"
				"<uri>http://gdata.youtube.com/feeds/api/users/eluves</uri>"
			"</author>"
			"<media:group>"
				"<media:title type='plain'>Judas Priest - Painkiller</media:title>"
				"<media:credit role='uploader' scheme='urn:youtube'>eluves</media:credit>"
				"<media:category label='Music' scheme='http://gdata.youtube.com/schemas/2007/categories.cat'>Music</media:category>"
			"</media:group>"
		"</entry>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_YOUTUBE_VIDEO (video));
	g_clear_error (&error);

	/* Test the app:control values */
	g_assert (gdata_youtube_video_is_draft (video) == TRUE);

	state = gdata_youtube_video_get_state (video);
	g_assert_cmpstr (gdata_youtube_state_get_name (state), ==, "blacklisted");
	g_assert_cmpstr (gdata_youtube_state_get_message (state), ==, "This video is not available in your country");
	g_assert (gdata_youtube_state_get_reason_code (state) == NULL);
	g_assert (gdata_youtube_state_get_help_uri (state) == NULL);

	/* TODO: more tests on entry properties */

	g_object_unref (video);
}

static void
test_parsing_yt_recorded (void)
{
	GDataYouTubeVideo *video;
	GTimeVal recorded;
	gchar *xml;
	GError *error = NULL;

	video = GDATA_YOUTUBE_VIDEO (gdata_parsable_new_from_xml (GDATA_TYPE_YOUTUBE_VIDEO,
		"<entry xmlns='http://www.w3.org/2005/Atom' "
			"xmlns:media='http://video.search.yahoo.com/mrss' "
			"xmlns:yt='http://gdata.youtube.com/schemas/2007' "
			"xmlns:gd='http://schemas.google.com/g/2005' "
			"gd:etag='W/\"CEMFSX47eCp7ImA9WxVUGEw.\"'>"
			"<id>tag:youtube.com,2008:video:JAagedeKdcQ</id>"
			"<published>2006-05-16T14:06:37.000Z</published>"
			"<updated>2009-03-23T12:46:58.000Z</updated>"
			"<category scheme='http://schemas.google.com/g/2005#kind' term='http://gdata.youtube.com/schemas/2007#video'/>"
			"<title>Judas Priest - Painkiller</title>"
			"<link rel='http://www.iana.org/assignments/relation/alternate' type='text/html' href='http://www.youtube.com/watch?v=JAagedeKdcQ'/>"
			"<link rel='http://www.iana.org/assignments/relation/self' type='application/atom+xml' href='http://gdata.youtube.com/feeds/api/videos/JAagedeKdcQ?client=ytapi-google-jsdemo'/>"
			"<author>"
				"<name>eluves</name>"
				"<uri>http://gdata.youtube.com/feeds/api/users/eluves</uri>"
			"</author>"
			"<media:group>"
				"<media:title type='plain'>Judas Priest - Painkiller</media:title>"
				"<media:credit role='uploader' scheme='urn:youtube'>eluves</media:credit>"
				"<media:category label='Music' scheme='http://gdata.youtube.com/schemas/2007/categories.cat'>Music</media:category>"
			"</media:group>"
			"<yt:recorded>2003-08-03</yt:recorded>"
		"</entry>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_YOUTUBE_VIDEO (video));
	g_clear_error (&error);

	/* Test the recorded date */
	gdata_youtube_video_get_recorded (video, &recorded);
	g_assert_cmpint (recorded.tv_sec, ==, 1059868800);
	g_assert_cmpint (recorded.tv_usec, ==, 0);

	/* Update the state and see if the XML's written out OK */
	recorded.tv_sec = 1128229200;
	gdata_youtube_video_set_recorded (video, &recorded);

	/* Check the XML */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (video));
	g_assert_cmpstr (xml, ==,
			 "<entry xmlns='http://www.w3.org/2005/Atom' "
				"xmlns:media='http://video.search.yahoo.com/mrss' "
				"xmlns:gd='http://schemas.google.com/g/2005' "
				"xmlns:yt='http://gdata.youtube.com/schemas/2007' "
				"xmlns:app='http://www.w3.org/2007/app' "
				"gd:etag='W/\"CEMFSX47eCp7ImA9WxVUGEw.\"'>"
				"<title type='text'>Judas Priest - Painkiller</title>"
				"<id>tag:youtube.com,2008:video:JAagedeKdcQ</id>"
				"<updated>2009-03-23T12:46:58Z</updated>"
				"<published>2006-05-16T14:06:37Z</published>"
				"<category term='http://gdata.youtube.com/schemas/2007#video' scheme='http://schemas.google.com/g/2005#kind'/>"
				"<link href='http://www.youtube.com/watch?v=JAagedeKdcQ' rel='http://www.iana.org/assignments/relation/alternate' type='text/html'/>"
				"<link href='http://gdata.youtube.com/feeds/api/videos/JAagedeKdcQ?client=ytapi-google-jsdemo' rel='http://www.iana.org/assignments/relation/self' type='application/atom+xml'/>"
				"<author>"
					"<name>eluves</name>"
					"<uri>http://gdata.youtube.com/feeds/api/users/eluves</uri>"
				"</author>"
				"<media:group>"
					"<media:category scheme='http://gdata.youtube.com/schemas/2007/categories.cat' label='Music'>Music</media:category>"
					"<media:title type='plain'>Judas Priest - Painkiller</media:title>"
				"</media:group>"
				"<yt:recorded>2005-10-02</yt:recorded>"
				"<app:control>"
					"<app:draft>no</app:draft>"
				"</app:control>"
			 "</entry>");
	g_free (xml);

	/* TODO: more tests on entry properties */

	g_object_unref (video);
}

/*static void
test_parsing_comments_feed_link (void)
{
	GDataYouTubeVideo *video;
	GDataGDFeedLink *feed_link;
	GError *error = NULL;

	video = gdata_parsable_new_from_xml (GDATA_TYPE_YOUTUBE_VIDEO,
		"<entry xmlns='http://www.w3.org/2005/Atom' "
			"xmlns:media='http://search.yahoo.com/mrss/' "
			"xmlns:yt='http://gdata.youtube.com/schemas/2007' "
			"xmlns:gd='http://schemas.google.com/g/2005' "
			"gd:etag='W/\"CEMFSX47eCp7ImA9WxVUGEw.\"'>"
			"<id>tag:youtube.com,2008:video:JAagedeKdcQ</id>"
			"<published>2006-05-16T14:06:37.000Z</published>"
			"<updated>2009-03-23T12:46:58.000Z</updated>"
			"<category scheme='http://schemas.google.com/g/2005#kind' term='http://gdata.youtube.com/schemas/2007#video'/>"
			"<title>Judas Priest - Painkiller</title>"
			"<link rel='http://www.iana.org/assignments/relation/alternate' type='text/html' href='http://www.youtube.com/watch?v=JAagedeKdcQ'/>"
			"<link rel='http://www.iana.org/assignments/relation/self' type='application/atom+xml' href='http://gdata.youtube.com/feeds/api/videos/JAagedeKdcQ?client=ytapi-google-jsdemo'/>"
			"<author>"
				"<name>eluves</name>"
				"<uri>http://gdata.youtube.com/feeds/api/users/eluves</uri>"
			"</author>"
			"<media:group>"
				"<media:title type='plain'>Judas Priest - Painkiller</media:title>"
				"<media:credit role='uploader' scheme='urn:youtube'>eluves</media:credit>"
				"<media:category label='Music' scheme='http://gdata.youtube.com/schemas/2007/categories.cat'>Music</media:category>"
			"</media:group>"
			"<gd:comments>"
				"<gd:feedLink href='http://gdata.youtube.com/feeds/api/videos/JAagedeKdcQ/comments' countHint='13021'/>"
			"</gd:comments>"
		"</entry>", -1, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_YOUTUBE_VIDEO (video));
	g_clear_error (&error);

	* Test the feed link *
	feed_link = gdata_youtube_video_get_comments_feed_link (video);
	g_assert (feed_link != NULL);
	g_assert (feed_link->rel == NULL);
	g_assert_cmpstr (feed_link->href, ==, "http://gdata.youtube.com/feeds/api/videos/JAagedeKdcQ/comments");
	g_assert_cmpuint (feed_link->count_hint, ==, 13021);
	g_assert (feed_link->read_only == FALSE);

	* TODO: more tests on entry properties *

	g_object_unref (video);
}*/

static void
test_query_uri (void)
{
	gdouble latitude, longitude, radius;
	gboolean has_location;
	gchar *query_uri;
	GDataYouTubeQuery *query = gdata_youtube_query_new ("q");

	gdata_youtube_query_set_format (query, GDATA_YOUTUBE_FORMAT_RTSP_H263_AMR);
	g_assert_cmpuint (gdata_youtube_query_get_format (query), ==, 1);

	/* Location */
	gdata_youtube_query_set_location (query, 45.01364, -97.12356, 112.5, TRUE);
	gdata_youtube_query_get_location (query, &latitude, &longitude, &radius, &has_location);

	g_assert_cmpfloat (latitude, ==, 45.01364);
	g_assert_cmpfloat (longitude, ==, -97.12356);
	g_assert_cmpfloat (radius, ==, 112.5);
	g_assert (has_location == TRUE);

	query_uri = gdata_query_get_query_uri (GDATA_QUERY (query), "http://example.com");
	g_assert_cmpstr (query_uri, ==, "http://example.com?q=q&time=all_time&safeSearch=none&format=1&location=45.013640,-97.123560!&location-radius=112.500000m");
	g_free (query_uri);

	gdata_youtube_query_set_location (query, G_MAXDOUBLE, 0.6672, 52.8, TRUE);

	query_uri = gdata_query_get_query_uri (GDATA_QUERY (query), "http://example.com");
	g_assert_cmpstr (query_uri, ==, "http://example.com?q=q&time=all_time&safeSearch=none&format=1&location=!");
	g_free (query_uri);

	gdata_youtube_query_set_location (query, G_MAXDOUBLE, G_MAXDOUBLE, 0.0, FALSE);

	query_uri = gdata_query_get_query_uri (GDATA_QUERY (query), "http://example.com");
	g_assert_cmpstr (query_uri, ==, "http://example.com?q=q&time=all_time&safeSearch=none&format=1");
	g_free (query_uri);

	/* Language */
	gdata_youtube_query_set_language (query, "fr");
	g_assert_cmpstr (gdata_youtube_query_get_language (query), ==, "fr");

	gdata_youtube_query_set_order_by (query, "relevance_lang_fr");
	g_assert_cmpstr (gdata_youtube_query_get_order_by (query), ==, "relevance_lang_fr");

	gdata_youtube_query_set_restriction (query, "192.168.0.1");
	g_assert_cmpstr (gdata_youtube_query_get_restriction (query), ==, "192.168.0.1");

	query_uri = gdata_query_get_query_uri (GDATA_QUERY (query), "http://example.com");
	g_assert_cmpstr (query_uri, ==, "http://example.com?q=q&time=all_time&safeSearch=none&format=1&lr=fr&orderby=relevance_lang_fr&restriction=192.168.0.1");
	g_free (query_uri);

	gdata_youtube_query_set_safe_search (query, GDATA_YOUTUBE_SAFE_SEARCH_STRICT);
	g_assert_cmpuint (gdata_youtube_query_get_safe_search (query), ==, GDATA_YOUTUBE_SAFE_SEARCH_STRICT);

	query_uri = gdata_query_get_query_uri (GDATA_QUERY (query), "http://example.com");
	g_assert_cmpstr (query_uri, ==, "http://example.com?q=q&time=all_time&safeSearch=strict&format=1&lr=fr&orderby=relevance_lang_fr&restriction=192.168.0.1");
	g_free (query_uri);

	gdata_youtube_query_set_sort_order (query, GDATA_YOUTUBE_SORT_ASCENDING);
	g_assert_cmpuint (gdata_youtube_query_get_sort_order (query), ==, GDATA_YOUTUBE_SORT_ASCENDING);

	gdata_youtube_query_set_age (query, GDATA_YOUTUBE_AGE_THIS_WEEK);
	g_assert_cmpuint (gdata_youtube_query_get_age (query), ==, GDATA_YOUTUBE_AGE_THIS_WEEK);

	gdata_youtube_query_set_uploader (query, GDATA_YOUTUBE_UPLOADER_PARTNER);
	g_assert_cmpuint (gdata_youtube_query_get_uploader (query), ==, GDATA_YOUTUBE_UPLOADER_PARTNER);

	/* Check the built URI with a normal feed URI */
	query_uri = gdata_query_get_query_uri (GDATA_QUERY (query), "http://example.com");
	g_assert_cmpstr (query_uri, ==, "http://example.com?q=q&time=this_week&safeSearch=strict&format=1&lr=fr&orderby=relevance_lang_fr&restriction=192.168.0.1&sortorder=ascending&uploader=partner");
	g_free (query_uri);

	/* …and with a feed URI with pre-existing arguments */
	query_uri = gdata_query_get_query_uri (GDATA_QUERY (query), "http://example.com?foobar=shizzle");
	g_assert_cmpstr (query_uri, ==, "http://example.com?foobar=shizzle&q=q&time=this_week&safeSearch=strict&format=1&lr=fr&orderby=relevance_lang_fr&restriction=192.168.0.1&sortorder=ascending&uploader=partner");
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

	g_test_add_func ("/youtube/authentication", test_authentication);
	if (g_test_thorough () == TRUE)
		g_test_add_func ("/youtube/authentication_async", test_authentication_async);
	g_test_add_func ("/youtube/query/standard_feed", test_query_standard_feed);
	if (g_test_thorough () == TRUE)
		g_test_add_func ("/youtube/query/standard_feed_async", test_query_standard_feed_async);
	g_test_add_func ("/youtube/query/related", test_query_related);
	if (g_test_thorough () == TRUE)
		g_test_add_func ("/youtube/query/related_async", test_query_related_async);
	if (g_test_slow () == TRUE)
		g_test_add_func ("/youtube/upload/simple", test_upload_simple);
	g_test_add_func ("/youtube/parsing/app:control", test_parsing_app_control);
	/*g_test_add_func ("/youtube/parsing/comments/feedLink", test_parsing_comments_feed_link);*/
	g_test_add_func ("/youtube/parsing/yt:recorded", test_parsing_yt_recorded);
	g_test_add_func ("/youtube/query/uri", test_query_uri);

	retval = g_test_run ();
	if (service != NULL)
		g_object_unref (service);

	return retval;
}
