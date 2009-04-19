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

#include <glib.h>

#ifndef GDATA_GDATA_H
#define GDATA_GDATA_H

G_BEGIN_DECLS

/**
 * GDataGDRating:
 * @min: the minimum rating which can be chosen (typically %1)
 * @max: the maximum rating which can be chosen (typically %5)
 * @num_raters: the number of people who have rated the item
 * @average: the average rating for the item
 *
 * A structure fully representing a GData "rating" element. All fields are required.
 *
 * See the <ulink type="http" url="http://code.google.com/apis/gdata/elements.html#gdRating">GData specification</ulink>
 * for more information.
 *
 * Currently, rel and value functionality is not implemented in #GDataGDRating.
 **/
typedef struct {
	guint min;
	guint max;
	guint num_raters;
	gdouble average;
} GDataGDRating;

GDataGDRating *gdata_gd_rating_new (guint min, guint max, guint num_raters, gdouble average);
void gdata_gd_rating_free (GDataGDRating *self);

/**
 * GDataGDFeedLink:
 * @href: the URI of the related feed
 * @rel: the relationship between the related feed and the current item, or %NULL
 * @count_hint: a hint about the number of items in the related feed
 *
 * A structure fully representing a GData "rating" element. The @href field is required, but the others are optional.
 *
 * See the <ulink type="http" url="http://code.google.com/apis/gdata/elements.html#gdFeedLink">GData specification</ulink>
 * for more information.
 *
 * Currently, readOnly and feed functionality is not implemented in #GDataGDFeedLink.
 **/
typedef struct {
	gchar *rel;
	gchar *href;
	guint count_hint;
	/* TODO: support readOnly and feed */
} GDataGDFeedLink;

GDataGDFeedLink *gdata_gd_feed_link_new (const gchar *href, const gchar *rel, guint count_hint);
void gdata_gd_feed_link_free (GDataGDFeedLink *self);

/**
 * GDataGDWho:
 * @rel: the relationship between the item and this person, or %NULL
 * @value_string: a string to represent the person, or %NULL
 * @email: the person's e-mail address, or %NULL
 *
 * A structure fully representing a GData "who" element. All fields are optional.
 *
 * See the <ulink type="http" url="http://code.google.com/apis/gdata/elements.html#gdWho">GData specification</ulink>
 * for more information.
 *
 * Currently, entryLink functionality is not implemented in #GDataGDWho.
 **/
typedef struct {
	gchar *rel;
	gchar *value_string;
	gchar *email;
	/* TODO: deal with the attendeeType, attendeeStatus and entryLink */
} GDataGDWho;

GDataGDWho *gdata_gd_who_new (const gchar *rel, const gchar *value_string, const gchar *email);
void gdata_gd_who_free (GDataGDWho *self);

/**
 * GDataGDWhere:
 * @rel: the relationship between the item and this place, or %NULL
 * @value_string: a string to represent the place, or %NULL
 * @label: a human-readable label for the place, or %NULL
 *
 * A structure fully representing a GData "where" element. All fields are optional.
 *
 * See the <ulink type="http" url="http://code.google.com/apis/gdata/elements.html#gdWhere">GData specification</ulink>
 * for more information.
 *
 * Currently, entryLink functionality is not implemented in #GDataGDWhere.
 **/
typedef struct {
	gchar *rel;
	gchar *value_string;
	gchar *label;
	/* TODO: deal with the entryLink */
} GDataGDWhere;

GDataGDWhere *gdata_gd_where_new (const gchar *rel, const gchar *value_string, const gchar *label);
void gdata_gd_where_free (GDataGDWhere *self);

/**
 * GDataGDEmailAddress:
 * @address: the e-mail address
 * @rel: the relationship between the e-mail address and its owner, or %NULL
 * @label: a human-readable label for the e-mail address, or %NULL
 * @primary: %TRUE if this e-mail address is its owner's primary address, %FALSE otherwise
 *
 * A structure fully representing a GData "email" element. The @address field is required, but the others
 * are optional.
 *
 * See the <ulink type="http" url="http://code.google.com/apis/gdata/elements.html#gdEmail">GData specification</ulink>
 * for more information.
 **/
typedef struct {
	gchar *address;
	gchar *rel;
	gchar *label;
	gboolean primary;
} GDataGDEmailAddress;

