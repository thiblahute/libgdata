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
 * SECTION:gdata-access-handler
 * @short_description: GData access handler interface
 * @stability: Unstable
 * @include: gdata/gdata-access-handler.h
 *
 * #GDataAccessHandler is an interface which can be implemented by #GDataEntry<!-- -->s which can have their permissions controlled by an
 * access control list (ACL). It has a set of methods which allow the #GDataAccessRule<!-- -->s for the access handler/entry to be retrieved,
 * added, modified and deleted, with immediate effect.
 *
 * When implementing the interface, classes must implement an <function>is_owner_rule</function> function.
 **/

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <string.h>

#include "gdata-access-handler.h"
#include "gdata-private.h"
#include "gdata-access-rule.h"

GType
gdata_access_handler_get_type (void)
{
	static GType access_handler_type = 0;

	if (!access_handler_type) {
		access_handler_type = g_type_register_static_simple (G_TYPE_INTERFACE, "GDataAccessHandler",
								     sizeof (GDataAccessHandlerIface),
								     NULL, 0, NULL, 0);
		g_type_interface_add_prerequisite (access_handler_type, GDATA_TYPE_ENTRY);
	}

	return access_handler_type;
}

/**
 * gdata_access_handler_get_rules:
 * @self: a #GDataAccessHandler
 * @service: a #GDataService
 * @cancellable: optional #GCancellable object, or %NULL
 * @progress_callback: a #GDataQueryProgressCallback to call when a rule is loaded, or %NULL
 * @progress_user_data: data to pass to the @progress_callback function
 * @error: a #GError, or %NULL
 *
 * Retrieves a #GDataFeed containing all the access rules which apply to the given #GDataAccessHandler. Only the owner of a #GDataAccessHandler may
 * view its rule feed.
 *
 * If @cancellable is not %NULL, then the operation can be cancelled by triggering the @cancellable object from another thread.
 * If the operation was cancelled, the error %G_IO_ERROR_CANCELLED will be returned.
 *
 * A %GDATA_SERVICE_ERROR_WITH_QUERY will be returned if the server indicates there is a problem with the query.
 *
 * For each rule in the response feed, @progress_callback will be called in the main thread. If there was an error parsing the XML response,
 * a #GDataParserError will be returned.
 *
 * Return value: a #GDataFeed of access control rules, or %NULL; unref with g_object_unref()
 *
 * Since: 0.3.0
 **/
GDataFeed *
gdata_access_handler_get_rules (GDataAccessHandler *self, GDataService *service, GCancellable *cancellable, GDataQueryProgressCallback progress_callback,
				gpointer progress_user_data, GError **error)
{
	GDataServiceClass *klass;
	GDataFeed *feed;
	GDataLink *link;
	SoupMessage *message;
	guint status;

	/* TODO: async version */
	g_return_val_if_fail (GDATA_IS_ENTRY (self), NULL);
	g_return_val_if_fail (GDATA_IS_SERVICE (service), NULL);

	/* Get the ACL URI */
	link = gdata_entry_look_up_link (GDATA_ENTRY (self), "http://schemas.google.com/acl/2007#accessControlList");
	g_assert (link != NULL);
	message = soup_message_new (SOUP_METHOD_GET, gdata_link_get_uri (link));

	/* Make sure subclasses set their headers */
	klass = GDATA_SERVICE_GET_CLASS (service);
	if (klass->append_query_headers != NULL)
		klass->append_query_headers (service, message);

	/* Send the message */
	status = _gdata_service_send_message (service, message, error);
	if (status == SOUP_STATUS_NONE) {
		g_object_unref (message);
		return NULL;
	}

	/* Check for cancellation */
	if (g_cancellable_set_error_if_cancelled (cancellable, error) == TRUE) {
		g_object_unref (message);
		return NULL;
	}

	if (status != 200) {
		/* Error */
		g_assert (klass->parse_error_response != NULL);
		klass->parse_error_response (service, GDATA_SERVICE_ERROR_WITH_QUERY, status, message->reason_phrase, message->response_body->data,
					     message->response_body->length, error);
		g_object_unref (message);
		return NULL;
	}

	g_assert (message->response_body->data != NULL);

	feed = _gdata_feed_new_from_xml (GDATA_TYPE_FEED, message->response_body->data, message->response_body->length, GDATA_TYPE_ACCESS_RULE,
					 progress_callback, progress_user_data, error);
	g_object_unref (message);

	return feed;
}

