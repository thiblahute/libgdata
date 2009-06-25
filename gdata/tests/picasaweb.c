/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Richard Schwarting 2009 <aquarichy@gmail.com>
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
#include <string.h>

#include "gdata.h"
#include "common.h"

#define PW_USERNAME "libgdata.picasaweb@gmail.com"
/* the following two properties will change if a new album is added */
#define NUM_ALBUMS 3
#define TEST_ALBUM_INDEX 2

/* TODO: probably a better way to do this; some kind of data associated with the test suite? */
static GDataService *service = NULL;
static GMainLoop *main_loop = NULL;

static void
test_authentication (void)
{
	gboolean retval;
	GError *error = NULL;

	/* Create a service */
	service = GDATA_SERVICE (gdata_picasaweb_service_new (CLIENT_ID));

	g_assert (service != NULL);
	g_assert (GDATA_IS_SERVICE (service));
	g_assert_cmpstr (gdata_service_get_client_id (service), ==, CLIENT_ID);

	/* Log in */
	retval = gdata_service_authenticate (service, PW_USERNAME, PASSWORD, NULL, &error);
	g_assert_no_error (error);
	g_assert (retval == TRUE);
	g_clear_error (&error);

	/* Check all is as it should be */
	g_assert (gdata_service_is_authenticated (service) == TRUE);
	g_assert_cmpstr (gdata_service_get_username (service), ==, PW_USERNAME);
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
	g_assert_cmpstr (gdata_service_get_username (service), ==, PW_USERNAME);
	g_assert_cmpstr (gdata_service_get_password (service), ==, PASSWORD);
}


static void
test_authentication_async (void)
{
	/* Create a service */
	service = GDATA_SERVICE (gdata_picasaweb_service_new (CLIENT_ID));

	g_assert (service != NULL);
	g_assert (GDATA_IS_SERVICE (service));

	gdata_service_authenticate_async (service, PW_USERNAME, PASSWORD, NULL, (GAsyncReadyCallback) test_authentication_async_cb, NULL);

	main_loop = g_main_loop_new (NULL, TRUE);
	g_main_loop_run (main_loop);
	g_main_loop_unref (main_loop);
}

static void
test_upload_simple (void)
{
	GDataCategory *category;
	GDataPicasaWebFile *photo;
	GDataPicasaWebFile *photo_new;
	GFile *photo_file;
	gchar *xml;
	GError *error = NULL;

	g_assert (service != NULL);

	photo = gdata_picasaweb_file_new (NULL);

	gdata_entry_set_title (GDATA_ENTRY (photo), "Photo Entry Title");
	gdata_picasaweb_file_set_caption (photo, "Photo Summary");
	/* TODO: Have it add this category automatically? Same for GDataCalendarEvent */
	category = gdata_category_new ("http://schemas.google.com/photos/2007#photo", "http://schemas.google.com/g/2005#kind", NULL);
	gdata_entry_add_category (GDATA_ENTRY (photo), category);

	/* Check the XML */
	xml = gdata_entry_get_xml (GDATA_ENTRY (photo));
	g_assert_cmpstr (xml, ==,
			 "<entry "
				"xmlns='http://www.w3.org/2005/Atom' "
				"xmlns:gphoto='http://schemas.google.com/photos/2007' "
				"xmlns:media='http://video.search.yahoo.com/mrss' "
				"xmlns:gd='http://schemas.google.com/g/2005' "
				"xmlns:app='http://www.w3.org/2007/app'>"
				"<title type='text'>Photo Entry Title</title>"
				"<summary type='text'>Photo Summary</summary>"
				"<category term='http://schemas.google.com/photos/2007#photo' scheme='http://schemas.google.com/g/2005#kind'/>"
				"<gphoto:position>0.000000</gphoto:position>"
				"<gphoto:commentingEnabled>true</gphoto:commentingEnabled>"
				"<media:group>"
					"<media:title type='plain'>Photo Entry Title</media:title>"
					"<media:description type='plain'>Photo Summary</media:description>"
				"</media:group>"
			 "</entry>");
	g_free(xml);

	/* File is public domain: http://en.wikipedia.org/wiki/File:German_garden_gnome_cropped.jpg */
	photo_file = g_file_new_for_path ("photo.jpg"); // TODO make sure file is loaded from tests/ directory

	/* Upload the photo */
	/* TODO right now, it will just go to the default album, we want an uploading one :| */
	photo_new = gdata_picasaweb_service_upload_file (GDATA_PICASAWEB_SERVICE (service), NULL, photo, photo_file, NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_PICASAWEB_FILE (photo_new));
	g_clear_error (&error);

	/* TODO: check entries and feed properties */

	g_object_unref (photo);
	g_object_unref (photo_new);
	g_object_unref (photo_file);
}


