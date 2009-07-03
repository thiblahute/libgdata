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
 * SECTION:gdata-media-group
 * @short_description: Media RSS group element
 * @stability: Unstable
 * @include: gdata/media/gdata-media-group.h
 *
 * #GDataMediaGroup represents a "group" element from the
 * <ulink type="http" url="http://video.search.yahoo.com/mrss">Media RSS specification</ulink>.
 *
 * It is private API, since implementing classes are likely to proxy the properties and functions
 * of #GDataMediaGroup as appropriate; most entry types which implement #GDataMediaGroup have no use
 * for most of its properties, and it would be unnecessary and confusing to expose #GDataMediaGroup itself.
 *
 * For this reason, properties have not been implemented on #GDataMediaGroup (yet).
 **/

#include <glib.h>
#include <libxml/parser.h>
#include <string.h>

#include "gdata-media-group.h"
#include "gdata-parsable.h"
#include "gdata-parser.h"
#include "gdata-private.h"
#include "media/gdata-media-category.h"
#include "media/gdata-media-credit.h"
#include "media/gdata-media-thumbnail.h"

static void gdata_media_group_dispose (GObject *object);
static void gdata_media_group_finalize (GObject *object);
static gboolean parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error);
static void get_xml (GDataParsable *parsable, GString *xml_string);
static void get_namespaces (GDataParsable *parsable, GHashTable *namespaces);

struct _GDataMediaGroupPrivate {
	gchar *keywords;
	gchar *player_uri;
	GHashTable *restricted_countries;
	GList *thumbnails; /* GDataMediaThumbnail */
	gchar *title;
	GDataMediaCategory *category;
	GList *contents; /* GDataMediaContent */
	GDataMediaCredit *credit;
	gchar *description;
};

G_DEFINE_TYPE (GDataMediaGroup, gdata_media_group, GDATA_TYPE_PARSABLE)
#define GDATA_MEDIA_GROUP_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_MEDIA_GROUP, GDataMediaGroupPrivate))

static void
gdata_media_group_class_init (GDataMediaGroupClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataMediaGroupPrivate));

	gobject_class->dispose = gdata_media_group_dispose;
	gobject_class->finalize = gdata_media_group_finalize;

	parsable_class->parse_xml = parse_xml;
	parsable_class->get_xml = get_xml;
	parsable_class->get_namespaces = get_namespaces;
	parsable_class->element_name = "group";
	parsable_class->element_namespace = "media";
}

static void
gdata_media_group_init (GDataMediaGroup *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_MEDIA_GROUP, GDataMediaGroupPrivate);
	self->priv->restricted_countries = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
}

static void
gdata_media_group_dispose (GObject *object)
{
	GDataMediaGroupPrivate *priv = GDATA_MEDIA_GROUP (object)->priv;

	if (priv->category != NULL)
		g_object_unref (priv->category);
	priv->category = NULL;

	if (priv->credit != NULL)
		g_object_unref (priv->credit);
	priv->credit = NULL;

	if (priv->contents != NULL) {
		g_list_foreach (priv->contents, (GFunc) g_object_unref, NULL);
		g_list_free (priv->contents);
	}
	priv->contents = NULL;

	if (priv->thumbnails != NULL) {
		g_list_foreach (priv->thumbnails, (GFunc) g_object_unref, NULL);
		g_list_free (priv->thumbnails);
	}
	priv->thumbnails = NULL;

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_media_group_parent_class)->dispose (object);
}

static void
gdata_media_group_finalize (GObject *object)
{
	GDataMediaGroupPrivate *priv = GDATA_MEDIA_GROUP (object)->priv;

	g_free (priv->keywords);
	g_free (priv->player_uri);
	g_hash_table_destroy (priv->restricted_countries);
	g_free (priv->title);
	g_free (priv->description);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_media_group_parent_class)->finalize (object);
}

