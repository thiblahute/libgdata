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
 * SECTION:gdata-gdata
 * @short_description: GData namespace API
 * @stability: Unstable
 * @include: gdata/gdata-gdata.h
 *
 * The structures here represent several core GData-specific elements used by various different GData-based services, from the "gdata" namespace.
 *
 * For more information on the common GData elements, see the <ulink type="http" url="http://code.google.com/apis/gdata/elements.html#gdReference">
 * GData specification</ulink>.
 **/

#include "gdata-gdata.h"

/**
 * gdata_gd_rating_new:
 * @min: the minimum rating which can be chosen (typically %1)
 * @max: the maximum rating which can be chosen (typically %5)
 * @num_raters: the number of people who have rated the item
 * @average: the average rating for the item
 *
 * Creates a new #GDataGDRating. More information is available in the <ulink type="http"
 * url="http://code.google.com/apis/gdata/elements.html#gdRating">GData specification</ulink>.
 *
 * Currently, rel and value functionality is not implemented in #GDataGDRating.
 *
 * Return value: a new #GDataGDRating
 **/
GDataGDRating *
gdata_gd_rating_new (guint min, guint max, guint num_raters, gdouble average)
{
	GDataGDRating *self = g_slice_new (GDataGDRating);
	self->min = min;
	self->max = max;
	self->num_raters = num_raters;
	self->average = average;
	return self;
}

/**
 * gdata_gd_rating_free:
 * @self: a #GDataGDRating
 *
 * Frees a #GDataGDRating.
 **/
void
gdata_gd_rating_free (GDataGDRating *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_slice_free (GDataGDRating, self);
}

/**
 * gdata_gd_feed_link_new:
 * @href: the URI of the related feed
 * @rel: the relationship between the related feed and the current item, or %NULL
 * @count_hint: a hint about the number of items in the related feed
 *
 * Creates a new #GDataGDFeedLink. More information is available in the <ulink type="http"
 * url="http://code.google.com/apis/gdata/elements.html#gdFeedLink">GData specification</ulink>.
 *
 * Currently, readOnly and feed functionality is not implemented in #GDataGDFeedLink.
 *
 * Return value: a new #GDataGDFeedLink, or %NULL on error
 **/
GDataGDFeedLink *
gdata_gd_feed_link_new (const gchar *href, const gchar *rel, guint count_hint)
{
	GDataGDFeedLink *self;

	g_return_val_if_fail (href != NULL, NULL);

	self = g_slice_new (GDataGDFeedLink);
	self->href = g_strdup (href);
	self->rel = g_strdup (rel);
	self->count_hint = count_hint;
	return self;
}

/**
 * gdata_gd_feed_link_free:
 * @self: a #GDataGDFeedLink
 *
 * Frees a #GDataGDFeedLink.
 **/
void
gdata_gd_feed_link_free (GDataGDFeedLink *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->href);
	g_free (self->rel);
	g_slice_free (GDataGDFeedLink, self);
}

/**
 * gdata_gd_when_new:
 * @start_time: when the event starts or (for zero-duration events) when it occurs
 * @end_time: when the event ends, or %NULL
 * @value_string: a string to represent the time period, or %NULL
 * @reminders: a #GList of #GDataGDReminder<!-- -->s for the time period, or %NULL
 *
 * Creates a new #GDataGDWhen. More information is available in the <ulink type="http"
 * url="http://code.google.com/apis/gdata/elements.html#gdWhen">GData specification</ulink>.
 *
 * This function takes ownership of @reminders, so the list (or its entries) must not be freed
 * by the caller after a call to gdata_gd_when_new has finished.
 *
 * Return value: a new #GDataGDWhen, or %NULL on error
 **/
GDataGDWhen *
gdata_gd_when_new (GTimeVal *start_time, GTimeVal *end_time, const gchar *value_string, GList *reminders)
{
	GDataGDWhen *self;

	g_return_val_if_fail (start_time != NULL, NULL);

	self = g_slice_new (GDataGDWhen);

	self->start_time = *start_time;
	if (end_time != NULL) {
		self->end_time = *end_time;
	} else {
		self->end_time.tv_sec = 0;
		self->end_time.tv_usec = 0;
	}

	self->value_string = g_strdup (value_string);
	self->reminders = reminders;

	return self;
}

