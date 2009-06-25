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
 * SECTION:gdata-generator
 * @short_description: Atom generator element
 * @stability: Unstable
 * @include: gdata/atom/gdata-generator.h
 *
 * #GDataGenerator represents a "generator" element from the
 * <ulink type="http" url="http://www.atomenabled.org/developers/syndication/atom-format-spec.php">Atom specification</ulink>.
 **/

#include <glib.h>
#include <libxml/parser.h>

#include "gdata-generator.h"
#include "gdata-parsable.h"
#include "gdata-parser.h"

static void gdata_generator_finalize (GObject *object);
static void gdata_generator_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static gboolean pre_parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *root_node, gpointer user_data, GError **error);
static gboolean parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *root_node, gpointer user_data, GError **error);

struct _GDataGeneratorPrivate {
	gchar *name;
	gchar *uri;
	gchar *version;
};

enum {
	PROP_NAME = 1,
	PROP_URI,
	PROP_VERSION
};

G_DEFINE_TYPE (GDataGenerator, gdata_generator, GDATA_TYPE_PARSABLE)
#define GDATA_GENERATOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_GENERATOR, GDataGeneratorPrivate))

static void
gdata_generator_class_init (GDataGeneratorClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataGeneratorPrivate));

	gobject_class->get_property = gdata_generator_get_property;
	gobject_class->finalize = gdata_generator_finalize;

	parsable_class->pre_parse_xml = pre_parse_xml;
	parsable_class->parse_xml = parse_xml;

	/**
	 * GDataGenerator:name:
	 *
	 * A human-readable name for the generating agent.
	 *
	 * For more information, see the
	 * <ulink type="http" url="http://www.atomenabled.org/developers/syndication/atom-format-spec.php#element.generator">Atom specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_NAME,
				g_param_spec_string ("name",
					"Name", "A human-readable name for the generating agent.",
					NULL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataGenerator:uri:
	 *
	 * An IRI reference that is relevant to the agent.
	 *
	 * For more information, see the
	 * <ulink type="http" url="http://www.atomenabled.org/developers/syndication/atom-format-spec.php#element.generator">Atom specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_URI,
				g_param_spec_string ("uri",
					"URI", "An IRI reference that is relevant to the agent.",
					NULL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataGenerator:version:
	 *
	 * Indicates the version of the generating agent.
	 *
	 * For more information, see the
	 * <ulink type="http" url="http://www.atomenabled.org/developers/syndication/atom-format-spec.php#element.generator">Atom specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_VERSION,
				g_param_spec_string ("version",
					"Version", "Indicates the version of the generating agent.",
					NULL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_generator_init (GDataGenerator *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_GENERATOR, GDataGeneratorPrivate);
}

static void
gdata_generator_finalize (GObject *object)
{
	GDataGeneratorPrivate *priv = GDATA_GENERATOR (object)->priv;

	xmlFree (priv->name);
	xmlFree (priv->uri);
	xmlFree (priv->version);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_generator_parent_class)->finalize (object);
}

static void
gdata_generator_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataGeneratorPrivate *priv = GDATA_GENERATOR (object)->priv;

	switch (property_id) {
		case PROP_NAME:
			g_value_set_string (value, priv->name);
			break;
		case PROP_URI:
			g_value_set_string (value, priv->uri);
			break;
		case PROP_VERSION:
			g_value_set_string (value, priv->version);
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
	GDataGeneratorPrivate *priv = GDATA_GENERATOR (parsable)->priv;

	priv->uri = (gchar*) xmlGetProp (root_node, (xmlChar*) "uri");
	priv->version = (gchar*) xmlGetProp (root_node, (xmlChar*) "version");

	return TRUE;
}

static gboolean
parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error)
{
	xmlChar *name = xmlNodeListGetString (doc, node->children, TRUE);
	if (name != NULL && *name == '\0') {
		xmlFree (name);
		return gdata_parser_error_required_content_missing (node, error);
	}

	GDATA_GENERATOR (parsable)->priv->name = (gchar*) name;

	return TRUE;
}

/**
 * gdata_generator_compare:
 * @a: a #GDataGenerator, or %NULL
 * @b: another #GDataGenerator, or %NULL
 *
 * Compares the two generators in a strcmp() fashion. %NULL values are handled gracefully, with
 * %0 returned if both @a and @b are %NULL, %-1 if @a is %NULL and %1 if @b is %NULL.
 *
 * The comparison of non-%NULL values is done on the basis of the @name property of the #GDataGenerator<!-- -->s.
 *
 * Return value: %0 if @a equals @b, %-1 or %1 as appropriate otherwise
 *
 * Since: 0.4.0
 **/
gint
gdata_generator_compare (const GDataGenerator *a, const GDataGenerator *b)
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
 * gdata_generator_get_name:
 * @self: a #GDataGenerator
 *
 * Gets the #GDataGenerator:name property.
 *
 * Return value: the generator's name
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_generator_get_name (GDataGenerator *self)
{
	g_return_val_if_fail (GDATA_IS_GENERATOR (self), NULL);
	return self->priv->name;
}

/**
 * gdata_generator_get_uri:
 * @self: a #GDataGenerator
 *
 * Gets the #GDataGenerator:uri property.
 *
 * Return value: the generator's URI, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_generator_get_uri (GDataGenerator *self)
{
	g_return_val_if_fail (GDATA_IS_GENERATOR (self), NULL);
	return self->priv->uri;
}

/**
 * gdata_generator_get_version:
 * @self: a #GDataGenerator
 *
 * Gets the #GDataGenerator:version property.
 *
 * Return value: the generator's version, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_generator_get_version (GDataGenerator *self)
{
	g_return_val_if_fail (GDATA_IS_GENERATOR (self), NULL);
	return self->priv->version;
}