static gboolean
parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error)
{
	GDataMediaGroup *self = GDATA_MEDIA_GROUP (parsable);

	if (xmlStrcmp (node->name, (xmlChar*) "title") == 0) {
		/* media:title */
		xmlChar *title = xmlNodeListGetString (doc, node->children, TRUE);
		gdata_media_group_set_title (self, (gchar*) title);
		xmlFree (title);
	} else if (xmlStrcmp (node->name, (xmlChar*) "description") == 0) {
		/* media:description */
		xmlChar *description = xmlNodeListGetString (doc, node->children, TRUE);
		gdata_media_group_set_description (self, (gchar*) description);
		xmlFree (description);
	} else if (xmlStrcmp (node->name, (xmlChar*) "keywords") == 0) {
		/* media:keywords */
		xmlChar *keywords = xmlNodeListGetString (doc, node->children, TRUE);
		gdata_media_group_set_keywords (self, (gchar*) keywords);
		xmlFree (keywords);
	} else if (xmlStrcmp (node->name, (xmlChar*) "category") == 0) {
		/* media:category */
		GDataMediaCategory *category = GDATA_MEDIA_CATEGORY (_gdata_parsable_new_from_xml_node (GDATA_TYPE_MEDIA_CATEGORY, doc,
													node, NULL, error));
		if (category == NULL)
			return FALSE;

		gdata_media_group_set_category (self, category);
	} else if (xmlStrcmp (node->name, (xmlChar*) "content") == 0) {
		/* media:content */
		GDataMediaContent *content = GDATA_MEDIA_CONTENT (_gdata_parsable_new_from_xml_node (GDATA_TYPE_MEDIA_CONTENT, doc, node, NULL, error));
		if (content == NULL)
			return FALSE;

		_gdata_media_group_add_content (self, content);
	} else if (xmlStrcmp (node->name, (xmlChar*) "credit") == 0) {
		/* media:credit */
		GDataMediaCredit *credit = GDATA_MEDIA_CREDIT (_gdata_parsable_new_from_xml_node (GDATA_TYPE_MEDIA_CREDIT, doc, node, NULL, error));
		if (credit == NULL)
			return FALSE;

		if (self->priv->credit != NULL) {
			g_object_unref (credit);
			return gdata_parser_error_duplicate_element (node, error);
		}

		_gdata_media_group_set_credit (self, credit);
	} else if (xmlStrcmp (node->name, (xmlChar*) "player") == 0) {
		/* media:player */
		xmlChar *player_uri = xmlGetProp (node, (xmlChar*) "url");

		g_free (self->priv->player_uri);
		self->priv->player_uri = g_strdup ((gchar*) player_uri);

		xmlFree (player_uri);
	} else if (xmlStrcmp (node->name, (xmlChar*) "rating") == 0) {
		/* media:rating */
		xmlChar *countries;
		gchar **country_list, **country;

		countries = xmlGetProp (node, (xmlChar*) "country");
		country_list = g_strsplit ((const gchar*) countries, ",", -1);
		xmlFree (countries);

		/* Add all the listed countries to the restricted countries table */
		for (country = country_list; *country != NULL; country++)
			g_hash_table_insert (self->priv->restricted_countries, *country, GUINT_TO_POINTER (TRUE));
		g_free (country_list);
	} else if (xmlStrcmp (node->name, (xmlChar*) "restriction") == 0) {
		/* media:restriction */
		xmlChar *type, *countries, *relationship;
		gchar **country_list, **country;
		gboolean relationship_bool;

		/* Check the type property is "country" */
		type = xmlGetProp (node, (xmlChar*) "type");
		if (xmlStrcmp (type, (xmlChar*) "country") != 0) {
			gdata_parser_error_unknown_property_value (node, "type", (gchar*) type, error);
			xmlFree (type);
			return FALSE;
		}
		xmlFree (type);

		relationship = xmlGetProp (node, (xmlChar*) "relationship");
		if (xmlStrcmp (relationship, (xmlChar*) "allow") == 0)
			relationship_bool = FALSE; /* it's *not* a restricted country */
		else if (xmlStrcmp (relationship, (xmlChar*) "deny") == 0)
			relationship_bool = TRUE; /* it *is* a restricted country */
		else {
			gdata_parser_error_unknown_property_value (node, "relationship", (gchar*) relationship, error);
			xmlFree (relationship);
			return FALSE;
		}
		xmlFree (relationship);

		countries = xmlNodeListGetString (doc, node->children, TRUE);
		country_list = g_strsplit ((const gchar*) countries, " ", -1);
		xmlFree (countries);

		/* Add "all" to the table, since it's an exception table */
		g_hash_table_insert (self->priv->restricted_countries, g_strdup ("all"), GUINT_TO_POINTER (!relationship_bool));

		/* Add all the listed countries to the restricted countries table */
		for (country = country_list; *country != NULL; country++)
			g_hash_table_insert (self->priv->restricted_countries, *country, GUINT_TO_POINTER (relationship_bool));
		g_free (country_list);
	} else if (xmlStrcmp (node->name, (xmlChar*) "thumbnail") == 0) {
		/* media:thumbnail */
		GDataMediaThumbnail *thumb = GDATA_MEDIA_THUMBNAIL (_gdata_parsable_new_from_xml_node (GDATA_TYPE_MEDIA_THUMBNAIL, doc,
												       node, NULL, error));
		if (thumb == NULL)
			return FALSE;

		self->priv->thumbnails = g_list_prepend (self->priv->thumbnails, thumb);
	} else if (GDATA_PARSABLE_CLASS (gdata_media_group_parent_class)->parse_xml (parsable, doc, node, user_data, error) == FALSE) {
		/* Error! */
		return FALSE;
	}

	return TRUE;
}

