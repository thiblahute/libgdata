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

#include "gdata-calendar-event.h"
#include "gdata-private.h"
#include "gdata-service.h"
#include "gdata-gdata.h"
#include "gdata-parser.h"
#include "gdata-types.h"

static void gdata_calendar_event_finalize (GObject *object);
static void gdata_calendar_event_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_calendar_event_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void get_xml (GDataEntry *entry, GString *xml_string);
static const gchar *get_namespaces (GDataEntry *entry);

struct _GDataCalendarEventPrivate {
	gchar *timezone;
};

/*
enum {
	PROP_TIMEZONE = 1,
};*/

G_DEFINE_TYPE (GDataCalendarEvent, gdata_calendar_event, GDATA_TYPE_ENTRY)
#define GDATA_CALENDAR_EVENT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_CALENDAR_EVENT, GDataCalendarEventPrivate))

static void
gdata_calendar_event_class_init (GDataCalendarEventClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataEntryClass *entry_class = GDATA_ENTRY_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataCalendarEventPrivate));

	gobject_class->set_property = gdata_calendar_event_set_property;
	gobject_class->get_property = gdata_calendar_event_get_property;
	gobject_class->finalize = gdata_calendar_event_finalize;

	entry_class->get_xml = get_xml;
	entry_class->get_namespaces = get_namespaces;

	/*g_object_class_install_property (gobject_class, PROP_TIMEZONE,
				g_param_spec_string ("timezone",
					"Timezone", "TODO",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));*/
}

static void
gdata_calendar_event_init (GDataCalendarEvent *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_CALENDAR_EVENT, GDataCalendarEventPrivate);
}

static void
gdata_calendar_event_finalize (GObject *object)
{
	GDataCalendarEventPrivate *priv = GDATA_CALENDAR_EVENT_GET_PRIVATE (object);

	g_free (priv->timezone);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_calendar_event_parent_class)->finalize (object);
}

static void
gdata_calendar_event_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataCalendarEventPrivate *priv = GDATA_CALENDAR_EVENT_GET_PRIVATE (object);

/*	switch (property_id) {
		case PROP_TIMEZONE:
			g_value_set_string (value, priv->timezone);
			break;
		default:
			/* We don't have any other property... /
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}*/
}

static void
gdata_calendar_event_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataCalendarEvent *self = GDATA_CALENDAR_EVENT (object);

/*	switch (property_id) {
		case PROP_TIMEZONE:
			gdata_calendar_event_set_timezone (self, g_value_get_string (value));
			break;
		default:
			/* We don't have any other property... *
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}*/
}

GDataCalendarEvent *
gdata_calendar_event_new (void)
{
	return g_object_new (GDATA_TYPE_CALENDAR_EVENT, NULL);
}

GDataCalendarEvent *
gdata_calendar_event_new_from_xml (const gchar *xml, gint length, GError **error)
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

	return _gdata_calendar_event_new_from_xml_node (doc, node, error);
}

GDataCalendarEvent *
_gdata_calendar_event_new_from_xml_node (xmlDoc *doc, xmlNode *node, GError **error)
{
	GDataCalendarEvent *event;

	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);
	g_return_val_if_fail (xmlStrcmp (node->name, (xmlChar*) "entry") == 0, FALSE);

	event = gdata_calendar_event_new ();

	node = node->xmlChildrenNode;
	while (node != NULL) {
		if (_gdata_calendar_event_parse_xml_node (event, doc, node, error) == FALSE) {
			g_object_unref (event);
			return NULL;
		}
		node = node->next;
	}

	return event;
}

gboolean
_gdata_calendar_event_parse_xml_node (GDataCalendarEvent *self, xmlDoc *doc, xmlNode *node, GError **error)
{
	GError *child_error = NULL;

	g_return_val_if_fail (GDATA_IS_CALENDAR_EVENT (self), FALSE);
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);

	if (xmlStrcmp (node->name, (xmlChar*) "timezone") == 0) {
		/* gCal:timezone */
		xmlChar *timezone = xmlGetProp (node, (xmlChar*) "value");
		if (timezone == NULL)
			return gdata_parser_error_required_property_missing ("gCal:timezone", "value", error);
		/*gdata_calendar_event_set_timezone (self, (gchar*) timezone);*/
		xmlFree (timezone);
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
	GDataCalendarEventPrivate *priv = GDATA_CALENDAR_EVENT (entry)->priv;

	/* Chain up to the parent class */
	GDATA_ENTRY_CLASS (gdata_calendar_event_parent_class)->get_xml (entry, xml_string);

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

/*const gchar *
gdata_calendar_event_get_timezone (GDataCalendarEvent *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_EVENT (self), NULL);
	return self->priv->timezone;
}

void
gdata_calendar_event_set_timezone (GDataCalendarEvent *self, const gchar *timezone)
{
	g_return_if_fail (GDATA_IS_CALENDAR_EVENT (self));

	g_free (self->priv->timezone);
	self->priv->timezone = g_strdup (timezone);
	g_object_notify (G_OBJECT (self), "timezone");
}*/
