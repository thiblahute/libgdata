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

GDataQuery *gdata_query_new (const gchar *q);
GDataQuery *gdata_query_new_with_limits (const gchar *q, gint start_index, gint max_results);
GDataQuery *gdata_query_new_for_id (const gchar *entry_id);

gchar *gdata_query_get_query_uri (GDataQuery *self, const gchar *feed_uri);
void gdata_query_next_page (GDataQuery *self);
gboolean gdata_query_previous_page (GDataQuery *self);

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
