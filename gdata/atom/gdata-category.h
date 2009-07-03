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

#ifndef GDATA_CATEGORY_H
#define GDATA_CATEGORY_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-parsable.h>

G_BEGIN_DECLS

#define GDATA_TYPE_CATEGORY		(gdata_category_get_type ())
#define GDATA_CATEGORY(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_CATEGORY, GDataCategory))
#define GDATA_CATEGORY_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_CATEGORY, GDataCategoryClass))
#define GDATA_IS_CATEGORY(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_CATEGORY))
#define GDATA_IS_CATEGORY_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_CATEGORY))
#define GDATA_CATEGORY_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_CATEGORY, GDataCategoryClass))

typedef struct _GDataCategoryPrivate	GDataCategoryPrivate;

/**
 * GDataCategory:
 *
 * All the fields in the #GDataCategory structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataParsable parent;
	GDataCategoryPrivate *priv;
} GDataCategory;

/**
 * GDataCategoryClass:
 *
 * All the fields in the #GDataCategoryClass structure are private and should never be accessed directly.
 *
 * Since: 0.4.0
 **/
typedef struct {
	/*< private >*/
	GDataParsableClass parent;
} GDataCategoryClass;

GType gdata_category_get_type (void) G_GNUC_CONST;

GDataCategory *gdata_category_new (const gchar *term, const gchar *scheme, const gchar *label) G_GNUC_WARN_UNUSED_RESULT;
gint gdata_category_compare (const GDataCategory *a, const GDataCategory *b);

const gchar *gdata_category_get_term (GDataCategory *self);
void gdata_category_set_term (GDataCategory *self, const gchar *term);

const gchar *gdata_category_get_scheme (GDataCategory *self);
void gdata_category_set_scheme (GDataCategory *self, const gchar *scheme);

const gchar *gdata_category_get_label (GDataCategory *self);
void gdata_category_set_label (GDataCategory *self, const gchar *label);

G_END_DECLS

#endif /* !GDATA_CATEGORY_H */
