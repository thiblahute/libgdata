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

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <libxml/parser.h>
#include <string.h>

#include "gdata-media-group.h"
#include "gdata-private.h"
#include "gdata-service.h"
#include "gdata-media-rss.h"

static void gdata_media_group_finalize (GObject *object);
static void gdata_media_group_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_media_group_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

struct _GDataMediaGroupPrivate {
	gchar *keywords;
	gchar *player_uri;
	GDataMediaRating *rating;
	GDataMediaRestriction *restriction;
	GList *thumbnails; /* GDataMediaThumbnail */
	gchar *title;
	GDataMediaCategory *category;
	GList *contents; /* GDataMediaContent */
	GDataMediaCredit *credit;
	gchar *description;
};

enum {
	PROP_KEYWORDS = 1,
	PROP_PLAYER_URI,
	PROP_RATING,
	PROP_RESTRICTION,
	PROP_TITLE,
	PROP_CATEGORY,
	PROP_CREDIT,
	PROP_DESCRIPTION
};

G_DEFINE_TYPE (GDataMediaGroup, gdata_media_group, G_TYPE_OBJECT)
#define GDATA_MEDIA_GROUP_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_MEDIA_GROUP, GDataMediaGroupPrivate))

static void
gdata_media_group_class_init (GDataMediaGroupClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataMediaGroupPrivate));

	gobject_class->set_property = gdata_media_group_set_property;
	gobject_class->get_property = gdata_media_group_get_property;
	gobject_class->finalize = gdata_media_group_finalize;

	g_object_class_install_property (gobject_class, PROP_KEYWORDS,
				g_param_spec_string ("keywords",
					"Keywords", "TODO",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_PLAYER_URI,
				g_param_spec_string ("player-uri",
					"Player URI", "TODO",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_RATING,
				g_param_spec_pointer ("rating",
					"Rating", "TODO",
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_RESTRICTION,
				g_param_spec_pointer ("restriction",
					"Restriction", "TODO",
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_TITLE,
				g_param_spec_string ("title",
					"Title", "TODO",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_CATEGORY,
				g_param_spec_pointer ("category",
					"Category", "TODO",
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_CREDIT,
				g_param_spec_pointer ("credit",
					"Credit", "TODO",
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_DESCRIPTION,
				g_param_spec_string ("description",
					"Description", "TODO",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_media_group_init (GDataMediaGroup *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_MEDIA_GROUP, GDataMediaGroupPrivate);
}

static void
gdata_media_group_finalize (GObject *object)
{
	GDataMediaGroupPrivate *priv = GDATA_MEDIA_GROUP_GET_PRIVATE (object);

	g_free (priv->keywords);
	g_free (priv->player_uri);
	gdata_media_rating_free (priv->rating);
	gdata_media_restriction_free (priv->restriction);
	g_list_foreach (priv->thumbnails, (GFunc) gdata_media_thumbnail_free, NULL);
	g_list_free (priv->thumbnails);
	g_free (priv->title);
	gdata_media_category_free (priv->category);
	g_list_foreach (priv->contents, (GFunc) gdata_media_content_free, NULL);
	g_list_free (priv->contents);
	gdata_media_credit_free (priv->credit);
	g_free (priv->description);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_media_group_parent_class)->finalize (object);
}

static void
gdata_media_group_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataMediaGroupPrivate *priv = GDATA_MEDIA_GROUP_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_KEYWORDS:
			g_value_set_string (value, priv->keywords);
			break;
		case PROP_PLAYER_URI:
			g_value_set_string (value, priv->player_uri);
			break;
		case PROP_RATING:
			g_value_set_pointer (value, priv->rating);
			break;
		case PROP_RESTRICTION:
			g_value_set_pointer (value, priv->restriction);
			break;
		case PROP_TITLE:
			g_value_set_string (value, priv->title);
			break;
		case PROP_CATEGORY:
			g_value_set_pointer (value, priv->category);
			break;
		case PROP_CREDIT:
			g_value_set_pointer (value, priv->credit);
			break;
		case PROP_DESCRIPTION:
			g_value_set_string (value, priv->description);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gdata_media_group_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataMediaGroupPrivate *priv = GDATA_MEDIA_GROUP_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_KEYWORDS:
			gdata_media_group_set_keywords (GDATA_MEDIA_GROUP (object), g_value_get_string (value));
			break;
		case PROP_PLAYER_URI:
			gdata_media_group_set_player_uri (GDATA_MEDIA_GROUP (object), g_value_get_string (value));
			break;
		case PROP_RATING:
			gdata_media_group_set_rating (GDATA_MEDIA_GROUP (object), g_value_get_pointer (value));
			break;
		case PROP_RESTRICTION:
			gdata_media_group_set_restriction (GDATA_MEDIA_GROUP (object), g_value_get_pointer (value));
			break;
		case PROP_TITLE:
			gdata_media_group_set_title (GDATA_MEDIA_GROUP (object), g_value_get_string (value));
			break;
		case PROP_CATEGORY:
			gdata_media_group_set_category (GDATA_MEDIA_GROUP (object), g_value_get_pointer (value));
			break;
		case PROP_CREDIT:
			gdata_media_group_set_credit (GDATA_MEDIA_GROUP (object), g_value_get_pointer (value));
			break;
		case PROP_DESCRIPTION:
			gdata_media_group_set_description (GDATA_MEDIA_GROUP (object), g_value_get_string (value));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

GDataMediaGroup *
gdata_media_group_new (void)
{
	return g_object_new (GDATA_TYPE_MEDIA_GROUP, NULL);
}

GDataMediaGroup *
_gdata_media_group_new_from_xml_node (xmlDoc *doc, xmlNode *node, GError **error)
{
	GDataMediaGroup *group;

	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);
	g_return_val_if_fail (xmlStrcmp (node->name, (xmlChar*) "group") == 0, FALSE);

	group = gdata_media_group_new ();

	node = node->xmlChildrenNode;
	while (node != NULL) {
		if (_gdata_media_group_parse_xml_node (group, doc, node, error) == FALSE) {
			g_object_unref (group);
			return NULL;
		}
		node = node->next;
	}

	return group;
}

gboolean
_gdata_media_group_parse_xml_node (GDataMediaGroup *self, xmlDoc *doc, xmlNode *node, GError **error)
{
	g_return_val_if_fail (GDATA_IS_MEDIA_GROUP (self), FALSE);
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);

	if (xmlStrcmp (node->name, (xmlChar*) "title") == 0) {
		/* media:title */
		xmlChar *title = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		gdata_media_group_set_title (self, (gchar*) title);
		xmlFree (title);
	} else if (xmlStrcmp (node->name, (xmlChar*) "description") == 0) {
		/* media:description */
		xmlChar *description = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		gdata_media_group_set_description (self, (gchar*) description);
		xmlFree (description);
	} else if (xmlStrcmp (node->name, (xmlChar*) "keywords") == 0) {
		/* media:keywords */
		xmlChar *keywords = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		gdata_media_group_set_keywords (self, (gchar*) keywords);
		xmlFree (keywords);
	} else if (xmlStrcmp (node->name, (xmlChar*) "category") == 0) {
		/* media:category */
		xmlChar *scheme, *label, *content;
		GDataMediaCategory *category;

		scheme = xmlGetProp (node, (xmlChar*) "scheme");
		label = xmlGetProp (node, (xmlChar*) "label");
		content = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);

		category = gdata_media_category_new ((gchar*) content, (gchar*) scheme, (gchar*) label);
		gdata_media_group_set_category (self, category);
	} else if (xmlStrcmp (node->name, (xmlChar*) "content") == 0) {
		/* media:content */
		xmlChar *uri, *type, *is_default, *expression, *duration, *format;
		gboolean is_default_bool;
		GDataMediaExpression expression_enum;
		gint duration_int, format_int;
		GDataMediaContent *content;

		/* Parse isDefault */
		is_default = xmlGetProp (node, (xmlChar*) "isDefault");
		if (is_default == NULL || xmlStrcmp (is_default, (xmlChar*) "false") == 0)
			is_default_bool = FALSE;
		else if (xmlStrcmp (is_default, (xmlChar*) "true") == 0)
			is_default_bool = TRUE;
		else {
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
				     _("Unknown value \"%s\" of a <media:content> @isDefault property."),
				     is_default);
			xmlFree (is_default);
			return FALSE;
		}
		xmlFree (is_default);

		/* Parse expression */
		expression = xmlGetProp (node, (xmlChar*) "expression");
		if (xmlStrcmp (expression, (xmlChar*) "sample") == 0)
			expression_enum = GDATA_MEDIA_EXPRESSION_SAMPLE;
		else if (xmlStrcmp (expression, (xmlChar*) "full") == 0)
			expression_enum = GDATA_MEDIA_EXPRESSION_FULL;
		else if (xmlStrcmp (expression, (xmlChar*) "nonstop") == 0)
			expression_enum = GDATA_MEDIA_EXPRESSION_NONSTOP;
		else {
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
				     _("Unknown value \"%s\" of a <media:content> @expression property."),
				     expression);
			xmlFree (expression);
			return FALSE;
		}
		xmlFree (expression);

		/* Parse duration */
		duration = xmlGetProp (node, (xmlChar*) "duration");
		if (duration == NULL)
			duration_int = -1;
		else
			duration_int = MAX (atoi ((gchar*) duration), -1);
		xmlFree (duration);

		format = xmlGetProp (node, (xmlChar*) "format");
		if (format == NULL)
			format_int = -1;
		else
			format_int = MAX (atoi ((gchar*) format), -1);
		xmlFree (format);

		uri = xmlGetProp (node, (xmlChar*) "url");
		type = xmlGetProp (node, (xmlChar*) "type");

		content = gdata_media_content_new ((gchar*) uri, (gchar*) type, is_default_bool, expression_enum, duration_int, format_int);
		gdata_media_group_add_content (self, content);
	} else if (xmlStrcmp (node->name, (xmlChar*) "credit") == 0) {
		/* media:credit */
		xmlChar *role, *type, *content;
		GDataMediaCredit *credit;

		/* Check the role property is "uploader" */
		role = xmlGetProp (node, (xmlChar*) "role");
		if (xmlStrcmp (role, (xmlChar*) "uploader") != 0) {
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
				     _("Unknown value \"%s\" of a <media:credit> @role property."),
				     (gchar*) role);
			xmlFree (role);
			return FALSE;
		}
		xmlFree (role);

		/* Check the type property */
		type = xmlGetProp (node, (xmlChar*) "type");
		if (type != NULL && xmlStrcmp (role, (xmlChar*) "partner") != 0) {
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
				     _("Unknown value \"%s\" of a <media:credit> @type property."),
				     (gchar*) type);
			xmlFree (type);
			return FALSE;
		}

		content = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);

		credit = gdata_media_credit_new ((gchar*) content, (type != NULL) ? TRUE : FALSE);
		gdata_media_group_set_credit (self, credit);

		xmlFree (type);
		xmlFree (content);
	} else if (xmlStrcmp (node->name, (xmlChar*) "player") == 0) {
		/* media:player */
		xmlChar *player_uri = xmlGetProp (node, (xmlChar*) "url");
		gdata_media_group_set_player_uri (self, (gchar*) player_uri);
		xmlFree (player_uri);
	} else if (xmlStrcmp (node->name, (xmlChar*) "rating") == 0) {
		/* media:rating */
		xmlChar *scheme, *country;
		GDataMediaRating *rating;

		scheme = xmlGetProp (node, (xmlChar*) "scheme");
		country = xmlGetProp (node, (xmlChar*) "country");

		rating = gdata_media_rating_new ((gchar*) scheme, (gchar*) country);
		gdata_media_group_set_rating (self, rating);

		xmlFree (scheme);
		xmlFree (country);
	} else if (xmlStrcmp (node->name, (xmlChar*) "restriction") == 0) {
		/* media:restriction */
		xmlChar *type, *countries, *relationship;
		gboolean relationship_bool;
		GDataMediaRestriction *restriction;

		/* Check the type property is "country" */
		type = xmlGetProp (node, (xmlChar*) "type");
		if (xmlStrcmp (type, (xmlChar*) "country") != 0) {
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
				     _("Unknown value \"%s\" of a <media:restriction> @type property."),
				     (gchar*) type);
			xmlFree (type);
			return FALSE;
		}
		xmlFree (type);

		countries = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		relationship = xmlGetProp (node, (xmlChar*) "relationship");

		if (xmlStrcmp (relationship, (xmlChar*) "allow") == 0)
			relationship_bool = TRUE;
		else if (xmlStrcmp (relationship, (xmlChar*) "deny") == 0)
			relationship_bool = FALSE;
		else {
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
				     _("Unknown value \"%s\" of a <media:restriction> @relationship property."),
				     (gchar*) relationship);
			xmlFree (relationship);
			return FALSE;
		}
		xmlFree (relationship);

		restriction = gdata_media_restriction_new ((gchar*) countries, relationship_bool);
		gdata_media_group_set_restriction (self, restriction);
	} else if (xmlStrcmp (node->name, (xmlChar*) "thumbnail") == 0) {
		/* media:thumbnail */
		xmlChar *uri, *width, *height, *time;
		guint width_uint, height_uint;
		gint time_int;
		GDataMediaThumbnail *thumbnail;

		/* Get the width and height */
		width = xmlGetProp (node, (xmlChar*) "width");
		height = xmlGetProp (node, (xmlChar*) "height");
		if (width == NULL || height == NULL) {
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
				     _("A required @width/@height property of a <media:thumbnail> was not present."));
			return FALSE;
		}

		width_uint = MAX (atoi ((gchar*) width), 0);
		height_uint = MAX (atoi ((gchar*) height), 0);
		xmlFree (width);
		xmlFree (height);

		/* Get and parse the time */
		time = xmlGetProp (node, (xmlChar*) "time");
		if (time == NULL) {
			time_int = -1;
		} else {
			time_int = gdata_media_thumbnail_parse_time ((gchar*) time);
			if (time_int == -1) {
				g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
					     _("The @time property (\"%s\") of a <media:thumbnail> could not be parsed."),
					     (gchar*) time);
				xmlFree (time);
				return FALSE;
			}
			xmlFree (time);
		}

		uri = xmlGetProp (node, (xmlChar*) "url");

		thumbnail = gdata_media_thumbnail_new ((gchar*) uri, width_uint, height_uint, time_int);
		gdata_media_group_add_thumbnail (self, thumbnail);
	} else {
		g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_UNHANDLED_XML_ELEMENT,
			     _("Unhandled <%s:%s> element as a child of <media:group>."),
			     node->ns->prefix, node->name);
		return FALSE;
	}

	return TRUE;
}

