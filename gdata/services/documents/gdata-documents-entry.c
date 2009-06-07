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
#include "gdata-gdata.h"
#include "gdata-parser.h"
#include "gdata-types.h"
#include "gdata-private.h"
#include  "gdata-access-handler.h"

static void gdata_documents_entry_access_handler_init (GDataAccessHandlerIface *iface);
static void gdata_documents_entry_finalize (GObject *object);
static void get_namespaces (GDataEntry *entry, GHashTable *namespaces);
static void get_xml (GDataEntry *entry, GString *xml_string);
static void gdata_documents_entry_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdata_documents_entry_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static gboolean parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error);


struct _GDataDocumentsEntryPrivate 
{
	GTimeVal edited;
	GTimeVal last_viewed;
	gchar *path;
	gchar *resource_id;
	GHashTable *mime_types;
	gboolean writers_can_invite;
	GDataGDFeedLink *access_rules_uri;
	GDataAuthor *last_modified_by;
	GDataFeed *access_rules;
};

enum {
	PROP_EDITED = 1,
	PROP_LAST_VIEWED,
	PROP_PATH,
	PROP_RESSOURCE_ID,
	PROOP_LAST_MODIFIED_BY,
	PROP_WRITERS_CAN_INVITE,
	PROP_ACCESS_RULES_URI,
	PROP_LAST_MODIFIED_BY
};

G_DEFINE_ABSTRACT_TYPE_WITH_CODE (GDataDocumentsEntry, gdata_documents_entry, GDATA_TYPE_ENTRY,\
	   	G_IMPLEMENT_INTERFACE (GDATA_TYPE_ACCESS_HANDLER, (GInterfaceInitFunc) gdata_documents_entry_access_handler_init ))
#define GDATA_DOCUMENTS_ENTRY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_DOCUMENTS_ENTRY, GDataDocumentsEntryPrivate))

