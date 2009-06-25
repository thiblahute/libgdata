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

static void gdata_documents_presentation_finalize (GObject *object);
static void get_xml (GDataEntry *entry, GString *xml_string);
static gboolean parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error);


struct _GDataDocumentsPresentationPrivate 
{
	/*TODO*/
};

G_DEFINE_TYPE (GDataDocumentsPresentation, gdata_documents_presentation, GDATA_TYPE_DOCUMENTS_ENTRY)
#define GDATA_DOCUMENTS_PRESENTATION_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_DOCUMENTS_PRESENTATION, GDataDocumentsPresentationPrivate))

static void
gdata_documents_presentation_class_init (GDataDocumentsPresentationClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);
	GDataDocumentsEntryClass *documents_entry_class = GDATA_DOCUMENTS_ENTRY_CLASS (klass);

	gobject_class->finalize = gdata_documents_presentation_finalize;

	parsable_class->get_xml = get_xml;
	parsable_class->parse_xml = parse_xml;

	/*TODO Properties?*/

}

GDataDocumentsPresentation*
gdata_documents_presentation_new(const gchar *id)
{
	return g_object_new (GDATA_TYPE_DOCUMENTS_PRESENTATION, "id", id, NULL);
}

GDataDocumentsPresentation*
gdata_documents_presentation_new_from_xml (const gchar *xml, gint length, GError **error)
{
	return GDATA_DOCUMENTS_PRESENTATION (_gdata_entry_new_from_xml (GDATA_TYPE_DOCUMENTS_PRESENTATION, xml, length, error));
}

static gboolean
parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error)
{
	GDataDocumentsPresentation *self;

	g_return_val_if_fail (GDATA_IS_DOCUMENTS_PRESENTATION (parsable), FALSE);
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);

	self = GDATA_DOCUMENTS_PRESENTATION (parsable);
	

	if (GDATA_PARSABLE_CLASS (gdata_documents_presentation_parent_class)->parse_xml (parsable, doc, node, user_data, error) == FALSE) {
		/* Error! */
		return FALSE;
	}

	/*TODO*/

	return TRUE;
}

static void
gdata_documents_presentation_init (GDataDocumentsPresentation *self)
{
	/*Nothing to be here*/
}

static void
gdata_documents_presentation_finalize (GObject *object)
{
	/*GDataDocumentsPresentationPrivate *priv = GDATA_DOCUMENTS_PRESENTATION_GET_PRIVATE (object);*/

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_documents_presentation_parent_class)->finalize (object);
}

static void 
get_xml (GDataEntry *entry, GString *xml_string)
{
	/*chain up to the parent class*/
	GDATA_PARSABLE_CLASS (gdata_documents_presentation_parent_class)->get_xml (entry, xml_string);

	gchar *document_id = gdata_documents_entry_get_document_id (GDATA_DOCUMENTS_ENTRY (entry));

	if (document_id != NULL)
		g_string_append_printf (xml_string, "<gd:resourceId>presentation:%s</gd:resourceId>", document_id);

	g_free (document_id);
}

/* gdata_documents_presentation_download_document:
 * @self: a #GDataDocumentsPresentation
 * @service: a #GDataDocumentsService
 * @content_type: return location for the document's content type, or %NULL; free with g_free()
 * @destination_folder: the destination folder
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
gdata_documents_presentation_download_document (GDataDocumentsEntry *self, GDataDocumentsService *service, gchar **content_type,
										gchar *export_format, gchar *destination_folder, gboolean replace_file_if_exist, GCancellable *cancellable, GError **error)
{
	GString *link_href;
	GFile *destination_file;
	gchar *document_id;

	/* TODO: async version */
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_PRESENTATION (self), NULL);
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_SERVICE (service), NULL);
	g_return_val_if_fail (export_format != NULL, NULL);

	document_id = gdata_documents_entry_get_document_id (self);
	g_return_val_if_fail (document_id != NULL, NULL);


	link_href = g_string_new ("http://docs.google.com/feeds/download/presentations/Export?exportFormat=");
	g_string_append_printf (link_href, "%s&docID=%s", export_format, document_id);

	/*Chain up to the parent class*/
	destination_file = gdata_documents_entry_download_document (GDATA_DOCUMENTS_ENTRY (self), service, content_type, \
			link_href->str, destination_folder, export_format, replace_file_if_exist, cancellable, error);

	g_string_free (link_href, FALSE);
	return destination_file;
}
