/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Thibault Saunier <saunierthibault@gmail.com
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
 * SECTION:gdata-documents-feed
 * @short_description: GData documents feed object
 * @stability: Unstable
 * @include: gdata/services/documents/gdata-documents-feed.h
 *
 * #GDataDocumentsFeed is a list of entries (#GDataDocumentsEntry) returned as the result of a query to a #GDataDocumentsService, or given as the input to another
 * operation on the online service. It also has pieces of data associated with the query on the #GDataDocumentsService, such as the query title
 * or timestamp when it was last updated.
 *
 * Each #GDataDocumentsEntry represents a single object on the google documents online service, such as a text documents, presentation documents, spreadsheet documents or
 * folders, and the #GDataDocumentsFeed represents a collection of those objects.
 **/

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <libxml/parser.h>
#include <string.h>

#include "gdata-documents-feed.h"
#include "gdata-types.h"
#include "gdata-private.h"
#include "gdata-service.h"

static gboolean parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *root_node, gpointer user_data, GError **error);
static gboolean is_text_entry (xmlDoc *doc, xmlNode *node);
static gboolean is_spreadsheet_entry (xmlDoc *doc, xmlNode *node);
static gboolean is_presentation_entry (xmlDoc *doc, xmlNode *node);
static gboolean is_folder_entry (xmlDoc *doc, xmlNode *node);

struct _GDataDocumentsFeedPrivate {
	/**/;
};

G_DEFINE_TYPE (GDataDocumentsFeed, gdata_documents_feed, GDATA_TYPE_FEED)
#define GDATA_DOCUMENTS_FEED_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_DOCUMENTS_FEED, GDataDocumentsFeedPrivate))

static void
gdata_documents_feed_class_init (GDataDocumentsFeedClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);
	GDataFeed *feed_class = GDATA_FEED_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataDocumentsFeedPrivate));

	parsable_class->parse_xml = parse_xml;

}

static void
gdata_documents_feed_init (GDataDocumentsFeed *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_DOCUMENTS_FEED, GDataDocumentsFeedPrivate);
}

GDataDocumentsFeed*
_gdata_documents_feed_new_from_xml (GType feed_type, const gchar *xml, gint length, GType entry_type,\
			  GDataQueryProgressCallback progress_callback, gpointer progress_user_data, GError **error)
{
/*	ParseData *data;*/
	GDataDocumentsFeed *feed;

	g_return_val_if_fail (g_type_is_a (feed_type, GDATA_TYPE_DOCUMENTS_FEED) == TRUE, FALSE);
	g_return_val_if_fail (xml != NULL, NULL);
	g_return_val_if_fail (g_type_is_a (entry_type, GDATA_TYPE_DOCUMENTS_ENTRY) == TRUE, FALSE);

/*	data = g_slice_new (ParseData);
	data->entry_type = entry_type;
	data->progress_callback = progress_callback;
	data->progress_user_data = progress_user_data;
	data->entry_i = 0;
*/	
	feed = GDATA_DOCUMENTS_FEED (_gdata_parsable_new_from_xml (feed_type, "feed", xml, length, progress_user_data, error));
	/*g_slice_free (ParseData, data);*/

	return feed;
}

static gboolean
parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error)
{
	GDataDocumentsFeed *self;
	
	g_return_val_if_fail (GDATA_IS_FEED (parsable), FALSE);
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);

	self = GDATA_DOCUMENTS_FEED (parsable);
	/*ParseData *data = user_data;*/

	if (xmlStrcmp (node->name, (xmlChar*) "entry") == 0) {
		GDataEntry *entry;
		GDataAuthor *author;
		if (is_spreadsheet_entry (doc, node))
		{
			entry = GDATA_DOCUMENTS_SPREADSHEET (_gdata_parsable_new_from_xml_node (GDATA_TYPE_DOCUMENTS_SPREADSHEET, "entry", doc, node, NULL, error));
			
		}
		else if ( is_text_entry (doc, node))
			entry = GDATA_ENTRY (_gdata_parsable_new_from_xml_node (GDATA_TYPE_DOCUMENTS_TEXT, "entry", doc, node, NULL, error));
		else if (is_presentation_entry (doc, node))
			entry = GDATA_ENTRY (_gdata_parsable_new_from_xml_node (GDATA_TYPE_DOCUMENTS_PRESENTATION, "entry", doc, node, NULL, error));
		else if (is_folder_entry (doc, node))
			entry = GDATA_ENTRY (_gdata_parsable_new_from_xml_node (GDATA_TYPE_DOCUMENTS_FOLDER, "entry", doc, node, NULL, error));
		if (entry == NULL)
			return FALSE;

		/* TODO: call the callback function as parse_xml in gdata-feed.c does (may need new private API), and add the entry to the
		 * GDataFeed's list of entries (does need new private API) */

		/* Call the progress callback in the main thread */
		/*if (data->progress_callback != NULL) {
			ProgressCallbackData *progress_data;

			* Build the data for the callback *
			progress_data = g_slice_new (ProgressCallbackData);
			progress_data->progress_callback = data->progress_callback;
			progress_data->progress_user_data = data->progress_user_data;
			progress_data->entry = g_object_ref (entry);
			progress_data->entry_i = data->entry_i;
			progress_data->total_results = MIN (GDATA_FEED(self)->priv->items_per_page, self->priv->total_results);

			*Send the callback; use G_PRIORITY_DEFAULT rather than G_PRIORITY_DEFAULT_IDLE
			*to contend with the priorities used by the callback functions in GAsyncResult *
			g_idle_add_full (G_PRIORITY_DEFAULT, (GSourceFunc) progress_callback_idle, progress_data, NULL);
		}
		data->entry_i++;*/
		author = gdata_documents_entry_get_last_modified_by (entry);
		if (author)
		_gdata_feed_add_entry (self, entry);
	} else if (GDATA_PARSABLE_CLASS (gdata_documents_feed_parent_class)->parse_xml (parsable, doc, node, user_data, error) == FALSE) {
		/* Error! */
		return FALSE;
	}
	return TRUE;
}

