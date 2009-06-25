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
 * SECTION:gdata-gd-organization
 * @short_description: GData organization element
 * @stability: Unstable
 * @include: gdata/gd/gdata-gd-organization.h
 *
 * #GDataGDOrganization represents an "organization" element from the
 * <ulink type="http" url="http://code.google.com/apis/gdata/docs/1.0/elements.html#gdOrganization">GData specification</ulink>.
 **/

#include <glib.h>
#include <libxml/parser.h>

#include "gdata-gd-organization.h"
#include "gdata-parsable.h"
#include "gdata-parser.h"

static void gdata_gd_organization_finalize (GObject *object);
static void gdata_gd_organization_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_gd_organization_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static gboolean pre_parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *root_node, gpointer user_data, GError **error);
static gboolean parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *root_node, gpointer user_data, GError **error);
static void pre_get_xml (GDataParsable *parsable, GString *xml_string);
static void get_xml (GDataParsable *parsable, GString *xml_string);
static void get_namespaces (GDataParsable *parsable, GHashTable *namespaces);

struct _GDataGDOrganizationPrivate {
	gchar *name;
	gchar *title;
	gchar *relation_type;
	gchar *label;
	gboolean is_primary;
};

enum {
	PROP_NAME = 1,
	PROP_TITLE,
	PROP_RELATION_TYPE,
	PROP_LABEL,
	PROP_IS_PRIMARY
};

G_DEFINE_TYPE (GDataGDOrganization, gdata_gd_organization, GDATA_TYPE_PARSABLE)
#define GDATA_GD_ORGANIZATION_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_GD_ORGANIZATION, GDataGDOrganizationPrivate))

