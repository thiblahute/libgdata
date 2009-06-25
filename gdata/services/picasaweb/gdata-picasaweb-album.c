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
 * SECTION:gdata-picasaweb-album
 * @short_description: GData PicasaWeb album object
 * @stability: Unstable
 * @include: gdata/services/picasaweb/gdata-picasaweb-album.h
 *
 * #GDataPicasaWebAlbum is a subclass of #GDataEntry to represent an album from Google PicasaWeb.
 *
 * For more details of Google PicasaWeb's GData API, see the <ulink type="http" url="http://code.google.com/apis/picasaweb/reference.html">
 * online documentation</ulink>.
 **/

/* TODO: support the album cover/icon ? */

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <libxml/parser.h>
#include <string.h>

#include "gdata-picasaweb-album.h"
#include "gdata-private.h"
#include "gdata-service.h"
#include "gdata-parsable.h"
#include "gdata-parser.h"
#include "gdata-types.h"
#include "media/gdata-media-group.h"
#include "gdata-picasaweb-enums.h"

static void gdata_picasaweb_album_dispose (GObject *object);
static void gdata_picasaweb_album_finalize (GObject *object);
static void gdata_picasaweb_album_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_picasaweb_album_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void get_xml (GDataParsable *parsable, GString *xml_string);
static gboolean parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error);
static void get_namespaces (GDataParsable *parsable, GHashTable *namespaces);

struct _GDataPicasaWebAlbumPrivate {
	gchar *user;
	gchar *nickname;
	GTimeVal edited;
	gchar *name; /* album title, usable in URIs */
	gchar *location;
	GDataPicasaWebVisibility visibility;
	GTimeVal timestamp;
	guint num_photos;
	guint num_photos_remaining;
	glong bytes_used;
	gboolean is_commenting_enabled;
	guint comment_count;

	/* media:group */
	GDataMediaGroup *media_group;
};

enum {
	PROP_USER = 1,
	PROP_NICKNAME,
	PROP_EDITED,
	PROP_NAME,
	PROP_LOCATION,
	PROP_VISIBILITY,
	PROP_TIMESTAMP,
	PROP_NUM_PHOTOS,
	PROP_NUM_PHOTOS_REMAINING,
	PROP_BYTES_USED,
	PROP_IS_COMMENTING_ENABLED,
	PROP_COMMENT_COUNT,
	PROP_DESCRIPTION,
	PROP_TAGS
};

G_DEFINE_TYPE (GDataPicasaWebAlbum, gdata_picasaweb_album, GDATA_TYPE_ENTRY)
#define GDATA_PICASAWEB_ALBUM_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_PICASAWEB_ALBUM, GDataPicasaWebAlbumPrivate))

