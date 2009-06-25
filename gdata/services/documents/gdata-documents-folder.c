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

static void gdata_documents_folder_finalize (GObject *object);
static void get_xml (GDataEntry *entry, GString *xml_string);
static gboolean parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error);


struct _GDataDocumentsFolderPrivate 
{
	/*TODO*/
};

G_DEFINE_TYPE (GDataDocumentsFolder, gdata_documents_folder, GDATA_TYPE_DOCUMENTS_ENTRY)
#define GDATA_DOCUMENTS_FOLDER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_DOCUMENTS_ENTRY, GDataDocumentsEntryPrivate))

static void
gdata_documents_folder_class_init (GDataDocumentsFolderClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataEntryClass *entry_class = GDATA_ENTRY_CLASS (klass);
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);
	GDataDocumentsEntryClass *documents_entry_class = GDATA_DOCUMENTS_ENTRY_CLASS (klass);

	gobject_class->finalize = gdata_documents_folder_finalize;

	parsable_class->get_xml = get_xml;
	parsable_class->parse_xml = parse_xml;

	/*TODO Properties?*/

}

static void
gdata_documents_folder (GDataDocumentsFolder *self)
{
	/*Nothing to be here*/
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

static gboolean
parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error)
{
	GDataDocumentsFolder *self;

	g_return_val_if_fail (GDATA_IS_DOCUMENTS_FOLDER (parsable), FALSE);
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);

	self = GDATA_DOCUMENTS_FOLDER (parsable);
	

	if (GDATA_PARSABLE_CLASS (gdata_documents_folder_parent_class)->parse_xml (parsable, doc, node, user_data, error) == FALSE) {
		/* Error! */
		return FALSE;
	}

	/*TODO*/

	return TRUE;
}

static void
gdata_documents_folder_init (GDataDocumentsFolder *self)
{
}

static void
gdata_documents_folder_finalize (GObject *object)
{
	/*GDataDocumentsFolderPrivate *priv = GDATA_DOCUMENTS_FOLDER_GET_PRIVATE (object);*/

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_documents_folder_parent_class)->finalize (object);
}

static void 
get_xml (GDataEntry *entry, GString *xml_string)
{
	/*chain up to the parent class*/
	GDATA_PARSABLE_CLASS (gdata_documents_folder_parent_class)->get_xml (entry, xml_string);

	gchar *document_id = gdata_documents_entry_get_document_id (GDATA_DOCUMENTS_ENTRY (entry));

	if (document_id != NULL)
		g_string_append_printf (xml_string, "<gd:resourceId>folder:%s</gd:resourceId>", document_id);
	g_free (document_id);
}
