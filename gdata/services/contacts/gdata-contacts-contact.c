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
 * SECTION:gdata-contacts-contact
 * @short_description: GData Contacts contact object
 * @stability: Unstable
 * @include: gdata/services/contacts/gdata-contacts-contact.h
 *
 * #GDataContactsContact is a subclass of #GDataEntry to represent a contact from a Google address book.
 *
 * For more details of Google Contacts' GData API, see the <ulink type="http" url="http://code.google.com/apis/contacts/docs/2.0/reference.html">
 * online documentation</ulink>.
 **/

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <libxml/parser.h>
#include <string.h>

#include "gdata-contacts-contact.h"
#include "gdata-parser.h"
#include "gdata-types.h"
#include "gdata-private.h"

/* The maximum number of extended properties the server allows us. See
 * http://code.google.com/apis/contacts/docs/2.0/reference.html#ProjectionsAndExtended.
 * When updating this, make sure to update the API documentation for gdata_contacts_contact_get_extended_property()
 * and gdata_contacts_contact_set_extended_property(). */
#define MAX_N_EXTENDED_PROPERTIES 10

static void gdata_contacts_contact_dispose (GObject *object);
static void gdata_contacts_contact_finalize (GObject *object);
static void gdata_contacts_contact_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void get_xml (GDataParsable *parsable, GString *xml_string);
static gboolean parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error);
static void get_namespaces (GDataParsable *parsable, GHashTable *namespaces);

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
	gchar *photo_etag;
};

enum {
	PROP_EDITED = 1,
	PROP_DELETED,
	PROP_HAS_PHOTO
};

G_DEFINE_TYPE (GDataContactsContact, gdata_contacts_contact, GDATA_TYPE_ENTRY)
#define GDATA_CONTACTS_CONTACT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDATA_TYPE_CONTACTS_CONTACT, GDataContactsContactPrivate))

