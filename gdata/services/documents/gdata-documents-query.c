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

/**
 * SECTION:gdata-documents-query
 * @short_description: GData Documents query object
 * @stability: Unstable
 * @include: gdata/services/documents/gdata-documents-query.h
 *
 * #GDataDocumentsQuery represents a collection of query parameters specific to the Google Document service, which go above and beyond
 * those catered for by #GDataQuery.
 *
 * For more information on the custom GData query parameters supported by #GDataDocumentsQuery, see the <ulink type="http"
 * url="http://code.google.com/apis/contacts/docs/2.0/reference.html#Parameters">online documentation</ulink>.
 **/

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <string.h>

#include "gdata-documents-query.h"
#include "gdata-query.h"


struct _GDataDocumentsQueryPrivate 
{
	gboolean deleted;
	gchar *folder;
	gchar *order_by; /*date, type, */
	gchar *document_type; /*ENUM? word, spreadsheet, presentation*/
	gboolean starred;
	gboolean metadata;
	gboolean content;
};

G_DEFINE_TYPE (GDataDocumentsQuery, gdata_documents_query, GDATA_TYPE_QUERY)
#define GDATA_DOCUMENTS_QUERY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_DOCUMENTS_QUERY, GGDataDocumentsQueryPrivate))

static void
gdata_documents_query_class_init (GDataDocumentsQueryClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataQueryClass *query_class = GDATA_QUERY_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataDocumentsQueryPrivate));

/*	gobject_class->set_property = gdata_contacts_query_set_property;
	gobject_class->get_property = gdata_contacts_query_get_property;
	gobject_class->finalize = gdata_contacts_query_finalize;

	query_class->get_query_uri = get_query_uri;*/
}

static void
gdata_documents_query (GDataDocumentsQuery *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_DOCUMENTS_QUERY, GDataDocumentsQueryPrivate);
}


static void
gdata_documents_query_init (GDataDocumentsQuery *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_DOCUMENTS_QUERY, GDataDocumentsQueryPrivate);
}

