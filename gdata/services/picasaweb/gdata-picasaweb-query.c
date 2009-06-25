/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Richard Schwarting 2009 <aquarichy@gmail.com>
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
 * SECTION:gdata-picasaweb-query
 * @short_description: GData PicasaWeb query object
 * @stability: Unstable
 * @include: gdata/services/picasaweb/gdata-picasaweb-query.h
 *
 * #GDataPicasaWebQuery represents a collection of query parameters specific to the Google PicasaWeb service, which go above and beyond
 * those catered for by #GDataQuery.
 *
 * For more information on the custom GData query parameters supported by #GDataPicasaWebQuery, see the <ulink type="http"
 * url="http://code.google.com/apis/picasaweb/reference.html#Parameters">online documentation</ulink>.
 **/

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <string.h>

#include "gdata-picasaweb-query.h"
#include "gdata-query.h"
#include "gdata-picasaweb-enums.h"

static void gdata_picasaweb_query_finalize (GObject *object);
static void gdata_picasaweb_query_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_picasaweb_query_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void get_query_uri (GDataQuery *self, const gchar *feed_uri, GString *query_uri, gboolean *params_started);

struct _GDataPicasaWebQueryPrivate {
	GDataPicasaWebVisibility visibility;
	gchar *thumbnail_size;
	gchar *image_size;
	gchar *tag;
	gchar *location;

	struct {
		gdouble north;
		gdouble east;
		gdouble south;
		gdouble west;
	} bounding_box;
};

enum {
	PROP_VISIBILITY = 1,
	PROP_THUMBNAIL_SIZE,
	PROP_IMAGE_SIZE,
	PROP_TAG,
	PROP_LOCATION
};

G_DEFINE_TYPE (GDataPicasaWebQuery, gdata_picasaweb_query, GDATA_TYPE_QUERY)
#define GDATA_PICASAWEB_QUERY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_PICASAWEB_QUERY, GDataPicasaWebQueryPrivate))

