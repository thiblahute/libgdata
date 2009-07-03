/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Richard Schwarting 2009 <aquarichy@gmail.com>
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
 * SECTION:gdata-picasaweb-service
 * @short_description: GData PicasaWeb service object
 * @stability: Unstable
 * @include: gdata/services/picasaweb/gdata-picasaweb-service.h
 *
 * #GDataPicasaWebService is a subclass of #GDataService for communicating with the GData API of Google PicasaWeb. It supports querying for files
 * and albums, and uploading files.
 *
 * For more details of PicasaWeb's GData API, see the <ulink type="http" url="http://code.google.com/apis/picasaweb/developers_guide_protocol.html">
 * online documentation</ulink>.
 **/

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>

#include "gdata-service.h"
#include "gdata-picasaweb-service.h"
#include "gdata-private.h"
#include "gdata-parser.h"
#include "atom/gdata-link.h"

G_DEFINE_TYPE (GDataPicasaWebService, gdata_picasaweb_service, GDATA_TYPE_SERVICE)

static void
gdata_picasaweb_service_class_init (GDataPicasaWebServiceClass *klass)
{
	GDataServiceClass *service_class = GDATA_SERVICE_CLASS (klass);
	service_class->service_name = "lh2";
}

static void
gdata_picasaweb_service_init (GDataPicasaWebService *self)
{
	/* Nothing to see here */
}

/*
 * This constructs the URI we want to access for querying albums.
 *
 * Remember to free the URI in the caller.
*/
static gchar *
create_uri (GDataPicasaWebService *self, const gchar *username)
{
	if (username == NULL) {
		/* Ensure we're authenticated first */
		if (gdata_service_is_authenticated (GDATA_SERVICE (self)) == FALSE)
			return NULL;

		/* Querying Picasa albums for the "default" user when logged in returns the albums for the authenticated user */
		username = "default";
	}

	return g_strdup_printf ("http://picasaweb.google.com/data/feed/api/user/%s", username);
}

/**
 * gdata_picasaweb_service_new:
 * @client_id: your application's client ID
 *
 * Creates a new #GDataPicasaWebService. The @client_id must be unique for your application, and as registered with Google.
 *
 * Return value: a new #GDataPicasaWebService, or %NULL
 *
 * Since: 0.4.0
 **/
GDataPicasaWebService *
gdata_picasaweb_service_new (const gchar *client_id)
{
	g_return_val_if_fail (client_id != NULL, NULL);

	return g_object_new (GDATA_TYPE_PICASAWEB_SERVICE,
			     "client-id", client_id,
			     NULL);
}

/**
 * gdata_picasaweb_service_query_all_albums:
 * @self: a #GDataPicasaWebService
 * @query: a #GDataQuery with the query parameters, or %NULL
 * @username: the username of the user whose albums you wish to retrieve, or %NULL
 * @cancellable: optional #GCancellable object, or %NULL
 * @progress_callback: a #GDataQueryProgressCallback to call when an entry is loaded, or %NULL
 * @progress_user_data: data to pass to the @progress_callback function
 * @error: a #GError, or %NULL
 *
 * Queries the service to return a list of all albums belonging to the specified @username which match the given
 * @query. If a user is authenticated with the service, @username can be set as %NULL to return a list of albums belonging
 * to the currently-authenticated user.
 *
 * For more details, see gdata_service_query().
 *
 * Return value: a #GDataFeed of query results; unref with g_object_unref()
 *
 * Since: 0.4.0
 **/
