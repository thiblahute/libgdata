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

#ifndef GDATA_CALENDAR_QUERY_H
#define GDATA_CALENDAR_QUERY_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-query.h>
#include <gdata/gdata-types.h>

G_BEGIN_DECLS

#define GDATA_TYPE_CALENDAR_QUERY		(gdata_calendar_query_get_type ())
#define GDATA_CALENDAR_QUERY(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_CALENDAR_QUERY, GDataCalendarQuery))
#define GDATA_CALENDAR_QUERY_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_CALENDAR_QUERY, GDataCalendarQueryClass))
#define GDATA_IS_CALENDAR_QUERY(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_CALENDAR_QUERY))
#define GDATA_IS_CALENDAR_QUERY_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_CALENDAR_QUERY))
#define GDATA_CALENDAR_QUERY_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_CALENDAR_QUERY, GDataCalendarQueryClass))

typedef struct _GDataCalendarQueryPrivate	GDataCalendarQueryPrivate;

typedef struct {
	GDataQuery parent;
	GDataCalendarQueryPrivate *priv;
} GDataCalendarQuery;

typedef struct {
	GDataQueryClass parent;
} GDataCalendarQueryClass;

GType gdata_calendar_query_get_type (void);

#include <gdata/services/calendar/gdata-calendar-service.h>

GDataCalendarQuery *gdata_calendar_query_new (GDataCalendarService *service, const gchar *q);
GDataCalendarQuery *gdata_calendar_query_new_with_limits (GDataCalendarService *service, const gchar *q, GTimeVal *start_min, GTimeVal *start_max);

gboolean gdata_calendar_query_get_future_events (GDataCalendarQuery *self);
void gdata_calendar_query_set_future_events (GDataCalendarQuery *self, gboolean future_events);
const gchar *gdata_calendar_query_get_order_by (GDataCalendarQuery *self);
void gdata_calendar_query_set_order_by (GDataCalendarQuery *self, const gchar *order_by);
void gdata_calendar_query_get_recurrence_expansion_start (GDataCalendarQuery *self, GTimeVal *start);
void gdata_calendar_query_set_recurrence_expansion_start (GDataCalendarQuery *self, GTimeVal *start);
void gdata_calendar_query_get_recurrence_expansion_end (GDataCalendarQuery *self, GTimeVal *end);
void gdata_calendar_query_set_recurrence_expansion_end (GDataCalendarQuery *self, GTimeVal *end);
gboolean gdata_calendar_query_get_single_events (GDataCalendarQuery *self);
void gdata_calendar_query_set_single_events (GDataCalendarQuery *self, gboolean single_events);
const gchar *gdata_calendar_query_get_sort_order (GDataCalendarQuery *self);
void gdata_calendar_query_set_sort_order (GDataCalendarQuery *self, const gchar *sort_order);
void gdata_calendar_query_get_start_min (GDataCalendarQuery *self, GTimeVal *start_min);
void gdata_calendar_query_set_start_min (GDataCalendarQuery *self, GTimeVal *start_min);
void gdata_calendar_query_get_start_max (GDataCalendarQuery *self, GTimeVal *start_max);
void gdata_calendar_query_set_start_max (GDataCalendarQuery *self, GTimeVal *start_max);
const gchar *gdata_calendar_query_get_ctz (GDataCalendarQuery *self);
void gdata_calendar_query_set_ctz (GDataCalendarQuery *self, const gchar *ctz);

G_END_DECLS

#endif /* !GDATA_CALENDAR_QUERY_H */