static void
test_photo (void)
{
	GError *error = NULL;
	GDataFeed *album_feed;
	GDataFeed *photo_feed;
	GList *albums;
	GList *files;
	GDataEntry *album_entry;
	GDataEntry *photo_entry;
	GDataPicasaWebAlbum *album;
	GDataPicasaWebFile *photo;
	GList *list;
	GDataMediaContent *content;
	GDataMediaThumbnail *thumbnail;
	GTimeVal _time;
	gchar *str;

	album_feed = gdata_picasaweb_service_query_all_albums (GDATA_PICASAWEB_SERVICE (service), NULL, NULL, NULL, NULL, NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (album_feed));
	g_clear_error (&error);

	albums = gdata_feed_get_entries (album_feed);
	album_entry = GDATA_ENTRY (g_list_nth_data (albums, TEST_ALBUM_INDEX));
	album = GDATA_PICASAWEB_ALBUM (album_entry);

	photo_feed = gdata_picasaweb_service_query_files (GDATA_PICASAWEB_SERVICE (service), GDATA_PICASAWEB_ALBUM (album), NULL, NULL, NULL,
							  NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (photo_feed));
	g_clear_error (&error);

	files = gdata_feed_get_entries (photo_feed);
	photo_entry = GDATA_ENTRY (g_list_nth_data (files, 0));
	photo = GDATA_PICASAWEB_FILE (photo_entry);

	gdata_picasaweb_file_get_edited (photo, &_time);
	str = g_time_val_to_iso8601 (&_time);
	g_assert_cmpstr (str, ==, "2009-04-26T06:57:03.474000Z");
	g_free (str);

	/* tests */

	g_assert_cmpstr (gdata_picasaweb_file_get_caption (photo), ==, "Ginger cookie caption");
	g_assert_cmpstr (gdata_picasaweb_file_get_version (photo), ==, "29"); /* 1240729023474000"); */ /* TODO check how constant this even is */
	g_assert_cmpfloat (gdata_picasaweb_file_get_position (photo), ==, 0.0);
	g_assert_cmpstr (gdata_picasaweb_file_get_album_id (photo), ==, "5328889949261497249");
	g_assert_cmpuint (gdata_picasaweb_file_get_width (photo), ==, 2576);
	g_assert_cmpuint (gdata_picasaweb_file_get_height (photo), ==, 1932);
	g_assert_cmpuint (gdata_picasaweb_file_get_size (photo), ==, 1124730);
	// TODO: file wasn't uploaded with client assigned; g_assert_cmpstr (gdata_picasaweb_file_get_client (photo), ==, ??);
	// TODO: file wasn't uploaded with checksum assigned; g_assert_cmpstr (gdata_picasaweb_file_get_checksum (photo), ==, ??);

	gdata_picasaweb_file_get_timestamp (photo, &_time);
	str = g_time_val_to_iso8601 (&_time);
	g_assert_cmpstr (str, ==, "2008-12-06T18:32:10Z");
	g_free (str);

	g_assert_cmpstr (gdata_picasaweb_file_get_video_status (photo), ==, NULL);
	/* todo: not a good test of video status; want to upload a video for it */
	g_assert_cmpuint (gdata_picasaweb_file_is_commenting_enabled (photo), ==, TRUE);
	g_assert_cmpuint (gdata_picasaweb_file_get_comment_count (photo), ==, 1);
	g_assert_cmpuint (gdata_picasaweb_file_get_rotation (photo), ==, 0);

	g_assert_cmpstr (gdata_picasaweb_file_get_caption (photo), ==, "Ginger cookie caption");
	g_assert_cmpstr (gdata_picasaweb_file_get_tags (photo), ==, "cookies");
	g_assert_cmpstr (gdata_entry_get_title (GDATA_ENTRY (photo)), ==, "100_0269.jpg");

	g_assert_cmpstr (gdata_picasaweb_file_get_credit (photo), ==, "libgdata.picasaweb");

	/* TODO put up warning not to free this, if we are going to be passing them our own internal copy! */
	list = gdata_picasaweb_file_get_contents (photo);

	g_assert_cmpuint (g_list_length (list), ==, 1);
	content = GDATA_MEDIA_CONTENT (list->data);
	g_assert_cmpstr (gdata_media_content_get_uri (content), ==,
			 "http://lh3.ggpht.com/_1kdcGyvOb8c/SfQFWPnuovI/AAAAAAAAAB0/MI0L4Sd11Eg/100_0269.jpg");
	g_assert_cmpstr (gdata_media_content_get_content_type (content), ==, "image/jpeg");
	g_assert_cmpuint (gdata_media_content_is_default (content), ==, FALSE);
	g_assert_cmpint (gdata_media_content_get_duration (content), ==, 0); /* doesn't apply to photos, but let's sanity-check it */

	list = gdata_picasaweb_file_get_thumbnails (photo);

	g_assert_cmpuint (g_list_length (list), ==, 3);
	thumbnail = GDATA_MEDIA_THUMBNAIL (list->data);
	g_assert_cmpstr (gdata_media_thumbnail_get_uri (thumbnail), ==,
			 "http://lh3.ggpht.com/_1kdcGyvOb8c/SfQFWPnuovI/AAAAAAAAAB0/MI0L4Sd11Eg/s288/100_0269.jpg");
	g_assert_cmpuint (gdata_media_thumbnail_get_width (thumbnail), ==, 288);
	g_assert_cmpuint (gdata_media_thumbnail_get_height (thumbnail), ==, 216);
	/* TODO consider testing time, gint64 */
}