static void
gdata_contacts_contact_class_init (GDataContactsContactClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GDataParsableClass *parsable_class = GDATA_PARSABLE_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataContactsContactPrivate));

	gobject_class->get_property = gdata_contacts_contact_get_property;
	gobject_class->dispose = gdata_contacts_contact_dispose;
	gobject_class->finalize = gdata_contacts_contact_finalize;

	parsable_class->parse_xml = parse_xml;
	parsable_class->get_xml = get_xml;
	parsable_class->get_namespaces = get_namespaces;

	/**
	 * GDataContactsContact:edited:
	 *
	 * The last time the contact was edited. If the contact has not been edited yet, the content indicates the time it was created.
	 *
	 * For more information, see the <ulink type="http" url="http://www.atomenabled.org/developers/protocol/#appEdited">
	 * Atom Publishing Protocol specification</ulink>.
	 *
	 * Since: 0.2.0
	 **/
	g_object_class_install_property (gobject_class, PROP_EDITED,
				g_param_spec_boxed ("edited",
					"Edited", "The last time the contact was edited.",
					GDATA_TYPE_G_TIME_VAL,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataContactsContact:deleted:
	 *
	 * Whether the entry has been deleted.
	 *
	 * Since: 0.2.0
	 **/
	g_object_class_install_property (gobject_class, PROP_DELETED,
				g_param_spec_boolean ("deleted",
					"Deleted", "Whether the entry has been deleted.",
					FALSE,
					G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataContactsContact:has-photo:
	 *
	 * Whether the contact has a photo.
	 *
	 * Since: 0.4.0
	 **/
	g_object_class_install_property (gobject_class, PROP_HAS_PHOTO,
				g_param_spec_boolean ("has-photo",
					"Has photo?", "Whether the contact has a photo.",
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
gdata_contacts_contact_dispose (GObject *object)
{
	GDataContactsContactPrivate *priv = GDATA_CONTACTS_CONTACT (object)->priv;

	if (priv->organizations != NULL) {
		g_list_foreach (priv->organizations, (GFunc) g_object_unref, NULL);
		g_list_free (priv->organizations);
	}
	priv->organizations = NULL;

	if (priv->email_addresses != NULL) {
		g_list_foreach (priv->email_addresses, (GFunc) g_object_unref, NULL);
		g_list_free (priv->email_addresses);
	}
	priv->email_addresses = NULL;

	if (priv->im_addresses != NULL) {
		g_list_foreach (priv->im_addresses, (GFunc) g_object_unref, NULL);
		g_list_free (priv->im_addresses);
	}
	priv->im_addresses = NULL;

	if (priv->postal_addresses != NULL) {
		g_list_foreach (priv->postal_addresses, (GFunc) g_object_unref, NULL);
		g_list_free (priv->postal_addresses);
	}
	priv->postal_addresses = NULL;

	if (priv->phone_numbers != NULL) {
		g_list_foreach (priv->phone_numbers, (GFunc) g_object_unref, NULL);
		g_list_free (priv->phone_numbers);
	}
	priv->phone_numbers = NULL;

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_contacts_contact_parent_class)->dispose (object);
}

static void
gdata_contacts_contact_finalize (GObject *object)
{
	GDataContactsContactPrivate *priv = GDATA_CONTACTS_CONTACT (object)->priv;

	g_hash_table_destroy (priv->extended_properties);
	g_hash_table_destroy (priv->groups);
	g_free (priv->photo_etag);

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
		case PROP_HAS_PHOTO:
			g_value_set_boolean (value, (priv->photo_etag != NULL) ? TRUE : FALSE);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

/**
 * gdata_contacts_contact_new:
 * @id: the contact's ID, or %NULL
 *
 * Creates a new #GDataContactsContact with the given ID and default properties.
 *
 * Return value: a new #GDataContactsContact; unref with g_object_unref()
 *
 * Since: 0.2.0
 **/
GDataContactsContact *
gdata_contacts_contact_new (const gchar *id)
{
	return g_object_new (GDATA_TYPE_CONTACTS_CONTACT, "id", id, NULL);
}

static gboolean
parse_xml (GDataParsable *parsable, xmlDoc *doc, xmlNode *node, gpointer user_data, GError **error)
{
	GDataContactsContact *self;

	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (parsable), FALSE);
	g_return_val_if_fail (doc != NULL, FALSE);
	g_return_val_if_fail (node != NULL, FALSE);

	self = GDATA_CONTACTS_CONTACT (parsable);

	if (xmlStrcmp (node->name, (xmlChar*) "edited") == 0) {
		/* app:edited */
		/* TODO: Should be in GDataEntry? */
		xmlChar *edited = xmlNodeListGetString (doc, node->children, TRUE);
		if (g_time_val_from_iso8601 ((gchar*) edited, &(self->priv->edited)) == FALSE) {
			/* Error */
			gdata_parser_error_not_iso8601_format (node, (gchar*) edited, error);
			xmlFree (edited);
			return FALSE;
		}
		xmlFree (edited);
	} else if (xmlStrcmp (node->name, (xmlChar*) "email") == 0) {
		/* gd:email */
		GDataGDEmailAddress *email = GDATA_GD_EMAIL_ADDRESS (_gdata_parsable_new_from_xml_node (GDATA_TYPE_GD_EMAIL_ADDRESS, doc,
													node, NULL, error));
		if (email == NULL)
			return FALSE;

		gdata_contacts_contact_add_email_address (self, email);
	} else if (xmlStrcmp (node->name, (xmlChar*) "im") == 0) {
		/* gd:im */
		GDataGDIMAddress *im = GDATA_GD_IM_ADDRESS (_gdata_parsable_new_from_xml_node (GDATA_TYPE_GD_IM_ADDRESS, doc, node, NULL, error));
		if (im == NULL)
			return FALSE;

		gdata_contacts_contact_add_im_address (self, im);
	} else if (xmlStrcmp (node->name, (xmlChar*) "phoneNumber") == 0) {
		/* gd:phoneNumber */
		GDataGDPhoneNumber *number = GDATA_GD_PHONE_NUMBER (_gdata_parsable_new_from_xml_node (GDATA_TYPE_GD_PHONE_NUMBER, doc,
												       node, NULL, error));
		if (number == NULL)
			return FALSE;

		gdata_contacts_contact_add_phone_number (self, number);
	} else if (xmlStrcmp (node->name, (xmlChar*) "postalAddress") == 0) {
		/* gd:postalAddress */
		GDataGDPostalAddress *address = GDATA_GD_POSTAL_ADDRESS (_gdata_parsable_new_from_xml_node (GDATA_TYPE_GD_POSTAL_ADDRESS,
													    doc, node, NULL, error));
		if (address == NULL)
			return FALSE;

		gdata_contacts_contact_add_postal_address (self, address);
	} else if (xmlStrcmp (node->name, (xmlChar*) "organization") == 0) {
		/* gd:organization */
		GDataGDOrganization *organization = GDATA_GD_ORGANIZATION (_gdata_parsable_new_from_xml_node (GDATA_TYPE_GD_ORGANIZATION,
													      doc, node, NULL, error));
		if (organization == NULL)
			return FALSE;

		gdata_contacts_contact_add_organization (self, organization);
	} else if (xmlStrcmp (node->name, (xmlChar*) "extendedProperty") == 0) {
		/* gd:extendedProperty */
		xmlChar *name, *value;
		xmlBuffer *buffer = NULL;

		name = xmlGetProp (node, (xmlChar*) "name");
		if (name == NULL)
			return gdata_parser_error_required_property_missing (node, "name", error);

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
			return gdata_parser_error_required_property_missing (node, "href", error);

		/* Has it been deleted? */
		deleted = xmlGetProp (node, (xmlChar*) "deleted");
		if (deleted == NULL || xmlStrcmp (deleted, (xmlChar*) "false") == 0)
			deleted_bool = FALSE;
		else if (xmlStrcmp (deleted, (xmlChar*) "true") == 0)
			deleted_bool = TRUE;
		else {
			gdata_parser_error_unknown_property_value (node, "deleted", (gchar*) deleted, error);
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
	} else {
		/* If we haven't yet found a photo, check to see if it's a photo <link> element */
		if (self->priv->photo_etag == NULL && xmlStrcmp (node->name, (xmlChar*) "link") == 0) {
			xmlChar *rel = xmlGetProp (node, (xmlChar*) "rel");
			if (xmlStrcmp (rel, (xmlChar*) "http://schemas.google.com/contacts/2008/rel#photo") == 0) {
				xmlChar *etag;

				/* It's the photo link (http://code.google.com/apis/contacts/docs/2.0/reference.html#Photos), whose ETag we should
				 * note down, then pass onto the parent class to parse properly */
				etag = xmlGetProp (node, (xmlChar*) "etag");
				self->priv->photo_etag = g_strdup ((gchar*) etag);
				xmlFree (etag);
			}
			xmlFree (rel);
		}

		if (GDATA_PARSABLE_CLASS (gdata_contacts_contact_parent_class)->parse_xml (parsable, doc, node, user_data, error) == FALSE) {
			/* Error! */
			return FALSE;
		}
	}

	return TRUE;
}

static void
get_child_xml (GList *list, GString *xml_string)
{
	GList *i;

	for (i = list; i != NULL; i = i->next) {
		gchar *xml = _gdata_parsable_get_xml (GDATA_PARSABLE (i->data), FALSE);
		g_string_append (xml_string, xml);
		g_free (xml);
	}
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
get_xml (GDataParsable *parsable, GString *xml_string)
{
	GDataContactsContactPrivate *priv = GDATA_CONTACTS_CONTACT (parsable)->priv;

	/* Chain up to the parent class */
	GDATA_PARSABLE_CLASS (gdata_contacts_contact_parent_class)->get_xml (parsable, xml_string);

	/* Lists of stuff */
	get_child_xml (priv->email_addresses, xml_string);
	get_child_xml (priv->im_addresses, xml_string);
	get_child_xml (priv->phone_numbers, xml_string);
	get_child_xml (priv->postal_addresses, xml_string);
	get_child_xml (priv->organizations, xml_string);

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
get_namespaces (GDataParsable *parsable, GHashTable *namespaces)
{
	/* Chain up to the parent class */
	GDATA_PARSABLE_CLASS (gdata_contacts_contact_parent_class)->get_namespaces (parsable, namespaces);

	g_hash_table_insert (namespaces, (gchar*) "gd", (gchar*) "http://schemas.google.com/g/2005");
	g_hash_table_insert (namespaces, (gchar*) "gContact", (gchar*) "http://schemas.google.com/contact/2008");
	g_hash_table_insert (namespaces, (gchar*) "app", (gchar*) "http://www.w3.org/2007/app");
}

/**
 * gdata_contacts_contact_get_edited:
 * @self: a #GDataContactsContact
 * @edited: a #GTimeVal
 *
 * Gets the #GDataContactsContact:edited property and puts it in @edited. If the property is unset,
 * both fields in the #GTimeVal will be set to %0.
 *
 * Since: 0.2.0
 **/
void
gdata_contacts_contact_get_edited (GDataContactsContact *self, GTimeVal *edited)
{
	g_return_if_fail (GDATA_IS_CONTACTS_CONTACT (self));
	g_return_if_fail (edited != NULL);
	*edited = self->priv->edited;
}

/**
 * gdata_contacts_contact_add_email_address:
 * @self: a #GDataContactsContact
 * @email_address: a #GDataGDEmailAddress to add
 *
 * Adds an e-mail address to the contact's list of e-mail addresses and increments its reference count.
 *
 * Note that only one e-mail address per contact may be marked as "primary". Insertion and update operations
 * (with gdata_contacts_service_insert_contact()) will return an error if more than one e-mail address
 * is marked as primary.
 *
 * Duplicate e-mail addresses will not be added to the list.
 *
 * Since: 0.2.0
 **/
void
gdata_contacts_contact_add_email_address (GDataContactsContact *self, GDataGDEmailAddress *email_address)
{
	g_return_if_fail (GDATA_IS_CONTACTS_CONTACT (self));
	g_return_if_fail (GDATA_IS_GD_EMAIL_ADDRESS (email_address));

	if (g_list_find_custom (self->priv->email_addresses, email_address, (GCompareFunc) gdata_gd_email_address_compare) == NULL)
		self->priv->email_addresses = g_list_append (self->priv->email_addresses, g_object_ref (email_address));
}

/**
 * gdata_contacts_contact_get_email_addresses:
 * @self: a #GDataContactsContact
 *
 * Gets a list of the e-mail addresses owned by the contact.
 *
 * Return value: a #GList of #GDataGDEmailAddress<!-- -->es, or %NULL
 *
 * Since: 0.2.0
 **/
GList *
gdata_contacts_contact_get_email_addresses (GDataContactsContact *self)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), NULL);
	return self->priv->email_addresses;
}

/**
 * gdata_contacts_contact_get_primary_email_address:
 * @self: a #GDataContactsContact
 *
 * Gets the contact's primary e-mail address, if one exists.
 *
 * Return value: a #GDataGDEmailAddress, or %NULL
 *
 * Since: 0.2.0
 **/
GDataGDEmailAddress *
gdata_contacts_contact_get_primary_email_address (GDataContactsContact *self)
{
	GList *i;

	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), NULL);

	for (i = self->priv->email_addresses; i != NULL; i = i->next) {
		if (gdata_gd_email_address_is_primary (GDATA_GD_EMAIL_ADDRESS (i->data)) == TRUE)
			return GDATA_GD_EMAIL_ADDRESS (i->data);
	}

	return NULL;
}

/**
 * gdata_contacts_contact_add_im_address:
 * @self: a #GDataContactsContact
 * @im_address: a #GDataGDIMAddress to add
 *
 * Adds an IM (instant messaging) address to the contact's list of IM addresses and increments its reference count.
 *
 * Note that only one IM address per contact may be marked as "primary". Insertion and update operations
 * (with gdata_contacts_service_insert_contact()) will return an error if more than one IM address
 * is marked as primary.
 *
 * Duplicate IM addresses will not be added to the list.
 *
 * Since: 0.2.0
 **/
void
gdata_contacts_contact_add_im_address (GDataContactsContact *self, GDataGDIMAddress *im_address)
{
	g_return_if_fail (GDATA_IS_CONTACTS_CONTACT (self));
	g_return_if_fail (GDATA_IS_GD_IM_ADDRESS (im_address));

	if (g_list_find_custom (self->priv->im_addresses, im_address, (GCompareFunc) gdata_gd_im_address_compare) == NULL)
		self->priv->im_addresses = g_list_append (self->priv->im_addresses, g_object_ref (im_address));
}

/**
 * gdata_contacts_contact_get_im_addresses:
 * @self: a #GDataContactsContact
 *
 * Gets a list of the IM addresses owned by the contact.
 *
 * Return value: a #GList of #GDataGDIMAddress<!-- -->es, or %NULL
 *
 * Since: 0.2.0
 **/
GList *
gdata_contacts_contact_get_im_addresses (GDataContactsContact *self)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), NULL);
	return self->priv->im_addresses;
}

