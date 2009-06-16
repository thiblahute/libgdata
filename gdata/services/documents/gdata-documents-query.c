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

#include "gdata-documents-query.h"
#include "gdata-query.h"

static void gdata_documents_query_finalize (GObject *object);
static void gdata_documents_query_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_documents_query_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void get_query_uri (GDataQuery *self, const gchar *feed_uri, GString *query_uri, gboolean *params_started);

struct _GDataDocumentsQueryPrivate 
{
	gboolean show_deleted;
	gboolean show_folder;
	gboolean exact_title;
	gchar *folder_id;
	gchar *title;
	gchar *emails;
};

enum{
	PROP_DELETED = 1,
	PROP_ONLY_STARRED,
	PROP_SHOW_FOLDER,
	PROP_EXACT_TITLE,
	PROP_FOLDER_ID,
	PROP_TITLE,
	PROP_EMAILS
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
	 * GDataDocumentsQuery:show_deleted:
	 *
	 * A shortcut to request all documents that have been deleted.
	 **/
	g_object_class_install_property (gobject_class, PROP_DELETED,
				g_param_spec_boolean ("show-deleted",
					"Deleted?", "A shortcut to request all documents that have been deleted.",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataDocumentsQuery:show_folder:
	 *
	 * Specifies if the request also returns  folders.
	 **/
	g_object_class_install_property (gobject_class, PROP_SHOW_FOLDER,
				g_param_spec_boolean ("show-folders",
					"Show folders?", "Specifies if the request also returns folders.",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataDocumentsQuery:exact_title:
	 *
	 * Specifies the exact title of the document querried
	 **/
	g_object_class_install_property (gobject_class, PROP_EXACT_TITLE,
				g_param_spec_boolean ("exact-title",
					"Exact title", "Specifies if the title of the querry is the exact title or not.",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataDocumentsQuery:folder_id:
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
	 * GDataDocumentsQuery:emails:
	 *
	 * Specifies about 
	 **/
	g_object_class_install_property (gobject_class, PROP_EMAILS,
				g_param_spec_string ("emails",
					"Emails", "Specifies the emails of the persons collaborating on the document concerned the querry sperated by '%2C', or %NULL if it is unset.",
					"NULL",
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
		case PROP_SHOW_FOLDER:
			g_value_set_boolean (value, priv->show_folder);
			break;
		case PROP_FOLDER_ID:
			g_value_set_boolean (value, priv->folder_id);
			break;
		case PROP_EXACT_TITLE:
			g_value_set_boolean (value, priv->exact_title);
			break;
		case PROP_TITLE:
			g_value_set_string (value, priv->title);
			break;
		case PROP_EMAILS:
			g_value_set_string (value, priv->emails);
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
		case PROP_SHOW_FOLDER:
			gdata_documents_query_set_show_folder (self, g_value_get_boolean (value));
			break;
		case PROP_EXACT_TITLE:
			gdata_documents_query_set_exact_title (self, g_value_get_boolean (value));
			break;
		case PROP_FOLDER_ID:
			gdata_documents_query_set_folder_id (self, g_value_get_string (value));
			break;
		case PROP_TITLE:
			gdata_documents_query_set_title (self, g_value_get_string (value));
			break;
		case PROP_EMAILS:
			gdata_documents_query_set_emails (self, g_value_get_string (value));
			break;
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
	g_free (priv->emails);
}

static void 
get_query_uri (GDataQuery *self, const gchar *feed_uri, GString *query_uri, gboolean *params_started)
{
	GDataDocumentsQueryPrivate *priv = GDATA_DOCUMENTS_QUERY (self)->priv;
	
	#define APPEND_SEP g_string_append_c (query_uri, (*params_started == FALSE) ? '?' : '&'); *params_started = TRUE;

	if (priv->folder_id != NULL)
		g_string_append_printf (query_uri, "/folder\\%3A=%s", priv->folder_id);

	/* Chain up to the parent class */
	GDATA_QUERY_CLASS (gdata_documents_query_parent_class)->get_query_uri (self, feed_uri, query_uri, params_started);

	if  (priv->emails != NULL){
		APPEND_SEP
		g_string_append_printf (query_uri, "writer=%s", priv->emails);
	}
	if (priv->title != NULL){
		APPEND_SEP
		g_string_append_printf (query_uri, "?title=", priv->title);
		if (priv->exact_title = TRUE){
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
	if (priv->show_folder == TRUE)
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
 * gdata_documents_query_get_show_folder:
 * @self: a #GDataDocumentsQuery
 *
 * Gets the #GDataDocumentsQuery:show-folder property.
 *
 * Return value: %TRUE if the querry takes care about folder, %FALSE otherwise.
 **/
gboolean 
gdata_documents_query_get_show_folder (GDataDocumentsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_QUERY (self), FALSE);
	return self->priv->show_folder;
}

/**
 * gdata_documents_query_get_show_folder:
 * @self: a #GDataDocumentsQuery
 * @show_folder: %TRUE if the querry takes care about folder, %FALSE otherwise.
 *
 * Sets the #GDataDocumentsQuery:show-folder property to show_folder.
 **/
void 
gdata_documents_query_set_show_folder (GDataDocumentsQuery *self, gboolean show_folder)
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_QUERY (self));
	self->priv->show_folder=show_folder;
	g_object_notify (G_OBJECT (self), "show-folder");
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
gdata_documents_query_set_title (GDataDocumentsQuery *self, gchar *title)
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_QUERY (self));
	self->priv->title = g_strdup (title);
	g_object_notify (G_OBJECT (self), "title");
}

/**
 * gdata_documents_query_get_exact_title:
 * @self: a #GDataDocumentsQuery
 *
 * Gets the #GDataDocumentsQuery:exact_title property.
 *
 * Return value: %TRUE if the title is the exact title of the document we are querying, or %FALSE otherwise.
 **/
gboolean
gdata_documents_query_get_exact_title (GDataDocumentsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_QUERY (self), NULL);
	return self->priv->exact_title;
}

/**
 * gdata_documents_query_set_exact_title:
 * @self: a #GDataDocumentsQuery
 * @exact_title: %TRUE if the title is the exact title of the document we are querying, or %FALSE otherwise
 *
 * Sets the #GDataDocumentsQuery:exact_title property to exact_title.
 * */
void 
gdata_documents_query_set_exact_title (GDataDocumentsQuery *self, gboolean exact_title)
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_QUERY (self));
	self->priv->exact_title = exact_title;
	g_object_notify (G_OBJECT (self), "exact-title");
}

/**
 * gdata_documents_query_get_emails:
 * @self: a #GDataDocumentsQuery
 *
 * Gets the #GDataDocumentsQuery:emails property.
 *
 * Return value: the emails of the persons concerned by the querry sperated by "%2C", or %NULL if it is unset
 **/
gchar* 
gdata_documents_query_get_emails (GDataDocumentsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_QUERY (self), NULL);
	return self->priv->emails;
}

/**
 * gdata_documents_query_set_emails:
 * @self: a #GDataDocumentsQuery
 *
 * Sets the #GDataDocumentsQuery:emails property to @emails.
 **/
void 
gdata_documents_query_set_emails (GDataDocumentsQuery *self, gchar *emails)
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_QUERY (self));
	self->priv->emails=emails;
	g_object_notify (G_OBJECT (self), "emails");
}