/**
 * gdata_gd_when_free:
 * @self: a #GDataGDWhen
 *
 * Frees a #GDataGDWhen.
 **/
void
gdata_gd_when_free (GDataGDWhen *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->value_string);
	g_list_foreach (self->reminders, (GFunc) gdata_gd_reminder_free, NULL);
	g_list_free (self->reminders);
	g_slice_free (GDataGDWhen, self);
}

/**
 * gdata_gd_who_new:
 * @rel: the relationship between the item and this person, or %NULL
 * @value_string: a string to represent the person, or %NULL
 * @email: the person's e-mail address, or %NULL
 *
 * Creates a new #GDataGDWho. More information is available in the <ulink type="http"
 * url="http://code.google.com/apis/gdata/elements.html#gdWho">GData specification</ulink>.
 *
 * Currently, entryLink functionality is not implemented in #GDataGDWho.
 *
 * Return value: a new #GDataGDWho
 **/
GDataGDWho *
gdata_gd_who_new (const gchar *rel, const gchar *value_string, const gchar *email)
{
	GDataGDWho *self = g_slice_new (GDataGDWho);
	self->rel = g_strdup (rel);
	self->email = g_strdup (email);
	self->value_string = g_strdup (value_string);
	return self;
}

/**
 * gdata_gd_who_free:
 * @self: a #GDataGDWho
 *
 * Frees a #GDataGDWho.
 **/
void
gdata_gd_who_free (GDataGDWho *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->rel);
	g_free (self->email);
	g_free (self->value_string);
	g_slice_free (GDataGDWho, self);
}

/**
 * gdata_gd_where_new:
 * @rel: the relationship between the item and this place, or %NULL
 * @value_string: a string to represent the place, or %NULL
 * @label: a human-readable label for the place, or %NULL
 *
 * Creates a new #GDataGDWhere. More information is available in the <ulink type="http"
 * url="http://code.google.com/apis/gdata/elements.html#gdWhere">GData specification</ulink>.
 *
 * Currently, entryLink functionality is not implemented in #GDataGDWhere.
 *
 * Return value: a new #GDataGDWhere
 **/
GDataGDWhere *
gdata_gd_where_new (const gchar *rel, const gchar *value_string, const gchar *label)
{
	GDataGDWhere *self = g_slice_new (GDataGDWhere);
	self->rel = g_strdup (rel);
	self->label = g_strdup (label);
	self->value_string = g_strdup (value_string);
	return self;
}

/**
 * gdata_gd_where_free:
 * @self: a #GDataGDWhere
 *
 * Frees a #GDataGDWhere.
 **/
void
gdata_gd_where_free (GDataGDWhere *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->rel);
	g_free (self->label);
	g_free (self->value_string);
	g_slice_free (GDataGDWhere, self);
}

/**
 * gdata_gd_email_address_new:
 * @address: the e-mail address
 * @rel: the relationship between the e-mail address and its owner, or %NULL
 * @label: a human-readable label for the e-mail address, or %NULL
 * @primary: %TRUE if this e-mail address is its owner's primary address, %FALSE otherwise
 *
 * Creates a new #GDataGDEmailAddress. More information is available in the <ulink type="http"
 * url="http://code.google.com/apis/gdata/elements.html#gdEmail">GData specification</ulink>.
 *
 * Return value: a new #GDataGDEmailAddress, or %NULL on error
 **/
GDataGDEmailAddress *
gdata_gd_email_address_new (const gchar *address, const gchar *rel, const gchar *label, gboolean primary)
{
	GDataGDEmailAddress *self;

	g_return_val_if_fail (address != NULL, NULL);

	self = g_slice_new (GDataGDEmailAddress);
	self->address = g_strdup (address);
	self->rel = g_strdup (rel);
	self->label = g_strdup (label);
	self->primary = primary;
	return self;
}

/**
 * gdata_gd_email_address_free:
 * @self: a #GDataGDEmailAddress
 *
 * Frees a #GDataGDEmailAddress.
 **/
