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
 * SECTION:gdata-calendar-query
 * @short_description: GData Calendar query object
 * @stability: Unstable
 * @include: gdata/services/calendar/gdata-calendar-query.h
 *
 * #GDataCalendarQuery represents a collection of query parameters specific to the Google Calendar service, which go above and beyond
 * those catered for by #GDataQuery.
 *
 * For more information on the custom GData query parameters supported by #GDataCalendarQuery, see the <ulink type="http"
 * url="http://code.google.com/apis/calendar/docs/2.0/reference.html#Parameters">online documentation</ulink>.
 **/

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <string.h>

#include "gdata-calendar-query.h"
#include "gdata-query.h"

static void gdata_calendar_query_finalize (GObject *object);
static void gdata_calendar_query_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_calendar_query_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void get_query_uri (GDataQuery *self, const gchar *feed_uri, GString *query_uri, gboolean *params_started);

struct _GDataCalendarQueryPrivate {
	gboolean future_events;
	gchar *order_by; /* TODO: enum? #defined values? */
	GTimeVal recurrence_expansion_start;
	GTimeVal recurrence_expansion_end;
	gboolean single_events;
	gchar *sort_order; /* TODO: enum? */
	GTimeVal start_min;
	GTimeVal start_max;
	gchar *timezone;
};

enum {
	PROP_FUTURE_EVENTS = 1,
	PROP_ORDER_BY,
	PROP_RECURRENCE_EXPANSION_START,
	PROP_RECURRENCE_EXPANSION_END,
	PROP_SINGLE_EVENTS,
	PROP_SORT_ORDER,
	PROP_START_MIN,
	PROP_START_MAX,
	PROP_TIMEZONE
};

G_DEFINE_TYPE (GDataCalendarQuery, gdata_calendar_query, GDATA_TYPE_QUERY)
#define GDATA_CALENDAR_QUERY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_CALENDAR_QUERY, GDataCalendarQueryPrivate))

