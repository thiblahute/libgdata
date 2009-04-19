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

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <libxml/parser.h>
#include <string.h>

#include "gdata-contacts-contact.h"
#include "gdata-gdata.h"
#include "gdata-parser.h"
#include "gdata-types.h"
#include "gdata-private.h"

static void gdata_contacts_contact_finalize (GObject *object);
static void gdata_contacts_contact_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void get_xml (GDataEntry *entry, GString *xml_string);
static gboolean parse_xml (GDataEntry *entry, xmlDoc *doc, xmlNode *node, GError **error);
static void get_namespaces (GDataEntry *entry, GHashTable *namespaces);

struct _GDataContactsContactPrivate {
	GTimeVal edited;
	GList *email_addresses; /* GDataGDEmailAddress */
	GList *im_addresses; /* GDataGDIMAddress */
	GList *phone_numbers; /* GDataGDPhoneNumber */
	GList *postal_addresses; /* GDataGDPostalAddress */
	GList *organizations; /* GDataGDOrganization */
	GHashTable *extended_properties;
	GHashTable *groups;
	gboolean deleted;
};

enum {
	PROP_EDITED = 1,
	PROP_DELETED
};

G_DEFINE_TYPE (GDataContactsContact, gdata_contacts_contact, GDATA_TYPE_ENTRY)
#define GDATA_CONTACTS_CONTACT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_CONTACTS_CONTACT, GDataContactsContactPrivate))

static void
gdata_contacts_contact_class_init (GDataContactsContactClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataEntryClass *entry_class = GDATA_ENTRY_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataContactsContactPrivate));

	gobject_class->get_property = gdata_contacts_contact_get_property;
	gobject_class->finalize = gdata_contacts_contact_finalize;

	entry_class->get_xml = get_xml;
	entry_class->parse_xml = parse_xml;
	entry_class->get_namespaces = get_namespaces;

	g_object_class_install_property (gobject_class, PROP_EDITED,
				g_param_spec_boxed ("edited",
					"Edited", "TODO",
					G_TYPE_TIME_VAL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_DELETED,
				g_param_spec_boolean ("deleted",
					"Deleted", "TODO",
					FALSE,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
}

static void
gdata_contacts_contact_init (GDataContactsContact *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_CONTACTS_CONTACT, GDataContactsContactPrivate);
	self->priv->extended_properties = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
	self->priv->groups = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
}

static void
gdata_contacts_contact_finalize (GObject *object)
{
	GDataContactsContactPrivate *priv = GDATA_CONTACTS_CONTACT_GET_PRIVATE (object);

	g_list_foreach (priv->email_addresses, (GFunc) gdata_gd_email_address_free, NULL);
	g_list_free (priv->email_addresses);
	g_list_foreach (priv->im_addresses, (GFunc) gdata_gd_im_address_free, NULL);
	g_list_free (priv->im_addresses);
	g_list_foreach (priv->phone_numbers, (GFunc) gdata_gd_phone_number_free, NULL);
	g_list_free (priv->phone_numbers);
	g_list_foreach (priv->postal_addresses, (GFunc) gdata_gd_postal_address_free, NULL);
	g_list_free (priv->postal_addresses);
	g_list_foreach (priv->organizations, (GFunc) gdata_gd_organization_free, NULL);
	g_list_free (priv->organizations);
	g_hash_table_destroy (priv->extended_properties);
	g_hash_table_destroy (priv->groups);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_contacts_contact_parent_class)->finalize (object);
}

