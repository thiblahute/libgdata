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

static GDataContactsContact *
get_contact (void)
{
	GDataFeed *feed;
	GDataEntry *entry;
	GList *entries;
	GError *error = NULL;

	g_assert (service != NULL);

	feed = gdata_contacts_service_query_contacts (GDATA_CONTACTS_SERVICE (service), NULL, NULL, NULL, NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (feed));
	g_clear_error (&error);

	entries = gdata_feed_get_entries (feed);
	g_assert (entries != NULL);
	entry = entries->data;
	g_assert (GDATA_IS_CONTACTS_CONTACT (entry));

	g_object_ref (entry);
	g_object_unref (feed);

	return GDATA_CONTACTS_CONTACT (entry);
}

static void
test_authentication (void)
{
	gboolean retval;
	GError *error = NULL;

	/* Create a service */
	service = GDATA_SERVICE (gdata_contacts_service_new (CLIENT_ID));

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
test_query_all_contacts (void)
{
	GDataFeed *feed;
	GError *error = NULL;

	g_assert (service != NULL);

	feed = gdata_contacts_service_query_contacts (GDATA_CONTACTS_SERVICE (service), NULL, NULL, NULL, NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (feed));
	g_clear_error (&error);

	/* TODO: check entries and feed properties */

	g_object_unref (feed);
}

static void
test_query_all_contacts_async_cb (GDataService *service, GAsyncResult *async_result, gpointer user_data)
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
test_query_all_contacts_async (void)
{
	g_assert (service != NULL);

	gdata_contacts_service_query_contacts_async (GDATA_CONTACTS_SERVICE (service), NULL, NULL, NULL,
						     NULL, (GAsyncReadyCallback) test_query_all_contacts_async_cb, NULL);

	main_loop = g_main_loop_new (NULL, TRUE);
	g_main_loop_run (main_loop);
	g_main_loop_unref (main_loop);
}

static void
test_insert_simple (void)
{
	GDataContactsContact *contact, *new_contact;
	GDataCategory *category;
	GDataGDEmailAddress *email_address1, *email_address2;
	GDataGDPhoneNumber *phone_number1, *phone_number2;
	GDataGDIMAddress *im_address;
	GDataGDPostalAddress *postal_address;
	gchar *xml;
	GError *error = NULL;

	g_assert (service != NULL);

	contact = gdata_contacts_contact_new (NULL);

	gdata_entry_set_title (GDATA_ENTRY (contact), "Elizabeth Bennet");
	gdata_entry_set_content (GDATA_ENTRY (contact), "Notes");
	/* TODO: Have it add this category automatically? Same for GDataCalendarEvent */
	category = gdata_category_new ("http://schemas.google.com/contact/2008#contact", "http://schemas.google.com/g/2005#kind", NULL);
	gdata_entry_add_category (GDATA_ENTRY (contact), category);
	g_object_unref (category);
	email_address1 = gdata_gd_email_address_new ("liz@gmail.com", "http://schemas.google.com/g/2005#work", NULL, FALSE);
	gdata_contacts_contact_add_email_address (contact, email_address1);
	g_object_unref (email_address1);
	email_address2 = gdata_gd_email_address_new ("liz@example.org", "http://schemas.google.com/g/2005#home", NULL, FALSE);
	gdata_contacts_contact_add_email_address (contact, email_address2);
	g_object_unref (email_address2);
	phone_number1 = gdata_gd_phone_number_new ("(206)555-1212", "http://schemas.google.com/g/2005#work", NULL, NULL, TRUE);
	gdata_contacts_contact_add_phone_number (contact, phone_number1);
	g_object_unref (phone_number1);
	phone_number2 = gdata_gd_phone_number_new ("(206)555-1213", "http://schemas.google.com/g/2005#home", NULL, NULL, FALSE);
	gdata_contacts_contact_add_phone_number (contact, phone_number2);
	g_object_unref (phone_number2);
	im_address = gdata_gd_im_address_new ("liz@gmail.com", "http://schemas.google.com/g/2005#GOOGLE_TALK", "http://schemas.google.com/g/2005#home",
					      NULL, FALSE);
	gdata_contacts_contact_add_im_address (contact, im_address);
	g_object_unref (im_address);
	postal_address = gdata_gd_postal_address_new ("1600 Amphitheatre Pkwy Mountain View", "http://schemas.google.com/g/2005#work", NULL, TRUE);
	gdata_contacts_contact_add_postal_address (contact, postal_address);
	g_object_unref (postal_address);

	/* Add some extended properties */
	g_assert (gdata_contacts_contact_set_extended_property (contact, "TITLE", NULL) == TRUE);
	g_assert (gdata_contacts_contact_set_extended_property (contact, "ROLE", "") == TRUE);
	g_assert (gdata_contacts_contact_set_extended_property (contact, "CALURI", "http://example.com/") == TRUE);

	/* Check the XML */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (contact));
	g_assert_cmpstr (xml, ==,
			 "<entry xmlns='http://www.w3.org/2005/Atom' "
			 	"xmlns:gd='http://schemas.google.com/g/2005' "
			 	"xmlns:app='http://www.w3.org/2007/app' "
			 	"xmlns:gContact='http://schemas.google.com/contact/2008'>"
			 	"<title type='text'>Elizabeth Bennet</title>"
			 	"<content type='text'>Notes</content>"
				"<category term='http://schemas.google.com/contact/2008#contact' scheme='http://schemas.google.com/g/2005#kind'/>"
				"<gd:email address='liz@gmail.com' rel='http://schemas.google.com/g/2005#work' primary='false'/>"
				"<gd:email address='liz@example.org' rel='http://schemas.google.com/g/2005#home' primary='false'/>"
				"<gd:im address='liz@gmail.com' protocol='http://schemas.google.com/g/2005#GOOGLE_TALK' "
					"rel='http://schemas.google.com/g/2005#home' primary='false'/>"
				"<gd:phoneNumber rel='http://schemas.google.com/g/2005#work' primary='true'>(206)555-1212</gd:phoneNumber>"
				"<gd:phoneNumber rel='http://schemas.google.com/g/2005#home' primary='false'>(206)555-1213</gd:phoneNumber>"
				"<gd:postalAddress rel='http://schemas.google.com/g/2005#work' primary='true'>"
					"1600 Amphitheatre Pkwy Mountain View"
				"</gd:postalAddress>"
				"<gd:extendedProperty name='CALURI'>http://example.com/</gd:extendedProperty>"
			 "</entry>");
	g_free (xml);

	/* Insert the contact */
	new_contact = gdata_contacts_service_insert_contact (GDATA_CONTACTS_SERVICE (service), contact, NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_CONTACTS_CONTACT (new_contact));
	g_clear_error (&error);

	/* TODO: check entries and feed properties */

	g_object_unref (contact);
	g_object_unref (new_contact);
}