void
gdata_gd_email_address_free (GDataGDEmailAddress *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->address);
	g_free (self->rel);
	g_free (self->label);
	g_slice_free (GDataGDEmailAddress, self);
}

/**
 * gdata_gd_im_address_new:
 * @address: the IM address
 * @protocol: a URI identifying the IM protocol, or %NULL
 * @rel: the relationship between the IM address and its owner, or %NULL
 * @label: a human-readable label for the IM address, or %NULL
 * @primary: %TRUE if this IM address is its owner's primary address, %FALSE otherwise
 *
 * Creates a new #GDataGDIMAddress. More information is available in the <ulink type="http"
 * url="http://code.google.com/apis/gdata/elements.html#gdIm">GData specification</ulink>.
 *
 * Return value: a new #GDataGDIMAddress, or %NULL on error
 **/
GDataGDIMAddress *
gdata_gd_im_address_new (const gchar *address, const gchar *protocol, const gchar *rel, const gchar *label, gboolean primary)
{
	GDataGDIMAddress *self;

	g_return_val_if_fail (address != NULL, NULL);

	self = g_slice_new (GDataGDIMAddress);
	self->address = g_strdup (address);
	self->protocol = g_strdup (protocol);
	self->rel = g_strdup (rel);
	self->label = g_strdup (label);
	self->primary = primary;
	return self;
}

/**
 * gdata_gd_im_address_free:
 * @self: a #GDataGDIMAddress
 *
 * Frees a #GDataGDIMAddress.
 **/
void
gdata_gd_im_address_free (GDataGDIMAddress *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->address);
	g_free (self->protocol);
	g_free (self->rel);
	g_free (self->label);
	g_slice_free (GDataGDIMAddress, self);
}

/**
 * gdata_gd_phone_number_new:
 * @number: the phone number, in human-readable format
 * @rel: the relationship between the phone number and its owner, or %NULL
 * @label: a human-readable label for the phone number, or %NULL
 * @uri: a "tel URI" to represent the number formally (see
 * <ulink type="http" url="http://www.ietf.org/rfc/rfc3966.txt">RFC 3966</ulink>), or %NULL
 * @primary: %TRUE if this phone number is its owner's primary number, %FALSE otherwise
 *
 * Creates a new #GDataGDPhoneNumber. More information is available in the <ulink type="http"
 * url="http://code.google.com/apis/gdata/elements.html#gdPhoneNumber">GData specification</ulink>.
 *
 * Return value: a new #GDataGDPhoneNumber, or %NULL on error
 **/
GDataGDPhoneNumber *
gdata_gd_phone_number_new (const gchar *number, const gchar *rel, const gchar *label, const gchar *uri, gboolean primary)
{
	GDataGDPhoneNumber *self;

	g_return_val_if_fail (number != NULL, NULL);

	self = g_slice_new (GDataGDPhoneNumber);
	self->number = g_strdup (number);
	self->rel = g_strdup (rel);
	self->label = g_strdup (label);
	self->uri = g_strdup (uri);
	self->primary = primary;
	return self;
}

/**
 * gdata_gd_phone_number_free:
 * @self: a #GDataGDPhoneNumber
 *
 * Frees a #GDataGDPhoneNumber.
 **/
void
gdata_gd_phone_number_free (GDataGDPhoneNumber *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->number);
	g_free (self->rel);
	g_free (self->label);
	g_free (self->uri);
	g_slice_free (GDataGDPhoneNumber, self);
}

/**
 * gdata_gd_postal_address_new:
 * @address: the postal address, in human-readable format (new lines are significant)
 * @rel: the relationship between the address and its owner, or %NULL
 * @label: a human-readable label for the address, or %NULL
 * @primary: %TRUE if this phone number is its owner's primary number, %FALSE otherwise
 *
 * Creates a new #GDataGDPostalAddress. More information is available in the <ulink type="http"
 * url="http://code.google.com/apis/gdata/elements.html#gdPostalAddress">GData specification</ulink>.
 *
 * Return value: a new #GDataGDPostalAddress, or %NULL on error
 **/
