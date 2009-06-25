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

#ifndef GDATA_CONTACTS_CONTACT_H
#define GDATA_CONTACTS_CONTACT_H

#include <glib.h>
#include <glib-object.h>

#include <gdata/gdata-entry.h>
#include <gdata/gd/gdata-gd-email-address.h>
#include <gdata/gd/gdata-gd-im-address.h>
#include <gdata/gd/gdata-gd-organization.h>
#include <gdata/gd/gdata-gd-phone-number.h>
#include <gdata/gd/gdata-gd-postal-address.h>

G_BEGIN_DECLS

#define GDATA_TYPE_CONTACTS_CONTACT		(gdata_contacts_contact_get_type ())
#define GDATA_CONTACTS_CONTACT(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GDATA_TYPE_CONTACTS_CONTACT, GDataContactsContact))
#define GDATA_CONTACTS_CONTACT_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GDATA_TYPE_CONTACTS_CONTACT, GDataContactsContactClass))
#define GDATA_IS_CONTACTS_CONTACT(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GDATA_TYPE_CONTACTS_CONTACT))
#define GDATA_IS_CONTACTS_CONTACT_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GDATA_TYPE_CONTACTS_CONTACT))
#define GDATA_CONTACTS_CONTACT_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GDATA_TYPE_CONTACTS_CONTACT, GDataContactsContactClass))

typedef struct _GDataContactsContactPrivate	GDataContactsContactPrivate;

/**
 * GDataContactsContact:
 *
 * All the fields in the #GDataContactsContact structure are private and should never be accessed directly.
 **/
typedef struct {
	GDataEntry parent;
	GDataContactsContactPrivate *priv;
} GDataContactsContact;

/**
 * GDataContactsContactClass:
 *
 * All the fields in the #GDataContactsContactClass structure are private and should never be accessed directly.
 **/
typedef struct {
	/*< private >*/
	GDataEntryClass parent;
} GDataContactsContactClass;

GType gdata_contacts_contact_get_type (void) G_GNUC_CONST;

GDataContactsContact *gdata_contacts_contact_new (const gchar *id) G_GNUC_WARN_UNUSED_RESULT;
GDataContactsContact *gdata_contacts_contact_new_from_xml (const gchar *xml, gint length, GError **error) G_GNUC_WARN_UNUSED_RESULT;

void gdata_contacts_contact_get_edited (GDataContactsContact *self, GTimeVal *edited);
gboolean gdata_contacts_contact_is_deleted (GDataContactsContact *self);

void gdata_contacts_contact_add_email_address (GDataContactsContact *self, GDataGDEmailAddress *email_address);
GList *gdata_contacts_contact_get_email_addresses (GDataContactsContact *self);
GDataGDEmailAddress *gdata_contacts_contact_get_primary_email_address (GDataContactsContact *self);

void gdata_contacts_contact_add_im_address (GDataContactsContact *self, GDataGDIMAddress *im_address);
GList *gdata_contacts_contact_get_im_addresses (GDataContactsContact *self);
GDataGDIMAddress *gdata_contacts_contact_get_primary_im_address (GDataContactsContact *self);

void gdata_contacts_contact_add_phone_number (GDataContactsContact *self, GDataGDPhoneNumber *phone_number);
GList *gdata_contacts_contact_get_phone_numbers (GDataContactsContact *self);
GDataGDPhoneNumber *gdata_contacts_contact_get_primary_phone_number (GDataContactsContact *self);

void gdata_contacts_contact_add_postal_address (GDataContactsContact *self, GDataGDPostalAddress *postal_address);
GList *gdata_contacts_contact_get_postal_addresses (GDataContactsContact *self);
GDataGDPostalAddress *gdata_contacts_contact_get_primary_postal_address (GDataContactsContact *self);

void gdata_contacts_contact_add_organization (GDataContactsContact *self, GDataGDOrganization *organization);
GList *gdata_contacts_contact_get_organizations (GDataContactsContact *self);
GDataGDOrganization *gdata_contacts_contact_get_primary_organization (GDataContactsContact *self);

const gchar *gdata_contacts_contact_get_extended_property (GDataContactsContact *self, const gchar *name);
GHashTable *gdata_contacts_contact_get_extended_properties (GDataContactsContact *self);
gboolean gdata_contacts_contact_set_extended_property (GDataContactsContact *self, const gchar *name, const gchar *value);

void gdata_contacts_contact_add_group (GDataContactsContact *self, const gchar *href);
void gdata_contacts_contact_remove_group (GDataContactsContact *self, const gchar *href);
gboolean gdata_contacts_contact_is_group_deleted (GDataContactsContact *self, const gchar *href);
GList *gdata_contacts_contact_get_groups (GDataContactsContact *self) G_GNUC_WARN_UNUSED_RESULT;

#include <gdata/services/contacts/gdata-contacts-service.h>

gboolean gdata_contacts_contact_has_photo (GDataContactsContact *self);
gchar *gdata_contacts_contact_get_photo (GDataContactsContact *self, GDataContactsService *service, gsize *length, gchar **content_type,
					  GCancellable *cancellable, GError **error) G_GNUC_WARN_UNUSED_RESULT;
gboolean gdata_contacts_contact_set_photo (GDataContactsContact *self, GDataService *service, const gchar *data, gsize length,
					   GCancellable *cancellable, GError **error);

G_END_DECLS

#endif /* !GDATA_CONTACTS_CONTACT_H */