static void
test_query_uri (void)
{
	gchar *query_uri;
	GDataContactsQuery *query = gdata_contacts_query_new ("q");

	gdata_contacts_query_set_order_by (query, "lastmodified");
	g_assert_cmpstr (gdata_contacts_query_get_order_by (query), ==, "lastmodified");

	gdata_contacts_query_set_show_deleted (query, TRUE);
	g_assert (gdata_contacts_query_show_deleted (query) == TRUE);

	gdata_contacts_query_set_sort_order (query, "descending");
	g_assert_cmpstr (gdata_contacts_query_get_sort_order (query), ==, "descending");

	gdata_contacts_query_set_group (query, "http://www.google.com/feeds/contacts/groups/jo@gmail.com/base/1234a");
	g_assert_cmpstr (gdata_contacts_query_get_group (query), ==, "http://www.google.com/feeds/contacts/groups/jo@gmail.com/base/1234a");

	/* Check the built query URI with a normal feed URI */
	query_uri = gdata_query_get_query_uri (GDATA_QUERY (query), "http://example.com");
	g_assert_cmpstr (query_uri, ==, "http://example.com?q=q&orderby=lastmodified&showdeleted=true&sortorder=descending"
					"&group=http%3A%2F%2Fwww.google.com%2Ffeeds%2Fcontacts%2Fgroups%2Fjo%40gmail.com%2Fbase%2F1234a");
	g_free (query_uri);

	/* …with a feed URI with a trailing slash */
	query_uri = gdata_query_get_query_uri (GDATA_QUERY (query), "http://example.com/");
	g_assert_cmpstr (query_uri, ==, "http://example.com/?q=q&orderby=lastmodified&showdeleted=true&sortorder=descending"
					"&group=http%3A%2F%2Fwww.google.com%2Ffeeds%2Fcontacts%2Fgroups%2Fjo%40gmail.com%2Fbase%2F1234a");
	g_free (query_uri);

	/* …with a feed URI with pre-existing arguments */
	query_uri = gdata_query_get_query_uri (GDATA_QUERY (query), "http://example.com/bar/?test=test&this=that");
	g_assert_cmpstr (query_uri, ==, "http://example.com/bar/?test=test&this=that&q=q&orderby=lastmodified&showdeleted=true&sortorder=descending"
					"&group=http%3A%2F%2Fwww.google.com%2Ffeeds%2Fcontacts%2Fgroups%2Fjo%40gmail.com%2Fbase%2F1234a");
	g_free (query_uri);

	g_object_unref (query);
}