GDataFeed *
gdata_picasaweb_service_query_all_albums (GDataPicasaWebService *self, GDataQuery *query, const gchar *username, GCancellable *cancellable,
					  GDataQueryProgressCallback progress_callback, gpointer progress_user_data, GError **error)
{
	gchar *uri;
	GDataFeed *album_feed;

	g_return_val_if_fail (GDATA_IS_PICASAWEB_SERVICE (self), NULL);
	g_return_val_if_fail (query == NULL || GDATA_IS_QUERY (query), NULL);

	uri = create_uri (self, username);
	if (uri == NULL) {
		g_set_error_literal (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_AUTHENTICATION_REQUIRED,
				     _("You must specify a username or be authenticated to query all albums."));
		return NULL;
	}

	/* Execute the query */
	album_feed = gdata_service_query (GDATA_SERVICE (self), uri, query, GDATA_TYPE_PICASAWEB_ALBUM,
					  cancellable, progress_callback, progress_user_data, error);
	g_free (uri);

	return album_feed;
}

/**
 * gdata_picasaweb_service_query_all_albums_async:
 * @self: a #GDataPicasaWebService
 * @query: a #GDataQuery with the query parameters, or %NULL
 * @username: the username of the user whose albums you wish to retrieve, or %NULL
 * @cancellable: optional #GCancellable object, or %NULL
 * @progress_callback: a #GDataQueryProgressCallback to call when an entry is loaded, or %NULL
 * @progress_user_data: data to pass to the @progress_callback function
 * @callback: a #GAsyncReadyCallback to call when authentication is finished
 * @user_data: data to pass to the @callback function
 *
 * Queries the service to return a list of all albums belonging to the specified @username which match the given
 * @query. @self, @query and @username are all reffed/copied when this function is called, so can safely be unreffed/freed after
 * this function returns.
 *
 * For more details, see gdata_picasaweb_service_query_all_albums(), which is the synchronous version of
 * this function, and gdata_service_query_async(), which is the base asynchronous query function.
 *
 * Since: 0.4.0
 **/
void
gdata_picasaweb_service_query_all_albums_async (GDataPicasaWebService *self, GDataQuery *query, const gchar *username,
						GCancellable *cancellable, GDataQueryProgressCallback progress_callback, gpointer progress_user_data,
						GAsyncReadyCallback callback, gpointer user_data)
{
	gchar *uri;

	g_return_if_fail (GDATA_IS_PICASAWEB_SERVICE (self));
	g_return_if_fail (query == NULL || GDATA_IS_QUERY (query));
	g_return_if_fail (callback != NULL);

	uri = create_uri (self, username);
	if (uri == NULL) {
		g_simple_async_report_error_in_idle (G_OBJECT (self), callback, user_data,
						     GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_AUTHENTICATION_REQUIRED,
						     _("You must specify a username or be authenticated to query all albums."));
		return;
	}

	/* Schedule the async query */
	gdata_service_query_async (GDATA_SERVICE (self), uri, query, GDATA_TYPE_PICASAWEB_ALBUM, cancellable, progress_callback, progress_user_data,
				   callback, user_data);
	g_free (uri);
}

/**
 * gdata_picasaweb_service_query_files:
 * @self: a #GDataPicasaWebService
 * @album: a #GDataPicasaWebAlbum from which to retrieve the files, or %NULL
 * @query: a #GDataQuery with the query parameters, or %NULL
 * @cancellable: optional #GCancellable object, or %NULL
 * @progress_callback: a #GDataQueryProgressCallback to call when an entry is loaded, or %NULL
 * @progress_user_data: data to pass to the @progress_callback function
 * @error: a #GError, or %NULL
 *
 * Queries the specified @album for a list of the files which match the given @query. If @album is %NULL and a user is
 * authenticated with the service, the user's default album will be queried.
 *
 * For more details, see gdata_service_query().
 *
 * Return value: a #GDataFeed of query results; unref with g_object_unref()
 *
 * Since: 0.4.0
 **/