static void
gdata_calendar_query_class_init (GDataCalendarQueryClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataQueryClass *query_class = GDATA_QUERY_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataCalendarQueryPrivate));

	gobject_class->set_property = gdata_calendar_query_set_property;
	gobject_class->get_property = gdata_calendar_query_get_property;
	gobject_class->finalize = gdata_calendar_query_finalize;

	query_class->get_query_uri = get_query_uri;

	/**
	 * GDataCalendarQuery:future-events:
	 *
	 * A shortcut to request all events that are scheduled for future times. Overrides the
	 * #GDataCalendarQuery:recurrence-expansion-start, #GDataCalendarQuery:recurrence-expansion-end,
	 * #GDataCalendarQuery:start-min and #GDataCalendarQuery:start-max properties.
	 **/
	g_object_class_install_property (gobject_class, PROP_FUTURE_EVENTS,
				g_param_spec_boolean ("future-events",
					"Future events?", "A shortcut to request all events that are scheduled for future times.",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataCalendarQuery:order-by:
	 *
	 * Specifies order of entries in a feed. Supported values are <literal>lastmodified</literal> and
	 * <literal>starttime</literal>.
	 **/
	g_object_class_install_property (gobject_class, PROP_ORDER_BY,
				g_param_spec_string ("order-by",
					"Order by", "Specifies order of entries in a feed.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataCalendarQuery:recurrence-expansion-start:
	 *
	 * Specifies beginning of time period for which to expand recurring events, inclusive.
	 **/
	g_object_class_install_property (gobject_class, PROP_RECURRENCE_EXPANSION_START,
				g_param_spec_boxed ("recurrence-expansion-start",
					"Recurrence expansion start", "Specifies beginning of time period for which to expand recurring events.",
					GDATA_TYPE_G_TIME_VAL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataCalendarQuery:recurrence-expansion-end:
	 *
	 * Specifies end of time period for which to expand recurring events, exclusive.
	 **/
	g_object_class_install_property (gobject_class, PROP_RECURRENCE_EXPANSION_END,
				g_param_spec_boxed ("recurrence-expansion-end",
					"Recurrence expansion end", "Specifies end of time period for which to expand recurring events.",
					GDATA_TYPE_G_TIME_VAL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataCalendarQuery:single-events:
	 *
	 * Indicates whether recurring events should be expanded or represented as a single event.
	 **/
	g_object_class_install_property (gobject_class, PROP_SINGLE_EVENTS,
				g_param_spec_boolean ("single-events",
					"Single events?", "Indicates whether recurring events should be expanded or represented as a single event.",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataCalendarQuery:sort-order:
	 *
	 * Specifies direction of sorting. Supported values are <literal>ascending</literal> and
	 * <literal>descending</literal>.
	 **/
	g_object_class_install_property (gobject_class, PROP_SORT_ORDER,
				g_param_spec_string ("sort-order",
					"Sort order", "Specifies direction of sorting.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataCalendarQuery:start-min:
	 *
	 * Together with #GDataCalendarQuery:start-max creates a timespan such that only events that are within the timespan are returned.
	 *
	 * #GDataCalendarQuery:start-min is inclusive, while #GDataCalendarQuery:start-max is exclusive. Events that overlap the range are
	 * included.
	 *
	 * If not specified, the default #GDataCalendarQuery:start-min is <literal>1970-01-01</literal>.
	 **/
	g_object_class_install_property (gobject_class, PROP_START_MIN,
				g_param_spec_boxed ("start-min",
					"Start min", "A timespan such that only events that are within the timespan are returned.",
					GDATA_TYPE_G_TIME_VAL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataCalendarQuery:start-max:
	 *
	 * Together with #GDataCalendarQuery:start-min creates a timespan such that only events that are within the timespan are returned
	 *
	 * #GDataCalendarQuery:start-min is inclusive, while #GDataCalendarQuery:start-max is exclusive. Events that overlap the range are
	 * included.
	 *
	 * If not specified, the default #GDataCalendarQuery:start-max is <literal>2031-01-01</literal>.
	 **/
	g_object_class_install_property (gobject_class, PROP_START_MAX,
				g_param_spec_boxed ("start-max",
					"Start max", "A timespan such that only events that are within the timespan are returned.",
					GDATA_TYPE_G_TIME_VAL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataCalendarQuery:timezone:
	 *
	 * The current timezone. If not specified, times are returned in UTC.
	 *
	 * Since: 0.2.0
	 **/
	g_object_class_install_property (gobject_class, PROP_TIMEZONE,
				g_param_spec_string ("timezone",
					"Timezone", "The current timezone. If not specified, times are returned in UTC.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_calendar_query_init (GDataCalendarQuery *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_CALENDAR_QUERY, GDataCalendarQueryPrivate);
}

static void
gdata_calendar_query_finalize (GObject *object)
{
	GDataCalendarQueryPrivate *priv = GDATA_CALENDAR_QUERY_GET_PRIVATE (object);

	g_free (priv->order_by);
	g_free (priv->sort_order);
	g_free (priv->timezone);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_calendar_query_parent_class)->finalize (object);
}

static void
gdata_calendar_query_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataCalendarQueryPrivate *priv = GDATA_CALENDAR_QUERY_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_FUTURE_EVENTS:
			g_value_set_boolean (value, priv->future_events);
			break;
		case PROP_ORDER_BY:
			g_value_set_string (value, priv->order_by);
			break;
		case PROP_RECURRENCE_EXPANSION_START:
			g_value_set_boxed (value, &(priv->recurrence_expansion_start));
			break;
		case PROP_RECURRENCE_EXPANSION_END:
			g_value_set_boxed (value, &(priv->recurrence_expansion_end));
			break;
		case PROP_SINGLE_EVENTS:
			g_value_set_boolean (value, priv->single_events);
			break;
		case PROP_SORT_ORDER:
			g_value_set_string (value, priv->sort_order);
			break;
		case PROP_START_MIN:
			g_value_set_boxed (value, &(priv->start_min));
			break;
		case PROP_START_MAX:
			g_value_set_boxed (value, &(priv->start_max));
			break;
		case PROP_TIMEZONE:
			g_value_set_string (value, priv->timezone);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gdata_calendar_query_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataCalendarQuery *self = GDATA_CALENDAR_QUERY (object);

	switch (property_id) {
		case PROP_FUTURE_EVENTS:
			gdata_calendar_query_set_future_events (self, g_value_get_boolean (value));
			break;
		case PROP_ORDER_BY:
			gdata_calendar_query_set_order_by (self, g_value_get_string (value));
			break;
		case PROP_RECURRENCE_EXPANSION_START:
			gdata_calendar_query_set_recurrence_expansion_start (self, g_value_get_boxed (value));
			break;
		case PROP_RECURRENCE_EXPANSION_END:
			gdata_calendar_query_set_recurrence_expansion_end (self, g_value_get_boxed (value));
			break;
		case PROP_SINGLE_EVENTS:
			gdata_calendar_query_set_single_events (self, g_value_get_boolean (value));
			break;
		case PROP_SORT_ORDER:
			gdata_calendar_query_set_sort_order (self, g_value_get_string (value));
			break;
		case PROP_START_MIN:
			gdata_calendar_query_set_start_min (self, g_value_get_boxed (value));
			break;
		case PROP_START_MAX:
			gdata_calendar_query_set_start_max (self, g_value_get_boxed (value));
			break;
		case PROP_TIMEZONE:
			gdata_calendar_query_set_timezone (self, g_value_get_string (value));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
get_query_uri (GDataQuery *self, const gchar *feed_uri, GString *query_uri, gboolean *params_started)
{
	GDataCalendarQueryPrivate *priv = GDATA_CALENDAR_QUERY (self)->priv;

	#define APPEND_SEP g_string_append_c (query_uri, (*params_started == FALSE) ? '?' : '&'); *params_started = TRUE;

	/* Chain up to the parent class */
	GDATA_QUERY_CLASS (gdata_calendar_query_parent_class)->get_query_uri (self, feed_uri, query_uri, params_started);

	APPEND_SEP
	if (priv->future_events == TRUE)
		g_string_append (query_uri, "futureevents=true");
	else
		g_string_append (query_uri, "futureevents=false");

	if (priv->order_by != NULL) {
		APPEND_SEP
		g_string_append (query_uri, "orderby=");
		g_string_append_uri_escaped (query_uri, priv->order_by, NULL, TRUE);
	}

	if (priv->recurrence_expansion_start.tv_sec != 0 || priv->recurrence_expansion_start.tv_usec != 0) {
		gchar *recurrence_expansion_start;

		APPEND_SEP
		g_string_append (query_uri, "recurrence-expansion-start=");
		recurrence_expansion_start = g_time_val_to_iso8601 (&(priv->recurrence_expansion_start));
		g_string_append (query_uri, recurrence_expansion_start);
		g_free (recurrence_expansion_start);
	}

	if (priv->recurrence_expansion_end.tv_sec != 0 || priv->recurrence_expansion_end.tv_usec != 0) {
		gchar *recurrence_expansion_end;

		APPEND_SEP
		g_string_append (query_uri, "recurrence-expansion-end=");
		recurrence_expansion_end = g_time_val_to_iso8601 (&(priv->recurrence_expansion_end));
		g_string_append (query_uri, recurrence_expansion_end);
		g_free (recurrence_expansion_end);
	}

	APPEND_SEP
	if (priv->single_events == TRUE)
		g_string_append (query_uri, "singleevents=true");
	else
		g_string_append (query_uri, "singleevents=false");

	if (priv->sort_order != NULL) {
		APPEND_SEP
		g_string_append (query_uri, "sortorder=");
		g_string_append_uri_escaped (query_uri, priv->sort_order, NULL, TRUE);
	}

	if (priv->start_min.tv_sec != 0 || priv->start_min.tv_usec != 0) {
		gchar *start_min;

		APPEND_SEP
		g_string_append (query_uri, "start-min=");
		start_min = g_time_val_to_iso8601 (&(priv->start_min));
		g_string_append (query_uri, start_min);
		g_free (start_min);
	}

	if (priv->start_max.tv_sec != 0 || priv->start_max.tv_usec != 0) {
		gchar *start_max;

		APPEND_SEP
		g_string_append (query_uri, "start-max=");
		start_max = g_time_val_to_iso8601 (&(priv->start_max));
		g_string_append (query_uri, start_max);
		g_free (start_max);
	}

	if (priv->timezone != NULL) {
		APPEND_SEP
		g_string_append (query_uri, "ctz=");
		g_string_append_uri_escaped (query_uri, priv->timezone, NULL, TRUE);
	}
}

/**
 * gdata_calendar_query_new:
 * @q: a query string
 *
 * Creates a new #GDataCalendarQuery with its #GDataQuery:q property set to @q.
 *
 * Return value: a new #GDataCalendarQuery
 **/
GDataCalendarQuery *
gdata_calendar_query_new (const gchar *q)
{
	return g_object_new (GDATA_TYPE_CALENDAR_QUERY, "q", q, NULL);
}

/**
 * gdata_calendar_query_new_with_limits:
 * @q: a query string
 * @start_min: a starting time for the event period
 * @start_max: an ending time for the event period
 *
 * Creates a new #GDataCalendarQuery with its #GDataQuery:q property set to @q, and the time limits @start_min and @start_max
 * applied.
 *
 * Return value: a new #GDataCalendarQuery
 **/
GDataCalendarQuery *
gdata_calendar_query_new_with_limits (const gchar *q, GTimeVal *start_min, GTimeVal *start_max)
{
	return g_object_new (GDATA_TYPE_CALENDAR_QUERY,
			     "q", q,
			     "start-min", start_min,
			     "start-max", start_max,
			     NULL);
}

/**
 * gdata_calendar_query_get_future_events:
 * @self: a #GDataCalendarQuery
 *
 * Gets the #GDataCalendarQuery:future-events property.
 *
 * Return value: the future events property
 **/
gboolean
gdata_calendar_query_get_future_events (GDataCalendarQuery *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_QUERY (self), FALSE);
	return self->priv->future_events;
}

/**
 * gdata_calendar_query_set_future_events:
 * @self: a #GDataCalendarQuery
 * @future_events: %TRUE to unconditionally show future events, %FALSE otherwise
 *
 * Sets the #GDataCalendarQuery:future-events property of the #GDataCalendarQuery to @future_events.
 **/
void
gdata_calendar_query_set_future_events (GDataCalendarQuery *self, gboolean future_events)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));
	self->priv->future_events = future_events;
	g_object_notify (G_OBJECT (self), "future-events");
}

/**
 * gdata_calendar_query_get_order_by:
 * @self: a #GDataCalendarQuery
 *
 * Gets the #GDataCalendarQuery:order-by property.
 *
 * Return value: the order by property, or %NULL if it is unset
 **/
const gchar *
gdata_calendar_query_get_order_by (GDataCalendarQuery *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_QUERY (self), NULL);
	return self->priv->order_by;
}

/**
 * gdata_calendar_query_set_order_by:
 * @self: a #GDataCalendarQuery
 * @order_by: a new order by string, or %NULL
 *
 * Sets the #GDataCalendarQuery:order-by property of the #GDataCalendarQuery to the new order by string, @order_by.
 *
 * Set @order_by to %NULL to unset the property in the query URI.
 **/
void
gdata_calendar_query_set_order_by (GDataCalendarQuery *self, const gchar *order_by)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));

	g_free (self->priv->order_by);
	self->priv->order_by = g_strdup (order_by);
	g_object_notify (G_OBJECT (self), "order-by");
}

/**
 * gdata_calendar_query_get_recurrence_expansion_start:
 * @self: a #GDataCalendarQuery
 * @start: a #GTimeVal
 *
 * Gets the #GDataCalendarQuery:recurrence-expansion-start property and puts it
 * in @start. If the property is unset, both fields in the #GTimeVal will be set to %0.
 **/
void
gdata_calendar_query_get_recurrence_expansion_start (GDataCalendarQuery *self, GTimeVal *start)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));
	g_return_if_fail (start != NULL);
	*start = self->priv->recurrence_expansion_start;
}

/**
 * gdata_calendar_query_set_recurrence_expansion_start:
 * @self: a #GDataCalendarQuery
 * @start: a new start time
 *
 * Sets the #GDataCalendarQuery:recurrence-expansion-start property of the #GDataCalendarQuery
 * to the new time/date, @start.
 *
 * Set @start to %NULL to unset the property in the query URI.
 **/
void
gdata_calendar_query_set_recurrence_expansion_start (GDataCalendarQuery *self, GTimeVal *start)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));

	if (start == NULL) {
		self->priv->recurrence_expansion_start.tv_sec = 0;
		self->priv->recurrence_expansion_start.tv_usec = 0;
	} else {
		self->priv->recurrence_expansion_start = *start;
	}

	g_object_notify (G_OBJECT (self), "recurrence-expansion-start");
}

/**
 * gdata_calendar_query_get_recurrence_expansion_end:
 * @self: a #GDataCalendarQuery
 * @end: a #GTimeVal
 *
 * Gets the #GDataCalendarQuery:recurrence-expansion-end property and puts it
 * in @end. If the property is unset, both fields in the #GTimeVal will be set to %0.
 **/
void
gdata_calendar_query_get_recurrence_expansion_end (GDataCalendarQuery *self, GTimeVal *end)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));
	g_return_if_fail (end != NULL);
	*end = self->priv->recurrence_expansion_end;
}