static void
gdata_documents_entry_class_init (GDataDocumentsEntryClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);
	GDataEntryClass *entry_class = GDATA_ENTRY_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataDocumentsEntryPrivate));

	gobject_class->set_property = gdata_documents_entry_set_property;
	gobject_class->get_property = gdata_documents_entry_get_property;
	gobject_class->finalize = gdata_documents_entry_finalize;

	parsable_class->parse_xml = parse_xml;
	
	entry_class->get_xml = get_xml;
	entry_class->get_namespaces = get_namespaces;

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
	 * GDataDocumentsEntry::last_viewed
	 *
	 * The last time the documentsEntry has been view. TODO if never viewed 
	 *
	 **/
	g_object_class_install_property (gobject_class, PROP_LAST_VIEWED,
				g_param_spec_boxed ("last-viewed",
					"Last viewed", "The last time the document entry has been viewed.",
					GDATA_TYPE_G_TIME_VAL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
	/**
	 * GDataDocumentsEntry:writers_can_invite:
	 *
	 * Indicates whether the document entry writers can invite others to write in the document.
	 */
	g_object_class_install_property (gobject_class, PROP_WRITERS_CAN_INVITE,
				g_param_spec_boxed ("writers-can-invite",
					"Writer can invite?", "Indicates whether writers can invite or not.",
					FALSE,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
	
	/**
	 * GDataDocumentsEntry:path
	 *
	 * Indicates in what path the documentsEntry is.
	 **/
	g_object_class_install_property (gobject_class, PROP_PATH,
				g_param_spec_string ("path",
					"Path", "Indicates in what path the documentsEntry is.",
					NULL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataDocumentsEntry:resource_id
	 *
	 * Indicates the of the resourceId.
	 **/
	g_object_class_install_property (gobject_class, PROP_RESSOURCE_ID,
				g_param_spec_string ("resource-id",
					"resource id", "Indicates the resourcesId of the entry.",
					NULL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataDocumentsEntry:access_rules_uri
	 *
	 * A link to the document_entry's acls feed. It points to a #GDataGDFeedLink.
	 *
	 * Indicates the uri to get the documentsEntry access_Rules' feed.
	 **/
	g_object_class_install_property (gobject_class, PROP_ACCESS_RULES_URI,
				g_param_spec_pointer ("access-rules-uri",
					"access rules_uri", "Indicates the uri to get the documentsEntry access rules feed.",
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataDocumentsEntry:last_modified_by
	 *
	 * Indicates the author of the last modification. It points to a #GDataAuthor.
	 **/
	g_object_class_install_property (gobject_class, PROP_LAST_MODIFIED_BY,
				g_param_spec_pointer ("last-modified-by",
					"Last modified by", "Indicates the author of the last modification.", 
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


static void
gdata_documents_entry (GDataDocumentsEntry *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_DOCUMENTS_ENTRY, GDataDocumentsEntryPrivate);
	self->priv->mime_types = NULL;
}

GDataDocumentsEntry*
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

	g_return_val_if_fail (GDATA_IS_DOCUMENTS_ENTRY (parsable), FALSE);
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);

	self = GDATA_DOCUMENTS_ENTRY (parsable);

	if (xmlStrcmp (node->name, (xmlChar*) "edited") == 0) {
		xmlChar *edited = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		if (g_time_val_from_iso8601 ((gchar*) edited, &(self->priv->edited)) == FALSE) {
			gdata_parser_error_not_iso8601_format (node, (gchar*) edited, error);
			xmlFree (edited);
			return FALSE;
		}
		xmlFree (edited);
	}else if (xmlStrcmp (node->name, (xmlChar*) "lastViewed") == 0) {
		xmlChar *last_viewed = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		if (g_time_val_from_iso8601 ((gchar*) last_viewed, &(self->priv->last_viewed)) == FALSE) {
			gdata_parser_error_not_iso8601_format (node, (gchar*) last_viewed, error);
			xmlFree (last_viewed);
			return FALSE;
		}
		xmlFree (last_viewed);
	}else if (xmlStrcmp (node->name, (xmlChar*) "writersCanInvite") ==  0){
		xmlChar *_writers_can_invite = xmlGetProp (node, (xmlChar*) "value");
		if (strcmp ( (char*) _writers_can_invite, (char*) "true") == 0)
			gdata_documents_entry_set_writers_can_invite (self, TRUE);
		else if (strcmp ( (char*) _writers_can_invite, (char*) "false") == 0)
			gdata_documents_entry_set_writers_can_invite (self, FALSE);
		else
			return gdata_parser_error_unknown_property_value ("writersCanWrite", "value", (const gchar*) _writers_can_invite, error);
		g_free (_writers_can_invite);
	}else if (xmlStrcmp (node->name, (xmlChar*) "resourceId") ==  0){
		gdata_documents_entry_set_resource_id  (self, (gchar*) xmlNodeListGetString (doc, node->children, TRUE));
	}else if (xmlStrcmp (node->name, (xmlChar*) "feedLink") ==  0){
		xmlChar *rel, *href, *read_only, *count_hint;

		rel = xmlGetProp (node, (xmlChar*) "rel");
		href = xmlGetProp (node, (xmlChar*) "href");
		read_only = xmlGetProp (node, (xmlChar*) "readOnly");
		count_hint = xmlGetProp (node, (xmlChar*) "countHint");
		gint count_hint_uint;

		if (count_hint == NULL)
			count_hint_uint = 0;
		else
			count_hint_uint = strtoul ((gchar*) count_hint, NULL, 10);
		xmlFree (count_hint);
		
		gdata_gd_feed_link_free (self->priv->access_rules_uri);
		gdata_documents_entry_set_access_rules_uri (self, gdata_gd_feed_link_new ((gchar*) href, (gchar*) rel, count_hint_uint,\
														((xmlStrcmp (read_only, (xmlChar*) "true") == 0) ? TRUE : FALSE)));
	}else if (xmlStrcmp (node->name, (xmlChar*) "lastModifiedBy") ==  0){
		GDataAuthor *last_modified_by;
		xmlNode *last_modified_by_node;
		xmlChar *name = NULL, *email = NULL;

		last_modified_by_node = node->children;
		while (last_modified_by_node != NULL) {
			if (xmlStrcmp (last_modified_by_node->name, (xmlChar*) "name") == 0)
				name = xmlNodeListGetString (doc, last_modified_by_node->children, TRUE);
			else if (xmlStrcmp (last_modified_by_node->name, (xmlChar*) "email") == 0)
				email = xmlNodeListGetString (doc, last_modified_by_node->children, TRUE);
			else{
				gdata_parser_error_unhandled_element (node, error);
				xmlFree (name);
				xmlFree (email);
				return FALSE;
			}
			last_modified_by_node = last_modified_by_node->next;
		}
		gdata_documents_entry_set_last_modified_by (self, gdata_author_new ((gchar*) name, NULL, (gchar*) email));
	}else if (GDATA_PARSABLE_CLASS (gdata_documents_entry_parent_class)->parse_xml (parsable, doc, node, user_data, error) == FALSE) {
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
	g_hash_table_destroy (priv->mime_types);
	g_free (priv->access_rules_uri);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_documents_entry_parent_class)->finalize (object);
}

static void
gdata_documents_entry_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataDocumentsEntryPrivate *priv = GDATA_DOCUMENTS_ENTRY_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_PATH:
			g_value_set_string (value, priv->path);
			break;
		case PROP_RESSOURCE_ID:
			g_value_set_string (value, priv->resource_id);
			break;
		case PROP_WRITERS_CAN_INVITE:
			g_value_set_boolean (value, priv->writers_can_invite);
			break;
		case PROP_ACCESS_RULES_URI:
			g_value_set_pointer (value, priv->access_rules_uri);
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
			gdata_documents_entry_set_path (self, g_value_get_string (value));
			break;
		case PROP_RESSOURCE_ID:
			gdata_documents_entry_set_resource_id (self, g_value_get_string (value));
			break;
		case PROP_WRITERS_CAN_INVITE:
			gdata_documents_entry_set_writers_can_invite (self, g_value_get_boolean (value));
			break;
		case PROP_ACCESS_RULES_URI:
			gdata_documents_entry_set_access_rules_uri (self, g_value_get_pointer (value));
			break;
		case PROP_LAST_MODIFIED_BY:
			gdata_documents_entry_set_last_modified_by (self, g_value_get_pointer (value));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void 
get_xml (GDataEntry *entry, GString *xml_string)
{ 
	GDataEntryPrivate *priv = self->priv;
	;
}

static void
get_namespaces (GDataEntry *entry, GHashTable *namespaces)
{
	/*TODO check it after writing get_xml*/
	/* Chain up to the parent class */
	GDATA_ENTRY_CLASS (gdata_documents_entry_parent_class)->get_namespaces (entry, namespaces);

	g_hash_table_insert (namespaces, (gchar*) "gd", (gchar*) "http://schemas.google.com/g/2005");
	g_hash_table_insert (namespaces, (gchar*) "docs", (gchar*) "http://schemas.google.com/docs/2007#document");

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
gdata_documents_entry_get_edited ( GDataDocumentsEntry *self, GTimeVal *edited)
{
	g_return_val_if_fail ( GDATA_IS_DOCUMENTS_ENTRY ( self ), NULL );
	g_return_if_fail (edited != NULL);
	*edited = self->priv->edited;
}

/** 
 * gdata_documents_entry_get_last_viewed:
 * @self: a #GDataDocumentsEntry
 * @last_viewed: a #GTimeVal
 *
 * Gets the #GDataDocumentsEntry:last_viewed property and puts it in @last_viewed. If the property is unset,
 * both fields in the #GTimeVal will be set to %0.
 **/
void
gdata_documents_entry_get_last_viewed ( GDataDocumentsEntry *self, GTimeVal *last_viewed)
{
	g_return_val_if_fail ( GDATA_IS_DOCUMENTS_ENTRY ( self ), NULL );
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
gdata_documents_entry_get_path (GDataDocumentsEntry *self )
{
	g_return_val_if_fail ( GDATA_IS_DOCUMENTS_ENTRY ( self ), NULL );
	return self->priv->path;
}

/**
 * gdata_documents_entry_set_path:
 * @self: a #GDataDocumentsEntry
 * @path: a new path (or NULL?)
 *
 * Sets the #GDataDocumentsEntry:path property to path.
 **/
void 
gdata_documents_entry_set_path (GDataDocumentsEntry *self, const gchar *path )
{
	g_return_if_fail ( GDATA_IS_DOCUMENTS_ENTRY ( self ) );
	self->priv->path = g_strdup ( path );
	g_object_notify (G_OBJECT (self), "path");
}

/**
 * gdata_documents_entry_get_resource_id:
 * @self: a #GDataDocumentsEntry
 *
 * Gets the #GDataDocumentsEntry:resource_id property.
 *
 * Return value: the resource_id of the entry.
 **/
gchar*
gdata_documents_entry_get_resource_id (GDataDocumentsEntry *self )
{
	g_return_val_if_fail ( GDATA_IS_DOCUMENTS_ENTRY ( self ), NULL );
	return self->priv->path;
}

/**
 * gdata_documents_entry_set_resource_id:
 * @self: a #GDataDocumentsEntry
 * @path: a new resource_id (or NULL?)
 *
 * Sets the #GDataDocumentsEntry:resource_id property to resource_id.
 **/
void 
gdata_documents_entry_set_resource_id (GDataDocumentsEntry *self, const gchar *resource_id )
{
	g_return_if_fail ( GDATA_IS_DOCUMENTS_ENTRY ( self ) );
	self->priv->resource_id = g_strdup ( resource_id );
	g_object_notify (G_OBJECT (self), "resource-id");
}

/**
 * gdata_documents_entry_get_access_rules_uri:
 * @self: a #GDataDocumentsEntry
 *
 * Gets the #GDataDocumentsEntry:access_rules_uri property.
 *
 * Return value: the access_rules_uri of this document.
 **/
GDataGDFeedLink*
gdata_documents_entry_get_access_rules_uri (GDataDocumentsEntry *self )
{
	g_return_val_if_fail ( GDATA_IS_DOCUMENTS_ENTRY ( self ), NULL );
	return self->priv->access_rules_uri;
}

/**
 * gdata_documents_entry_set_access_rules_uri:
 * @self: a #GDataDocumentsEntry
 *
 * Sets the #GDataDocumentsEntry:access_rules_uri property to _access_rules_uri.
 **/
void 
gdata_documents_entry_set_access_rules_uri (GDataDocumentsEntry *self, GDataGDFeedLink *access_rules_uri)
{
	g_return_if_fail ( GDATA_IS_DOCUMENTS_ENTRY ( self ) );
	self->priv->access_rules_uri =  access_rules_uri ;
	g_object_notify (G_OBJECT (self), "access-rules-uri");
}


/**
 * gdata_documents_entry_set_writers_can_invite:
 * @self: a #GDataDocumentsEntry
 * @writers_can_invite: %TRUE if writers can invite other personns to write on the document or %false otherwise
 *
 * Sets the #GDataDocumentsEntry:writers_can_invite to writers_can_invite.
 **/
void 
gdata_documents_entry_set_writers_can_invite(GDataDocumentsEntry *self, gboolean writers_can_invite)
{
	g_return_if_fail ( GDATA_IS_DOCUMENTS_ENTRY ( self ) );
	self->priv->writers_can_invite = writers_can_invite;
	//g_object_notify (G_OBJECT (self), "writers-can-invite");
}

/**
 * gdata_documents_entry_get_writers_can_invite:
 * @self: a #GDataDocumentsEntry
 *
 * Gets the #GDataDocumentsEntry:writers_can_invite property.
 *
 * Return value: %TRUE if writers can incite other personns to write on the document, %FALSE otherwise
 **/
gboolean
gdata_documents_entry_get_writers_can_invite ( GDataDocumentsEntry *self )
{
	g_return_val_if_fail ( GDATA_IS_DOCUMENTS_ENTRY ( self ), NULL );
	return self->priv->writers_can_invite;
}

/**
 * gdata_documents_entry_add_a_mime_type:
 * @self: a #GDataDocumentsEntry
 * @extension: The extension refering to the mime type
 * @mime_type: The mime type  string
 *
 * Adds  new mime type to the mime_types GHashtable.
 **/
void 
gdata_documents_entry_add_a_mime_type (GDataDocumentsEntry *self, gchar *extension, gchar *mime_type )
{
	g_return_if_fail ( GDATA_IS_DOCUMENTS_ENTRY ( self ));
	g_hash_table_insert (self->priv->mime_types, (gchar*)extension, (gchar*)mime_type);
}

/**
 * gdata_documents_entry_set_last_modified_by:
 * @self: a #GDataDocumentsEntry
 * @last_modified_by: the GDataAuthor refering to this document entry
 *
 * Sets the GDataDocumentsEntry:last_modified_by to last_modified_by
 **/
void 
gdata_documents_entry_set_last_modified_by (GDataDocumentsEntry *self, GDataAuthor *last_modified_by)
{
	g_return_if_fail ( GDATA_IS_DOCUMENTS_ENTRY ( self ) );
	self->priv->last_modified_by = last_modified_by;
	g_object_notify (G_OBJECT (self), "last-modified-by");
}

/** 
 * gdata_documents_entry_get_last_modified_by:
 * @self: a #GDataDocumentsEntry
 * @last_modified_by: a #GDataAuthor
 *
 * Return value: the last_modified_by of the entry.
 **/
GDataAuthor*
gdata_documents_entry_get_last_modified_by (GDataDocumentsEntry *self)
{
	g_return_val_if_fail ( GDATA_IS_DOCUMENTS_ENTRY (self), NULL );
	return self->priv->last_modified_by;
}
