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
#include "gdata-gdata.h"
#include "gdata-parser.h"
#include "gdata-types.h"
#include "gdata-private.h"

static void gdata_documents_spreadsheet_finalize (GObject *object);
static void get_xml (GDataEntry *entry, GString *xml_string);
static void gdata_documents_spreadsheet_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_documents_spreadsheet_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static gboolean parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error);

struct _GDataDocumentsSpreadsheetPrivate
{
};

enum {
	PROP_KEY
};

G_DEFINE_TYPE (GDataDocumentsSpreadsheet, gdata_documents_spreadsheet, GDATA_TYPE_DOCUMENTS_ENTRY)
#define GDATA_DOCUMENTS_SPREADSHEET_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_DOCUMENTS_SPREADSHEET, GDataDocumentsSpreadsheetPrivate))

static void
gdata_documents_spreadsheet_class_init (GDataDocumentsSpreadsheetClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataDocumentsEntryClass *documents_entry_class = GDATA_DOCUMENTS_ENTRY_CLASS (klass);
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);
	GDataEntryClass *entry_class = GDATA_ENTRY_CLASS (klass);


	gobject_class->finalize = gdata_documents_spreadsheet_finalize;

	entry_class->get_xml = get_xml;
	parsable_class->parse_xml = parse_xml;
}

static void
gdata_documents_spreadsheet (GDataDocumentsSpreadsheet *self)
{
	/*Nothing to be here*/
}

GDataDocumentsSpreadsheet*
gdata_documents_spreadsheet_new(const gchar *id)
{
	return g_object_new (GDATA_TYPE_DOCUMENTS_SPREADSHEET, "id", id, NULL);
}

GDataDocumentsSpreadsheet *
gdata_documents_spreadsheet_new_from_xml (const gchar *xml, gint length, GError **error)
{
	return GDATA_DOCUMENTS_SPREADSHEET (_gdata_entry_new_from_xml (GDATA_TYPE_DOCUMENTS_SPREADSHEET, xml, length, error));
}

static void
gdata_documents_entry_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	/*GDataDocumentsSpreadsheetPrivate *priv = GDATA_DOCUMENTS_SPREADSHEET_GET_PRIVATE (object);*/

	switch (property_id) {
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gdata_documents_entry_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataDocumentsEntry *self = GDATA_DOCUMENTS_ENTRY (object);

	switch (property_id) {
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static gboolean
parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error)
{
	GDataDocumentsSpreadsheet *self;

	g_return_val_if_fail (GDATA_IS_DOCUMENTS_SPREADSHEET (parsable), FALSE);
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);

	self = GDATA_DOCUMENTS_SPREADSHEET (parsable);
	

	if (GDATA_PARSABLE_CLASS (gdata_documents_spreadsheet_parent_class)->parse_xml (parsable, doc, node, user_data, error) == FALSE) {
		/* Error! */
		return FALSE;
	}

	/*TODO*/

	return TRUE;
}

static void
gdata_documents_spreadsheet_init (GDataDocumentsSpreadsheet *self)
{
	/*Nothing to be here*/
}

static void
gdata_documents_spreadsheet_finalize (GObject *object)
{

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_documents_spreadsheet_parent_class)->finalize (object);
}

static void 
get_xml (GDataEntry *entry, GString *xml_string)
{
	/*chain up to the parent class*/
	GDATA_ENTRY_CLASS (gdata_documents_spreadsheet_parent_class)->get_xml (entry, xml_string);

	gchar *document_id = gdata_documents_entry_get_document_id (GDATA_DOCUMENTS_ENTRY (entry));

	if (document_id != NULL)
		g_string_append_printf (xml_string, "<gd:resourceId>spreadsheet:%s</gd:resourceId>", document_id);

	g_free (document_id);
}

/* gdata_documents_spreadsheet_download_document:
 * @self: a #GDataDocumentsPresentation
 * @service: a #GDataDocumentsService
 * @length: return location for the document length, in bytes
 * @content_type: return location for the document's content type, or %NULL; free with g_free()
 * @gid: refers to the number of the spreasheet sheet you want to download, -1 if you want to download all.
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
gchar *
gdata_documents_spreadsheet_download_document (GDataDocumentsEntry *self, GDataDocumentsService *service, gsize *length, gchar **content_type,
										gchar *gid, gchar *fmcmd, GCancellable *cancellable, GError **error)
{
	GString *link_href;
	gchar *data;

	/* TODO: async version */
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_SPREADSHEET (self), NULL);
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_SERVICE (service), NULL);
	g_return_val_if_fail (length != NULL, NULL);

	link_href = g_string_new ("http://spreadsheets.google.com/feeds/download/spreadsheets/Export?key=");
	if (fmcmd != NULL)
		g_print ("FMCMD: %s\n", fmcmd);

	g_string_append_printf (link_href, "%s&fmcmd=%s", gdata_documents_entry_get_document_id (self), fmcmd);

	if (strcmp (gid, "-1") != 0)
		g_string_append_printf (link_href, "&gid=%d", gid);

	/*Chain up to the parent class*/
	data = gdata_documents_entry_download_document (GDATA_DOCUMENTS_ENTRY (self), service, length, content_type, link_href->str, cancellable, error);

	return data;
}
