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
 * SECTION:gdata-documents-entry
 * @short_description: GData document entry object abstract class
 * @stability: Unstable
 * @include: gdata/services/document/gdata-documents-entry.h
 *
 * #GDataDocumentsEntry is a subclass of #GDataEntry to represent an entry from a Google Document entry.
 *
 * For more details of Google document' GData API, see the <ulink type="http://code.google.com/apis/document/docs/2.0/developers_guide_protocol.html">
 * online documentation</ulink>.
 **/

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <libxml/parser.h>
#include <string.h>

#include "gdata-documents-entry.h"
#include "gdata-parser.h"
#include "gdata-types.h"
#include "gdata-private.h"
#include  "gdata-access-handler.h"

static void gdata_documents_entry_access_handler_init (GDataAccessHandlerIface *iface);
static void gdata_documents_entry_finalize (GObject *object);
static void gdata_entry_dispose (GObject *object);
static void get_namespaces (GDataParsable *parsable, GHashTable *namespaces);
static void get_xml (GDataParsable *parsable, GString *xml_string);
static void gdata_documents_entry_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_documents_entry_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static gboolean parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error);
void got_chunk_cb (SoupMessage *message, SoupBuffer *chunk, gpointer user_data);


struct _GDataDocumentsEntryPrivate 
{
	GTimeVal edited;
	GTimeVal last_viewed;
	gchar *path;
	gchar *document_id;
	gboolean writers_can_invite;
	GDataAuthor *last_modified_by;
	GDataFeed *access_rules;
};

enum {
	PROP_EDITED = 1,
	PROP_LAST_VIEWED,
	PROP_PATH,
	PROP_DOCUMENT_ID,
	PROOP_LAST_MODIFIED_BY,
	PROP_WRITERS_CAN_INVITE,
	PROP_ACCESS_RULES_URI,
	PROP_ACCESS_RULES,
	PROP_LAST_MODIFIED_BY,
	PROP_LOCAL_FILE
};

G_DEFINE_TYPE_WITH_CODE (GDataDocumentsEntry, gdata_documents_entry, GDATA_TYPE_ENTRY,\
	   	G_IMPLEMENT_INTERFACE (GDATA_TYPE_ACCESS_HANDLER, (GInterfaceInitFunc) gdata_documents_entry_access_handler_init ))
#define GDATA_DOCUMENTS_ENTRY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_DOCUMENTS_ENTRY, GDataDocumentsEntryPrivate))

