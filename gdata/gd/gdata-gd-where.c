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

/**
 * SECTION:gdata-gd-where
 * @short_description: GData where element
 * @stability: Unstable
 * @include: gdata/gd/gdata-gd-where.h
 *
 * #GDataGDWhere represents a "where" element from the
 * <ulink type="http" url="http://code.google.com/apis/gdata/docs/1.0/elements.html#gdWhere">GData specification</ulink>.
 **/

#include <glib.h>
#include <libxml/parser.h>

#include "gdata-gd-where.h"
#include "gdata-parsable.h"
#include "gdata-parser.h"

static void gdata_gd_where_finalize (GObject *object);
static void gdata_gd_where_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_gd_where_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static gboolean pre_parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *root_node, gpointer user_data, GError **error);
static gboolean parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *root_node, gpointer user_data, GError **error);
static void pre_get_xml (GDataParsable *parsable, GString *xml_string);
static void get_xml (GDataParsable *parsable, GString *xml_string);
static void get_namespaces (GDataParsable *parsable, GHashTable *namespaces);

struct _GDataGDWherePrivate {
	gchar *relation_type;
	gchar *value_string;
	gchar *label;
};

enum {
	PROP_RELATION_TYPE = 1,
	PROP_VALUE_STRING,
	PROP_LABEL
};

G_DEFINE_TYPE (GDataGDWhere, gdata_gd_where, GDATA_TYPE_PARSABLE)
#define GDATA_GD_WHERE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_GD_WHERE, GDataGDWherePrivate))

static void
gdata_gd_where_class_init (GDataGDWhereClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataGDWherePrivate));

	gobject_class->get_property = gdata_gd_where_get_property;
	gobject_class->set_property = gdata_gd_where_set_property;
	gobject_class->finalize = gdata_gd_where_finalize;

	parsable_class->pre_parse_xml = pre_parse_xml;
	parsable_class->parse_xml = parse_xml;
	parsable_class->pre_get_xml = pre_get_xml;
	parsable_class->get_xml = get_xml;
	parsable_class->get_namespaces = get_namespaces;

	/**
	 * GDataGDWhere:relation-type:
	 *
	 * Specifies the relationship between the containing entity and the contained location.
	 *
	 * For more information, see the
	 * <ulink type="http" url="http://code.google.com/apis/gdata/docs/1.0/elements.html#gdWhere">GData specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_RELATION_TYPE,
				g_param_spec_string ("relation-type",
					"Relation type", "Specifies the relationship between the containing entity and the contained location.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataGDWhere:value-string:
	 *
	 * A simple string value that can be used as a representation of this location.
	 *
	 * For more information, see the
	 * <ulink type="http" url="http://code.google.com/apis/gdata/docs/1.0/elements.html#gdWhere">GData specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_VALUE_STRING,
				g_param_spec_string ("value-string",
					"Value string", "A simple string value that can be used as a representation of this location.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataGDWhere:label:
	 *
	 * Specifies a user-readable label to distinguish this location from other locations.
	 *
	 * For more information, see the
	 * <ulink type="http" url="http://code.google.com/apis/gdata/docs/1.0/elements.html#gdWhere">GData specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_LABEL,
				g_param_spec_string ("label",
					"Label", "Specifies a user-readable label to distinguish this location from other locations.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_gd_where_init (GDataGDWhere *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_GD_WHERE, GDataGDWherePrivate);
}

static void
gdata_gd_where_finalize (GObject *object)
{
	GDataGDWherePrivate *priv = GDATA_GD_WHERE (object)->priv;

	g_free (priv->relation_type);
	g_free (priv->value_string);
	g_free (priv->label);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_gd_where_parent_class)->finalize (object);
}

static void
gdata_gd_where_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataGDWherePrivate *priv = GDATA_GD_WHERE (object)->priv;

	switch (property_id) {
		case PROP_RELATION_TYPE:
			g_value_set_string (value, priv->relation_type);
			break;
		case PROP_VALUE_STRING:
			g_value_set_string (value, priv->value_string);
			break;
		case PROP_LABEL:
			g_value_set_string (value, priv->label);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gdata_gd_where_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataGDWhere *self = GDATA_GD_WHERE (object);

	switch (property_id) {
		case PROP_RELATION_TYPE:
			gdata_gd_where_set_relation_type (self, g_value_get_string (value));
			break;
		case PROP_VALUE_STRING:
			gdata_gd_where_set_value_string (self, g_value_get_string (value));
			break;
		case PROP_LABEL:
			gdata_gd_where_set_label (self, g_value_get_string (value));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static gboolean
pre_parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *root_node, gpointer user_data, GError **error)
{
	xmlChar *rel, *value_string, *label;
	GDataGDWherePrivate *priv = GDATA_GD_WHERE (parsable)->priv;

	rel = xmlGetProp (root_node, (xmlChar*) "rel");
	if (rel != NULL && *rel == '\0') {
		xmlFree (rel);
		return gdata_parser_error_required_property_missing (root_node, "rel", error);
	}

	label = xmlGetProp (root_node, (xmlChar*) "label");
	value_string = xmlGetProp (root_node, (xmlChar*) "valueString");

	priv->relation_type = g_strdup ((gchar*) rel);
	priv->value_string = g_strdup ((gchar*) value_string);
	priv->label = g_strdup ((gchar*) label);

	xmlFree (rel);
	xmlFree (value_string);
	xmlFree (label);

	return TRUE;
}

static gboolean
parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error)
{
	GDataGDWherePrivate *priv = GDATA_GD_WHERE (parsable)->priv;

	/* TODO: deal with the entryLink */

	return TRUE;
}