static void
gdata_picasaweb_query_class_init (GDataPicasaWebQueryClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataQueryClass *query_class = GDATA_QUERY_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataPicasaWebQueryPrivate));

	gobject_class->get_property = gdata_picasaweb_query_get_property;
	gobject_class->set_property = gdata_picasaweb_query_set_property;
	gobject_class->finalize = gdata_picasaweb_query_finalize;

	query_class->get_query_uri = get_query_uri;

	/**
	 * GDataPicasaWebQuery:visibility:
	 *
	 * Specifies which albums should be listed, in terms of their visibility (#GDataPicasaWebAlbum:visibility).
	 *
	 * Set the property to %0 to list all albums, regardless of their visibility. Otherwise, use values from #GDataPicasaWebVisibility.
	 *
	 * For more information, see the <ulink type="http" url="http://code.google.com/apis/picasaweb/reference.html#Visibility">
	 * online documentation</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_VISIBILITY,
				g_param_spec_int ("visibility",
					"Visibility", "Specifies which albums should be listed, in terms of their visibility.",
					0, GDATA_PICASAWEB_PRIVATE, 0,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataPicasaWebQuery:thumbnail-size:
	 *
	 * A comma-separated list of thumbnail sizes (width in pixels) to return. Only certain sizes are allowed, and whether the thumbnail should be
	 * cropped or scaled can be specified; for more information, see the
	 * <ulink type="http" url="http://code.google.com/apis/picasaweb/reference.html#Parameters">online documentation</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_THUMBNAIL_SIZE,
				g_param_spec_string ("thumbnail-size",
					"Thumbnail size", "A comma-separated list of thumbnail sizes (width in pixels) to return.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataPicasaWebQuery:image-size:
	 *
	 * A comma-separated list of image sizes (width in pixels) to return. Only certain sizes are allowed, and whether the image should be
	 * cropped or scaled can be specified; for more information, see the
	 * <ulink type="http" url="http://code.google.com/apis/picasaweb/reference.html#Parameters">online documentation</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_IMAGE_SIZE,
				g_param_spec_string ("image-size",
					"Image size", "A comma-separated list of image sizes (width in pixels) to return.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataPicasaWebQuery:tag:
	 *
	 * A tag which returned results must contain.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_TAG,
				g_param_spec_string ("tag",
					"Tag", "A tag which returned results must contain.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataPicasaWebQuery:location:
	 *
	 * A location to search for photos, e.g. "London".
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_LOCATION,
				g_param_spec_string ("location",
					"Location", "A location to search for photos, e.g. \"London\".",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_picasaweb_query_init (GDataPicasaWebQuery *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_PICASAWEB_QUERY, GDataPicasaWebQueryPrivate);
}

static void
gdata_picasaweb_query_finalize (GObject *object)
{
	GDataPicasaWebQueryPrivate *priv = GDATA_PICASAWEB_QUERY_GET_PRIVATE (object);

	g_free (priv->thumbnail_size);
	g_free (priv->image_size);
	g_free (priv->tag);
	g_free (priv->location);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_picasaweb_query_parent_class)->finalize (object);
}

static void
gdata_picasaweb_query_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataPicasaWebQueryPrivate *priv = GDATA_PICASAWEB_QUERY_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_VISIBILITY:
			g_value_set_int (value, priv->visibility);
			break;
		case PROP_THUMBNAIL_SIZE:
			g_value_set_string (value, priv->thumbnail_size);
			break;
		case PROP_IMAGE_SIZE:
			g_value_set_string (value, priv->image_size);
			break;
		case PROP_TAG:
			g_value_set_string (value, priv->tag);
			break;
		case PROP_LOCATION:
			g_value_set_string (value, priv->location);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gdata_picasaweb_query_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataPicasaWebQuery *self = GDATA_PICASAWEB_QUERY (object);

	switch (property_id) {
		case PROP_VISIBILITY:
			gdata_picasaweb_query_set_visibility (self, g_value_get_int (value));
			break;
		case PROP_THUMBNAIL_SIZE:
			gdata_picasaweb_query_set_thumbnail_size (self, g_value_get_string (value));
			break;
		case PROP_IMAGE_SIZE:
			gdata_picasaweb_query_set_image_size (self, g_value_get_string (value));
			break;
		case PROP_TAG:
			gdata_picasaweb_query_set_tag (self, g_value_get_string (value));
			break;
		case PROP_LOCATION:
			gdata_picasaweb_query_set_location (self, g_value_get_string (value));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
get_query_uri (GDataQuery *self, const gchar *feed_uri, GString *query_uri, gboolean *params_started)
{
	GDataPicasaWebQueryPrivate *priv = GDATA_PICASAWEB_QUERY (self)->priv;

	#define APPEND_SEP g_string_append_c (query_uri, (*params_started == FALSE) ? '?' : '&'); *params_started = TRUE;

	/* Chain up to the parent class */
	GDATA_QUERY_CLASS (gdata_picasaweb_query_parent_class)->get_query_uri (self, feed_uri, query_uri, params_started);

	APPEND_SEP
	if (priv->visibility == 0)
		g_string_append (query_uri, "access=all");
	else if (priv->visibility == GDATA_PICASAWEB_PUBLIC)
		g_string_append (query_uri, "access=public");
	else if (priv->visibility == GDATA_PICASAWEB_PRIVATE)
		g_string_append (query_uri, "access=private");
	else
		g_assert_not_reached ();

	if (priv->thumbnail_size != NULL) {
		APPEND_SEP
		g_string_append (query_uri, "thumbsize=");
		g_string_append_uri_escaped (query_uri, priv->thumbnail_size, NULL, FALSE);
	}

	if (priv->image_size != NULL) {
		APPEND_SEP
		g_string_append (query_uri, "imgmax=");
		g_string_append_uri_escaped (query_uri, priv->image_size, NULL, FALSE);
	}

	if (priv->tag != NULL) {
		APPEND_SEP
		g_string_append (query_uri, "tag=");
		g_string_append_uri_escaped (query_uri, priv->tag, NULL, TRUE);
	}

	if (priv->bounding_box.north != priv->bounding_box.south && priv->bounding_box.east != priv->bounding_box.west) {
		APPEND_SEP
		g_string_append_printf (query_uri, "bbox=%f,%f,%f,%f",
					priv->bounding_box.west, priv->bounding_box.south,
					priv->bounding_box.east, priv->bounding_box.north);
	}

	if (priv->location != NULL) {
		APPEND_SEP
		g_string_append (query_uri, "l=");
		g_string_append_uri_escaped (query_uri, priv->location, NULL, TRUE);
	}
}