static void
gdata_picasaweb_album_class_init (GDataPicasaWebAlbumClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataPicasaWebAlbumPrivate));

	gobject_class->get_property = gdata_picasaweb_album_get_property;
	gobject_class->set_property = gdata_picasaweb_album_set_property;
	gobject_class->dispose = gdata_picasaweb_album_dispose;
	gobject_class->finalize = gdata_picasaweb_album_finalize;

	parsable_class->parse_xml = parse_xml;
	parsable_class->get_xml = get_xml;
	parsable_class->get_namespaces = get_namespaces;

	/**
	 * GDataPicasaWeb:user
	 *
	 * The username of the album owner.
	 *
	 * For more information, see the <ulink type="http" url="http://code.google.com/apis/picasaweb/reference.html#gphoto_user">
	 * gphoto specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_USER,
					 g_param_spec_string ("user",
							      "User", "The username of the album owner.",
							      NULL,
							      G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataPicasaWeb:nickname
	 *
	 * The user's nickname. This is a user-specified value that should be used when referring to the user by name.
	 *
	 * For more information, see the <ulink type="http" url="http://code.google.com/apis/picasaweb/reference.html#gphoto_nickname">
	 * gphoto specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_NICKNAME,
					 g_param_spec_string ("nickname",
							      "Nickname", "The user's nickname.",
							      NULL,
							      G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataPicasaWeb:edited
	 *
	 * The time this album was last edited. If the album has not been edited yet, the content indicates the time it was created.
	 *
	 * For more information, see the <ulink type="http" url="http://www.atomenabled.org/developers/protocol/#appEdited">
	 * Atom Publishing Protocol specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_EDITED,
					 g_param_spec_boxed ("edited",
							     "Edited", "The time this album was last edited.",
							     GDATA_TYPE_G_TIME_VAL,
							     G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataPicasaWeb:name
	 *
	 * The name of the album, which is the URI-usable name derived from the album title (#GDataEntry:title).
	 *
	 * For more information, see the <ulink type="http" url="http://code.google.com/apis/picasaweb/reference.html#gphoto_name">
	 * gphoto specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_NAME,
					 g_param_spec_string ("name",
							      "Name", "The name of the album.",
							      NULL,
							      G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataPicasaWeb:location
	 *
	 * The user-specified location associated with the album. A place name.
	 *
	 * For more information, see the <ulink type="http" url="http://code.google.com/apis/picasaweb/reference.html#gphoto_location">
	 * gphoto specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_LOCATION,
					 g_param_spec_string ("location",
							      "Location", "The user-specified location associated with the album.",
							      NULL,
							      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataPicasaWeb:visibility
	 *
	 * The visibility (or access rights) of the album.
	 *
	 * For more information, see the <ulink type="http" url="http://code.google.com/apis/picasaweb/reference.html#gphoto_access">
	 * gphoto specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	/* TODO: atom:rights duplicates this? */
	g_object_class_install_property (gobject_class, PROP_VISIBILITY,
					 g_param_spec_enum ("visibility",
							    "Visibility", "The visibility (or access rights) of the album.",
							    GDATA_TYPE_PICASAWEB_VISIBILITY, GDATA_PICASAWEB_PUBLIC,
							    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataPicasaWeb:timestamp
	 *
	 * The timestamp of when the album occurred, settable by the user.
	 *
	 * For more information, see the <ulink type="http" url="http://code.google.com/apis/picasaweb/reference.html#gphoto_timestamp">
	 * gphoto specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_TIMESTAMP,
					 g_param_spec_boxed ("timestamp",
							     "Timestamp", "The timestamp of when the album occurred, settable by the user.",
							     GDATA_TYPE_G_TIME_VAL,
							     G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/* TODO: Change to photo-count? */
	/**
	 * GDataPicasaWebAlbum:num-photos
	 *
	 * The number of photos and videos in the album.
	 *
	 * For more information, see the <ulink type="http" url="http://code.google.com/apis/picasaweb/reference.html#gphoto_numphotos">
	 * gphoto specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_NUM_PHOTOS,
					 g_param_spec_uint ("num-photos",
							    "Number of photos", "The number of photos and videos in the album.",
							    0, G_MAXUINT, 0,
							    G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/* TODO: Change to remaining-photos-count? */
	/**
	 * GDataPicasaWebAlbum:num-photos-remaining
	 *
	 * The number of photos and videos that can still be uploaded to this album.
	 * This doesn't account for quota, just a hardcoded maximum number per album set by Google.
	 *
	 * For more information, see the <ulink type="http" url="http://code.google.com/apis/picasaweb/reference.html#gphoto_numphotosremaining">
	 * gphoto specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_NUM_PHOTOS_REMAINING,
					 g_param_spec_uint ("num-photos-remaining",
							    "Number of photo spaces remaining", "The number of photos and videos that can still be"
							    " uploaded to this album.",
							    0, G_MAXUINT, 0,
							    G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataPicasaWebAlbum:bytes-used:
	 *
	 * The number of bytes consumed by this album and its contents. Note that this is only set if the authenticated user is the owner of the
	 * album; it's otherwise %-1.
	 *
	 * For more information, see the <ulink type="http" url="http://code.google.com/apis/picasaweb/reference.html#gphoto_bytesUsed">
	 * gphoto specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_BYTES_USED,
					 g_param_spec_long ("bytes-used",
							    "Number of bytes used", "The number of bytes consumed by this album and its contents.",
							    -1, G_MAXLONG, -1,
							    G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataPicasaWebAlbum:commenting-enabled:
	 *
	 * Whether commenting is enabled for this album.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_IS_COMMENTING_ENABLED,
					 g_param_spec_boolean ("is-commenting-enabled",
							       "Commenting enabled?", "Whether commenting is enabled for this album.",
							       FALSE,
							       G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataPicasaWebAlbum:comment-count:
	 *
	 * The number of comments on the album.
	 *
	 * For more information, see the <ulink type="http" url="http://code.google.com/apis/picasaweb/reference.html#gphoto_commentCount">
	 * gphoto specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_COMMENT_COUNT,
					 g_param_spec_uint ("comment-count",
							    "Comment count", "The number of comments on the album.",
							    0, G_MAXUINT, 0,
							    G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataPicasaWebAlbum:description:
	 *
	 * Description of the album.
	 *
	 * For more information, see the <ulink type="http" url="http://code.google.com/apis/picasaweb/reference.html#media_description">
	 * Media RSS specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_DESCRIPTION,
					 g_param_spec_string ("description",
							      "Description", "Description of the album.",
							      NULL,
							      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataPicasaWebAlbum:tags:
	 *
	 * A comma-separated list of tags associated with the album; all the tags associated with the individual photos in the album.
	 *
	 * For more information, see the <ulink type="http" url="http://code.google.com/apis/picasaweb/reference.html#media_keywords">
	 * Media RSS specification</ulink>.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_TAGS,
					 g_param_spec_string ("tags",
							      "Tags", "A comma-separated list of tags associated with the album",
							      NULL,
							      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
notify_title_cb (GDataPicasaWebAlbum *self, GParamSpec *pspec, gpointer user_data)
{
	/* Update our media:group title */
	if (self->priv->media_group != NULL)
		gdata_media_group_set_title (self->priv->media_group, gdata_entry_get_title (GDATA_ENTRY (self)));
}

static void
gdata_picasaweb_album_init (GDataPicasaWebAlbum *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_PICASAWEB_ALBUM, GDataPicasaWebAlbumPrivate);
	self->priv->media_group = g_object_new (GDATA_TYPE_MEDIA_GROUP, NULL);

	/* Connect to the notify::title signal from GDataEntry so our media:group title can be kept in sync */
	g_signal_connect (GDATA_ENTRY (self), "notify::title", G_CALLBACK (notify_title_cb), NULL);
}

static void
gdata_picasaweb_album_dispose (GObject *object)
{
	GDataPicasaWebAlbumPrivate *priv = GDATA_PICASAWEB_ALBUM_GET_PRIVATE (object);

	if (priv->media_group != NULL)
		g_object_unref (priv->media_group);
	priv->media_group = NULL;

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_picasaweb_album_parent_class)->dispose (object);
}

