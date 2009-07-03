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
 * SECTION:gdata-gd-who
 * @short_description: GData who element
 * @stability: Unstable
 * @include: gdata/gd/gdata-gd-who.h
 *
 * #GDataGDWho represents an "who" element from the
 * <ulink type="http" url="http://code.google.com/apis/gdata/docs/1.0/elements.html#gdWho">GData specification</ulink>.
 **/

#include <glib.h>
#include <libxml/parser.h>

#include "gdata-gd-who.h"
#include "gdata-parsable.h"
#include "gdata-parser.h"

static void gdata_gd_who_finalize (GObject *object);
static void gdata_gd_who_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_gd_who_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static gboolean pre_parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *root_node, gpointer user_data, GError **error);
static gboolean parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *root_node, gpointer user_data, GError **error);
static void pre_get_xml (GDataParsable *parsable, GString *xml_string);
static void get_xml (GDataParsable *parsable, GString *xml_string);
static void get_namespaces (GDataParsable *parsable, GHashTable *namespaces);

struct _GDataGDWhoPrivate {
	gchar *relation_type;
	gchar *value_string;
	gchar *email_address;
};

enum {
	PROP_RELATION_TYPE = 1,
	PROP_VALUE_STRING,
	PROP_EMAIL_ADDRESS
};

G_DEFINE_TYPE (GDataGDWho, gdata_gd_who, GDATA_TYPE_PARSABLE)
#define GDATA_GD_WHO_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_GD_WHO, GDataGDWhoPrivate))

static void
gdata_gd_who_class_init (GDataGDWhoClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataGDWhoPrivate));

	gobject_class->get_property = gdata_gd_who_get_property;
	gobject_class->set_property = gdata_gd_who_set_property;
	gobject_class->finalize = gdata_gd_who_finalize;

	parsable_class->pre_parse_xml = pre_parse_xml;
	parsable_class->parse_xml = parse_xml;
	parsable_class->pre_get_xml = pre_get_xml;
	parsable_class->get_xml = get_xml;
	parsable_class->get_namespaces = get_namespaces;
	parsable_class->element_name = "who";
	parsable_class->element_namespace = "gd";

	/**
	 * GDataGDWho:relation-type:
	 *
	 * Specifies the relationship between the containing entity and the contained person.
	 *
	 * For more information, see the
	 * <ulink type="http" url="http://code.google.com/apis/gdata/docs/1.0/elements.html#gdWho">GData specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_RELATION_TYPE,
				g_param_spec_string ("relation-type",
					"Relation type", "Specifies the relationship between the containing entity and the contained person.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataGDWho:value-string:
	 *
	 * A simple string value that can be used as a representation of this person.
	 *
	 * For more information, see the
	 * <ulink type="http" url="http://code.google.com/apis/gdata/docs/1.0/elements.html#gdWho">GData specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_VALUE_STRING,
				g_param_spec_string ("value-string",
					"Value string", "A simple string value that can be used as a representation of this person.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataGDWho:email-address:
	 *
	 * The e-mail address of the person represented by the #GDataWho.
	 *
	 * For more information, see the
	 * <ulink type="http" url="http://code.google.com/apis/gdata/docs/1.0/elements.html#gdWho">GData specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_EMAIL_ADDRESS,
				g_param_spec_string ("email-address",
					"E-mail address", "The e-mail address of the person represented by the #GDataWho.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_gd_who_init (GDataGDWho *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_GD_WHO, GDataGDWhoPrivate);
}

static void
gdata_gd_who_finalize (GObject *object)
{
	GDataGDWhoPrivate *priv = GDATA_GD_WHO (object)->priv;

	g_free (priv->relation_type);
	g_free (priv->value_string);
	g_free (priv->email_address);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_gd_who_parent_class)->finalize (object);
}

static void
gdata_gd_who_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataGDWhoPrivate *priv = GDATA_GD_WHO (object)->priv;

	switch (property_id) {
		case PROP_RELATION_TYPE:
			g_value_set_string (value, priv->relation_type);
			break;
		case PROP_VALUE_STRING:
			g_value_set_string (value, priv->value_string);
			break;
		case PROP_EMAIL_ADDRESS:
			g_value_set_string (value, priv->email_address);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gdata_gd_who_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataGDWho *self = GDATA_GD_WHO (object);

	switch (property_id) {
		case PROP_RELATION_TYPE:
			gdata_gd_who_set_relation_type (self, g_value_get_string (value));
			break;
		case PROP_VALUE_STRING:
			gdata_gd_who_set_value_string (self, g_value_get_string (value));
			break;
		case PROP_EMAIL_ADDRESS:
			gdata_gd_who_set_email_address (self, g_value_get_string (value));
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
	xmlChar *rel, *value_string, *email;
	GDataGDWhoPrivate *priv = GDATA_GD_WHO (parsable)->priv;

	rel = xmlGetProp (root_node, (xmlChar*) "rel");
	if (rel != NULL && *rel == '\0') {
		xmlFree (rel);
		return gdata_parser_error_required_property_missing (root_node, "rel", error);
	}

	email = xmlGetProp (root_node, (xmlChar*) "email");
	if (email != NULL && *email == '\0') {
		xmlFree (rel);
		xmlFree (email);
		return gdata_parser_error_required_property_missing (root_node, "email", error);
	}
	value_string = xmlGetProp (root_node, (xmlChar*) "valueString");

	priv->relation_type = g_strdup ((gchar*) rel);
	priv->value_string = g_strdup ((gchar*) value_string);
	priv->email_address = g_strdup ((gchar*) email);

	xmlFree (rel);
	xmlFree (value_string);
	xmlFree (email);

	return TRUE;
}

static gboolean
parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error)
{
	GDataGDWhoPrivate *priv = GDATA_GD_WHO (parsable)->priv;

	/* TODO: deal with the attendeeType, attendeeStatus and entryLink */

	return TRUE;
}

