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

static GDataDocumentsEntry *
get_document (void)
{
	GDataDocumentsFeed *feed;
	GDataEntry *entry;
	GDataDocumentsQuery *query;
	GList *i;
	GList *entries;
	GError *error = NULL;

	g_assert (service != NULL);

	feed = gdata_documents_service_query_documents (GDATA_DOCUMENTS_SERVICE (service), NULL, NULL, NULL, NULL, &error);
	for (i = gdata_feed_get_entries (feed); i != NULL; i = i->next)
	{
		g_print ("Document Type: %s\n", G_OBJECT_TYPE_NAME (i->data));
		if (GDATA_IS_DOCUMENTS_TEXT (i->data))
				return GDATA_DOCUMENTS_TEXT (i->data);
	}
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (feed));
	g_clear_error (&error);

	entries = gdata_feed_get_entries (feed);
	g_assert (entries != NULL);
	entry = entries->data;

	/*Test if evrything is fine*/
	g_assert (GDATA_IS_DOCUMENTS_ENTRY (entry));
	g_object_ref (entry);
	g_object_unref (feed);

	return GDATA_DOCUMENTS_ENTRY (entry);
}

static void
test_authentication (void)
{
	gboolean retval;
	GError *error = NULL;

	/* Create a service */
	service = gdata_documents_service_new (CLIENT_ID);

	g_assert (service != NULL);
	g_assert (GDATA_IS_SERVICE (service));
	g_assert_cmpstr (gdata_service_get_client_id (service), ==, CLIENT_ID);

	/* Log in */
	retval = gdata_service_authenticate (service, DOCUMENTS_USERNAME, PASSWORD, NULL, &error);
	g_assert_no_error (error);
	g_assert (retval == TRUE);
	g_clear_error (&error);

	/* Check all is as it should be */
	g_assert (gdata_service_is_authenticated (service) == TRUE);
	g_assert_cmpstr (gdata_service_get_username (service), ==, DOCUMENTS_USERNAME);
	g_assert_cmpstr (gdata_service_get_password (service), ==, PASSWORD);
}

