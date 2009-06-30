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
 * SECTION:gdata-documents-folder
 * @short_description: GData documents folder object
 * @stability: Unstable
 * @include: gdata/services/document/gdata-documents-folder.h
 *
 * #GDataDocumentsFolder is a subclass of #GDataDocumentsEntry to represent a folder from the Google Documents.
 *
 * For more details of Google document' GData API, see the <ulink type="http://code.google.com/apis/document/docs/2.0/developers_guide_protocol.html">
 * online documentation</ulink>.
 **/

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <libxml/parser.h>
#include <string.h>

#include "gdata-documents-folder.h"
#include "gdata-parser.h"
#include "gdata-types.h"
#include "gdata-private.h"

static void get_xml (GDataEntry *entry, GString *xml_string);


G_DEFINE_TYPE (GDataDocumentsFolder, gdata_documents_folder, GDATA_TYPE_DOCUMENTS_ENTRY)
#define GDATA_DOCUMENTS_FOLDER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_DOCUMENTS_ENTRY, GDataDocumentsEntryPrivate))

static void
gdata_documents_folder_class_init (GDataDocumentsFolderClass *klass)
{
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);
	parsable_class->get_xml = get_xml;

}

GDataDocumentsFolder*
gdata_documents_folder_new(const gchar *id)
{
	return g_object_new (GDATA_TYPE_DOCUMENTS_FOLDER, "id", id, NULL);
}

GDataDocumentsFolder*
gdata_documents_folder_new_from_xml (const gchar *xml, gint length, GError **error)
{
	return GDATA_DOCUMENTS_FOLDER (_gdata_entry_new_from_xml (GDATA_TYPE_DOCUMENTS_FOLDER, xml, length, error));
}

static void
gdata_documents_folder_init (GDataDocumentsFolder *self)
{
	/*Why am I writing it?*/
}

static void 
get_xml (GDataEntry *entry, GString *xml_string)
{
	/*chain up to the parent class*/
	GDATA_PARSABLE_CLASS (gdata_documents_folder_parent_class)->get_xml (entry, xml_string);

	gchar *document_id = gdata_documents_entry_get_document_id (GDATA_DOCUMENTS_ENTRY (entry));

	if (document_id != NULL)
		g_string_append_printf (xml_string, "<gd:resourceId>folder:%s</gd:resourceId>", document_id);
}