static void
test_photo_feed_entry (void)
{
	GDataFeed *album_feed;
	GDataFeed *photo_feed;
	GError *error = NULL;
	GDataEntry *entry;
	GDataPicasaWebAlbum *album;
	GList *albums;
	GList *files;
	GDataEntry *photo_entry;
	gchar *str;
	GTimeVal _time;

	album_feed = gdata_picasaweb_service_query_all_albums (GDATA_PICASAWEB_SERVICE (service), NULL, NULL, NULL, NULL, NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (album_feed));
	g_clear_error (&error);

	albums = gdata_feed_get_entries (album_feed);
	entry = GDATA_ENTRY (g_list_nth_data (albums, TEST_ALBUM_INDEX));
	album = GDATA_PICASAWEB_ALBUM (entry);

	photo_feed = gdata_picasaweb_service_query_files (GDATA_PICASAWEB_SERVICE (service), GDATA_PICASAWEB_ALBUM (album), NULL, NULL, NULL,
							  NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (photo_feed));
	g_clear_error (&error);

	files = gdata_feed_get_entries (photo_feed);
	photo_entry = GDATA_ENTRY (g_list_nth_data (files, 0));

	/* tests */

	g_assert_cmpuint (g_list_length (files), ==, 1);

	g_assert_cmpstr (gdata_entry_get_title (photo_entry), ==, "100_0269.jpg");
	g_assert_cmpstr (gdata_entry_get_id (photo_entry), ==, "5328890138794566386");
	g_assert_cmpstr (gdata_entry_get_etag (photo_entry), !=, NULL);

	gdata_entry_get_updated (photo_entry, &_time);
	str = g_time_val_to_iso8601 (&_time);
	g_assert_cmpstr (str, ==, "2009-04-26T06:57:03.474000Z");
	g_free (str);

	gdata_entry_get_published (photo_entry, &_time);
	str = g_time_val_to_iso8601 (&_time);
	g_assert_cmpstr (str, ==, "2009-04-26T06:55:20Z");
	g_free (str);

	g_assert_cmpstr (gdata_entry_get_content (photo_entry), ==,
			 "http://lh3.ggpht.com/_1kdcGyvOb8c/SfQFWPnuovI/AAAAAAAAAB0/MI0L4Sd11Eg/100_0269.jpg");
	g_assert_cmpstr (gdata_entry_get_xml (photo_entry), !=, NULL);
	g_assert_cmpuint (strlen (gdata_entry_get_xml (photo_entry)), >, 0);
}