/**
 * gdata_contacts_contact_get_primary_im_address:
 * @self: a #GDataContactsContact
 *
 * Gets the contact's primary IM address, if one exists.
 *
 * Return value: a #GDataGDIMAddress, or %NULL
 *
 * Since: 0.2.0
 **/
GDataGDIMAddress *
gdata_contacts_contact_get_primary_im_address (GDataContactsContact *self)
{
	GList *i;

	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), NULL);

	for (i = self->priv->im_addresses; i != NULL; i = i->next) {
		if (gdata_gd_im_address_is_primary (GDATA_GD_IM_ADDRESS (i->data)) == TRUE)
			return GDATA_GD_IM_ADDRESS (i->data);
	}

	return NULL;
}

/**
 * gdata_contacts_contact_add_phone_number:
 * @self: a #GDataContactsContact
 * @phone_number: a #GDataGDPhoneNumber to add
 *
 * Adds a phone number to the contact's list of phone numbers and increments its reference count
 *
 * Note that only one phone number per contact may be marked as "primary". Insertion and update operations
 * (with gdata_contacts_service_insert_contact()) will return an error if more than one phone number
 * is marked as primary.
 *
 * Duplicate phone numbers will not be added to the list.
 *
 * Since: 0.2.0
 **/
void
gdata_contacts_contact_add_phone_number (GDataContactsContact *self, GDataGDPhoneNumber *phone_number)
{
	g_return_if_fail (GDATA_IS_CONTACTS_CONTACT (self));
	g_return_if_fail (GDATA_IS_GD_PHONE_NUMBER (phone_number));

	if (g_list_find_custom (self->priv->phone_numbers, phone_number, (GCompareFunc) gdata_gd_phone_number_compare) == NULL)
		self->priv->phone_numbers = g_list_append (self->priv->phone_numbers, g_object_ref (phone_number));
}

