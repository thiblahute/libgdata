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
 * SECTION:gdata-youtube-content
 * @short_description: YouTube content element
 * @stability: Unstable
 * @include: gdata/services/youtube/gdata-youtube-content.h
 *
 * #GDataYouTubeContent represents the YouTube-specific customizations to #GDataMediaContent. For more information,
 * see the <ulink type="http" url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_media:content">
 * online documentation</ulink>.
 **/

#include <glib.h>
#include <libxml/parser.h>

#include "gdata-youtube-content.h"
#include "gdata-parsable.h"
#include "gdata-parser.h"
#include "gdata-youtube-enums.h"

static void gdata_youtube_content_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static gboolean pre_parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *root_node, gpointer user_data, GError **error);
static void get_namespaces (GDataParsable *parsable, GHashTable *namespaces);

struct _GDataYouTubeContentPrivate {
	GDataYouTubeFormat format;
};

enum {
	PROP_FORMAT = 1
};

G_DEFINE_TYPE (GDataYouTubeContent, gdata_youtube_content, GDATA_TYPE_MEDIA_CONTENT)
#define GDATA_YOUTUBE_CONTENT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_YOUTUBE_CONTENT, GDataYouTubeContentPrivate))

static void
gdata_youtube_content_class_init (GDataYouTubeContentClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataYouTubeContentPrivate));

	gobject_class->get_property = gdata_youtube_content_get_property;

	parsable_class->pre_parse_xml = pre_parse_xml;
	parsable_class->get_namespaces = get_namespaces;

	/**
	 * GDataYouTubeContent:format:
	 *
	 * The video format of the video object.
	 *
	 * For more information, see the
	 * <ulink type="http" url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_media:content">
	 * YouTube documentation</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_FORMAT,
				g_param_spec_enum ("format",
					"Format", "The video format of the video object.",
					GDATA_TYPE_YOUTUBE_FORMAT, GDATA_YOUTUBE_FORMAT_UNKNOWN,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_youtube_content_init (GDataYouTubeContent *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_YOUTUBE_CONTENT, GDataYouTubeContentPrivate);
}

static void
gdata_youtube_content_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataYouTubeContentPrivate *priv = GDATA_YOUTUBE_CONTENT (object)->priv;

	switch (property_id) {
		case PROP_FORMAT:
			g_value_set_enum (value, priv->format);
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
	xmlChar *format;

	/* Chain up to the parent class */
	GDATA_PARSABLE_CLASS (gdata_youtube_content_parent_class)->pre_parse_xml (parsable, doc, root_node, user_data, error);

	format = xmlGetProp (root_node, (xmlChar*) "format");
	GDATA_YOUTUBE_CONTENT (parsable)->priv->format = (format == NULL) ? GDATA_YOUTUBE_FORMAT_UNKNOWN : strtoul ((gchar*) format, NULL, 10);

	return TRUE;
}

static void
get_namespaces (GDataParsable *parsable, GHashTable *namespaces)
{
	/* Chain up to the parent class */
	GDATA_PARSABLE_CLASS (gdata_youtube_content_parent_class)->get_namespaces (parsable, namespaces);

	g_hash_table_insert (namespaces, (gchar*) "youtube", (gchar*) "http://gdata.youtube.com/schemas/2007");
}

/**
 * gdata_youtube_content_get_format:
 * @self: a #GDataYouTubeContent
 *
 * Gets the #GDataYouTubeContent:format property.
 *
 * Return value: the video format, or %GDATA_YOUTUBE_FORMAT_UNKNOWN
 *
 * Since: 0.4.0
 **/
GDataYouTubeFormat
gdata_youtube_content_get_format (GDataYouTubeContent *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_CONTENT (self), GDATA_YOUTUBE_FORMAT_UNKNOWN);
	return self->priv->format;
}