/**
 * gdata_access_handler_insert_rule:
 * @self: a #GDataAccessHandler
 * @service: a #GDataService
 * @rule: the #GDataAccessRule to insert
 * @cancellable: optional #GCancellable object, or %NULL
 * @error: a #GError, or %NULL
 *
 * Inserts @rule in the access control list of the #GDataAccessHandler.
 *
 * The service will return an updated version of the rule, which is the return value of this function on success.
 *
 * If @cancellable is not %NULL, then the operation can be cancelled by triggering the @cancellable object from another thread.
 * If the operation was cancelled, the error %G_IO_ERROR_CANCELLED will be returned.
 *
 * If the rule is marked as already having been inserted a %GDATA_SERVICE_ERROR_ENTRY_ALREADY_INSERTED error will be returned immediately
 * (there will be no network requests).
 *
 * If there is an error inserting the rule, a %GDATA_SERVICE_ERROR_WITH_INSERTION error will be returned.
 *
 * Return value: an updated #GDataAccessRule, or %NULL
 *
 * Since: 0.3.0
 **/
GDataAccessRule *
gdata_access_handler_insert_rule (GDataAccessHandler *self, GDataService *service, GDataAccessRule *rule, GCancellable *cancellable, GError **error)
{
	GDataServiceClass *klass;
	GDataAccessRule *updated_rule;
	GDataLink *link;
	SoupMessage *message;
	gchar *upload_data;
	guint status;

	g_return_val_if_fail (GDATA_IS_ENTRY (self), NULL);
	g_return_val_if_fail (GDATA_IS_SERVICE (service), NULL);
	g_return_val_if_fail (GDATA_IS_ACCESS_RULE (rule), NULL);

	if (gdata_entry_is_inserted (GDATA_ENTRY (rule)) == TRUE) {
		g_set_error_literal (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_ENTRY_ALREADY_INSERTED,
				     _("The rule has already been inserted."));
		return NULL;
	}

	/* Get the ACL URI */
	link = gdata_entry_look_up_link (GDATA_ENTRY (self), "http://schemas.google.com/acl/2007#accessControlList");
	g_assert (link != NULL);
	message = soup_message_new (SOUP_METHOD_POST, gdata_link_get_uri (link));

	/* Make sure subclasses set their headers */
	klass = GDATA_SERVICE_GET_CLASS (service);
	if (klass->append_query_headers != NULL)
		klass->append_query_headers (service, message);

	/* Append the data */
	upload_data = gdata_parsable_get_xml (GDATA_PARSABLE (rule));
	soup_message_set_request (message, "application/atom+xml", SOUP_MEMORY_TAKE, upload_data, strlen (upload_data));

	/* Send the message */
	status = _gdata_service_send_message (service, message, error);
	if (status == SOUP_STATUS_NONE) {
		g_object_unref (message);
		return NULL;
	}

	/* Check for cancellation */
	if (g_cancellable_set_error_if_cancelled (cancellable, error) == TRUE) {
		g_object_unref (message);
		return NULL;
	}

	if (status != 201) {
		/* Error */
		g_assert (klass->parse_error_response != NULL);
		klass->parse_error_response (service, GDATA_SERVICE_ERROR_WITH_INSERTION, status, message->reason_phrase, message->response_body->data,
					     message->response_body->length, error);
		g_object_unref (message);
		return NULL;
	}

	/* Build the updated entry */
	g_assert (message->response_body->data != NULL);

	/* Parse the XML; create and return a new GDataEntry of the same type as @entry */
	updated_rule = GDATA_ACCESS_RULE (gdata_parsable_new_from_xml (G_OBJECT_TYPE (rule), message->response_body->data,
								       message->response_body->length, error));
	g_object_unref (message);

	return updated_rule;
}