GDataFeed *
gdata_picasaweb_service_query_files (GDataPicasaWebService *self, GDataPicasaWebAlbum *album, GDataQuery *query, GCancellable *cancellable,
				     GDataQueryProgressCallback progress_callback, gpointer progress_user_data, GError **error)
{
	/* TODO: Async variant */
	const gchar *uri;

	if (album != NULL) {
		GDataLink *link = gdata_entry_look_up_link (GDATA_ENTRY (album), "http://schemas.google.com/g/2005#feed");
		if (link == NULL) {
			/* Error */
			g_set_error_literal (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
					     _("The album did not have a feed link."));
			return NULL;
		}
		uri = gdata_link_get_uri (link);
	} else {
		/* Default URI */
		uri = "http://picasaweb.google.com/data/feed/api/user/default/albumid/default";
	}

	/* Execute the query */
	return gdata_service_query (GDATA_SERVICE (self), uri, GDATA_QUERY (query), GDATA_TYPE_PICASAWEB_FILE, cancellable,
				    progress_callback, progress_user_data, error);
}

/**
 * gdata_picasaweb_service_upload_file:
 * @self: a #GDataPicasaWebService
 * @album: a #GDataPicasaWebAlbum into which to insert the file, or %NULL
 * @file: a #GDataPicasaWebFile to insert
 * @actual_file: the actual file to upload
 * @cancellable: optional #GCancellable object, or %NULL
 * @error: a #GError, or %NULL
 *
 * Uploads a file (photo or video) to the given PicasaWeb @album, using the @actual_file from disk and the metadata from @file. If @album is
 * %NULL, the file will be uploaded to the currently-authenticated user's "Drop Box" album. A user must be authenticated to use this function.
 *
 * If @file has already been inserted, a %GDATA_SERVICE_ERROR_ENTRY_ALREADY_INSERTED error will be returned. If no user is authenticated
 * with the service, %GDATA_SERVICE_ERROR_AUTHENTICATION_REQUIRED will be returned.
 *
 * If there is a problem reading @actual_file, an error from g_file_load_contents() or g_file_query_info() will be returned. Other errors from
 * #GDataServiceError can be returned for other exceptional conditions, as determined by the server.
 *
 * Return value: the inserted #GDataPicasaWebFile with updated properties from @file; unref with g_object_unref()
 *
 * Since: 0.4.0
 **/
