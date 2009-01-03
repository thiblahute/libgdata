/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Philip Withnall 2008 <philip@tecnocode.co.uk>
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

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <libsoup/soup.h>
#include <string.h>

#include "gdata-service.h"
#include "gdata-private.h"

GQuark
gdata_service_error_quark (void)
{
	return g_quark_from_static_string ("gdata-service-error-quark");
}

static void gdata_service_dispose (GObject *object);
static void gdata_service_finalize (GObject *object);
static void gdata_service_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_service_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
/*static gboolean real_authenticate (GDataService *self, const gchar *username, const gchar *password, GError **error);*/
static gboolean real_parse_authentication_response (GDataService *self, const gchar *response_body, GError **error);

struct _GDataServicePrivate {
	SoupSession *session;
	gchar *auth_token;
	gchar *client_id;
	gboolean logged_in; /* TODO: propertyise */
};

enum {
	PROP_CLIENT_ID = 1
};

G_DEFINE_TYPE (GDataService, gdata_service, G_TYPE_OBJECT)
#define GDATA_SERVICE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_SERVICE, GDataServicePrivate))

static void
gdata_service_class_init (GDataServiceClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataServicePrivate));

	gobject_class->set_property = gdata_service_set_property;
	gobject_class->get_property = gdata_service_get_property;
	gobject_class->dispose = gdata_service_dispose;
	gobject_class->finalize = gdata_service_finalize;

	klass->service_name = "xapi";
	klass->authentication_uri = "https://www.google.com/accounts/ClientLogin";
	klass->parse_authentication_response = real_parse_authentication_response;

	g_object_class_install_property (gobject_class, PROP_CLIENT_ID,
				g_param_spec_string ("client-id",
					"Client ID", "A client ID for your application (see http://code.google.com/apis/youtube/2.0/developers_guide_protocol_api_query_parameters.html#clientsp).",
					NULL,
					G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_service_init (GDataService *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_SERVICE, GDataServicePrivate);
	self->priv->session = soup_session_async_new ();
}

static void
gdata_service_dispose (GObject *object)
{
	GDataServicePrivate *priv = GDATA_SERVICE_GET_PRIVATE (object);

	if (priv->session != NULL)
		g_object_unref (priv->session);
	priv->session = NULL;

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_service_parent_class)->dispose (object);
}

static void
gdata_service_finalize (GObject *object)
{
	GDataServicePrivate *priv = GDATA_SERVICE_GET_PRIVATE (object);

	g_free (priv->auth_token);
	g_free (priv->client_id);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_service_parent_class)->finalize (object);
}

static void
gdata_service_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataServicePrivate *priv = GDATA_SERVICE_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_CLIENT_ID:
			g_value_set_string (value, priv->client_id);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gdata_service_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataServicePrivate *priv = GDATA_SERVICE_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_CLIENT_ID:
			priv->client_id = g_value_dup_string (value);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static gboolean
real_parse_authentication_response (GDataService *self, const gchar *response_body, GError **error)
{
	gchar *auth_start, *auth_end;

	/* Parse the response */
	auth_start = strstr (response_body, "Auth=");
	if (auth_start == NULL)
		goto protocol_error;
	auth_start += strlen ("Auth=");

	auth_end = strstr (auth_start, "\n");
	if (auth_end == NULL)
		goto protocol_error;

	self->priv->auth_token = g_strndup (auth_start, auth_end - auth_start);
	if (self->priv->auth_token == NULL || strlen (self->priv->auth_token) == 0)
		goto protocol_error;

	return TRUE;

protocol_error:
	g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
		     _("The server returned a malformed response."));
	return FALSE;
}

gboolean
gdata_service_authenticate (GDataService *self, const gchar *username, const gchar *password, GError **error)
{
	GDataServiceClass *klass;
	SoupMessage *message;
	gchar *request_body;
	guint status;
	gboolean retval;

	g_return_val_if_fail (GDATA_IS_SERVICE (self), FALSE);
	g_return_val_if_fail (username != NULL, FALSE);
	g_return_val_if_fail (password != NULL, FALSE);

	/* Prepare the request */
	klass = GDATA_SERVICE_GET_CLASS (self);
	request_body = soup_form_encode ("accountType", "HOSTED_OR_GOOGLE",
					 "Email", username,
					 "Passwd", password,
					 "service", klass->service_name,
					 "source", self->priv->client_id,
					 NULL);

	/* Build the message */
	message = soup_message_new (SOUP_METHOD_POST, klass->authentication_uri);
	soup_message_set_request (message, "application/x-www-form-urlencoded", SOUP_MEMORY_TAKE, request_body, strlen (request_body));

	/* Send the message */
	status = soup_session_send_message (self->priv->session, message);

	if (status != 200) {
		/* Error */
		/* TODO: Handle CAPTCHA requests and other errors more specifically */
		g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_AUTHENTICATING,
			     _("TODO: error code %u when authenticating"), status);
		g_object_unref (message);
		self->priv->logged_in = FALSE;
		return FALSE;
	}

	g_assert (message->response_body->data != NULL);

	retval = klass->parse_authentication_response (self, message->response_body->data, error);
	g_object_unref (message);

	self->priv->logged_in = retval;

	return retval;
}