const gchar *
gdata_media_group_get_keywords (GDataMediaGroup *self)
{
	g_return_val_if_fail (GDATA_IS_MEDIA_GROUP (self), NULL);
	return self->priv->keywords;
}

void
gdata_media_group_set_keywords (GDataMediaGroup *self, const gchar *keywords)
{
	g_return_if_fail (GDATA_IS_MEDIA_GROUP (self));

	g_free (self->priv->keywords);
	self->priv->keywords = g_strdup (keywords);
	g_object_notify (G_OBJECT (self), "keywords");
}

const gchar *
gdata_media_group_get_player_uri (GDataMediaGroup *self)
{
	g_return_val_if_fail (GDATA_IS_MEDIA_GROUP (self), NULL);
	return self->priv->player_uri;
}

void
gdata_media_group_set_player_uri (GDataMediaGroup *self, const gchar *player_uri)
{
	g_return_if_fail (GDATA_IS_MEDIA_GROUP (self));

	g_free (self->priv->player_uri);
	self->priv->player_uri = g_strdup (player_uri);
	g_object_notify (G_OBJECT (self), "player-uri");
}

GDataMediaRating *
gdata_media_group_get_rating (GDataMediaGroup *self)
{
	g_return_val_if_fail (GDATA_IS_MEDIA_GROUP (self), NULL);
	return self->priv->rating;
}

