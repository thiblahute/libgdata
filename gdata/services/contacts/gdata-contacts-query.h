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

#ifndef GDATA_CONTACTS_QUERY_H
#define GDATA_CONTACTS_QUERY_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-query.h>

G_BEGIN_DECLS

#define GDATA_TYPE_CONTACTS_QUERY		(gdata_contacts_query_get_type ())
#define GDATA_CONTACTS_QUERY(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_CONTACTS_QUERY, GDataContactsQuery))
#define GDATA_CONTACTS_QUERY_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_CONTACTS_QUERY, GDataContactsQueryClass))
#define GDATA_IS_CONTACTS_QUERY(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_CONTACTS_QUERY))
#define GDATA_IS_CONTACTS_QUERY_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_CONTACTS_QUERY))
#define GDATA_CONTACTS_QUERY_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_CONTACTS_QUERY, GDataContactsQueryClass))

typedef struct _GDataContactsQueryPrivate	GDataContactsQueryPrivate;

/**
 * GDataContactsQuery:
 *
 * All the fields in the #GDataContactsQuery structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataQuery parent;
	GDataContactsQueryPrivate *priv;
} GDataContactsQuery;

/**
 * GDataContactsQueryClass:
 *
 * All the fields in the #GDataContactsQueryClass structure are private and should never be accessed directly.
 **/
typedef struct {
	/*< private >*/
	GDataQueryClass parent;
} GDataContactsQueryClass;

GType gdata_contacts_query_get_type (void) G_GNUC_CONST;

GDataContactsQuery *gdata_contacts_query_new (const gchar *q) G_GNUC_WARN_UNUSED_RESULT;
GDataContactsQuery *gdata_contacts_query_new_with_limits (const gchar *q, gint start_index, gint max_results) G_GNUC_WARN_UNUSED_RESULT;

const gchar *gdata_contacts_query_get_order_by (GDataContactsQuery *self);
void gdata_contacts_query_set_order_by (GDataContactsQuery *self, const gchar *order_by);
gboolean gdata_contacts_query_show_deleted (GDataContactsQuery *self);
void gdata_contacts_query_set_show_deleted (GDataContactsQuery *self, gboolean show_deleted);
const gchar *gdata_contacts_query_get_sort_order (GDataContactsQuery *self);
void gdata_contacts_query_set_sort_order (GDataContactsQuery *self, const gchar *sort_order);
const gchar *gdata_contacts_query_get_group (GDataContactsQuery *self);
void gdata_contacts_query_set_group (GDataContactsQuery *self, const gchar *group);

G_END_DECLS

#endif /* !GDATA_CONTACTS_QUERY_H */
