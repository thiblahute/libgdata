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
const gchar *_gdata_parsable_get_extra_xml (GDataParsable *self);
GHashTable *_gdata_parsable_get_extra_namespaces (GDataParsable *self);

#include "gdata-feed.h"
GDataFeed *_gdata_feed_new_from_xml (GType feed_type, const gchar *xml, gint length, GType entry_type,
				     GDataQueryProgressCallback progress_callback, gpointer progress_user_data, GError **error) G_GNUC_WARN_UNUSED_RESULT;

#include "gdata-entry.h"
GDataEntry *_gdata_entry_new_from_xml (GType entry_type, const gchar *xml, gint length, GError **error) G_GNUC_WARN_UNUSED_RESULT;

#include "gdata-parser.h"
gboolean gdata_parser_error_required_content_missing (const gchar *element_name, GError **error);
gboolean gdata_parser_error_not_iso8601_format (const gchar *element_name, const gchar *parent_element_name, const gchar *actual_value, GError **error);
gboolean gdata_parser_error_unhandled_element (const gchar *element_namespace, const gchar *element_name,
					       const gchar *parent_element_name, GError **error);
gboolean gdata_parser_error_unknown_property_value (const gchar *element_name, const gchar *property_name, const gchar *actual_value, GError **error);
gboolean gdata_parser_error_required_property_missing (const gchar *element_name, const gchar *property_name, GError **error);
gboolean gdata_parser_error_required_element_missing (const gchar *element_name, const gchar *parent_element_name, GError **error);
gboolean gdata_parser_error_duplicate_element (const gchar *element_name, const gchar *parent_element_name, GError **error);
gboolean gdata_parser_time_val_from_date (const gchar *date, GTimeVal *_time);
gchar *gdata_parser_date_from_time_val (GTimeVal *_time) G_GNUC_WARN_UNUSED_RESULT;

G_END_DECLS

#endif /* !GDATA_PRIVATE_H */