static void
pre_get_xml (GDataParsable *parsable, GString *xml_string)
{
	GDataGDWherePrivate *priv = GDATA_GD_WHERE (parsable)->priv;

	if (priv->label != NULL)
		g_string_append_printf (xml_string, " label='%s'", priv->label);
	if (priv->relation_type != NULL)
		g_string_append_printf (xml_string, " rel='%s'", priv->relation_type);
	if (priv->value_string != NULL)
		g_string_append_printf (xml_string, " valueString='%s'", priv->value_string);
}

static void
get_xml (GDataParsable *parsable, GString *xml_string)
{
	GDataGDWherePrivate *priv = GDATA_GD_WHERE (parsable)->priv;

	/* TODO: deal with the entryLink */
}

static void
get_namespaces (GDataParsable *parsable, GHashTable *namespaces)
{
	g_hash_table_insert (namespaces, (gchar*) "gd", (gchar*) "http://schemas.google.com/g/2005");
}

/**
 * gdata_gd_where_new:
 * @relation_type: the relationship between the item and this place, or %NULL
 * @value_string: a string to represent the place, or %NULL
 * @label: a human-readable label for the place, or %NULL
 *
 * Creates a new #GDataGDWhere. More information is available in the <ulink type="http"
 * url="http://code.google.com/apis/gdata/docs/1.0/elements.html#gdWhere">GData specification</ulink>.
 *
 * Currently, entryLink functionality is not implemented in #GDataGDWhere.
 *
 * Return value: a new #GDataGDWhere; unref with g_object_unref()
 **/
GDataGDWhere *
gdata_gd_where_new (const gchar *relation_type, const gchar *value_string, const gchar *label)
{
	g_return_val_if_fail (relation_type == NULL || *relation_type != '\0', NULL);
	return g_object_new (GDATA_TYPE_GD_WHERE, "relation-type", relation_type, "value-string", value_string, "label", label, NULL);
}