static void
gdata_contacts_contact_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataContactsContactPrivate *priv = GDATA_CONTACTS_CONTACT_GET_PRIVATE (object);

	switch (property_id) {
		case PROP_EDITED:
			g_value_set_boxed (value, &(priv->edited));
			break;
		case PROP_DELETED:
			g_value_set_boolean (value, priv->deleted);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

GDataContactsContact *
gdata_contacts_contact_new (const gchar *id)
{
	return g_object_new (GDATA_TYPE_CONTACTS_CONTACT, "id", id, NULL);
}

GDataContactsContact *
gdata_contacts_contact_new_from_xml (const gchar *xml, gint length, GError **error)
{
	return GDATA_CONTACTS_CONTACT (_gdata_entry_new_from_xml (GDATA_TYPE_CONTACTS_CONTACT, xml, length, error));
}

static gboolean
parse_xml (GDataEntry *entry, xmlDoc *doc, xmlNode *node, GError **error)
{
	GDataContactsContact *self = GDATA_CONTACTS_CONTACT (entry);

	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), FALSE);
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);

	if (xmlStrcmp (node->name, (xmlChar*) "edited") == 0) {
		/* app:edited */
		/* TODO: Should be in GDataEntry? */
		xmlChar *edited;
		GTimeVal edited_timeval;

		edited = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		if (g_time_val_from_iso8601 ((gchar*) edited, &edited_timeval) == FALSE) {
			/* Error */
			gdata_parser_error_not_iso8601_format ("app:edited", "entry", (gchar*) edited, error);
			xmlFree (edited);
			return FALSE;
		}

		gdata_contacts_contact_set_edited (self, &edited_timeval);
		xmlFree (edited);
	} else if (xmlStrcmp (node->name, (xmlChar*) "email") == 0) {
		/* gd:email */
		xmlChar *address, *rel, *label, *primary;
		gboolean primary_bool;
		GDataGDEmailAddress *email_address;

		address = xmlGetProp (node, (xmlChar*) "address");
		if (address == NULL)
			return gdata_parser_error_required_property_missing ("gd:email", "address", error);

		rel = xmlGetProp (node, (xmlChar*) "rel");
		label = xmlGetProp (node, (xmlChar*) "label");
		if (rel != NULL && label != NULL) {
			/* TODO: error (can't have both) */
		}

		/* Is it the primary e-mail address? */
		primary = xmlGetProp (node, (xmlChar*) "primary");
		if (xmlStrcmp (primary, (xmlChar*) "true") == 0)
			primary_bool = TRUE;
		else if (xmlStrcmp (primary, (xmlChar*) "false") == 0)
			primary_bool = FALSE;
		else {
			gdata_parser_error_unknown_property_value ("gd:email", "primary", (gchar*) primary, error);
			xmlFree (primary);
			return FALSE;
		}
		xmlFree (primary);

		/* Build the e-mail address */
		email_address = gdata_gd_email_address_new ((gchar*) address, (gchar*) rel, (gchar*) label, primary_bool);
		xmlFree (address);
		xmlFree (rel);
		xmlFree (label);

		gdata_contacts_contact_add_email_address (self, email_address);
	} else if (xmlStrcmp (node->name, (xmlChar*) "im") == 0) {
		/* gd:im */
		xmlChar *address, *rel, *label, *protocol, *primary;
		gboolean primary_bool;
		GDataGDIMAddress *im_address;

		address = xmlGetProp (node, (xmlChar*) "address");
		if (address == NULL)
			return gdata_parser_error_required_property_missing ("gd:im", "address", error);

		rel = xmlGetProp (node, (xmlChar*) "rel");
		label = xmlGetProp (node, (xmlChar*) "label");
		if (rel != NULL && label != NULL) {
			/* TODO: error (can't have both) */
		}
		protocol = xmlGetProp (node, (xmlChar*) "protocol");

		/* Is it the primary IM address? */
		primary = xmlGetProp (node, (xmlChar*) "primary");
		if (xmlStrcmp (primary, (xmlChar*) "true") == 0)
			primary_bool = TRUE;
		else if (xmlStrcmp (primary, (xmlChar*) "false") == 0)
			primary_bool = FALSE;
		else {
			gdata_parser_error_unknown_property_value ("gd:im", "primary", (gchar*) primary, error);
			xmlFree (primary);
			return FALSE;
		}
		xmlFree (primary);

		/* Build the IM address */
		im_address = gdata_gd_im_address_new ((gchar*) address, (gchar*) protocol, (gchar*) rel, (gchar*) label, primary_bool);
		xmlFree (address);
		xmlFree (rel);
		xmlFree (label);
		xmlFree (protocol);

		gdata_contacts_contact_add_im_address (self, im_address);
	} else if (xmlStrcmp (node->name, (xmlChar*) "phoneNumber") == 0) {
		/* gd:phoneNumber */
		xmlChar *number, *rel, *label, *uri, *primary;
		gboolean primary_bool;
		GDataGDPhoneNumber *phone_number;

		number = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		if (number == NULL)
			return gdata_parser_error_required_content_missing ("gd:phoneNumber", error);

		rel = xmlGetProp (node, (xmlChar*) "rel");
		label = xmlGetProp (node, (xmlChar*) "label");
		if (rel != NULL && label != NULL) {
			/* TODO: error (can't have both) */
		}
		uri = xmlGetProp (node, (xmlChar*) "uri");

		/* Is it the primary phone number? */
		primary = xmlGetProp (node, (xmlChar*) "primary");
		if (xmlStrcmp (primary, (xmlChar*) "true") == 0)
			primary_bool = TRUE;
		else if (xmlStrcmp (primary, (xmlChar*) "false") == 0)
			primary_bool = FALSE;
		else {
			gdata_parser_error_unknown_property_value ("gd:phoneNumber", "primary", (gchar*) primary, error);
			xmlFree (primary);
			return FALSE;
		}
		xmlFree (primary);

		/* Build the phone number */
		phone_number = gdata_gd_phone_number_new ((gchar*) number, (gchar*) rel, (gchar*) label, (gchar*) uri, primary_bool);
		xmlFree (number);
		xmlFree (rel);
		xmlFree (label);
		xmlFree (uri);

		gdata_contacts_contact_add_phone_number (self, phone_number);
	} else if (xmlStrcmp (node->name, (xmlChar*) "postalAddress") == 0) {
		/* gd:postalAddress */
		xmlChar *address, *rel, *label, *primary;
		gboolean primary_bool;
		GDataGDPostalAddress *postal_address;

		address = xmlNodeListGetString (doc, node->xmlChildrenNode, TRUE);
		if (address == NULL)
			return gdata_parser_error_required_content_missing ("gd:postalAddress", error);

		rel = xmlGetProp (node, (xmlChar*) "rel");
		label = xmlGetProp (node, (xmlChar*) "label");
		if (rel != NULL && label != NULL) {
			/* TODO: error (can't have both) */
		}

		/* Is it the primary postal address? */
		primary = xmlGetProp (node, (xmlChar*) "primary");
		if (xmlStrcmp (primary, (xmlChar*) "true") == 0)
			primary_bool = TRUE;
		else if (xmlStrcmp (primary, (xmlChar*) "false") == 0)
			primary_bool = FALSE;
		else {
			gdata_parser_error_unknown_property_value ("gd:postalAddress", "primary", (gchar*) primary, error);
			xmlFree (primary);
			return FALSE;
		}
		xmlFree (primary);

		/* Build the postal address */
		postal_address = gdata_gd_postal_address_new ((gchar*) address, (gchar*) rel, (gchar*) label, primary_bool);
		xmlFree (address);
		xmlFree (rel);
		xmlFree (label);

		gdata_contacts_contact_add_postal_address (self, postal_address);
	} else if (xmlStrcmp (node->name, (xmlChar*) "organization") == 0) {
		/* gd:organization */
		xmlChar *name = NULL, *title = NULL, *rel, *label, *primary;
		gboolean primary_bool;
		GDataGDOrganization *organization;
		xmlNode *child_node;

		for (child_node = node->xmlChildrenNode; child_node != NULL; child_node = child_node->next) {
			if (xmlStrcmp (child_node->name, (xmlChar*) "orgName") == 0) {
				/* gd:orgName */
				if (name != NULL) {
					xmlFree (name);
					xmlFree (title);
					return gdata_parser_error_duplicate_element ("gd:orgName", "gd:organization", error);
				}
				name = xmlNodeListGetString (doc, child_node->xmlChildrenNode, TRUE);
			} else if (xmlStrcmp (child_node->name, (xmlChar*) "orgTitle") == 0) {
				/* gd:orgTitle */
				if (title != NULL) {
					xmlFree (name);
					xmlFree (title);
					return gdata_parser_error_duplicate_element ("gd:orgTitle", "gd:organization", error);
				}
				title = xmlNodeListGetString (doc, child_node->xmlChildrenNode, TRUE);
			} else {
				/* Error */
				gdata_parser_error_unhandled_element ((gchar*) child_node->ns->prefix, (gchar*) child_node->name,
								      "gd:organization", error);
				xmlFree (name);
				xmlFree (title);
				return FALSE;
			}
		}

		rel = xmlGetProp (node, (xmlChar*) "rel");
		label = xmlGetProp (node, (xmlChar*) "label");
		if (rel != NULL && label != NULL) {
			/* TODO: error (can't have both) */
		}

		/* Is it the primary organisation? */
		primary = xmlGetProp (node, (xmlChar*) "primary");
		if (xmlStrcmp (primary, (xmlChar*) "true") == 0)
			primary_bool = TRUE;
		else if (xmlStrcmp (primary, (xmlChar*) "false") == 0)
			primary_bool = FALSE;
		else {
			gdata_parser_error_unknown_property_value ("gd:organization", "primary", (gchar*) primary, error);
			xmlFree (primary);
			return FALSE;
		}
		xmlFree (primary);

		/* Build the organisation */
		organization = gdata_gd_organization_new ((gchar*) name, (gchar*) title, (gchar*) rel, (gchar*) label, primary_bool);
		xmlFree (name);
		xmlFree (title);
		xmlFree (rel);
		xmlFree (label);

		gdata_contacts_contact_add_organization (self, organization);
	} else if (xmlStrcmp (node->name, (xmlChar*) "extendedProperty") == 0) {
		/* gd:extendedProperty */
		xmlChar *name, *value;
		xmlBuffer *buffer = NULL;

		name = xmlGetProp (node, (xmlChar*) "name");
		if (name == NULL)
			return gdata_parser_error_required_property_missing ("gd:extendedProperty", "name", error);

		/* Get either the value property, or the element's content */
		value = xmlGetProp (node, (xmlChar*) "value");
		if (value == NULL) {
			/* Use the element's content instead (arbitrary XML) */
			buffer = xmlBufferCreate ();
			xmlNodeDump (buffer, doc, node, 0, 0);
			value = (xmlChar*) xmlBufferContent (buffer);
			xmlBufferFree (buffer);
		}

		gdata_contacts_contact_set_extended_property (self, (gchar*) name, (gchar*) value);

		if (buffer == NULL)
			xmlFree (value);
	} else if (xmlStrcmp (node->name, (xmlChar*) "groupMembershipInfo") == 0) {
		/* gContact:groupMembershipInfo */
		xmlChar *href, *deleted;
		gboolean deleted_bool;

		href = xmlGetProp (node, (xmlChar*) "href");
		if (href == NULL)
			return gdata_parser_error_required_property_missing ("gContact:groupMembershipInfo", "href", error);

		/* Has it been deleted? */
		deleted = xmlGetProp (node, (xmlChar*) "deleted");
		if (xmlStrcmp (deleted, (xmlChar*) "true") == 0)
			deleted_bool = TRUE;
		else if (xmlStrcmp (deleted, (xmlChar*) "false") == 0)
			deleted_bool = FALSE;
		else {
			gdata_parser_error_unknown_property_value ("gContact:groupMembershipInfo", "deleted", (gchar*) deleted, error);
			xmlFree (deleted);
			return FALSE;
		}
		xmlFree (deleted);

		/* Insert it into the hash table */
		g_hash_table_insert (self->priv->groups, g_strdup ((gchar*) href), GUINT_TO_POINTER (deleted_bool));
		xmlFree (href);
	} else if (xmlStrcmp (node->name, (xmlChar*) "deleted") == 0) {
		/* gd:deleted */
		self->priv->deleted = TRUE;
	} else if (GDATA_ENTRY_CLASS (gdata_contacts_contact_parent_class)->parse_xml (entry, doc, node, error) == FALSE) {
		/* Error! */
		return FALSE;
	}

	return TRUE;
}

