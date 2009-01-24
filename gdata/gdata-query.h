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

#ifndef GDATA_QUERY_H
#define GDATA_QUERY_H

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

typedef enum {
	GDATA_QUERY_PARAM_Q = 1 << 0,
	GDATA_QUERY_PARAM_CATEGORIES = 1 << 1,
	GDATA_QUERY_PARAM_AUTHOR = 1 << 2,
	GDATA_QUERY_PARAM_UPDATED_MIN = 1 << 3,
	GDATA_QUERY_PARAM_UPDATED_MAX = 1 << 4,
	GDATA_QUERY_PARAM_PUBLISHED_MIN = 1 << 5,
	GDATA_QUERY_PARAM_PUBLISHED_MAX = 1 << 6,
	GDATA_QUERY_PARAM_START_INDEX = 1 << 7,
	GDATA_QUERY_PARAM_STRICT = 1 << 8,
	GDATA_QUERY_PARAM_MAX_RESULTS = 1 << 9,
	GDATA_QUERY_PARAM_ENTRY_ID = 1 << 10,
	GDATA_QUERY_PARAM_ALL = (1 << 11) - 1
} GDataQueryParam;

#define GDATA_TYPE_QUERY		(gdata_query_get_type ())
#define GDATA_QUERY(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_QUERY, GDataQuery))
#define GDATA_QUERY_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_QUERY, GDataQueryClass))
#define GDATA_IS_QUERY(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_QUERY))
#define GDATA_IS_QUERY_CLASS(k)		(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_QUERY))
#define GDATA_QUERY_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_QUERY, GDataQueryClass))

typedef struct _GDataQueryPrivate	GDataQueryPrivate;

typedef struct {
	GObject parent;
	GDataQueryPrivate *priv;
} GDataQuery;

typedef struct {
	GObjectClass parent;
} GDataQueryClass;

GType gdata_query_get_type (void);

#include "gdata-service.h"

GDataQuery *gdata_query_new (GDataService *service, const gchar *q);
GDataQuery *gdata_query_new_for_id (GDataService *service, const gchar *entry_id);

gchar *gdata_query_get_query_uri (GDataQuery *self, const gchar *feed_uri);

GDataService *gdata_query_get_service (GDataQuery *self);

const gchar *gdata_query_get_q (GDataQuery *self);
void gdata_query_set_q (GDataQuery *self, const gchar *q);
const gchar *gdata_query_get_categories (GDataQuery *self);
void gdata_query_set_categories (GDataQuery *self, const gchar *categories);
const gchar *gdata_query_get_author (GDataQuery *self);
void gdata_query_set_author (GDataQuery *self, const gchar *author);
const gchar *gdata_query_get_updated_min (GDataQuery *self);
void gdata_query_set_updated_min (GDataQuery *self, const gchar *updated_min);
const gchar *gdata_query_get_updated_max (GDataQuery *self);
void gdata_query_set_updated_max (GDataQuery *self, const gchar *updated_max);
const gchar *gdata_query_get_published_min (GDataQuery *self);
void gdata_query_set_published_min (GDataQuery *self, const gchar *published_min);
const gchar *gdata_query_get_published_max (GDataQuery *self);
void gdata_query_set_published_max (GDataQuery *self, const gchar *published_max);
gint gdata_query_get_start_index (GDataQuery *self);
void gdata_query_set_start_index (GDataQuery *self, gint start_index);
gboolean gdata_query_get_strict (GDataQuery *self);
void gdata_query_set_strict (GDataQuery *self, gboolean strict);
gint gdata_query_get_max_results (GDataQuery *self);
void gdata_query_set_max_results (GDataQuery *self, gint max_results);
const gchar *gdata_query_get_entry_id (GDataQuery *self);
void gdata_query_set_entry_id (GDataQuery *self, const gchar *entry_id);

G_END_DECLS

#endif /* !GDATA_QUERY_H */