static void
gdata_documents_entry_class_init (GDataDocumentsEntryClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataDocumentsEntryPrivate));

	gobject_class->set_property = gdata_documents_entry_set_property;
	gobject_class->get_property = gdata_documents_entry_get_property;
	gobject_class->finalize = gdata_documents_entry_finalize;
	gobject_class->dispose = gdata_entry_dispose;

	parsable_class->parse_xml = parse_xml;
	
	parsable_class->get_xml = get_xml;
	parsable_class->get_namespaces = get_namespaces;

	/**
	 * GDataDocumentsEntry::edited
	 *
	 * The last time the documentsEntry was edited. If the documentsEntry has not been edited yet, the content indicates the time it was created.
	 *
	 * For more information, see the <ulink type="http" url="http://www.atomenabled.org/developers/protocol/#appEdited">
	 * Atom Publishing Protocol specification</ulink>.
	 **/
	g_object_class_install_property (gobject_class, PROP_EDITED,
				g_param_spec_boxed ("edited",
					"Edited", "The last time the document entry was edited.",
					GDATA_TYPE_G_TIME_VAL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataDocumentsEntry::last-viewed
	 *
	 * The last time the documentsEntry has been view.
	 *
	 **/
	g_object_class_install_property (gobject_class, PROP_LAST_VIEWED,
				g_param_spec_boxed ("last-viewed",
					"Last viewed", "The last time the document entry has been viewed.",
					GDATA_TYPE_G_TIME_VAL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataDocumentsEntry:writers-can-invite:
	 *
	 * Indicates whether the document entry writers can invite others to write in the document.
	 **/
	g_object_class_install_property (gobject_class, PROP_WRITERS_CAN_INVITE,
				g_param_spec_boolean ("writers-can-invite",
					"Writer can invite?", "Indicates whether writers can invite or not.",
					FALSE,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	/**
	 * GDataDocumentsEntry:path
	 *
	 * Indicates in what path the documentsEntry is.
	 **/
	g_object_class_install_property (gobject_class, PROP_PATH,
				g_param_spec_string ("path",
					"Path", "Indicates in what path the documentsEntry is.",
					NULL,
					G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataDocumentsEntry:document-id
	 *
	 * Indicates the id of the entry.
	 **/
	g_object_class_install_property (gobject_class, PROP_DOCUMENT_ID,
				g_param_spec_string ("document-id",
					"Document id", "Indicates the id of the entry.",
					NULL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataDocumentsEntry:last-modified-by
	 *
	 * Indicates the author of the last modification. It points to a #GDataAuthor.
	 **/
	g_object_class_install_property (gobject_class, PROP_LAST_MODIFIED_BY,
				g_param_spec_object ("last-modified-by",
					"Last modified by", "Indicates the author of the last modification.", 
					GDATA_TYPE_AUTHOR,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
	/**
	 * GDataDocumentsEntry:access-rules
	 *
	 * #GDataFeed containing the document' ACLs.
	 **/
	g_object_class_install_property (gobject_class, PROP_LAST_MODIFIED_BY,
				g_param_spec_object ("access-rules",
					"Access rules", "#GDataFeed containing the document' ACLs.", 
					GDATA_TYPE_FEED,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_documents_entry_init (GDataDocumentsEntry *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_DOCUMENTS_ENTRY, GDataDocumentsEntryPrivate);
}

static gboolean
is_owner_rule (GDataAccessRule *rule)
{
	return (strcmp (gdata_access_rule_get_role (rule), "owner") == 0) ? TRUE : FALSE;
}

static void 
gdata_documents_entry_access_handler_init (GDataAccessHandlerIface *iface)
{
	iface->is_owner_rule = is_owner_rule;
}

GDataDocumentsEntry *
gdata_documents_entry_new(const gchar *id)
{
	return g_object_new (GDATA_TYPE_DOCUMENTS_ENTRY, "id", id, NULL);
}

/**
 * gdata_documents_entry_new_from_xml:
 * @xml: an XML string
 * @length: the length in characters of @xml, or %-1
 * @error: a #GError, or %NULL
 *
 * Creates a new #GDataDocumentsEntry from an XML string. If @length is %-1, the length of
 * the string will be calculated.
 *
 * Errors from #GDataParserError can be returned if problems are found in the XML.
 *
 * Return value: a new #GDataDocumentsEntry, or %NULL; unref with g_object_unref()
 **/
GDataDocumentsEntry *
gdata_documents_entry_new_from_xml (const gchar *xml, gint length, GError **error)
{
	return GDATA_DOCUMENTS_ENTRY (_gdata_entry_new_from_xml (GDATA_TYPE_DOCUMENTS_ENTRY, xml, length, error));
}

static gboolean
parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error)
{
	GDataDocumentsEntry *self;

	self = GDATA_DOCUMENTS_ENTRY (parsable);

	if (xmlStrcmp (node->name, (xmlChar*) "edited") == 0) {
		xmlChar *edited = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		if (g_time_val_from_iso8601 ((gchar*) edited, &(self->priv->edited)) == FALSE) {
			gdata_parser_error_not_iso8601_format (node, (gchar*) edited, error);
			xmlFree (edited);
			return FALSE;
		}
		xmlFree (edited);
	} else if (xmlStrcmp (node->name, (xmlChar*) "lastViewed") == 0) {
		xmlChar *last_viewed = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		if (g_time_val_from_iso8601 ((gchar*) last_viewed, &(self->priv->last_viewed)) == FALSE) {
			gdata_parser_error_not_iso8601_format (node, (gchar*) last_viewed, error);
			xmlFree (last_viewed);
			return FALSE;
		}
		xmlFree (last_viewed);
	} else if (xmlStrcmp (node->name, (xmlChar*) "writersCanInvite") ==  0) {
		xmlChar *_writers_can_invite = xmlGetProp (node, (xmlChar*) "value");
		if (xmlStrcmp (_writers_can_invite, (xmlChar*) "true") == 0)
			gdata_documents_entry_set_writers_can_invite (self, TRUE);
		else if (xmlStrcmp (_writers_can_invite, (xmlChar*) "false") == 0)
			gdata_documents_entry_set_writers_can_invite (self, FALSE);
		else
			return gdata_parser_error_unknown_property_value (node, "value", (const gchar*) _writers_can_invite, error);
		xmlFree (_writers_can_invite);
	} else if (xmlStrcmp (node->name, (xmlChar*) "resourceId") ==  0) {
		gchar **document_id, *document_id_str;
		xmlChar *ressource_id;

		ressource_id = xmlNodeListGetString (doc, node->children, TRUE);
		g_return_val_if_fail (ressource_id != NULL, FALSE);
		document_id_str = g_strdup ((gchar*) ressource_id);
		document_id = g_strsplit (document_id_str, ":", 2);
		gdata_documents_entry_set_document_id  (self, document_id[1]);
		xmlFree (ressource_id);
		g_free (document_id);
		g_free (document_id_str);
	} else if (xmlStrcmp (node->name, (xmlChar*) "feedLink") ==  0) {
		GDataLink *link = GDATA_LINK (_gdata_parsable_new_from_xml_node (GDATA_TYPE_LINK, "feedLink", doc, node, NULL, error));
		if (link == NULL)
			return FALSE;
		gdata_entry_add_link (GDATA_ENTRY (self), link); 
	} else if (xmlStrcmp (node->name, (xmlChar*) "lastModifiedBy") ==  0) {
		GDataAuthor *last_modified_by = GDATA_AUTHOR (_gdata_parsable_new_from_xml_node (GDATA_TYPE_AUTHOR, "lastModifiedBy", doc, node, NULL, error));
		if (last_modified_by == NULL)
			return FALSE;
		self->priv->last_modified_by = last_modified_by;
	} else if (GDATA_PARSABLE_CLASS (gdata_documents_entry_parent_class)->parse_xml (parsable, doc, node, user_data, error) == FALSE) {
		/* Error! */
		return FALSE;
	}

	return TRUE;
}

static void
gdata_documents_entry_finalize (GObject *object)
{
	GDataDocumentsEntryPrivate *priv = GDATA_DOCUMENTS_ENTRY_GET_PRIVATE (object);

	g_free (priv->path);
	g_free (priv->document_id);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_documents_entry_parent_class)->finalize (object);
}

static void
gdata_entry_dispose (GObject *object)
{
	GDataDocumentsEntryPrivate *priv = GDATA_DOCUMENTS_ENTRY_GET_PRIVATE (object);

	if (priv->last_modified_by != NULL)
		g_object_unref (priv->last_modified_by);
	priv->last_modified_by = NULL;

	if (priv->access_rules != NULL)
		g_object_unref (priv->access_rules); 
	priv->access_rules = NULL; 
}

static void
gdata_documents_entry_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataDocumentsEntryPrivate *priv = GDATA_DOCUMENTS_ENTRY_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_PATH:
			g_value_set_string (value, priv->path);
			break;
		case PROP_DOCUMENT_ID:
			g_value_set_string (value, priv->document_id);
			break;
		case PROP_WRITERS_CAN_INVITE:
			g_value_set_boolean (value, priv->writers_can_invite);
			break;
		case PROP_EDITED:
			g_value_set_boxed (value, &(priv->edited));
			break;
		case PROP_LAST_VIEWED:
			g_value_set_boxed (value, &(priv->last_viewed));
			break;
		case PROP_LAST_MODIFIED_BY:
			g_value_set_pointer (value, priv->last_modified_by);
			break;
		case PROP_ACCESS_RULES:
			g_value_set_object (value, priv->access_rules);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
gdata_documents_entry_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GDataDocumentsEntry *self = GDATA_DOCUMENTS_ENTRY (object);

	switch (property_id) {
		case PROP_PATH:
			gdata_documents_entry_set_path (self);
			break;
		case PROP_DOCUMENT_ID:
			gdata_documents_entry_set_document_id (self, g_value_get_string (value));
			break;
		case PROP_WRITERS_CAN_INVITE:
			gdata_documents_entry_set_writers_can_invite (self, g_value_get_boolean (value));
			break;
		/*case PROP_ACCESS_RULES:
			gdata_documents_entry_set_access_rules (self, g_value_get_object (value));
			break; TODO*/
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void 
get_xml (GDataParsable *parsable, GString *xml_string)
{ 
  GDataDocumentsEntryPrivate *priv = GDATA_DOCUMENTS_ENTRY (parsable)->priv;
  gchar *title;
  GList *categories;
 
  title = g_markup_escape_text (gdata_entry_get_title (GDATA_ENTRY (parsable)), -1);
  g_string_append_printf (xml_string, "<title type='text'>%s</title>", title);
  g_free (title);
 
  for (categories = gdata_entry_get_categories (GDATA_ENTRY (parsable)); categories != NULL; categories = categories->next) {
    GDataCategory *category = (GDataCategory*) categories->data;
 
    if (strcmp (gdata_category_get_scheme (category), "http://schemas.google.com/g/2005#kind") == 0) {
      g_string_append_printf (xml_string, "<category term='%s'", gdata_category_get_term (category));
 
      if (G_LIKELY (gdata_category_get_scheme (category) != NULL))
        g_string_append_printf (xml_string, " scheme='%s'", gdata_category_get_scheme (category));
 
      if (G_UNLIKELY (gdata_category_get_label (category) != NULL)) {
        gchar *label = g_markup_escape_text (gdata_category_get_label (category), -1);
        g_string_append_printf (xml_string, " label='%s'", label);
        g_free (label);
      }
 
      g_string_append (xml_string, "/>");
    }
  }
 
  if (priv->writers_can_invite == TRUE)
    g_string_append (xml_string, "<docs:writersCanInvite value='true'/>");
  else
    g_string_append (xml_string, "<docs:writersCanInvite value='false'/>");
}


static void
get_namespaces (GDataParsable *parsable, GHashTable *namespaces)
{
	/* Chain up to the parent class */
	GDATA_PARSABLE_CLASS (gdata_documents_entry_parent_class)->get_namespaces (parsable, namespaces);

	g_hash_table_insert (namespaces, (gchar*) "docs", (gchar*) "http://schemas.google.com/docs/2007");

}

/** 
 * gdata_documents_entry_get_edited:
 * @self: a #GDataDocumentsEntry
 * @edited: a #GTimeVal
 *
 * Gets the #GDataDocumentsEntry:edited property and puts it in @edited. If the property is unset,
 * both fields in the #GTimeVal will be set to %0.
 **/
void
gdata_documents_entry_get_edited (GDataDocumentsEntry *self, GTimeVal *edited)
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_ENTRY (self));
	g_return_if_fail (edited == NULL);
	edited = &(self->priv->edited);
}

/** 
 * gdata_documents_entry_get_last_viewed:
 * @self: a #GDataDocumentsEntry
 * @last_viewed: a #GTimeVal
 *
 * Gets the #GDataDocumentsEntry:last-viewed property and puts it in @last_viewed. If the property is unset,
 * both fields in the #GTimeVal will be set to %0.
 **/
void
gdata_documents_entry_get_last_viewed (GDataDocumentsEntry *self, GTimeVal *last_viewed)
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_ENTRY (self));
	g_return_if_fail (last_viewed != NULL);
	*last_viewed = self->priv->last_viewed;
}

/**
 * gdata_documents_entry_get_path:
 * @self: a #GDataDocumentsEntry
 *
 * Gets the #GDataDocumentsEntry:path property.
 *
 * Return value: the path in wich the document is.
 **/
gchar *
gdata_documents_entry_get_path (GDataDocumentsEntry *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_ENTRY (self), NULL);

	return self->priv->path;
}

/**
 * gdata_documents_entry_set_path:
 * @self: a #GDataDocumentsEntry
 *
 * Create or recreates the path property with the document properties. 
 **/
void 
gdata_documents_entry_set_path (GDataDocumentsEntry *self)
{
	GList *element, *parent_folders_list;
	g_return_if_fail (GDATA_IS_DOCUMENTS_ENTRY (self));

	g_free (self->priv->path);
	parent_folders_list = gdata_entry_look_up_links (GDATA_ENTRY (self), "http://schemas.google.com/docs/2007#parent");
	for (element = parent_folders_list; element != NULL; element = element->next) {
		if (self->priv->path == NULL)
			self->priv->path = g_strdup (gdata_link_get_title (((GDataLink*) element->data)));
		else
			self->priv->path = g_strconcat (self->priv->path, gdata_link_get_title (((GDataLink*) element->data)), NULL);
	}
	g_object_notify (G_OBJECT (self), "path");
}

/**
 * gdata_documents_entry_get_document_id:
 * @self: a #GDataDocumentsEntry
 *
 * Gets the #GDataDocumentsEntry:document_id property.
 *
 * Return value: the document-id property.
 **/
gchar*
gdata_documents_entry_get_document_id (GDataDocumentsEntry *self )
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_ENTRY (self ), NULL);
	return self->priv->document_id;
}

/**
 * gdata_documents_entry_set_document_id:
 * @self: a #GDataDocumentsEntry
 * @path: a new document_id (or NULL?)
 *
 * Sets the #GDataDocumentsEntry:document-id property to @document_id.
 **/
void 
gdata_documents_entry_set_document_id (GDataDocumentsEntry *self, const gchar *document_id )
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_ENTRY (self ));
	self->priv->document_id = g_strdup (document_id);
	g_object_notify (G_OBJECT (self), "document-id");
}

/**
 * gdata_documents_entry_set_writers_can_invite:
 * @self: a #GDataDocumentsEntry
 * @writers_can_invite: %TRUE if writers can invite other personns to write on the document or %FALSE otherwise
 *
 * Sets the #GDataDocumentsEntry:writers-can-invite to @writers_can_invite.
 **/
void 
gdata_documents_entry_set_writers_can_invite(GDataDocumentsEntry *self, gboolean writers_can_invite)
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_ENTRY (self));
	self->priv->writers_can_invite = writers_can_invite;
	g_object_notify (G_OBJECT (self), "writers-can-invite");
}

/**
 * gdata_documents_entry_get_writers_can_invite:
 * @self: a #GDataDocumentsEntry
 *
 * Gets the #GDataDocumentsEntry:writers_can_invite property.
 *
 * Return value: %TRUE if writers can invite other persons to write on the document, %FALSE otherwise
 **/
gboolean
gdata_documents_entry_get_writers_can_invite (GDataDocumentsEntry *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_ENTRY (self ), FALSE);
	return self->priv->writers_can_invite;
}

