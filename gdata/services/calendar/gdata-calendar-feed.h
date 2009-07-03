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

#ifndef GDATA_CALENDAR_FEED_H
#define GDATA_CALENDAR_FEED_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-parsable.h>
#include <gdata/gdata-feed.h>

G_BEGIN_DECLS

#define GDATA_TYPE_CALENDAR_FEED		(gdata_calendar_feed_get_type ())
#define GDATA_CALENDAR_FEED(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_CALENDAR_FEED, GDataCalendarFeed))
#define GDATA_CALENDAR_FEED_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_CALENDAR_FEED, GDataCalendarFeedClass))
#define GDATA_IS_CALENDAR_FEED(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_CALENDAR_FEED))
#define GDATA_IS_CALENDAR_FEED_CLASS(k)		(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_CALENDAR_FEED))
#define GDATA_CALENDAR_FEED_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_CALENDAR_FEED, GDataCalendarFeedClass))

typedef struct _GDataCalendarFeedPrivate	GDataCalendarFeedPrivate;

/**
 * GDataCalendarFeed:
 *
 * All the fields in the #GDataCalendarFeed structure are private and should never be accessed directly.
 *
 * Since: 0.3.0
 **/
typedef struct {
	GDataFeed parent;
	GDataCalendarFeedPrivate *priv;
} GDataCalendarFeed;

/**
 * GDataCalendarFeedClass:
 *
 * All the fields in the #GDataCalendarFeedClass structure are private and should never be accessed directly.
 *
 * Since: 0.3.0
 **/
typedef struct {
	/*< private >*/
	GDataFeedClass parent;
} GDataCalendarFeedClass;

GType gdata_calendar_feed_get_type (void);

const gchar *gdata_calendar_feed_get_timezone (GDataCalendarFeed *self);
guint gdata_calendar_feed_get_times_cleaned (GDataCalendarFeed *self);

G_END_DECLS

#endif /* !GDATA_CALENDAR_FEED_H */