/**
 * gdata_calendar_query_set_recurrence_expansion_end:
 * @self: a #GDataCalendarQuery
 * @end: a new end time
 *
 * Sets the #GDataCalendarQuery:recurrence-expansion-end property of the #GDataCalendarQuery
 * to the new time/date, @end.
 *
 * Set @end to %NULL to unset the property in the query URI.
 **/
void
gdata_calendar_query_set_recurrence_expansion_end (GDataCalendarQuery *self, GTimeVal *end)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));

	if (end == NULL) {
		self->priv->recurrence_expansion_end.tv_sec = 0;
		self->priv->recurrence_expansion_end.tv_usec = 0;
	} else {
		self->priv->recurrence_expansion_end = *end;
	}

	g_object_notify (G_OBJECT (self), "recurrence-expansion-end");
}

/**
 * gdata_calendar_query_get_single_events:
 * @self: a #GDataCalendarQuery
 *
 * Gets the #GDataCalendarQuery:single-events property.
 *
 * Return value: the single events property
 **/
gboolean
gdata_calendar_query_get_single_events (GDataCalendarQuery *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_QUERY (self), FALSE);
	return self->priv->single_events;
}

/**
 * gdata_calendar_query_set_single_events:
 * @self: a #GDataCalendarQuery
 * @single_events: %TRUE to show recurring events as single events, %FALSE otherwise
 *
 * Sets the #GDataCalendarQuery:single-events property of the #GDataCalendarQuery to @single_events.
 **/
