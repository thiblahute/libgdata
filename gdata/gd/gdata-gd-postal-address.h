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

#ifndef GDATA_GD_POSTAL_ADDRESS_H
#define GDATA_GD_POSTAL_ADDRESS_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-parsable.h>

G_BEGIN_DECLS

#define GDATA_TYPE_GD_POSTAL_ADDRESS		(gdata_gd_postal_address_get_type ())
#define GDATA_GD_POSTAL_ADDRESS(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_GD_POSTAL_ADDRESS, GDataGDPostalAddress))
#define GDATA_GD_POSTAL_ADDRESS_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_GD_POSTAL_ADDRESS, GDataGDPostalAddressClass))
#define GDATA_IS_GD_POSTAL_ADDRESS(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_GD_POSTAL_ADDRESS))
#define GDATA_IS_GD_POSTAL_ADDRESS_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_GD_POSTAL_ADDRESS))
#define GDATA_GD_POSTAL_ADDRESS_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_GD_POSTAL_ADDRESS, GDataGDPostalAddressClass))

typedef struct _GDataGDPostalAddressPrivate	GDataGDPostalAddressPrivate;

/**
 * GDataGDPostalAddress:
 *
 * All the fields in the #GDataGDPostalAddress structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataParsable parent;
	GDataGDPostalAddressPrivate *priv;
} GDataGDPostalAddress;

/**
 * GDataGDPostalAddressClass:
 *
 * All the fields in the #GDataGDPostalAddressClass structure are private and should never be accessed directly.
 *
 * Since: 0.4.0
 **/
typedef struct {
	/*< private >*/
	GDataParsableClass parent;
} GDataGDPostalAddressClass;

GType gdata_gd_postal_address_get_type (void) G_GNUC_CONST;

GDataGDPostalAddress *gdata_gd_postal_address_new (const gchar *address, const gchar *relation_type,
						   const gchar *label, gboolean is_primary) G_GNUC_WARN_UNUSED_RESULT;
gint gdata_gd_postal_address_compare (const GDataGDPostalAddress *a, const GDataGDPostalAddress *b);

const gchar *gdata_gd_postal_address_get_address (GDataGDPostalAddress *self);
void gdata_gd_postal_address_set_address (GDataGDPostalAddress *self, const gchar *address);

const gchar *gdata_gd_postal_address_get_relation_type (GDataGDPostalAddress *self);
void gdata_gd_postal_address_set_relation_type (GDataGDPostalAddress *self, const gchar *relation_type);

const gchar *gdata_gd_postal_address_get_label (GDataGDPostalAddress *self);
void gdata_gd_postal_address_set_label (GDataGDPostalAddress *self, const gchar *label);

gboolean gdata_gd_postal_address_is_primary (GDataGDPostalAddress *self);
void gdata_gd_postal_address_set_is_primary (GDataGDPostalAddress *self, gboolean is_primary);

G_END_DECLS

#endif /* !GDATA_GD_POSTAL_ADDRESS_H */