static void
get_extended_property_xml_cb (const gchar *name, const gchar *value, GString *xml_string)
{
	g_string_append_printf (xml_string, "<gd:extendedProperty name='%s'>%s</gd:extendedProperty>", name, value);
}

static void
get_group_xml_cb (const gchar *href, gpointer deleted, GString *xml_string)
{
	g_string_append_printf (xml_string, "<gContact:groupMembershipInfo href='%s'/>", href);
}

static void
get_xml (GDataEntry *entry, GString *xml_string)
{
	GDataContactsContactPrivate *priv = GDATA_CONTACTS_CONTACT (entry)->priv;
	GList *i;

	/* Chain up to the parent class */
	GDATA_ENTRY_CLASS (gdata_contacts_contact_parent_class)->get_xml (entry, xml_string);

	/* E-mail addresses */
	for (i = priv->email_addresses; i != NULL; i = i->next) {
		GDataGDEmailAddress *email_address = (GDataGDEmailAddress*) i->data;

		/* rel and label are mutually exclusive */
		if (email_address->rel != NULL)
			g_string_append_printf (xml_string, "<gd:email address='%s' rel='%s'", email_address->address, email_address->rel);
		else if (email_address->label != NULL)
			g_string_append_printf (xml_string, "<gd:email address='%s' label='%s'", email_address->address, email_address->label);

		if (email_address->primary == TRUE)
			g_string_append (xml_string, " primary='true'/>");
		else
			g_string_append (xml_string, " primary='false'/>");
	}

	/* IM addresses */
	for (i = priv->im_addresses; i != NULL; i = i->next) {
		GDataGDIMAddress *im_address = (GDataGDIMAddress*) i->data;

		if (im_address->protocol != NULL)
			g_string_append_printf (xml_string, "<gd:im address='%s' protocol='%s'", im_address->address, im_address->protocol);
		else
			g_string_append_printf (xml_string, "<gd:im address='%s'", im_address->address);

		/* rel and label are mutually exclusive */
		if (im_address->rel != NULL)
			g_string_append_printf (xml_string, " rel='%s'", im_address->rel);
		else if (im_address->label != NULL)
			g_string_append_printf (xml_string, " label='%s'", im_address->label);

		if (im_address->primary == TRUE)
			g_string_append (xml_string, " primary='true'/>");
		else
			g_string_append (xml_string, " primary='false'/>");
	}

	/* Phone numbers */
	for (i = priv->phone_numbers; i != NULL; i = i->next) {
		GDataGDPhoneNumber *phone_number = (GDataGDPhoneNumber*) i->data;

		if (phone_number->uri != NULL)
			g_string_append_printf (xml_string, "<gd:phoneNumber uri='%s'", phone_number->uri);
		else
			g_string_append (xml_string, "<gd:phoneNumber");

		/* rel and label are mutually exclusive */
		if (phone_number->rel != NULL)
			g_string_append_printf (xml_string, " rel='%s'", phone_number->rel);
		else if (phone_number->label != NULL)
			g_string_append_printf (xml_string, " label='%s'", phone_number->label);

		if (phone_number->primary == TRUE)
			g_string_append (xml_string, " primary='true'>");
		else
			g_string_append (xml_string, " primary='false'>");

		/* Append the phone number itself */
		g_string_append_printf (xml_string, "%s</gd:phoneNumber>", phone_number->phone_number);
	}

	/* Postal addresses */
	for (i = priv->postal_addresses; i != NULL; i = i->next) {
		GDataGDPostalAddress *postal_address = (GDataGDPostalAddress*) i->data;

		g_string_append (xml_string, "<gd:postalAddress");

		/* rel and label are mutually exclusive */
		if (postal_address->rel != NULL)
			g_string_append_printf (xml_string, " rel='%s'", postal_address->rel);
		else if (postal_address->label != NULL)
			g_string_append_printf (xml_string, " label='%s'", postal_address->label);

		if (postal_address->primary == TRUE)
			g_string_append (xml_string, " primary='true'>");
		else
			g_string_append (xml_string, " primary='false'>");

		/* Append the address itself */
		g_string_append_printf (xml_string, "%s</gd:postalAddress>", postal_address->address);
	}

	/* Organisations */
	for (i = priv->organizations; i != NULL; i = i->next) {
		GDataGDOrganization *organisation = (GDataGDOrganization*) i->data;

		g_string_append (xml_string, "<gd:organization");

		/* rel and label are mutually exclusive */
		if (organisation->rel != NULL)
			g_string_append_printf (xml_string, " rel='%s'", organisation->rel);
		else if (organisation->label != NULL)
			g_string_append_printf (xml_string, " label='%s'", organisation->label);

		if (organisation->primary == TRUE)
			g_string_append (xml_string, " primary='true'");
		else
			g_string_append (xml_string, " primary='false'");

		/* Organisation details */
		if (organisation->name == NULL && organisation->title == NULL) {
			g_string_append (xml_string, "/>");
		} else {
			g_string_append_c (xml_string, '>');
			if (organisation->name != NULL)
				g_string_append_printf (xml_string, "<gd:orgName>%s</gd:orgName>", organisation->name);
			if (organisation->title != NULL)
				g_string_append_printf (xml_string, "<gd:orgTitle>%s</gd:orgTitle>", organisation->title);
		}
	}

	/* Extended properties */
	g_hash_table_foreach (priv->extended_properties, (GHFunc) get_extended_property_xml_cb, xml_string);

	/* Group membership info */
	g_hash_table_foreach (priv->groups, (GHFunc) get_group_xml_cb, xml_string);

	/* TODO:
	 * - Finish supporting all tags
	 * - Check things are escaped (or not) as appropriate
	 * - Write a function to encapsulate g_markup_escape_text and
	 *   g_string_append_printf to reduce the number of allocations
	 */
}