static void
test_parser_minimal (void)
{
	GDataContactsContact *contact;
	GError *error = NULL;

	g_test_bug ("580330");

	contact = GDATA_CONTACTS_CONTACT (gdata_parsable_new_from_xml (GDATA_TYPE_CONTACTS_CONTACT,
		"<entry xmlns='http://www.w3.org/2005/Atom' "
			"xmlns:gd='http://schemas.google.com/g/2005' "
			"gd:etag='&quot;QngzcDVSLyp7ImA9WxJTFkoITgU.&quot;'>"
			"<id>http://www.google.com/m8/feeds/contacts/libgdata.test@googlemail.com/base/1b46cdd20bfbee3b</id>"
			"<updated>2009-04-25T15:21:53.688Z</updated>"
			"<app:edited xmlns:app='http://www.w3.org/2007/app'>2009-04-25T15:21:53.688Z</app:edited>"
			"<category scheme='http://schemas.google.com/g/2005#kind' term='http://schemas.google.com/contact/2008#contact'/>"
			"<title></title>" /* Here's where it all went wrong */
			"<link rel='http://schemas.google.com/contacts/2008/rel#photo' type='image/*' href='http://www.google.com/m8/feeds/photos/media/libgdata.test@googlemail.com/1b46cdd20bfbee3b'/>"
			"<link rel='http://www.iana.org/assignments/relation/self' type='application/atom+xml' href='http://www.google.com/m8/feeds/contacts/libgdata.test@googlemail.com/full/1b46cdd20bfbee3b'/>"
			"<link rel='http://www.iana.org/assignments/relation/edit' type='application/atom+xml' href='http://www.google.com/m8/feeds/contacts/libgdata.test@googlemail.com/full/1b46cdd20bfbee3b'/>"
			"<gd:email rel='http://schemas.google.com/g/2005#other' address='bob@example.com'/>"
		"</entry>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_ENTRY (contact));
	g_clear_error (&error);

	/* Check the contact's properties */
	g_assert (gdata_entry_get_title (GDATA_ENTRY (contact)) != NULL);
	g_assert (*gdata_entry_get_title (GDATA_ENTRY (contact)) == '\0');

	/* TODO: Check the other properties */

	g_object_unref (contact);
}

static void
test_photo_has_photo (void)
{
	GDataContactsContact *contact;
	gsize length = 0;
	gchar *content_type = NULL;
	GError *error = NULL;

	contact = GDATA_CONTACTS_CONTACT (gdata_parsable_new_from_xml (GDATA_TYPE_CONTACTS_CONTACT,
		"<entry xmlns='http://www.w3.org/2005/Atom' "
			"xmlns:gd='http://schemas.google.com/g/2005'>"
			"<id>http://www.google.com/m8/feeds/contacts/libgdata.test@googlemail.com/base/1b46cdd20bfbee3b</id>"
			"<updated>2009-04-25T15:21:53.688Z</updated>"
			"<category scheme='http://schemas.google.com/g/2005#kind' term='http://schemas.google.com/contact/2008#contact'/>"
			"<title></title>" /* Here's where it all went wrong */
			"<link rel='http://schemas.google.com/contacts/2008/rel#photo' type='image/*' "
				"href='http://www.google.com/m8/feeds/photos/media/libgdata.test@googlemail.com/1b46cdd20bfbee3b'/>"
		"</entry>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_ENTRY (contact));
	g_clear_error (&error);

	/* Check for no photo */
	g_assert (gdata_contacts_contact_has_photo (contact) == FALSE);
	g_assert (gdata_contacts_contact_get_photo (contact, GDATA_CONTACTS_SERVICE (service), &length, &content_type, NULL, &error) == NULL);
	g_assert_cmpint (length, ==, 0);
	g_assert (content_type == NULL);
	g_assert_no_error (error);

	g_clear_error (&error);
	g_free (content_type);
	g_object_unref (contact);

	/* Try again with a photo */
	contact = GDATA_CONTACTS_CONTACT (gdata_parsable_new_from_xml (GDATA_TYPE_CONTACTS_CONTACT,
		"<entry xmlns='http://www.w3.org/2005/Atom' "
			"xmlns:gd='http://schemas.google.com/g/2005'>"
			"<id>http://www.google.com/m8/feeds/contacts/libgdata.test@googlemail.com/base/1b46cdd20bfbee3b</id>"
			"<updated>2009-04-25T15:21:53.688Z</updated>"
			"<category scheme='http://schemas.google.com/g/2005#kind' term='http://schemas.google.com/contact/2008#contact'/>"
			"<title></title>" /* Here's where it all went wrong */
			"<link rel='http://schemas.google.com/contacts/2008/rel#photo' type='image/*' "
				"href='http://www.google.com/m8/feeds/photos/media/libgdata.test@googlemail.com/1b46cdd20bfbee3b' "
				"gd:etag='&quot;QngzcDVSLyp7ImA9WxJTFkoITgU.&quot;'/>"
		"</entry>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_ENTRY (contact));
	g_clear_error (&error);

	g_assert (gdata_contacts_contact_has_photo (contact) == TRUE);
	g_object_unref (contact);
}

static void
test_photo_add (void)
{
	GDataContactsContact *contact;
	gchar *data;
	gsize length;
	gboolean retval;
	GError *error = NULL;

	/* Get the photo */
	/* TODO: Fix the path */
	g_assert (g_file_get_contents ("/home/philip/Development/libgdata/gdata/tests/photo.jpg", &data, &length, NULL) == TRUE);

	/* Add it to the contact */
	contact = get_contact ();
	retval = gdata_contacts_contact_set_photo (contact, service, data, length, NULL, &error);
	g_assert_no_error (error);
	g_assert (retval == TRUE);

	g_clear_error (&error);
	g_object_unref (contact);
	g_free (data);
}

static void
test_photo_get (void)
{
	GDataContactsContact *contact;
	gchar *data, *content_type = NULL;
	gsize length = 0;
	GError *error = NULL;

	contact = get_contact ();
	g_assert (gdata_contacts_contact_has_photo (contact) == TRUE);

	/* Get the photo from the network */
	data = gdata_contacts_contact_get_photo (contact, GDATA_CONTACTS_SERVICE (service), &length, &content_type, NULL, &error);
	g_assert_no_error (error);
	g_assert (data != NULL);
	g_assert (length != 0);
	g_assert_cmpstr (content_type, ==, "image/jpg");

	g_assert (gdata_contacts_contact_has_photo (contact) == TRUE);

	g_free (content_type);
	g_free (data);
	g_object_unref (contact);
	g_clear_error (&error);
}

static void
test_photo_delete (void)
{
	GDataContactsContact *contact;
	GError *error = NULL;

	contact = get_contact ();
	g_assert (gdata_contacts_contact_has_photo (contact) == TRUE);

	/* Remove the contact's photo */
	g_assert (gdata_contacts_contact_set_photo (contact, service, NULL, 0, NULL, &error) == TRUE);
	g_assert_no_error (error);

	g_assert (gdata_contacts_contact_has_photo (contact) == FALSE);

	g_clear_error (&error);
	g_object_unref (contact);
}

int
main (int argc, char *argv[])
{
	gint retval;

	g_type_init ();
	g_thread_init (NULL);
	g_test_init (&argc, &argv, NULL);
	g_test_bug_base ("http://bugzilla.gnome.org/show_bug.cgi?id=");

	g_test_add_func ("/contacts/authentication", test_authentication);
	g_test_add_func ("/contacts/query/all_contacts", test_query_all_contacts);
	if (g_test_thorough () == TRUE)
		g_test_add_func ("/contacts/query/all_contacts_async", test_query_all_contacts_async);
	if (g_test_slow () == TRUE)
		g_test_add_func ("/contacts/insert/simple", test_insert_simple);
	g_test_add_func ("/contacts/query/uri", test_query_uri);
	g_test_add_func ("/contacts/parser/minimal", test_parser_minimal);
	g_test_add_func ("/contacts/photo/has_photo", test_photo_has_photo);
	if (g_test_slow () == TRUE) {
		g_test_add_func ("/contacts/photo/add", test_photo_add);
		g_test_add_func ("/contacts/photo/get", test_photo_get);
		g_test_add_func ("/contacts/photo/delete", test_photo_delete);
	}

	retval = g_test_run ();
	if (service != NULL)
		g_object_unref (service);

	return retval;
}