static void
get_xml (GDataParsable *parsable, GString *xml_string)
{
	GDataMediaGroupPrivate *priv = GDATA_MEDIA_GROUP (parsable)->priv;

	/* Media category */
	if (priv->category != NULL) {
		gchar *xml = _gdata_parsable_get_xml (GDATA_PARSABLE (priv->category), FALSE);
		g_string_append (xml_string, xml);
		g_free (xml);
	}

	if (priv->title != NULL) {
		gchar *title = g_markup_escape_text (priv->title, -1);
		g_string_append_printf (xml_string, "<media:title type='plain'>%s</media:title>", title);
		g_free (title);
	}

	if (priv->description != NULL) {
		gchar *description = g_markup_escape_text (priv->description, -1);
		g_string_append_printf (xml_string, "<media:description type='plain'>%s</media:description>", description);
		g_free (description);
	}

	if (priv->keywords != NULL) {
		gchar *keywords = g_markup_escape_text (priv->keywords, -1);
		g_string_append_printf (xml_string, "<media:keywords>%s</media:keywords>", keywords);
		g_free (keywords);
	}
}

static void
get_namespaces (GDataParsable *parsable, GHashTable *namespaces)
{
	g_hash_table_insert (namespaces, (gchar*) "media", (gchar*) "http://video.search.yahoo.com/mrss");
}

/**
 * gdata_media_group_get_title:
 * @self: a #GDataMediaGroup
 *
 * Gets the #GDataMediaGroup:title property.
 *
 * Return value: the group's title, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_media_group_get_title (GDataMediaGroup *self)
{
	g_return_val_if_fail (GDATA_IS_MEDIA_GROUP (self), NULL);
	return self->priv->title;
}

/**
 * gdata_media_group_set_title:
 * @self: a #GDataMediaGroup
 * @title: the group's new title, or %NULL
 *
 * Sets the #GDataMediaGroup:title property to @title.
 *
 * Set @title to %NULL to unset the property.
 *
 * Since: 0.4.0
 **/
