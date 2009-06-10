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
 * url="http://code.google.com/apis/contacts/docs/2.0/reference.html#Parameters">online documentation</ulink>.
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
	gboolean deleted;
	gboolean starred;
	gboolean metadata;
	gboolean content;
	gboolean only_sharred;
	gboolean show_folder;
	gchar *folder_id;
	gchar *title;
	gchar *exact_title;
	gchar *export_format;
	GList *types;
	GList *emails;
};

enum{
	PROP_DELETED = 1,
	PROP_STARRED,
	PROP_METADATA,
	PROP_CONTENT,
	PROP_ONLY_SHARRED,
	PROP_FOLDER_ID,
	PROP_TITLE,
	PROP_EXACT_TITLE,
	PROP_EXPORT_FORMAT,
	PROP_TYPES,
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
	 * GDataDocumentsQuery:deleted:
	 *
	 * A shortcut to request all documents that have been deleted.
	 **/
	g_object_class_install_property (gobject_class, PROP_DELETED,
				g_param_spec_boolean ("deleted",
					"Deleted?", "A shortcut to request all documents that have been deleted.",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataDocumentsQuery:starred:
	 *
	 * A shortcut to request all starred documents.
	 **/
	g_object_class_install_property (gobject_class, PROP_STARRED,
				g_param_spec_boolean ("starred",
					"Starred?", "A shortcut to request all starred documents.",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataDocumentsQuery:metadata:
	 *
	 * Specifies if the request is with or without metadatas
	 **/
	g_object_class_install_property (gobject_class, PROP_METADATA,
				g_param_spec_boolean ("metadata",
					"Metadata?", "Specifies if the request is with or without metadatas.",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataDocumentsQuery:content:
	 *
	 * Specifies if the request is with or without the content
	 **/
	g_object_class_install_property (gobject_class, PROP_CONTENT,
				g_param_spec_boolean ("content",
					"Content?", "Specifies if the request is with or without content.",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataDocumentsQuery:only_sharred:
	 *
	 * Specifies if the request concerned only sharred documents.
	 **/
	g_object_class_install_property (gobject_class, PROP_ONLY_SHARRED,
				g_param_spec_boolean ("only-sharred",
					"Only sharred?", "Specifies if the request concerned only sharred documents.",
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
	 * GDataDocumentsQuery:exact_title:
	 *
	 * Specifies the exact title of the document querried
	 **/
	g_object_class_install_property (gobject_class, PROP_EXACT_TITLE,
				g_param_spec_string ("exact-title",
					"Exact title", "Specifies the exact title of the document querried.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataDocumentsQuery:export_format:
	 *
	 * Specifies the exportation format.
	 **/
	g_object_class_install_property (gobject_class, PROP_EXACT_TITLE,
				g_param_spec_string ("export-format",
					"Export format", "Specifies the exportation format.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataDocumentsQuery:types:
	 *
	 * Specifies about which kind(s) of documents the querry is. Basicly it means: document, spreadsheet, presentation, starred, folder, trashed.
	 **/
	g_object_class_install_property (gobject_class, PROP_TYPES,
				g_param_spec_pointer ("types",
					"Types", "Specifies about what kinds of documents the querry is.",
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataDocumentsQuery:emails:
	 *
	 * Specifies about who the querry is. 
	 **/
	g_object_class_install_property (gobject_class, PROP_EMAILS,
				g_param_spec_pointer ("emails",
					"Emails", "Specifies about who the querry is.",
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
			g_value_set_boolean (value, priv->deleted);
			break;
		case PROP_STARRED:
			g_value_set_boolean (value, priv->starred);
			break;
		case PROP_METADATA:
			g_value_set_boolean (value, priv->metadata);
			break;
		case PROP_CONTENT:
			g_value_set_boolean (value, priv->content);
			break;
		case PROP_FOLDER_ID:
			g_value_set_string (value, priv->folder_id);
			break;
		case PROP_TYPES:
			g_value_set_string (value, priv->types);
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
			gdata_documents_query_set_deleted (self, g_value_get_boolean (value));
			break;
		case PROP_STARRED:
			gdata_documents_query_set_starred (self, g_value_get_boxed (value));
			break;
		case PROP_METADATA:
			gdata_documents_query_set_metadata (self, g_value_get_boxed (value));
			break;
		case PROP_CONTENT:
			gdata_documents_query_set_content (self, g_value_get_boolean (value));
			break;
		case PROP_FOLDER_ID:
			gdata_documents_query_set_folder_id (self, g_value_get_string (value));
			break;
		case PROP_TYPES:
			gdata_documents_query_set_types (self, g_value_get_string (value));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void 
gdata_documents_query_finalize (GObject *object)
{
	GDataDocumentsQueryPrivate *priv = GDATA_DOCUMENTS_QUERY_GET_PRIVATE (object);

	g_free (priv->folder_id);
	g_free (priv->title);
	g_free (priv->export_format);
	g_free (priv->exact_title);
	g_list_foreach (priv->types, (GFunc) g_free, NULL);
	g_list_foreach (priv->emails, (GFunc) g_free, NULL);
}

static void 
get_query_uri (GDataQuery *self, const gchar *feed_uri, GString *query_uri, gboolean *params_started)
{
	/*TODO*/;
}

gboolean 
gdata_documents_query_get_deleted (GDataDocumentsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_QUERY (self), FALSE);
	return self->priv->deleted;
}

void 
gdata_documents_query_set_deleted (GDataDocumentsQuery *self, gboolean deleted)
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_QUERY (self));
	self->priv->deleted = deleted;
	g_object_notify (G_OBJECT (self), "deleted");
}

gboolean 
gdata_documents_query_get_starred (GDataDocumentsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_QUERY (self), FALSE);
	return self->priv->starred;
}

void 
gdata_documents_query_set_starred (GDataDocumentsQuery *self, gboolean starred)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_QUERY (self), FALSE);
	self->priv->starred=starred;
	g_object_notify (G_OBJECT (self), "starred");
}

gboolean 
gdata_documents_query_get_metadata (GDataDocumentsQuery *self)
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_QUERY (self));
	return self->priv->metadata;
}

void 
gdata_documents_query_set_metadata (GDataDocumentsQuery *self, gboolean metadata)
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_QUERY (self));
	self->priv->metadata=metadata;
	g_object_notify (G_OBJECT (self), "metadata");
}


gboolean 
gdata_documents_query_get_content (GDataDocumentsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_QUERY (self), FALSE);
	return self->priv->content;
}

void 
gdata_documents_query_set_content (GDataDocumentsQuery *self, gboolean content)
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_QUERY (self));
	self->priv->content = content;
	g_object_notify (G_OBJECT (self), "deleted");
}

gboolean 
gdata_documents_query_get_only_sharred (GDataDocumentsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_QUERY (self), FALSE);
	return self->priv->only_sharred;
}

void 
gdata_documents_query_set_only_sharred (GDataDocumentsQuery *self, gboolean only_sharred)
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_QUERY (self));
	self->priv->only_sharred=only_sharred;
	g_object_notify (G_OBJECT (self), "only-sharred");
}

gboolean 
gdata_documents_query_get_show_folder (GDataDocumentsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_QUERY (self), FALSE);
	return self->priv->show_folder;
}

void 
gdata_documents_query_set_show_folder (GDataDocumentsQuery *self, gboolean show_folder)
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_QUERY (self));
	self->priv->show_folder=show_folder;
	g_object_notify (G_OBJECT (self), "show-folder");
}


