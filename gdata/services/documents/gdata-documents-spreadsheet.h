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

#ifndef GDATA_DOCUMENTS_SPREADSHEET_H
#define GDATA_DOCUMENTS_SPREADSHEET_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/services/documents/gdata-documents-entry.h>
/*#include <gdata/gdata-gdata.h> FIXME*/
#include <gdata/gdata-types.h>

G_BEGIN_DECLS

#define GDATA_TYPE_DOCUMENTS_SPREADSHEET			(gdata_documents_spreadsheet_get_type ())
#define GDATA_DOCUMENTS_SPREADSHEET(o)				(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_DOCUMENTS_SPREADSHEET, GDataDocumentsSpreadsheet))
#define GDATA_DOCUMENTS_SPREADSHEET_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_DOCUMENTS_SPREADSHEET, GDataDocumentsSpreadsheetClass))
#define GDATA_IS_DOCUMENTS_SPREADSHEET(o)			(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_DOCUMENTS_SPREADSHEET))
#define GDATA_IS_DOCUMENTS_SPREADSHEET_CLASS(k)		(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_DOCUMENTS_SPREADSHEET))
#define GDATA_DOCUMENTS_SPREADSHEET_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_DOCUMENTS_SPREADSHEET, GDataDocumentsSpreadsheetClass))

typedef struct _GDataDocumentsSpreadsheetPrivate GDataDocumentsSpreadsheetPrivate;

/* 
 * GDataDocumentsSpreadsheet:
 *
 * All the fields in the #GDataDocumentsSpreadsheet structure are private and should never be accessed directly.
 */

typedef struct {
	GDataDocumentsEntry parent;
	GDataDocumentsSpreadsheetPrivate *priv;
} GDataDocumentsSpreadsheet;

typedef struct {
	/*< private >*/
	GDataDocumentsEntryClass parent;
} GDataDocumentsSpreadsheetClass;

GType gdata_documents_spreadsheet_get_type (void)G_GNUC_CONST;

GDataDocumentsSpreadsheet *gdata_documents_spreadsheet_new (const gchar *id) G_GNUC_WARN_UNUSED_RESULT;
GDataDocumentsSpreadsheet *gdata_documents_spreadsheet_new_from_xml (const gchar *xml, gint length, GError **error) G_GNUC_WARN_UNUSED_RESULT;

GFile *gdata_documents_spreadsheet_download_document (GDataDocumentsEntry *self, GDataDocumentsService *service, gchar **content_type,\
										gchar *gid, gchar *fmcmd, gchar *destination_folder, gboolean replace_file_if_exist, GCancellable *cancellable, GError **error);
G_END_DECLS

#endif /* !GDATA_DOCUMENTS_SPREADSHEET_H */