static void
test_photo_feed (void)
{
	GError *error = NULL;
	GDataFeed *album_feed;
	GDataFeed *photo_feed;
	GDataEntry *entry;
	GDataPicasaWebAlbum *album;
	GList *albums;

	album_feed = gdata_picasaweb_service_query_all_albums (GDATA_PICASAWEB_SERVICE (service), NULL, NULL, NULL, NULL, NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (album_feed));
	g_clear_error (&error);

	albums = gdata_feed_get_entries (album_feed);
	entry = GDATA_ENTRY (g_list_nth_data (albums, TEST_ALBUM_INDEX));
	album = GDATA_PICASAWEB_ALBUM (entry);

	/* tests */

	photo_feed = gdata_picasaweb_service_query_files (GDATA_PICASAWEB_SERVICE (service), GDATA_PICASAWEB_ALBUM (album), NULL, NULL, NULL,
							  NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (photo_feed));
	g_clear_error (&error);

	g_assert_cmpstr (gdata_feed_get_title (photo_feed), ==, "Test Album 1 - Venice - Public");
	g_assert_cmpstr (gdata_feed_get_id (photo_feed), ==,
			 "http://picasaweb.google.com/data/feed/user/libgdata.picasaweb/albumid/5328889949261497249");
	g_assert_cmpstr (gdata_feed_get_etag (photo_feed), !=, NULL);
	g_assert_cmpuint (gdata_feed_get_items_per_page (photo_feed), ==, 1000);
	g_assert_cmpuint (gdata_feed_get_start_index (photo_feed), ==, 1);
	g_assert_cmpuint (gdata_feed_get_total_results (photo_feed), ==, 1);
}