static void
get_namespaces (GDataEntry *entry, GHashTable *namespaces)
{
	/* Chain up to the parent class */
	GDATA_ENTRY_CLASS (gdata_contacts_contact_parent_class)->get_namespaces (entry, namespaces);

	g_hash_table_insert (namespaces, (gchar*) "gd", (gchar*) "http://schemas.google.com/g/2005");
	g_hash_table_insert (namespaces, (gchar*) "gContact", (gchar*) "http://schemas.google.com/contact/2008");
	g_hash_table_insert (namespaces, (gchar*) "app", (gchar*) "http://www.w3.org/2007/app");
}

void
gdata_contacts_contact_get_edited (GDataContactsContact *self, GTimeVal *edited)
{
	g_return_if_fail (GDATA_IS_CONTACTS_CONTACT (self));
	g_return_if_fail (edited != NULL);
	*edited = self->priv->edited;
}

void
gdata_contacts_contact_set_edited (GDataContactsContact *self, GTimeVal *edited)
{
	g_return_if_fail (GDATA_IS_CONTACTS_CONTACT (self));
	g_return_if_fail (edited != NULL);
	self->priv->edited = *edited;
	g_object_notify (G_OBJECT (self), "edited");
}

void
gdata_contacts_contact_add_email_address (GDataContactsContact *self, GDataGDEmailAddress *email_address)
{
	g_return_if_fail (GDATA_IS_CONTACTS_CONTACT (self));
	g_return_if_fail (email_address != NULL);

	self->priv->email_addresses = g_list_append (self->priv->email_addresses, email_address);
}

