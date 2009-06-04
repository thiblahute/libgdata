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
#include "common.h"

int
main (int argc, char *argv[])
{
	g_type_init ();

	GError **error = NULL;	
	const gchar *xml=\
		"<entry gd:etag='W/\"CUUNSXYyfCp7ImA9WxRVGUo.\"'> <id>http://docs.google.com/feeds/folders/private/full/folder%3folder_id/document%3Adocument_id</id>\
		  <published>0001-01-03T00:00:00.000Z</published>\
		  <updated>2008-09-02T05:42:27.203Z</updated>\
		  <category scheme=\"http://schemas.google.com/g/2005#kind\" term=\"http://schemas.google.com/docs/2007#document\" label=\"document\"/>\
		  <category scheme=\"http://schemas.google.com/docs/2007/folders/test.user@gmail.com\" term=\"Folder 1\" label=\"Folder 1\"/>\
		  <category scheme=\"http://schemas.google.com/docs/2007/folders/test.user@gmail.com\" term=\"Folder 2\" label=\"Folder 2\"/>\
		  <title type=\"text\">Document 1</title>\
		  <content type=\"text/html\" src=\"http://docs.google.com/feeds/download/documents/RawDocContents?\"/>\
		  <link rel=\"http://schemas.google.com/docs/2007#parent\" type=\"application/atom+xml\"\
			href=\"http://docs.google.com/feeds/documents/private/full/folder%3Afolder_id\" \title=\"Folder 1\"/>\
		  <link rel=\"http://schemas.google.com/docs/2007#parent\" type=\"application/atom+xml\"\
			href=\"http://docs.google.com/feeds/documents/private/full/folder%3Afolder_id2\" title=\"Folder \"/>\
		  <link rel=\"alternate\" type=\"text/html\" href=\"http://docs.google.com/Doc?id=document_id\"/>\
		  <link rel=\"self\" type=\"application/atom+xml\"\
			href=\"http://docs.google.com/feeds/folders/private/full/folder%3Afolder_id/document%3Adocument_id\"/>\
		  <link rel=\"edit\" type=\"application/atom+xml\"\
			href=\"http://docs.google.com/feeds/folders/private/full/folder%3Afolder_id/document%3Adocument_id\"/>\
		  <link rel=\"edit-media\" type=\"text/html\" href=\"http://docs.google.com/feeds/media/private/full/document%3Adocument_id\"/>\
		  <author>\
			<name>test.user</name>\
			<email>test.user@gmail.com</email>\
		  </author>\
		  <gd:resourceId>folder:folder_id</gd:resourceId>\
		</entry>";


	GDataDocumentsSpreadsheet *documents_spreadsheet = GDATA_DOCUMENTS_SPREADSHEET( gdata_documents_spreadsheet_new_from_xml (xml, strlen(xml), error) );
	GDataDocumentsPresentation *documents_presentation = GDATA_DOCUMENTS_PRESENTATION( gdata_documents_presentation_new (CLIENT_ID) );
	GDataDocumentsText *documents_text = GDATA_DOCUMENTS_TEXT ( gdata_documents_text_new (CLIENT_ID) );

	/*gdata_documents_entry_set_path (documents_spreadsheet, "spreadsheet");*/
	g_print ("etag: %s\n" ,gdata_entry_get_etag (documents_spreadsheet));
	g_print ("id: %s\n" ,gdata_entry_get_id (documents_spreadsheet));
//	gdata_documents_entry_set_writers_can_invite (documents_spreadsheet, TRUE);
	
	g_print ("content: %s\n", gdata_entry_get_content (documents_spreadsheet));

	g_print ("title: %s\n" ,gdata_entry_get_title (documents_spreadsheet));

	gdata_documents_entry_set_path (documents_text, "text");
	g_print ("2: %s\n", gdata_documents_entry_get_path (documents_text));
	g_print ("textID: %s\n", gdata_entry_get_id (documents_text));

	gdata_documents_entry_set_path (documents_presentation, "presentation");
	g_print ("3: %s\n", gdata_documents_entry_get_path (documents_presentation));

	return 0;
}