static SoupMessage *
get_soup_message (GDataAccessHandler *access_handler, GDataAccessRule *rule, const gchar *method)
{
	GDataLink *link;
	SoupMessage *message;
	GString *uri_string;
	gchar *uri;
	const gchar *scope_type, *scope_value;

	/* Get the edit URI */
	link = gdata_entry_look_up_link (GDATA_ENTRY (rule), GDATA_LINK_EDIT);
	if (link != NULL)
		return soup_message_new (method, gdata_link_get_uri (link));

	/* Try building the URI instead */
	link = gdata_entry_look_up_link (GDATA_ENTRY (access_handler), "http://schemas.google.com/acl/2007#accessControlList");
	g_assert (link != NULL);
	gdata_access_rule_get_scope (rule, &scope_type, &scope_value);

	uri_string = g_string_sized_new (strlen (gdata_link_get_uri (link)) + 30);
	g_string_append_printf (uri_string, "%s/", gdata_link_get_uri (link));
	g_string_append_uri_escaped (uri_string, scope_type, NULL, TRUE);
	if (scope_value != NULL) {
		g_string_append (uri_string, "%3A");
		g_string_append_uri_escaped (uri_string, scope_value, NULL, TRUE);
	}

	uri = g_string_free (uri_string, FALSE);
	message = soup_message_new (method, uri);
	g_free (uri);

	return message;
}

/**
 * gdata_access_handler_update_rule:
 * @self: a #GDataAccessHandler
 * @service: a #GDataService
 * @rule: the #GDataAccessRule to update
 * @cancellable: optional #GCancellable object, or %NULL
 * @error: a #GError, or %NULL
 *
 * Updates @rule in the access control list of the #GDataAccessHandler.
 *
 * The service will return an updated version of the rule, which is the return value of this function on success.
 *
 * If @cancellable is not %NULL, then the operation can be cancelled by triggering the @cancellable object from another thread.
 * If the operation was cancelled, the error %G_IO_ERROR_CANCELLED will be returned.
 *
 * If there is an error updating the rule, a %GDATA_SERVICE_ERROR_WITH_UPDATE error will be returned.
 *
 * Return value: an updated #GDataAccessRule, or %NULL
 *
 * Since: 0.3.0
 **/
GDataAccessRule *
gdata_access_handler_update_rule (GDataAccessHandler *self, GDataService *service, GDataAccessRule *rule, GCancellable *cancellable, GError **error)
{
	GDataServiceClass *klass;
	GDataAccessRule *updated_rule;
	SoupMessage *message;
	gchar *upload_data;
	guint status;

	g_return_val_if_fail (GDATA_IS_ENTRY (self), NULL);
	g_return_val_if_fail (GDATA_IS_SERVICE (service), NULL);
	g_return_val_if_fail (GDATA_IS_ACCESS_RULE (rule), NULL);

	message = get_soup_message (self, rule, SOUP_METHOD_PUT);

	/* Make sure subclasses set their headers */
	klass = GDATA_SERVICE_GET_CLASS (service);
	if (klass->append_query_headers != NULL)
		klass->append_query_headers (service, message);

	/* Looks like ACLs don't support ETags */

	/* Append the data */
	upload_data = gdata_parsable_get_xml (GDATA_PARSABLE (rule));
	soup_message_set_request (message, "application/atom+xml", SOUP_MEMORY_TAKE, upload_data, strlen (upload_data));

	/* Send the message */
	status = _gdata_service_send_message (service, message, error);
	if (status == SOUP_STATUS_NONE) {
		g_object_unref (message);
		return NULL;
	}

	/* Check for cancellation */
	if (g_cancellable_set_error_if_cancelled (cancellable, error) == TRUE) {
		g_object_unref (message);
		return NULL;
	}

	if (status != 200) {
		/* Error */
		g_assert (klass->parse_error_response != NULL);
		klass->parse_error_response (service, GDATA_SERVICE_ERROR_WITH_UPDATE, status, message->reason_phrase, message->response_body->data,
					     message->response_body->length, error);
		g_object_unref (message);
		return NULL;
	}

	/* Build the updated entry */
	g_assert (message->response_body->data != NULL);

	/* Parse the XML; create and return a new GDataEntry of the same type as @entry */
	updated_rule = GDATA_ACCESS_RULE (gdata_parsable_new_from_xml (G_OBJECT_TYPE (rule), message->response_body->data,
								       message->response_body->length, error));
	g_object_unref (message);

	return updated_rule;
}

