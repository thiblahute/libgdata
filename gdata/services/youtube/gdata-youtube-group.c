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
 * SECTION:gdata-youtube-group
 * @short_description: YouTube group element
 * @stability: Unstable
 * @include: gdata/services/youtube/gdata-youtube-group.h
 *
 * #GDataYouTubeGroup represents the YouTube-specific customizations to #GDataMediaGroup. For more information,
 * see the <ulink type="http" url="http://code.google.com/apis/youtube/2.0/reference.html#youtube_data_api_tag_media:group">
 * online documentation</ulink>.
 **/

#include <glib.h>
#include <libxml/parser.h>

#include "gdata-youtube-group.h"
#include "gdata-parsable.h"
#include "gdata-parser.h"
#include "gdata-private.h"
#include "gdata-youtube-enums.h"
#include "gdata-youtube-content.h"
#include "gdata-youtube-credit.h"

static void gdata_youtube_group_finalize (GObject *object);
static gboolean parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error);
static void get_xml (GDataParsable *parsable, GString *xml_string);
static void get_namespaces (GDataParsable *parsable, GHashTable *namespaces);

struct _GDataYouTubeGroupPrivate {
	guint duration;
	gboolean is_private;
	GTimeVal uploaded;
	gchar *video_id;
};

G_DEFINE_TYPE (GDataYouTubeGroup, gdata_youtube_group, GDATA_TYPE_MEDIA_GROUP)
#define GDATA_YOUTUBE_GROUP_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_YOUTUBE_GROUP, GDataYouTubeGroupPrivate))

static void
gdata_youtube_group_class_init (GDataYouTubeGroupClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataYouTubeGroupPrivate));

	gobject_class->finalize = gdata_youtube_group_finalize;

	parsable_class->parse_xml = parse_xml;
	parsable_class->get_xml = get_xml;
	parsable_class->get_namespaces = get_namespaces;
}

static void
gdata_youtube_group_init (GDataYouTubeGroup *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_YOUTUBE_GROUP, GDataYouTubeGroupPrivate);
}

static void
gdata_youtube_group_finalize (GObject *object)
{
	GDataYouTubeGroupPrivate *priv = GDATA_YOUTUBE_GROUP (object)->priv;

	xmlFree (priv->video_id);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_youtube_group_parent_class)->finalize (object);
}

static gboolean
parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error)
{
	GDataYouTubeGroup *self = GDATA_YOUTUBE_GROUP (parsable);

	if (xmlStrcmp (node->name, (xmlChar*) "content") == 0) {
		/* media:content */
		GDataYouTubeContent *content = GDATA_YOUTUBE_CONTENT (_gdata_parsable_new_from_xml_node (GDATA_TYPE_YOUTUBE_CONTENT, doc,
													 node, NULL, error));
		if (content == NULL)
			return FALSE;

		_gdata_media_group_add_content (GDATA_MEDIA_GROUP (self), GDATA_MEDIA_CONTENT (content));
	} else if (xmlStrcmp (node->name, (xmlChar*) "credit") == 0) {
		/* media:credit */
		GDataYouTubeCredit *credit = GDATA_YOUTUBE_CREDIT (_gdata_parsable_new_from_xml_node (GDATA_TYPE_YOUTUBE_CREDIT, doc,
												      node, NULL, error));
		if (credit == NULL)
			return FALSE;

		if (gdata_media_group_get_credit (GDATA_MEDIA_GROUP (self)) != NULL) {
			g_object_unref (credit);
			return gdata_parser_error_duplicate_element (node, error);
		}

		_gdata_media_group_set_credit (GDATA_MEDIA_GROUP (self), GDATA_MEDIA_CREDIT (credit));
	} else if (xmlStrcmp (node->name, (xmlChar*) "duration") == 0) {
		/* yt:duration */
		xmlChar *duration = xmlGetProp (node, (xmlChar*) "seconds");
		if (duration == NULL)
			return gdata_parser_error_required_property_missing (node, "seconds", error);

		self->priv->duration = strtoul ((gchar*) duration, NULL, 10);
		xmlFree (duration);
	} else if (xmlStrcmp (node->name, (xmlChar*) "private") == 0) {
		/* yt:private */
		gdata_youtube_group_set_is_private (self, TRUE);
	} else if (xmlStrcmp (node->name, (xmlChar*) "uploaded") == 0) {
		/* yt:uploaded */
		xmlChar *uploaded;
		GTimeVal uploaded_timeval;

		uploaded = xmlNodeListGetString (doc, node->children, TRUE);
		if (g_time_val_from_iso8601 ((gchar*) uploaded, &uploaded_timeval) == FALSE) {
			/* Error */
			gdata_parser_error_not_iso8601_format (node, (gchar*) uploaded, error);
			xmlFree (uploaded);
			return FALSE;
		}
		xmlFree (uploaded);

		self->priv->uploaded = uploaded_timeval;
	} else if (xmlStrcmp (node->name, (xmlChar*) "videoid") == 0) {
		/* yt:videoid */
		xmlChar *video_id = xmlNodeListGetString (doc, node->children, TRUE);
		if (self->priv->video_id != NULL) {
			xmlFree (video_id);
			return gdata_parser_error_duplicate_element (node, error);
		}
		self->priv->video_id = (gchar*) video_id;
	} else if (GDATA_PARSABLE_CLASS (gdata_youtube_group_parent_class)->parse_xml (parsable, doc, node, user_data, error) == FALSE) {
		/* Error! */
		return FALSE;
	}

	return TRUE;
}