GList *
gdata_contacts_contact_get_email_addresses (GDataContactsContact *self)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), NULL);
	return self->priv->email_addresses;
}

void
gdata_contacts_contact_add_im_address (GDataContactsContact *self, GDataGDIMAddress *im_address)
{
	g_return_if_fail (GDATA_IS_CONTACTS_CONTACT (self));
	g_return_if_fail (im_address != NULL);

	self->priv->im_addresses = g_list_append (self->priv->im_addresses, im_address);
}

GList *
gdata_contacts_contact_get_im_addresses (GDataContactsContact *self)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), NULL);
	return self->priv->im_addresses;
}

void
gdata_contacts_contact_add_phone_number (GDataContactsContact *self, GDataGDPhoneNumber *phone_number)
{
	g_return_if_fail (GDATA_IS_CONTACTS_CONTACT (self));
	g_return_if_fail (phone_number != NULL);

	self->priv->phone_numbers = g_list_append (self->priv->phone_numbers, phone_number);
}

GList *
gdata_contacts_contact_get_phone_numbers (GDataContactsContact *self)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), NULL);
	return self->priv->phone_numbers;
}

void
gdata_contacts_contact_add_postal_address (GDataContactsContact *self, GDataGDPostalAddress *postal_address)
{
	g_return_if_fail (GDATA_IS_CONTACTS_CONTACT (self));
	g_return_if_fail (postal_address != NULL);

	self->priv->postal_addresses = g_list_append (self->priv->postal_addresses, postal_address);
}

