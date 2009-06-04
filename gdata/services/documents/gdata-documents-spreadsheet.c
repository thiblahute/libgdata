
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
static void get_namespaces (GDataEntry *entry, GHashTable *namespaces);
static void get_xml (GDataEntry *entry, GString *xml_string);
static void gdata_documents_spreadsheet_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_documents_spreadsheet_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static gboolean parse_xml (GDataEntry *entry, xmlDoc *doc, xmlNode *node, GError **error);


struct _GDataDocumentsSpreadsheetPrivate
{
	GHashTable *fmcmd;
	gchar *key; 
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

	g_type_class_add_private (klass, sizeof (GDataDocumentsSpreadsheetPrivate));

	gobject_class->finalize = gdata_documents_spreadsheet_finalize;

	entry_class->get_xml = get_xml;
	parsable_class->parse_xml = parse_xml;
	entry_class->get_namespaces = get_namespaces;
	/**
	 * GDataDocumentsEntry:key
	 *
	 * Indicates 
	 */
	g_object_class_install_property (gobject_class, PROP_KEY,
				g_param_spec_string ("key",
					"Key", "Indicates the uri to get the documentsEntry's access Rules feed.",
					NULL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_documents_spreadsheet (GDataDocumentsSpreadsheet *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_DOCUMENTS_SPREADSHEET, GDataDocumentsSpreadsheetPrivate);
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
	GDataDocumentsSpreadsheetPrivate *priv = GDATA_DOCUMENTS_SPREADSHEET_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_KEY:
			g_value_set_string (value, priv->key);
			break;
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
		case PROP_KEY:
			gdata_documents_entry_set_key (self, g_value_get_string (value));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static gboolean
parse_xml (GDataEntry *entry, xmlDoc *doc, xmlNode *node, GError **error)
{
	GDataDocumentsSpreadsheet *self = GDATA_DOCUMENTS_SPREADSHEET (entry);

	g_return_val_if_fail (GDATA_IS_DOCUMENTS_SPREADSHEET (self), FALSE);
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);

	/*TODO*/

	return TRUE;
}

static void
gdata_documents_spreadsheet_init (GDataDocumentsSpreadsheet *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_DOCUMENTS_SPREADSHEET, GDataDocumentsSpreadsheetPrivate);
}

static void
gdata_documents_spreadsheet_finalize (GObject *object)
{
	GDataDocumentsSpreadsheetPrivate *priv = GDATA_DOCUMENTS_SPREADSHEET_GET_PRIVATE (object);

	g_free (priv->key);
	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_documents_spreadsheet_parent_class)->finalize (object);
}

static void 
get_xml (GDataEntry *entry, GString *xml_string)
{
	/*TODO*/
	;
}

/* TODO need get namespaces??*/

void 
gdata_documents_spreadsheet_add_a_fmcmd (GDataDocumentsEntry *self, gchar *fmcmd, gchar *mime_type)
{
	/*TODO*/;
}

void gdata_documents_spreadsheet_add_fmcmds (GDataDocumentsEntry *self)
{
	/*TODO*/;
}

GHashTable *gdata_documents_entry_get_fmcmds (GDataDocumentsEntry *self)
{
	/*TODO*/;
}

static void
get_namespaces (GDataEntry *entry, GHashTable *namespaces)
{
	/*TODO check it after writing get_xml*/
	/* Chain up to the parent class */
	GDATA_ENTRY_CLASS (gdata_documents_spreadsheet_parent_class)->get_namespaces (entry, namespaces);

	g_hash_table_insert (namespaces, (gchar*) "gd", (gchar*) "http://schemas.google.com/g/2005");
	g_hash_table_insert (namespaces, (gchar*) "docs", (gchar*) "http://schemas.google.com/docs/2007#document");

}

gchar*
gdata_documents_entry_get_key (GDataDocumentsSpreadsheet *self )
{
	g_return_if_fail ( GDATA_IS_DOCUMENTS_SPREADSHEET ( self ) );
 	return self->priv->key;
}

void  
gdata_documents_entry_set_key (GDataDocumentsSpreadsheet *self, const gchar *key )
{
	g_return_if_fail ( GDATA_IS_DOCUMENTS_SPREADSHEET ( self ) );
	self->priv->key = g_strdup (key);
	g_object_notify (G_OBJECT (self), "key");
}