/** 
 * gdata_documents_entry_get_last_modified_by:
 * @self: a #GDataDocumentsEntry
 * @last_modified_by: a #GDataAuthor
 *
 * Return value: the last-modified-by property.
 **/
GDataAuthor *
gdata_documents_entry_get_last_modified_by (GDataDocumentsEntry *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_ENTRY (self), NULL );
	return self->priv->last_modified_by;
}

/** 
 * gdata_documents_entry_get_access_rules:
 * @self: a #GDataDocumentsEntry
 *
 * Return value: the GDataFeed acces-rules property or %NULL.
 **/
GDataFeed *
gdata_documents_entry_get_access_rules (GDataDocumentsEntry *self)
{
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_ENTRY (self), NULL);
	return self->priv->access_rules;
}

/**
 * gdata_documents_entry_set_access_rules:
 * @self: a #GDataDocumentsEntry
 * @service: a #GDataService
 * @cancellable: optional #GCancellable object, or %NULL
 * @progress_callback: a #GDataQueryProgressCallback to call when a rule is loaded, or %NULL
 * @progress_user_data: data to pass to the @progress_callback function
 * @error: a #GError, or %NULL
 *
 * Retrieves a #GDataFeed containing all the access rules which apply to the given #GDataDocumentsEntry. Only the owner of a #GDataDocumentsEntry may
 * view its rule feed.
 *
 * If @cancellable is not %NULL, then the operation can be cancelled by triggering the @cancellable object from another thread.
 * If the operation was cancelled, the error %G_IO_ERROR_CANCELLED will be returned.
 *
 * A %GDATA_SERVICE_ERROR_WITH_QUERY will be returned if the server indicates there is a problem with the query.
 *
 * For each rule in the response feed, @progress_callback will be called in the main thread. If there was an error parsing the XML response,
 * a #GDataParserError will be returned.
 *
 * Sets the access_rules with the feeds that has been retrieved.
 **/

