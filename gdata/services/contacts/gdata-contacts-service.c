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

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <libsoup/soup.h>
#include <string.h>

#include "gdata-contacts-service.h"
#include "gdata-service.h"
#include "gdata-private.h"
#include "gdata-contacts-query.h"

/* Standards reference here: http://code.google.com/apis/contacts/docs/2.0/reference.html */

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

GDataContactsService *
gdata_contacts_service_new (const gchar *client_id)
{
	g_return_val_if_fail (client_id != NULL, NULL);

	return g_object_new (GDATA_TYPE_CONTACTS_SERVICE,
			     "client-id", client_id,
			     NULL);
}

GDataFeed *
gdata_contacts_service_query_contacts (GDataContactsService *self, GDataContactsQuery *query, GCancellable *cancellable,
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

void
gdata_contacts_service_query_contacts_async (GDataContactsService *self, GDataContactsQuery *query, GCancellable *cancellable,
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

/* TODO: Async variant */
GDataContactsContact *
gdata_contacts_service_insert_contact (GDataContactsService *self, GDataContactsContact *contact, GCancellable *cancellable, GError **error)
{
	gchar *uri;
	GDataEntry *entry;

	g_return_val_if_fail (GDATA_IS_CONTACTS_SERVICE (self), NULL);
	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (contact), NULL);

	uri = g_strdup_printf ("http://www.google.com/m8/feeds/contacts/%s/full", gdata_service_get_username (GDATA_SERVICE (self)));

	entry = gdata_service_insert_entry (GDATA_SERVICE (self), uri, GDATA_ENTRY (contact), cancellable, error);
	g_free (uri);

	return GDATA_CONTACTS_CONTACT (entry);
}
