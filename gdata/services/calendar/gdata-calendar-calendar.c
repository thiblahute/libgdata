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
 * SECTION:gdata-calendar-calendar
 * @short_description: GData Calendar calendar object
 * @stability: Unstable
 * @include: gdata/services/calendar/gdata-calendar-calendar.h
 *
 * #GDataCalendarCalendar is a subclass of #GDataEntry to represent a calendar from Google Calendar.
 *
 * For more details of Google Calendar's GData API, see the <ulink type="http" url="http://code.google.com/apis/calendar/docs/2.0/reference.html">
 * online documentation</ulink>.
 **/

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <libxml/parser.h>
#include <string.h>

#include "gdata-calendar-calendar.h"
#include "gdata-private.h"
#include "gdata-service.h"
#include "gdata-parser.h"
#include "gdata-types.h"
#include "gdata-access-handler.h"

static void gdata_calendar_calendar_access_handler_init (GDataAccessHandlerIface *iface);
static void gdata_calendar_calendar_finalize (GObject *object);
static void gdata_calendar_calendar_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_calendar_calendar_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void get_xml (GDataParsable *parsable, GString *xml_string);
static gboolean parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error);
static void get_namespaces (GDataParsable *parsable, GHashTable *namespaces);

struct _GDataCalendarCalendarPrivate {
	gchar *timezone;
	guint times_cleaned;
	gboolean is_hidden;
	GDataColor colour;
	gboolean is_selected;
	gchar *access_level;

	GTimeVal edited;
};

enum {
	PROP_TIMEZONE = 1,
	PROP_TIMES_CLEANED,
	PROP_IS_HIDDEN,
	PROP_COLOR,
	PROP_IS_SELECTED,
	PROP_ACCESS_LEVEL,
	PROP_EDITED
};

G_DEFINE_TYPE_WITH_CODE (GDataCalendarCalendar, gdata_calendar_calendar, GDATA_TYPE_ENTRY,
			 G_IMPLEMENT_INTERFACE (GDATA_TYPE_ACCESS_HANDLER, gdata_calendar_calendar_access_handler_init))
#define GDATA_CALENDAR_CALENDAR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_CALENDAR_CALENDAR, GDataCalendarCalendarPrivate))

