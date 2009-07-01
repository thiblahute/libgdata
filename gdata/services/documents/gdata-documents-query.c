/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Thibault Saunier 2009 <saunierthibault@gmail.com>
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
 * SECTION:gdata-documents-query
 * @short_description: GData Documents query object
 * @stability: Unstable
 * @include: gdata/services/documents/gdata-documents-query.h
 *
 * #GDataDocumentsQuery represents a collection of query parameters specific to the Google Document service, which go above and beyond
 * those catered for by #GDataQuery.
 *
 * For more information on the custom GData query parameters supported by #GDataDocumentsQuery, see the <ulink type="http"
 * url="http://code.google.com/apis/documents/docs/2.0/reference.html#Parameters">online documentation</ulink>.
 **/

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <string.h>

#include <gdata/gd/gdata-gd-email-address.h>
#include "gdata-documents-query.h"
#include "gdata-query.h"

static void gdata_documents_query_finalize (GObject *object);
static void gdata_documents_query_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_documents_query_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void get_query_uri (GDataQuery *self, const gchar *feed_uri, GString *query_uri, gboolean *params_started);

struct _GDataDocumentsQueryPrivate 
{
	gboolean show_deleted;
	gboolean show_folders;
	gboolean exact_title;
	gchar *folder_id;
	gchar *title;
	GList *collaborators_address; /*GDataGDEmailAddress*/
	GList *readers_address; /*GDataGDEmailAddress*/
};

enum{
	PROP_DELETED = 1,
	PROP_ONLY_STARRED,
	PROP_SHOW_FOLDERS,
	PROP_EXACT_TITLE,
	PROP_FOLDER_ID,
	PROP_TITLE ,
	PROP_COLLABORATORS_EMAILS,
	PROP_READERS_EMAILS
};

G_DEFINE_TYPE (GDataDocumentsQuery, gdata_documents_query, GDATA_TYPE_QUERY)
#define GDATA_DOCUMENTS_QUERY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_DOCUMENTS_QUERY, GDataDocumentsQueryPrivate))

