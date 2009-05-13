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

GDataGDRating *gdata_gd_rating_new (guint min, guint max, guint num_raters, gdouble average) G_GNUC_WARN_UNUSED_RESULT;
void gdata_gd_rating_free (GDataGDRating *self);

/**
 * GDataGDFeedLink:
 * @href: the URI of the related feed
 * @rel: the relationship between the related feed and the current item, or %NULL
 * @count_hint: a hint about the number of items in the related feed
 * @read_only: %TRUE if the feed is read-only, %FALSE otherwise
 *
 * A structure fully representing a GData "rating" element. The @href field is required, but the others are optional.
 *
 * See the <ulink type="http" url="http://code.google.com/apis/gdata/elements.html#gdFeedLink">GData specification</ulink>
 * for more information.
 *
 * Currently, embedded feed functionality is not implemented in #GDataGDFeedLink.
 **/
typedef struct {
	gchar *rel;
	gchar *href;
	guint count_hint;
	gboolean read_only;
} GDataGDFeedLink;

GDataGDFeedLink *gdata_gd_feed_link_new (const gchar *href, const gchar *rel, guint count_hint, gboolean read_only) G_GNUC_WARN_UNUSED_RESULT;
void gdata_gd_feed_link_free (GDataGDFeedLink *self);

/**
 * GDataGDWhen:
 * @start_time: when the event starts or (for zero-duration events) when it occurs
 * @end_time: when the event ends
 * @is_date: %TRUE if @start_time and @end_time specify dates rather than times, %FALSE otherwise
 * @value_string: a string to represent the time period, or %NULL
 * @reminders: a #GList of #GDataGDReminder<!-- -->s for the time period, or %NULL
 *
 * A structure fully representing a GData "when" element. The @start_time field is required, but the others are optional.
 *
 * If @end_time is empty (all fields are zero), the structure is considered to represent: an instance in time if
 * @start_time is a time (if @is_date is %FALSE), or an entire day if @start_time is a date (if @is_date is %TRUE).
 *
 * See the <ulink type="http" url="http://code.google.com/apis/gdata/elements.html#gdWhen">GData specification</ulink>
 * for more information.
 **/
typedef struct {
	GTimeVal start_time;
	GTimeVal end_time;
	gboolean is_date;
	gchar *value_string;
	GList *reminders;
} GDataGDWhen;

GDataGDWhen *gdata_gd_when_new (GTimeVal *start_time, GTimeVal *end_time, gboolean is_date, const gchar *value_string, GList *reminders);
void gdata_gd_when_free (GDataGDWhen *self);

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

GDataGDWho *gdata_gd_who_new (const gchar *rel, const gchar *value_string, const gchar *email) G_GNUC_WARN_UNUSED_RESULT;
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

GDataGDWhere *gdata_gd_where_new (const gchar *rel, const gchar *value_string, const gchar *label) G_GNUC_WARN_UNUSED_RESULT;
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

GDataGDEmailAddress *gdata_gd_email_address_new (const gchar *address, const gchar *rel, const gchar *label, gboolean primary) G_GNUC_WARN_UNUSED_RESULT;
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

GDataGDIMAddress *gdata_gd_im_address_new (const gchar *address, const gchar *protocol, const gchar *rel,
					   const gchar *label, gboolean primary) G_GNUC_WARN_UNUSED_RESULT;
void gdata_gd_im_address_free (GDataGDIMAddress *self);

/**
 * GDataGDPhoneNumber:
 * @number: the phone number, in human-readable format
 * @rel: the relationship between the phone number and its owner, or %NULL
 * @label: a human-readable label for the phone number, or %NULL
 * @uri: a "tel URI" to represent the number formally (see
 * <ulink type="http" url="http://www.ietf.org/rfc/rfc3966.txt">RFC 3966</ulink>), or %NULL
 * @primary: %TRUE if this phone number is its owner's primary number, %FALSE otherwise
 *
 * A structure fully representing a GData "phoneNumber" element. The @number field is required,
 * but the others are optional.
 *
 * See the <ulink type="http" url="http://code.google.com/apis/gdata/elements.html#gdPhoneNumber">GData specification</ulink>
 * for more information.
 **/
typedef struct {
	gchar *number;
	gchar *rel;
	gchar *label;
	gchar *uri;
	gboolean primary;
} GDataGDPhoneNumber;

GDataGDPhoneNumber *gdata_gd_phone_number_new (const gchar *number, const gchar *rel, const gchar *label,
					       const gchar *uri, gboolean primary) G_GNUC_WARN_UNUSED_RESULT;
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

GDataGDPostalAddress *gdata_gd_postal_address_new (const gchar *address, const gchar *rel, const gchar *label,
						   gboolean primary) G_GNUC_WARN_UNUSED_RESULT;
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

GDataGDOrganization *gdata_gd_organization_new (const gchar *name, const gchar *title, const gchar *rel,
						const gchar *label, gboolean primary) G_GNUC_WARN_UNUSED_RESULT;
void gdata_gd_organization_free (GDataGDOrganization *self);

/**
 * GDataGDReminder:
 * @method: the notification method the reminder should use, or %NULL
 * @absolute_time: the absolute time for the reminder, or %NULL
 * @days: number of days before the event's start time for the reminder, or %-1
 * @hours: number of hours before the event's start time for the reminder, or %-1
 * @minutes: number of minutes before the event's start time for the reminder, or %-1
 *
 * A structure fully representing a GData "reminder" element. All fields are optional. The @days, @hours
 * and @minutes fields are mutually exclusive with each other, and all mutually exclusive with @absolute_time.
 *
 * See the <ulink type="http" url="http://code.google.com/apis/gdata/elements.html#gdReminder">GData specification</ulink>
 * for more information.
 **/
typedef struct {
	gchar *method;
	GTimeVal absolute_time;
	gint days;
	gint hours;
	gint minutes;
} GDataGDReminder;

GDataGDReminder *gdata_gd_reminder_new (const gchar *method, GTimeVal *absolute_time, gint days, gint hours, gint minutes);
void gdata_gd_reminder_free (GDataGDReminder *self);

G_END_DECLS

#endif /* !GDATA_GDATA_H */
