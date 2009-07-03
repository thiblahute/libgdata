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

#ifndef GDATA_GD_WHERE_H
#define GDATA_GD_WHERE_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-parsable.h>

G_BEGIN_DECLS

#define GDATA_TYPE_GD_WHERE		(gdata_gd_where_get_type ())
#define GDATA_GD_WHERE(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_GD_WHERE, GDataGDWhere))
#define GDATA_GD_WHERE_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_GD_WHERE, GDataGDWhereClass))
#define GDATA_IS_GD_WHERE(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_GD_WHERE))
#define GDATA_IS_GD_WHERE_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_GD_WHERE))
#define GDATA_GD_WHERE_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_GD_WHERE, GDataGDWhereClass))

typedef struct _GDataGDWherePrivate	GDataGDWherePrivate;

/**
 * GDataGDWhere:
 *
 * All the fields in the #GDataGDWhere structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataParsable parent;
	GDataGDWherePrivate *priv;
} GDataGDWhere;

/**
 * GDataGDWhereClass:
 *
 * All the fields in the #GDataGDWhereClass structure are private and should never be accessed directly.
 *
 * Since: 0.4.0
 **/
typedef struct {
	/*< private >*/
	GDataParsableClass parent;
} GDataGDWhereClass;

GType gdata_gd_where_get_type (void) G_GNUC_CONST;

GDataGDWhere *gdata_gd_where_new (const gchar *relation_type, const gchar *value_string, const gchar *label) G_GNUC_WARN_UNUSED_RESULT;
gint gdata_gd_where_compare (const GDataGDWhere *a, const GDataGDWhere *b);

const gchar *gdata_gd_where_get_relation_type (GDataGDWhere *self);
void gdata_gd_where_set_relation_type (GDataGDWhere *self, const gchar *relation_type);

const gchar *gdata_gd_where_get_value_string (GDataGDWhere *self);
void gdata_gd_where_set_value_string (GDataGDWhere *self, const gchar *value_string);

const gchar *gdata_gd_where_get_label (GDataGDWhere *self);
void gdata_gd_where_set_label (GDataGDWhere *self, const gchar *label);

G_END_DECLS

#endif /* !GDATA_GD_WHERE_H */