gchar*
gdata_documents_query_get_folder_id (GDataDocumentsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_QUERY (self), FALSE);
	return self->priv->folder_id;
}

void 
gdata_documents_query_set_folder_id (GDataDocumentsQuery *self, gchar *folder_id)
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_QUERY (self));
	self->priv->folder_id= g_strdup (folder_id);
	g_object_notify (G_OBJECT (self), "folder-id");
}

gchar*
gdata_documents_query_get_title (GDataDocumentsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_QUERY (self), FALSE);
	return self->priv->title;
}

void 
gdata_documents_query_set_title (GDataDocumentsQuery *self, gchar *title)
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_QUERY (self));
	self->priv->title = g_strdup (title);
	g_object_notify (G_OBJECT (self), "title");

}


gchar*
gdata_documents_query_get_export_format (GDataDocumentsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_QUERY (self), NULL);
	return self->priv->export_format;
}

void 
gdata_documents_query_set_export_format (GDataDocumentsQuery *self, gchar *export_format)
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_QUERY (self));
	self->priv->export_format=strdup (export_format);
	g_object_notify (G_OBJECT (self), "export-format");
}


gchar*
gdata_documents_query_get_exact_title (GDataDocumentsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_QUERY (self), NULL);
	return self->priv->exact_title;
}

void 
gdata_documents_query_set_exact_title (GDataDocumentsQuery *self, gchar *exact_title)
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_QUERY (self));
	self->priv->exact_title = g_strdup (exact_title);
	g_object_notify (G_OBJECT (self), "exact-title");
}


GList*
gdata_documents_query_get_types (GDataDocumentsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_QUERY (self), NULL);
	return self->priv->types;
}

void 
gdata_documents_query_set_types (GDataDocumentsQuery *self, GList *type)
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_QUERY (self));
	self->priv->types = type;
	g_object_notify (G_OBJECT (self), "type");
}


GList* 
gdata_documents_query_get_emails (GDataDocumentsQuery *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_QUERY (self), NULL);
	return self->priv->emails;
}

void 
gdata_documents_query_set_emails (GDataDocumentsQuery *self, GList *emails)
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_QUERY (self));
	self->priv->emails=emails;
	g_object_notify (G_OBJECT (self), "emails");
}