GList *
gdata_contacts_contact_get_postal_addresses (GDataContactsContact *self)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), NULL);
	return self->priv->postal_addresses;
}

void
gdata_contacts_contact_add_organization (GDataContactsContact *self, GDataGDOrganization *organization)
{
	g_return_if_fail (GDATA_IS_CONTACTS_CONTACT (self));
	g_return_if_fail (organization != NULL);

	self->priv->organizations = g_list_append (self->priv->organizations, organization);
}

GList *
gdata_contacts_contact_get_organizations (GDataContactsContact *self)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), NULL);
	return self->priv->organizations;
}

const gchar *
gdata_contacts_contact_get_extended_property (GDataContactsContact *self, const gchar *name)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), NULL);
	g_return_val_if_fail (name != NULL, NULL);
	return g_hash_table_lookup (self->priv->extended_properties, name);
}

void
gdata_contacts_contact_set_extended_property (GDataContactsContact *self, const gchar *name, const gchar *value)
{
	g_return_if_fail (GDATA_IS_CONTACTS_CONTACT (self));
	g_return_if_fail (name != NULL);

	if (value == NULL)
		g_hash_table_remove (self->priv->extended_properties, name);
	else
		g_hash_table_insert (self->priv->extended_properties, g_strdup (name), g_strdup (value));
}