/**
 * gdata_contacts_contact_get_phone_numbers:
 * @self: a #GDataContactsContact
 *
 * Gets a list of the phone numbers owned by the contact.
 *
 * Return value: a #GList of #GDataGDPhoneNumber<!-- -->s, or %NULL
 *
 * Since: 0.2.0
 **/
GList *
gdata_contacts_contact_get_phone_numbers (GDataContactsContact *self)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), NULL);
	return self->priv->phone_numbers;
}

/**
 * gdata_contacts_contact_get_primary_phone_number:
 * @self: a #GDataContactsContact
 *
 * Gets the contact's primary phone number, if one exists.
 *
 * Return value: a #GDataGDPhoneNumber, or %NULL
 *
 * Since: 0.2.0
 **/
GDataGDPhoneNumber *
gdata_contacts_contact_get_primary_phone_number (GDataContactsContact *self)
{
	GList *i;

	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), NULL);

	for (i = self->priv->phone_numbers; i != NULL; i = i->next) {
		if (gdata_gd_phone_number_is_primary (GDATA_GD_PHONE_NUMBER (i->data)) == TRUE)
			return GDATA_GD_PHONE_NUMBER (i->data);
	}

	return NULL;
}

/**
 * gdata_contacts_contact_add_postal_address:
 * @self: a #GDataContactsContact
 * @postal_address: a #GDataGDPostalAddress to add
 *
 * Adds a postal address to the contact's list of postal addresses and increments its reference count.
 *
 * Note that only one postal address per contact may be marked as "primary". Insertion and update operations
 * (with gdata_contacts_service_insert_contact()) will return an error if more than one postal address
 * is marked as primary.
 *
 * Duplicate postal addresses will not be added to the list.
 *
 * Since: 0.2.0
 **/