static void
test_remove_all_documents_and_folders (void)
{
	GDataDocumentsFeed *feed;
	GDataDocumentsQuery *query;
	GError *error = NULL;
	GList *i;

	g_assert (service != NULL);

	query = gdata_documents_query_new (NULL);
	gdata_documents_query_set_show_folders (query, TRUE);

	feed = gdata_documents_service_query_documents (GDATA_DOCUMENTS_SERVICE (service), query, NULL, NULL, NULL, &error);
	for (i = gdata_feed_get_entries (feed); i != NULL; i = i->next)
	{
		gdata_service_delete_entry (GDATA_SERVICE (service), GDATA_ENTRY (i->data), NULL, error);
	}
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (feed));
	g_clear_error (&error);

	/* TODO: check entries and feed properties */
	g_object_unref (feed);
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
	gdata_documents_query_set_show_folders (query, TRUE);

	feed = gdata_documents_service_query_documents (GDATA_DOCUMENTS_SERVICE (service), query, NULL, NULL, NULL, &error);
	for (i = gdata_feed_get_entries (feed); i != NULL; i = i->next)
	{
		if (GDATA_IS_DOCUMENTS_PRESENTATION (i->data))
			g_print ("	Presentation: %s Access Rules %d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
		if (GDATA_IS_DOCUMENTS_SPREADSHEET (i->data))
			g_print ("	Spreasheet: %s Access Rules %d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
		if (GDATA_IS_DOCUMENTS_TEXT (i->data))
			g_print ("	Document: %s Access Rules %d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
		if (GDATA_IS_DOCUMENTS_FOLDER (i->data)){
			g_print ("	Folder: %s Access Rules %d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
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

	feed = gdata_documents_service_query_documents (GDATA_DOCUMENTS_SERVICE (service), NULL, NULL, NULL, NULL, &error);
	for (i = gdata_feed_get_entries (feed); i != NULL; i = i->next)
	{
		if (GDATA_IS_DOCUMENTS_PRESENTATION (i->data))
			g_print ("	Presentation: %s Access Rules%d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
		if (GDATA_IS_DOCUMENTS_SPREADSHEET (i->data)){
			g_print ("	Spreasheet: %s Access Rules%d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
		}
		if (GDATA_IS_DOCUMENTS_TEXT (i->data))
			g_print ("	Document: %s Access Rules%d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
		if (GDATA_IS_DOCUMENTS_FOLDER (i->data))
			g_print ("	Folder: %s Access Rules%d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
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
			g_print ("	Presentation: %s Access Rules%d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
		if (GDATA_IS_DOCUMENTS_SPREADSHEET (i->data))
			g_print ("	Spreasheet: %s Access Rules%d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
		if (GDATA_IS_DOCUMENTS_TEXT (i->data))
			g_print ("	Document: %s Access Rules%d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
		if (GDATA_IS_DOCUMENTS_FOLDER (i->data))
			g_print ("	Folder: %s Access Rules%d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
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
test_upload_metadata_remove (void)
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

	/*Check evrything went fine*/
	g_assert_no_error (error);
	g_assert (GDATA_IS_DOCUMENTS_ENTRY (new_document));

	/*Delete entry*/
	gdata_service_delete_entry (service, new_document, NULL, error);
	/*Check if evrything is as it should be*/
	g_assert_no_error (error);
	g_assert (GDATA_IS_DOCUMENTS_ENTRY (new_document));
	g_clear_error (&error);
	g_object_unref (document);
	g_object_unref (new_document);
}

static void
test_upload_metadata_file (void)
{
	GDataDocumentsText *document, *new_document;
	GFile *document_file;
	GDataCategory *category;
	GError *error = NULL;

	g_assert (service != NULL);

	document_file = g_file_new_for_path ( "/home/thibault/workspace/gsoc/libgdata/libgdata/gdata/tests/test.odt");

	document = gdata_documents_spreadsheet_new (NULL);
	category = gdata_category_new ("http://schemas.google.com/docs/2007#document", "http://schemas.google.com/g/2005#kind", "document");
	gdata_entry_set_title (GDATA_ENTRY (document), "nexTextDoc");
	gdata_entry_add_category (GDATA_ENTRY (document), category);

	/* Insert the document */
	new_document = gdata_documents_service_upload_document (GDATA_DOCUMENTS_SERVICE (service), document, document_file, NULL, TRUE, NULL, &error);

	/*Check if evrything was fine*/
	g_assert_no_error (error);
	g_assert (GDATA_IS_DOCUMENTS_ENTRY (new_document));
	g_clear_error (&error);

	g_object_unref (document);
	g_object_unref (new_document);
}

static void
test_upload_file (void)
{
	GDataDocumentsSpreadsheet *new_document;
	GFile *document_file;
	GDataCategory *category;
	GError *error = NULL;

	g_assert (service != NULL);

	document_file = g_file_new_for_path ( "/home/thibault/workspace/gsoc/libgdata/libgdata/gdata/tests/test.ppt");

	category = gdata_category_new ("http://schemas.google.com/docs/2007#presentation", "http://schemas.google.com/g/2005#kind", "presentation");

	/* Insert the document */
	new_document = gdata_documents_service_upload_document (GDATA_DOCUMENTS_SERVICE (service), NULL, document_file, NULL, FALSE, NULL, &error);
	
	/*Check is evrything is fine*/
	g_assert_no_error (error);
	g_assert (GDATA_IS_DOCUMENTS_ENTRY (new_document));
	g_clear_error (&error);
	g_object_unref (new_document);
}

static void
test_add_remove_file_from_folder (void)
{
	GDataDocumentsFolder *folder, *new_folder;
	GDataDocumentsSpreadsheet *document, *new_document;
	GFile *document_file;
	GDataCategory *folder_category, *document_category;
	GError *error = NULL;

	g_assert (service != NULL);

	document_file = g_file_new_for_path ( "/home/thibault/workspace/gsoc/libgdata/libgdata/gdata/tests/test.ods");

	folder = gdata_documents_folder_new (NULL);
	folder_category = gdata_category_new ("http://schemas.google.com/docs/2007#folder", "http://schemas.google.com/g/2005#kind", "folder");
	gdata_entry_set_title (GDATA_ENTRY (folder), "newFolder");
	gdata_entry_add_category (GDATA_ENTRY (folder), folder_category);

	document = gdata_documents_spreadsheet_new (NULL);
	document_category = gdata_category_new ("http://schemas.google.com/docs/2007#document", "http://schemas.google.com/g/2005#kind", "document");
	gdata_entry_set_title (GDATA_ENTRY (document), "testingDocument");
	gdata_entry_add_category (GDATA_ENTRY (document), document_category);


	/* Insert the folder */
	new_folder = gdata_documents_service_upload_document (GDATA_DOCUMENTS_SERVICE (service), folder, NULL, NULL, TRUE, NULL, &error);
	g_assert_no_error (error);
	/* Insert the document in the new folder */
	new_document = gdata_documents_service_upload_document (GDATA_DOCUMENTS_SERVICE (service), document, document_file, new_folder, TRUE, NULL, &error);
	g_print ("\nNew document %d\n", new_document);
	/*remove document from the folder*/
	gdata_documents_service_remove_document_from_folder (GDATA_DOCUMENTS_SERVICE (service), new_document, new_folder, NULL, &error);

	/*Check is evrything is fine*/
	g_assert_no_error (error);
	g_assert (GDATA_IS_DOCUMENTS_ENTRY (new_document));
	g_assert (GDATA_IS_DOCUMENTS_ENTRY (new_folder));
	g_clear_error (&error);
	g_object_unref (document);
	g_object_unref (new_document);
	g_object_unref (folder);
	g_object_unref (new_folder);
}

static void
test_add_file_folder_and_move (void)
{
	GDataDocumentsFolder *folder, *new_folder;
	GDataDocumentsSpreadsheet *document, *new_document;
	GFile *document_file;
	GDataCategory *folder_category, *document_category;
	GError *error = NULL;

	g_assert (service != NULL);

	document_file = g_file_new_for_path ( "/home/thibault/workspace/gsoc/libgdata/libgdata/gdata/tests/test.ods");

	folder = gdata_documents_folder_new (NULL);
	folder_category = gdata_category_new ("http://schemas.google.com/docs/2007#folder", "http://schemas.google.com/g/2005#kind", "folder");
	gdata_entry_set_title (GDATA_ENTRY (folder), "theFolder");
	gdata_entry_add_category (GDATA_ENTRY (folder), folder_category);

	document = gdata_documents_spreadsheet_new (NULL);
	document_category = gdata_category_new ("http://schemas.google.com/docs/2007#document", "http://schemas.google.com/g/2005#kind", "document");
	gdata_entry_set_title (GDATA_ENTRY (document), "theDocument");
	gdata_entry_add_category (GDATA_ENTRY (document), document_category);


	/* Insert the folder */
	new_folder = gdata_documents_service_upload_document (GDATA_DOCUMENTS_SERVICE (service), folder, NULL, NULL, TRUE, NULL, &error);
	g_assert_no_error (error);
	/* Insert the document in the new folder */
	new_document = gdata_documents_service_upload_document (GDATA_DOCUMENTS_SERVICE (service), document, document_file, NULL, TRUE, NULL, &error);
	g_print ("\nNew document %d\n", new_document);
	/*remove document from the folder*/
	gdata_documents_service_move_document_to_folder (GDATA_DOCUMENTS_SERVICE (service), new_document, new_folder, NULL, &error);

	/*Check is evrything is fine*/
	g_assert_no_error (error);
	g_assert (GDATA_IS_DOCUMENTS_ENTRY (new_document));
	g_assert (GDATA_IS_DOCUMENTS_ENTRY (new_folder));
	g_clear_error (&error);
	g_object_unref (document);
	g_object_unref (new_document);
	g_object_unref (folder);
	g_object_unref (new_folder);
}

static void
test_upload_file_metadata_in_new_folder (void)
{
	GDataDocumentsFolder *folder, *new_folder;
	GDataDocumentsSpreadsheet *document, *new_document;
	GFile *document_file;
	GDataCategory *folder_category, *document_category;
	GError *error = NULL;

	g_assert (service != NULL);

	document_file = g_file_new_for_path ( "/home/thibault/workspace/gsoc/libgdata/libgdata/gdata/tests/test.ods");

	folder = gdata_documents_folder_new (NULL);
	folder_category = gdata_category_new ("http://schemas.google.com/docs/2007#folder", "http://schemas.google.com/g/2005#kind", "folder");
	gdata_entry_set_title (GDATA_ENTRY (folder), "newFolder");
	gdata_entry_add_category (GDATA_ENTRY (folder), folder_category);

	document = gdata_documents_spreadsheet_new (NULL);
	document_category = gdata_category_new ("http://schemas.google.com/docs/2007#document", "http://schemas.google.com/g/2005#kind", "document");
	gdata_entry_set_title (GDATA_ENTRY (document), "testingDocument");
	gdata_entry_add_category (GDATA_ENTRY (document), document_category);


	/* Insert the folder */
	new_folder = gdata_documents_service_upload_document (GDATA_DOCUMENTS_SERVICE (service), GDATA_DOCUMENTS_FOLDER (folder), NULL, NULL, TRUE, NULL, &error);
	g_assert_no_error (error);
	g_assert (GDATA_IS_DOCUMENTS_FOLDER (new_folder));
	g_clear_error (&error);

	/* Insert the document in the new folder */
	new_document = gdata_documents_service_upload_document (GDATA_DOCUMENTS_SERVICE (service), document, document_file, new_folder, TRUE, NULL, &error);

	/*Check is evrything is fine*/
	g_assert_no_error (error);
	g_assert (GDATA_IS_DOCUMENTS_ENTRY (new_document));
	g_clear_error (&error);
	g_object_unref (document);
	g_object_unref (new_document);
	g_object_unref (folder);
	g_object_unref (new_folder);
}

static void
test_update_metadata (void)
{
	GDataDocumentsText *document, *new_document, *updated_document;
	GDataCategory *category;
	GError *error = NULL;

	g_assert (service != NULL);

	document = gdata_documents_spreadsheet_new (NULL);
	category = gdata_category_new ("http://schemas.google.com/docs/2007#document", "http://schemas.google.com/g/2005#kind", "document");
	gdata_entry_set_title (GDATA_ENTRY (document), "nexTextDoc");
	gdata_entry_add_category (GDATA_ENTRY (document), category);

	/* Insert the document */
	new_document = gdata_documents_service_upload_document (GDATA_DOCUMENTS_SERVICE (service), document, NULL, NULL, TRUE, NULL, &error);

	/*Check if evrything was fine*/
	g_assert_no_error (error);
	g_assert (GDATA_IS_DOCUMENTS_ENTRY (new_document));

	/* Update the document */
	updated_document = gdata_documents_service_update_document (GDATA_DOCUMENTS_SERVICE (service), new_document, NULL, TRUE, NULL, &error);

	/*Check the update went fine*/
	g_assert_no_error (error);
	g_clear_error (&error);
	g_object_unref (document);
	g_object_unref (new_document);
	g_object_unref (updated_document);
}

static void
test_update_metadata_file (void)
{
	GDataDocumentsText *document, *new_document, *updated_document;
	GFile *document_file;
	GDataCategory *category;
	GError *error = NULL;

	g_assert (service != NULL);

	document_file = g_file_new_for_path ( "/home/thibault/workspace/gsoc/libgdata/libgdata/gdata/tests/test.odt");

	document = gdata_documents_spreadsheet_new (NULL);
	category = gdata_category_new ("http://schemas.google.com/docs/2007#document", "http://schemas.google.com/g/2005#kind", "document");
	gdata_entry_set_title (GDATA_ENTRY (document), "nexTextDoc");
	gdata_entry_add_category (GDATA_ENTRY (document), category);

	/* Insert the document */
	new_document = gdata_documents_service_upload_document (GDATA_DOCUMENTS_SERVICE (service), document, document_file, NULL, TRUE, NULL, &error);
	/*Check that evrything was fine*/
	g_assert_no_error (error);
	g_assert (GDATA_IS_DOCUMENTS_ENTRY (new_document));

	/* Update the document */
	updated_document = gdata_documents_service_update_document (GDATA_DOCUMENTS_SERVICE (service), new_document, document_file, TRUE, NULL, &error);

	/*Check that evrything was fine*/
	g_assert_no_error (error);
	g_assert (GDATA_IS_DOCUMENTS_ENTRY (new_document));
	g_clear_error (&error);

	g_object_unref (document);
	g_object_unref (new_document);
	g_object_unref (updated_document);
}

static void
test_update_file (void)
{
	GDataDocumentsPresentation *new_document, *updated_document;
	GFile *document_file;
	GDataCategory *category;
	GError *error = NULL;

	g_assert (service != NULL);

	document_file = g_file_new_for_path ( "/home/thibault/workspace/gsoc/libgdata/libgdata/gdata/tests/test.ppt");

	/* Insert the document */
	new_document = gdata_documents_service_upload_document (GDATA_DOCUMENTS_SERVICE (service), NULL, document_file, NULL, FALSE, NULL, &error);
	/*Test if everything was fine with the upload*/
	g_assert (GDATA_IS_DOCUMENTS_ENTRY (new_document));
	g_assert_no_error (error);

	/* Update the document */
	updated_document = gdata_documents_service_update_document (GDATA_DOCUMENTS_SERVICE (service), new_document, document_file, FALSE, NULL, &error);

	/*Test if everything was fine with the update*/
	g_assert_no_error (error);
	g_assert (GDATA_IS_DOCUMENTS_ENTRY (updated_document));
	g_clear_error (&error);

	g_object_unref (new_document);
	g_object_unref (updated_document);
}

static void
test_download_all_documents (void)
{
	GDataDocumentsFeed *feed;
	GError *error = NULL;
	gchar *content_type = NULL;
	gchar *destination_folder = "/tmp";
	GFile *destination_file;
	GList *i;

	feed = gdata_documents_service_query_documents (GDATA_DOCUMENTS_SERVICE (service), NULL, NULL, NULL, NULL, &error);
	for (i = gdata_feed_get_entries (feed); i != NULL; i = i->next)
	{
		if (GDATA_IS_DOCUMENTS_PRESENTATION (i->data)){
			destination_file = gdata_documents_presentation_download_document (GDATA_DOCUMENTS_PRESENTATION (i->data), GDATA_DOCUMENTS_SERVICE (service),\
					&content_type, GDATA_DOCUMENTS_PRESENTATION_PPT, destination_folder, TRUE, NULL, &error);
			if ( destination_file != NULL)
				g_print ("	Presentation destination: %s\n", g_file_get_uri (destination_file));
		}else if (GDATA_IS_DOCUMENTS_SPREADSHEET (i->data)){
			destination_file = gdata_documents_spreadsheet_download_document (i->data, GDATA_DOCUMENTS_SERVICE (service),\
					&content_type, "-1", GDATA_DOCUMENTS_SPREADSHEET_ODS, destination_folder, TRUE, NULL, &error);
			if ( destination_file != NULL)
				g_print ("	Spreasheet destination: %s\n", g_file_get_uri (destination_file));
		}else if (GDATA_IS_DOCUMENTS_TEXT (i->data)){
			destination_file = gdata_documents_text_download_document (i->data, GDATA_DOCUMENTS_SERVICE (service), &content_type, GDATA_DOCUMENTS_TEXT_ODT,\
				   	destination_folder, TRUE, NULL, &error);
			if ( destination_file != NULL)
				g_print ("	Document destination: %s\n", g_file_get_uri (destination_file));
		}else if (GDATA_IS_DOCUMENTS_FOLDER (i->data))
			g_print ("	Folder: %s Access Rules%d\n", gdata_entry_get_title (i->data), gdata_documents_entry_get_access_rules(i->data));
		g_assert_no_error (error);
	}
	g_assert_no_error (error);
	g_assert (GDATA_IS_FEED (feed));
	g_object_unref (feed);
	g_clear_error (&error);
}

static void
test_add_collaborator_for_this_document(void)
{
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
/*
	g_test_add_func ("/documents/query/all_documents_with_folder", test_query_all_documents_with_folder);
	g_test_add_func ("/documents/query/all_documents", test_query_all_documents);
	g_test_add_func ("/documents/query/all_documents_async", test_query_all_documents_async);

	g_test_add_func ("/documents/upload/only_file", test_upload_file);
	g_test_add_func ("/documents/upload/metadata_file", test_upload_metadata_file);
	g_test_add_func ("/documents/upload/only_metadata_delete", test_upload_metadata_remove);
	g_test_add_func ("/documents/upload/metadata_file_in_new_folder", test_upload_file_metadata_in_new_folder);

	g_test_add_func ("/documents/update/only_metadata", test_update_metadata);
 	g_test_add_func ("/documents/update/only_file", test_update_file);
	g_test_add_func ("/documents/update/metadata_file", test_update_metadata_file);
	g_test_add_func ("/documents/download/download_all_documents", test_download_all_documents);

*/
	g_test_add_func ("/documents/move/remove_from_folder", test_add_remove_file_from_folder);
	g_test_add_func ("/documents/move/move_to_folder", test_add_file_folder_and_move);

	g_test_add_func ("/documents/remove/all", test_remove_all_documents_and_folders);

	retval = g_test_run ();
	if (service != NULL)
		g_object_unref (service);

	return retval;
}