static void
gdata_documents_query_class_init (GDataDocumentsQueryClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataQueryClass *query_class = GDATA_QUERY_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataDocumentsQueryPrivate));

	gobject_class->set_property = gdata_documents_query_set_property;
	gobject_class->get_property = gdata_documents_query_get_property;
	gobject_class->finalize = gdata_documents_query_finalize;

	query_class->get_query_uri = get_query_uri;

	/**
	 * GDataDocumentsQuery:show-deleted:
	 *
	 * A shortcut to request all documents that have been deleted.
	 **/
	g_object_class_install_property (gobject_class, PROP_DELETED,
				g_param_spec_boolean ("show-deleted",
					"Deleted?", "A shortcut to request all documents that have been deleted.",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataDocumentsQuery:show-folders:
	 *
	 * Specifies if the request also returns folders.
	 **/
	g_object_class_install_property (gobject_class, PROP_SHOW_FOLDERS,
				g_param_spec_boolean ("show-folders",
					"Show folders?", "Specifies if the request also returns folders.",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataDocumentsQuery:exact-title:
	 *
	 * Specifies the exact title of the document querried
	 **/
	g_object_class_install_property (gobject_class, PROP_EXACT_TITLE,
				g_param_spec_boolean ("exact-title",
					"Exact title", "Specifies if the title of the querry is the exact title or not.",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataDocumentsQuery:folder-id:
	 *
	 * Specifies about which folder the querry is.",
	 **/
	g_object_class_install_property (gobject_class, PROP_FOLDER_ID,
				g_param_spec_string ("folder-id",
					"Folder id", "Specifies about which folder the querry is.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataDocumentsQuery:title:
	 *
	 * Specifies a part of the title of the document querried
	 **/
	g_object_class_install_property (gobject_class, PROP_TITLE,
				g_param_spec_string ("title",
					"Title", "Specifies a part of the title of the document querried.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataDocumentsQuery:collaborators-address:
	 *
	 * Specifies about 
	 **/
	g_object_class_install_property (gobject_class, PROP_COLLABORATORS_EMAILS,
				g_param_spec_pointer ("collaborators-address",
					"Collaborators address addresses", "Specifies the address of the persons collaborating on the document, or %NULL if it is unset.",
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	/**
	 * GDataDocumentsQuery:readers-address:
	 *
	 * Specifies about 
	 **/
	g_object_class_install_property (gobject_class, PROP_READERS_EMAILS,
				g_param_spec_pointer ("readers-address",
					"Readers address addresses", "Specifies the address of the persons who can read the document, or %NULL if it is unset.",
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_documents_query_init (GDataDocumentsQuery *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_DOCUMENTS_QUERY, GDataDocumentsQueryPrivate);
}

static void
gdata_documents_query_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataDocumentsQueryPrivate *priv = GDATA_DOCUMENTS_QUERY_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_DELETED:
			g_value_set_boolean (value, priv->show_deleted);
			break;
		case PROP_SHOW_FOLDERS:
			g_value_set_boolean (value, priv->show_folders);
			break;
		case PROP_FOLDER_ID:
			g_value_set_string (value, priv->folder_id);
			break;
		case PROP_EXACT_TITLE:
			g_value_set_boolean (value, priv->exact_title);
			break;
		case PROP_TITLE:
			g_value_set_string (value, priv->title);
			break;
		case PROP_COLLABORATORS_EMAILS:
			g_value_set_pointer (value, priv->collaborators_address);
			break;
		case PROP_READERS_EMAILS:
			g_value_set_pointer (value, priv->readers_address);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}
static void
gdata_documents_query_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataDocumentsQuery *self = GDATA_DOCUMENTS_QUERY (object);

	switch (property_id) {
		case PROP_DELETED:
			gdata_documents_query_set_show_deleted (self, g_value_get_boolean (value));
			break;
		case PROP_SHOW_FOLDERS:
			gdata_documents_query_set_show_folders (self, g_value_get_boolean (value));
			break;
		case PROP_FOLDER_ID:
			gdata_documents_query_set_folder_id (self, g_value_dup_string (value));
			break;
		/*case PROP_TITLE:
			gdata_documents_query_set_title (self, g_value_get_string (value)); TODO 
			break;*/
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}
/**
 * gdata_documents_query_new:
 * @q: a query string
 *
 * Creates a new #GDataDocumentsQuery with its #GDataQuery:q property set to @q.
 *
 * Return value: a new #GDataDocumentsQuery
 **/
GDataDocumentsQuery *
gdata_documents_query_new (const gchar *q)
{
	return g_object_new (GDATA_TYPE_DOCUMENTS_QUERY, "q", q, NULL);
}

/**
 * gdata_documents_query_new_with_limits:
 * @q: a query string
 * @start_index: a one-based start index for the results
 * @max_results: the maximum number of results to return
 *
 * Creates a new #GDataDocumentsQuery with its #GDataQuery:q property set to @q, and the limits @start_index and @max_results
 * applied.
 *
 * Return value: a new #GDataDocumentsQuery
 **/
GDataDocumentsQuery *
gdata_documents_query_new_with_limits (const gchar *q, gint start_index, gint max_results)
{
	return g_object_new (GDATA_TYPE_DOCUMENTS_QUERY,
			     "q", q,
			     "start-index", start_index,
			     "max-results", max_results,
			     NULL);
}

static void 
gdata_documents_query_finalize (GObject *object)
{
	GDataDocumentsQueryPrivate *priv = GDATA_DOCUMENTS_QUERY_GET_PRIVATE (object);

	g_free (priv->folder_id);
	g_free (priv->title);
}

static void 
get_query_uri (GDataQuery *self, const gchar *feed_uri, GString *query_uri, gboolean *params_started)
{
	GDataDocumentsQueryPrivate *priv = GDATA_DOCUMENTS_QUERY (self)->priv;
	
	#define APPEND_SEP g_string_append_c (query_uri, (*params_started == FALSE) ? '?' : '&'); *params_started = TRUE;

	if (priv->folder_id != NULL)
		g_string_append_printf (query_uri, "/folder%%3A=%s", priv->folder_id);

	/* Chain up to the parent class */
	GDATA_QUERY_CLASS (gdata_documents_query_parent_class)->get_query_uri (self, feed_uri, query_uri, params_started);

	if  (priv->collaborators_address != NULL){
		GList *collaborator_address;
		APPEND_SEP
		collaborator_address = priv->collaborators_address;
		g_string_append_printf (query_uri, "writer=%s", gdata_gd_email_address_get_address (collaborator_address->data));
		for (collaborator_address = collaborator_address->next; collaborator_address != NULL; collaborator_address = collaborator_address->next)
			g_string_append_printf (query_uri, ";%s", gdata_gd_email_address_get_address (collaborator_address->data));
	} 
	if  (priv->readers_address != NULL){
		GList *reader_address;
		APPEND_SEP
		reader_address = priv->readers_address;
		g_string_append_printf (query_uri, "writer=%s", gdata_gd_email_address_get_address (reader_address->data));
		for (reader_address = reader_address->next; reader_address != NULL; reader_address = reader_address->next)
			g_string_append_printf (query_uri, ";%s", gdata_gd_email_address_get_address (reader_address->data));
	} 
	if (priv->title != NULL){
		APPEND_SEP
		g_string_append_printf (query_uri, "title=%s", priv->title);
		if (priv->exact_title == TRUE) {
			APPEND_SEP
			g_string_append (query_uri, "title-exact=true");
		}
	}

	APPEND_SEP
	if (priv->show_deleted == TRUE)
		g_string_append (query_uri, "showdeleted=true");
	else
		g_string_append (query_uri, "showdeleted=false");

	APPEND_SEP
	if (priv->show_folders == TRUE)
		g_string_append (query_uri, "showfolders=true");
	else
		g_string_append (query_uri, "showfolders=false");
}

/**
 * gdata_documents_query_get_show_deleted:
 * @self: a #GDataDocumentsQuery
 *ta_documents_query_get_only_sharred
 * Gets the #GDataDocumentsQuery:show_deleted property.
 *
 * Return value: %TRUE if the request takes care of deleted entries otherwise %FALSE
 **/
gboolean 
gdata_documents_query_get_show_deleted (GDataDocumentsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_QUERY (self), FALSE);
	return self->priv->show_deleted;
}

/**
 * gdata_documents_query_set_show_deleted:
 * @self: a #GDataDocumentsQuery
 * @show_deleted: %TRUE if the request takes care of show_deleted entries otherwise %FALSE
 *
 * Sets the #GDataDocumentsQuery:show_deleted property to @show_deleted.
 **/
void 
gdata_documents_query_set_show_deleted (GDataDocumentsQuery *self, gboolean show_deleted)
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_QUERY (self));
	self->priv->show_deleted = show_deleted;
	g_object_notify (G_OBJECT (self), "show-deleted");
}

/**
 * gdata_documents_query_get_show_folders:
 * @self: a #GDataDocumentsQuery
 *
 * Gets the #GDataDocumentsQuery:show-folders property.
 *
 * Return value: %TRUE if the querry takes care about folder, %FALSE otherwise.
 **/
gboolean 
gdata_documents_query_get_show_folders (GDataDocumentsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_QUERY (self), FALSE);
	return self->priv->show_folders;
}

/**
 * gdata_documents_query_set_show_folders:
 * @self: a #GDataDocumentsQuery
 * @show_folders: %TRUE if the querry takes care about folder, %FALSE otherwise.
 *
 * Sets the #GDataDocumentsQuery:show-folders property to show_folders.
 **/
void 
gdata_documents_query_set_show_folders (GDataDocumentsQuery *self, gboolean show_folders)
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_QUERY (self));
	self->priv->show_folders = show_folders;
	g_object_notify (G_OBJECT (self), "show-folders");
}


/**
 * gdata_documents_query_get_folder_id:
 * @self: a #GDataDocumentsQuery
 *
 * Gets the #GDataDocumentsQuery:folder-id property.
 *
 * Return value: The id of the folder concerned by this querry, or %NULL if it is unset
 **/
gchar*
gdata_documents_query_get_folder_id (GDataDocumentsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_QUERY (self), FALSE);
	return self->priv->folder_id;
}

/**
 * gdata_documents_query_set_folder_id:
 * @self: a #GDataDocumentsQuery
 * @folder_id: The id of the folder concerned by this querry, or %NULL if it is unset
 *
 * Sets the #GDataDocumentsQuery:folder-id property to @folder_id.
 **/
void 
gdata_documents_query_set_folder_id (GDataDocumentsQuery *self, gchar *folder_id)
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_QUERY (self));
	self->priv->folder_id= g_strdup (folder_id);
	g_object_notify (G_OBJECT (self), "folder-id");
}

/**
 * gdata_documents_query_get_title:
 * @self: a #GDataDocumentsQuery
 *
 * Gets the #GDataDocumentsQuery:title property.
 *
 * Return value: A part of the title of the document we are querying, or %NULL if it is unset
 **/
gchar*
gdata_documents_query_get_title (GDataDocumentsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_QUERY (self), NULL);
	return self->priv->title;
}

/**
 * gdata_documents_query_set_title:
 * @self: a #GDataDocumentsQuery
 * @title: A part of the title of the document we are querying, or %NULL if it is unset
 *
 * Sets the #GDataDocumentsQuery:title property to @title.
 **/
void 
gdata_documents_query_set_title (GDataDocumentsQuery *self, gchar *title, gboolean exact_title)
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_QUERY (self));
	self->priv->title = g_strdup (title);
	self->priv->exact_title = exact_title;
	g_object_notify (G_OBJECT (self), "exact-title");
	g_object_notify (G_OBJECT (self), "title");
}