void
gdata_media_group_set_title (GDataMediaGroup *self, const gchar *title)
{
	g_return_if_fail (GDATA_IS_MEDIA_GROUP (self));
	g_free (self->priv->title);
	self->priv->title = g_strdup (title);
}

/**
 * gdata_media_group_get_description:
 * @self: a #GDataMediaGroup
 *
 * Gets the #GDataMediaGroup:description property.
 *
 * Return value: the group's description, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_media_group_get_description (GDataMediaGroup *self)
{
	g_return_val_if_fail (GDATA_IS_MEDIA_GROUP (self), NULL);
	return self->priv->description;
}

/**
 * gdata_media_group_set_description:
 * @self: a #GDataMediaGroup
 * @description: the group's new description, or %NULL
 *
 * Sets the #GDataMediaGroup:description property to @description.
 *
 * Set @description to %NULL to unset the property.
 *
 * Since: 0.4.0
 **/
void
gdata_media_group_set_description (GDataMediaGroup *self, const gchar *description)
{
	g_return_if_fail (GDATA_IS_MEDIA_GROUP (self));
	g_free (self->priv->description);
	self->priv->description = g_strdup (description);
}

/**
 * gdata_media_group_get_keywords:
 * @self: a #GDataMediaGroup
 *
 * Gets the #GDataMediaGroup:keywords property.
 *
 * Return value: the group's keywords, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_media_group_get_keywords (GDataMediaGroup *self)
{
	g_return_val_if_fail (GDATA_IS_MEDIA_GROUP (self), NULL);
	return self->priv->keywords;
}

/**
 * gdata_media_group_set_keywords:
 * @self: a #GDataMediaGroup
 * @keywords: the group's new keywords, or %NULL
 *
 * Sets the #GDataMediaGroup:keywords property to @keywords.
 *
 * Set @keywords to %NULL to unset the property.
 *
 * Since: 0.4.0
 **/
void
gdata_media_group_set_keywords (GDataMediaGroup *self, const gchar *keywords)
{
	g_return_if_fail (GDATA_IS_MEDIA_GROUP (self));
	g_free (self->priv->keywords);
	self->priv->keywords = g_strdup (keywords);
}

/**
 * gdata_media_group_get_category:
 * @self: a #GDataMediaGroup
 *
 * Gets the #GDataMediaGroup:category property.
 *
 * Return value: a #GDataMediaCategory giving the group's category, or %NULL
 **/
GDataMediaCategory *
gdata_media_group_get_category (GDataMediaGroup *self)
{
	g_return_val_if_fail (GDATA_IS_MEDIA_GROUP (self), NULL);
	return self->priv->category;
}

/**
 * gdata_media_group_set_category:
 * @self: a #GDataMediaGroup
 * @category: a new #GDataMediaCategory, or %NULL
 *
 * Sets the #GDataMediaGroup:category property to @category, and increments its reference count.
 **/
void
gdata_media_group_set_category (GDataMediaGroup *self, GDataMediaCategory *category)
{
	g_return_if_fail (GDATA_IS_MEDIA_GROUP (self));
	g_return_if_fail (category == NULL || GDATA_IS_MEDIA_CATEGORY (category));

	if (self->priv->category != NULL)
		g_object_unref (self->priv->category);
	self->priv->category = (category == NULL) ? NULL : g_object_ref (category);
}

static gint
content_compare_cb (const GDataMediaContent *content, const gchar *type)
{
	return strcmp (gdata_media_content_get_content_type ((GDataMediaContent*) content), type);
}

/**
 * gdata_media_group_look_up_content:
 * @self: a #GDataMediaGroup
 * @type: the MIME type of the content desired
 *
 * Looks up a #GDataMediaContent from the group with the given MIME type. The group's list of contents is
 * a list of URIs to various formats of the group content itself, such as the SWF URI or RTSP stream for a video.
 *
 * Return value: a #GDataMediaContent matching @type, or %NULL
 **/
