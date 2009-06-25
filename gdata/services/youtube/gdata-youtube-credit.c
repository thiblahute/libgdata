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
 * SECTION:gdata-youtube-credit
 * @short_description: YouTube credit element
 * @stability: Unstable
 * @include: gdata/services/youtube/gdata-youtube-credit.h
 *
 * #GDataYouTubeCredit represents the YouTube-specific customizations to #GDataMediaCredit. For more information,
 * see the <ulink type="http" url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_media:credit">
 * online documentation</ulink>.
 **/

#include <glib.h>
#include <libxml/parser.h>

#include "gdata-youtube-credit.h"
#include "gdata-parsable.h"
#include "gdata-parser.h"

static void gdata_youtube_credit_finalize (GObject *object);
static void gdata_youtube_credit_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static gboolean pre_parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *root_node, gpointer user_data, GError **error);
static void pre_get_xml (GDataParsable *parsable, GString *xml_string);
static void get_namespaces (GDataParsable *parsable, GHashTable *namespaces);

struct _GDataYouTubeCreditPrivate {
	gchar *entity_type;
};

enum {
	PROP_ENTITY_TYPE = 1
};

G_DEFINE_TYPE (GDataYouTubeCredit, gdata_youtube_credit, GDATA_TYPE_MEDIA_CREDIT)
#define GDATA_YOUTUBE_CREDIT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_YOUTUBE_CREDIT, GDataYouTubeCreditPrivate))

static void
gdata_youtube_credit_class_init (GDataYouTubeCreditClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataYouTubeCreditPrivate));

	gobject_class->get_property = gdata_youtube_credit_get_property;
	gobject_class->finalize = gdata_youtube_credit_finalize;

	parsable_class->pre_parse_xml = pre_parse_xml;
	parsable_class->pre_get_xml = pre_get_xml;
	parsable_class->get_namespaces = get_namespaces;

	/**
	 * GDataYouTubeCredit:entity-type:
	 *
	 * The type of entity who is credited. Currently this can only be "partner", for a YouTube partner.
	 *
	 * For more information, see the
	 * <ulink type="http" url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_media:credit">
	 * YouTube documentation</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_ENTITY_TYPE,
				g_param_spec_string ("entity-type",
					"Entity type", "The type of entity who is credited.",
					NULL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_youtube_credit_init (GDataYouTubeCredit *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_YOUTUBE_CREDIT, GDataYouTubeCreditPrivate);
}

static void
gdata_youtube_credit_finalize (GObject *object)
{
	GDataYouTubeCreditPrivate *priv = GDATA_YOUTUBE_CREDIT (object)->priv;

	xmlFree (priv->entity_type);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_youtube_credit_parent_class)->finalize (object);
}

static void
gdata_youtube_credit_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataYouTubeCreditPrivate *priv = GDATA_YOUTUBE_CREDIT (object)->priv;

	switch (property_id) {
		case PROP_ENTITY_TYPE:
			g_value_set_string (value, priv->entity_type);
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
	/* Chain up to the parent class */
	GDATA_PARSABLE_CLASS (gdata_youtube_credit_parent_class)->pre_parse_xml (parsable, doc, root_node, user_data, error);

	GDATA_YOUTUBE_CREDIT (parsable)->priv->entity_type = (gchar*) xmlGetProp (root_node, (xmlChar*) "type");

	return TRUE;
}

static void
pre_get_xml (GDataParsable *parsable, GString *xml_string)
{
	GDataYouTubeCreditPrivate *priv = GDATA_YOUTUBE_CREDIT (parsable)->priv;

	/* Chain up to the parent class */
	GDATA_PARSABLE_CLASS (gdata_youtube_credit_parent_class)->pre_get_xml (parsable, xml_string);

	if (priv->entity_type != NULL)
		g_string_append_printf (xml_string, " type='%s'", priv->entity_type);
}

static void
get_namespaces (GDataParsable *parsable, GHashTable *namespaces)
{
	/* Chain up to the parent class */
	GDATA_PARSABLE_CLASS (gdata_youtube_credit_parent_class)->get_namespaces (parsable, namespaces);

	g_hash_table_insert (namespaces, (gchar*) "youtube", (gchar*) "http://gdata.youtube.com/schemas/2007");
}

/**
 * gdata_youtube_credit_get_entity_type:
 * @self: a #GDataYouTubeCredit
 *
 * Gets the #GDataYouTubeCredit:entity-type property.
 *
 * Return value: the type of the credited user (e.g. "partner"), or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_youtube_credit_get_entity_type (GDataYouTubeCredit *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_CREDIT (self), NULL);
	return self->priv->entity_type;
}
