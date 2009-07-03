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
 * SECTION:gdata-calendar-feed
 * @short_description: GData Calendar feed object
 * @stability: Unstable
 * @include: gdata/services/calendar/gdata-calendar-feed.h
 *
 * #GDataCalendarFeed is a subclass of #GDataFeed to represent a results feed from Google Calendar. It adds a couple of
 * properties which are specific to the Google Calendar API.
 **/

#include <glib.h>
#include <libxml/parser.h>

#include "gdata-calendar-feed.h"
#include "gdata-feed.h"
#include "gdata-private.h"

static void gdata_calendar_feed_finalize (GObject *object);
static void gdata_calendar_feed_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static gboolean parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error);

struct _GDataCalendarFeedPrivate {
	gchar *timezone;
	guint times_cleaned;
};

enum {
	PROP_TIMEZONE = 1,
	PROP_TIMES_CLEANED
};

G_DEFINE_TYPE (GDataCalendarFeed, gdata_calendar_feed, GDATA_TYPE_FEED)
#define GDATA_CALENDAR_FEED_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_CALENDAR_FEED, GDataCalendarFeedPrivate))

static void
gdata_calendar_feed_class_init (GDataCalendarFeedClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataCalendarFeedPrivate));

	gobject_class->get_property = gdata_calendar_feed_get_property;
	gobject_class->finalize = gdata_calendar_feed_finalize;

	parsable_class->parse_xml = parse_xml;

	/**
	 * GDataCalendarFeed:timezone:
	 *
	 * The timezone in which the feed's times are given.
	 *
	 * Since: 0.3.0
	 **/
	g_object_class_install_property (gobject_class, PROP_TIMEZONE,
				g_param_spec_string ("timezone",
					"Timezone", "The timezone in which the feed's times are given.",
					NULL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataCalendarFeed:times-cleaned:
	 *
	 * The number of times the feed has been completely cleared of entries.
	 *
	 * Since: 0.3.0
	 **/
	g_object_class_install_property (gobject_class, PROP_TIMES_CLEANED,
				g_param_spec_uint ("times-cleaned",
					"Times cleaned", "The number of times the feed has been completely cleared of entries.",
					0, G_MAXUINT, 0,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_calendar_feed_init (GDataCalendarFeed *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_CALENDAR_FEED, GDataCalendarFeedPrivate);
}

static void
gdata_calendar_feed_finalize (GObject *object)
{
	GDataCalendarFeedPrivate *priv = GDATA_CALENDAR_FEED_GET_PRIVATE (object);

	xmlFree (priv->timezone);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_calendar_feed_parent_class)->finalize (object);
}

static void
gdata_calendar_feed_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataCalendarFeedPrivate *priv = GDATA_CALENDAR_FEED_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_TIMEZONE:
			g_value_set_string (value, priv->timezone);
			break;
		case PROP_TIMES_CLEANED:
			g_value_set_uint (value, priv->times_cleaned);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static gboolean
parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error)
{
	GDataCalendarFeed *self = GDATA_CALENDAR_FEED (parsable);

	if (xmlStrcmp (node->name, (xmlChar*) "timezone") == 0) {
		/* gCal:timezone */
		xmlChar *_timezone = xmlGetProp (node, (xmlChar*) "value");
		if (_timezone == NULL)
			return gdata_parser_error_required_property_missing (node, "value", error);
		xmlFree (self->priv->timezone);
		self->priv->timezone = (gchar*) _timezone;
	} else if (xmlStrcmp (node->name, (xmlChar*) "timesCleaned") == 0) {
		/* gCal:timesCleaned */
		xmlChar *times_cleaned = xmlGetProp (node, (xmlChar*) "value");
		if (times_cleaned == NULL)
			return gdata_parser_error_required_property_missing (node, "value", error);
		self->priv->times_cleaned = strtoul ((gchar*) times_cleaned, NULL, 10);
		xmlFree (times_cleaned);
	} else if (GDATA_PARSABLE_CLASS (gdata_calendar_feed_parent_class)->parse_xml (parsable, doc, node, user_data, error) == FALSE) {
		/* Error! */
		return FALSE;
	}

	return TRUE;
}

/**
 * gdata_calendar_feed_get_timezone:
 * @self: a #GDataCalendarFeed
 *
 * Gets the #GDataCalendarFeed:timezone property.
 *
 * Return value: the feed's timezone, or %NULL
 *
 * Since: 0.3.0
 **/
const gchar *
gdata_calendar_feed_get_timezone (GDataCalendarFeed *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_FEED (self), NULL);
	return self->priv->timezone;
}

/**
 * gdata_calendar_feed_get_times_cleaned:
 * @self: a #GDataCalendarFeed
 *
 * Gets the #GDataCalendarFeed:times-cleaned property.
 *
 * Return value: the number of times the feed has been totally emptied
 *
 * Since: 0.3.0
 **/
guint
gdata_calendar_feed_get_times_cleaned (GDataCalendarFeed *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_FEED (self), 0);
	return self->priv->times_cleaned;
}
