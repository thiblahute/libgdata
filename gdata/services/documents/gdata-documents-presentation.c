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

/**
 * SECTION:gdata-documents-presentation
 * @short_description: GData documents presentation object
 * @stability: Unstable
 * @include: gdata/services/document/gdata-documents-presentation.h
 *
 * #GDataDocumentsPresentation is a subclass of #GDataDocumentsEntry to represent a presentation from a Google Document.
 *
 * For more details of Google document' GData API, see the <ulink type="http://code.google.com/apis/document/docs/2.0/developers_guide_protocol.html">
 * online documentation</ulink>.
 **/

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <libxml/parser.h>
#include <string.h>

#include "gdata-documents-presentation.h"
#include "gdata-documents-service.h"
#include "gdata-parser.h"
#include "gdata-types.h"
#include "gdata-private.h"

static void get_xml (GDataParsable *parsable, GString *xml_string);

G_DEFINE_TYPE (GDataDocumentsPresentation, gdata_documents_presentation, GDATA_TYPE_DOCUMENTS_ENTRY)
#define GDATA_DOCUMENTS_PRESENTATION_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_DOCUMENTS_PRESENTATION, GDataDocumentsPresentationPrivate))

static void
gdata_documents_presentation_class_init (GDataDocumentsPresentationClass *klass)
{
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);

	parsable_class->get_xml = get_xml;
}

GDataDocumentsPresentation*
gdata_documents_presentation_new(const gchar *id)
{
	return g_object_new (GDATA_TYPE_DOCUMENTS_PRESENTATION, "id", id, NULL);
}

static void
gdata_documents_presentation_init (GDataDocumentsPresentation *self)
{
	/*Why am I writing it?*/
}

static void 
get_xml (GDataParsable *parsable, GString *xml_string)
{
	/*chain up to the parent class*/
	GDATA_PARSABLE_CLASS (gdata_documents_presentation_parent_class)->get_xml (parsable, xml_string);

	gchar *document_id = gdata_documents_entry_get_document_id (GDATA_DOCUMENTS_ENTRY (parsable));

	if (document_id != NULL)
		g_string_append_printf (xml_string, "<gd:resourceId>presentation:%s</gd:resourceId>", document_id);
}

/* gdata_documents_presentation_download_document:
 * @self : a #GDataDocumentsPresentation
 * @service : a #GDataDocumentsService
 * @content_type : return location for the document's content type, or %NULL; free with g_free()
 * @export_format : a #GDataDocumentsPresentationFormat export format
 * @destination_directory: the destination folder
 * @cancellable: optional #GCancellable object, or %NULL
 * @error: a #GError, or %NULL
 *
 * Downloads and returns the documents described here. If the documents doesn't exist, %NULL is returned, but
 * no error is set in @error. TODO
 *
 * If @cancellable is not %NULL, then the operation can be cancelled by triggering the @cancellable object from another thread.
 * If the operation was cancelled, the error %G_IO_ERROR_CANCELLED will be returned.
 *
 * If there is an error getting the documents, a %GDATA_SERVICE_ERROR_WITH_QUERY error will be returned.
 *
 * Return value: the document's data, or %NULL; free with g_free()
 **/
GFile *
gdata_documents_presentation_download_document (GDataDocumentsPresentation *self, GDataDocumentsService *service, gchar **content_type,
											    GDataDocumentsPresentationFormat export_format, GFile *destination_directory,
												gboolean replace_file_if_exist, GCancellable *cancellable, GError **error)
{
	GFile *destination_file;
	gchar *document_id, *export_format_str=NULL, *link_href;

	/* TODO: async version */
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_PRESENTATION (self), NULL);
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_SERVICE (service), NULL);

	document_id = gdata_documents_entry_get_document_id (GDATA_DOCUMENTS_ENTRY (self));
	g_return_val_if_fail (document_id != NULL, NULL);

	if (export_format == GDATA_DOCUMENTS_PRESENTATION_PDF)
		export_format_str = "pdf"; 
	else if (export_format == GDATA_DOCUMENTS_PRESENTATION_PNG)
		export_format_str = "png"; 
	else if (export_format == GDATA_DOCUMENTS_PRESENTATION_PPT)
		export_format_str = "ppt"; 
	else if (export_format == GDATA_DOCUMENTS_PRESENTATION_SWF)
		export_format_str = "swf"; 
	else if (export_format == GDATA_DOCUMENTS_PRESENTATION_TXT)
		export_format_str = "txt"; 
	g_return_val_if_fail (export_format_str != NULL, NULL);

	link_href = g_strdup_printf ("http://docs.google.com/feeds/download/presentations/Export?exportFormat=%s&docID=%s", export_format_str, document_id);

	/*Chain up to the parent class*/
	destination_file = _gdata_documents_entry_download_document (GDATA_DOCUMENTS_ENTRY (self), GDATA_SERVICE (service), content_type, \
			link_href, destination_directory, export_format_str, replace_file_if_exist, cancellable, error);

	g_free (link_href);
	return destination_file;
}