void 
gdata_documents_entry_set_access_rules (GDataDocumentsEntry *self, GDataService *service, GCancellable *cancellable, GDataQueryProgressCallback progress_callback,\
										gpointer progress_user_data, GError **error)
{
	g_return_if_fail (GDATA_IS_DOCUMENTS_ENTRY (self));
	/* TODO check how to cast self to GDATA_ACCESS_HANDLER*/
	self->priv->access_rules = gdata_access_handler_get_rules (GDATA_ACCESS_HANDLER (self), service, cancellable, progress_callback, progress_user_data, error);
	g_object_notify (G_OBJECT (self), "access-rules");
}

void 
got_chunk_cb (SoupMessage *message, SoupBuffer *chunk, gpointer user_data)
{
	g_output_stream_write (G_OUTPUT_STREAM (user_data), (const void *) chunk->data, chunk->length, NULL, NULL);
}

/* _gdata_documents_entry_download_document:
 * @self: a #GDataDocumentsEntry
 * @service: an authenticated #GDataDocumentsService
 * @content_type: return location for the document's content type, or %NULL; free with g_free()
 * @link: The link to download the document;
 * @destination_directory: the destination file, if it's wrong an error will be set
 * @file_extension: the extension of the downloading file
 * @replace_file_if_exist: %TRUE if you want to replace the file if it exists, %FALSE otherwise.
 * @cancellable: optional #GCancellable object, or %NULL
 * @error: a #GError, or %NULL
 *
 * Downloads and returns the documents descibed here. If the documents doesn't exist the download document will be an HTML file containing the error explanation.
 *
 * If @cancellable is not %NULL, then the operation can be cancelled by triggering the @cancellable object from another thread.
 * If the operation was cancelled, the error %G_IO_ERROR_CANCELLED will be returned.
 * if @replace_file_if_exist is set to %FALSE, a G_IO_ERROR_EXISTS will be set.
 * if @service isn't authenticated, a GDATA_SERVICE_ERROR_AUTHENTICATION_REQUIRED is set.
 *
 * If there is an error getting the documents, a %GDATA_SERVICE_ERROR_WITH_QUERY error will be returned.
 *
 * Return value: the document's data, or %NULL; free with g_object_unref()
 **/
