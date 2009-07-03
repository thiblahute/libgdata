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
 * SECTION:gdata-media-credit
 * @short_description: Media RSS credit element
 * @stability: Unstable
 * @include: gdata/media/gdata-media-credit.h
 *
 * #GDataMediaCredit represents a "credit" element from the
 * <ulink type="http" url="http://video.search.yahoo.com/mrss">Media RSS specification</ulink>.
 **/

#include <glib.h>
#include <libxml/parser.h>

#include "gdata-media-credit.h"
#include "gdata-parsable.h"
#include "gdata-parser.h"
#include "gdata-types.h"

static void gdata_media_credit_finalize (GObject *object);
static void gdata_media_credit_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static gboolean pre_parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *root_node, gpointer user_data, GError **error);
static gboolean parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error);
static void get_namespaces (GDataParsable *parsable, GHashTable *namespaces);

struct _GDataMediaCreditPrivate {
	gchar *credit;
	gchar *scheme;
	gchar *role;
};

enum {
	PROP_CREDIT = 1,
	PROP_SCHEME,
	PROP_ROLE
};

G_DEFINE_TYPE (GDataMediaCredit, gdata_media_credit, GDATA_TYPE_PARSABLE)
#define GDATA_MEDIA_CREDIT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_MEDIA_CREDIT, GDataMediaCreditPrivate))

static void
gdata_media_credit_class_init (GDataMediaCreditClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataMediaCreditPrivate));

	gobject_class->get_property = gdata_media_credit_get_property;
	gobject_class->finalize = gdata_media_credit_finalize;

	parsable_class->pre_parse_xml = pre_parse_xml;
	parsable_class->parse_xml = parse_xml;
	parsable_class->get_namespaces = get_namespaces;
	parsable_class->element_name = "credit";
	parsable_class->element_namespace = "media";

	/**
	 * GDataMediaCredit:credit:
	 *
	 * The credited entity's name.
	 *
	 * For more information, see the <ulink type="http" url="http://video.search.yahoo.com/mrss">Media RSS specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_CREDIT,
				g_param_spec_string ("credit",
					"Credit", "The credited entity's name.",
					NULL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataMediaCredit:scheme:
	 *
	 * A URI that identifies the role scheme.
	 *
	 * For more information, see the <ulink type="http" url="http://video.search.yahoo.com/mrss">Media RSS specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_SCHEME,
				g_param_spec_string ("scheme",
					"Scheme", "A URI that identifies the role scheme.",
					NULL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataMediaCredit:role:
	 *
	 * The role the credited entity played in the production of the media.
	 *
	 * For more information, see the <ulink type="http" url="http://video.search.yahoo.com/mrss">Media RSS specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_ROLE,
				g_param_spec_string ("role",
					"Role", "The role the credited entity played in the production of the media.",
					NULL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_media_credit_init (GDataMediaCredit *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_MEDIA_CREDIT, GDataMediaCreditPrivate);
}

static void
gdata_media_credit_finalize (GObject *object)
{
	GDataMediaCreditPrivate *priv = GDATA_MEDIA_CREDIT (object)->priv;

	g_free (priv->credit);
	g_free (priv->scheme);
	g_free (priv->role);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_media_credit_parent_class)->finalize (object);
}

static void
gdata_media_credit_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataMediaCreditPrivate *priv = GDATA_MEDIA_CREDIT (object)->priv;

	switch (property_id) {
		case PROP_CREDIT:
			g_value_set_string (value, priv->credit);
			break;
		case PROP_SCHEME:
			g_value_set_string (value, priv->scheme);
			break;
		case PROP_ROLE:
			g_value_set_string (value, priv->role);
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
	GDataMediaCreditPrivate *priv = GDATA_MEDIA_CREDIT (parsable)->priv;
	xmlChar *credit, *scheme, *role;
	guint i;

	credit = xmlNodeListGetString (doc, root_node->children, TRUE);
	if (credit == NULL || *credit == '\0') {
		xmlFree (credit);
		return gdata_parser_error_required_content_missing (root_node, error);
	}

	scheme = xmlGetProp (root_node, (xmlChar*) "scheme");
	if (scheme != NULL && *scheme == '\0') {
		xmlFree (scheme);
		xmlFree (credit);
		return gdata_parser_error_required_property_missing (root_node, "scheme", error);
	} else if (scheme == NULL) {
		/* Default */
		scheme = xmlStrdup ((xmlChar*) "urn:ebu");
	}

	role = xmlGetProp (root_node, (xmlChar*) "role");

	priv->credit = g_strdup ((gchar*) credit);
	priv->scheme = g_strdup ((gchar*) scheme);
	priv->role = g_strdup ((gchar*) role);

	/* Convert the role to lower case */
	if (priv->role != NULL) {
		for (i = 0; priv->role[i] != '\0'; i++)
			priv->role[i] = g_ascii_tolower (priv->role[i]);
	}

	xmlFree (credit);
	xmlFree (scheme);
	xmlFree (role);

	return TRUE;
}

static gboolean
parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error)
{
	/* Textual content's handled in pre_parse_xml */
	if (node->type != XML_ELEMENT_NODE)
		return TRUE;

	if (GDATA_PARSABLE_CLASS (gdata_media_credit_parent_class)->parse_xml (parsable, doc, node, user_data, error) == FALSE) {
		/* Error! */
		return FALSE;
	}

	return TRUE;
}

static void
get_namespaces (GDataParsable *parsable, GHashTable *namespaces)
{
	g_hash_table_insert (namespaces, (gchar*) "media", (gchar*) "http://video.search.yahoo.com/mrss");
}

/**
 * gdata_media_credit_get_credit:
 * @self: a #GDataMediaCredit
 *
 * Gets the #GDataMediaCredit:credit property.
 *
 * Return value: the name of the credited entity
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_media_credit_get_credit (GDataMediaCredit *self)
{
	g_return_val_if_fail (GDATA_IS_MEDIA_CREDIT (self), NULL);
	return self->priv->credit;
}

/**
 * gdata_media_credit_get_scheme:
 * @self: a #GDataMediaCredit
 *
 * Gets the #GDataMediaCredit:scheme property.
 *
 * Return value: the credit's role scheme, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_media_credit_get_scheme (GDataMediaCredit *self)
{
	g_return_val_if_fail (GDATA_IS_MEDIA_CREDIT (self), NULL);
	return self->priv->scheme;
}

/**
 * gdata_media_credit_get_role:
 * @self: a #GDataMediaCredit
 *
 * Gets the #GDataMediaCredit:role property.
 *
 * Return value: the credited entity's role, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_media_credit_get_role (GDataMediaCredit *self)
{
	g_return_val_if_fail (GDATA_IS_MEDIA_CREDIT (self), NULL);
	return self->priv->role;
}
