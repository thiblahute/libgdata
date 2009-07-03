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

#ifndef GDATA_MEDIA_CATEGORY_H
#define GDATA_MEDIA_CATEGORY_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-parsable.h>

G_BEGIN_DECLS

#define GDATA_TYPE_MEDIA_CATEGORY		(gdata_media_category_get_type ())
#define GDATA_MEDIA_CATEGORY(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_MEDIA_CATEGORY, GDataMediaCategory))
#define GDATA_MEDIA_CATEGORY_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_MEDIA_CATEGORY, GDataMediaCategoryClass))
#define GDATA_IS_MEDIA_CATEGORY(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_MEDIA_CATEGORY))
#define GDATA_IS_MEDIA_CATEGORY_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_MEDIA_CATEGORY))
#define GDATA_MEDIA_CATEGORY_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_MEDIA_CATEGORY, GDataMediaCategoryClass))

typedef struct _GDataMediaCategoryPrivate	GDataMediaCategoryPrivate;

/**
 * GDataMediaCategory:
 *
 * All the fields in the #GDataMediaCategory structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataParsable parent;
	GDataMediaCategoryPrivate *priv;
} GDataMediaCategory;

/**
 * GDataMediaCategoryClass:
 *
 * All the fields in the #GDataMediaCategoryClass structure are private and should never be accessed directly.
 *
 * Since: 0.4.0
 **/
typedef struct {
	/*< private >*/
	GDataParsableClass parent;
} GDataMediaCategoryClass;

GType gdata_media_category_get_type (void) G_GNUC_CONST;

GDataMediaCategory *gdata_media_category_new (const gchar *category, const gchar *scheme, const gchar *label) G_GNUC_WARN_UNUSED_RESULT;

const gchar *gdata_media_category_get_category (GDataMediaCategory *self);
void gdata_media_category_set_category (GDataMediaCategory *self, const gchar *category);

const gchar *gdata_media_category_get_scheme (GDataMediaCategory *self);
void gdata_media_category_set_scheme (GDataMediaCategory *self, const gchar *scheme);

const gchar *gdata_media_category_get_label (GDataMediaCategory *self);
void gdata_media_category_set_label (GDataMediaCategory *self, const gchar *label);

G_END_DECLS

#endif /* !GDATA_MEDIA_CATEGORY_H */
