/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Philip Withnall 2008 <philip@tecnocode.co.uk>
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

#include "gdata-entry.h"
#include "gdata-types.h"
#include "gdata-service.h"
#include "gdata-private.h"

static void gdata_entry_finalize (GObject *object);
static void gdata_entry_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_entry_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

struct _GDataEntryPrivate {
	gchar *title;
	gchar *id;
	GTimeVal updated;
};

enum {
	PROP_TITLE = 1,
	PROP_ID,
	PROP_UPDATED
};

G_DEFINE_TYPE (GDataEntry, gdata_entry, G_TYPE_OBJECT)
#define GDATA_ENTRY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_ENTRY, GDataEntryPrivate))

static void
gdata_entry_class_init (GDataEntryClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataEntryPrivate));

	gobject_class->set_property = gdata_entry_set_property;
	gobject_class->get_property = gdata_entry_get_property;
	gobject_class->finalize = gdata_entry_finalize;

	g_object_class_install_property (gobject_class, PROP_TITLE,
				g_param_spec_string ("title",
					"Title", "The title for this entry.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_ID,
				g_param_spec_string ("id",
					"ID", "The ID for this entry.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_UPDATED,
				g_param_spec_boxed ("updated",
					"Updated", "The last update time for this entry.",
					G_TYPE_TIME_VAL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_entry_init (GDataEntry *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_ENTRY, GDataEntryPrivate);
}

static void
gdata_entry_finalize (GObject *object)
{
	GDataEntryPrivate *priv = GDATA_ENTRY_GET_PRIVATE (object);

	g_free (priv->title);
	g_free (priv->id);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_entry_parent_class)->finalize (object);
}

static void
gdata_entry_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataEntryPrivate *priv = GDATA_ENTRY_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_TITLE:
			g_value_set_string (value, priv->title);
			break;
		case PROP_ID:
			g_value_set_string (value, priv->id);
			break;
		case PROP_UPDATED:
			g_value_set_boxed (value, &(priv->updated));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gdata_entry_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataEntryPrivate *priv = GDATA_ENTRY_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_TITLE:
			gdata_entry_set_title (GDATA_ENTRY (object), g_value_get_string (value));
			break;
		case PROP_ID:
			gdata_entry_set_id (GDATA_ENTRY (object), g_value_get_string (value));
			break;
		case PROP_UPDATED:
			gdata_entry_set_updated (GDATA_ENTRY (object), g_value_get_boxed (value));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

GDataEntry *
gdata_entry_new (void)
{
	return g_object_new (GDATA_TYPE_ENTRY, NULL);
}

const gchar *
gdata_entry_get_title (GDataEntry *self)
{
	g_return_val_if_fail (GDATA_IS_ENTRY (self), NULL);
	return self->priv->title;
}

void
gdata_entry_set_title (GDataEntry *self, const gchar *title)
{
	g_return_if_fail (GDATA_IS_ENTRY (self));
	g_return_if_fail (title != NULL);

	g_free (self->priv->title);
	self->priv->title = g_strdup (title);
	g_object_notify (G_OBJECT (self), "title");
}

const gchar *
gdata_entry_get_id (GDataEntry *self)
{
	g_return_val_if_fail (GDATA_IS_ENTRY (self), NULL);
	return self->priv->id;
}

void
gdata_entry_set_id (GDataEntry *self, const gchar *id)
{
	g_return_if_fail (GDATA_IS_ENTRY (self));
	g_return_if_fail (id != NULL);

	g_free (self->priv->id);
	self->priv->id = g_strdup (id);
	g_object_notify (G_OBJECT (self), "id");
}

void
gdata_entry_get_updated (GDataEntry *self, GTimeVal *updated)
{
	g_return_if_fail (GDATA_IS_ENTRY (self));
	g_return_if_fail (updated != NULL);

	updated->tv_sec = self->priv->updated.tv_sec;
	updated->tv_usec = self->priv->updated.tv_usec;
}

void
gdata_entry_set_updated (GDataEntry *self, GTimeVal *updated)
{
	g_return_if_fail (GDATA_IS_ENTRY (self));
	g_return_if_fail (updated != NULL);

	self->priv->updated.tv_sec = updated->tv_sec;
	self->priv->updated.tv_usec = updated->tv_usec;
	g_object_notify (G_OBJECT (self), "updated");
}

GDataEntry *
_gdata_entry_new_from_xml_node (xmlDoc *doc, xmlNode *node, GError **error)
{
	GDataEntry *entry;

	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);
	g_return_val_if_fail (xmlStrcmp (node->name, (xmlChar*) "entry") == 0, FALSE);

	entry = gdata_entry_new ();

	node = node->xmlChildrenNode;
	while (node != NULL) {
		if (_gdata_entry_parse_xml_node (entry, doc, node, error) == FALSE) {
			g_object_unref (entry);
			return NULL;
		}
		node = node->next;
	}

	return entry;
}

gboolean
_gdata_entry_parse_xml_node (GDataEntry *self, xmlDoc *doc, xmlNode *node, GError **error)
{
	g_return_val_if_fail (GDATA_IS_ENTRY (self), FALSE);
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);

	if (xmlStrcmp (node->name, (xmlChar*) "title") == 0) { /* atom:title */
		xmlChar *title = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		gdata_entry_set_title (self, (gchar*) title);
		xmlFree (title);
	} else if (xmlStrcmp (node->name, (xmlChar*) "id") == 0) { /* atom:id */
		xmlChar *id = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		gdata_entry_set_id (self, (gchar*) id);
		xmlFree (id);
	} else if (xmlStrcmp (node->name, (xmlChar*) "updated") == 0) { /* atom:updated */
		xmlChar *updated;
		GTimeVal updated_timeval;

		updated = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		if (g_time_val_from_iso8601 ((gchar*) updated, &updated_timeval) == FALSE) {
			/* Error */
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
				     _("An <entry>'s <updated> element (\"%s\") was not in ISO8601 format."), updated);
			xmlFree (updated);
			return FALSE;
		}

		gdata_entry_set_updated (self, &updated_timeval);
		xmlFree (updated);
	} else if (xmlStrcmp (node->name, (xmlChar*) "published") == 0) /* atom:published */
		/* TODO */
		self = self;
	else if (xmlStrcmp (node->name, (xmlChar*) "category") == 0) /* atom:category */
		/* TODO */
		self = self;
	else if (xmlStrcmp (node->name, (xmlChar*) "content") == 0) /* atom:content */
		/* TODO */
		self = self;
	else if (xmlStrcmp (node->name, (xmlChar*) "link") == 0) /* atom:link */
		/* TODO */
		self = self;
	else if (xmlStrcmp (node->name, (xmlChar*) "author") == 0) /* atom:author */
		/* TODO */
		self = self;
	else if (xmlStrcmp (node->name, (xmlChar*) "rating") == 0) /* gd:rating */
		/* TODO */
		self = self;
	else if (xmlStrcmp (node->name, (xmlChar*) "comments") == 0) /* gd:comments */
		/* TODO */
		self = self;
	else {
		g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_UNHANDLED_XML_ELEMENT,
			     _("Unhandled <%s:%s> element as a child of <entry>."),
			     node->ns->prefix, node->name);
		return FALSE;
	}

	return TRUE;
}
