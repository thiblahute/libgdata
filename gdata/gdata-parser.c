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
#include <sys/time.h>
#include <time.h>

#include "gdata-service.h"
#include "gdata-parser.h"
#include "gdata-private.h"

GQuark
gdata_parser_error_quark (void)
{
	return g_quark_from_static_string ("gdata-parser-error-quark");
}

gboolean
gdata_parser_error_required_content_missing (const gchar *element_name, GError **error)
{
	/* Translators: the parameter is the name of an XML element.
	 * Do not translate the angle brackets ("<" and ">") — they enclose XML element names. */
	g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
		     _("A <%s> element was missing required content."),
		     element_name);
	return FALSE;
}

gboolean
gdata_parser_error_not_iso8601_format (const gchar *element_name, const gchar *parent_element_name, const gchar *actual_value, GError **error)
{
	/* Translators: the first parameter is the name of an XML element, the second parameter is the name of
	 * another XML element which is owned by (possessive) the first parameter, and the third parameter is
	 * the erroneous value (which was not in ISO 8601 format).
	 * Do not translate the angle brackets ("<" and ">") — they enclose XML element names.
	 *
	 * For example:
	 *  A <media:group>'s <uploaded> element content ("2009-05-06 26:30Z") was not in ISO 8601 format. */
	g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
		     _("A <%s>'s <%s> element content (\"%s\") was not in ISO 8601 format."),
		     parent_element_name, element_name, actual_value);
	return FALSE;
}

gboolean
gdata_parser_error_unhandled_element (const gchar *element_namespace, const gchar *element_name, const gchar *parent_element_name, GError **error)
{
	/* Translators: the first parameter is the name of an XML namespace, the second the name of an XML element, and the third the
	 * name of the XML parent element.
	 * Do not translate the angle brackets ("<" and ">") — they enclose XML element names. */
	g_set_error (error, GDATA_PARSER_ERROR, GDATA_PARSER_ERROR_UNHANDLED_XML_ELEMENT,
		     _("Unhandled <%s:%s> element as a child of <%s>."),
		     element_namespace, element_name, parent_element_name);
	return FALSE;
}

gboolean
gdata_parser_error_unknown_property_value (const gchar *element_name, const gchar *property_name, const gchar *actual_value, GError **error)
{
	/* Translators: the first parameter is an unknown value, the second is the name of an XML element, and the third is
	 * the name of an XML property.
	 * Do not translate the angle brackets ("<" and ">") — they enclose XML element names. Similarly, do not translate (or remove)
	 * the "@" from before the third parameter. */
	g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
		     _("Unknown value \"%s\" of a <%s> @%s property."),
		     actual_value, element_name, property_name);
	return FALSE;
}

gboolean
gdata_parser_error_required_property_missing (const gchar *element_name, const gchar *property_name, GError **error)
{
	/* Translators: the first parameter is the name of an XML property, and the second is the name of an XML element.
	 * Do not translate the angle brackets ("<" and ">") — they enclose XML element names. Similarly, do not translate (or remove)
	 * the "@" from before the first parameter. */
	g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
		     _("A required @%s property of a <%s> was not present."),
		     property_name, element_name);
	return FALSE;
}

gboolean
gdata_parser_error_required_element_missing (const gchar *element_name, const gchar *parent_element_name, GError **error)
{
	/* Translators: the first parameter is the name of an XML element, and the second is the name of the XML parent element.
	 * Do not translate the angle brackets ("<" and ">") — they enclose XML element names. */
	g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
		     _("A required <%s> element as a child of <%s> was not present."),
		     element_name, parent_element_name);
	return FALSE;
}

gboolean
gdata_parser_error_duplicate_element (const gchar *element_name, const gchar *parent_element_name, GError **error)
{
	/* Translators: the first parameter is the name of an XML element, and the second is the name of the XML parent element.
	 * Do not translate the angle brackets ("<" and ">") — they enclose XML element names. */
	g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
		     _("A <%s> element as a child of <%s> was duplicated."),
		     element_name, parent_element_name);
	return FALSE;
}

gboolean
gdata_parser_time_val_from_date (const gchar *date, GTimeVal *_time)
{
	gchar *iso8601_date;
	gboolean success;

	if (strlen (date) != 10 && strlen (date) != 8)
		return FALSE;

	/* Note: This doesn't need translating, as it's outputting an ISO 8601 time string */
	iso8601_date = g_strdup_printf ("%sT00:00:00Z", date);
	success = g_time_val_from_iso8601 (iso8601_date, _time);
	g_free (iso8601_date);

	return success;
}

gchar *
gdata_parser_date_from_time_val (GTimeVal *_time)
{
	time_t secs;
	struct tm *tm;

	secs = _time->tv_sec;
	tm = gmtime (&secs);

	/* Note: This doesn't need translating, as it's outputting an ISO 8601 date string */
	return g_strdup_printf ("%4d-%02d-%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
}
