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

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <libxml/parser.h>
#include <string.h>

#include "gdata-calendar-calendar.h"
#include "gdata-private.h"
#include "gdata-service.h"
#include "gdata-gdata.h"
#include "gdata-parser.h"
#include "gdata-types.h"

static void gdata_calendar_calendar_finalize (GObject *object);
static void gdata_calendar_calendar_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_calendar_calendar_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void get_xml (GDataEntry *entry, GString *xml_string);
static const gchar *get_namespaces (GDataEntry *entry);

struct _GDataCalendarCalendarPrivate {
	gchar *timezone;
	guint times_cleaned;
	gboolean hidden;
	GDataColor colour;
	gboolean selected;
	gchar *access_level;

	GTimeVal edited;
};

enum {
	PROP_TIMEZONE = 1,
	PROP_TIMES_CLEANED,
	PROP_HIDDEN,
	PROP_COLOR,
	PROP_SELECTED,
	PROP_ACCESS_LEVEL,
	PROP_EDITED
};

G_DEFINE_TYPE (GDataCalendarCalendar, gdata_calendar_calendar, GDATA_TYPE_ENTRY)
#define GDATA_CALENDAR_CALENDAR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_CALENDAR_CALENDAR, GDataCalendarCalendarPrivate))

static void
gdata_calendar_calendar_class_init (GDataCalendarCalendarClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataEntryClass *entry_class = GDATA_ENTRY_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataCalendarCalendarPrivate));

	gobject_class->set_property = gdata_calendar_calendar_set_property;
	gobject_class->get_property = gdata_calendar_calendar_get_property;
	gobject_class->finalize = gdata_calendar_calendar_finalize;

	entry_class->get_xml = get_xml;
	entry_class->get_namespaces = get_namespaces;

	g_object_class_install_property (gobject_class, PROP_TIMEZONE,
				g_param_spec_string ("timezone",
					"Timezone", "TODO",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_TIMES_CLEANED,
				g_param_spec_uint ("times-cleaned",
					"Times cleaned", "TODO",
					0, G_MAXUINT, 0,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_HIDDEN,
				g_param_spec_boolean ("hidden",
					"Hidden", "TODO",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_COLOR,
				g_param_spec_boxed ("color",
					"Color", "TODO",
					GDATA_TYPE_COLOR,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_SELECTED,
				g_param_spec_boolean ("selected",
					"Selected", "TODO",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_ACCESS_LEVEL,
				g_param_spec_string ("access-level",
					"Access level", "TODO",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_EDITED,
				g_param_spec_boxed ("edited",
					"Edited", "TODO",
					G_TYPE_TIME_VAL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_calendar_calendar_init (GDataCalendarCalendar *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_CALENDAR_CALENDAR, GDataCalendarCalendarPrivate);
}

static void
gdata_calendar_calendar_finalize (GObject *object)
{
	GDataCalendarCalendarPrivate *priv = GDATA_CALENDAR_CALENDAR_GET_PRIVATE (object);

	g_free (priv->timezone);
	g_free (priv->access_level);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_calendar_calendar_parent_class)->finalize (object);
}

static void
gdata_calendar_calendar_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataCalendarCalendarPrivate *priv = GDATA_CALENDAR_CALENDAR_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_TIMEZONE:
			g_value_set_string (value, priv->timezone);
			break;
		case PROP_TIMES_CLEANED:
			g_value_set_uint (value, priv->times_cleaned);
			break;
		case PROP_HIDDEN:
			g_value_set_boolean (value, priv->hidden);
			break;
		case PROP_COLOR:
			g_value_set_boxed (value, &(priv->colour));
			break;
		case PROP_SELECTED:
			g_value_set_boolean (value, priv->selected);
			break;
		case PROP_ACCESS_LEVEL:
			g_value_set_string (value, priv->access_level);
			break;
		case PROP_EDITED:
			g_value_set_boxed (value, &(priv->edited));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gdata_calendar_calendar_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataCalendarCalendar *self = GDATA_CALENDAR_CALENDAR (object);

	switch (property_id) {
		case PROP_TIMEZONE:
			gdata_calendar_calendar_set_timezone (self, g_value_get_string (value));
			break;
		case PROP_TIMES_CLEANED:
			gdata_calendar_calendar_set_times_cleaned (self, g_value_get_uint (value));
			break;
		case PROP_HIDDEN:
			gdata_calendar_calendar_set_hidden (self, g_value_get_boolean (value));
			break;
		case PROP_COLOR:
			gdata_calendar_calendar_set_color (self, g_value_get_boxed (value));
			break;
		case PROP_SELECTED:
			gdata_calendar_calendar_set_selected (self, g_value_get_boolean (value));
			break;
		case PROP_ACCESS_LEVEL:
			gdata_calendar_calendar_set_access_level (self, g_value_get_string (value));
			break;
		case PROP_EDITED:
			gdata_calendar_calendar_set_edited (self, g_value_get_boxed (value));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

GDataCalendarCalendar *
gdata_calendar_calendar_new (void)
{
	return g_object_new (GDATA_TYPE_CALENDAR_CALENDAR, NULL);
}

GDataCalendarCalendar *
gdata_calendar_calendar_new_from_xml (const gchar *xml, gint length, GError **error)
{
	xmlDoc *doc;
	xmlNode *node;

	g_return_val_if_fail (xml != NULL, NULL);

	if (length == -1)
		length = strlen (xml);

	/* Parse the XML */
	doc = xmlReadMemory (xml, length, "entry.xml", NULL, 0);
	if (doc == NULL) {
		xmlError *xml_error = xmlGetLastError ();
		g_set_error (error, GDATA_PARSER_ERROR, GDATA_PARSER_ERROR_PARSING_STRING,
			     _("Error parsing XML: %s"),
			     xml_error->message);
		return NULL;
	}

	/* Get the root element */
	node = xmlDocGetRootElement (doc);
	if (node == NULL) {
		/* XML document's empty */
		xmlFreeDoc (doc);
		g_set_error (error, GDATA_PARSER_ERROR, GDATA_PARSER_ERROR_EMPTY_DOCUMENT,
			     _("Error parsing XML: %s"),
			     _("Empty document."));
		return NULL;
	}

	if (xmlStrcmp (node->name, (xmlChar*) "entry") != 0) {
		/* No <entry> element (required) */
		xmlFreeDoc (doc);
		gdata_parser_error_required_element_missing ("entry", "root", error);
		return NULL;
	}

	return _gdata_calendar_calendar_new_from_xml_node (doc, node, error);
}

GDataCalendarCalendar *
_gdata_calendar_calendar_new_from_xml_node (xmlDoc *doc, xmlNode *node, GError **error)
{
	GDataCalendarCalendar *calendar;

	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);
	g_return_val_if_fail (xmlStrcmp (node->name, (xmlChar*) "entry") == 0, FALSE);

	calendar = gdata_calendar_calendar_new ();

	node = node->xmlChildrenNode;
	while (node != NULL) {
		if (_gdata_calendar_calendar_parse_xml_node (calendar, doc, node, error) == FALSE) {
			g_object_unref (calendar);
			return NULL;
		}
		node = node->next;
	}

	return calendar;
}

gboolean
_gdata_calendar_calendar_parse_xml_node (GDataCalendarCalendar *self, xmlDoc *doc, xmlNode *node, GError **error)
{
	GError *child_error = NULL;

	g_return_val_if_fail (GDATA_IS_CALENDAR_CALENDAR (self), FALSE);
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);

	if (xmlStrcmp (node->name, (xmlChar*) "timezone") == 0) {
		/* gCal:timezone */
		xmlChar *timezone = xmlGetProp (node, (xmlChar*) "value");
		if (timezone == NULL)
			return gdata_parser_error_required_property_missing ("gCal:timezone", "value", error);
		gdata_calendar_calendar_set_timezone (self, (gchar*) timezone);
		xmlFree (timezone);
	} else if (xmlStrcmp (node->name, (xmlChar*) "timesCleaned") == 0) {
		/* gCal:timesCleaned */
		xmlChar *times_cleaned = xmlGetProp (node, (xmlChar*) "value");
		if (times_cleaned == NULL)
			return gdata_parser_error_required_property_missing ("gCal:timesCleaned", "value", error);
		gdata_calendar_calendar_set_times_cleaned (self, strtoul ((gchar*) times_cleaned, NULL, 10));
		xmlFree (times_cleaned);
	} else if (xmlStrcmp (node->name, (xmlChar*) "hidden") == 0) {
		/* gCal:hidden */
		xmlChar *hidden = xmlGetProp (node, (xmlChar*) "value");
		if (hidden == NULL)
			return gdata_parser_error_required_property_missing ("gCal:hidden", "value", error);
		gdata_calendar_calendar_set_hidden (self, (xmlStrcmp (hidden, (xmlChar*) "true") == 0) ? TRUE : FALSE);
		xmlFree (hidden);
	} else if (xmlStrcmp (node->name, (xmlChar*) "color") == 0) {
		/* gCal:color */
		xmlChar *value;
		GDataColor colour;

		value = xmlGetProp (node, (xmlChar*) "value");
		if (value == NULL)
			return gdata_parser_error_required_property_missing ("gCal:color", "value", error);
		if (gdata_color_from_hexadecimal ((gchar*) value, &colour) == FALSE) {
			/* Error */
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
				     _("A <%s>'s <%s> element (\"%s\") was not in hexadecimal RGB format."),
				     "entry", "gCal:color", value);
			xmlFree (value);
			return FALSE;
		}

		gdata_calendar_calendar_set_color (self, &colour);
		xmlFree (value);
	} else if (xmlStrcmp (node->name, (xmlChar*) "selected") == 0) {
		/* gCal:selected */
		xmlChar *selected = xmlGetProp (node, (xmlChar*) "value");
		if (selected == NULL)
			return gdata_parser_error_required_property_missing ("gCal:selected", "value", error);
		gdata_calendar_calendar_set_selected (self, (xmlStrcmp (selected, (xmlChar*) "true") == 0) ? TRUE : FALSE);
		xmlFree (selected);
	} else if (xmlStrcmp (node->name, (xmlChar*) "accesslevel") == 0) {
		/* gCal:accesslevel */
		xmlChar *value = xmlGetProp (node, (xmlChar*) "value");
		if (value == NULL)
			return gdata_parser_error_required_property_missing ("gCal:accesslevel", "value", error);
		gdata_calendar_calendar_set_access_level (self, (gchar*) value);
		xmlFree (value);
	} else if (xmlStrcmp (node->name, (xmlChar*) "edited") == 0) {
		/* app:edited */
		xmlChar *edited;
		GTimeVal edited_timeval;

		edited = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		if (g_time_val_from_iso8601 ((gchar*) edited, &edited_timeval) == FALSE) {
			/* Error */
			gdata_parser_error_not_iso8601_format ("app:edited", "entry", (gchar*) edited, error);
			xmlFree (edited);
			return FALSE;
		}

		gdata_calendar_calendar_set_edited (self, &edited_timeval);
		xmlFree (edited);
	} else if (_gdata_entry_parse_xml_node (GDATA_ENTRY (self), doc, node, &child_error) == FALSE) {
		if (g_error_matches (child_error, GDATA_PARSER_ERROR, GDATA_PARSER_ERROR_UNHANDLED_XML_ELEMENT) == TRUE) {
			g_error_free (child_error);
			gdata_parser_error_unhandled_element ((gchar*) node->ns->prefix, (gchar*) node->name, "entry", error);
		} else {
			g_propagate_error (error, child_error);
		}

		return FALSE;
	}

	return TRUE;
}

static void
get_xml (GDataEntry *entry, GString *xml_string)
{
	GDataCalendarCalendarPrivate *priv = GDATA_CALENDAR_CALENDAR (entry)->priv;

	/* Chain up to the parent class */
	GDATA_ENTRY_CLASS (gdata_calendar_calendar_parent_class)->get_xml (entry, xml_string);

	/* Add all the Calendar-specific XML */
	if (priv->timezone != NULL)
		g_string_append_printf (xml_string, "<gCal:timezone value='%s'/>", priv->timezone);

	/* TODO:
	 * - Finish supporting all tags
	 * - Check all tags here are valid for insertions and updates
	 * - Check things are escaped (or not) as appropriate
	 * - Write a function to encapsulate g_markup_escape_text and
	 *   g_string_append_printf to reduce the number of allocations
	 */
}

static const gchar *
get_namespaces (GDataEntry *entry)
{
	return "xmlns:gCal='http://schemas.google.com/gCal/2005' xmlns:app='http://www.w3.org/2007/app'";
}

const gchar *
gdata_calendar_calendar_get_timezone (GDataCalendarCalendar *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_CALENDAR (self), NULL);
	return self->priv->timezone;
}

void
gdata_calendar_calendar_set_timezone (GDataCalendarCalendar *self, const gchar *timezone)
{
	g_return_if_fail (GDATA_IS_CALENDAR_CALENDAR (self));

	g_free (self->priv->timezone);
	self->priv->timezone = g_strdup (timezone);
	g_object_notify (G_OBJECT (self), "timezone");
}

guint
gdata_calendar_calendar_get_times_cleaned (GDataCalendarCalendar *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_CALENDAR (self), 0);
	return self->priv->times_cleaned;
}

void
gdata_calendar_calendar_set_times_cleaned (GDataCalendarCalendar *self, guint times_cleaned)
{
	g_return_if_fail (GDATA_IS_CALENDAR_CALENDAR (self));
	self->priv->times_cleaned = times_cleaned;
	g_object_notify (G_OBJECT (self), "times-cleaned");
}

gboolean
gdata_calendar_calendar_get_hidden (GDataCalendarCalendar *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_CALENDAR (self), 0);
	return self->priv->hidden;
}

void
gdata_calendar_calendar_set_hidden (GDataCalendarCalendar *self, gboolean hidden)
{
	g_return_if_fail (GDATA_IS_CALENDAR_CALENDAR (self));
	self->priv->hidden = hidden;
	g_object_notify (G_OBJECT (self), "hidden");
}

void
gdata_calendar_calendar_get_color (GDataCalendarCalendar *self, GDataColor *color)
{
	g_return_if_fail (GDATA_IS_CALENDAR_CALENDAR (self));
	g_return_if_fail (color != NULL);

	color->red = self->priv->colour.red;
	color->blue = self->priv->colour.blue;
	color->green = self->priv->colour.green;
}

void
gdata_calendar_calendar_set_color (GDataCalendarCalendar *self, GDataColor *color)
{
	g_return_if_fail (GDATA_IS_CALENDAR_CALENDAR (self));
	g_return_if_fail (color != NULL);

	self->priv->colour.red = color->red;
	self->priv->colour.blue = color->blue;
	self->priv->colour.green = color->green;
	g_object_notify (G_OBJECT (self), "color");
}

gboolean
gdata_calendar_calendar_get_selected (GDataCalendarCalendar *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_CALENDAR (self), 0);
	return self->priv->selected;
}