void
gdata_contacts_contact_add_postal_address (GDataContactsContact *self, GDataGDPostalAddress *postal_address)
{
	g_return_if_fail (GDATA_IS_CONTACTS_CONTACT (self));
	g_return_if_fail (GDATA_IS_GD_POSTAL_ADDRESS (postal_address));

	if (g_list_find_custom (self->priv->postal_addresses, postal_address, (GCompareFunc) gdata_gd_postal_address_compare) == NULL)
		self->priv->postal_addresses = g_list_append (self->priv->postal_addresses, g_object_ref (postal_address));
}

/**
 * gdata_contacts_contact_get_postal_addresses:
 * @self: a #GDataContactsContact
 *
 * Gets a list of the postal addresses owned by the contact.
 *
 * Return value: a #GList of #GDataGDPostalAddress<!-- -->es, or %NULL
 *
 * Since: 0.2.0
 **/
GList *
gdata_contacts_contact_get_postal_addresses (GDataContactsContact *self)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), NULL);
	return self->priv->postal_addresses;
}

/**
 * gdata_contacts_contact_get_primary_postal_address:
 * @self: a #GDataContactsContact
 *
 * Gets the contact's primary postal address, if one exists.
 *
 * Return value: a #GDataGDPostalAddress, or %NULL
 *
 * Since: 0.2.0
 **/
GDataGDPostalAddress *
gdata_contacts_contact_get_primary_postal_address (GDataContactsContact *self)
{
	GList *i;

	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), NULL);

	for (i = self->priv->postal_addresses; i != NULL; i = i->next) {
		if (gdata_gd_postal_address_is_primary (GDATA_GD_POSTAL_ADDRESS (i->data)) == TRUE)
			return GDATA_GD_POSTAL_ADDRESS (i->data);
	}

	return NULL;
}

/**
 * gdata_contacts_contact_add_organization:
 * @self: a #GDataContactsContact
 * @organization: a #GDataGDOrganization to add
 *
 * Adds an organization to the contact's list of organizations (e.g. employers) and increments its reference count.
 *
 * Note that only one organization per contact may be marked as "primary". Insertion and update operations
 * (with gdata_contacts_service_insert_contact()) will return an error if more than one organization
 * is marked as primary.
 *
 * Duplicate organizations will not be added to the list.
 *
 * Since: 0.2.0
 **/
void
gdata_contacts_contact_add_organization (GDataContactsContact *self, GDataGDOrganization *organization)
{
	g_return_if_fail (GDATA_IS_CONTACTS_CONTACT (self));
	g_return_if_fail (organization != NULL);

	if (g_list_find_custom (self->priv->organizations, organization, (GCompareFunc) gdata_gd_organization_compare) == NULL)
		self->priv->organizations = g_list_append (self->priv->organizations, g_object_ref (organization));
}