static void
test_album (void)
{
	GDataFeed *album_feed;
	GError *error = NULL;
	GDataPicasaWebAlbum *album;
	GList *albums;
	GTimeVal _time;
	gchar *str;

	album_feed = gdata_picasaweb_service_query_all_albums (GDATA_PICASAWEB_SERVICE (service), NULL, NULL, NULL, NULL, NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (album_feed));
	g_clear_error (&error);

	albums = gdata_feed_get_entries (album_feed);
	album = GDATA_PICASAWEB_ALBUM (g_list_nth_data (albums, TEST_ALBUM_INDEX));

	/* Tests */
	g_assert_cmpstr (gdata_picasaweb_album_get_user (album), ==, "libgdata.picasaweb");
	g_assert_cmpstr (gdata_picasaweb_album_get_nickname (album), ==, "libgdata.picasaweb");

	gdata_picasaweb_album_get_edited (album, &_time);
	str = g_time_val_to_iso8601 (&_time);
	g_assert_cmpstr (str, ==, "2009-04-26T06:57:03.474000Z");
	g_free (str);

	g_assert_cmpstr (gdata_picasaweb_album_get_description (album), ==, "This is the test description.  This album should be in Venice.");
	g_assert_cmpint (gdata_picasaweb_album_get_visibility (album), ==, GDATA_PICASAWEB_PUBLIC);
	/* Google doesn't seem to be returning this one any more */
	/*g_assert_cmpstr (gdata_picasaweb_album_get_name (album), ==, "TestAlbum1VenicePublic");*/
	g_assert_cmpstr (gdata_picasaweb_album_get_location (album), ==, "Venice");

	gdata_picasaweb_album_get_timestamp (album, &_time);
	str = g_time_val_to_iso8601 (&_time);
	g_assert_cmpstr (str, ==, "2009-04-26T07:00:00Z");
	g_free (str);

	g_assert_cmpuint (gdata_picasaweb_album_get_num_photos (album), ==, 1);
	g_assert_cmpuint (gdata_picasaweb_album_get_num_photos_remaining (album), ==, 499);
	g_assert_cmpuint (gdata_picasaweb_album_get_bytes_used (album), ==, 1124730);
}

static void
test_album_feed_entry (void)
{
	GDataFeed *album_feed;
	GError *error = NULL;
	GDataEntry *entry;
	GList *albums;
	gchar *str, *xml;
	GTimeVal _time;

	album_feed = gdata_picasaweb_service_query_all_albums (GDATA_PICASAWEB_SERVICE (service), NULL, NULL, NULL, NULL, NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (album_feed));
	g_clear_error (&error);

	albums = gdata_feed_get_entries (album_feed);
	g_assert_cmpuint (g_list_length (albums), ==, NUM_ALBUMS);

	entry = GDATA_ENTRY (g_list_nth_data (albums, TEST_ALBUM_INDEX));
	g_assert (entry != NULL);

	g_object_ref (entry);
	g_object_unref (album_feed);

	/* Tests */
	g_assert_cmpstr (gdata_entry_get_title (entry), ==, "Test Album 1 - Venice - Public");
	g_assert_cmpstr (gdata_entry_get_id (entry), ==, "5328889949261497249");
	g_assert_cmpstr (gdata_entry_get_etag (entry), !=, NULL);

	gdata_entry_get_updated (entry, &_time);
	str = g_time_val_to_iso8601 (&_time);
	g_assert_cmpstr (str, ==, "2009-04-26T06:57:03.474000Z");
	g_free (str);

	gdata_entry_get_published (entry, &_time);
	str = g_time_val_to_iso8601 (&_time);
	g_assert_cmpstr (str, ==, "2009-04-26T07:00:00Z");
	g_free (str);

	// g_assert_cmpstr (gdata_entry_get_content (entry), !=, NULL);
	/* TODO */
	printf("** WARNING:%s:%d: gdata_entry_get_content(entry) returns null; valid?\n", __FILE__, __LINE__);
	xml = gdata_entry_get_xml (entry);
	g_assert_cmpstr (xml, !=, NULL);
	g_assert_cmpuint (strlen (xml), >, 0);
	g_free (xml);

	g_object_unref (entry);
}

static void
test_album_feed (void)
{
	GDataFeed *album_feed;
	GError *error = NULL;

	album_feed = gdata_picasaweb_service_query_all_albums (GDATA_PICASAWEB_SERVICE (service), NULL, NULL, NULL, NULL, NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (album_feed));
	g_clear_error (&error);

	/* tests */

	g_assert_cmpstr (gdata_feed_get_title (album_feed), ==, "libgdata.picasaweb");
	// TODO find out why subtitle == null when returned: no subtitle for feed? // printf("feed subtitle: %s\n", gdata_feed_get_subtitle(feed));
	g_assert_cmpstr (gdata_feed_get_id (album_feed), ==, "http://picasaweb.google.com/data/feed/user/libgdata.picasaweb");
	g_assert_cmpstr (gdata_feed_get_etag (album_feed), !=, NULL); /* this varies as albums change, like when a new image is uploaded in our test! */
	g_assert_cmpuint (gdata_feed_get_items_per_page (album_feed), ==, 1000);
	g_assert_cmpuint (gdata_feed_get_start_index (album_feed), ==, 1);
	g_assert_cmpuint (gdata_feed_get_total_results (album_feed), ==, NUM_ALBUMS);
}