gboolean
gdata_service_is_logged_in (GDataService *self)
{
	g_assert (GDATA_IS_SERVICE (self));
	return self->priv->logged_in;
}

/*static void
authenticate_async_thread (GSimpleAsyncResult *result, GObject *object, GCancellable *cancellable)
{

}

static void
real_authenticate_async (GDataService *self, const gchar *username, const gchar *password,
			 GCancellable *cancellable, GAsyncReadyCallback callback, gpointer user_data)
{
	GSimpleAsyncResult *result;
  NextAsyncOp *op;

  op = g_new0 (NextAsyncOp, 1);

  op->num_files = num_files;
  op->files = NULL;

	result = g_simple_async_result_new (G_OBJECT (self), callback, user_data, real_authenticate_async);
	g_simple_async_result_set_op_res_gpointer (result, op, (GDestroyNotify) next_async_op_free);
  
	g_simple_async_result_run_in_thread (result, authenticate_async_thread, G_PRIORITY_DEFAULT, cancellable);
	g_object_unref (result);
}

void
gdata_service_authenticate_async (GDataService *self, const gchar *username, const gchar *password,
				  GCancellable *cancellable, GAsyncReadyCallback *callback, gpointer user_data)
{
	GDataServiceClass *klass;

	g_return_if_fail (GDATA_IS_SERVICE (self));
	g_return_if_fail (username != NULL);
	g_return_if_fail (password != NULL);

	klass = GDATA_SERVICE_GET_CLASS (self);
	klass->authenticate_async (self, username, password, cancellable, next_async_callback_wrapper, user_data);
}
gboolean
gdata_service_authenticate_finish (GDataService *self, GAsyncResult *result, GError **error)
{
	/* TODO 
}*/

GDataFeed *
gdata_service_query (GDataService *self, const gchar *query_uri, GError **error)
{
	return gdata_service_query_full (self, query_uri, _gdata_feed_new_from_xml, error);
}

GDataFeed *
gdata_service_query_full (GDataService *self, const gchar *query_uri, GDataFeedParserFunc parser_func, GError **error)
{
	GDataServiceClass *klass;
	GDataFeed *feed;
	SoupMessage *message;
	gchar *authorisation_header;
	guint status;

	g_return_val_if_fail (GDATA_IS_SERVICE (self), NULL);
	g_return_val_if_fail (query_uri != NULL, NULL);

	message = soup_message_new (SOUP_METHOD_GET, query_uri);

	/* Set the authorisation header */
	authorisation_header = g_strdup_printf ("GoogleLogin auth=%s", self->priv->auth_token);
	soup_message_headers_append (message->request_headers, "Authorization", authorisation_header);
	g_free (authorisation_header);

	/* Set the GData-Version header to tell it we want to use the v2 API */
	soup_message_headers_append (message->request_headers, "GData-Version", "2");

	/* Make sure subclasses set their headers */
	klass = GDATA_SERVICE_GET_CLASS (self);
	if (klass->append_query_headers != NULL)
		klass->append_query_headers (self, message);

	/* Send the message */
	status = soup_session_send_message (self->priv->session, message);
	g_debug ("Status %u from service query to \"%s\".", status, query_uri);

	if (status != 200) {
		/* Error */
		/* TODO: Handle errors more specifically */
		g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_WITH_QUERY,
			     _("TODO: error code %u when querying"), status);
		g_object_unref (message);
		return FALSE;
	}

	g_assert (message->response_body->data != NULL);

	feed = parser_func (message->response_body->data, message->response_body->length, error);
	g_object_unref (message);

	return feed;
}

const gchar *
gdata_service_get_client_id (GDataService *self)
{
	g_return_val_if_fail (GDATA_IS_SERVICE (self), NULL);
	return self->priv->client_id;
}