/**
 * gdata_access_handler_delete_rule:
 * @self: a #GDataAccessHandler
 * @service: a #GDataService
 * @rule: the #GDataAccessRule to delete
 * @cancellable: optional #GCancellable object, or %NULL
 * @error: a #GError, or %NULL
 *
 * Deletes @rule from the access control list of the #GDataAccessHandler.
 *
 * If @cancellable is not %NULL, then the operation can be cancelled by triggering the @cancellable object from another thread.
 * If the operation was cancelled, the error %G_IO_ERROR_CANCELLED will be returned.
 *
 * If there is an error deleting the rule, a %GDATA_SERVICE_ERROR_WITH_DELETION error will be returned, unless the @rule was the owner's
 * rule; in which case, %GDATA_SERVICE_ERROR_FORBIDDEN will be returned without any network activity.
 *
 * Return value: %TRUE on success, %FALSE otherwise
 *
 * Since: 0.3.0
 **/
gboolean
gdata_access_handler_delete_rule (GDataAccessHandler *self, GDataService *service, GDataAccessRule *rule, GCancellable *cancellable, GError **error)
{
	GDataServiceClass *klass;
	GDataAccessHandlerIface *iface;
	SoupMessage *message;
	guint status;

	g_return_val_if_fail (GDATA_IS_ENTRY (self), FALSE);
	g_return_val_if_fail (GDATA_IS_SERVICE (service), FALSE);
	g_return_val_if_fail (GDATA_IS_ACCESS_RULE (rule), FALSE);

	/* The owner of the access handler can't be deleted */
	iface = GDATA_ACCESS_HANDLER_GET_IFACE (self);
	g_assert (iface->is_owner_rule != NULL);
	if (iface->is_owner_rule (rule) == TRUE) {
		g_set_error_literal (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_FORBIDDEN,
				     _("The owner's rule may not be deleted."));
		return FALSE;
	}

	message = get_soup_message (self, rule, SOUP_METHOD_DELETE);

	/* Make sure subclasses set their headers */
	klass = GDATA_SERVICE_GET_CLASS (service);
	if (klass->append_query_headers != NULL)
		klass->append_query_headers (service, message);

	/* Looks like ACLs don't support ETags */

	/* Send the message */
	status = _gdata_service_send_message (service, message, error);
	if (status == SOUP_STATUS_NONE) {
		g_object_unref (message);
		return FALSE;
	}

	/* Check for cancellation */
	if (g_cancellable_set_error_if_cancelled (cancellable, error) == TRUE) {
		g_object_unref (message);
		return FALSE;
	}

	if (status != 200) {
		/* Error */
		g_assert (klass->parse_error_response != NULL);
		klass->parse_error_response (service, GDATA_SERVICE_ERROR_WITH_DELETION, status, message->reason_phrase, message->response_body->data,
					     message->response_body->length, error);
		g_object_unref (message);
		return FALSE;
	}

	g_object_unref (message);

	return TRUE;
}