static void
test_query_all_albums (void)
{
	GDataFeed *album_feed;
	GDataFeed *photo_feed;
	GError *error = NULL;
	GList *albums;
	GDataEntry *entry;
	GDataPicasaWebAlbum *album;

	g_assert (service != NULL);

	/* TODO: find out whether I need to free this; probably */
	album_feed = gdata_picasaweb_service_query_all_albums (GDATA_PICASAWEB_SERVICE (service), NULL, NULL, NULL, NULL, NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (album_feed));
	g_clear_error (&error);

	albums = gdata_feed_get_entries (album_feed);
	/* g_object_unref (feed); TODO find out why this complains about not being an object */
	entry = GDATA_ENTRY (g_list_nth_data (albums, TEST_ALBUM_INDEX));
	album = GDATA_PICASAWEB_ALBUM (entry);

	photo_feed = gdata_picasaweb_service_query_files (GDATA_PICASAWEB_SERVICE (service), GDATA_PICASAWEB_ALBUM (album), NULL, NULL, NULL,
							  NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (photo_feed));
	g_clear_error (&error);

	//g_object_unref(photo_feed);
	//g_object_unref(album_feed);

	g_list_free(albums);
}

static void
test_query_all_albums_async_cb (GDataService *service, GAsyncResult *async_result, gpointer user_data)
{
	GDataFeed *feed;
	GError *error;

	feed = gdata_service_query_finish (service, async_result, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (feed));
	g_clear_error (&error);

	/* @TODO: Tests? */
	g_main_loop_quit (main_loop);

	g_object_unref (feed);
}

static void
test_query_all_albums_async (void)
{
	g_assert (service != NULL);

	gdata_picasaweb_service_query_all_albums_async (GDATA_PICASAWEB_SERVICE (service), NULL, NULL, NULL, NULL,
							NULL, (GAsyncReadyCallback) test_query_all_albums_async_cb, NULL);

	main_loop = g_main_loop_new (NULL, TRUE);
	g_main_loop_run (main_loop);
	g_main_loop_unref (main_loop);
}

/* TODO: test private, public albums, test uploading */

int
main (int argc, char *argv[])
{
	gint retval;

	g_type_init ();
	g_thread_init (NULL);
	g_test_init (&argc, &argv, NULL);
	g_test_bug_base ("http://bugzilla.gnome.org/show_bug.cgi?id=");

	g_test_add_func ("/picasaweb/authentication", test_authentication);
	if (g_test_thorough () == TRUE)
		g_test_add_func ("/picasaweb/authentication_async", test_authentication_async);
	g_test_add_func ("/picasaweb/query/all_albums", test_query_all_albums);
	if (g_test_thorough () == TRUE)
		g_test_add_func ("/picasaweb/query/all_albums_async", test_query_all_albums_async);
	g_test_add_func ("/picasaweb/query/album_feed", test_album_feed);
	g_test_add_func ("/picasaweb/query/album_feed_entry", test_album_feed_entry);
	g_test_add_func ("/picasaweb/query/album", test_album);
	g_test_add_func ("/picasaweb/query/photo_feed", test_photo_feed);
	g_test_add_func ("/picasaweb/query/photo_feed_entry", test_photo_feed_entry);
	g_test_add_func ("/picasaweb/query/photo", test_photo);
	g_test_add_func ("/picasaweb/upload/photo", test_upload_simple);

	retval = g_test_run ();
	if (service != NULL)
		g_object_unref (service);

	return retval;
}