static void
gdata_picasaweb_album_finalize (GObject *object)
{
	GDataPicasaWebAlbumPrivate *priv = GDATA_PICASAWEB_ALBUM_GET_PRIVATE (object);

	xmlFree (priv->user);
	xmlFree (priv->nickname);
	xmlFree (priv->name);
	g_free (priv->location);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_picasaweb_album_parent_class)->finalize (object);
}

static void
gdata_picasaweb_album_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataPicasaWebAlbumPrivate *priv = GDATA_PICASAWEB_ALBUM_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_USER:
			g_value_set_string (value, priv->user);
			break;
		case PROP_NICKNAME:
			g_value_set_string (value, priv->nickname);
			break;
		case PROP_EDITED:
			g_value_set_boxed (value, &(priv->edited));
			break;
		case PROP_NAME:
			g_value_set_string (value, priv->name);
			break;
		case PROP_LOCATION:
			g_value_set_string (value, priv->location);
			break;
		case PROP_VISIBILITY:
			g_value_set_enum (value, priv->visibility);
			break;
		case PROP_TIMESTAMP:
			g_value_set_boxed (value, &(priv->timestamp));
			break;
		case PROP_NUM_PHOTOS:
			g_value_set_uint (value, priv->num_photos);
			break;
		case PROP_NUM_PHOTOS_REMAINING:
			g_value_set_uint (value, priv->num_photos_remaining);
			break;
		case PROP_BYTES_USED:
			g_value_set_long (value, priv->bytes_used);
			break;
		case PROP_IS_COMMENTING_ENABLED:
			g_value_set_boolean (value, priv->is_commenting_enabled);
			break;
		case PROP_COMMENT_COUNT:
			g_value_set_uint (value, priv->comment_count);
			break;
		case PROP_DESCRIPTION:
			g_value_set_string (value, gdata_media_group_get_description (priv->media_group));
			break;
		case PROP_TAGS:
			g_value_set_string (value, gdata_media_group_get_keywords (priv->media_group));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gdata_picasaweb_album_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataPicasaWebAlbum *self = GDATA_PICASAWEB_ALBUM (object);

	switch (property_id) {
		case PROP_LOCATION:
			gdata_picasaweb_album_set_location (self, g_value_get_string (value));
			break;
		case PROP_VISIBILITY:
			gdata_picasaweb_album_set_visibility (self, g_value_get_enum (value));
			break;
		case PROP_TIMESTAMP:
			gdata_picasaweb_album_set_timestamp (self, g_value_get_boxed (value));
			break;
		case PROP_IS_COMMENTING_ENABLED:
			gdata_picasaweb_album_set_is_commenting_enabled (self, g_value_get_boolean (value));
			break;
		case PROP_DESCRIPTION:
			gdata_picasaweb_album_set_description (self, g_value_get_string (value));
			break;
		case PROP_TAGS:
			gdata_picasaweb_album_set_tags (self, g_value_get_string (value));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static gboolean
parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error)
{
	GDataPicasaWebAlbum *self = GDATA_PICASAWEB_ALBUM (parsable);

	if (xmlStrcmp (node->name, (xmlChar*) "group") == 0) {
		/* media:group */
		GDataMediaGroup *group = GDATA_MEDIA_GROUP (_gdata_parsable_new_from_xml_node (GDATA_TYPE_MEDIA_GROUP, "group", doc,
											       node, NULL, error));
		if (group == NULL)
			return FALSE;

		if (self->priv->media_group != NULL)
			/* We should really error here, but we can't, as priv->media_group has to be pre-populated
			 * in order for things like gdata_picasaweb_album_get_tags() to work. */
			g_object_unref (self->priv->media_group);

		self->priv->media_group = group;
	} else if (xmlStrcmp (node->name, (xmlChar*) "user") == 0) {
		/* gphoto:user */
		xmlChar *user = xmlNodeListGetString (doc, node->children, TRUE);
		if (user == NULL || *user == '\0')
			return gdata_parser_error_required_content_missing (node, error);
		xmlFree (self->priv->user);
		self->priv->user = (gchar*) user;
	} else if (xmlStrcmp (node->name, (xmlChar*) "nickname") == 0) {
		/* gphoto:nickname */
		xmlChar *nickname = xmlNodeListGetString (doc, node->children, TRUE);
		if (nickname == NULL || *nickname == '\0')
			return gdata_parser_error_required_content_missing (node, error);
		xmlFree (self->priv->nickname);
		self->priv->nickname = (gchar*) nickname;
	} else if (xmlStrcmp (node->name, (xmlChar*) "edited") == 0) {
		/* app:edited */
		xmlChar *edited = xmlNodeListGetString (doc, node->children, TRUE);
		if (g_time_val_from_iso8601 ((gchar*) edited, &(self->priv->edited)) == FALSE) {
			/* Error */
			gdata_parser_error_not_iso8601_format (node, (gchar*) edited, error);
			xmlFree (edited);
			return FALSE;
		}
		xmlFree (edited);
	} else if (xmlStrcmp (node->name, (xmlChar*) "summary") == 0) {
		/* gphoto:summary */
		/* @summary and @description are the same, so they're combined to @description */
		xmlChar *summary = xmlNodeListGetString (doc, node->children, TRUE);
		gdata_picasaweb_album_set_description (self, (gchar*) summary);
		xmlFree (summary);
	} else if (xmlStrcmp (node->name, (xmlChar*) "name") == 0) {
		/* gphoto:name */
		xmlChar *name = xmlNodeListGetString (doc, node->children, TRUE);
		if (name == NULL || *name == '\0') {
			xmlFree (name);
			return gdata_parser_error_required_content_missing (node, error);
		}
		xmlFree (self->priv->name);
		self->priv->name = (gchar*) name;
	} else if (xmlStrcmp (node->name, (xmlChar*) "location") == 0) {
		/* gphoto:location */
		xmlChar *location = xmlNodeListGetString (doc, node->children, TRUE);
		gdata_picasaweb_album_set_location (self, (gchar*) location);
		xmlFree (location);
	} else if (xmlStrcmp (node->name, (xmlChar*) "access") == 0) {
		/* gphoto:access */
		xmlChar *access = xmlNodeListGetString (doc, node->children, TRUE);
		if (xmlStrcmp (access, (xmlChar*) "public") == 0) {
			gdata_picasaweb_album_set_visibility (self, GDATA_PICASAWEB_PUBLIC);
		} else if (xmlStrcmp (access, (xmlChar*) "private") == 0) {
			gdata_picasaweb_album_set_visibility (self, GDATA_PICASAWEB_PRIVATE);
		} else {
			gdata_parser_error_unknown_content (node, (gchar*) access, error);
			xmlFree (access);
			return FALSE;
		}
		xmlFree (access);
	} else if (xmlStrcmp (node->name, (xmlChar*) "timestamp") == 0) {
		/* gphoto:timestamp */
		xmlChar *timestamp_str;
		gulong milliseconds;
		GTimeVal timestamp;

		timestamp_str = xmlNodeListGetString (doc, node->children, TRUE);
		milliseconds = strtoull ((char*) timestamp_str, NULL, 10);
		xmlFree (timestamp_str);

		timestamp.tv_sec = (glong) (milliseconds / 1000);
		timestamp.tv_usec = (glong) ((milliseconds % 1000) * 1000);
		gdata_picasaweb_album_set_timestamp (self, &timestamp);
	} else if (xmlStrcmp (node->name, (xmlChar*) "numphotos") == 0) {
		/* gphoto:numphotos */
		xmlChar *num_photos = xmlNodeListGetString (doc, node->children, TRUE);
		if (num_photos == NULL || *num_photos == '\0')
			return gdata_parser_error_required_content_missing (node, error);
		self->priv->num_photos = strtoul ((char*) num_photos, NULL, 10);
		xmlFree (num_photos);
	} else if (xmlStrcmp (node->name, (xmlChar*) "numphotosremaining") == 0) {
		/* gphoto:numphotosremaining */
		xmlChar *num_photos_remaining = xmlNodeListGetString (doc, node->children, TRUE);
		if (num_photos_remaining == NULL || *num_photos_remaining == '\0')
			return gdata_parser_error_required_content_missing (node, error);
		self->priv->num_photos_remaining = strtoul ((char*) num_photos_remaining, NULL, 10);
		xmlFree (num_photos_remaining);
	} else if (xmlStrcmp (node->name, (xmlChar*) "bytesUsed") == 0) {
		/* gphoto:bytesUsed */
		xmlChar *bytes_used = xmlNodeListGetString (doc, node->children, TRUE);
		if (bytes_used == NULL || *bytes_used == '\0')
			return gdata_parser_error_required_content_missing (node, error);
		self->priv->bytes_used = strtol ((char*) bytes_used, NULL, 10);
		xmlFree (bytes_used);
	} else if (xmlStrcmp (node->name, (xmlChar*) "commentingEnabled") == 0) {
		/* gphoto:commentingEnabled */
		xmlChar *commenting_enabled = xmlNodeListGetString (doc, node->children, TRUE);
		if (commenting_enabled == NULL || *commenting_enabled == '\0')
			return gdata_parser_error_required_content_missing (node, error);
		gdata_picasaweb_album_set_is_commenting_enabled (self, (xmlStrcmp (commenting_enabled, (xmlChar*) "true") == 0) ? TRUE : FALSE);
		xmlFree (commenting_enabled);
	} else if (xmlStrcmp (node->name, (xmlChar*) "commentCount") == 0) {
		xmlChar *comment_count = xmlNodeListGetString (doc, node->children, TRUE);
		if (comment_count == NULL || *comment_count == '\0')
			return gdata_parser_error_required_content_missing (node, error);
		self->priv->comment_count = strtoul ((char*) comment_count, NULL, 10);
		xmlFree (comment_count);
	} else if (GDATA_PARSABLE_CLASS (gdata_picasaweb_album_parent_class)->parse_xml (parsable, doc, node, user_data, error) == FALSE) {
		/* Error! */
		return FALSE;
	}

	return TRUE;
}