/**
 * gdata_gd_where_compare:
 * @a: a #GDataGDWhere, or %NULL
 * @b: another #GDataGDWhere, or %NULL
 *
 * Compares the two places in a strcmp() fashion. %NULL values are handled gracefully, with
 * %0 returned if both @a and @b are %NULL, %-1 if @a is %NULL and %1 if @b is %NULL.
 *
 * The comparison of non-%NULL values is done on the basis of the @label and @value_string properties of the #GDataGDWhere<!-- -->s.
 *
 * Return value: %0 if @a equals @b, %-1 or %1 as appropriate otherwise
 *
 * Since: 0.4.0
 **/
gint
gdata_gd_where_compare (const GDataGDWhere *a, const GDataGDWhere *b)
{
	gint value_string_cmp;

	if (a == NULL && b != NULL)
		return -1;
	else if (b == NULL)
		return 1;

	if (a == b)
		return 0;

	value_string_cmp = g_strcmp0 (a->priv->value_string, b->priv->value_string);
	if (value_string_cmp == 0 && g_strcmp0 (a->priv->label, b->priv->label))
		return 0;
	return value_string_cmp;
}

/**
 * gdata_gd_where_get_relation_type:
 * @self: a #GDataGDWhere
 *
 * Gets the #GDataGDWhere:relation-type property.
 *
 * Return value: the relation type, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_gd_where_get_relation_type (GDataGDWhere *self)
{
	g_return_val_if_fail (GDATA_IS_GD_WHERE (self), NULL);
	return self->priv->relation_type;
}

/**
 * gdata_gd_where_set_relation_type:
 * @self: a #GDataGDWhere
 * @relation_type: the new relation type
 *
 * Sets the #GDataGDWhere:relation-type property to @relation_type.
 *
 * Set @relation_type to %NULL to unset the property.
 *
 * Since: 0.4.0
 **/
void
gdata_gd_where_set_relation_type (GDataGDWhere *self, const gchar *relation_type)
{
	g_return_if_fail (GDATA_IS_GD_WHERE (self));
	g_return_if_fail (relation_type == NULL || *relation_type != '\0');

	g_free (self->priv->relation_type);
	self->priv->relation_type = g_strdup (relation_type);
	g_object_notify (G_OBJECT (self), "relation-type");
}

/**
 * gdata_gd_where_get_value_string:
 * @self: a #GDataGDWhere
 *
 * Gets the #GDataGDWhere:value-string property.
 *
 * Return value: the value string, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_gd_where_get_value_string (GDataGDWhere *self)
{
	g_return_val_if_fail (GDATA_IS_GD_WHERE (self), NULL);
	return self->priv->value_string;
}

/**
 * gdata_gd_where_set_value_string:
 * @self: a #GDataGDWhere
 * @value_string: the new value string, or %NULL
 *
 * Sets the #GDataGDWhere:value-string property to @value_string.
 *
 * Set @value_string to %NULL to unset the property.
 *
 * Since: 0.4.0
 **/
void
gdata_gd_where_set_value_string (GDataGDWhere *self, const gchar *value_string)
{
	g_return_if_fail (GDATA_IS_GD_WHERE (self));

	g_free (self->priv->value_string);
	self->priv->value_string = g_strdup (value_string);
	g_object_notify (G_OBJECT (self), "value-string");
}

/**
 * gdata_gd_where_get_label:
 * @self: a #GDataGDWhere
 *
 * Gets the #GDataGDWhere:label property.
 *
 * Return value: the label, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_gd_where_get_label (GDataGDWhere *self)
{
	g_return_val_if_fail (GDATA_IS_GD_WHERE (self), NULL);
	return self->priv->label;
}

/**
 * gdata_gd_where_set_label:
 * @self: a #GDataGDWhere
 * @label: the new label, or %NULL
 *
 * Sets the #GDataGDWhere:label property to @label.
 *
 * Set @label to %NULL to unset the property.
 *
 * Since: 0.4.0
 **/
void
gdata_gd_where_set_label (GDataGDWhere *self, const gchar *label)
{
	g_return_if_fail (GDATA_IS_GD_WHERE (self));

	g_free (self->priv->label);
	self->priv->label = g_strdup (label);
	g_object_notify (G_OBJECT (self), "label");
}
