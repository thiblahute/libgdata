/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Philip Withnall 2008-2009 <philip@tecnocode.co.uk>
 * 
 * GData Client is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GData Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GData Client.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GDATA_SERVICE_H
#define GDATA_SERVICE_H

#include <glib.h>
#include <glib-object.h>
#include <libsoup/soup.h>
#include <libxml/parser.h>

#include "gdata-feed.h"

G_BEGIN_DECLS

typedef enum {
	GDATA_SERVICE_ERROR_UNAVAILABLE = 1,
	GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
	GDATA_SERVICE_ERROR_WITH_QUERY,
	GDATA_SERVICE_ERROR_ENTRY_ALREADY_INSERTED,
	GDATA_SERVICE_ERROR_WITH_INSERTION,
	GDATA_SERVICE_ERROR_AUTHENTICATION_REQUIRED
} GDataServiceError;

/* http://code.google.com/apis/accounts/docs/AuthForInstalledApps.html#Errors */
typedef enum {
	GDATA_AUTHENTICATION_ERROR_BAD_AUTHENTICATION = 1,
	GDATA_AUTHENTICATION_ERROR_NOT_VERIFIED,
	GDATA_AUTHENTICATION_ERROR_TERMS_NOT_AGREED,
	GDATA_AUTHENTICATION_ERROR_CAPTCHA_REQUIRED,
	GDATA_AUTHENTICATION_ERROR_ACCOUNT_DELETED,
	GDATA_AUTHENTICATION_ERROR_ACCOUNT_DISABLED,
	GDATA_AUTHENTICATION_ERROR_SERVICE_DISABLED
} GDataAuthenticationError;

typedef GDataEntry *(*GDataEntryParserFunc) (xmlDoc *doc, xmlNode *node, GError **error);

#define GDATA_TYPE_SERVICE		(gdata_service_get_type ())
#define GDATA_SERVICE(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_SERVICE, GDataService))
#define GDATA_SERVICE_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_SERVICE, GDataServiceClass))
#define GDATA_IS_SERVICE(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_SERVICE))
#define GDATA_IS_SERVICE_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_SERVICE))
#define GDATA_SERVICE_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_SERVICE, GDataServiceClass))

#define GDATA_SERVICE_ERROR		gdata_service_error_quark ()
#define GDATA_AUTHENTICATION_ERROR	gdata_authentication_error_quark ()

typedef struct _GDataServicePrivate	GDataServicePrivate;

typedef struct {
	GObject parent;
	GDataServicePrivate *priv;
} GDataService;

typedef struct {
	GObjectClass parent;

	const gchar *service_name;
	const gchar *authentication_uri;

	gboolean (*parse_authentication_response) (GDataService *self, const gchar *response_body, GError **error);
	void (*append_query_headers) (GDataService *self, SoupMessage *message);
} GDataServiceClass;

GType gdata_service_get_type (void);
GQuark gdata_service_error_quark (void);
GQuark gdata_authentication_error_quark (void);

gboolean gdata_service_authenticate (GDataService *self, const gchar *username, const gchar *password, GCancellable *cancellable, GError **error);
void gdata_service_authenticate_async (GDataService *self, const gchar *username, const gchar *password,
				       GCancellable *cancellable, GAsyncReadyCallback callback, gpointer user_data);
gboolean gdata_service_authenticate_finish (GDataService *self, GAsyncResult *async_result, GError **error);

#include "gdata-query.h"

GDataFeed *gdata_service_query (GDataService *self, const gchar *feed_uri, GDataQuery *query, GDataEntryParserFunc parser_func,
				GCancellable *cancellable, GError **error);
void gdata_service_query_async (GDataService *self, const gchar *feed_uri, GDataQuery *query, GDataEntryParserFunc parser_func,
				GCancellable *cancellable, GAsyncReadyCallback callback, gpointer user_data);
GDataFeed *gdata_service_query_finish (GDataService *self, GAsyncResult *async_result, GError **error);

gboolean gdata_service_insert_entry (GDataService *self, const gchar *upload_uri, GDataEntry *entry, GCancellable *cancellable, GError **error);
/* TODO: async versions */

gboolean gdata_service_is_authenticated (GDataService *self);
const gchar *gdata_service_get_client_id (GDataService *self);
const gchar *gdata_service_get_username (GDataService *self);
const gchar *gdata_service_get_password (GDataService *self);
SoupSession *gdata_service_get_session (GDataService *self);

G_END_DECLS

#endif /* !GDATA_SERVICE_H */