static void
pre_get_xml (GDataParsable *parsable, GString *xml_string)
{
	GDataGDWhoPrivate *priv = GDATA_GD_WHO (parsable)->priv;

	if (priv->email_address != NULL)
		g_string_append_printf (xml_string, " email='%s'", priv->email_address);
	if (priv->relation_type != NULL)
		g_string_append_printf (xml_string, " rel='%s'", priv->relation_type);
	if (priv->value_string != NULL) {
		gchar *value_string = g_markup_escape_text (priv->value_string, -1);
		g_string_append_printf (xml_string, " valueString='%s'", value_string);
		g_free (value_string);
	}
}

static void
get_xml (GDataParsable *parsable, GString *xml_string)
{
	GDataGDWhoPrivate *priv = GDATA_GD_WHO (parsable)->priv;

	/* TODO: deal with the attendeeType, attendeeStatus and entryLink */
}

static void
get_namespaces (GDataParsable *parsable, GHashTable *namespaces)
{
	g_hash_table_insert (namespaces, (gchar*) "gd", (gchar*) "http://schemas.google.com/g/2005");
}

/**
 * gdata_gd_who_new:
 * @relation_type: the relationship between the item and this person, or %NULL
 * @value_string: a string to represent the person, or %NULL
 * @email_address: the person's e-mail address, or %NULL
 *
 * Creates a new #GDataGDWho. More information is available in the <ulink type="http"
 * url="http://code.google.com/apis/gdata/docs/1.0/elements.html#gdWho">GData specification</ulink>.
 *
 * Currently, entryLink functionality is not implemented in #GDataGDWho.
 *
 * Return value: a new #GDataGDWho; unref with g_object_unref()
 **/
GDataGDWho *
gdata_gd_who_new (const gchar *relation_type, const gchar *value_string, const gchar *email_address)
{
	g_return_val_if_fail (relation_type == NULL || *relation_type != '\0', NULL);
	g_return_val_if_fail (email_address == NULL || *email_address != '\0', NULL);
	return g_object_new (GDATA_TYPE_GD_WHO, "relation-type", relation_type, "value-string", value_string, "email-address", email_address, NULL);
}

