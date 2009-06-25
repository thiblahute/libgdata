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

#ifndef GDATA_GD_PHONE_NUMBER_H
#define GDATA_GD_PHONE_NUMBER_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-parsable.h>

G_BEGIN_DECLS

#define GDATA_TYPE_GD_PHONE_NUMBER		(gdata_gd_phone_number_get_type ())
#define GDATA_GD_PHONE_NUMBER(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_GD_PHONE_NUMBER, GDataGDPhoneNumber))
#define GDATA_GD_PHONE_NUMBER_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_GD_PHONE_NUMBER, GDataGDPhoneNumberClass))
#define GDATA_IS_GD_PHONE_NUMBER(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_GD_PHONE_NUMBER))
#define GDATA_IS_GD_PHONE_NUMBER_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_GD_PHONE_NUMBER))
#define GDATA_GD_PHONE_NUMBER_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_GD_PHONE_NUMBER, GDataGDPhoneNumberClass))

typedef struct _GDataGDPhoneNumberPrivate	GDataGDPhoneNumberPrivate;

/**
 * GDataGDPhoneNumber:
 *
 * All the fields in the #GDataGDPhoneNumber structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataParsable parent;
	GDataGDPhoneNumberPrivate *priv;
} GDataGDPhoneNumber;

/**
 * GDataGDPhoneNumberClass:
 *
 * All the fields in the #GDataGDPhoneNumberClass structure are private and should never be accessed directly.
 *
 * Since: 0.4.0
 **/
typedef struct {
	/*< private >*/
	GDataParsableClass parent;
} GDataGDPhoneNumberClass;

GType gdata_gd_phone_number_get_type (void) G_GNUC_CONST;

GDataGDPhoneNumber *gdata_gd_phone_number_new (const gchar *number, const gchar *relation_type, const gchar *label, const gchar *uri,
					       gboolean is_primary) G_GNUC_WARN_UNUSED_RESULT;
gint gdata_gd_phone_number_compare (const GDataGDPhoneNumber *a, const GDataGDPhoneNumber *b);

const gchar *gdata_gd_phone_number_get_number (GDataGDPhoneNumber *self);
void gdata_gd_phone_number_set_number (GDataGDPhoneNumber *self, const gchar *number);

const gchar *gdata_gd_phone_number_get_uri (GDataGDPhoneNumber *self);
void gdata_gd_phone_number_set_uri (GDataGDPhoneNumber *self, const gchar *uri);

const gchar *gdata_gd_phone_number_get_relation_type (GDataGDPhoneNumber *self);
void gdata_gd_phone_number_set_relation_type (GDataGDPhoneNumber *self, const gchar *relation_type);

const gchar *gdata_gd_phone_number_get_label (GDataGDPhoneNumber *self);
void gdata_gd_phone_number_set_label (GDataGDPhoneNumber *self, const gchar *label);

gboolean gdata_gd_phone_number_is_primary (GDataGDPhoneNumber *self);
void gdata_gd_phone_number_set_is_primary (GDataGDPhoneNumber *self, gboolean is_primary);

G_END_DECLS

#endif /* !GDATA_GD_PHONE_NUMBER_H */