void
gdata_calendar_query_set_single_events (GDataCalendarQuery *self, gboolean single_events)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));
	self->priv->single_events = single_events;
	g_object_notify (G_OBJECT (self), "single-events");
}

/**
 * gdata_calendar_query_get_sort_order:
 * @self: a #GDataCalendarQuery
 *
 * Gets the #GDataCalendarQuery:sort-order property.
 *
 * Return value: the sort order property, or %NULL if it is unset
 **/
const gchar *
gdata_calendar_query_get_sort_order (GDataCalendarQuery *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_QUERY (self), NULL);
	return self->priv->sort_order;
}

/**
 * gdata_calendar_query_set_sort_order:
 * @self: a #GDataCalendarQuery
 * @sort_order: a new sort order string, or %NULL
 *
 * Sets the #GDataCalendarQuery:sort-order property of the #GDataCalendarQuery to the new sort order string, @sort_order.
 *
 * Set @sort_order to %NULL to unset the property in the query URI.
 **/
void
gdata_calendar_query_set_sort_order (GDataCalendarQuery *self, const gchar *sort_order)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));

	g_free (self->priv->sort_order);
	self->priv->sort_order = g_strdup (sort_order);
	g_object_notify (G_OBJECT (self), "sort-order");
}

/**
 * gdata_calendar_query_get_start_min:
 * @self: a #GDataCalendarQuery
 * @start_min: a #GTimeVal
 *
 * Gets the #GDataCalendarQuery:start-min property and puts it
 * in @start_min. If the property is unset, both fields in the #GTimeVal will be set to %0.
 **/
