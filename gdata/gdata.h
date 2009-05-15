/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * GData Client
 * Copyright (C) Philip Withnall 2008-2009 <philip@tecnocode.co.uk>
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

#ifndef GDATA_H
#define GDATA_H

/* Core files */
#include <gdata/gdata-entry.h>
#include <gdata/gdata-feed.h>
#include <gdata/gdata-service.h>
#include <gdata/gdata-types.h>
#include <gdata/gdata-parser.h>
#include <gdata/gdata-query.h>
#include <gdata/gdata-enums.h>

/* Namespaces */
#include <gdata/gdata-atom.h>
#include <gdata/gdata-gdata.h>
#include <gdata/gdata-media-rss.h>

/* Services */

/* YouTube */
#include <gdata/services/youtube/gdata-youtube-service.h>
#include <gdata/services/youtube/gdata-youtube-query.h>
#include <gdata/services/youtube/gdata-youtube-video.h>
#include <gdata/services/youtube/gdata-youtube.h>
#include <gdata/services/youtube/gdata-youtube-enums.h>

/* Google Calendar */
#include <gdata/services/calendar/gdata-calendar-service.h>
#include <gdata/services/calendar/gdata-calendar-calendar.h>
#include <gdata/services/calendar/gdata-calendar-event.h>
#include <gdata/services/calendar/gdata-calendar-query.h>

/* Google Contacts */
#include <gdata/services/contacts/gdata-contacts-service.h>
#include <gdata/services/contacts/gdata-contacts-contact.h>
#include <gdata/services/contacts/gdata-contacts-query.h>

#endif /* !GDATA_H */
