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

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <string.h>

#include "gdata-contacts-query.h"
#include "gdata-query.h"

/* Reference: http://code.google.com/apis/contacts/docs/2.0/reference.html#Parameters */

static void gdata_contacts_query_finalize (GObject *object);
static void gdata_contacts_query_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_contacts_query_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

/* TODO: Actually override GDataQuery's get_query_uri function to return a URI including all our custom parameters */
struct _GDataContactsQueryPrivate {
	gchar *order_by; /* TODO: enum? #defined values? */
	gboolean show_deleted;
	gchar *sort_order; /* TODO: enum? */
	gchar *group;
};

enum {
	PROP_ORDER_BY = 1,
	PROP_SHOW_DELETED,
	PROP_SORT_ORDER,
	PROP_GROUP
};

G_DEFINE_TYPE (GDataContactsQuery, gdata_contacts_query, GDATA_TYPE_QUERY)
#define GDATA_CONTACTS_QUERY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_CONTACTS_QUERY, GDataContactsQueryPrivate))

static void
gdata_contacts_query_class_init (GDataContactsQueryClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataContactsQueryPrivate));

	gobject_class->set_property = gdata_contacts_query_set_property;
	gobject_class->get_property = gdata_contacts_query_get_property;
	gobject_class->finalize = gdata_contacts_query_finalize;

	g_object_class_install_property (gobject_class, PROP_ORDER_BY,
				g_param_spec_string ("order-by",
					"Order by", "TODO",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_SHOW_DELETED,
				g_param_spec_boolean ("show-deleted",
					"Show deleted", "TODO",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_SORT_ORDER,
				g_param_spec_string ("sort-order",
					"Sort order", "TODO",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_GROUP,
				g_param_spec_string ("group",
					"Group", "TODO",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_contacts_query_init (GDataContactsQuery *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_CONTACTS_QUERY, GDataContactsQueryPrivate);
}

static void
gdata_contacts_query_finalize (GObject *object)
{
	GDataContactsQueryPrivate *priv = GDATA_CONTACTS_QUERY_GET_PRIVATE (object);

	g_free (priv->order_by);
	g_free (priv->sort_order);
	g_free (priv->group);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_contacts_query_parent_class)->finalize (object);
}

static void
gdata_contacts_query_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataContactsQueryPrivate *priv = GDATA_CONTACTS_QUERY_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_ORDER_BY:
			g_value_set_string (value, priv->order_by);
			break;
		case PROP_SHOW_DELETED:
			g_value_set_boolean (value, priv->show_deleted);
			break;
		case PROP_SORT_ORDER:
			g_value_set_string (value, priv->sort_order);
			break;
		case PROP_GROUP:
			g_value_set_string (value, priv->group);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gdata_contacts_query_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataContactsQuery *self = GDATA_CONTACTS_QUERY (object);

	switch (property_id) {
		case PROP_ORDER_BY:
			gdata_contacts_query_set_order_by (self, g_value_get_string (value));
			break;
		case PROP_SHOW_DELETED:
			gdata_contacts_query_set_show_deleted (self, g_value_get_boolean (value));
			break;
		case PROP_SORT_ORDER:
			gdata_contacts_query_set_sort_order (self, g_value_get_string (value));
			break;
		case PROP_GROUP:
			gdata_contacts_query_set_group (self, g_value_get_string (value));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

GDataContactsQuery *
gdata_contacts_query_new (const gchar *q)
{
	return g_object_new (GDATA_TYPE_CONTACTS_QUERY, "q", q, NULL);
}

GDataContactsQuery *
gdata_contacts_query_new_with_limits (const gchar *q, GTimeVal *start_min, GTimeVal *start_max)
{
	return g_object_new (GDATA_TYPE_CONTACTS_QUERY,
			     "q", q,
			     "start-min", start_min,
			     "start-max", start_max,
			     NULL);
}

const gchar *
gdata_contacts_query_get_order_by (GDataContactsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_QUERY (self), NULL);
	return self->priv->order_by;
}

void
gdata_contacts_query_set_order_by (GDataContactsQuery *self, const gchar *order_by)
{
	g_return_if_fail (GDATA_IS_CONTACTS_QUERY (self));

	g_free (self->priv->order_by);
	self->priv->order_by = g_strdup (order_by);
	g_object_notify (G_OBJECT (self), "order-by");
}

gboolean
gdata_contacts_query_show_deleted (GDataContactsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_QUERY (self), FALSE);
	return self->priv->show_deleted;
}

void
gdata_contacts_query_set_show_deleted (GDataContactsQuery *self, gboolean show_deleted)
{
	g_return_if_fail (GDATA_IS_CONTACTS_QUERY (self));
	self->priv->show_deleted = show_deleted;
	g_object_notify (G_OBJECT (self), "show-deleted");
}

const gchar *
gdata_contacts_query_get_sort_order (GDataContactsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_QUERY (self), NULL);
	return self->priv->sort_order;
}

void
gdata_contacts_query_set_sort_order (GDataContactsQuery *self, const gchar *sort_order)
{
	g_return_if_fail (GDATA_IS_CONTACTS_QUERY (self));

	g_free (self->priv->sort_order);
	self->priv->sort_order = g_strdup (sort_order);
	g_object_notify (G_OBJECT (self), "sort-order");
}

const gchar *
gdata_contacts_query_get_group (GDataContactsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_QUERY (self), NULL);
	return self->priv->group;
}

void
gdata_contacts_query_set_group (GDataContactsQuery *self, const gchar *group)
{
	g_return_if_fail (GDATA_IS_CONTACTS_QUERY (self));

	g_free (self->priv->group);
	self->priv->group = g_strdup (group);
	g_object_notify (G_OBJECT (self), "group");
}
