/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Thibault Saunier <saunierthibault@gmail.com
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

/**
 * SECTION:gdata-documents-spreadsheet
 * @short_description: GData documents spreadsheet object
 * @stability: Unstable
 * @include: gdata/services/document/gdata-documents-spreadsheet.h
 *
 * #GDataDocumentsSpreadsheet is a subclass of #GDataDocumentsEntry to represent a spreadsheet from a Google Document.
 *
 * For more details of Google document' GData API, see the <ulink type="http://code.google.com/apis/document/docs/2.0/developers_guide_protocol.html">
 * online documentation</ulink>.
 **/

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <libxml/parser.h>
#include <string.h>

#include "gdata-documents-spreadsheet.h"
#include "gdata-parser.h"
#include "gdata-types.h"
#include "gdata-private.h"


static void get_xml (GDataParsable *parsable, GString *xml_string);

G_DEFINE_TYPE (GDataDocumentsSpreadsheet, gdata_documents_spreadsheet, GDATA_TYPE_DOCUMENTS_ENTRY)
#define GDATA_DOCUMENTS_SPREADSHEET_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_DOCUMENTS_SPREADSHEET, GDataDocumentsSpreadsheetPrivate))

static void
gdata_documents_spreadsheet_class_init (GDataDocumentsSpreadsheetClass *klass)
{
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);

	parsable_class->get_xml = get_xml;
}

GDataDocumentsSpreadsheet*
gdata_documents_spreadsheet_new(const gchar *id)
{
	return g_object_new (GDATA_TYPE_DOCUMENTS_SPREADSHEET, "id", id, NULL);
}

static void
gdata_documents_spreadsheet_init (GDataDocumentsSpreadsheet *self)
{
	/*Why am I riting it?*/
}

static void 
get_xml (GDataParsable *parsable, GString *xml_string)
{
	/*chain up to the parent class*/
	GDATA_PARSABLE_CLASS (gdata_documents_spreadsheet_parent_class)->get_xml (parsable, xml_string);

	gchar *document_id = gdata_documents_entry_get_document_id (GDATA_DOCUMENTS_ENTRY (parsable));

	if (document_id != NULL)
		g_string_append_printf (xml_string, "<gd:resourceId>spreadsheet:%s</gd:resourceId>", document_id);
}

/* gdata_documents_spreadsheet_download_document:
 * @self: a #GDataDocumentsPresentation
 * @service: a #GDataDocumentsService
 * @content_type: return location for the document's content type, or %NULL; free with g_free()
 * @gid: Usefull for %GDATA_DOCUMENTS_SPREADSHEET_CSV and %GDATA_DOCUMENTS_SPREADSHEET_TSV, else set it to -1.
 * @destination_directory: the destination folder
 * @cancellable: optional #GCancellable object, or %NULL
 * @error: a #GError, or %NULL
 *
 * Downloads and returns the documents described here. If the documents doesn't exist, %NULL is returned, but
 * no error is set in @error. TODO
 *
 * If @cancellable is not %NULL, then the operation can be cancelled by triggering the @cancellable object from another thread.
 * If the operation was cancelled, the error %G_IO_ERROR_CANCELLED will be returned.
 * If the  export format is wrong, returns NULL.
 *
 * When requesting a %GDATA_DOCUMENTS_SPREADSHEET_CSV or %GDATA_DOCUMENTS_SPREADSHEET_TSV file you must specify an additional parameter called @gid 
 * which indicates which grid, or sheet, you wish to get (the index is 0 based, so gid 1 actually refers to the
 * second sheet sheet on a given spreadsheet). 
 *
 * If there is an error getting the documents, a %GDATA_SERVICE_ERROR_WITH_QUERY error will be returned.
 *
 * Return value: the document's data, or %NULL; free with g_free()
 **/
GFile *
gdata_documents_spreadsheet_download_document (GDataDocumentsSpreadsheet *self, GDataDocumentsService *service, gchar **content_type, gint gid,
	   	GDataDocumentsSpreadsheetFormat export_format, GFile *destination_directory, gboolean replace_file_if_exist, GCancellable *cancellable, GError **error)
{
	GString *link_href;
	GFile *destination_file;
	gchar *document_id, *extension, *fmcmd;
	GDataService *spreadsheet_service;

	/* TODO: async version */
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_SPREADSHEET (self), NULL);
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_SERVICE (service), NULL);

	document_id = gdata_documents_entry_get_document_id (GDATA_DOCUMENTS_ENTRY (self));

	if (export_format == GDATA_DOCUMENTS_SPREADSHEET_XLS) {
		extension = "xls";
		fmcmd = "4";
	}else if (export_format == GDATA_DOCUMENTS_SPREADSHEET_CSV) {
		extension = "csv";
		fmcmd = "5";
	}else if (export_format == GDATA_DOCUMENTS_SPREADSHEET_PDF) {
		extension = "pdf";
		fmcmd = "12";
	}else if (export_format == GDATA_DOCUMENTS_SPREADSHEET_ODS) {
		extension = "ods";
		fmcmd = "13";
	}else if (export_format == GDATA_DOCUMENTS_SPREADSHEET_TSV) {
		extension = "tsv";
		fmcmd = "23";
	}else if (export_format == GDATA_DOCUMENTS_SPREADSHEET_HTML) {
		extension = "html";
		fmcmd = "102";
	}else{
		return NULL;
	}

	g_return_val_if_fail (document_id != NULL, NULL);
	g_return_val_if_fail (extension != NULL, NULL);
	g_return_val_if_fail (fmcmd != NULL, NULL);

	link_href = g_string_new ("http://spreadsheets.google.com/feeds/download/spreadsheets/Export?key=");
	g_string_append_printf (link_href, "%s&fmcmd=%s", document_id, fmcmd);

	if (gid != -1)
		g_string_append_printf (link_href, "&gid=%d", gid); /*TODO*/

	/*Get the spreadsheet service*/
	spreadsheet_service = gdata_documents_service_get_spreadsheet_service (service);
	/*Chain up to the parent class*/
	destination_file = _gdata_documents_entry_download_document (GDATA_DOCUMENTS_ENTRY (self), spreadsheet_service, content_type, link_href->str, destination_directory, extension, replace_file_if_exist, cancellable, error);

	g_string_free (link_href, TRUE);
	return destination_file;
}