/**
 * gdata_documents_query_get_exact_title:
 * @self: a #GDataDocumentsQuery
 *
 * Gets the #GDataDocumentsQuery:exact-title property.
 *
 * Return value: %TRUE if the title is the exact title of the document we are querying, or %FALSE otherwise.
 **/
gboolean
gdata_documents_query_get_exact_title (GDataDocumentsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_QUERY (self), FALSE);
	return self->priv->exact_title;
}

/**
 * gdata_documents_query_get_collaborators_address:
 * @self: a #GDataDocumentsQuery
 *
 * Gets the #GDataDocumentsQuery:collaborators-address property.
 *
 * Return value: a list of #GDataGDEmailAddress of the collaborators concerned by the querry, or %NULL if it is unset.
 **/
GList * 
gdata_documents_query_get_collaborators_address (GDataDocumentsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_QUERY (self), NULL);
	return self->priv->collaborators_address;
}

/**
 * gdata_documents_query_get_readers_address:
 * @self: a #GDataDocumentsQuery
 *
 * Gets the #GDataDocumentsQuery:collaborators-address property.
 *
 * Return value: a list of #GDataGDEmailAddress of the readers concerned by the querry, or %NULL if it is unset.
 **/
GList * 
gdata_documents_query_get_readers_address (GDataDocumentsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_QUERY (self), NULL);
	return self->priv->readers_address;
}