/**
 * gdata_gd_who_compare:
 * @a: a #GDataGDWho, or %NULL
 * @b: another #GDataGDWho, or %NULL
 *
 * Compares the two people in a strcmp() fashion. %NULL values are handled gracefully, with
 * %0 returned if both @a and @b are %NULL, %-1 if @a is %NULL and %1 if @b is %NULL.
 *
 * The comparison of non-%NULL values is done on the basis of the @email and @value_string properties of the #GDataGDWho<!-- -->s.
 *
 * Return value: %0 if @a equals @b, %-1 or %1 as appropriate otherwise
 *
 * Since: 0.4.0
 **/
gint
gdata_gd_who_compare (const GDataGDWho *a, const GDataGDWho *b)
{
	if (a == NULL && b != NULL)
		return -1;
	else if (b == NULL)
		return 1;

	if (a == b)
		return 0;

	if (g_strcmp0 (a->priv->value_string, b->priv->value_string) == 0 && g_strcmp0 (a->priv->email_address, b->priv->email_address) == 0)
		return 0;
	return 1;
}

/**
 * gdata_gd_who_get_relation_type:
 * @self: a #GDataGDWho
 *
 * Gets the #GDataGDWho:relation-type property.
 *
 * Return value: the relation type, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_gd_who_get_relation_type (GDataGDWho *self)
{
	g_return_val_if_fail (GDATA_IS_GD_WHO (self), NULL);
	return self->priv->relation_type;
}

/**
 * gdata_gd_who_set_relation_type:
 * @self: a #GDataGDWho
 * @relation_type: the new relation type
 *
 * Sets the #GDataGDWho:relation-type property to @relation_type.
 *
 * Set @relation_type to %NULL to unset the property.
 *
 * Since: 0.4.0
 **/
void
gdata_gd_who_set_relation_type (GDataGDWho *self, const gchar *relation_type)
{
	g_return_if_fail (GDATA_IS_GD_WHO (self));
	g_return_if_fail (relation_type == NULL || *relation_type != '\0');

	g_free (self->priv->relation_type);
	self->priv->relation_type = g_strdup (relation_type);
	g_object_notify (G_OBJECT (self), "relation-type");
}

/**
 * gdata_gd_who_get_value_string:
 * @self: a #GDataGDWho
 *
 * Gets the #GDataGDWho:value-string property.
 *
 * Return value: the value string, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_gd_who_get_value_string (GDataGDWho *self)
{
	g_return_val_if_fail (GDATA_IS_GD_WHO (self), NULL);
	return self->priv->value_string;
}

/**
 * gdata_gd_who_set_value_string:
 * @self: a #GDataGDWho
 * @value_string: the new value string, or %NULL
 *
 * Sets the #GDataGDWho:value-string property to @value_string.
 *
 * Set @value_string to %NULL to unset the property.
 *
 * Since: 0.4.0
 **/
void
gdata_gd_who_set_value_string (GDataGDWho *self, const gchar *value_string)
{
	g_return_if_fail (GDATA_IS_GD_WHO (self));

	g_free (self->priv->value_string);
	self->priv->value_string = g_strdup (value_string);
	g_object_notify (G_OBJECT (self), "value-string");
}

/**
 * gdata_gd_who_get_email_address:
 * @self: a #GDataGDWho
 *
 * Gets the #GDataGDWho:email-address property.
 *
 * Return value: the e-mail address, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_gd_who_get_email_address (GDataGDWho *self)
{
	g_return_val_if_fail (GDATA_IS_GD_WHO (self), NULL);
	return self->priv->email_address;
}

/**
 * gdata_gd_who_set_email_address:
 * @self: a #GDataGDWho
 * @email_address: the new e-mail address, or %NULL
 *
 * Sets the #GDataGDWho:email-address property to @email_address.
 *
 * Set @email_address to %NULL to unset the property.
 *
 * Since: 0.4.0
 **/
void
gdata_gd_who_set_email_address (GDataGDWho *self, const gchar *email_address)
{
	g_return_if_fail (GDATA_IS_GD_WHO (self));
	g_return_if_fail (email_address == NULL || *email_address != '\0');

	g_free (self->priv->email_address);
	self->priv->email_address = g_strdup (email_address);
	g_object_notify (G_OBJECT (self), "email-address");
}
