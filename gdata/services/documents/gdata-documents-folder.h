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

#ifndef GDATA_DOCUMENTS_FOLDER_H
#define GDATA_DOCUMENTS_FOLDER_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/services/documents/gdata-documents-entry.h>
#include <gdata/gdata-types.h>

G_BEGIN_DECLS

#define GDATA_TYPE_DOCUMENTS_FOLDER			(gdata_documents_folder_get_type ())
#define GDATA_DOCUMENTS_FOLDER(o)				(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_DOCUMENTS_FOLDER, GDataDocumentsFolder))
#define GDATA_DOCUMENTS_FOLDER_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_DOCUMENTS_FOLDER, GDataDocumentsFolderClass))
#define GDATA_IS_DOCUMENTS_FOLDER(o)			(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_DOCUMENTS_FOLDER))
#define GDATA_IS_DOCUMENTS_FOLDER_CLASS(k)		(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_DOCUMENTS_FOLDER))
#define GDATA_DOCUMENTS_FOLDER_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_DOCUMENTS_FOLDER, GDataDocumentsFolderClass))

typedef struct _GDataDocumentsFolderPrivate GDataDocumentsFolderPrivate;

/* 
 * GDataDocumentsFolder:
 *
 * All the fields in the #GDataDocumentsFolder structure are private and should never be accessed directly.
 */

typedef struct {
	GDataDocumentsEntry parent;
	GDataDocumentsFolderPrivate *priv;
} GDataDocumentsFolder;

typedef struct {
	/*< private >*/
	GDataDocumentsEntryClass parent;
} GDataDocumentsFolderClass;

GType gdata_documents_folder_get_type (void)G_GNUC_CONST;

GDataDocumentsFolder *gdata_documents_folder_new (const gchar *id) G_GNUC_WARN_UNUSED_RESULT;
GDataDocumentsFolder *gdata_documents_folder_new_from_xml (const gchar *xml, gint length, GError **error) G_GNUC_WARN_UNUSED_RESULT;


G_END_DECLS

#endif /* !GDATA_DOCUMENTS_FOLDER_H */
