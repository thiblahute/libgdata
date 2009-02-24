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
#include <string.h>

#include "gdata-calendar-query.h"
#include "gdata-query.h"

/* Reference: http://code.google.com/apis/calendar/docs/2.0/reference.html#Parameters */

static void gdata_calendar_query_finalize (GObject *object);
static void gdata_calendar_query_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_calendar_query_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

struct _GDataCalendarQueryPrivate {
	gboolean future_events;
	gchar *order_by; /* TODO: enum? #defined values? */
	gboolean recurrence_expansion_start_set;
	GTimeVal recurrence_expansion_start;
	gboolean recurrence_expansion_end_set;
	GTimeVal recurrence_expansion_end;
	gboolean single_events;
	gchar *sort_order; /* TODO: enum? */
	gboolean start_min_set;
	GTimeVal start_min;
	gboolean start_max_set;
	GTimeVal start_max;
	gchar *ctz;
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
	PROP_CTZ
};

G_DEFINE_TYPE (GDataCalendarQuery, gdata_calendar_query, GDATA_TYPE_QUERY)
#define GDATA_CALENDAR_QUERY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_CALENDAR_QUERY, GDataCalendarQueryPrivate))

static void
gdata_calendar_query_class_init (GDataCalendarQueryClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataCalendarQueryPrivate));

	gobject_class->set_property = gdata_calendar_query_set_property;
	gobject_class->get_property = gdata_calendar_query_get_property;
	gobject_class->finalize = gdata_calendar_query_finalize;

	g_object_class_install_property (gobject_class, PROP_FUTURE_EVENTS,
				g_param_spec_boolean ("future-events",
					"Future events", "TODO",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_ORDER_BY,
				g_param_spec_string ("order-by",
					"Order by", "TODO",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_RECURRENCE_EXPANSION_START,
				g_param_spec_boxed ("recurrence-expansion-start",
					"Recurrence expansion start", "TODO",
					G_TYPE_TIME_VAL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_RECURRENCE_EXPANSION_END,
				g_param_spec_boxed ("recurrence-expansion-end",
					"Recurrence expansion end", "TODO",
					G_TYPE_TIME_VAL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_SINGLE_EVENTS,
				g_param_spec_boolean ("single-events",
					"Single events", "TODO",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_SORT_ORDER,
				g_param_spec_string ("sort-order",
					"Sort order", "TODO",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_START_MIN,
				g_param_spec_boxed ("start-min",
					"Start min", "TODO",
					G_TYPE_TIME_VAL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_START_MAX,
				g_param_spec_boxed ("start-max",
					"Start max", "TODO",
					G_TYPE_TIME_VAL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_CTZ,
				g_param_spec_string ("ctz",
					"CTZ", "TODO",
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
	g_free (priv->ctz);

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
		case PROP_CTZ:
			g_value_set_string (value, priv->ctz);
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
		case PROP_CTZ:
			gdata_calendar_query_set_ctz (self, g_value_get_string (value));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

GDataCalendarQuery *
gdata_calendar_query_new (GDataCalendarService *service, const gchar *q)
{
	return g_object_new (GDATA_TYPE_CALENDAR_QUERY,
			     "service", service,
			     "q", q,
			     NULL);
}

GDataCalendarQuery *
gdata_calendar_query_new_with_limits (GDataCalendarService *service, const gchar *q, GTimeVal *start_min, GTimeVal *start_max)
{
	return g_object_new (GDATA_TYPE_CALENDAR_QUERY,
			     "service", service,
			     "q", q,
			     "start-min", start_min,
			     "start-max", start_max,
			     NULL);
}

gboolean
gdata_calendar_query_get_future_events (GDataCalendarQuery *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_QUERY (self), FALSE);
	return self->priv->future_events;
}

void
gdata_calendar_query_set_future_events (GDataCalendarQuery *self, gboolean future_events)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));
	self->priv->future_events = future_events;
	g_object_notify (G_OBJECT (self), "future-events");
}

const gchar *
gdata_calendar_query_get_order_by (GDataCalendarQuery *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_QUERY (self), NULL);
	return self->priv->order_by;
}

void
gdata_calendar_query_set_order_by (GDataCalendarQuery *self, const gchar *order_by)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));

	g_free (self->priv->order_by);
	self->priv->order_by = g_strdup (order_by);
	g_object_notify (G_OBJECT (self), "order-by");
}

void
gdata_calendar_query_get_recurrence_expansion_start (GDataCalendarQuery *self, GTimeVal *start)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));
	g_return_if_fail (start != NULL);

	start->tv_sec = self->priv->recurrence_expansion_start.tv_sec;
	start->tv_usec = self->priv->recurrence_expansion_start.tv_usec;
}