/**
 * gdata_contacts_contact_get_organizations:
 * @self: a #GDataContactsContact
 *
 * Gets a list of the organizations to which the contact belongs.
 *
 * Return value: a #GList of #GDataGDOrganization<!-- -->s, or %NULL
 *
 * Since: 0.2.0
 **/
GList *
gdata_contacts_contact_get_organizations (GDataContactsContact *self)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), NULL);
	return self->priv->organizations;
}

/**
 * gdata_contacts_contact_get_primary_organization:
 * @self: a #GDataContactsContact
 *
 * Gets the contact's primary organization, if one exists.
 *
 * Return value: a #GDataGDOrganization, or %NULL
 *
 * Since: 0.2.0
 **/
GDataGDOrganization *
gdata_contacts_contact_get_primary_organization (GDataContactsContact *self)
{
	GList *i;

	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), NULL);

	for (i = self->priv->organizations; i != NULL; i = i->next) {
		if (gdata_gd_organization_is_primary (GDATA_GD_ORGANIZATION (i->data)) == TRUE)
			return GDATA_GD_ORGANIZATION (i->data);
	}

	return NULL;
}

/**
 * gdata_contacts_contact_get_extended_property:
 * @self: a #GDataContactsContact
 * @name: the property name; an arbitrary, unique string
 *
 * Gets the value of an extended property of the contact. Each contact can have up to 10 client-set extended
 * properties to store data of the client's choosing.
 *
 * Return value: the property's value, or %NULL
 *
 * Since: 0.2.0
 **/
const gchar *
gdata_contacts_contact_get_extended_property (GDataContactsContact *self, const gchar *name)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), NULL);
	g_return_val_if_fail (name != NULL, NULL);
	return g_hash_table_lookup (self->priv->extended_properties, name);
}

/**
 * gdata_contacts_contact_get_extended_properties:
 * @self: a #GDataContactsContact
 *
 * Gets the full list of extended properties of the contact; a hash table mapping property name to value.
 *
 * Return value: a #GHashTable of extended properties
 *
 * Since: 0.4.0
 **/
GHashTable *
gdata_contacts_contact_get_extended_properties (GDataContactsContact *self)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), NULL);
	return self->priv->extended_properties;
}

/**
 * gdata_contacts_contact_set_extended_property:
 * @self: a #GDataContactsContact
 * @name: the property name; an arbitrary, unique string
 * @value: the property value, or %NULL
 *
 * Sets the value of a contact's extended property. Extended property names are unique (but of the client's choosing),
 * and reusing the same property name will result in the old value of that property being overwritten.
 *
 * To unset a property, set @value to %NULL.
 *
 * A contact may have up to 10 extended properties, and each should be reasonably small (i.e. not a photo or ringtone).
 * For more information, see the <ulink type="http"
 * url="http://code.google.com/apis/contacts/docs/2.0/reference.html#ProjectionsAndExtended">online documentation</ulink>.
 * %FALSE will be returned if you attempt to add more than 10 extended properties.
 *
 * Return value: %TRUE if the property was updated or deleted successfully, %FALSE otherwise
 *
 * Since: 0.2.0
 **/
gboolean
gdata_contacts_contact_set_extended_property (GDataContactsContact *self, const gchar *name, const gchar *value)
{
	GHashTable *extended_properties = self->priv->extended_properties;

	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), FALSE);
	g_return_val_if_fail (name != NULL, FALSE);

	if (value == NULL || *value == '\0') {
		/* Removing a property */
		g_hash_table_remove (extended_properties, name);
		return TRUE;
	}

	/* We can't add more than MAX_N_EXTENDED_PROPERTIES */
	if (g_hash_table_lookup (extended_properties, name) == NULL &&
	    g_hash_table_size (extended_properties) >= MAX_N_EXTENDED_PROPERTIES)
		return FALSE;

	/* Updating an existing property or adding a new one */
	g_hash_table_insert (extended_properties, g_strdup (name), g_strdup (value));

	return TRUE;
}

/**
 * gdata_contacts_contact_add_group:
 * @self: a #GDataContactsContact
 * @href: the group's ID URI
 *
 * Adds the contact to the given group. @href should be a URI.
 *
 * Since: 0.2.0
 **/
void
gdata_contacts_contact_add_group (GDataContactsContact *self, const gchar *href)
{
	g_return_if_fail (GDATA_IS_CONTACTS_CONTACT (self));
	g_return_if_fail (href != NULL);
	g_hash_table_insert (self->priv->groups, (gchar*) href, GUINT_TO_POINTER (FALSE));
}