GDataGDEmailAddress *gdata_gd_email_address_new (const gchar *address, const gchar *rel, const gchar *label, gboolean primary);
void gdata_gd_email_address_free (GDataGDEmailAddress *self);

/**
 * GDataGDIMAddress:
 * @address: the IM address
 * @protocol: a URI identifying the IM protocol, or %NULL
 * @rel: the relationship between the IM address and its owner, or %NULL
 * @label: a human-readable label for the IM address, or %NULL
 * @primary: %TRUE if this IM address is its owner's primary address, %FALSE otherwise
 *
 * A structure fully representing a GData "im" element. The @address field is required, but the others are optional.
 *
 * See the <ulink type="http" url="http://code.google.com/apis/gdata/elements.html#gdIm">GData specification</ulink>
 * for more information.
 **/
typedef struct {
	gchar *address;
	gchar *protocol;
	gchar *rel;
	gchar *label;
	gboolean primary;
} GDataGDIMAddress;

GDataGDIMAddress *gdata_gd_im_address_new (const gchar *address, const gchar *protocol, const gchar *rel, const gchar *label, gboolean primary);
void gdata_gd_im_address_free (GDataGDIMAddress *self);

/**
 * GDataGDPhoneNumber:
 * @phone_number: the phone number, in human-readable format
 * @rel: the relationship between the phone number and its owner, or %NULL
 * @label: a human-readable label for the phone number, or %NULL
 * @uri: a "tel URI" to represent the number formally (see
 * <ulink type="http" url="http://www.ietf.org/rfc/rfc3966.txt">RFC 3966</ulink>), or %NULL
 * @primary: %TRUE if this phone number is its owner's primary number, %FALSE otherwise
 *
 * A structure fully representing a GData "phoneNumber" element. The @phone_number field is required,
 * but the others are optional.
 *
 * See the <ulink type="http" url="http://code.google.com/apis/gdata/elements.html#gdPhoneNumber">GData specification</ulink>
 * for more information.
 **/
typedef struct {
	gchar *phone_number;
	gchar *rel;
	gchar *label;
	gchar *uri;
	gboolean primary;
} GDataGDPhoneNumber;

GDataGDPhoneNumber *gdata_gd_phone_number_new (const gchar *phone_number, const gchar *rel, const gchar *label, const gchar *uri, gboolean primary);
void gdata_gd_phone_number_free (GDataGDPhoneNumber *self);

/**
 * GDataGDPostalAddress:
 * @address: the postal address, in human-readable format (new lines are significant)
 * @rel: the relationship between the address and its owner, or %NULL
 * @label: a human-readable label for the address, or %NULL
 * @primary: %TRUE if this phone number is its owner's primary number, %FALSE otherwise
 *
 * A structure fully representing a GData "postalAddress" element. The @address field is required,
 * but the others are optional.
 *
 * See the <ulink type="http" url="http://code.google.com/apis/gdata/elements.html#gdPostalAddress">GData specification</ulink>
 * for more information.
 **/
typedef struct {
	gchar *address;
	gchar *rel;
	gchar *label;
	gboolean primary;
} GDataGDPostalAddress;

GDataGDPostalAddress *gdata_gd_postal_address_new (const gchar *address, const gchar *rel, const gchar *label, gboolean primary);
void gdata_gd_postal_address_free (GDataGDPostalAddress *self);

/**
 * GDataGDOrganization:
 * @name: the name of the organization, or %NULL
 * @title: the owner's title within the organization, or %NULL
 * @rel: the relationship between the organization and its owner, or %NULL
 * @label: a human-readable label for the organization, or %NULL
 * @primary: %TRUE if this organization is its owner's primary organization, %FALSE otherwise
 *
 * A structure fully representing a GData "organization" element. All fields are optional.
 *
 * See the <ulink type="http" url="http://code.google.com/apis/gdata/elements.html#gdOrganization">GData specification</ulink>
 * for more information.
 **/
typedef struct {
	gchar *name;
	gchar *title;
	gchar *rel;
	gchar *label;
	gboolean primary;
} GDataGDOrganization;

GDataGDOrganization *gdata_gd_organization_new (const gchar *name, const gchar *title, const gchar *rel, const gchar *label, gboolean primary);
void gdata_gd_organization_free (GDataGDOrganization *self);

G_END_DECLS

#endif /* !GDATA_GDATA_H */
