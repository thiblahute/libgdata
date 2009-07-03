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

#ifndef GDATA_GD_ORGANIZATION_H
#define GDATA_GD_ORGANIZATION_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-parsable.h>

G_BEGIN_DECLS

#define GDATA_TYPE_GD_ORGANIZATION		(gdata_gd_organization_get_type ())
#define GDATA_GD_ORGANIZATION(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_GD_ORGANIZATION, GDataGDOrganization))
#define GDATA_GD_ORGANIZATION_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_GD_ORGANIZATION, GDataGDOrganizationClass))
#define GDATA_IS_GD_ORGANIZATION(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_GD_ORGANIZATION))
#define GDATA_IS_GD_ORGANIZATION_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_GD_ORGANIZATION))
#define GDATA_GD_ORGANIZATION_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_GD_ORGANIZATION, GDataGDOrganizationClass))

typedef struct _GDataGDOrganizationPrivate	GDataGDOrganizationPrivate;

/**
 * GDataGDOrganization:
 *
 * All the fields in the #GDataGDOrganization structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataParsable parent;
	GDataGDOrganizationPrivate *priv;
} GDataGDOrganization;

/**
 * GDataGDOrganizationClass:
 *
 * All the fields in the #GDataGDOrganizationClass structure are private and should never be accessed directly.
 *
 * Since: 0.4.0
 **/
typedef struct {
	/*< private >*/
	GDataParsableClass parent;
} GDataGDOrganizationClass;

GType gdata_gd_organization_get_type (void) G_GNUC_CONST;

GDataGDOrganization *gdata_gd_organization_new (const gchar *name, const gchar *title, const gchar *relation_type,
						const gchar *label, gboolean is_primary) G_GNUC_WARN_UNUSED_RESULT;
gint gdata_gd_organization_compare (const GDataGDOrganization *a, const GDataGDOrganization *b);

const gchar *gdata_gd_organization_get_name (GDataGDOrganization *self);
void gdata_gd_organization_set_name (GDataGDOrganization *self, const gchar *name);

const gchar *gdata_gd_organization_get_title (GDataGDOrganization *self);
void gdata_gd_organization_set_title (GDataGDOrganization *self, const gchar *title);

const gchar *gdata_gd_organization_get_relation_type (GDataGDOrganization *self);
void gdata_gd_organization_set_relation_type (GDataGDOrganization *self, const gchar *relation_type);

const gchar *gdata_gd_organization_get_label (GDataGDOrganization *self);
void gdata_gd_organization_set_label (GDataGDOrganization *self, const gchar *label);

gboolean gdata_gd_organization_is_primary (GDataGDOrganization *self);
void gdata_gd_organization_set_is_primary (GDataGDOrganization *self, gboolean is_primary);

G_END_DECLS

#endif /* !GDATA_GD_ORGANIZATION_H */