void
gdata_contacts_contact_add_group (GDataContactsContact *self, const gchar *href)
{
	g_return_if_fail (GDATA_IS_CONTACTS_CONTACT (self));
	g_return_if_fail (href != NULL);
	g_hash_table_insert (self->priv->groups, (gchar*) href, GUINT_TO_POINTER (FALSE));
}

void
gdata_contacts_contact_remove_group (GDataContactsContact *self, const gchar *href)
{
	g_return_if_fail (GDATA_IS_CONTACTS_CONTACT (self));
	g_return_if_fail (href != NULL);
	g_hash_table_remove (self->priv->groups, href);
}

gboolean
gdata_contacts_contact_is_group_deleted (GDataContactsContact *self, const gchar *href)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), FALSE);
	g_return_val_if_fail (href != NULL, FALSE);
	return GPOINTER_TO_UINT (g_hash_table_lookup (self->priv->groups, href));
}

static void
get_groups_cb (const gchar *href, gpointer deleted, GList **groups)
{
	*groups = g_list_prepend (*groups, (gchar*) href);
}

GList *
gdata_contacts_contact_get_groups (GDataContactsContact *self)
{
	GList *groups;

	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), NULL);

	g_hash_table_foreach (self->priv->groups, (GHFunc) get_groups_cb, &groups);
	return g_list_reverse (groups);
}

gboolean
gdata_contacts_contact_is_deleted (GDataContactsContact *self)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), FALSE);
	return self->priv->deleted;
}