/**
 * gdata_contacts_contact_remove_group:
 * @self: a #GDataContactsContact
 * @href: the group's ID URI
 *
 * Removes the contact from the given group. @href should be a URI.
 *
 * Since: 0.2.0
 **/
void
gdata_contacts_contact_remove_group (GDataContactsContact *self, const gchar *href)
{
	g_return_if_fail (GDATA_IS_CONTACTS_CONTACT (self));
	g_return_if_fail (href != NULL);
	g_hash_table_remove (self->priv->groups, href);
}

/**
 * gdata_contacts_contact_is_group_deleted:
 * @self: a #GDataContactsContact
 * @href: the group's ID URI
 *
 * Returns whether the contact has recently been removed from the given group. This
 * will always return %FALSE unless #GDataContactsQuery:show-deleted has been set to
 * %TRUE for the query which returned the contact.
 *
 * Return value: %TRUE if the contact has recently been removed from the group, %FALSE otherwise
 *
 * Since: 0.2.0
 **/
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

/**
 * gdata_contacts_contact_get_groups:
 * @self: a #GDataContactsContact
 *
 * Gets a list of the groups to which the contact belongs.
 *
 * Return value: a #GList of constant group ID URIs, or %NULL; free with g_list_free()
 *
 * Since: 0.2.0
 **/
GList *
gdata_contacts_contact_get_groups (GDataContactsContact *self)
{
	GList *groups;

	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), NULL);

	g_hash_table_foreach (self->priv->groups, (GHFunc) get_groups_cb, &groups);
	return g_list_reverse (groups);
}

/**
 * gdata_contacts_contact_is_deleted:
 * @self: a #GDataContactsContact
 *
 * Returns whether the contact has recently been deleted. This will always return
 * %FALSE unless #GDataContactsQuery:show-deleted has been set to
 * %TRUE for the query which returned the contact; then this function will return
 * %TRUE only if the contact has been deleted.
 *
 * If a contact has been deleted, no other information is available about it. This
 * is designed to allow contacts to be deleted from local address books using
 * incremental updates from the server (e.g. with #GDataQuery:updated-min and
 * #GDataContactsQuery:show-deleted).
 *
 * Return value: %TRUE if the contact has been deleted, %FALSE otherwise
 *
 * Since: 0.2.0
 **/
gboolean
gdata_contacts_contact_is_deleted (GDataContactsContact *self)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), FALSE);
	return self->priv->deleted;
}

/**
 * gdata_contacts_contact_has_photo:
 * @self: a #GDataContactsContact
 *
 * Returns whether the contact has a photo attached to their contact entry. If the contact
 * does have a photo, it can be returned using gdata_contacts_contact_get_photo().
 *
 * Return value: %TRUE if the contact has a photo, %FALSE otherwise
 *
 * Since: 0.4.0
 **/
gboolean
gdata_contacts_contact_has_photo (GDataContactsContact *self)
{
	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), FALSE);
	return (self->priv->photo_etag != NULL) ? TRUE : FALSE;
}

/**
 * gdata_contacts_contact_get_photo:
 * @self: a #GDataContactsContact
 * @service: a #GDataContactsService
 * @length: return location for the image length, in bytes
 * @content_type: return location for the image's content type, or %NULL; free with g_free()
 * @cancellable: optional #GCancellable object, or %NULL
 * @error: a #GError, or %NULL
 *
 * Downloads and returns the contact's photo, if they have one. If the contact doesn't
 * have a photo (i.e. gdata_contacts_contact_has_photo() returns %FALSE), %NULL is returned, but
 * no error is set in @error.
 *
 * If @cancellable is not %NULL, then the operation can be cancelled by triggering the @cancellable object from another thread.
 * If the operation was cancelled, the error %G_IO_ERROR_CANCELLED will be returned.
 *
 * If there is an error getting the photo, a %GDATA_SERVICE_ERROR_WITH_QUERY error will be returned.
 *
 * Return value: the image data, or %NULL; free with g_free()
 *
 * Since: 0.4.0
 **/
