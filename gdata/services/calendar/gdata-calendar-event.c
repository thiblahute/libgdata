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
	GTimeVal edited;
	gchar *event_status;
	gchar *visibility;
	gchar *transparency;
	gchar *uid;
	guint sequence;
	GTimeVal start_time;
	GTimeVal end_time;
	gchar *when_value;
	GList *reminders;
	gboolean guests_can_modify; /* TODO: Merge these three somehow? */
	gboolean guests_can_invite_others;
	gboolean guests_can_see_guests;
	gboolean anyone_can_add_self;
	GList *people;
	GList *places;
};

enum {
	PROP_EDITED = 1,
	PROP_EVENT_STATUS,
	PROP_VISIBILITY,
	PROP_TRANSPARENCY,
	PROP_UID,
	PROP_SEQUENCE,
	PROP_START_TIME,
	PROP_END_TIME,
	PROP_WHEN_VALUE,
	PROP_GUESTS_CAN_MODIFY,
	PROP_GUESTS_CAN_INVITE_OTHERS,
	PROP_GUESTS_CAN_SEE_GUESTS,
	PROP_ANYONE_CAN_ADD_SELF
};

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

	g_object_class_install_property (gobject_class, PROP_EDITED,
				g_param_spec_boxed ("edited",
					"Edited", "TODO",
					G_TYPE_TIME_VAL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_EVENT_STATUS,
				g_param_spec_string ("event-status",
					"Event status", "TODO",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_VISIBILITY,
				g_param_spec_string ("visibility",
					"Visibility", "TODO",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_TRANSPARENCY,
				g_param_spec_string ("transparency",
					"Transparency", "TODO",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_UID,
				g_param_spec_string ("uid",
					"UID", "TODO",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_SEQUENCE,
				g_param_spec_uint ("sequence",
					"Sequence", "TODO",
					0, G_MAXUINT, 0,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_START_TIME,
				g_param_spec_boxed ("start-time",
					"Start time", "TODO",
					G_TYPE_TIME_VAL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_END_TIME,
				g_param_spec_boxed ("end-time",
					"End time", "TODO",
					G_TYPE_TIME_VAL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	/* TODO: when-value is a stupid name */
	g_object_class_install_property (gobject_class, PROP_WHEN_VALUE,
				g_param_spec_string ("when-value",
					"When value", "TODO",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_GUESTS_CAN_MODIFY,
				g_param_spec_boolean ("guests-can-modify",
					"Guests can modify", "TODO",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_GUESTS_CAN_INVITE_OTHERS,
				g_param_spec_boolean ("guests-can-invite-others",
					"Guests can invite others", "TODO",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_GUESTS_CAN_SEE_GUESTS,
				g_param_spec_boolean ("guests-can-see-guests",
					"Guests can see guests", "TODO",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_ANYONE_CAN_ADD_SELF,
				g_param_spec_boolean ("anyone-can-add-self",
					"Anyone can add self", "TODO",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
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

	g_free (priv->event_status);
	g_free (priv->visibility);
	g_free (priv->transparency);
	g_free (priv->uid);
	g_free (priv->when_value);
	g_list_foreach (priv->people, (GFunc) gdata_gd_who_free, NULL);
	g_list_free (priv->people);
	g_list_foreach (priv->places, (GFunc) gdata_gd_where_free, NULL);
	g_list_free (priv->places);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_calendar_event_parent_class)->finalize (object);
}

static void
gdata_calendar_event_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataCalendarEventPrivate *priv = GDATA_CALENDAR_EVENT_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_EDITED:
			g_value_set_boxed (value, &(priv->edited));
			break;
		case PROP_EVENT_STATUS:
			g_value_set_string (value, priv->event_status);
			break;
		case PROP_VISIBILITY:
			g_value_set_string (value, priv->visibility);
			break;
		case PROP_TRANSPARENCY:
			g_value_set_string (value, priv->transparency);
			break;
		case PROP_UID:
			g_value_set_string (value, priv->uid);
			break;
		case PROP_SEQUENCE:
			g_value_set_uint (value, priv->sequence);
			break;
		case PROP_START_TIME:
			g_value_set_boxed (value, &(priv->start_time));
			break;
		case PROP_END_TIME:
			g_value_set_boxed (value, &(priv->end_time));
			break;
		case PROP_WHEN_VALUE:
			g_value_set_string (value, priv->when_value);
			break;
		case PROP_GUESTS_CAN_MODIFY:
			g_value_set_boolean (value, priv->guests_can_modify);
			break;
		case PROP_GUESTS_CAN_INVITE_OTHERS:
			g_value_set_boolean (value, priv->guests_can_invite_others);
			break;
		case PROP_GUESTS_CAN_SEE_GUESTS:
			g_value_set_boolean (value, priv->guests_can_see_guests);
			break;
		case PROP_ANYONE_CAN_ADD_SELF:
			g_value_set_boolean (value, priv->anyone_can_add_self);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gdata_calendar_event_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataCalendarEvent *self = GDATA_CALENDAR_EVENT (object);

	switch (property_id) {
		case PROP_EDITED:
			gdata_calendar_event_set_edited (self, g_value_get_boxed (value));
			break;
		case PROP_EVENT_STATUS:
			gdata_calendar_event_set_event_status (self, g_value_get_string (value));
			break;
		case PROP_VISIBILITY:
			gdata_calendar_event_set_visibility (self, g_value_get_string (value));
			break;
		case PROP_TRANSPARENCY:
			gdata_calendar_event_set_transparency (self, g_value_get_string (value));
			break;
		case PROP_UID:
			gdata_calendar_event_set_uid (self, g_value_get_string (value));
			break;
		case PROP_SEQUENCE:
			gdata_calendar_event_set_sequence (self, g_value_get_uint (value));
			break;
		case PROP_START_TIME:
			gdata_calendar_event_set_start_time (self, g_value_get_boxed (value));
			break;
		case PROP_END_TIME:
			gdata_calendar_event_set_end_time (self, g_value_get_boxed (value));
			break;
		case PROP_WHEN_VALUE:
			gdata_calendar_event_set_when_value (self, g_value_get_string (value));
			break;
		case PROP_GUESTS_CAN_MODIFY:
			gdata_calendar_event_set_guests_can_modify (self, g_value_get_boolean (value));
			break;
		case PROP_GUESTS_CAN_INVITE_OTHERS:
			gdata_calendar_event_set_guests_can_invite_others (self, g_value_get_boolean (value));
			break;
		case PROP_GUESTS_CAN_SEE_GUESTS:
			gdata_calendar_event_set_guests_can_see_guests (self, g_value_get_boolean (value));
			break;
		case PROP_ANYONE_CAN_ADD_SELF:
			gdata_calendar_event_set_anyone_can_add_self (self, g_value_get_boolean (value));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
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

	if (xmlStrcmp (node->name, (xmlChar*) "edited") == 0) {
		/* app:edited */
		xmlChar *edited;
		GTimeVal edited_timeval;

		edited = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		if (g_time_val_from_iso8601 ((gchar*) edited, &edited_timeval) == FALSE) {
			/* Error */
			gdata_parser_error_not_iso8601_format ("entry", "app:edited", (gchar*) edited, error);
			xmlFree (edited);
			return FALSE;
		}

		gdata_calendar_event_set_edited (self, &edited_timeval);
		xmlFree (edited);
	} else if (xmlStrcmp (node->name, (xmlChar*) "comments") == 0) {
		/* gd:comments */
		xmlChar *rel, *href, *count_hint;
		guint count_hint_uint;
		GDataGDFeedLink *feed_link;

		/* TODO: This is actually the child of the <comments> element */

		count_hint = xmlGetProp (node, (xmlChar*) "countHint");
		if (count_hint == NULL)
			count_hint_uint = 0;
		else
			count_hint_uint = strtoul ((gchar*) count_hint, NULL, 10);
		xmlFree (count_hint);

		rel = xmlGetProp (node, (xmlChar*) "rel");
		href = xmlGetProp (node, (xmlChar*) "href");

		feed_link = gdata_gd_feed_link_new ((gchar*) href, (gchar*) rel, count_hint_uint);
		/*gdata_calendar_event_set_comments_feed_link (self, feed_link);*/

		xmlFree (rel);
		xmlFree (href);
	} else if (xmlStrcmp (node->name, (xmlChar*) "eventStatus") == 0) {
		/* gd:eventStatus */
		xmlChar *value = xmlGetProp (node, (xmlChar*) "value");
		if (value == NULL)
			return gdata_parser_error_required_property_missing ("gd:eventStatus", "value", error);
		gdata_calendar_event_set_event_status (self, (gchar*) value);
		xmlFree (value);
	} else if (xmlStrcmp (node->name, (xmlChar*) "visibility") == 0) {
		/* gd:visibility */
		xmlChar *value = xmlGetProp (node, (xmlChar*) "value");
		if (value == NULL)
			return gdata_parser_error_required_property_missing ("gd:visibility", "value", error);
		gdata_calendar_event_set_visibility (self, (gchar*) value);
		xmlFree (value);
	} else if (xmlStrcmp (node->name, (xmlChar*) "transparency") == 0) {
		/* gd:transparency */
		xmlChar *value = xmlGetProp (node, (xmlChar*) "value");
		if (value == NULL)
			return gdata_parser_error_required_property_missing ("gd:transparency", "value", error);
		gdata_calendar_event_set_transparency (self, (gchar*) value);
		xmlFree (value);
	} else if (xmlStrcmp (node->name, (xmlChar*) "uid") == 0) {
		/* gCal:uid */
		xmlChar *value = xmlGetProp (node, (xmlChar*) "value");
		if (value == NULL)
			return gdata_parser_error_required_property_missing ("gCal:uid", "value", error);
		gdata_calendar_event_set_uid (self, (gchar*) value);
		xmlFree (value);
	} else if (xmlStrcmp (node->name, (xmlChar*) "sequence") == 0) {
		/* gCal:sequence */
		xmlChar *value;
		guint value_uint;

		value = xmlGetProp (node, (xmlChar*) "value");
		if (value == NULL)
			return gdata_parser_error_required_property_missing ("gCal:sequence", "value", error);
		else
			value_uint = strtoul ((gchar*) value, NULL, 10);
		xmlFree (value);

		gdata_calendar_event_set_sequence (self, value_uint);
	} else if (xmlStrcmp (node->name, (xmlChar*) "when") == 0) {
		/* gd:when */
		xmlChar *start_time, *end_time, *value;
		GTimeVal start_time_timeval, end_time_timeval;

		/* Start time */
		start_time = xmlGetProp (node, (xmlChar*) "startTime");
		if (g_time_val_from_iso8601 ((gchar*) start_time, &start_time_timeval) == FALSE) {
			/* Error */
			gdata_parser_error_not_iso8601_format ("entry", "gd:when", (gchar*) start_time, error);
			xmlFree (start_time);
			return FALSE;
		}
		xmlFree (start_time);
		gdata_calendar_event_set_start_time (self, &start_time_timeval);

		/* End time (optional) */
		end_time = xmlGetProp (node, (xmlChar*) "endTime");
		if (end_time == NULL)
			gdata_calendar_event_set_end_time (self, NULL);
		else {
			if (g_time_val_from_iso8601 ((gchar*) end_time, &end_time_timeval) == FALSE) {
				/* Error */
				gdata_parser_error_not_iso8601_format ("entry", "gd:when", (gchar*) end_time, error);
				xmlFree (end_time);
				return FALSE;
			}
			xmlFree (end_time);
			gdata_calendar_event_set_end_time (self, &end_time_timeval);
		}

		/* Value (optional) */
		value = xmlGetProp (node, (xmlChar*) "value");
		gdata_calendar_event_set_when_value (self, (gchar*) value);
		xmlFree (value);

		/* TODO: Deal with reminders (<gd:reminder> child elements) */
	} else if (xmlStrcmp (node->name, (xmlChar*) "guestsCanModify") == 0) {
		/* gCal:guestsCanModify */
		xmlChar *value = xmlGetProp (node, (xmlChar*) "value");
		if (value == NULL)
			return gdata_parser_error_required_property_missing ("gCal:guestsCanModify", "value", error);
		gdata_calendar_event_set_guests_can_modify (self, (xmlStrcmp (value, (xmlChar*) "true") == 0) ? TRUE : FALSE);
		xmlFree (value);
	} else if (xmlStrcmp (node->name, (xmlChar*) "guestsCanInviteOthers") == 0) {
		/* gCal:guestsCanInviteOthers */
		xmlChar *value = xmlGetProp (node, (xmlChar*) "value");
		if (value == NULL)
			return gdata_parser_error_required_property_missing ("gCal:guestsCanInviteOthers", "value", error);
		gdata_calendar_event_set_guests_can_invite_others (self, (xmlStrcmp (value, (xmlChar*) "true") == 0) ? TRUE : FALSE);
		xmlFree (value);
	} else if (xmlStrcmp (node->name, (xmlChar*) "guestsCanSeeGuests") == 0) {
		/* gCal:guestsCanSeeGuests */
		xmlChar *value = xmlGetProp (node, (xmlChar*) "value");
		if (value == NULL)
			return gdata_parser_error_required_property_missing ("gCal:guestsCanSeeGuests", "value", error);
		gdata_calendar_event_set_guests_can_see_guests (self, (xmlStrcmp (value, (xmlChar*) "true") == 0) ? TRUE : FALSE);
		xmlFree (value);
	} else if (xmlStrcmp (node->name, (xmlChar*) "anyoneCanAddSelf") == 0) {
		/* gCal:anyoneCanAddSelf */
		xmlChar *value = xmlGetProp (node, (xmlChar*) "value");
		if (value == NULL)
			return gdata_parser_error_required_property_missing ("gCal:anyoneCanAddSelf", "value", error);
		gdata_calendar_event_set_anyone_can_add_self (self, (xmlStrcmp (value, (xmlChar*) "true") == 0) ? TRUE : FALSE);
		xmlFree (value);
	} else if (xmlStrcmp (node->name, (xmlChar*) "who") == 0) {
		/* gd:who */
		xmlChar *email, *rel, *value_string;
		GDataGDWho *who;

		rel = xmlGetProp (node, (xmlChar*) "rel");
		value_string = xmlGetProp (node, (xmlChar*) "valueString");
		email = xmlGetProp (node, (xmlChar*) "email");
		/* TODO: deal with the attendeeType, attendeeStatus and entryLink */

		who = gdata_gd_who_new ((gchar*) rel, (gchar*) value_string, (gchar*) email);
		xmlFree (rel);
		xmlFree (value_string);
		xmlFree (email);

		gdata_calendar_event_add_person (self, who);
	} else if (xmlStrcmp (node->name, (xmlChar*) "where") == 0) {
		/* gd:where */
		xmlChar *label, *rel, *value_string;
		GDataGDWhere *where;

		rel = xmlGetProp (node, (xmlChar*) "rel");
		value_string = xmlGetProp (node, (xmlChar*) "valueString");
		label = xmlGetProp (node, (xmlChar*) "label");
		/* TODO: deal with the entryLink */

		where = gdata_gd_where_new ((gchar*) rel, (gchar*) value_string, (gchar*) label);
		xmlFree (rel);
		xmlFree (value_string);
		xmlFree (label);

		gdata_calendar_event_add_place (self, where);
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
	/*if (priv->timezone != NULL)
		g_string_append_printf (xml_string, "<gCal:timezone value='%s'/>", priv->timezone);*/

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

void
gdata_calendar_event_get_edited (GDataCalendarEvent *self, GTimeVal *edited)
{
	g_return_if_fail (GDATA_IS_CALENDAR_EVENT (self));
	g_return_if_fail (edited != NULL);

	edited->tv_sec = self->priv->edited.tv_sec;
	edited->tv_usec = self->priv->edited.tv_usec;
}

void
gdata_calendar_event_set_edited (GDataCalendarEvent *self, GTimeVal *edited)
{
	g_return_if_fail (GDATA_IS_CALENDAR_EVENT (self));
	g_return_if_fail (edited != NULL);

	self->priv->edited.tv_sec = edited->tv_sec;
	self->priv->edited.tv_usec = edited->tv_usec;
	g_object_notify (G_OBJECT (self), "edited");
}

const gchar *
gdata_calendar_event_get_event_status (GDataCalendarEvent *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_EVENT (self), NULL);
	return self->priv->event_status;
}

void
gdata_calendar_event_set_event_status (GDataCalendarEvent *self, const gchar *event_status)
{
	g_return_if_fail (GDATA_IS_CALENDAR_EVENT (self));

	g_free (self->priv->event_status);
	self->priv->event_status = g_strdup (event_status);
	g_object_notify (G_OBJECT (self), "event-status");
}

const gchar *
gdata_calendar_event_get_visibility (GDataCalendarEvent *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_EVENT (self), NULL);
	return self->priv->visibility;
}

void
gdata_calendar_event_set_visibility (GDataCalendarEvent *self, const gchar *visibility)
{
	g_return_if_fail (GDATA_IS_CALENDAR_EVENT (self));

	g_free (self->priv->visibility);
	self->priv->visibility = g_strdup (visibility);
	g_object_notify (G_OBJECT (self), "visibility");
}

const gchar *
gdata_calendar_event_get_transparency (GDataCalendarEvent *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_EVENT (self), NULL);
	return self->priv->transparency;
}

void
gdata_calendar_event_set_transparency (GDataCalendarEvent *self, const gchar *transparency)
{
	g_return_if_fail (GDATA_IS_CALENDAR_EVENT (self));

	g_free (self->priv->transparency);
	self->priv->transparency = g_strdup (transparency);
	g_object_notify (G_OBJECT (self), "transparency");
}

const gchar *
gdata_calendar_event_get_uid (GDataCalendarEvent *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_EVENT (self), NULL);
	return self->priv->uid;
}

void
gdata_calendar_event_set_uid (GDataCalendarEvent *self, const gchar *uid)
{
	g_return_if_fail (GDATA_IS_CALENDAR_EVENT (self));

	g_free (self->priv->uid);
	self->priv->uid = g_strdup (uid);
	g_object_notify (G_OBJECT (self), "uid");
}

guint
gdata_calendar_event_get_sequence (GDataCalendarEvent *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_EVENT (self), 0);
	return self->priv->sequence;
}

void
gdata_calendar_event_set_sequence (GDataCalendarEvent *self, guint sequence)
{
	g_return_if_fail (GDATA_IS_CALENDAR_EVENT (self));
	self->priv->sequence = sequence;
	g_object_notify (G_OBJECT (self), "sequence");
}

void
gdata_calendar_event_get_start_time (GDataCalendarEvent *self, GTimeVal *start_time)
{
	g_return_if_fail (GDATA_IS_CALENDAR_EVENT (self));
	g_return_if_fail (start_time != NULL);

	start_time->tv_sec = self->priv->start_time.tv_sec;
	start_time->tv_usec = self->priv->start_time.tv_usec;
}

void
gdata_calendar_event_set_start_time (GDataCalendarEvent *self, GTimeVal *start_time)
{
	g_return_if_fail (GDATA_IS_CALENDAR_EVENT (self));
	g_return_if_fail (start_time != NULL);

	self->priv->start_time.tv_sec = start_time->tv_sec;
	self->priv->start_time.tv_usec = start_time->tv_usec;
	g_object_notify (G_OBJECT (self), "start-time");
}

void
gdata_calendar_event_get_end_time (GDataCalendarEvent *self, GTimeVal *end_time)
{
	g_return_if_fail (GDATA_IS_CALENDAR_EVENT (self));
	g_return_if_fail (end_time != NULL);

	end_time->tv_sec = self->priv->end_time.tv_sec;
	end_time->tv_usec = self->priv->end_time.tv_usec;
}

void
gdata_calendar_event_set_end_time (GDataCalendarEvent *self, GTimeVal *end_time)
{
	g_return_if_fail (GDATA_IS_CALENDAR_EVENT (self));

	if (end_time == NULL) {
		self->priv->end_time.tv_sec = 0;
		self->priv->end_time.tv_usec = 0;
	} else {
		self->priv->end_time.tv_sec = end_time->tv_sec;
		self->priv->end_time.tv_usec = end_time->tv_usec;
	}
	g_object_notify (G_OBJECT (self), "end-time");
}

const gchar *
gdata_calendar_event_get_when_value (GDataCalendarEvent *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_EVENT (self), NULL);
	return self->priv->when_value;
}

void
gdata_calendar_event_set_when_value (GDataCalendarEvent *self, const gchar *when_value)
{
	g_return_if_fail (GDATA_IS_CALENDAR_EVENT (self));

	g_free (self->priv->when_value);
	self->priv->when_value = g_strdup (when_value);
	g_object_notify (G_OBJECT (self), "when-value");
}

gboolean
gdata_calendar_event_get_guests_can_modify (GDataCalendarEvent *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_EVENT (self), FALSE);
	return self->priv->guests_can_modify;
}

void
gdata_calendar_event_set_guests_can_modify (GDataCalendarEvent *self, gboolean guests_can_modify)
{
	g_return_if_fail (GDATA_IS_CALENDAR_EVENT (self));
	self->priv->guests_can_modify = guests_can_modify;
	g_object_notify (G_OBJECT (self), "guests-can-modify");
}

gboolean
gdata_calendar_event_get_guests_can_invite_others (GDataCalendarEvent *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_EVENT (self), FALSE);
	return self->priv->guests_can_invite_others;
}

void
gdata_calendar_event_set_guests_can_invite_others (GDataCalendarEvent *self, gboolean guests_can_invite_others)
{
	g_return_if_fail (GDATA_IS_CALENDAR_EVENT (self));
	self->priv->guests_can_invite_others = guests_can_invite_others;
	g_object_notify (G_OBJECT (self), "guests-can-invite-others");
}

gboolean
gdata_calendar_event_get_guests_can_see_guests (GDataCalendarEvent *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_EVENT (self), FALSE);
	return self->priv->guests_can_see_guests;
}

void
gdata_calendar_event_set_guests_can_see_guests (GDataCalendarEvent *self, gboolean guests_can_see_guests)
{
	g_return_if_fail (GDATA_IS_CALENDAR_EVENT (self));
	self->priv->guests_can_see_guests = guests_can_see_guests;
	g_object_notify (G_OBJECT (self), "guests-can-see-guests");
}

gboolean
gdata_calendar_event_get_anyone_can_add_self (GDataCalendarEvent *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_EVENT (self), FALSE);
	return self->priv->anyone_can_add_self;
}

void
gdata_calendar_event_set_anyone_can_add_self (GDataCalendarEvent *self, gboolean anyone_can_add_self)
{
	g_return_if_fail (GDATA_IS_CALENDAR_EVENT (self));
	self->priv->anyone_can_add_self = anyone_can_add_self;
	g_object_notify (G_OBJECT (self), "anyone-can-add-self");
}

void
gdata_calendar_event_add_person (GDataCalendarEvent *self, GDataGDWho *who)
{
	g_return_if_fail (GDATA_IS_CALENDAR_EVENT (self));
	g_return_if_fail (who != NULL);

	self->priv->people = g_list_append (self->priv->people, who);
}

void
gdata_calendar_event_add_place (GDataCalendarEvent *self, GDataGDWhere *where)
{
	g_return_if_fail (GDATA_IS_CALENDAR_EVENT (self));
	g_return_if_fail (where != NULL);

	self->priv->places = g_list_append (self->priv->places, where);
}
