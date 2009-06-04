/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Philip Withnall 2009 <philip@tecnocode.co.uk>
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

const gchar *gdata_documents_query_get_order_by (GDataDocumentsQuery *self);
void gdata_documents_query_set_order_by (GDataDocumentsQuery *self, const gchar *order_by);

gboolean gdata_documents_query_show_deleted (GDataDocumentsQuery *self);
void gdata_documents_query_set_show_deleted (GDataDocumentsQuery *self, gboolean show_deleted);

gboolean gdata_documents_query_get_starred (GDataDocumentsQuery *self);
void gdata_documents_query_set_starred (GDataDocumentsQuery *self, gboolean starred);

gchar *gdata_documents_query_get_folder (GDataDocumentsQuery *self);
void gdata_documents_query_set_folder (GDataDocumentsQuery *self, const gchar *folder);

GType gdata_documents_query_get_document_type (GDataDocumentsQuery *self);
void gdata_documents_query_set_document_type (GDataDocumentsQuery *self, const gchar *type);

gboolean gdata_documents_query_is_starred (GDataDocumentsQuery *self);
void gdata_documents_query_set_starred (GDataDocumentsQuery *self, gboolean starred);

gboolean gdata_documents_query_is_metadatas (GDataDocumentsQuery *self);
void gdata_documents_query_set_metadatas (GDataDocumentsQuery *self, gboolean starred);

gboolean gdata_documents_query_is_content (GDataDocumentsQuery *self);
void gdata_documents_query_set_content (GDataDocumentsQuery *self, gboolean starred);

G_END_DECLS

#endif /* !GDATA_DOCUMENTS_QUERY_H */