static void
get_xml (GDataParsable *parsable, GString *xml_string)
{
	gchar *xml;
	GDataPicasaWebAlbumPrivate *priv = GDATA_PICASAWEB_ALBUM (parsable)->priv;

	/* Chain up to the parent class */
	GDATA_PARSABLE_CLASS (gdata_picasaweb_album_parent_class)->get_xml (parsable, xml_string);

	/* Add all the album-specific XML */
	/* TODO: gphoto:name?, gphoto:id */
	if (priv->location != NULL) {
		gchar *location = g_markup_escape_text (priv->location, -1);
		g_string_append_printf (xml_string, "<gphoto:location>%s</gphoto:location>", location);
		g_free (location);
	}

	if (priv->visibility == GDATA_PICASAWEB_PUBLIC)
		g_string_append (xml_string, "<gphoto:access>public</gphoto:access>");
	else if (priv->visibility == GDATA_PICASAWEB_PRIVATE)
		g_string_append (xml_string, "<gphoto:access>private</gphoto:access>");
	else
		g_assert_not_reached ();

	if (priv->timestamp.tv_sec != 0 || priv->timestamp.tv_usec != 0) {
		/* in milliseconds */
		g_string_append_printf (xml_string, "<gphoto:timestamp>%lu</gphoto:timestamp>",
					priv->timestamp.tv_sec * 1000 + priv->timestamp.tv_usec);
	}

	if (priv->is_commenting_enabled == FALSE)
		g_string_append (xml_string, "<gphoto:commentingEnabled>false</gphoto:commentingEnabled>");
	else
		g_string_append (xml_string, "<gphoto:commentingEnabled>true</gphoto:commentingEnabled>");

	/* media:group */
	xml = _gdata_parsable_get_xml (GDATA_PARSABLE (priv->media_group), "media:group", FALSE);
	g_string_append (xml_string, xml);
	g_free (xml);

	/* TODO: add GML support */
	/* TODO:
	 * - Finish supporting all tags
	 * - Check all tags here are valid for insertions and updates
	 * - Check things are escaped (or not) as appropriate
	 * - Write a function to encapsulate g_markup_escape_text and
	 *   g_string_append_printf to reduce the number of allocations
	 */
}