gchar *
gdata_contacts_contact_get_photo (GDataContactsContact *self, GDataContactsService *service, gsize *length, gchar **content_type,
				  GCancellable *cancellable, GError **error)
{
	GDataServiceClass *klass;
	GDataLink *link;
	SoupMessage *message;
	guint status;
	gchar *data;

	/* TODO: async version */
	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), NULL);
	g_return_val_if_fail (GDATA_IS_CONTACTS_SERVICE (service), NULL);
	g_return_val_if_fail (length != NULL, NULL);

	/* Return if there is no photo */
	if (gdata_contacts_contact_has_photo (self) == FALSE)
		return NULL;

	/* Get the photo URI */
	link = gdata_entry_look_up_link (GDATA_ENTRY (self), "http://schemas.google.com/contacts/2008/rel#photo");
	g_assert (link != NULL);
	message = soup_message_new (SOUP_METHOD_GET, gdata_link_get_uri (link));

	/* Make sure the headers are set */
	klass = GDATA_SERVICE_GET_CLASS (service);
	if (klass->append_query_headers != NULL)
		klass->append_query_headers (GDATA_SERVICE (service), message);

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

	g_assert (message->response_body->data != NULL);

	/* Sort out the return values */
	if (content_type != NULL)
		*content_type = g_strdup (soup_message_headers_get_content_type (message->response_headers, NULL));
	*length = message->response_body->length;
	data = g_memdup (message->response_body->data, message->response_body->length);

	/* Update the stored photo ETag */
	g_free (self->priv->photo_etag);
	self->priv->photo_etag = g_strdup (soup_message_headers_get_one (message->response_headers, "ETag"));
	g_object_unref (message);

	return data;
}

/**
 * gdata_contacts_contact_set_photo:
 * @self: a #GDataContactsContact
 * @service: a #GDataService
 * @data: the image data, or %NULL
 * @length: the image length, in bytes, or %0
 * @cancellable: optional #GCancellable object, or %NULL
 * @error: a #GError, or %NULL
 *
 * Sets the contact's photo to @data or, if @data is %NULL, deletes the contact's photo.
 *
 * If @cancellable is not %NULL, then the operation can be cancelled by triggering the @cancellable object from another thread.
 * If the operation was cancelled, the error %G_IO_ERROR_CANCELLED will be returned.
 *
 * If there is an error setting the photo, a %GDATA_SERVICE_ERROR_WITH_UPDATE error will be returned.
 *
 * Return value: %TRUE on success, %FALSE otherwise
 *
 * Since: 0.4.0
 **/
gboolean
gdata_contacts_contact_set_photo (GDataContactsContact *self, GDataService *service, const gchar *data, gsize length,
				  GCancellable *cancellable, GError **error)
{
	GDataServiceClass *klass;
	GDataLink *link;
	SoupMessage *message;
	guint status;
	gboolean adding_photo = FALSE, deleting_photo = FALSE;

	/* TODO: async version */
	g_return_val_if_fail (GDATA_IS_CONTACTS_CONTACT (self), FALSE);
	g_return_val_if_fail (GDATA_IS_SERVICE (service), FALSE);

	if (self->priv->photo_etag == NULL && data != NULL)
		adding_photo = TRUE;
	else if (self->priv->photo_etag != NULL && data == NULL)
		deleting_photo = TRUE;

	/* Get the photo URI */
	link = gdata_entry_look_up_link (GDATA_ENTRY (self), "http://schemas.google.com/contacts/2008/rel#photo");
	g_assert (link != NULL);
	if (deleting_photo == TRUE)
		message = soup_message_new (SOUP_METHOD_DELETE, gdata_link_get_uri (link));
	else
		message = soup_message_new (SOUP_METHOD_PUT, gdata_link_get_uri (link));

	/* Make sure the headers are set */
	klass = GDATA_SERVICE_GET_CLASS (service);
	if (klass->append_query_headers != NULL)
		klass->append_query_headers (service, message);

	/* Append the ETag header if possible */
	if (self->priv->photo_etag != NULL)
		soup_message_headers_append (message->request_headers, "If-Match", self->priv->photo_etag);

	if (deleting_photo == FALSE) {
		/* Append the data */
		soup_message_set_request (message, "image/*", SOUP_MEMORY_STATIC, (gchar*) data, length);
	}

	/* Send the message */
	status = _gdata_service_send_message (service, message, error);
	if (status == SOUP_STATUS_NONE) {
		g_object_unref (message);
		return FALSE;
	}

	/* Check for cancellation */
	if (g_cancellable_set_error_if_cancelled (cancellable, error) == TRUE) {
		g_object_unref (message);
		return FALSE;
	}

	if (status != 200) {
		/* Error */
		g_assert (klass->parse_error_response != NULL);
		klass->parse_error_response (service, GDATA_SERVICE_ERROR_WITH_UPDATE, status, message->reason_phrase, message->response_body->data,
					     message->response_body->length, error);
		g_object_unref (message);
		return FALSE;
	}

	/* Update the stored photo ETag */
	g_free (self->priv->photo_etag);
	self->priv->photo_etag = g_strdup (soup_message_headers_get_one (message->response_headers, "ETag"));
	g_object_unref (message);

	if (adding_photo == TRUE || deleting_photo == TRUE)
		g_object_notify (G_OBJECT (self), "has-photo");

	return TRUE;
}
