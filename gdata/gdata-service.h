/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Philip Withnall 2008-2009 <philip@tecnocode.co.uk>
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

#ifndef GDATA_SERVICE_H
#define GDATA_SERVICE_H

#include <glib.h>
#include <glib-object.h>
#include <libsoup/soup.h>
#include <libxml/parser.h>

#include <gdata/gdata-feed.h>

G_BEGIN_DECLS

/**
 * GDataServiceError:
 * @GDATA_SERVICE_ERROR_UNAVAILABLE: The service is unavailable due to maintainence or other reasons
 * @GDATA_SERVICE_ERROR_PROTOCOL_ERROR: The client or server unexpectedly strayed from the protocol (fatal error)
 * @GDATA_SERVICE_ERROR_WITH_QUERY: TODO
 * @GDATA_SERVICE_ERROR_ENTRY_ALREADY_INSERTED: An entry has already been inserted, and cannot be re-inserted
 * @GDATA_SERVICE_ERROR_WITH_INSERTION: TODO
 * @GDATA_SERVICE_ERROR_AUTHENTICATION_REQUIRED: The user attempted to do something which required authentication, and they weren't authenticated
 *
 * Error codes for #GDataService operations.
 **/
typedef enum {
	GDATA_SERVICE_ERROR_UNAVAILABLE = 1,
	GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
	GDATA_SERVICE_ERROR_WITH_QUERY,/* TODO: probably should die */
	GDATA_SERVICE_ERROR_ENTRY_ALREADY_INSERTED,
	GDATA_SERVICE_ERROR_WITH_INSERTION,/* TODO: probably should die */
	GDATA_SERVICE_ERROR_AUTHENTICATION_REQUIRED
} GDataServiceError;

/**
 * GDataAuthenticationError:
 * @GDATA_AUTHENTICATION_ERROR_BAD_AUTHENTICATION: The login request used a username or password that is not recognized.
 * @GDATA_AUTHENTICATION_ERROR_NOT_VERIFIED: The account email address has not been verified. The user will need to access their Google account
    directly to resolve the issue before logging in using a non-Google application.
 * @GDATA_AUTHENTICATION_ERROR_TERMS_NOT_AGREED: The user has not agreed to terms. The user will need to access their Google account directly to
    resolve the issue before logging in using a non-Google application.
 * @GDATA_AUTHENTICATION_ERROR_CAPTCHA_REQUIRED: A CAPTCHA is required. (A response with this error code will also contain an image URL and a
    CAPTCHA token.)
 * @GDATA_AUTHENTICATION_ERROR_ACCOUNT_DELETED: The user account has been deleted.
 * @GDATA_AUTHENTICATION_ERROR_ACCOUNT_DISABLED: The user account has been disabled.
 * @GDATA_AUTHENTICATION_ERROR_SERVICE_DISABLED: The user's access to the specified service has been disabled. (The user account may still be valid.)
 *
 * Error codes for #GDataDataService authentication operations. See http://code.google.com/apis/accounts/docs/AuthForInstalledApps.html#Errors for
 * the official reference.
 **/
typedef enum {
	GDATA_AUTHENTICATION_ERROR_BAD_AUTHENTICATION = 1,
	GDATA_AUTHENTICATION_ERROR_NOT_VERIFIED,
	GDATA_AUTHENTICATION_ERROR_TERMS_NOT_AGREED,
	GDATA_AUTHENTICATION_ERROR_CAPTCHA_REQUIRED,
	GDATA_AUTHENTICATION_ERROR_ACCOUNT_DELETED,
	GDATA_AUTHENTICATION_ERROR_ACCOUNT_DISABLED,
	GDATA_AUTHENTICATION_ERROR_SERVICE_DISABLED
} GDataAuthenticationError;

/**
 * GDataEntryParserFunc:
 * @doc: the XML document
 * @node: the XML node for an element; the <literal>&lt;entry&gt;</literal> element
 * @error: a #GError, or %NULL
 *
 * Callback function for operations which parse feeds, which should produce a new #GDataEntry from the provided entry XML.
 *
 * It can return any error, but #GDataParserError errors are most likely and most expected.
 *
 * Return value: a new #GDataEntry, or %NULL
 **/
