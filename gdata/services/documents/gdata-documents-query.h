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

#ifndef GDATA_DOCUMENTS_QUERY_H
#define GDATA_DOCUMENTS_QUERY_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-types.h>
#include <gdata/gdata-query.h>

G_BEGIN_DECLS

#define GDATA_TYPE_DOCUMENTS_QUERY		(gdata_documents_query_get_type ())
#define GDATA_DOCUMENTS_QUERY(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_DOCUMENTS_QUERY, GDataDocumentsQuery))
#define GDATA_DOCUMENTS_QUERY_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_DOCUMENTS_QUERY, GDataDocumentsQueryClass))
#define GDATA_IS_DOCUMENTS_QUERY(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_DOCUMENTS_QUERY))
#define GDATA_IS_DOCUMENTS_QUERY_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_DOCUMENTS_QUERY))
#define GDATA_DOCUMENTS_QUERY_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_DOCUMENTS_QUERY, GDataDocumentsQueryClass))

typedef struct _GDataDocumentsQueryPrivate GDataDocumentsQueryPrivate;

/**
 * GDataDocumentsQuery:
 *
 * All the fields in the #GDataDocumentsQuery structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataQuery parent;
	GDataDocumentsQueryPrivate *priv;
} GDataDocumentsQuery;

/**
 * GDataDocumentsQueryClass:
 *
 * All the fields in the #GDataDocumentsQueryClass structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataQueryClass parent;
	GDataDocumentsQueryPrivate *priv;
} GDataDocumentsQueryClass;

GType gdata_documents_query_get_type (void)G_GNUC_CONST;

GDataDocumentsQuery *gdata_documents_query_new (const gchar *q) G_GNUC_WARN_UNUSED_RESULT;
GDataDocumentsQuery *gdata_documents_query_new_with_limits (const gchar *q, gint start_index, gint max_results) G_GNUC_WARN_UNUSED_RESULT;
gboolean gdata_documents_query_get_show_deleted (GDataDocumentsQuery *self);
void gdata_documents_query_set_show_deleted (GDataDocumentsQuery *self, gboolean show_deleted);
gboolean gdata_documents_query_get_show_folders (GDataDocumentsQuery *self);
void gdata_documents_query_set_show_folders (GDataDocumentsQuery *self, gboolean show_folders);
gchar *gdata_documents_query_get_folder_id (GDataDocumentsQuery *self);
void gdata_documents_query_set_folder_id (GDataDocumentsQuery *self, gchar *folder_id);
gchar *gdata_documents_query_get_title (GDataDocumentsQuery *self);
void gdata_documents_query_set_title (GDataDocumentsQuery *self, gchar *title, gboolean exact_title);
gboolean gdata_documents_query_get_exact_title (GDataDocumentsQuery *self);
GList *gdata_documents_query_get_types (GDataDocumentsQuery *self);
void gdata_documents_query_set_types (GDataDocumentsQuery *self, GList *type);
gchar *gdata_documents_query_get_collaborators_address (GDataDocumentsQuery *self);
gchar *gdata_documents_query_get_readers_address (GDataDocumentsQuery *self);
void gdata_documents_query_add_a_reader_email_address (GDataDocumentsQuery *self, const gchar *reader_address);

G_END_DECLS

#endif /* !GDATA_DOCUMENTS_QUERY_H */