GFile *
_gdata_documents_entry_download_document (GDataDocumentsEntry *self, GDataService *service, gchar **content_type, gchar *link, GFile *destination_directory,\
						gchar *file_extension, gboolean replace_file_if_exist, GCancellable *cancellable, GError **error)
{
	GDataServiceClass *klass;
	GFileOutputStream *file_stream;
	GFile *destination_file;
	SoupMessage *message;
	guint status;
	const gchar *document_title;
	gchar *filename;

	/* TODO: async version */
	g_return_val_if_fail (GDATA_IS_DOCUMENTS_ENTRY (self), NULL);
	g_return_val_if_fail (GDATA_IS_SERVICE (service), NULL);

	/* Ensure we're authenticated first */
	if (gdata_service_is_authenticated (GDATA_SERVICE (service)) == FALSE) {
		g_set_error_literal (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_AUTHENTICATION_REQUIRED,
				     _("You must be authenticated to querry documents."));
		return NULL;
	}

	/*prepare the GFile*/
	document_title = gdata_entry_get_title (GDATA_ENTRY (self));
	filename = g_strdup_printf ("%s.%s", document_title, file_extension);
	destination_file = g_file_get_child (destination_directory, filename);
	g_free (filename);
	
	/*Check if the file exists*/
	if (g_file_query_exists (destination_file, cancellable) == TRUE) {
		if (replace_file_if_exist == TRUE)
			file_stream = g_file_replace (destination_file, NULL, TRUE, G_FILE_CREATE_NONE, cancellable, error);
		else{
			g_set_error (error, G_IO_ERROR_EXISTS, 1, NULL);
			return NULL;
		}
	}else
		file_stream = g_file_create (destination_file, G_FILE_CREATE_NONE, cancellable, error);

	/*Get the document URI */
	g_assert (link != NULL);
	message = soup_message_new (SOUP_METHOD_GET, link);
	soup_message_body_set_accumulate (message->response_body, FALSE);

	/*Make sure the headers are set */
	klass = GDATA_SERVICE_GET_CLASS (service);
	if (klass->append_query_headers != NULL)
		klass->append_query_headers (GDATA_SERVICE (service), message);

	/*connect the on_chunk signal to write the downloaded parts to the GFile*/
	g_signal_connect (message, "got-chunk", (GCallback) got_chunk_cb, file_stream);

	/* Send the message */
	status = _gdata_service_send_message (GDATA_SERVICE (service), message, error);
	if (status == SOUP_STATUS_NONE) {
		g_object_unref (message);
		return NULL;
	}

	/* Check for cancellation */
	if (g_cancellable_set_error_if_cancelled (cancellable, error) == TRUE) {
		g_object_unref (message);
		return NULL;
	}

	if (status != 200) {
		/* Error */
		g_assert (klass->parse_error_response != NULL);
		klass->parse_error_response (GDATA_SERVICE (service), GDATA_SERVICE_ERROR_WITH_QUERY, status, message->reason_phrase,
					     message->response_body->data, message->response_body->length, error);
		g_object_unref (message);
		return NULL;
	}

	/* Sort out the return values */
	if (content_type != NULL)
		*content_type = g_strdup (soup_message_headers_get_content_type (message->response_headers, NULL));

	g_object_unref (message);
	g_object_unref (file_stream);
	return destination_file;
}

