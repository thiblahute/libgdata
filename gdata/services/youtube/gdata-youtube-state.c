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
 * SECTION:gdata-youtube-state
 * @short_description: YouTube state element
 * @stability: Unstable
 * @include: gdata/services/youtube/gdata-youtube-state.h
 *
 * #GDataYouTubeState represents a "state" element from the
 * <ulink type="http" url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_yt:state">YouTube namespace</ulink>.
 **/

#include <glib.h>
#include <libxml/parser.h>

#include "gdata-youtube-state.h"
#include "gdata-parsable.h"
#include "gdata-parser.h"

static void gdata_youtube_state_finalize (GObject *object);
static void gdata_youtube_state_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static gboolean pre_parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *root_node, gpointer user_data, GError **error);
static gboolean parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error);
static void get_namespaces (GDataParsable *parsable, GHashTable *namespaces);

struct _GDataYouTubeStatePrivate {
	gchar *name;
	gchar *reason_code;
	gchar *help_uri;
	gchar *message;
};

enum {
	PROP_NAME = 1,
	PROP_REASON_CODE,
	PROP_HELP_URI,
	PROP_MESSAGE
};

G_DEFINE_TYPE (GDataYouTubeState, gdata_youtube_state, GDATA_TYPE_PARSABLE)
#define GDATA_YOUTUBE_STATE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_YOUTUBE_STATE, GDataYouTubeStatePrivate))

static void
gdata_youtube_state_class_init (GDataYouTubeStateClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataYouTubeStatePrivate));

	gobject_class->get_property = gdata_youtube_state_get_property;
	gobject_class->finalize = gdata_youtube_state_finalize;

	parsable_class->pre_parse_xml = pre_parse_xml;
	parsable_class->parse_xml = parse_xml;
	parsable_class->get_namespaces = get_namespaces;
	parsable_class->element_name = "state";
	parsable_class->element_namespace = "yt";

	/**
	 * GDataYouTubeState:name:
	 *
	 * The name of the status of the unpublished video. Valid values are: "processing", "restricted", "deleted", "rejected" and "failed".
	 *
	 * For more information, see the <ulink type="http" url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_yt:state">
	 * online documentation</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_NAME,
				g_param_spec_string ("name",
					"Name", "The name of the status of the unpublished video.",
					NULL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataYouTubeState:reason-code:
	 *
	 * The reason code explaining why the video failed to upload.
	 *
	 * For more information, see the <ulink type="http" url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_yt:state">
	 * online documentation</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_REASON_CODE,
				g_param_spec_string ("reason-code",
					"Reason code", "The reason code explaining why the video failed to upload.",
					NULL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataYouTubeState:help-uri:
	 *
	 * A URI for a YouTube Help Center page that may help the developer or the video owner to diagnose
	 * the reason that an upload failed or was rejected.
	 *
	 * For more information, see the <ulink type="http" url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_yt:state">
	 * online documentation</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_HELP_URI,
				g_param_spec_string ("help-uri",
					"Help URI", "A URI for a YouTube Help Center page.",
					NULL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataYouTubeState:message:
	 *
	 * A human-readable description of why the video failed to upload.
	 *
	 * For more information, see the <ulink type="http" url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_yt:state">
	 * online documentation</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_MESSAGE,
				g_param_spec_string ("message",
					"Message", "A human-readable description of why the video failed to upload.",
					NULL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_youtube_state_init (GDataYouTubeState *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_YOUTUBE_STATE, GDataYouTubeStatePrivate);
}

static void
gdata_youtube_state_finalize (GObject *object)
{
	GDataYouTubeStatePrivate *priv = GDATA_YOUTUBE_STATE (object)->priv;

	xmlFree (priv->name);
	xmlFree (priv->reason_code);
	xmlFree (priv->help_uri);
	xmlFree (priv->message);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_youtube_state_parent_class)->finalize (object);
}

static void
gdata_youtube_state_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataYouTubeStatePrivate *priv = GDATA_YOUTUBE_STATE (object)->priv;

	switch (property_id) {
		case PROP_NAME:
			g_value_set_string (value, priv->name);
			break;
		case PROP_REASON_CODE:
			g_value_set_string (value, priv->reason_code);
			break;
		case PROP_HELP_URI:
			g_value_set_string (value, priv->help_uri);
			break;
		case PROP_MESSAGE:
			g_value_set_string (value, priv->message);
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
	GDataYouTubeStatePrivate *priv = GDATA_YOUTUBE_STATE (parsable)->priv;
	xmlChar *name, *message, *reason_code, *help_uri;

	name = xmlGetProp (root_node, (xmlChar*) "name");
	if (name == NULL || *name == '\0') {
		xmlFree (name);
		return gdata_parser_error_required_property_missing (root_node, "name", error);
	}

	message = xmlNodeListGetString (doc, root_node->children, TRUE);
	reason_code = xmlGetProp (root_node, (xmlChar*) "reasonCode");
	help_uri = xmlGetProp (root_node, (xmlChar*) "helpUrl");

	priv->name = (gchar*) name;
	priv->reason_code = (gchar*) reason_code;
	priv->help_uri = (gchar*) help_uri;
	priv->message = (gchar*) message;

	return TRUE;
}

static gboolean
parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error)
{
	/* Textual content's handled in pre_parse_xml */
	if (node->type != XML_ELEMENT_NODE)
		return TRUE;

	if (GDATA_PARSABLE_CLASS (gdata_youtube_state_parent_class)->parse_xml (parsable, doc, node, user_data, error) == FALSE) {
		/* Error! */
		return FALSE;
	}

	return TRUE;
}

static void
get_namespaces (GDataParsable *parsable, GHashTable *namespaces)
{
	g_hash_table_insert (namespaces, (gchar*) "yt", (gchar*) "http://gdata.youtube.com/schemas/2007");
}

/**
 * gdata_youtube_state_get_name:
 * @self: a #GDataYouTubeState
 *
 * Gets the #GDataYouTubeState:name property.
 *
 * Return value: the status name
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_youtube_state_get_name (GDataYouTubeState *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_STATE (self), NULL);
	return self->priv->name;
}

/**
 * gdata_youtube_state_get_reason_code:
 * @self: a #GDataYouTubeState
 *
 * Gets the #GDataYouTubeState:reason-code property.
 *
 * Return value: the status reason code, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_youtube_state_get_reason_code (GDataYouTubeState *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_STATE (self), NULL);
	return self->priv->reason_code;
}

/**
 * gdata_youtube_state_get_help_uri:
 * @self: a #GDataYouTubeState
 *
 * Gets the #GDataYouTubeState:help-uri property.
 *
 * Return value: the help URI, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_youtube_state_get_help_uri (GDataYouTubeState *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_STATE (self), NULL);
	return self->priv->help_uri;
}

/**
 * gdata_youtube_state_get_message:
 * @self: a #GDataYouTubeState
 *
 * Gets the #GDataYouTubeState:message property.
 *
 * Return value: the status message, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_youtube_state_get_message (GDataYouTubeState *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_STATE (self), NULL);
	return self->priv->message;
}
