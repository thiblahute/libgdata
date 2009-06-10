/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * GData Client
 * Copyright (C) Thibault Saunier 2009 <saunierthibault@gmail.com>
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
#include <gdata/services/documents/gdata-documents-spreadsheet.h>
#include <gdata/services/documents/gdata-documents-presentation.h>
#include <gdata/services/documents/gdata-documents-text.h>
#include <gdata/services/documents/gdata-documents-folder.h>
#include <gdata/services/documents/gdata-documents-feed.h>
#include <gdata/gdata-entry.h>
#include <gdata/gdata-feed.h>
#include "common.h"

int
main (int argc, char *argv[])
{
	g_type_init ();

	GError **error_folder=NULL;
	GError **error_text=NULL;
	GError **error_spreadsheet=NULL;	
	GError **error_presentation = NULL;
	GError **error_feed = NULL;
	GDataAuthor *author;
	GTimeVal *editionTime =NULL;
	GType feed_type = GDATA_TYPE_DOCUMENTS_FEED;
	gint length;
	GType entry_type = GDATA_TYPE_DOCUMENTS_ENTRY;
	GDataQueryProgressCallback progress_callback = NULL;
	gpointer progress_user_data = NULL;
	GList *feed_entry_list;

	const gchar *xml_folder="<entry gd:etag='W/CUUNSXYyfCp7ImA9WxRVGUo.'><id>http://docs.google.com/feeds/folders/private/full/folder%3folder_id/document%3Adocument_id</id><published>0001-01-03T00:00:00.000Z</published><updated>2008-09-02T05:42:27.203Z</updated><category scheme='http://schemas.google.com/g/2005#kind' term='http://schemas.google.com/docs/2007#document' label='document'/><category scheme='http://schemas.google.com/docs/2007/folders/test.user@gmail.com' term='Folder 1' label='Folder 1'/><category scheme='http://schemas.google.com/docs/2007/folders/test.user@gmail.com' term='Folder 2' label='Folder 2'/><title type='TEEEEEEEEEEET'>Document 1</title><content type='text/html' src='http://docs.google.com/feeds/download/documents/RawDocContents?'/><link rel='http://schemas.google.com/docs/2007#parent' type='application/atom+xml' href='http://docs.google.com/feeds/documents/private/full/folder%3Afolder_id' title='Folder 1'/><link rel='http://schemas.google.com/docs/2007#parent' type='application/atom+xml' href='http://docs.google.com/feeds/documents/private/full/folder%3Afolder_id2' title='Folder'/><link rel='alternate' type='text/html' href='http://docs.google.com/Doc?id=document_id'/><link rel='self' type='application/atom+xml' href='http://docs.google.com/feeds/folders/private/full/folder%3Afolder_id/document%3Adocument_id'/><link rel='edit' type='application/atom+xml' href='http://docs.google.com/feeds/folders/private/full/folder%3Afolder_id/document%3Adocument_id'/><link rel='edit-media' type='text/html' href='http://docs.google.com/feeds/media/private/full/document%3Adocument_id'/><author><name>test.user</name><email>test.user@gmail.com</email></author><gd:resourceId>folder:folder_id</gd:resourceId></entry>";
	const gchar *xml_spreadsheet ="<entry gd:etag='BxAUSh5RAyp7ImBq'><id>http://docs.google.com/feeds/documents/private/full/spreadsheet%3Akey</id><published>2009-03-16T23:26:12.503Z</published><updated>2009-03-16T23:26:12.503Z</updated><app:edited xmlns:app='http://www.w3.org/2007/app'>2009-03-18T05:41:45.311Z</app:edited><category scheme='http://schemas.google.com/g/2005#kind'  term='http://schemas.google.com/docs/2007#spreadsheet' label='spreadsheet'/><category scheme='http://schemas.google.com/docs/2007/folders/user_email' term='My Favorite Spreadsheets' label='My Favorite Spreadsheets'/><title type='text'>Test Spreadsheet</title><content type='text/html' src='http://spreadsheets.google.com/feeds/download/spreadsheets/Export?fmcmd=102&amp;key=key'/><link rel='alternate' type='text/html' href='http://spreadsheets.google.com/ccc?key=key' /><link href='http://spreadsheets.google.com/feeds/worksheets/key/private/full'  rel='http://schemas.google.com/spreadsheets/2006#worksheetsfeed' type='application/atom+xml' /><link rel='self' type='application/atom+xml' href='http://docs.google.com/feeds/documents/private/full/spreadsheet%3Akey'/><link rel='edit' type='application/atom+xml' href='http://docs.google.com/feeds/documents/private/full/spreadsheet%3Akey'/><link rel='edit-media' type='text/html' href='http://docs.google.com/feeds/media/private/full/spreadsheet%3Akey'/><author><name>test.user</name><email>test.user@gmail.com</email></author><gd:resourceId>spreadsheet:key</gd:resourceId><gd:lastModifiedBy><name>test.user</name><email>test.user@gmail.com</email></gd:lastModifiedBy><gd:lastViewed>2009-03-10T20:22:42.987Z</gd:lastViewed><docs:writersCanInvite value='true'/><gd:feedLink rel='http://schemas.google.com/acl/2007#accessControlList' href='http://docs.google.com/feeds/acl/private/full/spreadsheet%3Akey'/></entry>";
	const gchar *xml_text= "<entry gd:etag='HxIRQkRWTip7ImBp'><id>http://docs.google.com/feeds/documents/private/full/document%3Adocument_id</id><published>2007-07-03T18:02:50.338Z</published><updated>2009-03-16T23:26:12.503Z</updated><app:edited xmlns:app='http://www.w3.org/2007/app'>2009-03-16T23:26:12.503Z</app:edited><category scheme='http://schemas.google.com/g/2005#kind' term='http://schemas.google.com/docs/2007#document' label='document'/><category scheme='http://schemas.google.com/g/2005/labels' term='http://schemas.google.com/g/2005/labels#starred' label='starred'/><title type='text'>Test Document</title><content src='http://docs.google.com/RawDocContents?action=fetch&amp;docID=document_id' type='text/html'/><link rel='alternate' type='text/html' href='http://foobar.com/Doc?id=document_id'/><link rel='self' type='application/atom+xml' href='http://docs.google.com/feeds/documents/private/full/document%3Adocument_id'/><link rel='edit' type='application/atom+xml' href='http://docs.google.com/feeds/documents/private/full/document%3Adocument_id'/><link rel='edit-media' type='text/html' href='http://docs.google.com/feeds/media/private/full/document%3Adocument_id'/><author><name>test.user</name><email>test.user@gmail.com</email></author><gd:resourceId>document:document_id</gd:resourceId><gd:lastModifiedBy><name>test.user</name><email>test.user@gmail.com</email></gd:lastModifiedBy><gd:lastViewed>2009-03-16T23:26:12.503Z</gd:lastViewed><docs:writersCanInvite value='true'/><gd:feedLink rel='http://schemas.google.com/acl/2007#accessControlList' href='http://docs.google.com/feeds/acl/private/full/document%3Adocument_id'/></entry>";
	const gchar *xml_presentation= "<entry gd:etag='HxIRQkRWTip7ImBp'><id>http://docs.google.com/feeds/documents/private/full/document%3Adocument_id</id><published>2007-07-03T18:02:50.338Z</published><updated>2009-03-16T23:26:12.503Z</updated><app:edited xmlns:app='http://www.w3.org/2007/app'>2009-03-16T23:26:12.503Z</app:edited><category scheme='http://schemas.google.com/g/2005#kind' term='http://schemas.google.com/docs/2007#document' label='document'/><category scheme='http://schemas.google.com/g/2005/labels' term='http://schemas.google.com/g/2005/labels#starred' label='starred'/><title type='text'>Test Document</title><content src='http://docs.google.com/RawDocContents?action=fetch&amp;docID=document_id' type='text/html'/><link rel='alternate' type='text/html' href='http://foobar.com/Doc?id=document_id'/><link rel='self' type='application/atom+xml' href='http://docs.google.com/feeds/documents/private/full/document%3Adocument_id'/><link rel='edit' type='application/atom+xml' href='http://docs.google.com/feeds/documents/private/full/document%3Adocument_id'/><link rel='edit-media' type='text/html' href='http://docs.google.com/feeds/media/private/full/document%3Adocument_id'/><author><name>test.user</name><email>test.user@gmail.com</email></author><gd:resourceId>document:document_id</gd:resourceId><gd:lastModifiedBy><name>test.user</name><email>test.user@gmail.com</email></gd:lastModifiedBy><gd:lastViewed>2009-03-16T23:26:12.503Z</gd:lastViewed><docs:writersCanInvite value='true'/><gd:feedLink rel='http://schemas.google.com/acl/2007#accessControlList' href='http://docs.google.com/feeds/acl/private/full/document%3Adocument_id'/></entry>";
	const gchar *xml_feed= "<?xml version='1.0' encoding='utf-8'?>\
<feed gd:etag='W/CkYFSHg-fyp7ImA9WxRVGUo.'>\
<id>http://www.google.com/calendar/feeds/default/alldocuments/full</id>\
<updated>2008-11-18T01:01:59.657Z</updated>\
<title>Coach's Calendar List</title>\
<author><name>Coach</name><email>user@gmail.com</email></author>\
<generator version='1.0' uri='http://www.google.com/calendar'>Google Calendar</generator>\
<openSearch:startIndex>1</openSearch:startIndex>\
<entry gd:etag='HxIRQkRWTip7ImBp'><id>http://docs.google.com/feeds/documents/private/full/document%3Adocument_id</id><published>2007-07-03T18:02:50.338Z</published><updated>2009-03-16T23:26:12.503Z</updated><app:edited xmlns:app='http://www.w3.org/2007/app'>2009-03-16T23:26:12.503Z</app:edited><category scheme='http://schemas.google.com/g/2005#kind' term='http://schemas.google.com/docs/2007#document' label='document'/><category scheme='http://schemas.google.com/g/2005/labels' term='http://schemas.google.com/g/2005/labels#starred' label='starred'/><title type='text'>Test Document</title><content src='http://docs.google.com/RawDocContents?action=fetch&amp;docID=document_id' type='text/html'/><link rel='alternate' type='text/html' href='http://foobar.com/Doc?id=document_id'/><link rel='self' type='application/atom+xml' href='http://docs.google.com/feeds/documents/private/full/document%3Adocument_id'/><link rel='edit' type='application/atom+xml' href='http://docs.google.com/feeds/documents/private/full/document%3Adocument_id'/><link rel='edit-media' type='text/html' href='http://docs.google.com/feeds/media/private/full/document%3Adocument_id'/><author><name>test.user</name><email>test.user@gmail.com</email></author><gd:resourceId>document:document_id</gd:resourceId><gd:lastModifiedBy><name>test.user</name><email>test.user@gmail.com</email></gd:lastModifiedBy><gd:lastViewed>2009-03-16T23:26:12.503Z</gd:lastViewed><docs:writersCanInvite value='true'/><gd:feedLink rel='http://schemas.google.com/acl/2007#accessControlList' href='http://docs.google.com/feeds/acl/private/full/document%3Adocument_id'/></entry>\
<entry gd:etag='W/CUUNSXYyfCp7ImA9WxRVGUo.'><id>http://docs.google.com/feeds/folders/private/full/folder%3folder_id/document%3Adocument_id</id><published>0001-01-03T00:00:00.000Z</published><updated>2008-09-02T05:42:27.203Z</updated><category scheme='http://schemas.google.com/g/2005#kind' term='http://schemas.google.com/docs/2007#document' label='document'/><category scheme='http://schemas.google.com/docs/2007/folders/test.user@gmail.com' term='Folder 1' label='Folder 1'/><category scheme='http://schemas.google.com/docs/2007/folders/test.user@gmail.com' term='Folder 2' label='Folder 2'/><title type='TEEEEEEEEEEET'>Document 1</title><content type='text/html' src='http://docs.google.com/feeds/download/documents/RawDocContents?'/><link rel='http://schemas.google.com/docs/2007#parent' type='application/atom+xml' href='http://docs.google.com/feeds/documents/private/full/folder%3Afolder_id' title='Folder 1'/><link rel='http://schemas.google.com/docs/2007#parent' type='application/atom+xml' href='http://docs.google.com/feeds/documents/private/full/folder%3Afolder_id2' title='Folder'/><link rel='alternate' type='text/html' href='http://docs.google.com/Doc?id=document_id'/><link rel='self' type='application/atom+xml' href='http://docs.google.com/feeds/folders/private/full/folder%3Afolder_id/document%3Adocument_id'/><link rel='edit' type='application/atom+xml' href='http://docs.google.com/feeds/folders/private/full/folder%3Afolder_id/document%3Adocument_id'/><link rel='edit-media' type='text/html' href='http://docs.google.com/feeds/media/private/full/document%3Adocument_id'/><author><name>test.user</name><email>test.user@gmail.com</email></author><gd:resourceId>folder:folder_id</gd:resourceId></entry>\
<entry gd:etag='HxIRQkRWTip7ImBp'><id>http://docs.google.com/feeds/documents/private/full/document%3Adocument_id</id><published>2007-07-03T18:02:50.338Z</published><updated>2009-03-16T23:26:12.503Z</updated><app:edited xmlns:app='http://www.w3.org/2007/app'>2009-03-16T23:26:12.503Z</app:edited><category scheme='http://schemas.google.com/g/2005#kind' term='http://schemas.google.com/docs/2007#document' label='document'/><category scheme='http://schemas.google.com/g/2005/labels' term='http://schemas.google.com/g/2005/labels#starred' label='starred'/><title type='text'>SUper Document</title><content src='http://docs.google.com/RawDocContents?action=fetch&amp;docID=document_id' type='text/html'/><link rel='alternate' type='text/html' href='http://foobar.com/Doc?id=document_id'/><link rel='self' type='application/atom+xml' href='http://docs.google.com/feeds/documents/private/full/document%3Adocument_id'/><link rel='edit' type='application/atom+xml' href='http://docs.google.com/feeds/documents/private/full/document%3Adocument_id'/><link rel='edit-media' type='text/html' href='http://docs.google.com/feeds/media/private/full/document%3Adocument_id'/><author><name>test.user</name><email>test.user@gmail.com</email></author><gd:resourceId>document:document_id</gd:resourceId><gd:lastModifiedBy><name>test.user</name><email>test.user@gmail.com</email></gd:lastModifiedBy><gd:lastViewed>2009-03-16T23:26:12.503Z</gd:lastViewed><docs:writersCanInvite value='true'/><gd:feedLink rel='http://schemas.google.com/acl/2007#accessControlList' href='http://docs.google.com/feeds/acl/private/full/document%3Adocument_id'/></entry>\
<entry gd:etag='BxAUSh5RAyp7ImBq'><id>http://docs.google.com/feeds/documents/private/full/spreadsheet%3Akey</id><published>2009-03-16T23:26:12.503Z</published><updated>2009-03-16T23:26:12.503Z</updated><app:edited xmlns:app='http://www.w3.org/2007/app'>2009-03-18T05:41:45.311Z</app:edited><category scheme='http://schemas.google.com/g/2005#kind'  term='http://schemas.google.com/docs/2007#spreadsheet' label='spreadsheet'/><category scheme='http://schemas.google.com/docs/2007/folders/user_email' term='My Favorite Spreadsheets' label='My Favorite Spreadsheets'/><title type='text'>Test Spreadsheet</title><content type='text/html' src='http://spreadsheets.google.com/feeds/download/spreadsheets/Export?fmcmd=102&amp;key=key'/><link rel='alternate' type='text/html' href='http://spreadsheets.google.com/ccc?key=key' /><link href='http://spreadsheets.google.com/feeds/worksheets/key/private/full'  rel='http://schemas.google.com/spreadsheets/2006#worksheetsfeed' type='application/atom+xml' /><link rel='self' type='application/atom+xml' href='http://docs.google.com/feeds/documents/private/full/spreadsheet%3Akey'/>\
<link rel='edit' type='application/atom+xml' href='http://docs.google.com/feeds/documents/private/full/spreadsheet%3Akey'/><link rel='edit-media' type='text/html' href='http://docs.google.com/feeds/media/private/full/spreadsheet%3Akey'/><author><name>test.user</name><email>test.user@gmail.com</email></author><gd:resourceId>spreadsheet:key</gd:resourceId><gd:lastModifiedBy><name>test.user</name><email>test.user@gmail.com</email></gd:lastModifiedBy><gd:lastViewed>2009-03-10T20:22:42.987Z</gd:lastViewed><docs:writersCanInvite value='true'/><gd:feedLink rel='http://schemas.google.com/acl/2007#accessControlList' href='http://docs.google.com/feeds/acl/private/full/spreadsheet%3Akey'/></entry></feed>";

	GDataDocumentsFolder *documents_folder = GDATA_DOCUMENTS_FOLDER ( gdata_documents_folder_new_from_xml (xml_folder, strlen (xml_folder), error_folder));
	GDataDocumentsSpreadsheet *documents_spreadsheet = GDATA_DOCUMENTS_SPREADSHEET( gdata_documents_spreadsheet_new_from_xml (xml_spreadsheet, \
															strlen(xml_spreadsheet), error_spreadsheet));

	GDataDocumentsText *documents_text = GDATA_DOCUMENTS_TEXT ( gdata_documents_text_new_from_xml (xml_text, strlen (xml_text), error_text));
	
	GDataDocumentsPresentation *documents_presentation = GDATA_DOCUMENTS_PRESENTATION ( gdata_documents_presentation_new_from_xml (xml_presentation,\
															 strlen (xml_presentation), error_presentation));
	
	GDataDocumentsFeed *feed;
	feed = GDATA_DOCUMENTS_FEED (_gdata_documents_feed_new_from_xml (feed_type, xml_feed, strlen (xml_feed) , entry_type, progress_callback, progress_user_data, error_feed));

	g_print ("\n====Folder ====\n");
	g_print ("Folder etag: %s\n" ,gdata_entry_get_etag (documents_folder));
	g_print ("Folder id: %s\n" ,gdata_entry_get_id (documents_folder));
	if (gdata_documents_entry_get_writers_can_invite (documents_folder))
		g_print ("Folder Writers_can_invite");
	g_print ("Folder content: %s\n", gdata_entry_get_content (documents_folder));
	g_print ("Folder title: %s\n" ,gdata_entry_get_title (documents_folder));
	author = gdata_documents_entry_get_last_modified_by (documents_folder);
	if (author)
		g_print ("Folder lastModified: %s\n" , author->name);
	editionTime = NULL;
	gdata_documents_entry_get_edited ( documents_folder, editionTime);


	g_print ("\n====Spreadsheet ====\n");
	g_print ("Spreadsheet etag: %s\n" ,gdata_entry_get_etag (documents_spreadsheet));
	g_print ("Spreadsheet id: %s\n" ,gdata_entry_get_id (documents_spreadsheet));
	if (gdata_documents_entry_get_writers_can_invite (documents_spreadsheet))
		g_print ("Spreadsheet Writers_can_invite\n");
	g_print ("Spreadsheet content: %s\n", gdata_entry_get_content (documents_spreadsheet));
	g_print ("Spreadsheet title: %s\n" ,gdata_entry_get_title (documents_spreadsheet));
	author = gdata_documents_entry_get_last_modified_by (documents_spreadsheet);
	if (author)
		g_print ("Spreadsheet lastModified: %s\n" , author->name);
	editionTime  = NULL;
	gdata_documents_entry_get_edited ( documents_spreadsheet, editionTime);

	g_print ("\n====Text ====\n");
	g_print ("Text etag: %s\n" ,gdata_entry_get_etag (documents_text));
	g_print ("Text id: %s\n" ,gdata_entry_get_id (documents_text));
	if (gdata_documents_entry_get_writers_can_invite (documents_text))
		g_print ("Text Writers_can_invite\n");
	g_print ("Text content: %s\n", gdata_entry_get_content (documents_text));
	g_print ("Text title: %s\n" ,gdata_entry_get_title (documents_text));
	author = gdata_documents_entry_get_last_modified_by (documents_text);
	if (author != NULL)
		g_print ("Text lastModified: %s\n" , author->name);
	editionTime  = NULL;
	gdata_documents_entry_get_edited ( documents_text, editionTime);

	g_print ("\n====Presentation ====\n");
	g_print ("Presentation etag: %s\n" ,gdata_entry_get_etag (documents_presentation));
	g_print ("Presentation id: %s\n" ,gdata_entry_get_id (documents_presentation));
	if (gdata_documents_entry_get_writers_can_invite (documents_presentation))
		g_print ("Presentation Writers_can_invite\n");
	g_print ("Presentation content: %s\n", gdata_entry_get_content (documents_presentation));
	g_print ("Presentation title: %s\n" ,gdata_entry_get_title (documents_presentation));
	author = gdata_documents_entry_get_last_modified_by (documents_presentation);
	if (author != NULL)
		g_print ("Presentation lastModified: %s\n" , author->name);
	editionTime  = NULL;
	gdata_documents_entry_get_edited ( documents_presentation, editionTime);

	g_print ("\n===== FEED ====\n");
	g_print ("Feed titlee: %s,\n", gdata_feed_get_title (feed));
	feed_entry_list = gdata_feed_get_entries (feed);
	g_print ("FirstEntry title: %s\n" ,gdata_entry_get_title (g_list_first (feed_entry_list)->data));
	g_print ("SecondeEntry title: %s\n" ,gdata_entry_get_title (g_list_first (feed_entry_list)->next->data));

	return 0;
}