static void
get_xml (GDataParsable *parsable, GString *xml_string)
{
	GDataYouTubeGroupPrivate *priv = GDATA_YOUTUBE_GROUP (parsable)->priv;

	/* Chain up to the parent class */
	GDATA_PARSABLE_CLASS (gdata_youtube_group_parent_class)->get_xml (parsable, xml_string);

	if (priv->is_private == TRUE)
		g_string_append (xml_string, "<yt:private/>");
}

static void
get_namespaces (GDataParsable *parsable, GHashTable *namespaces)
{
	/* Chain up to the parent class */
	GDATA_PARSABLE_CLASS (gdata_youtube_group_parent_class)->get_namespaces (parsable, namespaces);

	g_hash_table_insert (namespaces, (gchar*) "yt", (gchar*) "http://gdata.youtube.com/schemas/2007");
}

/**
 * gdata_youtube_group_get_duration:
 * @self: a #GDataYouTubeGroup
 *
 * Gets the #GDataYouTubeGroup:duration property.
 *
 * Return value: the video duration in seconds, or %0 if unknown
 **/
guint
gdata_youtube_group_get_duration (GDataYouTubeGroup *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_GROUP (self), 0);
	return self->priv->duration;
}

/**
 * gdata_youtube_group_is_private:
 * @self: a #GDataYouTubeGroup
 *
 * Gets the #GDataYouTubeGroup:is-private property.
 *
 * Return value: %TRUE if the video is private, %FALSE otherwise
 **/
gboolean
gdata_youtube_group_is_private (GDataYouTubeGroup *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_GROUP (self), FALSE);
	return self->priv->is_private;
}

/**
 * gdata_youtube_group_set_is_private:
 * @self: a #GDataYouTubeGroup
 * @is_private: whether the video is private
 *
 * Sets the #GDataYouTubeGroup:is-private property to decide whether the video is publicly viewable.
 **/
void
gdata_youtube_group_set_is_private (GDataYouTubeGroup *self, gboolean is_private)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_GROUP (self));
	self->priv->is_private = is_private;
}

/**
 * gdata_youtube_group_get_uploaded:
 * @self: a #GDataYouTubeGroup
 * @uploaded: a #GTimeVal
 *
 * Gets the #GDataYouTubeGroup:uploaded property and puts it in @uploaded. If the property is unset,
 * both fields in the #GTimeVal will be set to %0.
 **/
void
gdata_youtube_group_get_uploaded (GDataYouTubeGroup *self, GTimeVal *uploaded)
{
	g_return_if_fail (GDATA_IS_YOUTUBE_GROUP (self));
	*uploaded = self->priv->uploaded;
}

/**
 * gdata_youtube_group_get_video_id:
 * @self: a #GDataYouTubeGroup
 *
 * Gets the #GDataYouTubeGroup:video-id property.
 *
 * Return value: the video's unique and permanent ID
 **/
const gchar *
gdata_youtube_group_get_video_id (GDataYouTubeGroup *self)
{
	g_return_val_if_fail (GDATA_IS_YOUTUBE_GROUP (self), NULL);
	return self->priv->video_id;
}
