/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Philip Withnall 2009 <philip@tecnocode.co.uk>
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

#include <glib.h>

#ifndef GDATA_PARSER_H
#define GDATA_PARSER_H

G_BEGIN_DECLS

typedef enum {
	GDATA_PARSER_ERROR_PARSING_STRING = 1,
	GDATA_PARSER_ERROR_EMPTY_DOCUMENT,
	GDATA_PARSER_ERROR_UNHANDLED_XML_ELEMENT
} GDataParserError;

#define GDATA_PARSER_ERROR gdata_parser_error_quark ()
GQuark gdata_parser_error_quark (void);

gboolean gdata_parser_error_required_content_missing (const gchar *element_name, GError **error);
gboolean gdata_parser_error_not_iso8601_format (const gchar *parent_element_name, const gchar *element_name, const gchar *actual_value, GError **error);
gboolean gdata_parser_error_unhandled_element (const gchar *element_namespace, const gchar *element_name, const gchar *parent_element_name, GError **error);
gboolean gdata_parser_error_unknown_property_value (const gchar *element_name, const gchar *property_name, const gchar *actual_value, GError **error);
gboolean gdata_parser_error_required_property_missing (const gchar *element_name, const gchar *property_name, GError **error);
gboolean gdata_parser_error_required_element_missing (const gchar *element_name, const gchar *parent_element_name, GError **error);
gboolean gdata_parser_error_duplicate_element (const gchar *element_name, const gchar *parent_element_name, GError **error);

G_END_DECLS

#endif /* !GDATA_PARSER_H */
