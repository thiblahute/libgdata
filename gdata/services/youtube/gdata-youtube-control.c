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
 * SECTION:gdata-youtube-control
 * @short_description: Atom Publishing Protocol control element with YouTube-specific modifications
 * @stability: Unstable
 * @include: gdata/services/youtube/gdata-youtube-control.h
 *
 * #GDataYouTubeControl represents a "control" element from the
 * <ulink type="http" url="http://www.atomenabled.org/developers/protocol/#appControl">Atom Publishing Protocol specification</ulink>,
 * with support for the YouTube-specific <ulink type="http" url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_yt:state">
 * "state" element</ulink>.
 *
 * It is private API, since it would be unnecessary and confusing to expose #GDataYouTubeControl itself. There is the option
 * of breaking this up into #GDataAPPControl and extending #GDataYouTubeControl classes in the future, if more entry types implement
 * APP support.
 *
 * For these reasons, properties have not been implemented on #GDataYouTubeControl (yet).
 **/

#include <glib.h>
#include <libxml/parser.h>

#include "gdata-youtube-control.h"
#include "gdata-parsable.h"
#include "gdata-parser.h"
#include "gdata-private.h"
#include "gdata-youtube-state.h"

static void gdata_youtube_control_dispose (GObject *object);
static gboolean parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error);
static void get_xml (GDataParsable *parsable, GString *xml_string);
static void get_namespaces (GDataParsable *parsable, GHashTable *namespaces);

struct _GDataYouTubeControlPrivate {
	gboolean is_draft;
	GDataYouTubeState *state;
};

G_DEFINE_TYPE (GDataYouTubeControl, gdata_youtube_control, GDATA_TYPE_PARSABLE)
#define GDATA_YOUTUBE_CONTROL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_YOUTUBE_CONTROL, GDataYouTubeControlPrivate))

static void
gdata_youtube_control_class_init (GDataYouTubeControlClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataYouTubeControlPrivate));

	gobject_class->dispose = gdata_youtube_control_dispose;

	parsable_class->parse_xml = parse_xml;
	parsable_class->get_xml = get_xml;
	parsable_class->get_namespaces = get_namespaces;
	parsable_class->element_name = "control";
	parsable_class->element_namespace = "app";
}

static void
gdata_youtube_control_init (GDataYouTubeControl *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_YOUTUBE_CONTROL, GDataYouTubeControlPrivate);
}

static void
gdata_youtube_control_dispose (GObject *object)
{
	GDataYouTubeControlPrivate *priv = GDATA_YOUTUBE_CONTROL (object)->priv;

	if (priv->state != NULL)
		g_object_unref (priv->state);
	priv->state = NULL;

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_youtube_control_parent_class)->dispose (object);
}

static gboolean
parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error)
{
	GDataYouTubeControl *self = GDATA_YOUTUBE_CONTROL (parsable);

	if (xmlStrcmp (node->name, (xmlChar*) "draft") == 0) {
		/* app:draft */
		xmlChar *draft = xmlNodeListGetString (doc, node, TRUE);
		if (xmlStrcmp (draft, (xmlChar*) "no") == 0)
			self->priv->is_draft = FALSE;
		else
			self->priv->is_draft = TRUE;
		xmlFree (draft);
	} else if (xmlStrcmp (node->name, (xmlChar*) "state") == 0) {
		/* yt:state */
		GDataYouTubeState *state = GDATA_YOUTUBE_STATE (_gdata_parsable_new_from_xml_node (GDATA_TYPE_YOUTUBE_STATE, doc, node, NULL, error));
		if (state == NULL)
			return FALSE;

		if (self->priv->state != NULL) {
			g_object_unref (state);
			return gdata_parser_error_duplicate_element (node, error);
		}

		self->priv->state = state;
	} else if (GDATA_PARSABLE_CLASS (gdata_youtube_control_parent_class)->parse_xml (parsable, doc, node, user_data, error) == FALSE) {
		/* Error! */
		return FALSE;
	}

	return TRUE;
}

static void
get_xml (GDataParsable *parsable, GString *xml_string)
{
	GDataYouTubeControlPrivate *priv = GDATA_YOUTUBE_CONTROL (parsable)->priv;

	if (priv->is_draft == TRUE)
		g_string_append (xml_string, "<app:draft>yes</app:draft>");
	else
		g_string_append (xml_string, "<app:draft>no</app:draft>");
}

static void
get_namespaces (GDataParsable *parsable, GHashTable *namespaces)
{
	g_hash_table_insert (namespaces, (gchar*) "app", (gchar*) "http://www.w3.org/2007/app");
}

/**
 * gdata_youtube_control_is_draft:
 * @self: a #GDataYouTubeControl
 *
 * Gets the #GDataYouTubeControl:is-draft property.
 *
 * Return value: %TRUE if the object is a draft, %FALSE otherwise
 **/
gboolean
gdata_youtube_control_is_draft (GDataYouTubeControl *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_CONTROL (self), FALSE);
	return self->priv->is_draft;
}

/**
 * gdata_youtube_control_set_is_draft:
 * @self: a #GDataYouTubeControl
 * @is_draft: whether the object is a draft
 *
 * Sets the #GDataYouTubeControl:is-draft property to decide whether the object is a draft.
 **/
void
gdata_youtube_control_set_is_draft (GDataYouTubeControl *self, gboolean is_draft)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_CONTROL (self));
	self->priv->is_draft = is_draft;
}

/**
 * gdata_youtube_control_get_state:
 * @self: a #GDataYouTubeControl
 *
 * Gets the #GDataYouTubeControl:state property.
 *
 * For more information, see the <ulink type="http"
 * url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_yt:state">online documentation</ulink>.
 *
 * Return value: a #GDataYouTubeState showing the state of the video, or %NULL
 **/
GDataYouTubeState *
gdata_youtube_control_get_state (GDataYouTubeControl *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_CONTROL (self), NULL);
	return self->priv->state;
}
