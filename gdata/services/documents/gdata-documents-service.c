
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
 * SECTION:gdata-documents-service
 * @short_description: GData Documents service object
 * @stability: Unstable
 * @include: gdata/services/documents/gdata-documents-service.h
 *
 * #GDataDocumentsService is a subclass of #GDataService for communicating with the GData API of Google Documents. It supports querying
 * for, inserting, editing and deleting events from documents, as well as operations on the documents themselves.
 *
 * For more details of Google Document's GData API, see the <ulink type="http" url:"http://code.google.com/apis/document/docs/2.0/reference.html"
 * online documentation</ulink>. 
 *
 * Fore more details about the spreadsheet downloads handling, see the <ulink type="http" url:"http://groups.google.com/group/Google-Docs-Data-APIs/browse_thread/thread/bfc50e94e303a29a?pli=1"
 * Online explaination about the problem</ulink>
 **/

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <libsoup/soup.h>
#include <string.h>

#include "gdata-documents-service.h"
#include "gdata-service.h"
#include "gdata-private.h"
#include "gdata-documents-query.h"

static void gdata_documents_service_finalize (GObject *object);
static void gdata_documents_service_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_documents_service_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void notify_authenticated_cb (GObject *service, GParamSpec *pspec, GObject *self);

struct _GDataDocumentsServicePrivate {
	GDataService *spreadsheet_service;
};

enum {
	PROP_SPREADSHEET_SERVICE = 1
};

G_DEFINE_TYPE (GDataDocumentsService, gdata_documents_service, GDATA_TYPE_SERVICE)
#define GDATA_DOCUMENTS_SERVICE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_DOCUMENTS_SERVICE, GDataDocumentsServicePrivate))

static void
gdata_documents_service_class_init (GDataDocumentsServiceClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataServiceClass *service_class = GDATA_SERVICE_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataDocumentsServicePrivate));

	gobject_class->set_property = gdata_documents_service_set_property;
	gobject_class->get_property = gdata_documents_service_get_property;
	service_class->service_name = "writely";
	service_class->feed_type = GDATA_TYPE_DOCUMENTS_FEED;


	/**
	 * GDataService:spreadsheet_service:
	 *
	 * Another service for spreadsheets.
	 *
	 * Fore more details about the spreadsheet downloads handling, see the <ulink type="http" url:"http://groups.google.com/group/Google-Docs-Data-APIs/browse_thread/thread/bfc50e94e303a29a?pli=1"
	 * Online explaination about the problem</ulink>
	 **/
	g_object_class_install_property (gobject_class, PROP_SPREADSHEET_SERVICE,
				g_param_spec_pointer ("spreadsheet-service",
					"Spreadsheet service", "Another service for spreadsheets.",
					 G_PARAM_READWRITE));

}

static void
gdata_documents_service_init (GDataDocumentsService *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_DOCUMENTS_SERVICE, GDataDocumentsServicePrivate);
	g_signal_connect (self, "notify::authenticated", G_CALLBACK (notify_authenticated_cb), NULL);
}

/**
 * gdata_documents_service_new:
 * @client_id: your application's client ID
 *
 * Creates a new #GDataDocumentsService. The @client_id must be unique for your application, and as registered with Google.
 *
 * Return value: a new #GDataDocumentsService, or %NULL
 **/
GDataDocumentsService *
gdata_documents_service_new (const gchar *client_id)
{
	g_return_val_if_fail (client_id != NULL, NULL);

	return g_object_new (GDATA_TYPE_DOCUMENTS_SERVICE,
			     "client-id", client_id,
			     NULL);
}

static void 
gdata_documents_service_finalize (GObject *object)
{
	GDataDocumentsServicePrivate *priv = GDATA_DOCUMENTS_SERVICE_GET_PRIVATE (object);

	gdata_service_finalize (GDATA_SERVICE (priv->spreadsheet_service));
}
static void 
gdata_documents_service_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataDocumentsServicePrivate *priv = GDATA_DOCUMENTS_SERVICE_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_SPREADSHEET_SERVICE:
			g_value_set_pointer (value, priv->spreadsheet_service);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}
static void 
gdata_documents_service_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataDocumentsServicePrivate *priv = GDATA_DOCUMENTS_SERVICE_GET_PRIVATE (object);

	switch (property_id) {
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}

}


/**
 * gdata_documents_service_query_all_documents:
 * @self: a #GDataDocumentsService
 * @query: a #GDataQuery with the query parameters, or %NULL
 * @cancellable: optional #GCancellable object, or %NULL
 * @progress_callback: a #GDataQueryProgressCallback to call when an entry is loaded, or %NULL
 * @progress_user_data: data to pass to the @progress_callback function
 * @error: a #GError, or %NULL
 *
 * Queries the service to return a list of documents matching the given @query.
 *
 * For more details, see gdata_documents_query().
 *
 * Return value: a #GDataDocumentsFeed of query results; unref with g_object_unref()
 **/
