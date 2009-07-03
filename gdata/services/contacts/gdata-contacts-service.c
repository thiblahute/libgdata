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
 * SECTION:gdata-contacts-service
 * @short_description: GData Contacts service object
 * @stability: Unstable
 * @include: gdata/services/contacts/gdata-contacts-service.h
 *
 * #GDataContactsService is a subclass of #GDataService for communicating with the GData API of Google Contacts. It supports querying
 * for, inserting, editing and deleting contacts from a Google address book.
 *
 * For more details of Google Contacts' GData API, see the <ulink type="http" url="http://code.google.com/apis/contacts/docs/2.0/reference.html">
 * online documentation</ulink>.
 **/

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <libsoup/soup.h>
#include <string.h>

#include "gdata-contacts-service.h"
#include "gdata-service.h"
#include "gdata-private.h"
#include "gdata-query.h"

G_DEFINE_TYPE (GDataContactsService, gdata_contacts_service, GDATA_TYPE_SERVICE)
#define GDATA_CONTACTS_SERVICE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_CONTACTS_SERVICE, GDataContactsServicePrivate))

static void
gdata_contacts_service_class_init (GDataContactsServiceClass *klass)
{
	GDataServiceClass *service_class = GDATA_SERVICE_CLASS (klass);
	service_class->service_name = "cp";
}

static void
gdata_contacts_service_init (GDataContactsService *self)
{
	/* Nothing to see here */
}

/**
 * gdata_contacts_service_new:
 * @client_id: your application's client ID
 *
 * Creates a new #GDataContactsService. The @client_id must be unique for your application, and as registered with Google.
 *
 * Return value: a new #GDataContactsService, or %NULL
 *
 * Since: 0.2.0
 **/
GDataContactsService *
gdata_contacts_service_new (const gchar *client_id)
{
	g_return_val_if_fail (client_id != NULL, NULL);

	return g_object_new (GDATA_TYPE_CONTACTS_SERVICE,
			     "client-id", client_id,
			     NULL);
}

/**
 * gdata_contacts_service_query_contacts:
 * @self: a #GDataContactsService
 * @query: a #GDataQuery with the query parameters, or %NULL
 * @cancellable: optional #GCancellable object, or %NULL
 * @progress_callback: a #GDataQueryProgressCallback to call when an entry is loaded, or %NULL
 * @progress_user_data: data to pass to the @progress_callback function
 * @error: a #GError, or %NULL
 *
 * Queries the service to return a list of contacts matching the given @query.
 *
 * For more details, see gdata_service_query().
 *
 * Return value: a #GDataFeed of query results; unref with g_object_unref()
 *
 * Since: 0.2.0
 **/
GDataFeed *
gdata_contacts_service_query_contacts (GDataContactsService *self, GDataQuery *query, GCancellable *cancellable,
				       GDataQueryProgressCallback progress_callback, gpointer progress_user_data, GError **error)
{
	/* Ensure we're authenticated first */
	if (gdata_service_is_authenticated (GDATA_SERVICE (self)) == FALSE) {
		g_set_error_literal (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_AUTHENTICATION_REQUIRED,
				     _("You must be authenticated to query contacts."));
		return NULL;
	}

	return gdata_service_query (GDATA_SERVICE (self), "http://www.google.com/m8/feeds/contacts/default/full", GDATA_QUERY (query),
				    GDATA_TYPE_CONTACTS_CONTACT, cancellable, progress_callback, progress_user_data, error);
}

/**
 * gdata_contacts_service_query_contacts_async:
 * @self: a #GDataContactsService
 * @query: a #GDataQuery with the query parameters, or %NULL
 * @cancellable: optional #GCancellable object, or %NULL
 * @progress_callback: a #GDataQueryProgressCallback to call when an entry is loaded, or %NULL
 * @progress_user_data: data to pass to the @progress_callback function
 * @callback: a #GAsyncReadyCallback to call when authentication is finished
 * @user_data: data to pass to the @callback function
 *
 * Queries the service to return a list of contacts matching the given @query. @self and
 * @query are all reffed when this function is called, so can safely be unreffed after this function returns.
 *
 * For more details, see gdata_contacts_service_query_contacts(), which is the synchronous version of this function,
 * and gdata_service_query_async(), which is the base asynchronous query function.
 *
 * Since: 0.2.0
 **/
void
gdata_contacts_service_query_contacts_async (GDataContactsService *self, GDataQuery *query, GCancellable *cancellable,
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

	gdata_service_query_async (GDATA_SERVICE (self), "http://www.google.com/m8/feeds/contacts/default/full", GDATA_QUERY (query),
				   GDATA_TYPE_CONTACTS_CONTACT, cancellable, progress_callback, progress_user_data, callback, user_data);
}

/**
 * gdata_contacts_service_insert_contact:
 * @self: a #GDataContactsService
 * @contact: the #GDataContactsContact to insert
 * @cancellable: optional #GCancellable object, or %NULL
 * @error: a #GError, or %NULL
 *
 * Inserts @contact by uploading it to the online contacts service.
 *
 * For more details, see gdata_service_insert_entry().
 *
 * Return value: an updated #GDataContactsContact, or %NULL
 *
 * Since: 0.2.0
 **/
GDataContactsContact *
gdata_contacts_service_insert_contact (GDataContactsService *self, GDataContactsContact *contact, GCancellable *cancellable, GError **error)
{
	/* TODO: Async variant */
	gchar *uri;
	GDataEntry *entry;

	g_return_val_if_fail (GDATA_IS_CONTACTS_SERVICE (self), NULL);
	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (contact), NULL);

	uri = g_strdup_printf ("http://www.google.com/m8/feeds/contacts/%s/full", gdata_service_get_username (GDATA_SERVICE (self)));
	entry = gdata_service_insert_entry (GDATA_SERVICE (self), uri, GDATA_ENTRY (contact), cancellable, error);
	g_free (uri);

	return GDATA_CONTACTS_CONTACT (entry);
}

/**
 * gdata_contacts_service_update_contact:
 * @self: a #GDataContactsService
 * @contact: the #GDataContactsContact to update
 * @cancellable: optional #GCancellable object, or %NULL
 * @error: a #GError, or %NULL
 *
 * Updates @contact on the online contacts service.
 *
 * For more details, see gdata_service_update_entry().
 *
 * Return value: an updated #GDataContactsContact, or %NULL
 *
 * Since: 0.4.0
 **/
GDataContactsContact *
gdata_contacts_service_update_contact (GDataContactsService *self, GDataContactsContact *contact, GCancellable *cancellable, GError **error)
{
	/* TODO: Async variant */
	const gchar *uri;
	GDataLink *link;

	g_return_val_if_fail (GDATA_IS_CONTACTS_SERVICE (self), NULL);
	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (contact), NULL);

	/* Can't trust the edit URI the contact gives us, as it has the wrong projection; it uses the base projection, which
	 * doesn't allow for extended attributes to be set (for some weird reason). */
	link = gdata_entry_look_up_link (GDATA_ENTRY (contact), GDATA_LINK_EDIT);
	g_assert (link != NULL);
	uri = gdata_link_get_uri (link);
	g_assert (uri != NULL);
	uri = strstr (uri, "/base/");
	if (uri != NULL)
		memcpy ((char*) uri, "/full/", 6);

	return GDATA_CONTACTS_CONTACT (gdata_service_update_entry (GDATA_SERVICE (self), GDATA_ENTRY (contact), cancellable, error));
}