void
gdata_calendar_query_get_start_min (GDataCalendarQuery *self, GTimeVal *start_min)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));
	g_return_if_fail (start_min != NULL);
	*start_min = self->priv->start_min;
}

/**
 * gdata_calendar_query_set_start_min:
 * @self: a #GDataCalendarQuery
 * @start_min: a new minimum start time
 *
 * Sets the #GDataCalendarQuery:start-min property of the #GDataCalendarQuery
 * to the new time/date, @start_min.
 *
 * Set @start_min to %NULL to unset the property in the query URI.
 **/
void
gdata_calendar_query_set_start_min (GDataCalendarQuery *self, GTimeVal *start_min)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));

	if (start_min == NULL) {
		self->priv->start_min.tv_sec = 0;
		self->priv->start_min.tv_usec = 0;
	} else {
		self->priv->start_min = *start_min;
	}

	g_object_notify (G_OBJECT (self), "start-min");
}

/**
 * gdata_calendar_query_get_start_max:
 * @self: a #GDataCalendarQuery
 * @start_max: a #GTimeVal
 *
 * Gets the #GDataCalendarQuery:start-max property and puts it
 * in @start_max. If the property is unset, both fields in the #GTimeVal will be set to %0.
 **/
void
gdata_calendar_query_get_start_max (GDataCalendarQuery *self, GTimeVal *start_max)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));
	g_return_if_fail (start_max != NULL);
	*start_max = self->priv->start_max;
}