GDataPicasaWebFile *
gdata_picasaweb_service_upload_file (GDataPicasaWebService *self, GDataPicasaWebAlbum *album, GDataPicasaWebFile *file, GFile *actual_file,
				     GCancellable *cancellable, GError **error)
{
	/* TODO: Async variant */
	#define BOUNDARY_STRING "0xdeadbeef6e0808d5e6ed8bc168390bcc"

	GDataServiceClass *klass;
	SoupMessage *message;
	gchar *entry_xml, *upload_uri, *second_chunk_header, *upload_data, *file_contents, *i;
	const gchar *first_chunk_header, *footer, *album_id, *user_id;
	guint status;
	GFileInfo *actual_file_info;
	gsize content_length, first_chunk_header_length, second_chunk_header_length, entry_xml_length, file_length, footer_length;

	g_return_val_if_fail (GDATA_IS_PICASAWEB_SERVICE (self), NULL);
	g_return_val_if_fail (GDATA_IS_PICASAWEB_FILE (file), NULL);
	g_return_val_if_fail (G_IS_FILE (actual_file), NULL);

	if (gdata_entry_is_inserted (GDATA_ENTRY (file)) == TRUE) {
		g_set_error_literal (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_ENTRY_ALREADY_INSERTED,
				     _("The entry has already been inserted."));
		return NULL;
	}

	if (gdata_service_is_authenticated (GDATA_SERVICE (self)) == FALSE) {
		g_set_error_literal (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_AUTHENTICATION_REQUIRED,
				     _("You must be authenticated to upload a file."));
		return NULL;
	}

	/* PicasaWeb allows you to post to a default Dropbox */
	if (album == NULL)
		album_id = "default";
	else
		album_id = gdata_entry_get_id (GDATA_ENTRY (album));
	user_id = gdata_service_get_username (GDATA_SERVICE (self));

	upload_uri = g_strdup_printf ("http://picasaweb.google.com/data/feed/api/user/%s/albumid/%s", user_id, album_id);
	message = soup_message_new (SOUP_METHOD_POST, upload_uri);
	g_free (upload_uri);

	/* Make sure subclasses set their headers */
	klass = GDATA_SERVICE_GET_CLASS (self);
	if (klass->append_query_headers != NULL)
		klass->append_query_headers (GDATA_SERVICE (self), message);

	/* Get the data early so we can calculate the content length */
	if (g_file_load_contents (actual_file, NULL, &file_contents, &file_length, NULL, error) == FALSE) {
		g_object_unref (message);
		return NULL;
	}

	entry_xml = gdata_parsable_get_xml (GDATA_PARSABLE (file));

	/* Check for cancellation */
	if (g_cancellable_set_error_if_cancelled (cancellable, error) == TRUE) {
		g_object_unref (message);
		g_free (entry_xml);
		return NULL;
	}

	actual_file_info = g_file_query_info (actual_file, "standard::display-name,standard::content-type", G_FILE_QUERY_INFO_NONE, NULL, error);
	if (actual_file_info == NULL) {
		g_object_unref (message);
		g_free (entry_xml);
		return NULL;
	}

	/* Check for cancellation */
	if (g_cancellable_set_error_if_cancelled (cancellable, error) == TRUE) {
		g_object_unref (message);
		g_free (entry_xml);
		g_object_unref (actual_file_info);
		return NULL;
	}

	/* Add file-upload--specific headers */
	soup_message_headers_append (message->request_headers, "Slug", g_file_info_get_display_name (actual_file_info));

	first_chunk_header = "--" BOUNDARY_STRING "\nContent-Type: application/atom+xml; charset=UTF-8\n\n<?xml version='1.0'?>";
	second_chunk_header = g_strdup_printf ("\n--" BOUNDARY_STRING "\nContent-Type: %s\nContent-Transfer-Encoding: binary\n\n",
					       g_file_info_get_content_type (actual_file_info));
	footer = "\n--" BOUNDARY_STRING "--";

	g_object_unref (actual_file_info);

	first_chunk_header_length = strlen (first_chunk_header);
	second_chunk_header_length = strlen (second_chunk_header);
	footer_length = strlen (footer);
	entry_xml_length = strlen (entry_xml);

	content_length = first_chunk_header_length + entry_xml_length + second_chunk_header_length + file_length + footer_length;

	/* Build the upload data */
	upload_data = i = g_malloc (content_length);

	memcpy (upload_data, first_chunk_header, first_chunk_header_length);
	i += first_chunk_header_length;

	memcpy (i, entry_xml, entry_xml_length);
	i += entry_xml_length;
	g_free (entry_xml);

	memcpy (i, second_chunk_header, second_chunk_header_length);
	g_free (second_chunk_header);
	i += second_chunk_header_length;

	memcpy (i, file_contents, file_length);
	g_free (file_contents);
	i += file_length;

	memcpy (i, footer, footer_length);

	/* Append the data */
	soup_message_set_request (message, "multipart/related; boundary=" BOUNDARY_STRING, SOUP_MEMORY_TAKE, upload_data, content_length);

	/* Send the message */
	status = _gdata_service_send_message (GDATA_SERVICE (self), message, error);
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
		klass->parse_error_response (GDATA_SERVICE (self), GDATA_SERVICE_ERROR_WITH_INSERTION, status, message->reason_phrase,
					     message->response_body->data, message->response_body->length, error);
		g_object_unref (message);
		return NULL;
	}

	g_assert (message->response_body->data != NULL);

	return GDATA_PICASAWEB_FILE (gdata_parsable_new_from_xml (GDATA_TYPE_PICASAWEB_FILE, message->response_body->data,
								  (gint) message->response_body->length, error));
}