void
gdata_media_group_set_rating (GDataMediaGroup *self, GDataMediaRating *rating)
{
	g_return_if_fail (GDATA_IS_MEDIA_GROUP (self));

	gdata_media_rating_free (self->priv->rating); /* TODO: not so happy about this memory management */
	self->priv->rating = rating;
	g_object_notify (G_OBJECT (self), "rating");
}

GDataMediaRestriction *
gdata_media_group_get_restriction (GDataMediaGroup *self)
{
	g_return_val_if_fail (GDATA_IS_MEDIA_GROUP (self), NULL);
	return self->priv->restriction;
}

void
gdata_media_group_set_restriction (GDataMediaGroup *self, GDataMediaRestriction *restriction)
{
	g_return_if_fail (GDATA_IS_MEDIA_GROUP (self));

	gdata_media_restriction_free (self->priv->restriction); /* TODO: not so happy about this memory management */
	self->priv->restriction = restriction;
	g_object_notify (G_OBJECT (self), "restriction");
}

const gchar *
gdata_media_group_get_title (GDataMediaGroup *self)
{
	g_return_val_if_fail (GDATA_IS_MEDIA_GROUP (self), NULL);
	return self->priv->title;
}

void
gdata_media_group_set_title (GDataMediaGroup *self, const gchar *title)
{
	g_return_if_fail (GDATA_IS_MEDIA_GROUP (self));

	g_free (self->priv->title);
	self->priv->title = g_strdup (title);
	g_object_notify (G_OBJECT (self), "title");
}