/**
 * gdata_picasaweb_query_new:
 * @q: a query string
 *
 * Creates a new #GDataPicasaWebQuery with its #GDataQuery:q property set to @q.
 *
 * Return value: a new #GDataPicasaWebQuery
 *
 * Since: 0.4.0
 **/
GDataPicasaWebQuery *
gdata_picasaweb_query_new (const gchar *q)
{
	return g_object_new (GDATA_TYPE_PICASAWEB_QUERY, "q", q, NULL);
}

/**
 * gdata_picasaweb_query_get_visibility:
 * @self: a #GDataPicasaWebQuery
 *
 * Gets the #GDataPicasaWebQuery:visibility property.
 *
 * Return value: the visibility of the objects to retrieve, or %0 to retrieve all objects
 *
 * Since: 0.4.0
 **/
GDataPicasaWebVisibility
gdata_picasaweb_query_get_visibility (GDataPicasaWebQuery *self)
{
	g_return_val_if_fail (GDATA_IS_PICASAWEB_QUERY (self), 0);
	return self->priv->visibility;
}

/**
 * gdata_picasaweb_query_set_visibility:
 * @self: a #GDataPicasaWebQuery
 * @visibility: the visibility of the objects to retrieve, or %0 to retrieve all objects
 *
 * Sets the #GDataPicasaWebQuery:visibility property to @visibility.
 *
 * Since: 0.4.0
 **/
void
gdata_picasaweb_query_set_visibility (GDataPicasaWebQuery *self, GDataPicasaWebVisibility visibility)
{
	g_return_if_fail (GDATA_IS_PICASAWEB_QUERY (self));
	self->priv->visibility = visibility;
	g_object_notify (G_OBJECT (self), "visibility");
}

/**
 * gdata_picasaweb_query_get_thumbnail_size:
 * @self: a #GDataPicasaWebQuery
 *
 * Gets the #GDataPicasaWebQuery:thumbnail-size property.
 *
 * Return value: a comma-separated list of thumbnail sizes to retrieve, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_picasaweb_query_get_thumbnail_size (GDataPicasaWebQuery *self)
{
	g_return_val_if_fail (GDATA_IS_PICASAWEB_QUERY (self), NULL);
	return self->priv->thumbnail_size;
}

/**
 * gdata_picasaweb_query_set_thumbnail_size:
 * @self: a #GDataPicasaWebQuery
 * @thumbnail_size: a comma-separated list of thumbnail sizes to retrieve, or %NULL
 *
 * Sets the #GDataPicasaWebQuery:thumbnail-size property to @thumbnail_size.
 *
 * Set @thumbnail_size to %NULL to unset the property.
 *
 * Since: 0.4.0
 **/
void
gdata_picasaweb_query_set_thumbnail_size (GDataPicasaWebQuery *self, const gchar *thumbnail_size)
{
	g_return_if_fail (GDATA_IS_PICASAWEB_QUERY (self));

	g_free (self->priv->thumbnail_size);
	self->priv->thumbnail_size = g_strdup (thumbnail_size);
	g_object_notify (G_OBJECT (self), "thumbnail-size");
}

/**
 * gdata_picasaweb_query_get_image_size:
 * @self: a #GDataPicasaWebQuery
 *
 * Gets the #GDataPicasaWebQuery:image-size property.
 *
 * Return value: a comma-separated list of image sizes to retrieve, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_picasaweb_query_get_image_size (GDataPicasaWebQuery *self)
{
	g_return_val_if_fail (GDATA_IS_PICASAWEB_QUERY (self), NULL);
	return self->priv->image_size;
}

/**
 * gdata_picasaweb_query_set_image_size:
 * @self: a #GDataPicasaWebQuery
 * @image_size: a comma-separated list of image sizes to retrieve, or %NULL
 *
 * Sets the #GDataPicasaWebQuery:image-size property to @image_size.
 *
 * Set @image_size to %NULL to unset the property.
 *
 * Since: 0.4.0
 **/
void
gdata_picasaweb_query_set_image_size (GDataPicasaWebQuery *self, const gchar *image_size)
{
	g_return_if_fail (GDATA_IS_PICASAWEB_QUERY (self));

	g_free (self->priv->image_size);
	self->priv->image_size = g_strdup (image_size);
	g_object_notify (G_OBJECT (self), "image-size");
}