/**
 * gdata_documents_query_add_a_reader_email_address:
 * @self: a #GDataDocumentsQuery
 * @const gchar *reader_address
 *
 * Add @reader_address as #GDataGDEmailAddress to the #readers-address list of. 
 **/
void 
gdata_documents_query_add_a_reader_email_address (GDataDocumentsQuery *self, const gchar *reader_address)
{
	GDataGDEmailAddress *address;
	g_return_if_fail (GDATA_IS_DOCUMENTS_QUERY (self));

	address = gdata_gd_email_address_new (reader_address, "reader", NULL, FALSE);
	self->priv->readers_address = g_list_append (self->priv->readers_address, address);
	g_object_notify (G_OBJECT (self), "readers-address");
}

/**
 * gdata_documents_query_add_a_collaborator_email_address:
 * @self: a #GDataDocumentsQuery
 * @const gchar *collaborator_address
 *
 * Add @collaborator_address as #GDataGDEmailAddress to the #collaborators-address list. 
 **/
void 
gdata_documents_query_add_a_collaborator_email_address (GDataDocumentsQuery *self, const gchar *collaborator_address)
{
	GDataGDEmailAddress *address;
	g_return_if_fail (GDATA_IS_DOCUMENTS_QUERY (self));

	address = gdata_gd_email_address_new (collaborator_address, "collaborator", NULL, FALSE);
	self->priv->readers_address = g_list_append (self->priv->collaborators_address, address);
	g_object_notify (G_OBJECT (self), "collaborators-address");
}