GDataMediaCategory *
gdata_media_group_get_category (GDataMediaGroup *self)
{
	g_return_val_if_fail (GDATA_IS_MEDIA_GROUP (self), NULL);
	return self->priv->category;
}

void
gdata_media_group_set_category (GDataMediaGroup *self, GDataMediaCategory *category)
{
	g_return_if_fail (GDATA_IS_MEDIA_GROUP (self));

	gdata_media_category_free (self->priv->category); /* TODO: not so happy about this memory management */
	self->priv->category = category;
	g_object_notify (G_OBJECT (self), "category");
}

GDataMediaCredit *
gdata_media_group_get_credit (GDataMediaGroup *self)
{
	g_return_val_if_fail (GDATA_IS_MEDIA_GROUP (self), NULL);
	return self->priv->credit;
}

void
gdata_media_group_set_credit (GDataMediaGroup *self, GDataMediaCredit *credit)
{
	g_return_if_fail (GDATA_IS_MEDIA_GROUP (self));

	gdata_media_credit_free (self->priv->credit); /* TODO: not so happy about this memory management */
	self->priv->credit = credit;
	g_object_notify (G_OBJECT (self), "credit");
}

const gchar *
gdata_media_group_get_description (GDataMediaGroup *self)
{
	g_return_val_if_fail (GDATA_IS_MEDIA_GROUP (self), NULL);
	return self->priv->description;
}

void
gdata_media_group_set_description (GDataMediaGroup *self, const gchar *description)
{
	g_return_if_fail (GDATA_IS_MEDIA_GROUP (self));

	g_free (self->priv->description);
	self->priv->description = g_strdup (description);
	g_object_notify (G_OBJECT (self), "keywords");
}

/* TODO: More content API */
void
gdata_media_group_add_content (GDataMediaGroup *self, GDataMediaContent *content)
{
	g_return_if_fail (GDATA_IS_MEDIA_GROUP (self));
	g_return_if_fail (content != NULL);

	self->priv->contents = g_list_prepend (self->priv->contents, content);
}

/* TODO: More thumbnail API */
void
gdata_media_group_add_thumbnail (GDataMediaGroup *self, GDataMediaThumbnail *thumbnail)
{
	g_return_if_fail (GDATA_IS_MEDIA_GROUP (self));
	g_return_if_fail (thumbnail != NULL);

	self->priv->thumbnails = g_list_prepend (self->priv->thumbnails, thumbnail);
}