/**
 * gdata_picasaweb_query_get_tag:
 * @self: a #GDataPicasaWebQuery
 *
 * Gets the #GDataPicasaWebQuery:tag property.
 *
 * Return value: a tag which retrieved objects must have, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_picasaweb_query_get_tag (GDataPicasaWebQuery *self)
{
	g_return_val_if_fail (GDATA_IS_PICASAWEB_QUERY (self), NULL);
	return self->priv->tag;
}

/**
 * gdata_picasaweb_query_set_tag:
 * @self: a #GDataPicasaWebQuery
 * @tag: a tag which retrieved objects must have, or %NULL
 *
 * Sets the #GDataPicasaWebQuery:tag property to @tag.
 *
 * Set @tag to %NULL to unset the property.
 *
 * Since: 0.4.0
 **/
void
gdata_picasaweb_query_set_tag (GDataPicasaWebQuery *self, const gchar *tag)
{
	g_return_if_fail (GDATA_IS_PICASAWEB_QUERY (self));

	g_free (self->priv->tag);
	self->priv->tag = g_strdup (tag);
	g_object_notify (G_OBJECT (self), "tag");
}

/**
 * gdata_picasaweb_query_get_bounding_box:
 * @self: a #GDataPicasaWebQuery
 * @north: return location for the latitude of the top of the box, or %NULL
 * @east: return location for the longitude of the right of the box, or %NULL
 * @south: return location for the latitude of the south of the box, or %NULL
 * @west: return location for the longitude of the left of the box, or %NULL
 *
 * Gets the latitudes and longitudes of a bounding box, inside which all the results must lie.
 *
 * Since: 0.4.0
 **/
void
gdata_picasaweb_query_get_bounding_box (GDataPicasaWebQuery *self, gdouble *north, gdouble *east, gdouble *south, gdouble *west)
{
	g_return_if_fail (GDATA_IS_PICASAWEB_QUERY (self));

	if (north != NULL)
		*north = self->priv->bounding_box.north;
	if (east != NULL)
		*east = self->priv->bounding_box.east;
	if (south != NULL)
		*south = self->priv->bounding_box.south;
	if (west != NULL)
		*west = self->priv->bounding_box.west;
}

/**
 * gdata_picasaweb_query_set_bounding_box:
 * @self: a #GDataPicasaWebQuery
 * @north: latitude of the top of the box
 * @east: longitude of the right of the box
 * @south: latitude of the bottom of the box
 * @west: longitude of the left of the box
 *
 * Sets a bounding box, inside which all the returned results must lie.
 *
 * Set @north, @east, @south and @west to %0 to unset the property.
 *
 * Since: 0.4.0
 **/
void
gdata_picasaweb_query_set_bounding_box (GDataPicasaWebQuery *self, gdouble north, gdouble east, gdouble south, gdouble west)
{
	g_return_if_fail (GDATA_IS_PICASAWEB_QUERY (self));
	g_return_if_fail (north >= -90.0 && north <= 90.0);
	g_return_if_fail (south >= -90.0 && south <= 90.0);
	g_return_if_fail (east >= -180.0 && east <= 180.0);
	g_return_if_fail (west >= -180.0 && west <= 180.0);

	self->priv->bounding_box.north = north;
	self->priv->bounding_box.east = east;
	self->priv->bounding_box.south = south;
	self->priv->bounding_box.west = west;
}

/**
 * gdata_picasaweb_query_get_location:
 * @self: a #GDataPicasaWebQuery
 *
 * Gets the #GDataPicasaWebQuery:location property.
 *
 * Return value: a location which returned objects must be near, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_picasaweb_query_get_location (GDataPicasaWebQuery *self)
{
	g_return_val_if_fail (GDATA_IS_PICASAWEB_QUERY (self), NULL);
	return self->priv->location;
}

/**
 * gdata_picasaweb_query_set_location:
 * @self: a #GDataPicasaWebQuery
 * @location: a location which returned objects must be near, or %NULL
 *
 * Sets the #GDataPicasaWebQuery:location property to @location.
 *
 * Set @location to %NULL to unset the property.
 *
 * Since: 0.4.0
 **/
void
gdata_picasaweb_query_set_location (GDataPicasaWebQuery *self, const gchar *location)
{
	g_return_if_fail (GDATA_IS_PICASAWEB_QUERY (self));

	g_free (self->priv->location);
	self->priv->location = g_strdup (location);
	g_object_notify (G_OBJECT (self), "location");
}