void
gdata_calendar_query_set_recurrence_expansion_start (GDataCalendarQuery *self, GTimeVal *start)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));
	g_return_if_fail (start != NULL);

	self->priv->recurrence_expansion_start.tv_sec = start->tv_sec;
	self->priv->recurrence_expansion_start.tv_usec = start->tv_usec;
	g_object_notify (G_OBJECT (self), "recurrence-expansion-start");
}

void
gdata_calendar_query_get_recurrence_expansion_end (GDataCalendarQuery *self, GTimeVal *end)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));
	g_return_if_fail (end != NULL);

	end->tv_sec = self->priv->recurrence_expansion_start.tv_sec;
	end->tv_usec = self->priv->recurrence_expansion_start.tv_usec;
}

void
gdata_calendar_query_set_recurrence_expansion_end (GDataCalendarQuery *self, GTimeVal *end)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));
	g_return_if_fail (end != NULL);

	self->priv->recurrence_expansion_start.tv_sec = end->tv_sec;
	self->priv->recurrence_expansion_start.tv_usec = end->tv_usec;
	g_object_notify (G_OBJECT (self), "recurrence-expansion-end");
}

gboolean
gdata_calendar_query_get_single_events (GDataCalendarQuery *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_QUERY (self), FALSE);
	return self->priv->single_events;
}

void
gdata_calendar_query_set_single_events (GDataCalendarQuery *self, gboolean single_events)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));
	self->priv->single_events = single_events;
	g_object_notify (G_OBJECT (self), "single-events");
}

const gchar *
gdata_calendar_query_get_sort_order (GDataCalendarQuery *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_QUERY (self), NULL);
	return self->priv->sort_order;
}

void
gdata_calendar_query_set_sort_order (GDataCalendarQuery *self, const gchar *sort_order)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));

	g_free (self->priv->sort_order);
	self->priv->sort_order = g_strdup (sort_order);
	g_object_notify (G_OBJECT (self), "sort-order");
}

void
gdata_calendar_query_get_start_min (GDataCalendarQuery *self, GTimeVal *start_min)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));
	g_return_if_fail (start_min != NULL);

	start_min->tv_sec = self->priv->start_min.tv_sec;
	start_min->tv_usec = self->priv->start_min.tv_usec;
}

void
gdata_calendar_query_set_start_min (GDataCalendarQuery *self, GTimeVal *start_min)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));

	if (start_min == NULL) {
		self->priv->start_min_set = FALSE; /* TODO: got to be a better way than this */
		return;
	}
	self->priv->start_min_set = TRUE;

	self->priv->start_min.tv_sec = start_min->tv_sec;
	self->priv->start_min.tv_usec = start_min->tv_usec;
	g_object_notify (G_OBJECT (self), "start-min");
}

void
gdata_calendar_query_get_start_max (GDataCalendarQuery *self, GTimeVal *start_max)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));
	g_return_if_fail (start_max != NULL);

	start_max->tv_sec = self->priv->start_max.tv_sec;
	start_max->tv_usec = self->priv->start_max.tv_usec;
}

void
gdata_calendar_query_set_start_max (GDataCalendarQuery *self, GTimeVal *start_max)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));

	if (start_max == NULL) {
		self->priv->start_max_set = FALSE; /* TODO: got to be a better way than this */
		return;
	}
	self->priv->start_max_set = TRUE;

	self->priv->start_max.tv_sec = start_max->tv_sec;
	self->priv->start_max.tv_usec = start_max->tv_usec;
	g_object_notify (G_OBJECT (self), "start-max");
}

const gchar *
gdata_calendar_query_get_ctz (GDataCalendarQuery *self)
{
	g_return_val_if_fail (GDATA_IS_CALENDAR_QUERY (self), NULL);
	return self->priv->ctz;
}

void
gdata_calendar_query_set_ctz (GDataCalendarQuery *self, const gchar *ctz)
{
	g_return_if_fail (GDATA_IS_CALENDAR_QUERY (self));

	g_free (self->priv->ctz);
	self->priv->ctz = g_strdup (ctz);
	g_object_notify (G_OBJECT (self), "ctz");
}