static gboolean
is_spreadsheet_entry (xmlDoc *doc, xmlNode *node)
{
	gchar *document_type;
	xmlNodePtr entry_node;

	g_return_val_if_fail (node != NULL, FALSE);
	entry_node = node->children;	

	while (entry_node != NULL) {
		if ( strcmp ((gchar*) entry_node->name, "resourceId") == 0) {
			document_type = (gchar*) xmlGetProp (entry_node, (xmlChar*) "resourceId");
			document_type = xmlNodeListGetString (doc, entry_node->children, TRUE);
			document_type = g_strsplit ((const gchar *)document_type, ":", 2)[0];
			if (document_type == NULL) 
				return FALSE;
			if (strcmp( (const gchar*) document_type, "spreadsheet") == 0)
				return TRUE;
			else
				return FALSE;
		}
		entry_node= entry_node->next;
	}
	return FALSE;
}

static gboolean
is_text_entry (xmlDoc *doc, xmlNode *node)
{
	gchar *document_type;
	xmlNodePtr entry_node;

	g_return_val_if_fail (node != NULL, FALSE);
	entry_node = node->children;	

	while (entry_node != NULL) {
		if ( strcmp ((gchar*) entry_node->name, "resourceId") == 0) {
			document_type = (gchar*) xmlGetProp (entry_node, (xmlChar*) "resourceId");
			document_type = xmlNodeListGetString (doc, entry_node->children, TRUE);
			document_type = g_strsplit ((const gchar *)document_type, ":", 2)[0];
			if (document_type == NULL) 
				return FALSE;
			if (strcmp( (const gchar*) document_type, "document") == 0)
				return TRUE;
			else
				return FALSE;
		}
		entry_node= entry_node->next;
	}
	return FALSE;
}

static gboolean
is_presentation_entry (xmlDoc *doc, xmlNode *node)
{
	gchar *document_type;
	xmlNodePtr entry_node;

	g_return_val_if_fail (node != NULL, FALSE);
	entry_node = node->children;	

	while (entry_node != NULL) {
		if ( strcmp ((gchar*) entry_node->name, "resourceId") == 0) {
			document_type = (gchar*) xmlGetProp (entry_node, (xmlChar*) "resourceId");
			document_type = xmlNodeListGetString (doc, entry_node->children, TRUE);
			document_type = g_strsplit ((const gchar *)document_type, ":", 2)[0];
			if (document_type == NULL) 
				return FALSE;
			if (strcmp( (const gchar*) document_type, "presentation") == 0)
				return TRUE;
			else
				return FALSE;
		}
		entry_node= entry_node->next;
	}
	return FALSE;
}

static gboolean
is_folder_entry (xmlDoc *doc, xmlNode *node)
{
	gchar *document_type;
	xmlNodePtr entry_node;

	g_return_val_if_fail (node != NULL, FALSE);
	entry_node = node->children;	

	while (entry_node != NULL) {
		if ( strcmp ((gchar*) entry_node->name, "resourceId") == 0) {
			document_type = (gchar*) xmlGetProp (entry_node, (xmlChar*) "resourceId");
			document_type = xmlNodeListGetString (doc, entry_node->children, TRUE);
			document_type = g_strsplit ((const gchar *)document_type, ":", 2)[0];
			if (document_type == NULL) 
				return FALSE;
			if (strcmp( (const gchar*) document_type, "folder") == 0)
				return TRUE;
			else
				return FALSE;
		}
		entry_node= entry_node->next;
	}
	return FALSE;
}