GDataDocumentsFeed*
gdata_documents_service_query_documents(	GDataDocumentsService *self, GDataDocumentsQuery *query,
												GCancellable *cancellable, GDataQueryProgressCallback progress_callback,
												gpointer progress_user_data, GError **error)
{
	GDataDocumentsFeed *feed;
	GList *i;

	/* Ensure we're authenticated first */
	if (gdata_service_is_authenticated (GDATA_SERVICE (self)) == FALSE) {
		g_set_error_literal (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_AUTHENTICATION_REQUIRED,
				     _("You must be authenticated to query contacts."));
		return NULL;
	}

	feed = GDATA_DOCUMENTS_FEED (gdata_service_query (GDATA_SERVICE (self), "http://docs.google.com/feeds/documents/private/full", GDATA_QUERY (query),
				    GDATA_TYPE_DOCUMENTS_ENTRY, cancellable, progress_callback, progress_user_data, error));

	for (i = gdata_feed_get_entries (feed); i != NULL; i = i->next)
	{
		gdata_documents_entry_set_access_rules (i->data, self, cancellable,  progress_callback,	progress_user_data, error);
	}

	return feed;
}


/**
 * gdata_documents_service_query_all_documents_async:
 * @self: a #GDataDocumentsService
 * @query: a #GDataQuery with the query parameters, or %NULL
 * @cancellable: optional #GCancellable object, or %NULL
 * @progress_callback: a #GDataQueryProgressCallback to call when an entry is loaded, or %NULL
 * @progress_user_data: data to pass to the @progress_callback function
 * @callback: a #GAsyncReadyCallback to call when authentication is finished
 * @user_data: data to pass to the @callback function
 *
 * Queries the service to return a list of documents matching the given @query. @self and
 * @query are all reffed when this function is called, so can safely be unreffed after this function returns.
 *
 * For more details, see gdata_documents_service_query_all(), which is the synchronous version of this function,
 * and gdata_service_query_async(), which is the base asynchronous query function.
 **/
void*
gdata_documents_service_query_documents_async (GDataDocumentsService *self, GDataDocumentsQuery *query, GCancellable *cancellable,\
					     GDataQueryProgressCallback progress_callback, gpointer progress_user_data,
					     GAsyncReadyCallback callback, gpointer user_data)
{
	/* Ensure we're authenticated first */
	if (gdata_service_is_authenticated (GDATA_SERVICE (self)) == FALSE) {
		g_simple_async_report_error_in_idle (G_OBJECT (self), callback, user_data,
						     GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_AUTHENTICATION_REQUIRED,
						     _("You must be authenticated to query contacts."));
		return;
	}

	gdata_service_query_async (GDATA_SERVICE (self), "http://docs.google.com/feeds/documents/private/full", GDATA_QUERY (query),
				   GDATA_TYPE_DOCUMENTS_ENTRY, cancellable, progress_callback, progress_user_data, callback, user_data);
}

/**
 * gdata_documents_service_upload_document:
 * @self: a #GDataDocumentsService
 * @document_entry : the #GDataDocumentsEntry to insert
 * @document : the document to upload or %NULL if uploading without datas.
 * @metadata: TRUE if upload with metadata otherwise FALSE
 * @cancellable: optional #GCancellable object, or %NULL
 * @error: a #GError, or %NULL
 *
 * Upload @document to the online if not NULL otherwise create an empty document to the google documents service.
 *
 *
 * For more details, see gdata_service_insert_entry().
 *
 * Return value: an updated #GDataDocumentsEntry, or %NULL
 **/
GDataDocumentsEntry *
gdata_documents_service_upload_document (GDataDocumentsService *self, GDataDocumentsEntry *document_entry, GFile *document,\
		gboolean metadata, GCancellable *cancellable, GError **error)
{
	/* TODO: Async variant */
	gchar *uri;
	GDataEntry *entry;

	g_return_val_if_fail (GDATA_IS_DOCUMENTS_SERVICE (self), NULL);
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_ENTRY (document_entry), NULL);

	entry = gdata_service_insert_entry (GDATA_SERVICE (self), uri, GDATA_ENTRY (entry), cancellable, error);
	g_free (uri);

	return GDATA_DOCUMENTS_ENTRY (entry);
}

static void
notify_authenticated_cb (GObject *service, GParamSpec *pspec, GObject *self)
{
	GDataDocumentsServicePrivate *priv = GDATA_DOCUMENTS_SERVICE_GET_PRIVATE (GDATA_DOCUMENTS_SERVICE (service));

	GDataService *spreadsheet_service = g_object_new (GDATA_TYPE_SERVICE, "client-id", gdata_service_get_client_id (service), NULL);
	GDATA_SERVICE_GET_CLASS (spreadsheet_service)->service_name = "wise";
	gdata_service_authenticate (spreadsheet_service, gdata_service_get_username (service), gdata_service_get_password (service), NULL, NULL);
	priv->spreadsheet_service = spreadsheet_service;
}

GDataService *
gdata_documents_service_get_spreadsheet_service(GDataDocumentsService *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_SERVICE (self), NULL);
	g_return_val_if_fail (self->priv->spreadsheet_service !=NULL, NULL);
	return self->priv->spreadsheet_service;
}