typedef GDataEntry *(*GDataEntryParserFunc) (xmlDoc *doc, xmlNode *node, GError **error);

/**
 * GDataQueryProgressCallback:
 * @entry: a new #GDataEntry
 * @entry_key: the key of the entry (zero-based index of its position in the feed)
 * @entry_count: the total number of entries in the feed
 * @user_data: user data passed to the callback
 *
 * Callback function called for each #GDataEntry parsed in a #GDataFeed when loading the results of a query.
 *
 * It is called in the main thread, so there is no guarantee on the order in which the callbacks are executed,
 * or whether they will be called in a timely manner. It is, however, guaranteed that they will all be called before
 * the #GAsyncReadyCallback which signals the completion of the query is called.
 **/
typedef void (*GDataQueryProgressCallback) (GDataEntry *entry, guint entry_key, guint entry_count, gpointer user_data);

#define GDATA_TYPE_SERVICE		(gdata_service_get_type ())
#define GDATA_SERVICE(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_SERVICE, GDataService))
#define GDATA_SERVICE_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_SERVICE, GDataServiceClass))
#define GDATA_IS_SERVICE(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_SERVICE))
#define GDATA_IS_SERVICE_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_SERVICE))
#define GDATA_SERVICE_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_SERVICE, GDataServiceClass))

#define GDATA_SERVICE_ERROR		gdata_service_error_quark ()
#define GDATA_AUTHENTICATION_ERROR	gdata_authentication_error_quark ()

typedef struct _GDataServicePrivate	GDataServicePrivate;

/**
 * GDataService:
 *
 * All the fields in the #GDataService structure are private and should never be accessed directly.
 **/
typedef struct {
	GObject parent;
	GDataServicePrivate *priv;
} GDataService;

/**
 * GDataServiceClass:
 * @parent: the parent class
 * @service_name: the name of the service (for subclasses) as given in the service's GData API documentation
 * @authentication_uri: the authentication URI (for subclasses) if different from the Google ClientLogin default
 * @parse_authentication_response: a function to parse the response from the online service to an authentication request as
 * issued by gdata_service_authenticate(). It should return %TRUE if authentication was successful, and %FALSE if there was
 * an error.
 * @append_query_headers: a function to allow subclasses to append their own headers to queries before they are submitted
 * to the online service
 * @parse_error_response: a function to parse error responses to queries from the online service. It should set the error
 * from the status, reason phrase and response body it is passed.
 *
 * The class structure for the #GDataService type.
 **/
typedef struct {
	GObjectClass parent;

	const gchar *service_name;
	const gchar *authentication_uri;

	gboolean (*parse_authentication_response) (GDataService *self, guint status, const gchar *response_body, gint length, GError **error);
	void (*append_query_headers) (GDataService *self, SoupMessage *message);
	void (*parse_error_response) (GDataService *self, guint status, const gchar *reason_phrase,
				      const gchar *response_body, gint length, GError **error);
} GDataServiceClass;

GType gdata_service_get_type (void);
GQuark gdata_service_error_quark (void);
GQuark gdata_authentication_error_quark (void);

gboolean gdata_service_authenticate (GDataService *self, const gchar *username, const gchar *password, GCancellable *cancellable, GError **error);
void gdata_service_authenticate_async (GDataService *self, const gchar *username, const gchar *password,
				       GCancellable *cancellable, GAsyncReadyCallback callback, gpointer user_data);
gboolean gdata_service_authenticate_finish (GDataService *self, GAsyncResult *async_result, GError **error);

#include <gdata/gdata-query.h>

GDataFeed *gdata_service_query (GDataService *self, const gchar *feed_uri, GDataQuery *query, GDataEntryParserFunc parser_func,
				GCancellable *cancellable,
				GDataQueryProgressCallback progress_callback, gpointer progress_user_data, GError **error);
void gdata_service_query_async (GDataService *self, const gchar *feed_uri, GDataQuery *query, GDataEntryParserFunc parser_func,
				GCancellable *cancellable,
				GDataQueryProgressCallback progress_callback, gpointer progress_user_data,
				GAsyncReadyCallback callback, gpointer user_data);
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