void
gdata_calendar_calendar_set_selected (GDataCalendarCalendar *self, gboolean selected)
{
	g_return_if_fail (GDATA_IS_CALENDAR_CALENDAR (self));
	self->priv->selected = selected;
	g_object_notify (G_OBJECT (self), "selected");
}

const gchar *
gdata_calendar_calendar_get_access_level (GDataCalendarCalendar *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_CALENDAR (self), NULL);
	return self->priv->access_level;
}

void
gdata_calendar_calendar_set_access_level (GDataCalendarCalendar *self, const gchar *access_level)
{
	g_return_if_fail (GDATA_IS_CALENDAR_CALENDAR (self));

	g_free (self->priv->access_level);
	self->priv->access_level = g_strdup (access_level);
	g_object_notify (G_OBJECT (self), "access-level");
}

void
gdata_calendar_calendar_get_edited (GDataCalendarCalendar *self, GTimeVal *edited)
{
	g_return_if_fail (GDATA_IS_CALENDAR_CALENDAR (self));
	g_return_if_fail (edited != NULL);

	edited->tv_sec = self->priv->edited.tv_sec;
	edited->tv_usec = self->priv->edited.tv_usec;
}

void
gdata_calendar_calendar_set_edited (GDataCalendarCalendar *self, GTimeVal *edited)
{
	g_return_if_fail (GDATA_IS_CALENDAR_CALENDAR (self));
	g_return_if_fail (edited != NULL);

	self->priv->edited.tv_sec = edited->tv_sec;
	self->priv->edited.tv_usec = edited->tv_usec;
	g_object_notify (G_OBJECT (self), "edited");
}