GDataMediaContent *
gdata_media_group_look_up_content (GDataMediaGroup *self, const gchar *type)
{
	GList *element;

	g_return_val_if_fail (GDATA_IS_MEDIA_GROUP (self), NULL);
	g_return_val_if_fail (type != NULL, NULL);

	/* TODO: If type is required, and is unique, the contents can be stored in a hash table rather than a linked list */
	element = g_list_find_custom (self->priv->contents, type, (GCompareFunc) content_compare_cb);
	if (element == NULL)
		return NULL;
	return GDATA_MEDIA_CONTENT (element->data);
}

/**
 * gdata_media_group_get_contents:
 * @self: a #GDataMediaGroup
 *
 * Returns a list of #GDataMediaContent<!-- -->s, giving the content enclosed by the group.
 *
 * Return value: a #GList of #GDataMediaContent<!-- -->s,  or %NULL
 **/
GList *
gdata_media_group_get_contents (GDataMediaGroup *self)
{
	g_return_val_if_fail (GDATA_IS_MEDIA_GROUP (self), NULL);
	return self->priv->contents;
}

void
_gdata_media_group_add_content (GDataMediaGroup *self, GDataMediaContent *content)
{
	self->priv->contents = g_list_prepend (self->priv->contents, content);
}

/**
 * gdata_media_group_get_credit:
 * @self: a #GDataMediaGroup
 *
 * Gets the #GDataMediaGroup:credit property.
 *
 * Return value: a #GDataMediaCredit giving information on whom to credit for the media group, or %NULL
 **/
GDataMediaCredit *
gdata_media_group_get_credit (GDataMediaGroup *self)
{
	g_return_val_if_fail (GDATA_IS_MEDIA_GROUP (self), NULL);
	return self->priv->credit;
}

void
_gdata_media_group_set_credit (GDataMediaGroup *self, GDataMediaCredit *credit)
{
	self->priv->credit = credit;
}

/**
 * gdata_media_group_get_media_group:
 * @self: a #GDataMediaGroup
 *
 * Gets the #GDataMediaGroup:player-uri property.
 *
 * Return value: a URI where the media group is playable in a web browser, or %NULL
 **/
const gchar *
gdata_media_group_get_player_uri (GDataMediaGroup *self)
{
	g_return_val_if_fail (GDATA_IS_MEDIA_GROUP (self), NULL);
	return self->priv->player_uri;
}

/**
 * gdata_media_group_is_restricted_in_country:
 * @self: a #GDataMediaGroup
 * @country: an ISO 3166 two-letter country code to check
 *
 * Checks whether viewing of the media is restricted in @country, either by its content rating, or by the request of the producer.
 * The return value from this function is purely informational, and no obligation is assumed.
 *
 * Return value: %TRUE if the media is restricted in @country, %FALSE otherwise
 **/
gboolean
gdata_media_group_is_restricted_in_country (GDataMediaGroup *self, const gchar *country)
{
	g_return_val_if_fail (GDATA_IS_MEDIA_GROUP (self), FALSE);
	g_return_val_if_fail (country != NULL && *country != '\0', FALSE);

	if (GPOINTER_TO_UINT (g_hash_table_lookup (self->priv->restricted_countries, country)) == TRUE)
		return TRUE;

	return GPOINTER_TO_UINT (g_hash_table_lookup (self->priv->restricted_countries, "all"));
}

/**
 * gdata_media_group_get_thumbnails:
 * @self: a #GDataMediaGroup
 *
 * Gets a list of the thumbnails available for the group.
 *
 * Return value: a #GList of #GDataMediaThumbnail<!-- -->s, or %NULL
 **/
GList *
gdata_media_group_get_thumbnails (GDataMediaGroup *self)
{
	g_return_val_if_fail (GDATA_IS_MEDIA_GROUP (self), NULL);
	return self->priv->thumbnails;
}