static void
get_namespaces (GDataParsable *parsable, GHashTable *namespaces)
{
	GDataPicasaWebAlbumPrivate *priv = GDATA_PICASAWEB_ALBUM (parsable)->priv;

	/* Chain up to the parent class */
	GDATA_PARSABLE_CLASS (gdata_picasaweb_album_parent_class)->get_namespaces (parsable, namespaces);

	g_hash_table_insert (namespaces, (gchar*) "gphoto", (gchar*) "http://schemas.google.com/photos/2007");
	g_hash_table_insert (namespaces, (gchar*) "app", (gchar*) "http://www.w3.org/2007/app");

	/* Add the media:group namespaces */
	GDATA_PARSABLE_GET_CLASS (priv->media_group)->get_namespaces (GDATA_PARSABLE (priv->media_group), namespaces);
}

/**
 * gdata_picasaweb_album_new:
 * @id: the album's ID, or %NULL
 *
 * Creates a new #GDataPicasaWebAlbum with the given ID and default properties.
 *
 * Return value: a new #GDataPicasaWebAlbum; unref with g_object_unref()
 *
 * Since: 0.4.0
 **/
GDataPicasaWebAlbum *
gdata_picasaweb_album_new (const gchar *id)
{
	return g_object_new (GDATA_TYPE_PICASAWEB_ALBUM, "id", id, NULL);
}