static void
gdata_gd_organization_class_init (GDataGDOrganizationClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataGDOrganizationPrivate));

	gobject_class->get_property = gdata_gd_organization_get_property;
	gobject_class->set_property = gdata_gd_organization_set_property;
	gobject_class->finalize = gdata_gd_organization_finalize;

	parsable_class->pre_parse_xml = pre_parse_xml;
	parsable_class->parse_xml = parse_xml;
	parsable_class->pre_get_xml = pre_get_xml;
	parsable_class->get_xml = get_xml;
	parsable_class->get_namespaces = get_namespaces;

	/**
	 * GDataGDOrganization:name:
	 *
	 * The name of the organization.
	 *
	 * For more information, see the
	 * <ulink type="http" url="http://code.google.com/apis/gdata/docs/1.0/elements.html#gdOrganization">GData specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_NAME,
				g_param_spec_string ("name",
					"Name", "The name of the organization.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataGDOrganization:title:
	 *
	 * The title of a person within the organization.
	 *
	 * For more information, see the
	 * <ulink type="http" url="http://code.google.com/apis/gdata/docs/1.0/elements.html#gdOrganization">GData specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_TITLE,
				g_param_spec_string ("title",
					"Title", "The title of a person within the organization.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataGDOrganization:relation-type:
	 *
	 * A programmatic value that identifies the type of organization.
	 *
	 * For more information, see the
	 * <ulink type="http" url="http://code.google.com/apis/gdata/docs/1.0/elements.html#gdOrganization">GData specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_RELATION_TYPE,
				g_param_spec_string ("relation-type",
					"Relation type", "A programmatic value that identifies the type of organization.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataGDOrganization:label:
	 *
	 * A simple string value used to name this organization. It allows UIs to display a label such as "Work", "Volunteer",
	 * "Professional Society", etc.
	 *
	 * For more information, see the
	 * <ulink type="http" url="http://code.google.com/apis/gdata/docs/1.0/elements.html#gdOrganization">GData specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_LABEL,
				g_param_spec_string ("label",
					"Label", "A simple string value used to name this organization.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataGDOrganization:is-primary:
	 *
	 * Indicates which organization out of a group is primary.
	 *
	 * For more information, see the
	 * <ulink type="http" url="http://code.google.com/apis/gdata/docs/1.0/elements.html#gdOrganization">GData specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_IS_PRIMARY,
				g_param_spec_boolean ("is-primary",
					"Primary?", "Indicates which organization out of a group is primary.",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_gd_organization_init (GDataGDOrganization *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_GD_ORGANIZATION, GDataGDOrganizationPrivate);
}

static void
gdata_gd_organization_finalize (GObject *object)
{
	GDataGDOrganizationPrivate *priv = GDATA_GD_ORGANIZATION (object)->priv;

	g_free (priv->name);
	g_free (priv->title);
	g_free (priv->relation_type);
	g_free (priv->label);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_gd_organization_parent_class)->finalize (object);
}

static void
gdata_gd_organization_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataGDOrganizationPrivate *priv = GDATA_GD_ORGANIZATION (object)->priv;

	switch (property_id) {
		case PROP_NAME:
			g_value_set_string (value, priv->name);
			break;
		case PROP_TITLE:
			g_value_set_string (value, priv->title);
			break;
		case PROP_RELATION_TYPE:
			g_value_set_string (value, priv->relation_type);
			break;
		case PROP_LABEL:
			g_value_set_string (value, priv->label);
			break;
		case PROP_IS_PRIMARY:
			g_value_set_boolean (value, priv->is_primary);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gdata_gd_organization_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataGDOrganization *self = GDATA_GD_ORGANIZATION (object);

	switch (property_id) {
		case PROP_NAME:
			gdata_gd_organization_set_name (self, g_value_get_string (value));
			break;
		case PROP_TITLE:
			gdata_gd_organization_set_title (self, g_value_get_string (value));
			break;
		case PROP_RELATION_TYPE:
			gdata_gd_organization_set_relation_type (self, g_value_get_string (value));
			break;
		case PROP_LABEL:
			gdata_gd_organization_set_label (self, g_value_get_string (value));
			break;
		case PROP_IS_PRIMARY:
			gdata_gd_organization_set_is_primary (self, g_value_get_boolean (value));
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
	xmlChar *rel, *label, *primary;
	gboolean primary_bool;
	GDataGDOrganizationPrivate *priv = GDATA_GD_ORGANIZATION (parsable)->priv;

	rel = xmlGetProp (root_node, (xmlChar*) "rel");
	if (rel != NULL && *rel == '\0') {
		xmlFree (rel);
		return gdata_parser_error_required_property_missing (root_node, "rel", error);
	}

	/* Is it the primary organisation? */
	primary = xmlGetProp (root_node, (xmlChar*) "primary");
	if (primary == NULL || xmlStrcmp (primary, (xmlChar*) "false") == 0)
		primary_bool = FALSE;
	else if (xmlStrcmp (primary, (xmlChar*) "true") == 0)
		primary_bool = TRUE;
	else {
		gdata_parser_error_unknown_property_value (root_node, "primary", (gchar*) primary, error);
		xmlFree (primary);
		xmlFree (rel);
		return FALSE;
	}
	xmlFree (primary);

	/* Other properties */
	label = xmlGetProp (root_node, (xmlChar*) "label");

	priv->relation_type = g_strdup ((gchar*) rel);
	priv->label = g_strdup ((gchar*) label);
	priv->is_primary = primary_bool;

	xmlFree (rel);
	xmlFree (label);

	return TRUE;
}

static gboolean
parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error)
{
	GDataGDOrganizationPrivate *priv = GDATA_GD_ORGANIZATION (parsable)->priv;

	if (xmlStrcmp (node->name, (xmlChar*) "orgName") == 0) {
		/* gd:orgName */
		xmlChar *name;

		if (priv->name != NULL)
			return gdata_parser_error_duplicate_element (node, error);

		name = xmlNodeListGetString (doc, node->children, TRUE);
		priv->name = g_strdup ((gchar*) name);
		xmlFree (name);
	} else if (xmlStrcmp (node->name, (xmlChar*) "orgTitle") == 0) {
		/* gd:orgTitle */
		xmlChar *title;

		if (priv->title != NULL)
			return gdata_parser_error_duplicate_element (node, error);

		title = xmlNodeListGetString (doc, node->children, TRUE);
		priv->title = g_strdup ((gchar*) title);
		xmlFree (title);
	} else if (GDATA_PARSABLE_CLASS (gdata_gd_organization_parent_class)->parse_xml (parsable, doc, node, user_data, error) == FALSE) {
		/* Error! */
		return FALSE;
	}

	return TRUE;
}

