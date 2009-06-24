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
	service = gdata_documents_service_new (USERNAME);

	g_assert (service != NULL);
	g_assert (GDATA_IS_SERVICE (service));
	g_assert_cmpstr (gdata_service_get_client_id (service), ==, USERNAME);

	/* Log in */
	retval = gdata_service_authenticate (service, USERNAME, PASSWORD, NULL, &error);
	g_assert_no_error (error);
	g_assert (retval == TRUE);
	g_clear_error (&error);

	/* Check all is as it should be */
	g_assert (gdata_service_is_authenticated (service) == TRUE);
	g_assert_cmpstr (gdata_service_get_username (service), ==, CLIENT_ID);
	g_assert_cmpstr (gdata_service_get_password (service), ==, PASSWORD);
}

static void
test_query_all_documents_with_folder (void)
{
	GDataDocumentsFeed *feed;
	GDataDocumentsQuery *query;
	GError *error = NULL;
	GList *i;

	g_assert (service != NULL);

	query = gdata_documents_query_new (NULL);
	gdata_documents_query_set_show_folder (query, TRUE);

	feed = gdata_documents_service_query_documents (GDATA_DOCUMENTS_SERVICE (service), query, FALSE, NULL, NULL, NULL, &error);
	for (i = gdata_feed_get_entries (feed); i != NULL; i = i->next)
	{
		if (GDATA_IS_DOCUMENTS_PRESENTATION (i->data))
			g_print ("Presentation: %s Access Rules %d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
		if (GDATA_IS_DOCUMENTS_SPREADSHEET (i->data))
			g_print ("Spreasheet: %s Access Rules %d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
		if (GDATA_IS_DOCUMENTS_TEXT (i->data))
			g_print ("Document: %s Access Rules %d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
		if (GDATA_IS_DOCUMENTS_FOLDER (i->data)){
			g_print ("Folder: %s Access Rules %d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
		}
	}
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (feed));
	g_clear_error (&error);

	/* TODO: check entries and feed properties */

	g_object_unref (feed);
}
static void
test_query_all_documents (void)
{
	GDataDocumentsFeed *feed;
	GError *error = NULL;
	GList *i;

	g_assert (service != NULL);

	feed = gdata_documents_service_query_documents (GDATA_DOCUMENTS_SERVICE (service), FALSE, NULL, NULL, NULL, NULL, &error);
	for (i = gdata_feed_get_entries (feed); i != NULL; i = i->next)
	{
		if (GDATA_IS_DOCUMENTS_PRESENTATION (i->data))
			g_print ("Presentation: %s Access Rules%d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
		if (GDATA_IS_DOCUMENTS_SPREADSHEET (i->data)){
			g_print ("Spreasheet: %s Access Rules%d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
			if (strcmp (gdata_entry_get_title (i->data), "comptespapathibault2") == 0){
				gchar *content_type;
				gdata_documents_spreadsheet_download_document ( i->data, service, &content_type, "-1", "4", "/home/thibault", TRUE, NULL, error);
				g_print ("\n\nDownload\n:");
			}
		}
		if (GDATA_IS_DOCUMENTS_TEXT (i->data))
			g_print ("Document: %s Access Rules%d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
		if (GDATA_IS_DOCUMENTS_FOLDER (i->data))
			g_print ("Folder: %s Access Rules%d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
	}
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (feed));
	g_clear_error (&error);

	/* TODO: check entries and feed properties */

	g_object_unref (feed);
}

static void
test_query_all_documents_async_cb (GDataService *service, GAsyncResult *async_result, gpointer user_data)
{
	GDataDocumentsFeed *feed;
	GError *error = NULL;
	GList *i;

	feed = gdata_service_query_finish (service, async_result, &error);
	for (i = gdata_feed_get_entries (feed); i != NULL; i = i->next)
	{
		if (GDATA_IS_DOCUMENTS_PRESENTATION (i->data))
			g_print ("Presentation: %s Access Rules%d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
		if (GDATA_IS_DOCUMENTS_SPREADSHEET (i->data))
			g_print ("Spreasheet: %s Access Rules%d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
		if (GDATA_IS_DOCUMENTS_TEXT (i->data))
			g_print ("Document: %s Access Rules%d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
		if (GDATA_IS_DOCUMENTS_FOLDER (i->data))
			g_print ("Folder: %s Access Rules%d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
	}
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (feed));
	g_clear_error (&error);

	/* TODO: Tests? */
	g_main_loop_quit (main_loop);

	g_object_unref (feed);
}


static void
test_query_all_documents_async (void)
{
	g_assert (service != NULL);

	gdata_documents_service_query_documents_async (GDATA_DOCUMENTS_SERVICE (service), NULL, NULL, NULL,
						     NULL, (GAsyncReadyCallback) test_query_all_documents_async_cb, NULL);

	main_loop = g_main_loop_new (NULL, TRUE);
	g_main_loop_run (main_loop);
	g_main_loop_unref (main_loop);
}

static void
test_upload_metadata (void)
{
	GDataDocumentsSpreadsheet *document, *new_document;
	gchar *xml;
	GDataCategory *category;
	GError *error = NULL;

	g_assert (service != NULL);

	document = gdata_documents_spreadsheet_new (NULL);
	category = gdata_category_new ("http://schemas.google.com/docs/2007#spreadsheet", "http://schemas.google.com/g/2005#kind", "spreadsheet");

	gdata_entry_set_title (GDATA_ENTRY (document), "myNewSpreadsheet");
	gdata_entry_add_category (GDATA_ENTRY (document), category);

	/* Insert the document */
	new_document = gdata_documents_service_upload_document (GDATA_DOCUMENTS_SERVICE (service), document, NULL, NULL, TRUE, NULL, &error);

	/*Check if evrything is as it should be*/
	g_assert_no_error (error);
	g_assert (GDATA_IS_DOCUMENTS_ENTRY (new_document));
	g_clear_error (&error);
	g_object_unref (document);
	g_object_unref (new_document);

	/* TODO: check entries and feed properties */
}

static void
test_upload_metadata_file (void)
{
	GDataDocumentsSpreadsheet *document, *new_document;
	GDataDocumentsFolder *folder;
	gchar *xml;
	GDataCategory *category;
	GError *error = NULL;

	g_assert (service != NULL);

	document = gdata_documents_spreadsheet_new (NULL);
	folder = gdata_documents_folder_new ("rysSyYDFOJgnn3jPpaIN-_Q");
	category = gdata_category_new ("http://schemas.google.com/docs/2007#folder", "http://schemas.google.com/g/2005#kind", "folder");

	gdata_entry_set_title (GDATA_ENTRY (document), "myNewFolder");
	gdata_entry_add_category (GDATA_ENTRY (document), category);

	/* Insert the document */
	new_document = gdata_documents_service_upload_document (GDATA_DOCUMENTS_SERVICE (service), document, folder, NULL, TRUE, NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_DOCUMENTS_ENTRY (new_document));
	g_clear_error (&error);

	/* TODO: check entries and feed properties */

	g_object_unref (document);
	g_object_unref (new_document);
}

static void
test_document_download (void)
{
	GDataDocumentsFeed *feed;
	GError *error = NULL;
	gchar *content_type = NULL;
	gchar *destination_folder = "/tmp";
	GFile *destination_file;
	GList *i;

	feed = gdata_documents_service_query_documents (GDATA_DOCUMENTS_SERVICE (service), FALSE, NULL, NULL, NULL, NULL, &error);
	for (i = gdata_feed_get_entries (feed); i != NULL; i = i->next)
	{
		if (GDATA_IS_DOCUMENTS_PRESENTATION (i->data)){
			destination_file = gdata_documents_presentation_download_document (i->data, GDATA_DOCUMENTS_SERVICE (service), &content_type, "ppt", destination_folder, TRUE, NULL, &error);
			if ( destination_file != NULL)
				g_print ("Presentation destination: %s\n", g_file_get_uri (destination_file));
		}else if (GDATA_IS_DOCUMENTS_SPREADSHEET (i->data)){
			destination_file = gdata_documents_spreadsheet_download_document (i->data, GDATA_DOCUMENTS_SERVICE (service), &content_type, "-1", "102", destination_folder, TRUE, NULL, &error);
			if ( destination_file != NULL)
				g_print ("Spreasheet destination: %s\n", g_file_get_uri (destination_file));
		}else if (GDATA_IS_DOCUMENTS_TEXT (i->data)){
			destination_file = gdata_documents_text_download_document (i->data, GDATA_DOCUMENTS_SERVICE (service), &content_type, "odt", destination_folder, TRUE, NULL, &error);
			if ( destination_file != NULL)
				g_print ("Document destination: %s\n", g_file_get_uri (destination_file));
		}else if (GDATA_IS_DOCUMENTS_FOLDER (i->data))
			g_print ("Folder: %s Access Rules%d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
	}
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (feed));
	g_clear_error (&error);
}

int
main (int argc, char *argv[])
{
	gint retval;

	g_type_init ();
	g_thread_init (NULL);
	g_test_init (&argc, &argv, NULL);
	g_test_bug_base ("http://bugzilla.gnome.org/show_bug.cgi?id=");

	g_test_add_func ("/documents/authentication", test_authentication);
	g_test_add_func ("/documents/upload/only_metadata", test_upload_metadata_file);

/*
	g_test_add_func ("/documents/query/all_documents", test_query_all_documents);
	g_test_add_func ("/documents/query/all_documents_with_folder", test_query_all_documents_with_folder);
	g_test_add_func ("/documents/query/all_documents_async", test_query_all_documents_async);
	g_test_add_func ("/documents/documenyts/download", test_document_download);
	g_test_add_func ("/documents/upload/only_metadata", test_upload_metadata);
*/

	retval = g_test_run ();
	if (service != NULL)
		g_object_unref (service);

	return retval;
}