/**
 * gdata_picasaweb_album_new_from_xml:
 * @xml: an XML string
 * @length: the length in characters of @xml, or %-1
 * @error: a #GError, or %NULL
 *
 * Creates a new #GDataPicasaWebAlbum from an XML string. If @length is %-1, the length of
 * the string will be calculated.
 *
 * Errors from #GDataParserError can be returned if problems are found in the XML.
 *
 * Return value: a new #GDataPicasaWebAlbum, or %NULL; unref with g_object_unref()
 *
 * Since: 0.4.0
 **/
GDataPicasaWebAlbum *
gdata_picasaweb_album_new_from_xml (const gchar *xml, gint length, GError **error)
{
	return GDATA_PICASAWEB_ALBUM (_gdata_entry_new_from_xml (GDATA_TYPE_PICASAWEB_ALBUM, xml, length, error));
}

/**
 * gdata_picasaweb_album_get_user:
 * @self: a #GDataPicasaWebAlbum
 *
 * Gets the #GDataPicasaWebAlbum:user property.
 *
 * Return value: the album owner's username
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_picasaweb_album_get_user (GDataPicasaWebAlbum *self)
{
	g_return_val_if_fail (GDATA_IS_PICASAWEB_ALBUM (self), NULL);
	return self->priv->user;
}

/**
 * gdata_picasaweb_album_get_nickname:
 * @self: a #GDataPicasaWebAlbum
 *
 * Gets the #GDataPicasaWebAlbum:nickname property.
 *
 * Return value: the album owner's nickname
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_picasaweb_album_get_nickname (GDataPicasaWebAlbum *self)
{
	g_return_val_if_fail (GDATA_IS_PICASAWEB_ALBUM (self), NULL);
	return self->priv->nickname;
}

/**
 * gdata_picasaweb_album_get_edited:
 * @self: a #GDataPicasaWebAlbum
 * @edited: a #GTimeVal
 *
 * Gets the #GDataPicasaWebAlbum:edited property and puts it in @edited. If the property is unset,
 * both fields in the #GTimeVal will be set to %0.
 *
 * Since: 0.4.0
 **/
void
gdata_picasaweb_album_get_edited (GDataPicasaWebAlbum *self, GTimeVal *edited)
{
	g_return_if_fail (GDATA_IS_PICASAWEB_ALBUM (self));
	g_return_if_fail (edited != NULL);
	*edited = self->priv->edited;
}

/**
 * gdata_picasaweb_album_get_name:
 * @self: a #GDataPicasaWebAlbum
 *
 * Gets the #GDataPicasaWebAlbum:name property.
 *
 * Return value: the album's name, as usable in URIs, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_picasaweb_album_get_name (GDataPicasaWebAlbum *self)
{
	g_return_val_if_fail (GDATA_IS_PICASAWEB_ALBUM (self), NULL);
	return self->priv->name;
}

/**
 * gdata_picasaweb_album_get_location:
 * @self: a #GDataPicasaWebAlbum
 *
 * Gets the #GDataPicasaWebAlbum:location property.
 *
 * Return value: the album's location, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_picasaweb_album_get_location (GDataPicasaWebAlbum *self)
{
	g_return_val_if_fail (GDATA_IS_PICASAWEB_ALBUM (self), NULL);
	return self->priv->location;
}

/**
 * gdata_picasaweb_album_set_location:
 * @self: a #GDataPicasaWebAlbum
 * @location: the new album location
 *
 * Sets the #GDataPicasaWebAlbum:location property to @location.
 *
 * Set @location to %NULL to unset the property.
 *
 * Since: 0.4.0
 **/
void
gdata_picasaweb_album_set_location (GDataPicasaWebAlbum *self, const gchar *location)
{
	g_return_if_fail (GDATA_IS_PICASAWEB_ALBUM (self));

	g_free (self->priv->location);
	self->priv->location = g_strdup (location);
	g_object_notify (G_OBJECT (self), "location");
}

/**
 * gdata_picasaweb_album_get_visibility:
 * @self: a #GDataPicasaWebAlbum
 *
 * Gets the #GDataPicasaWebAlbum:visibility property.
 *
 * Return value: the album's visibility level
 *
 * Since: 0.4.0
 **/
GDataPicasaWebVisibility
gdata_picasaweb_album_get_visibility (GDataPicasaWebAlbum *self)
{
	g_return_val_if_fail (GDATA_IS_PICASAWEB_ALBUM (self), GDATA_PICASAWEB_PUBLIC);
	return self->priv->visibility;
}