static void
pre_get_xml (GDataParsable *parsable, GString *xml_string)
{
	GDataGDOrganizationPrivate *priv = GDATA_GD_ORGANIZATION (parsable)->priv;

	if (priv->relation_type != NULL)
		g_string_append_printf (xml_string, " rel='%s'", priv->relation_type);
	if (priv->label != NULL)
		g_string_append_printf (xml_string, " label='%s'", priv->label);

	if (priv->is_primary == TRUE)
		g_string_append (xml_string, " primary='true'");
	else
		g_string_append (xml_string, " primary='false'");
}

static void
get_xml (GDataParsable *parsable, GString *xml_string)
{
	GDataGDOrganizationPrivate *priv = GDATA_GD_ORGANIZATION (parsable)->priv;

	if (priv->name != NULL)
		g_string_append_printf (xml_string, "<gd:orgName>%s</gd:orgName>", priv->name);
	if (priv->title != NULL)
		g_string_append_printf (xml_string, "<gd:orgTitle>%s</gd:orgTitle>", priv->title);
}

static void
get_namespaces (GDataParsable *parsable, GHashTable *namespaces)
{
	g_hash_table_insert (namespaces, (gchar*) "gd", (gchar*) "http://schemas.google.com/g/2005");
}

/**
 * gdata_gd_organization_new:
 * @name: the name of the organization, or %NULL
 * @title: the owner's title within the organization, or %NULL
 * @relation_type: the relationship between the organization and its owner, or %NULL
 * @label: a human-readable label for the organization, or %NULL
 * @is_primary: %TRUE if this organization is its owner's primary organization, %FALSE otherwise
 *
 * Creates a new #GDataGDOrganization. More information is available in the <ulink type="http"
 * url="http://code.google.com/apis/gdata/docs/1.0/elements.html#gdOrganization">GData specification</ulink>.
 *
 * Return value: a new #GDataGDOrganization, or %NULL; unref with g_object_unref()
 *
 * Since: 0.2.0
 **/
GDataGDOrganization *
gdata_gd_organization_new (const gchar *name, const gchar *title, const gchar *relation_type, const gchar *label, gboolean is_primary)
{
	g_return_val_if_fail (relation_type == NULL || *relation_type != '\0', NULL);
	return g_object_new (GDATA_TYPE_GD_ORGANIZATION, "name", name, "title", title, "relation-type", relation_type,
			     "label", label, "is-primary", is_primary, NULL);
}

/**
 * gdata_gd_organization_compare:
 * @a: a #GDataGDOrganization, or %NULL
 * @b: another #GDataGDOrganization, or %NULL
 *
 * Compares the two organizations in a strcmp() fashion. %NULL values are handled gracefully, with
 * %0 returned if both @a and @b are %NULL, %-1 if @a is %NULL and %1 if @b is %NULL.
 *
 * The comparison of non-%NULL values is done on the basis of the @name property of the #GDataGDOrganization<!-- -->s.
 *
 * Return value: %0 if @a equals @b, %-1 or %1 as appropriate otherwise
 *
 * Since: 0.4.0
 **/
gint
gdata_gd_organization_compare (const GDataGDOrganization *a, const GDataGDOrganization *b)
{
	if (a == NULL && b != NULL)
		return -1;
	else if (b == NULL)
		return 1;

	if (a == b)
		return 0;
	return g_strcmp0 (a->priv->name, b->priv->name);
}

/**
 * gdata_gd_organization_get_name:
 * @self: a #GDataGDOrganization
 *
 * Gets the #GDataGDOrganization:name property.
 *
 * Return value: the organization's name, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_gd_organization_get_name (GDataGDOrganization *self)
{
	g_return_val_if_fail (GDATA_IS_GD_ORGANIZATION (self), NULL);
	return self->priv->name;
}

/**
 * gdata_gd_organization_set_name:
 * @self: a #GDataGDOrganization
 * @name: the new name for the organization
 *
 * Sets the #GDataGDOrganization:name property to @name.
 *
 * Set @name to %NULL to unset the property in the organization.
 *
 * Since: 0.4.0
 **/
void
gdata_gd_organization_set_name (GDataGDOrganization *self, const gchar *name)
{
	g_return_if_fail (GDATA_IS_GD_ORGANIZATION (self));

	g_free (self->priv->name);
	self->priv->name = g_strdup (name);
	g_object_notify (G_OBJECT (self), "name");
}

