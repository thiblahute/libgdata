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
 * SECTION:gdata-documents-text
 * @short_description: GData documents text object
 * @stability: Unstable
 * @include: gdata/services/document/gdata-documents-text.h
 *
 * #GDataDocumentsText is a subclass of #GDataDocumentsEntry to represent a text from a Google Document.
 *
 * For more details of Google document' GData API, see the <ulink type="http://code.google.com/apis/document/docs/2.0/developers_guide_protocol.html">
 * online documentation</ulink>.
 **/

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <libxml/parser.h>
#include <string.h>

#include "gdata-documents-text.h"
#include "gdata-gdata.h"
#include "gdata-parser.h"
#include "gdata-types.h"
#include "gdata-private.h"

static void gdata_documents_text_finalize (GObject *object);
static void get_namespaces (GDataEntry *entry, GHashTable *namespaces);
static void get_xml (GDataEntry *entry, GString *xml_string);
static gboolean parse_xml (GDataEntry *entry, xmlDoc *doc, xmlNode *node, GError **error);


struct _GDataDocumentsTextPrivate 
{
	/*TODO*/
};

G_DEFINE_TYPE (GDataDocumentsText, gdata_documents_text, GDATA_TYPE_DOCUMENTS_ENTRY)
#define GDATA_DOCUMENTS_TEXT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_DOCUMENTS_TEXT, GDataDocumentsTextClass))

static void
gdata_documents_text_class_init (GDataDocumentsTextClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataEntryClass *entry_class = GDATA_ENTRY_CLASS (klass);
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);
	GDataDocumentsEntryClass *documents_entry_class = GDATA_DOCUMENTS_ENTRY_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataDocumentsTextPrivate));

	gobject_class->finalize = gdata_documents_text_finalize;

	entry_class->get_xml = get_xml;
	parsable_class->parse_xml = parse_xml;
	entry_class->get_namespaces = get_namespaces;

	/*TODO Properties?*/
}

static void
gdata_documents_text_init (GDataDocumentsText *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_DOCUMENTS_TEXT, GDataDocumentsTextPrivate);
}

static void
gdata_documents_text (GDataDocumentsText *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_DOCUMENTS_TEXT, GDataDocumentsTextPrivate);
}

GDataDocumentsText*
gdata_documents_text_new(const gchar *id)
{
	return g_object_new (GDATA_TYPE_DOCUMENTS_TEXT, "id", id, NULL);
}

GDataDocumentsText *
gdata_documents_text_new_from_xml (const gchar *xml, gint length, GError **error)
{
	return GDATA_DOCUMENTS_TEXT (_gdata_entry_new_from_xml (GDATA_TYPE_DOCUMENTS_TEXT, xml, length, error));
}

static gboolean
parse_xml (GDataEntry *entry, xmlDoc *doc, xmlNode *node, GError **error)
{
	GDataDocumentsText *self = GDATA_DOCUMENTS_TEXT (entry);

	g_return_val_if_fail (GDATA_IS_DOCUMENTS_TEXT (self), FALSE);
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);

	/*TODO*/

	return TRUE;
}


static void
gdata_documents_text_finalize (GObject *object)
{
	GDataDocumentsTextPrivate *priv = GDATA_DOCUMENTS_TEXT_GET_PRIVATE (object);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_documents_text_parent_class)->finalize (object);
}

static void 
get_xml (GDataEntry *entry, GString *xml_string)
{
	/*TODO*/
	;
}

static void
get_namespaces (GDataEntry *entry, GHashTable *namespaces)
{
	/*TODO check it after writing get_xml*/
	/* Chain up to the parent class */
	GDATA_ENTRY_CLASS (gdata_documents_text_parent_class)->get_namespaces (entry, namespaces);

	g_hash_table_insert (namespaces, (gchar*) "gd", (gchar*) "http://schemas.google.com/g/2005");
	g_hash_table_insert (namespaces, (gchar*) "docs", (gchar*) "http://schemas.google.com/docs/2007#document");

}

/*void 
gdata_documents_entry_add_a_mime_type (GDataDocumentsEntry *self, gchar *extension, gchar *mime_type )
{
	g_return_if_fail ( GDATA_IS_DOCUMENTS_ENTRY ( self ));
	g_hash_table_insert (self->priv->mime_types, (gchar*)extension, (gchar*)mime_type);
}*/