/**
 * gdata_picasaweb_album_set_visibility:
 * @self: a #GDataPicasaWebAlbum
 * @visibility: the new album visibility level
 *
 * Sets the #GDataPicasaWebAlbum:visibility property to @visibility.
 *
 * Since: 0.4.0
 **/
void
gdata_picasaweb_album_set_visibility (GDataPicasaWebAlbum *self, GDataPicasaWebVisibility visibility)
{
	g_return_if_fail (GDATA_IS_PICASAWEB_ALBUM (self));

	self->priv->visibility = visibility;
	g_object_notify (G_OBJECT (self), "visibility");
}

/**
 * gdata_picasaweb_album_get_timestamp:
 * @self: a #GDataPicasaWebAlbum
 * @timestamp: a #GTimeVal
 *
 * Gets the #GDataPicasaWebAlbum:timestamp property and puts it in @timestamp. If the property is unset,
 * both fields in the #GTimeVal will be set to %0.
 *
 * Since: 0.4.0
 **/
void
gdata_picasaweb_album_get_timestamp (GDataPicasaWebAlbum *self, GTimeVal *timestamp)
{
	g_return_if_fail (GDATA_IS_PICASAWEB_ALBUM (self));
	g_return_if_fail (timestamp != NULL);
	*timestamp = self->priv->timestamp;
}

/**
 * gdata_picasaweb_album_set_timestamp:
 * @self: a #GDataPicasaWebAlbum
 * @timestamp: a #GTimeVal, or %NULL
 *
 * Sets the #GDataPicasaWebAlbum:timestamp property from values supplied by @timestamp.
 *
 * Set @timestamp to %NULL to unset the property.
 *
 * Since: 0.4.0
 **/
void
gdata_picasaweb_album_set_timestamp (GDataPicasaWebAlbum *self, GTimeVal *timestamp)
{
	g_return_if_fail (GDATA_IS_PICASAWEB_ALBUM (self));
	if (timestamp == NULL)
		self->priv->timestamp.tv_sec = self->priv->timestamp.tv_usec = 0;
	else
		self->priv->timestamp = *timestamp;
	g_object_notify (G_OBJECT (self), "timestamp");
}

/**
 * gdata_picasaweb_album_get_num_photos:
 * @self: a #GDataPicasaWebAlbum
 *
 * Gets the #GDataPicasaWebAlbum:num-photos property.
 *
 * Return value: the number of photos currently in the album
 *
 * Since: 0.4.0
 **/
guint
gdata_picasaweb_album_get_num_photos (GDataPicasaWebAlbum *self)
{
	g_return_val_if_fail (GDATA_IS_PICASAWEB_ALBUM (self), 0);
	return self->priv->num_photos;
}

/**
 * gdata_picasaweb_album_get_num_photos_remaining:
 * @self: a #GDataPicasaWebAlbum
 *
 * Gets the #GDataPicasaWebAlbum:num-photos-remaining property.
 *
 * Return value: the number of photos that can still be uploaded to the album
 *
 * Since: 0.4.0
 **/
guint
gdata_picasaweb_album_get_num_photos_remaining (GDataPicasaWebAlbum *self)
{
	g_return_val_if_fail (GDATA_IS_PICASAWEB_ALBUM (self), 0);
	return self->priv->num_photos_remaining;
}

/**
 * gdata_picasaweb_album_get_bytes_used:
 * @self: a #GDataPicasaWebAlbum
 *
 * Gets the #GDataPicasaWebAlbum:bytes-used property. It will return %-1 if the current authenticated
 * user is not the owner of the album.
 *
 * Return value: the number of bytes used by the album and its contents, or %-1
 *
 * Since: 0.4.0
 **/
glong
gdata_picasaweb_album_get_bytes_used (GDataPicasaWebAlbum *self)
{
	g_return_val_if_fail (GDATA_IS_PICASAWEB_ALBUM (self), -1);
	return self->priv->bytes_used;
}

/**
 * gdata_picasaweb_album_is_commenting_enabled:
 * @self: a #GDataPicasaWebAlbum
 *
 * Gets the #GDataPicasaWebAlbum:is-commenting-enabled property.
 *
 * Return value: %TRUE if commenting is enabled for the album, %FALSE otherwise
 *
 * Since: 0.4.0
 **/
gboolean
gdata_picasaweb_album_is_commenting_enabled (GDataPicasaWebAlbum *self)
{
	g_return_val_if_fail (GDATA_IS_PICASAWEB_ALBUM (self), FALSE);
	return self->priv->is_commenting_enabled;
}

/**
 * gdata_picasaweb_album_set_is_commenting_enabled:
 * @self: a #GDataPicasaWebAlbum
 * @is_commenting_enabled: %TRUE if commenting should be enabled for the album, %FALSE otherwise
 *
 * Sets the #GDataPicasaWebAlbum:is-commenting-enabled property to @is_commenting_enabled.
 *
 * Since: 0.4.0
 **/