/**
 * gdata_calendar_query_set_start_max:
 * @self: a #GDataCalendarQuery
 * @start_max: a new maximum start time
 *
 * Sets the #GDataCalendarQuery:start-max property of the #GDataCalendarQuery
 * to the new time/date, @start_max.
 *
 * Set @start_max to %NULL to unset the property in the query URI.
 **/
void
gdata_calendar_query_set_start_max (GDataCalendarQuery *self, GTimeVal *start_max)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));

	if (start_max == NULL) {
		self->priv->start_max.tv_sec = 0;
		self->priv->start_max.tv_usec = 0;
	} else {
		self->priv->start_max = *start_max;
	}

	g_object_notify (G_OBJECT (self), "start-max");
}

/**
 * gdata_calendar_query_get_timezone:
 * @self: a #GDataCalendarQuery
 *
 * Gets the #GDataCalendarQuery:timezone property.
 *
 * Return value: the timezone property, or %NULL if it is unset
 *
 * Since: 0.2.0
 **/
const gchar *
gdata_calendar_query_get_timezone (GDataCalendarQuery *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_QUERY (self), NULL);
	return self->priv->timezone;
}

/**
 * gdata_calendar_query_set_timezone:
 * @self: a #GDataCalendarQuery
 * @_timezone: a new timezone string, or %NULL
 *
 * Sets the #GDataCalendarQuery:timezone property of the #GDataCalendarQuery to the new timezone string, @timezone.
 *
 * Set @timezone to %NULL to unset the property in the query URI.
 *
 * Since: 0.2.0
 **/
void
gdata_calendar_query_set_timezone (GDataCalendarQuery *self, const gchar *_timezone)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));

	g_free (self->priv->timezone);

	/* Replace all spaces with underscores */
	if (_timezone != NULL) {
		gchar *zone, *i;

		zone = g_strdup (_timezone);
		for (i = zone; *i != '\0'; i++) {
			if (*i == ' ')
				*i = '_';
		}
		self->priv->timezone = zone;
	} else {
		self->priv->timezone = NULL;
	}

	g_object_notify (G_OBJECT (self), "timezone");
}
