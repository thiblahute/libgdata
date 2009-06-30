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

#include <glib.h>
#include <libxml/parser.h>
#include <libsoup/soup.h>

#include <gdata/gdata-service.h>

#ifndef GDATA_PRIVATE_H
#define GDATA_PRIVATE_H

G_BEGIN_DECLS

#include "gdata-service.h"
void _gdata_service_set_authenticated (GDataService *self, gboolean authenticated);
guint _gdata_service_send_message (GDataService *self, SoupMessage *message, GError **error);

#include "gdata-query.h"
void _gdata_query_set_next_uri (GDataQuery *self, const gchar *next_uri);
void _gdata_query_set_previous_uri (GDataQuery *self, const gchar *previous_uri);

#include "gdata-parsable.h"
GDataParsable *_gdata_parsable_new_from_xml (GType parsable_type, const gchar *first_element, const gchar *xml, gint length, gpointer user_data,
					     GError **error) G_GNUC_WARN_UNUSED_RESULT;
GDataParsable *_gdata_parsable_new_from_xml_node (GType parsable_type, const gchar *first_element, xmlDoc *doc, xmlNode *node, gpointer user_data,
						  GError **error) G_GNUC_WARN_UNUSED_RESULT;
gchar *_gdata_parsable_get_xml (GDataParsable *self, const gchar *first_element, gboolean at_top_level) G_GNUC_WARN_UNUSED_RESULT;

#include "gdata-feed.h"
GDataFeed *_gdata_feed_new_from_xml (GType feed_type, const gchar *xml, gint length, GType entry_type,
				     GDataQueryProgressCallback progress_callback, gpointer progress_user_data, GError **error) G_GNUC_WARN_UNUSED_RESULT;
void _gdata_feed_add_entry (GDataFeed *self, GDataEntry *entry);
gpointer _gdata_feed_parse_data_new(GType entry_type, GDataQueryProgressCallback progress_callback, gpointer progress_user_data);
void _gdata_feed_parse_data_free (gpointer data);
void _gdata_feed_call_progress_callback (GDataFeed *self, gpointer user_data, GDataEntry *entry);

#include "gdata-entry.h"
GDataEntry *_gdata_entry_new_from_xml (GType entry_type, const gchar *xml, gint length, GError **error) G_GNUC_WARN_UNUSED_RESULT;

#include "gdata/services/documents/gdata-documents-entry.h"
GFile *_gdata_documents_entry_download_document (GDataDocumentsEntry *self, GDataService *service, gchar **content_type, gchar *link, gchar *destination_folder,\
					gchar *file_extension, gboolean replace_file_if_exist, GCancellable *cancellable, GError **error) G_GNUC_WARN_UNUSED_RESULT;

#include "gdata-parser.h"

G_END_DECLS

#endif /* !GDATA_PRIVATE_H */