/**
 * gdata_gd_organization_get_title:
 * @self: a #GDataGDOrganization
 *
 * Gets the #GDataGDOrganization:title property.
 *
 * Return value: the organization's title, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_gd_organization_get_title (GDataGDOrganization *self)
{
	g_return_val_if_fail (GDATA_IS_GD_ORGANIZATION (self), NULL);
	return self->priv->title;
}

/**
 * gdata_gd_organization_set_title:
 * @self: a #GDataGDOrganization
 * @title: the new title for the organization, or %NULL
 *
 * Sets the #GDataGDOrganization:title property to @title.
 *
 * Set @title to %NULL to unset the property in the organization.
 *
 * Since: 0.4.0
 **/
void
gdata_gd_organization_set_title (GDataGDOrganization *self, const gchar *title)
{
	g_return_if_fail (GDATA_IS_GD_ORGANIZATION (self));

	g_free (self->priv->title);
	self->priv->title = g_strdup (title);
	g_object_notify (G_OBJECT (self), "title");
}

/**
 * gdata_gd_organization_get_relation_type:
 * @self: a #GDataGDOrganization
 *
 * Gets the #GDataGDOrganization:relation-type property.
 *
 * Return value: the organization's relation type, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_gd_organization_get_relation_type (GDataGDOrganization *self)
{
	g_return_val_if_fail (GDATA_IS_GD_ORGANIZATION (self), NULL);
	return self->priv->relation_type;
}

/**
 * gdata_gd_organization_set_relation_type:
 * @self: a #GDataGDOrganization
 * @relation_type: the new relation type for the organization, or %NULL
 *
 * Sets the #GDataGDOrganization:relation-type property to @relation_type.
 *
 * Set @relation_type to %NULL to unset the property in the organization.
 *
 * Since: 0.4.0
 **/
void
gdata_gd_organization_set_relation_type (GDataGDOrganization *self, const gchar *relation_type)
{
	g_return_if_fail (GDATA_IS_GD_ORGANIZATION (self));
	g_return_if_fail (relation_type == NULL || *relation_type != '\0');

	g_free (self->priv->relation_type);
	self->priv->relation_type = g_strdup (relation_type);
	g_object_notify (G_OBJECT (self), "relation-type");
}

/**
 * gdata_gd_organization_get_label:
 * @self: a #GDataGDOrganization
 *
 * Gets the #GDataGDOrganization:label property.
 *
 * Return value: the organization's label, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_gd_organization_get_label (GDataGDOrganization *self)
{
	g_return_val_if_fail (GDATA_IS_GD_ORGANIZATION (self), NULL);
	return self->priv->label;
}

/**
 * gdata_gd_organization_set_label:
 * @self: a #GDataGDOrganization
 * @label: the new label for the organization, or %NULL
 *
 * Sets the #GDataGDOrganization:label property to @label.
 *
 * Set @label to %NULL to unset the property in the organization.
 *
 * Since: 0.4.0
 **/
void
gdata_gd_organization_set_label (GDataGDOrganization *self, const gchar *label)
{
	g_return_if_fail (GDATA_IS_GD_ORGANIZATION (self));

	g_free (self->priv->label);
	self->priv->label = g_strdup (label);
	g_object_notify (G_OBJECT (self), "label");
}

/**
 * gdata_gd_organization_is_primary:
 * @self: a #GDataGDOrganization
 *
 * Gets the #GDataGDOrganization:is-primary property.
 *
 * Return value: %TRUE if this is the primary organization, %FALSE otherwise
 *
 * Since: 0.4.0
 **/
gboolean
gdata_gd_organization_is_primary (GDataGDOrganization *self)
{
	g_return_val_if_fail (GDATA_IS_GD_ORGANIZATION (self), FALSE);
	return self->priv->is_primary;
}

/**
 * gdata_gd_organization_set_is_primary:
 * @self: a #GDataGDOrganization
 * @is_primary: %TRUE if this is the primary organization, %FALSE otherwise
 *
 * Sets the #GDataGDOrganization:is-primary property to @is_primary.
 *
 * Since: 0.4.0
 **/
void
gdata_gd_organization_set_is_primary (GDataGDOrganization *self, gboolean is_primary)
{
	g_return_if_fail (GDATA_IS_GD_ORGANIZATION (self));

	self->priv->is_primary = is_primary;
	g_object_notify (G_OBJECT (self), "is-primary");
}