GDataGDPostalAddress *
gdata_gd_postal_address_new (const gchar *address, const gchar *rel, const gchar *label, gboolean primary)
{
	GDataGDPostalAddress *self;

	g_return_val_if_fail (address != NULL, NULL);

	self = g_slice_new (GDataGDPostalAddress);
	self->address = g_strdup (address);
	self->rel = g_strdup (rel);
	self->label = g_strdup (label);
	self->primary = primary;
	return self;
}

/**
 * gdata_gd_postal_address_free:
 * @self: a #GDataGDPostalAddress
 *
 * Frees a #GDataGDPostalAddress.
 **/
void
gdata_gd_postal_address_free (GDataGDPostalAddress *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->address);
	g_free (self->rel);
	g_free (self->label);
	g_slice_free (GDataGDPostalAddress, self);
}

/**
 * gdata_gd_organization_new:
 * @name: the name of the organization, or %NULL
 * @title: the owner's title within the organization, or %NULL
 * @rel: the relationship between the organization and its owner, or %NULL
 * @label: a human-readable label for the organization, or %NULL
 * @primary: %TRUE if this organization is its owner's primary organization, %FALSE otherwise
 *
 * Creates a new #GDataGDOrganization. More information is available in the <ulink type="http"
 * url="http://code.google.com/apis/gdata/elements.html#gdOrganization">GData specification</ulink>.
 *
 * Return value: a new #GDataGDOrganization, or %NULL on error
 **/
GDataGDOrganization *
gdata_gd_organization_new (const gchar *name, const gchar *title, const gchar *rel, const gchar *label, gboolean primary)
{
	GDataGDOrganization *self = g_slice_new (GDataGDOrganization);
	self->name = g_strdup (name);
	self->title = g_strdup (title);
	self->rel = g_strdup (rel);
	self->label = g_strdup (label);
	self->primary = primary;
	return self;
}

/**
 * gdata_gd_organization_free:
 * @self: a #GDataGDOrganization
 *
 * Frees a #GDataGDOrganization.
 **/
void
gdata_gd_organization_free (GDataGDOrganization *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->name);
	g_free (self->title);
	g_free (self->rel);
	g_free (self->label);
	g_slice_free (GDataGDOrganization, self);
}

/**
 * gdata_gd_reminder_new:
 * @method: the notification method the reminder should use, or %NULL
 * @absolute_time: the absolute time for the reminder, or %NULL
 * @days: number of days before the event's start time for the reminder, or %-1
 * @hours: number of hours before the event's start time for the reminder, or %-1
 * @minutes: number of minutes before the event's start time for the reminder, or %-1
 *
 * Creates a new #GDataGDReminder. More information is available in the <ulink type="http"
 * url="http://code.google.com/apis/gdata/elements.html#gdReminder">GData specification</ulink>.
 *
 * Return value: a new #GDataGDReminder, or %NULL on error
 **/
GDataGDReminder *
gdata_gd_reminder_new (const gchar *method, GTimeVal *absolute_time, gint days, gint hours, gint minutes)
{
	GDataGDReminder *self;

	g_return_val_if_fail (absolute_time != NULL && (days != -1 || hours != -1 || minutes != -1), NULL);
	g_return_val_if_fail (days != -1 && (hours != -1 || minutes != -1), NULL);
	g_return_val_if_fail (hours != -1 && (minutes != -1 || days != -1), NULL);
	g_return_val_if_fail (minutes != -1 && (days != -1 || hours != -1), NULL);

	self = g_slice_new (GDataGDReminder);

	if (absolute_time != NULL) {
		self->absolute_time = *absolute_time;
	} else {
		self->absolute_time.tv_sec = 0;
		self->absolute_time.tv_usec = 0;
	}

	self->method = g_strdup (method);
	self->days = days;
	self->hours = hours;
	self->minutes = minutes;

	return self;
}

/**
 * gdata_gd_reminder_free:
 * @self: a #GDataGDReminder
 *
 * Frees a #GDataGDReminder.
 **/
void
gdata_gd_reminder_free (GDataGDReminder *self)
{
	if (G_UNLIKELY (self == NULL))
		return;

	g_free (self->method);
	g_slice_free (GDataGDReminder, self);
}
