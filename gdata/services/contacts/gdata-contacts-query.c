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

/**
 * SECTION:gdata-contacts-query
 * @short_description: GData Contacts query object
 * @stability: Unstable
 * @include: gdata/services/contacts/gdata-contacts-query.h
 *
 * #GDataContactsQuery represents a collection of query parameters specific to the Google Contacts service, which go above and beyond
 * those catered for by #GDataQuery.
 *
 * For more information on the custom GData query parameters supported by #GDataContactsQuery, see the <ulink type="http"
 * url="http://code.google.com/apis/contacts/docs/2.0/reference.html#Parameters">online documentation</ulink>.
 **/

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <string.h>

#include "gdata-contacts-query.h"
#include "gdata-query.h"

static void gdata_contacts_query_finalize (GObject *object);
static void gdata_contacts_query_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_contacts_query_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void get_query_uri (GDataQuery *self, const gchar *feed_uri, GString *query_uri, gboolean *params_started);

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
	GDataQueryClass *query_class = GDATA_QUERY_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataContactsQueryPrivate));

	gobject_class->set_property = gdata_contacts_query_set_property;
	gobject_class->get_property = gdata_contacts_query_get_property;
	gobject_class->finalize = gdata_contacts_query_finalize;

	query_class->get_query_uri = get_query_uri;

	/**
	 * GDataContactsQuery:order-by:
	 *
	 * Sorting criterion. The only supported value is <literal>lastmodified</literal>.
	 *
	 * Since: 0.2.0
	 **/
	g_object_class_install_property (gobject_class, PROP_ORDER_BY,
				g_param_spec_string ("order-by",
					"Order by", "Sorting criterion.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataContactsQuery:show-deleted:
	 *
	 * Whether to include deleted contacts in the query feed. Deleted contacts return %TRUE
	 * from gdata_contacts_contact_is_deleted(), and have no other information. They do not
	 * normally appear in query results.
	 *
	 * Since: 0.2.0
	 **/
	g_object_class_install_property (gobject_class, PROP_SHOW_DELETED,
				g_param_spec_boolean ("show-deleted",
					"Show deleted?", "Whether to include deleted contacts in the query feed.",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataContactsQuery:sort-order:
	 *
	 * Sorting order direction. Can be either <literal>ascending</literal> or <literal>descending</literal>.
	 *
	 * Since: 0.2.0
	 **/
	g_object_class_install_property (gobject_class, PROP_SORT_ORDER,
				g_param_spec_string ("sort-order",
					"Sort order", "Sorting order direction.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataContactsQuery:group:
	 *
	 * Constrains the results to only the contacts belonging to the group specified. The value of this parameter
	 * should be a group ID URI.
	 *
	 * Since: 0.2.0
	 **/
	g_object_class_install_property (gobject_class, PROP_GROUP,
				g_param_spec_string ("group",
					"Group", "Constrains the results to only the contacts belonging to the group specified.",
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

static void
get_query_uri (GDataQuery *self, const gchar *feed_uri, GString *query_uri, gboolean *params_started)
{
	GDataContactsQueryPrivate *priv = GDATA_CONTACTS_QUERY (self)->priv;

	#define APPEND_SEP g_string_append_c (query_uri, (*params_started == FALSE) ? '?' : '&'); *params_started = TRUE;

	/* Chain up to the parent class */
	GDATA_QUERY_CLASS (gdata_contacts_query_parent_class)->get_query_uri (self, feed_uri, query_uri, params_started);

	if (priv->order_by != NULL) {
		APPEND_SEP
		g_string_append (query_uri, "orderby=");
		g_string_append_uri_escaped (query_uri, priv->order_by, NULL, TRUE);
	}

	APPEND_SEP
	if (priv->show_deleted == TRUE)
		g_string_append (query_uri, "showdeleted=true");
	else
		g_string_append (query_uri, "showdeleted=false");

	if (priv->sort_order != NULL) {
		APPEND_SEP
		g_string_append (query_uri, "sortorder=");
		g_string_append_uri_escaped (query_uri, priv->sort_order, NULL, TRUE);
	}

	if (priv->group != NULL) {
		APPEND_SEP
		g_string_append (query_uri, "group=");
		g_string_append_uri_escaped (query_uri, priv->group, NULL, TRUE);
	}
}

/**
 * gdata_contacts_query_new:
 * @q: a query string
 *
 * Creates a new #GDataContactsQuery with its #GDataQuery:q property set to @q.
 *
 * Return value: a new #GDataContactsQuery
 *
 * Since: 0.2.0
 **/
GDataContactsQuery *
gdata_contacts_query_new (const gchar *q)
{
	return g_object_new (GDATA_TYPE_CONTACTS_QUERY, "q", q, NULL);
}

/**
 * gdata_contacts_query_new_with_limits:
 * @q: a query string
 * @start_index: a one-based start index for the results
 * @max_results: the maximum number of results to return
 *
 * Creates a new #GDataContactsQuery with its #GDataQuery:q property set to @q, and the limits @start_index and @max_results
 * applied.
 *
 * Return value: a new #GDataContactsQuery
 *
 * Since: 0.2.0
 **/
GDataContactsQuery *
gdata_contacts_query_new_with_limits (const gchar *q, gint start_index, gint max_results)
{
	return g_object_new (GDATA_TYPE_CONTACTS_QUERY,
			     "q", q,
			     "start-index", start_index,
			     "max-results", max_results,
			     NULL);
}

/**
 * gdata_contacts_query_get_order_by:
 * @self: a #GDataContactsQuery
 *
 * Gets the #GDataContactsQuery:order-by property.
 *
 * Return value: the order by property, or %NULL if it is unset
 *
 * Since: 0.2.0
 **/
const gchar *
gdata_contacts_query_get_order_by (GDataContactsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_QUERY (self), NULL);
	return self->priv->order_by;
}

/**
 * gdata_contacts_query_set_order_by:
 * @self: a #GDataContactsQuery
 * @order_by: a new order by string, or %NULL
 *
 * Sets the #GDataContactsQuery:order-by property of the #GDataContactsQuery to the new order by string, @order_by.
 *
 * Set @order_by to %NULL to unset the property in the query URI.
 *
 * Since: 0.2.0
 **/
void
gdata_contacts_query_set_order_by (GDataContactsQuery *self, const gchar *order_by)
{
	g_return_if_fail (GDATA_IS_CONTACTS_QUERY (self));

	g_free (self->priv->order_by);
	self->priv->order_by = g_strdup (order_by);
	g_object_notify (G_OBJECT (self), "order-by");
}

/**
 * gdata_contacts_query_show_deleted:
 * @self: a #GDataContactsQuery
 *
 * Gets the #GDataContactsQuery:show-deleted property.
 *
 * Return value: %TRUE if deleted contacts should be shown, %FALSE otherwise
 *
 * Since: 0.2.0
 **/
gboolean
gdata_contacts_query_show_deleted (GDataContactsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_QUERY (self), FALSE);
	return self->priv->show_deleted;
}

/**
 * gdata_contacts_query_set_show_deleted:
 * @self: a #GDataContactsQuery
 * @show_deleted: %TRUE to show deleted contacts, %FALSE otherwise
 *
 * Sets the #GDataContactsQuery:show-deleted property of the #GDataContactsQuery.
 *
 * Since: 0.2.0
 **/
void
gdata_contacts_query_set_show_deleted (GDataContactsQuery *self, gboolean show_deleted)
{
	g_return_if_fail (GDATA_IS_CONTACTS_QUERY (self));
	self->priv->show_deleted = show_deleted;
	g_object_notify (G_OBJECT (self), "show-deleted");
}

/**
 * gdata_contacts_query_get_sort_order:
 * @self: a #GDataContactsQuery
 *
 * Gets the #GDataContactsQuery:sort-order property.
 *
 * Return value: the sort order property, or %NULL if it is unset
 *
 * Since: 0.2.0
 **/
const gchar *
gdata_contacts_query_get_sort_order (GDataContactsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_QUERY (self), NULL);
	return self->priv->sort_order;
}

/**
 * gdata_contacts_query_set_sort_order:
 * @self: a #GDataContactsQuery
 * @sort_order: a new sort order string, or %NULL
 *
 * Sets the #GDataContactsQuery:sort-order property of the #GDataContactsQuery to the new sort order string, @sort_order.
 *
 * Set @sort_order to %NULL to unset the property in the query URI.
 *
 * Since: 0.2.0
 **/
void
gdata_contacts_query_set_sort_order (GDataContactsQuery *self, const gchar *sort_order)
{
	g_return_if_fail (GDATA_IS_CONTACTS_QUERY (self));

	g_free (self->priv->sort_order);
	self->priv->sort_order = g_strdup (sort_order);
	g_object_notify (G_OBJECT (self), "sort-order");
}

/**
 * gdata_contacts_query_get_group:
 * @self: a #GDataContactsQuery
 *
 * Gets the #GDataContactsQuery:group property.
 *
 * Return value: the group property, or %NULL if it is unset
 *
 * Since: 0.2.0
 **/
const gchar *
gdata_contacts_query_get_group (GDataContactsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_QUERY (self), NULL);
	return self->priv->group;
}

/**
 * gdata_contacts_query_set_group:
 * @self: a #GDataContactsQuery
 * @group: a new group ID URI, or %NULL
 *
 * Sets the #GDataContactsQuery:group property of the #GDataContactsQuery to the new group ID URI, @group.
 *
 * Set @group to %NULL to unset the property in the query URI.
 *
 * Since: 0.2.0
 **/
void
gdata_contacts_query_set_group (GDataContactsQuery *self, const gchar *group)
{
	g_return_if_fail (GDATA_IS_CONTACTS_QUERY (self));

	g_free (self->priv->group);
	self->priv->group = g_strdup (group);
	g_object_notify (G_OBJECT (self), "group");
}