void
gdata_picasaweb_album_set_is_commenting_enabled (GDataPicasaWebAlbum *self, gboolean is_commenting_enabled)
{
	g_return_if_fail (GDATA_IS_PICASAWEB_ALBUM (self));
	self->priv->is_commenting_enabled = is_commenting_enabled;
	g_object_notify (G_OBJECT (self), "is-commenting-enabled");
}

/**
 * gdata_picasaweb_album_get_comment_count:
 * @self: a #GDataPicasaWebAlbum
 *
 * Gets the #GDataPicasaWebAlbum:comment-count property.
 *
 * Return value: the number of comments on the album
 *
 * Since: 0.4.0
 **/
guint
gdata_picasaweb_album_get_comment_count (GDataPicasaWebAlbum *self)
{
	g_return_val_if_fail (GDATA_IS_PICASAWEB_ALBUM (self), 0);
	return self->priv->comment_count;
}

/**
 * gdata_picasaweb_album_get_tags:
 * @self: a #GDataPicasaWebAlbum
 *
 * Gets the #GDataPicasaWebAlbum:tags property.
 *
 * Return value: a comma-separated list of tags associated with all the photos in the album, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_picasaweb_album_get_tags (GDataPicasaWebAlbum *self)
{
	g_return_val_if_fail (GDATA_IS_PICASAWEB_ALBUM (self), NULL);
	return gdata_media_group_get_keywords (self->priv->media_group);
}

/**
 * gdata_picasaweb_album_set_tags:
 * @self: a #GDataPicasaWebAlbum
 * @tags: the new comma-separated list of tags, or %NULL
 *
 * Sets the #GDataPicasaWebAlbum:tags property to @tags.
 *
 * Set @tags to %NULL to unset the album's tag list.
 *
 * Since: 0.4.0
 **/
void
gdata_picasaweb_album_set_tags (GDataPicasaWebAlbum *self, const gchar *tags)
{
	g_return_if_fail (GDATA_IS_PICASAWEB_ALBUM (self));

	gdata_media_group_set_keywords (self->priv->media_group, tags);
	g_object_notify (G_OBJECT (self), "tags");
}

/**
 * gdata_picasaweb_album_get_description:
 * @self: a #GDataPicasaWebAlbum
 *
 * Gets the #GDataPicasaWebAlbum:description property.
 *
 * Return value: the album's long text description, or %NULL
 *
 * Since: 0.4.0
 **/
const gchar *
gdata_picasaweb_album_get_description (GDataPicasaWebAlbum *self)
{
	g_return_val_if_fail (GDATA_IS_PICASAWEB_ALBUM (self), NULL);
	return gdata_media_group_get_description (self->priv->media_group);
}

/**
 * gdata_picasaweb_album_set_description:
 * @self: a #GDataPicasaWebAlbum
 * @description: the album's new description, or %NULL
 *
 * Sets the #GDataPicasaWebAlbum:description property to the new description, @description.
 *
 * Set @description to %NULL to unset the album's description.
 *
 * Since: 0.4.0
 **/
void
gdata_picasaweb_album_set_description (GDataPicasaWebAlbum *self, const gchar *description)
{
	g_return_if_fail (GDATA_IS_PICASAWEB_ALBUM (self));

	/* media:group/media:description is the same as atom:summary */
	gdata_media_group_set_description (self->priv->media_group, description);
	/*gdata_entry_set_summary (GDATA_ENTRY (self), description); TODO function doesn't exist yet */
	g_object_notify (G_OBJECT (self), "description");
}

/**
 * gdata_picasaweb_album_get_contents:
 * @self: a #GDataPicasaWebAlbum
 *
 * Returns a list of media content, such as the cover image for the album.
 *
 * Return value: a #GList of #GDataMediaContent items
 *
 * Since: 0.4.0
 **/
GList *
gdata_picasaweb_album_get_contents (GDataPicasaWebAlbum *self)
{
	g_return_val_if_fail (GDATA_IS_PICASAWEB_ALBUM (self), NULL);
	return gdata_media_group_get_contents (self->priv->media_group);
}

/**
 * gdata_picasaweb_album_get_thumbnails:
 * @self: a #GDataPicasaWebAlbum
 *
 * Returns a list of thumbnails, often at different sizes, for this album.
 *
 * Return value: a #GList of #GDataMediaThumbnail<!-- -->s, or %NULL
 *
 * Since: 0.4.0
 **/
GList *
gdata_picasaweb_album_get_thumbnails (GDataPicasaWebAlbum *self)
{
	g_return_val_if_fail (GDATA_IS_PICASAWEB_ALBUM (self), NULL);
	return gdata_media_group_get_thumbnails (self->priv->media_group);
}