static void
gdata_calendar_calendar_class_init (GDataCalendarCalendarClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataCalendarCalendarPrivate));

	gobject_class->set_property = gdata_calendar_calendar_set_property;
	gobject_class->get_property = gdata_calendar_calendar_get_property;
	gobject_class->finalize = gdata_calendar_calendar_finalize;

	parsable_class->parse_xml = parse_xml;
	parsable_class->get_xml = get_xml;
	parsable_class->get_namespaces = get_namespaces;

	/**
	 * GDataCalendarCalendar:timezone:
	 *
	 * The timezone in which the calendar's times are given.
	 **/
	g_object_class_install_property (gobject_class, PROP_TIMEZONE,
				g_param_spec_string ("timezone",
					"Timezone", "The timezone in which the calendar's times are given.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataCalendarCalendar:times-cleaned:
	 *
	 * The number of times the calendar has been completely cleared of events.
	 **/
	g_object_class_install_property (gobject_class, PROP_TIMES_CLEANED,
				g_param_spec_uint ("times-cleaned",
					"Times cleaned", "The number of times the calendar has been completely cleared of events.",
					0, G_MAXUINT, 0,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataCalendarCalendar:is-hidden:
	 *
	 * Indicates whether the calendar is visible.
	 *
	 * Since: 0.2.0
	 **/
	g_object_class_install_property (gobject_class, PROP_IS_HIDDEN,
				g_param_spec_boolean ("is-hidden",
					"Hidden?", "Indicates whether the calendar is visible.",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataCalendarCalendar:color:
	 *
	 * The color used to highlight the calendar in the user's browser.
	 **/
	g_object_class_install_property (gobject_class, PROP_COLOR,
				g_param_spec_boxed ("color",
					"Color", "The color used to highlight the calendar in the user's browser.",
					GDATA_TYPE_COLOR,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataCalendarCalendar:is-selected:
	 *
	 * Indicates whether the calendar is selected.
	 *
	 * Since: 0.2.0
	 **/
	g_object_class_install_property (gobject_class, PROP_IS_SELECTED,
				g_param_spec_boolean ("is-selected",
					"Selected?", "Indicates whether the calendar is selected.",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataCalendarCalendar:access-level:
	 *
	 * Indicates what level of access the authenticated user has to the calendar.
	 **/
	g_object_class_install_property (gobject_class, PROP_ACCESS_LEVEL,
				g_param_spec_string ("access-level",
					"Access level", "Indicates what level of access the authenticated user has to the calendar.",
					NULL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataCalendarCalendar:edited:
	 *
	 * The last time the calendar was edited. If the calendar has not been edited yet, the content indicates the time it was created.
	 *
	 * For more information, see the <ulink type="http" url="http://www.atomenabled.org/developers/protocol/#appEdited">
	 * Atom Publishing Protocol specification</ulink>.
	 **/
	g_object_class_install_property (gobject_class, PROP_EDITED,
				g_param_spec_boxed ("edited",
					"Edited", "The last time the calendar was edited.",
					GDATA_TYPE_G_TIME_VAL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_calendar_calendar_init (GDataCalendarCalendar *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_CALENDAR_CALENDAR, GDataCalendarCalendarPrivate);
}

static gboolean
is_owner_rule (GDataAccessRule *rule)
{
	return (strcmp (gdata_access_rule_get_role (rule), "http://schemas.google.com/gCal/2005#owner") == 0) ? TRUE : FALSE;
}

static void
gdata_calendar_calendar_access_handler_init (GDataAccessHandlerIface *iface)
{
	iface->is_owner_rule = is_owner_rule;
}

static void
gdata_calendar_calendar_finalize (GObject *object)
{
	GDataCalendarCalendarPrivate *priv = GDATA_CALENDAR_CALENDAR_GET_PRIVATE (object);

	g_free (priv->timezone);
	xmlFree ((xmlChar*) priv->access_level);

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
		case PROP_IS_HIDDEN:
			g_value_set_boolean (value, priv->is_hidden);
			break;
		case PROP_COLOR:
			g_value_set_boxed (value, &(priv->colour));
			break;
		case PROP_IS_SELECTED:
			g_value_set_boolean (value, priv->is_selected);
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
		case PROP_IS_HIDDEN:
			gdata_calendar_calendar_set_is_hidden (self, g_value_get_boolean (value));
			break;
		case PROP_COLOR:
			gdata_calendar_calendar_set_color (self, g_value_get_boxed (value));
			break;
		case PROP_IS_SELECTED:
			gdata_calendar_calendar_set_is_selected (self, g_value_get_boolean (value));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

/**
 * gdata_calendar_calendar_new:
 * @id: the calendar's ID, or %NULL
 *
 * Creates a new #GDataCalendarCalendar with the given ID and default properties.
 *
 * Return value: a new #GDataCalendarCalendar; unref with g_object_unref()
 **/
GDataCalendarCalendar *
gdata_calendar_calendar_new (const gchar *id)
{
	return g_object_new (GDATA_TYPE_CALENDAR_CALENDAR, "id", id, NULL);
}

/**
 * gdata_calendar_calendar_new_from_xml:
 * @xml: an XML string
 * @length: the length in characters of @xml, or %-1
 * @error: a #GError, or %NULL
 *
 * Creates a new #GDataCalendarCalendar from an XML string. If @length is %-1, the length of
 * the string will be calculated.
 *
 * Errors from #GDataParserError can be returned if problems are found in the XML.
 *
 * Return value: a new #GDataCalendarCalendar, or %NULL; unref with g_object_unref()
 **/
GDataCalendarCalendar *
gdata_calendar_calendar_new_from_xml (const gchar *xml, gint length, GError **error)
{
	return GDATA_CALENDAR_CALENDAR (_gdata_entry_new_from_xml (GDATA_TYPE_CALENDAR_CALENDAR, xml, length, error));
}

static gboolean
parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error)
{
	GDataCalendarCalendar *self;

	g_return_val_if_fail (GDATA_IS_CALENDAR_CALENDAR (parsable), FALSE);
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);

	self = GDATA_CALENDAR_CALENDAR (parsable);

	if (xmlStrcmp (node->name, (xmlChar*) "timezone") == 0) {
		/* gCal:timezone */
		xmlChar *_timezone = xmlGetProp (node, (xmlChar*) "value");
		if (_timezone == NULL)
			return gdata_parser_error_required_property_missing (node, "value", error);
		gdata_calendar_calendar_set_timezone (self, (gchar*) _timezone);
		xmlFree (_timezone);
	} else if (xmlStrcmp (node->name, (xmlChar*) "timesCleaned") == 0) {
		/* gCal:timesCleaned */
		xmlChar *times_cleaned = xmlGetProp (node, (xmlChar*) "value");
		if (times_cleaned == NULL)
			return gdata_parser_error_required_property_missing (node, "value", error);
		self->priv->times_cleaned = strtoul ((gchar*) times_cleaned, NULL, 10);
		xmlFree (times_cleaned);
	} else if (xmlStrcmp (node->name, (xmlChar*) "hidden") == 0) {
		/* gCal:hidden */
		xmlChar *hidden = xmlGetProp (node, (xmlChar*) "value");
		if (hidden == NULL)
			return gdata_parser_error_required_property_missing (node, "value", error);
		gdata_calendar_calendar_set_is_hidden (self, (xmlStrcmp (hidden, (xmlChar*) "true") == 0) ? TRUE : FALSE);
		xmlFree (hidden);
	} else if (xmlStrcmp (node->name, (xmlChar*) "color") == 0) {
		/* gCal:color */
		xmlChar *value;
		GDataColor colour;

		value = xmlGetProp (node, (xmlChar*) "value");
		if (value == NULL)
			return gdata_parser_error_required_property_missing (node, "value", error);
		if (gdata_color_from_hexadecimal ((gchar*) value, &colour) == FALSE) {
			/* Error */
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
				     /* Translators: the first parameter is the name of an XML element (including the angle brackets ("<" and ">"),
				      * and the second parameter is the erroneous value (which was not in hexadecimal RGB format).
				      *
				      * For example:
				      *  The content of a <entry/gCal:color> element ("00FG56") was not in hexadecimal RGB format. */
				     _("The content of a %s element (\"%s\") was not in hexadecimal RGB format."), "<entry/gCal:color>", value);
			xmlFree (value);

			return FALSE;
		}

		gdata_calendar_calendar_set_color (self, &colour);
		xmlFree (value);
	} else if (xmlStrcmp (node->name, (xmlChar*) "selected") == 0) {
		/* gCal:selected */
		xmlChar *selected = xmlGetProp (node, (xmlChar*) "value");
		if (selected == NULL)
			return gdata_parser_error_required_property_missing (node, "value", error);
		gdata_calendar_calendar_set_is_selected (self, (xmlStrcmp (selected, (xmlChar*) "true") == 0) ? TRUE : FALSE);
		xmlFree (selected);
	} else if (xmlStrcmp (node->name, (xmlChar*) "accesslevel") == 0) {
		/* gCal:accesslevel */
		self->priv->access_level = (gchar*) xmlGetProp (node, (xmlChar*) "value");
		if (self->priv->access_level == NULL)
			return gdata_parser_error_required_property_missing (node, "value", error);
	} else if (xmlStrcmp (node->name, (xmlChar*) "edited") == 0) {
		/* app:edited */
		xmlChar *edited = xmlNodeListGetString (doc, node->children, TRUE);
		if (g_time_val_from_iso8601 ((gchar*) edited, &(self->priv->edited)) == FALSE) {
			/* Error */
			gdata_parser_error_not_iso8601_format (node, (gchar*) edited, error);
			xmlFree (edited);
			return FALSE;
		}
		xmlFree (edited);
	} else if (GDATA_PARSABLE_CLASS (gdata_calendar_calendar_parent_class)->parse_xml (parsable, doc, node, user_data, error) == FALSE) {
		/* Error! */
		return FALSE;
	}

	return TRUE;
}

static void
get_xml (GDataParsable *parsable, GString *xml_string)
{
	gchar *colour;
	GDataCalendarCalendarPrivate *priv = GDATA_CALENDAR_CALENDAR (parsable)->priv;

	/* Chain up to the parent class */
	GDATA_PARSABLE_CLASS (gdata_calendar_calendar_parent_class)->get_xml (parsable, xml_string);

	/* Add all the Calendar-specific XML */
	if (priv->timezone != NULL) {
		gchar *_timezone = g_markup_escape_text (priv->timezone, -1);
		g_string_append_printf (xml_string, "<gCal:timezone value='%s'/>", _timezone);
		g_free (_timezone);
	}

	if (priv->is_hidden == TRUE)
		g_string_append (xml_string, "<gCal:hidden value='true'/>");
	else
		g_string_append (xml_string, "<gCal:hidden value='false'/>");

	colour = gdata_color_to_hexadecimal (&(priv->colour));
	g_string_append_printf (xml_string, "<gCal:color value='%s'/>", colour);
	g_free (colour);

	if (priv->is_selected == TRUE)
		g_string_append (xml_string, "<gCal:selected value='true'/>");
	else
		g_string_append (xml_string, "<gCal:selected value='false'/>");
}

static void
get_namespaces (GDataParsable *parsable, GHashTable *namespaces)
{
	/* Chain up to the parent class */
	GDATA_PARSABLE_CLASS (gdata_calendar_calendar_parent_class)->get_namespaces (parsable, namespaces);

	g_hash_table_insert (namespaces, (gchar*) "gCal", (gchar*) "http://schemas.google.com/gCal/2005");
	g_hash_table_insert (namespaces, (gchar*) "app", (gchar*) "http://www.w3.org/2007/app");
}

/**
 * gdata_calendar_calendar_get_timezone:
 * @self: a #GDataCalendarCalendar
 *
 * Gets the #GDataCalendarCalendar:timezone property.
 *
 * Return value: the calendar's timezone, or %NULL
 **/
const gchar *
gdata_calendar_calendar_get_timezone (GDataCalendarCalendar *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_CALENDAR (self), NULL);
	return self->priv->timezone;
}

/**
 * gdata_calendar_calendar_set_timezone:
 * @self: a #GDataCalendarCalendar
 * @_timezone: a new timezone, or %NULL
 *
 * Sets the #GDataCalendarCalendar:timezone property to the new timezone, @_timezone.
 *
 * Set @_timezone to %NULL to unset the property in the calendar.
 **/
void
gdata_calendar_calendar_set_timezone (GDataCalendarCalendar *self, const gchar *_timezone)
{
	/* Blame "timezone" in /usr/include/time.h:291 for the weird parameter naming */
	g_return_if_fail (GDATA_IS_CALENDAR_CALENDAR (self));

	g_free (self->priv->timezone);
	self->priv->timezone = g_strdup (_timezone);
	g_object_notify (G_OBJECT (self), "timezone");
}

/**
 * gdata_calendar_calendar_get_times_cleaned:
 * @self: a #GDataCalendarCalendar
 *
 * Gets the #GDataCalendarCalendar:times-cleaned property.
 *
 * Return value: the number of times the calendar has been totally emptied
 **/
guint
gdata_calendar_calendar_get_times_cleaned (GDataCalendarCalendar *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_CALENDAR (self), 0);
	return self->priv->times_cleaned;
}

/**
 * gdata_calendar_calendar_is_hidden:
 * @self: a #GDataCalendarCalendar
 *
 * Gets the #GDataCalendarCalendar:is-hidden property.
 *
 * Return value: %TRUE if the calendar is hidden, %FALSE otherwise
 *
 * Since: 0.2.0
 **/
gboolean
gdata_calendar_calendar_is_hidden (GDataCalendarCalendar *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_CALENDAR (self), 0);
	return self->priv->is_hidden;
}

/**
 * gdata_calendar_calendar_set_is_hidden:
 * @self: a #GDataCalendarCalendar
 * @is_hidden: %TRUE to hide the calendar, %FALSE otherwise
 *
 * Sets the #GDataCalendarCalendar:is-hidden property to @is_hidden.
 *
 * Since: 0.2.0
 **/
void
gdata_calendar_calendar_set_is_hidden (GDataCalendarCalendar *self, gboolean is_hidden)
{
	g_return_if_fail (GDATA_IS_CALENDAR_CALENDAR (self));
	self->priv->is_hidden = is_hidden;
	g_object_notify (G_OBJECT (self), "is-hidden");
}

/**
 * gdata_calendar_calendar_get_color:
 * @self: a #GDataCalendarCalendar
 * @color: a #GDataColor
 *
 * Gets the #GDataCalendarCalendar:color property and puts it in @color.
 **/
void
gdata_calendar_calendar_get_color (GDataCalendarCalendar *self, GDataColor *color)
{
	g_return_if_fail (GDATA_IS_CALENDAR_CALENDAR (self));
	g_return_if_fail (color != NULL);
	*color = self->priv->colour;
}

/**
 * gdata_calendar_calendar_set_color:
 * @self: a #GDataCalendarCalendar
 * @color: a new #GDataColor
 *
 * Sets the #GDataCalendarCalendar:color property to @color.
 **/
void
gdata_calendar_calendar_set_color (GDataCalendarCalendar *self, GDataColor *color)
{
	g_return_if_fail (GDATA_IS_CALENDAR_CALENDAR (self));
	g_return_if_fail (color != NULL);
	self->priv->colour = *color;
	g_object_notify (G_OBJECT (self), "color");
}

/**
 * gdata_calendar_calendar_is_selected:
 * @self: a #GDataCalendarCalendar
 *
 * Gets the #GDataCalendarCalendar:is-selected property.
 *
 * Return value: %TRUE if the calendar is selected, %FALSE otherwise
 *
 * Since: 0.2.0
 **/
gboolean
gdata_calendar_calendar_is_selected (GDataCalendarCalendar *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_CALENDAR (self), 0);
	return self->priv->is_selected;
}

/**
 * gdata_calendar_calendar_set_is_selected:
 * @self: a #GDataCalendarCalendar
 * @is_selected: %TRUE to select the calendar, %FALSE otherwise
 *
 * Sets the #GDataCalendarCalendar:is-selected property to @is_selected.
 *
 * Since: 0.2.0
 **/
void
gdata_calendar_calendar_set_is_selected (GDataCalendarCalendar *self, gboolean is_selected)
{
	g_return_if_fail (GDATA_IS_CALENDAR_CALENDAR (self));
	self->priv->is_selected = is_selected;
	g_object_notify (G_OBJECT (self), "is-selected");
}

/**
 * gdata_calendar_calendar_get_access_level:
 * @self: a #GDataCalendarCalendar
 *
 * Gets the #GDataCalendarCalendar:access-level property.
 *
 * Return value: the authenticated user's access level to the calendar, or %NULL
 **/
const gchar *
gdata_calendar_calendar_get_access_level (GDataCalendarCalendar *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_CALENDAR (self), NULL);
	return self->priv->access_level;
}

/**
 * gdata_calendar_calendar_get_edited:
 * @self: a #GDataCalendarCalendar
 * @edited: a #GTimeVal
 *
 * Gets the #GDataCalendarCalendar:edited property and puts it in @edited. If the property is unset,
 * both fields in the #GTimeVal will be set to %0.
 **/
void
gdata_calendar_calendar_get_edited (GDataCalendarCalendar *self, GTimeVal *edited)
{
	g_return_if_fail (GDATA_IS_CALENDAR_CALENDAR (self));
	g_return_if_fail (edited != NULL);
	*edited = self->priv->edited;
}
