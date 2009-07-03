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

#include <glib.h>
#include <locale.h>

#include "gdata.h"

static void
test_entry_get_xml (void)
{
	/*GTimeVal updated, published, updated2, published2;*/
	GDataEntry *entry, *entry2;
	GDataCategory *category;
	GDataLink *link;
	GDataAuthor *author;
	gchar *xml;
	GError *error = NULL;

	entry = gdata_entry_new (NULL);
	gdata_entry_set_title (entry, "Testing title & escaping");
	gdata_entry_set_content (entry, "This is some sample content testing, amongst other things, <markup> & odd characters‽");

	/*g_time_val_from_iso8601 ("2009-01-25T14:07:37.880860Z", &updated);
	gdata_entry_set_updated (entry, &updated);

	g_time_val_from_iso8601 ("2009-01-23T14:06:37.880860Z", &published);
	gdata_entry_set_published (entry, &published);*/

	/* Categories */
	category = gdata_category_new ("test", NULL, NULL);
	gdata_entry_add_category (entry, category);
	g_object_unref (category);
	category = gdata_category_new ("example", NULL, "Example stuff");
	gdata_entry_add_category (entry, category);
	g_object_unref (category);
	category = gdata_category_new ("Film", "http://gdata.youtube.com/schemas/2007/categories.cat", "Film & Animation");
	gdata_entry_add_category (entry, category);
	g_object_unref (category);

	/* Links */
	link = gdata_link_new ("http://test.com/", GDATA_LINK_SELF);
	gdata_link_set_content_type (link, "application/atom+xml");
	gdata_entry_add_link (entry, link);
	g_object_unref (link);
	link = gdata_link_new ("http://example.com/", NULL);
	gdata_entry_add_link (entry, link);
	g_object_unref (link);
	link = gdata_link_new ("http://test.mn/", GDATA_LINK_RELATED);
	gdata_link_set_content_type (link, "text/html");
	gdata_link_set_language (link, "mn");
	gdata_link_set_title (link, "A treatise on Mongolian test websites & other stuff.");
	gdata_link_set_length (link, 5010);
	gdata_entry_add_link (entry, link);
	g_object_unref (link);

	/* Authors */
	author = gdata_author_new ("Joe Bloggs", "http://example.com/", "joe@example.com");
	gdata_entry_add_author (entry, author);
	g_object_unref (author);
	author = gdata_author_new ("John Smith", NULL, "smith.john@example.com");
	gdata_entry_add_author (entry, author);
	g_object_unref (author);
	author = gdata_author_new ("F. Barr؟", NULL, NULL);
	gdata_entry_add_author (entry, author);
	g_object_unref (author);

	/* Check the generated XML's OK */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (entry));
	g_assert_cmpstr (xml, ==,
			 "<entry xmlns='http://www.w3.org/2005/Atom' xmlns:gd='http://schemas.google.com/g/2005'>"
				 "<title type='text'>Testing title &amp; escaping</title>"
				 /*"<updated>2009-01-25T14:07:37.880860Z</updated>"
				 "<published>2009-01-23T14:06:37.880860Z</published>"*/
				 "<content type='text'>This is some sample content testing, amongst other things, &lt;markup&gt; &amp; odd characters\342\200\275</content>"
				 "<category term='Film' scheme='http://gdata.youtube.com/schemas/2007/categories.cat' label='Film &amp; Animation'/>"
				 "<category term='example' label='Example stuff'/>"
				 "<category term='test'/>"
				 "<link href='http://test.mn/' title='A treatise on Mongolian test websites &amp; other stuff.' rel='http://www.iana.org/assignments/relation/related' type='text/html' hreflang='mn' length='5010'/>"
				 "<link href='http://example.com/' rel='http://www.iana.org/assignments/relation/alternate'/>"
				 "<link href='http://test.com/' rel='http://www.iana.org/assignments/relation/self' type='application/atom+xml'/>"
				 "<author><name>F. Barr\330\237</name></author>"
				 "<author><name>John Smith</name><email>smith.john@example.com</email></author>"
				 "<author><name>Joe Bloggs</name><uri>http://example.com/</uri><email>joe@example.com</email></author>"
			 "</entry>");

	/* Check again by re-parsing the XML to a GDataEntry */
	entry2 = GDATA_ENTRY (gdata_parsable_new_from_xml (GDATA_TYPE_ENTRY, xml, -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_ENTRY (entry2));
	g_clear_error (&error);
	g_free (xml);

	g_assert_cmpstr (gdata_entry_get_title (entry), ==, gdata_entry_get_title (entry2));
	g_assert_cmpstr (gdata_entry_get_id (entry), ==, gdata_entry_get_id (entry2)); /* should both be NULL */
	g_assert_cmpstr (gdata_entry_get_content (entry), ==, gdata_entry_get_content (entry2));

	/*gdata_entry_get_updated (entry, &updated);
	gdata_entry_get_updated (entry2, &updated2);
	g_assert_cmpuint (updated.tv_sec, ==, updated2.tv_sec);
	g_assert_cmpuint (updated.tv_usec, ==, updated2.tv_usec);

	gdata_entry_get_published (entry, &published);
	gdata_entry_get_published (entry2, &published2);
	g_assert_cmpuint (published.tv_sec, ==, published2.tv_sec);
	g_assert_cmpuint (published.tv_usec, ==, published2.tv_usec);*/

	/* TODO: Check categories, links and authors */

	g_object_unref (entry);
	g_object_unref (entry2);
}

static void
test_entry_parse_xml (void)
{
	GDataEntry *entry;
	gchar *xml;
	GError *error = NULL;

	/* Create an entry from XML with unhandled elements */
	entry = GDATA_ENTRY (gdata_parsable_new_from_xml (GDATA_TYPE_ENTRY,
		"<entry xmlns='http://www.w3.org/2005/Atom' xmlns:ns='http://example.com/'>"
			"<title type='text'>Testing unhandled XML</title>"
			"<updated>2009-01-25T14:07:37.880860Z</updated>"
			"<published>2009-01-23T14:06:37.880860Z</published>"
			"<content type='text'>Here we test unhandled XML elements.</content>"
			"<foobar>Test!</foobar>"
			"<barfoo shizzle='zing'/>"
			"<ns:barfoo shizzle='zing' fo='shizzle'>How about some characters‽</ns:barfoo>"
		 "</entry>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_ENTRY (entry));
	g_clear_error (&error);

	/* Now check the outputted XML from the entry still has the unhandled elements */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (entry));
	g_assert_cmpstr (xml, ==,
			 "<entry xmlns='http://www.w3.org/2005/Atom' xmlns:gd='http://schemas.google.com/g/2005' xmlns:ns='http://example.com/'>"
				"<title type='text'>Testing unhandled XML</title>"
				"<updated>2009-01-25T14:07:37.880860Z</updated>"
				"<published>2009-01-23T14:06:37.880860Z</published>"
				"<content type='text'>Here we test unhandled XML elements.</content>"
				"<foobar>Test!</foobar>"
				"<barfoo shizzle=\"zing\"/>"
				"<ns:barfoo shizzle=\"zing\" fo=\"shizzle\">How about some characters‽</ns:barfoo>"
			 "</entry>");
	g_free (xml);
}

static void
test_query_categories (void)
{
	GDataQuery *query;
	gchar *query_uri;

	query = gdata_query_new ("foobar");

	/* AND */
	gdata_query_set_categories (query, "Fritz/Laurie");
	query_uri = gdata_query_get_query_uri (query, "http://example.com");

	g_assert_cmpstr (query_uri, ==, "http://example.com/-/Fritz/Laurie?q=foobar");
	g_free (query_uri);

	/* OR */
	gdata_query_set_categories (query, "Fritz|Laurie");
	query_uri = gdata_query_get_query_uri (query, "http://example.com");

	g_assert_cmpstr (query_uri, ==, "http://example.com/-/Fritz%7CLaurie?q=foobar");
	g_free (query_uri);

	/* Combination */
	gdata_query_set_categories (query, "A|-{urn:google.com}B/-C");
	query_uri = gdata_query_get_query_uri (query, "http://example.com/gdata_test");

	g_assert_cmpstr (query_uri, ==, "http://example.com/gdata_test/-/A%7C-%7Burn%3Agoogle.com%7DB/-C?q=foobar");
	g_free (query_uri);

	/* Same combination without q param */
	gdata_query_set_q (query, NULL);
	query_uri = gdata_query_get_query_uri (query, "http://example.com");

	g_assert_cmpstr (query_uri, ==, "http://example.com/-/A%7C-%7Burn%3Agoogle.com%7DB/-C");
	g_free (query_uri);

	g_object_unref (query);
}

static void
test_color_parsing (void)
{
	GDataColor color;

	/* With hash */
	g_assert (gdata_color_from_hexadecimal ("#F99Ff0", &color) == TRUE);
	g_assert_cmpuint (color.red, ==, 249);
	g_assert_cmpuint (color.green, ==, 159);
	g_assert_cmpuint (color.blue, ==, 240);

	/* Without hash */
	g_assert (gdata_color_from_hexadecimal ("F99Ff0", &color) == TRUE);
	g_assert_cmpuint (color.red, ==, 249);
	g_assert_cmpuint (color.green, ==, 159);
	g_assert_cmpuint (color.blue, ==, 240);

	/* Invalid, but correct length */
	g_assert (gdata_color_from_hexadecimal ("foobar", &color) == FALSE);

	/* Wildly invalid */
	g_assert (gdata_color_from_hexadecimal ("this is not a real colour!", &color) == FALSE);
}

static void
test_color_output (void)
{
	GDataColor color;
	gchar *color_string;

	/* General test */
	g_assert (gdata_color_from_hexadecimal ("#F99Ff0", &color) == TRUE);
	color_string = gdata_color_to_hexadecimal (&color);
	g_assert_cmpstr (color_string, ==, "#f99ff0");
	g_free (color_string);

	/* Boundary tests */
	g_assert (gdata_color_from_hexadecimal ("#ffffff", &color) == TRUE);
	color_string = gdata_color_to_hexadecimal (&color);
	g_assert_cmpstr (color_string, ==, "#ffffff");
	g_free (color_string);

	g_assert (gdata_color_from_hexadecimal ("#000000", &color) == TRUE);
	color_string = gdata_color_to_hexadecimal (&color);
	g_assert_cmpstr (color_string, ==, "#000000");
	g_free (color_string);
}

/*static void
test_media_thumbnail_parse_time (const gchar *locale)
{
	g_test_bug ("584737");

	g_test_message ("Testing gdata_media_thumbnail_parse_time in the \"%s\" locale...", locale);
	g_assert_cmpstr (setlocale (LC_ALL, locale), ==, locale);

	g_assert_cmpint (gdata_media_thumbnail_parse_time ("00:01:42.500"), ==, 102500);
	g_assert_cmpint (gdata_media_thumbnail_parse_time ("00:02:45"), ==, 165000);
	g_assert_cmpint (gdata_media_thumbnail_parse_time ("12:00:15.000"), ==, 43215000);
	g_assert_cmpint (gdata_media_thumbnail_parse_time ("00:00:00"), ==, 0);
	g_assert_cmpint (gdata_media_thumbnail_parse_time ("foobar"), ==, -1);

	setlocale (LC_ALL, "");
}*/

static void
test_atom_author (void)
{
	GDataAuthor *author, *author2;
	gchar *xml;
	GError *error = NULL;

	author = GDATA_AUTHOR (gdata_parsable_new_from_xml (GDATA_TYPE_AUTHOR,
		"<author>"
			"<name>John Smöth</name>"
			"<uri>http://example.com/</uri>"
			"<email>john@example.com</email>"
		"</author>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_AUTHOR (author));
	g_clear_error (&error);

	/* Check the properties */
	g_assert_cmpstr (gdata_author_get_name (author), ==, "John Smöth");
	g_assert_cmpstr (gdata_author_get_uri (author), ==, "http://example.com/");
	g_assert_cmpstr (gdata_author_get_email_address (author), ==, "john@example.com");

	/* Compare it against another identical author */
	author2 = gdata_author_new ("John Smöth", "http://example.com/", "john@example.com");
	g_assert_cmpint (gdata_author_compare (author, author2), ==, 0);
	g_object_unref (author2);

	/* …and a different author */
	author2 = gdata_author_new ("Brian Blessed", NULL, NULL);
	g_assert_cmpint (gdata_author_compare (author, author2), !=, 0);
	g_object_unref (author2);

	/* Check the outputted XML is the same */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (author));
	g_assert_cmpstr (xml, ==,
			 "<author xmlns='http://www.w3.org/2005/Atom'>"
				"<name>John Smöth</name>"
				"<uri>http://example.com/</uri>"
				"<email>john@example.com</email>"
			 "</author>");
	g_free (xml);
	g_object_unref (author);

	/* Now parse an author with little information available */
	author = GDATA_AUTHOR (gdata_parsable_new_from_xml (GDATA_TYPE_AUTHOR,
		"<author>"
			"<name>James Johnson</name>"
		"</author>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_AUTHOR (author));
	g_clear_error (&error);

	/* Check the properties */
	g_assert_cmpstr (gdata_author_get_name (author), ==, "James Johnson");
	g_assert (gdata_author_get_uri (author) == NULL);
	g_assert (gdata_author_get_email_address (author) == NULL);
	g_object_unref (author);
}

static void
test_atom_category (void)
{
	GDataCategory *category, *category2;
	gchar *xml;
	GError *error = NULL;

	category = GDATA_CATEGORY (gdata_parsable_new_from_xml (GDATA_TYPE_CATEGORY,
		"<category term='jokes' scheme='http://foobar.com#categories' label='Jokes &amp; Trivia'/>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_CATEGORY (category));
	g_clear_error (&error);

	/* Check the properties */
	g_assert_cmpstr (gdata_category_get_term (category), ==, "jokes");
	g_assert_cmpstr (gdata_category_get_scheme (category), ==, "http://foobar.com#categories");
	g_assert_cmpstr (gdata_category_get_label (category), ==, "Jokes & Trivia");

	/* Compare it against another identical category */
	category2 = gdata_category_new ("jokes", "http://foobar.com#categories", "Jokes & Trivia");
	g_assert_cmpint (gdata_category_compare (category, category2), ==, 0);
	g_object_unref (category2);

	/* …and a different category */
	category2 = gdata_category_new ("sports", "http://foobar.com#categories", NULL);
	g_assert_cmpint (gdata_category_compare (category, category2), !=, 0);
	g_object_unref (category2);

	/* Check the outputted XML is the same */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (category));
	g_assert_cmpstr (xml, ==,
			 "<category xmlns='http://www.w3.org/2005/Atom' "
			 	"term='jokes' scheme='http://foobar.com#categories' label='Jokes &amp; Trivia'/>");
	g_free (xml);
	g_object_unref (category);

	/* Now parse a category with less information available */
	category = GDATA_CATEGORY (gdata_parsable_new_from_xml (GDATA_TYPE_CATEGORY,
		"<category term='sports'/>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_CATEGORY (category));
	g_clear_error (&error);

	/* Check the properties */
	g_assert_cmpstr (gdata_category_get_term (category), ==, "sports");
	g_assert (gdata_category_get_scheme (category) == NULL);
	g_assert (gdata_category_get_label (category) == NULL);
	g_object_unref (category);

	/* Try a category with custom content */
	category = GDATA_CATEGORY (gdata_parsable_new_from_xml (GDATA_TYPE_CATEGORY,
		"<category term='documentary'>"
			"<foobar/>"
			"<shizzle/>"
		"</category>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_CATEGORY (category));
	g_clear_error (&error);

	/* Check the outputted XML contains the unknown XML */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (category));
	g_assert_cmpstr (xml, ==,
			 "<category xmlns='http://www.w3.org/2005/Atom' term='documentary'>"
			 	"<foobar/>"
			 	"<shizzle/>"
			 "</category>");
	g_free (xml);
	g_object_unref (category);
}

static void
test_atom_generator (void)
{
	GDataGenerator *generator;
	GError *error = NULL;

	generator = GDATA_GENERATOR (gdata_parsable_new_from_xml (GDATA_TYPE_GENERATOR,
		"<generator uri='http://example.com/' version='15'>Bach &amp; Son's Generator</generator>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_GENERATOR (generator));
	g_clear_error (&error);

	/* Check the properties */
	g_assert_cmpstr (gdata_generator_get_name (generator), ==, "Bach & Son's Generator");
	g_assert_cmpstr (gdata_generator_get_uri (generator), ==, "http://example.com/");
	g_assert_cmpstr (gdata_generator_get_version (generator), ==, "15");
	g_object_unref (generator);

	/* Now parse a generator with less information available */
	generator = GDATA_GENERATOR (gdata_parsable_new_from_xml (GDATA_TYPE_GENERATOR,
		"<generator/>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_GENERATOR (generator));
	g_clear_error (&error);

	/* Check the properties */
	g_assert (gdata_generator_get_name (generator) == NULL);
	g_assert (gdata_generator_get_uri (generator) == NULL);
	g_assert (gdata_generator_get_version (generator) == NULL);
	g_object_unref (generator);
}

static void
test_atom_link (void)
{
	GDataLink *link, *link2;
	gchar *xml;
	GError *error = NULL;

	link = GDATA_LINK (gdata_parsable_new_from_xml (GDATA_TYPE_LINK,
		"<link href='http://example.com/' rel='http://test.com#link-type' type='text/plain' hreflang='de' "
			"title='All About Angle Brackets: &lt;, &gt;' length='2000'/>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_LINK (link));
	g_clear_error (&error);

	/* Check the properties */
	g_assert_cmpstr (gdata_link_get_uri (link), ==, "http://example.com/");
	g_assert_cmpstr (gdata_link_get_relation_type (link), ==, "http://test.com#link-type");
	g_assert_cmpstr (gdata_link_get_content_type (link), ==, "text/plain");
	g_assert_cmpstr (gdata_link_get_language (link), ==, "de");
	g_assert_cmpstr (gdata_link_get_title (link), ==, "All About Angle Brackets: <, >");
	g_assert_cmpint (gdata_link_get_length (link), ==, 2000);

	/* Compare it against another identical link */
	link2 = gdata_link_new ("http://example.com/", "http://test.com#link-type");
	g_assert_cmpint (gdata_link_compare (link, link2), ==, 0);
	gdata_link_set_content_type (link2, "text/plain");
	gdata_link_set_language (link2, "de");
	gdata_link_set_title (link2, "All About Angle Brackets: <, >");
	gdata_link_set_length (link2, 2000);
	g_assert_cmpint (gdata_link_compare (link, link2), ==, 0);

	/* Try with a dissimilar link */
	gdata_link_set_uri (link2, "http://gnome.org/");
	g_assert_cmpint (gdata_link_compare (link, link2), !=, 0);
	g_object_unref (link2);

	/* Check the outputted XML is the same */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (link));
	g_assert_cmpstr (xml, ==,
			 "<link xmlns='http://www.w3.org/2005/Atom' href='http://example.com/' title='All About Angle Brackets: &lt;, &gt;' "
			 	"rel='http://test.com#link-type' type='text/plain' hreflang='de' length='2000'/>");
	g_free (xml);
	g_object_unref (link);

	/* Now parse a link with less information available */
	link = GDATA_LINK (gdata_parsable_new_from_xml (GDATA_TYPE_LINK,
		"<link href='http://shizzle.com'>Test Content<foobar/></link>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_LINK (link));
	g_clear_error (&error);

	/* Check the properties */
	g_assert_cmpstr (gdata_link_get_uri (link), ==, "http://shizzle.com");
	g_assert_cmpstr (gdata_link_get_relation_type (link), ==, "http://www.iana.org/assignments/relation/alternate");
	g_assert (gdata_link_get_content_type (link) == NULL);
	g_assert (gdata_link_get_language (link) == NULL);
	g_assert (gdata_link_get_title (link) == NULL);
	g_assert (gdata_link_get_length (link) == -1);

	/* Check the outputted XML contains the unknown XML */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (link));
	g_assert_cmpstr (xml, ==,
			 "<link xmlns='http://www.w3.org/2005/Atom' href='http://shizzle.com' rel='http://www.iana.org/assignments/relation/alternate'>"
			 	"Test Content<foobar/></link>");
	g_free (xml);
	g_object_unref (link);
}

static void
test_gd_email_address (void)
{
	GDataGDEmailAddress *email, *email2;
	gchar *xml;
	GError *error = NULL;

	email = GDATA_GD_EMAIL_ADDRESS (gdata_parsable_new_from_xml (GDATA_TYPE_GD_EMAIL_ADDRESS,
		"<gd:email xmlns:gd='http://schemas.google.com/g/2005' label='Personal &amp; Private' rel='http://schemas.google.com/g/2005#home' "
			"address='fubar@gmail.com' primary='true'/>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_GD_EMAIL_ADDRESS (email));
	g_clear_error (&error);

	/* Check the properties */
	g_assert_cmpstr (gdata_gd_email_address_get_address (email), ==, "fubar@gmail.com");
	g_assert_cmpstr (gdata_gd_email_address_get_relation_type (email), ==, "http://schemas.google.com/g/2005#home");
	g_assert_cmpstr (gdata_gd_email_address_get_label (email), ==, "Personal & Private");
	g_assert (gdata_gd_email_address_is_primary (email) == TRUE);

	/* Compare it against another identical address */
	email2 = gdata_gd_email_address_new ("fubar@gmail.com", "http://schemas.google.com/g/2005#home", "Personal & Private", TRUE);
	g_assert_cmpint (gdata_gd_email_address_compare (email, email2), ==, 0);

	/* …and a different one */
	gdata_gd_email_address_set_address (email2, "test@example.com");
	g_assert_cmpint (gdata_gd_email_address_compare (email, email2), !=, 0);
	g_object_unref (email2);

	/* Check the outputted XML is the same */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (email));
	g_assert_cmpstr (xml, ==,
			 "<gd:email xmlns='http://www.w3.org/2005/Atom' xmlns:gd='http://schemas.google.com/g/2005' address='fubar@gmail.com' "
			 	"rel='http://schemas.google.com/g/2005#home' label='Personal &amp; Private' primary='true'/>");
	g_free (xml);
	g_object_unref (email);

	/* Now parse an address with less information available */
	email = GDATA_GD_EMAIL_ADDRESS (gdata_parsable_new_from_xml (GDATA_TYPE_GD_EMAIL_ADDRESS,
		"<gd:email xmlns:gd='http://schemas.google.com/g/2005' address='test@example.com'/>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_GD_EMAIL_ADDRESS (email));
	g_clear_error (&error);

	/* Check the properties */
	g_assert_cmpstr (gdata_gd_email_address_get_address (email), ==, "test@example.com");
	g_assert (gdata_gd_email_address_get_relation_type (email) == NULL);
	g_assert (gdata_gd_email_address_get_label (email) == NULL);
	g_assert (gdata_gd_email_address_is_primary (email) == FALSE);

	/* Check the outputted XML contains the unknown XML */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (email));
	g_assert_cmpstr (xml, ==,
			 "<gd:email xmlns='http://www.w3.org/2005/Atom' xmlns:gd='http://schemas.google.com/g/2005' address='test@example.com' "
			 	"primary='false'/>");
	g_free (xml);
	g_object_unref (email);
}

static void
test_gd_im_address (void)
{
	GDataGDIMAddress *im, *im2;
	gchar *xml;
	GError *error = NULL;

	im = GDATA_GD_IM_ADDRESS (gdata_parsable_new_from_xml (GDATA_TYPE_GD_IM_ADDRESS,
		"<gd:im xmlns:gd='http://schemas.google.com/g/2005' protocol='http://schemas.google.com/g/2005#MSN' address='foo@bar.msn.com' "
			"rel='http://schemas.google.com/g/2005#home' primary='true'/>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_GD_IM_ADDRESS (im));
	g_clear_error (&error);

	/* Check the properties */
	g_assert_cmpstr (gdata_gd_im_address_get_address (im), ==, "foo@bar.msn.com");
	g_assert_cmpstr (gdata_gd_im_address_get_protocol (im), ==, "http://schemas.google.com/g/2005#MSN");
	g_assert_cmpstr (gdata_gd_im_address_get_relation_type (im), ==, "http://schemas.google.com/g/2005#home");
	g_assert (gdata_gd_im_address_get_label (im) == NULL);
	g_assert (gdata_gd_im_address_is_primary (im) == TRUE);

	/* Compare it against another identical address */
	im2 = gdata_gd_im_address_new ("foo@bar.msn.com", "http://schemas.google.com/g/2005#MSN", "http://schemas.google.com/g/2005#home", NULL, TRUE);
	g_assert_cmpint (gdata_gd_im_address_compare (im, im2), ==, 0);

	/* …and a different one */
	gdata_gd_im_address_set_protocol (im2, "http://schemas.google.com/g/2005#GOOGLE_TALK");
	g_assert_cmpint (gdata_gd_im_address_compare (im, im2), !=, 0);
	g_object_unref (im2);

	/* Check the outputted XML is the same */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (im));
	g_assert_cmpstr (xml, ==,
			 "<gd:im xmlns='http://www.w3.org/2005/Atom' xmlns:gd='http://schemas.google.com/g/2005' "
			 	"address='foo@bar.msn.com' protocol='http://schemas.google.com/g/2005#MSN' "
				"rel='http://schemas.google.com/g/2005#home' primary='true'/>");
	g_free (xml);
	g_object_unref (im);

	/* Now parse an address with less information available */
	im = GDATA_GD_IM_ADDRESS (gdata_parsable_new_from_xml (GDATA_TYPE_GD_IM_ADDRESS,
		"<gd:im xmlns:gd='http://schemas.google.com/g/2005' label='Other &amp; Miscellaneous' address='foo@baz.example.com'/>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_GD_IM_ADDRESS (im));
	g_clear_error (&error);

	/* Check the properties */
	g_assert_cmpstr (gdata_gd_im_address_get_address (im), ==, "foo@baz.example.com");
	g_assert (gdata_gd_im_address_get_protocol (im) == NULL);
	g_assert (gdata_gd_im_address_get_relation_type (im) == NULL);
	g_assert_cmpstr (gdata_gd_im_address_get_label (im), ==, "Other & Miscellaneous");
	g_assert (gdata_gd_im_address_is_primary (im) == FALSE);

	/* Check the outputted XML contains the unknown XML */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (im));
	g_assert_cmpstr (xml, ==,
			 "<gd:im xmlns='http://www.w3.org/2005/Atom' xmlns:gd='http://schemas.google.com/g/2005' address='foo@baz.example.com' "
			 	"label='Other &amp; Miscellaneous' primary='false'/>");
	g_free (xml);
	g_object_unref (im);
}

static void
test_gd_organization (void)
{
	GDataGDOrganization *org, *org2;
	gchar *xml;
	GError *error = NULL;

	org = GDATA_GD_ORGANIZATION (gdata_parsable_new_from_xml (GDATA_TYPE_GD_ORGANIZATION,
		"<gd:organization xmlns:gd='http://schemas.google.com/g/2005' rel='http://schemas.google.com/g/2005#work' label='Work &amp; Occupation' "
			"primary='true'>"
			"<gd:orgName>Google, Inc.</gd:orgName>"
			"<gd:orgTitle>&lt;Angle Bracketeer&gt;</gd:orgTitle>"
		"</gd:organization>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_GD_ORGANIZATION (org));
	g_clear_error (&error);

	/* Check the properties */
	g_assert_cmpstr (gdata_gd_organization_get_name (org), ==, "Google, Inc.");
	g_assert_cmpstr (gdata_gd_organization_get_title (org), ==, "<Angle Bracketeer>");
	g_assert_cmpstr (gdata_gd_organization_get_relation_type (org), ==, "http://schemas.google.com/g/2005#work");
	g_assert_cmpstr (gdata_gd_organization_get_label (org), ==, "Work & Occupation");
	g_assert (gdata_gd_organization_is_primary (org) == TRUE);

	/* Compare it against another identical organization */
	org2 = gdata_gd_organization_new ("Google, Inc.", "<Angle Bracketeer>", "http://schemas.google.com/g/2005#work", "Work & Occupation", TRUE);
	g_assert_cmpint (gdata_gd_organization_compare (org, org2), ==, 0);

	/* …and a different one */
	gdata_gd_organization_set_title (org2, "Demoted!");
	g_assert_cmpint (gdata_gd_organization_compare (org, org2), !=, 0);
	g_object_unref (org2);

	/* Check the outputted XML is the same */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (org));
	g_assert_cmpstr (xml, ==,
			 "<gd:organization xmlns='http://www.w3.org/2005/Atom' xmlns:gd='http://schemas.google.com/g/2005' "
			 	"rel='http://schemas.google.com/g/2005#work' label='Work &amp; Occupation' primary='true'>"
				"<gd:orgName>Google, Inc.</gd:orgName>"
				"<gd:orgTitle>&lt;Angle Bracketeer&gt;</gd:orgTitle>"
			 "</gd:organization>");
	g_free (xml);
	g_object_unref (org);

	/* Now parse an organization with less information available */
	org = GDATA_GD_ORGANIZATION (gdata_parsable_new_from_xml (GDATA_TYPE_GD_ORGANIZATION,
		"<gd:organization xmlns:gd='http://schemas.google.com/g/2005'/>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_GD_ORGANIZATION (org));
	g_clear_error (&error);

	/* Check the properties */
	g_assert (gdata_gd_organization_get_name (org) == NULL);
	g_assert (gdata_gd_organization_get_title (org) == NULL);
	g_assert (gdata_gd_organization_get_relation_type (org) == NULL);
	g_assert (gdata_gd_organization_get_label (org) == NULL);
	g_assert (gdata_gd_organization_is_primary (org) == FALSE);

	/* Check the outputted XML contains the unknown XML */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (org));
	g_assert_cmpstr (xml, ==,
			 "<gd:organization xmlns='http://www.w3.org/2005/Atom' xmlns:gd='http://schemas.google.com/g/2005' primary='false'/>");
	g_free (xml);
	g_object_unref (org);
}

static void
test_gd_phone_number (void)
{
	GDataGDPhoneNumber *phone, *phone2;
	gchar *xml;
	GError *error = NULL;

	phone = GDATA_GD_PHONE_NUMBER (gdata_parsable_new_from_xml (GDATA_TYPE_GD_PHONE_NUMBER,
		"<gd:phoneNumber xmlns:gd='http://schemas.google.com/g/2005' rel='http://schemas.google.com/g/2005#mobile' "
			"label='Personal &amp; business calls only' uri='tel:+12065551212'>+1 206 555 1212</gd:phoneNumber>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_GD_PHONE_NUMBER (phone));
	g_clear_error (&error);

	/* Check the properties */
	g_assert_cmpstr (gdata_gd_phone_number_get_number (phone), ==, "+1 206 555 1212");
	g_assert_cmpstr (gdata_gd_phone_number_get_uri (phone), ==, "tel:+12065551212");
	g_assert_cmpstr (gdata_gd_phone_number_get_relation_type (phone), ==, "http://schemas.google.com/g/2005#mobile");
	g_assert_cmpstr (gdata_gd_phone_number_get_label (phone), ==, "Personal & business calls only");
	g_assert (gdata_gd_phone_number_is_primary (phone) == FALSE);

	/* Compare it against another identical number */
	phone2 = gdata_gd_phone_number_new ("+1 206 555 1212", "http://schemas.google.com/g/2005#mobile", "Personal & business calls only",
					    "tel:+12065551212", FALSE);
	g_assert_cmpint (gdata_gd_phone_number_compare (phone, phone2), ==, 0);

	/* …and a different one */
	gdata_gd_phone_number_set_number (phone2, "+1 206 555 1212 666");
	g_assert_cmpint (gdata_gd_phone_number_compare (phone, phone2), !=, 0);
	g_object_unref (phone2);

	/* Check the outputted XML is the same */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (phone));
	g_assert_cmpstr (xml, ==,
			 "<gd:phoneNumber xmlns='http://www.w3.org/2005/Atom' xmlns:gd='http://schemas.google.com/g/2005' "
			 	"uri='tel:+12065551212' rel='http://schemas.google.com/g/2005#mobile' label='Personal &amp; business calls only' "
			 	"primary='false'>+1 206 555 1212</gd:phoneNumber>");
	g_free (xml);
	g_object_unref (phone);

	/* Now parse a phone number with less information available, but some extraneous whitespace */
	phone = GDATA_GD_PHONE_NUMBER (gdata_parsable_new_from_xml (GDATA_TYPE_GD_PHONE_NUMBER,
		"<gd:phoneNumber xmlns:gd='http://schemas.google.com/g/2005'>  (425) 555-8080 ext. 72585  \n </gd:phoneNumber>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_GD_PHONE_NUMBER (phone));
	g_clear_error (&error);

	/* Check the properties */
	g_assert_cmpstr (gdata_gd_phone_number_get_number (phone), ==, "(425) 555-8080 ext. 72585");
	g_assert (gdata_gd_phone_number_get_uri (phone) == NULL);
	g_assert (gdata_gd_phone_number_get_relation_type (phone) == NULL);
	g_assert (gdata_gd_phone_number_get_label (phone) == NULL);
	g_assert (gdata_gd_phone_number_is_primary (phone) == FALSE);

	/* Check the outputted XML contains the unknown XML */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (phone));
	g_assert_cmpstr (xml, ==,
			 "<gd:phoneNumber xmlns='http://www.w3.org/2005/Atom' "
			 	"xmlns:gd='http://schemas.google.com/g/2005' primary='false'>(425) 555-8080 ext. 72585</gd:phoneNumber>");
	g_free (xml);
	g_object_unref (phone);
}

static void
test_gd_postal_address (void)
{
	GDataGDPostalAddress *postal, *postal2;
	gchar *xml;
	GError *error = NULL;

	postal = GDATA_GD_POSTAL_ADDRESS (gdata_parsable_new_from_xml (GDATA_TYPE_GD_POSTAL_ADDRESS,
		"<gd:postalAddress xmlns:gd='http://schemas.google.com/g/2005' label='Home &amp; Safe House' "
			"rel='http://schemas.google.com/g/2005#home' primary='true'>"
			" \t 500 West 45th Street\nNew York, NY 10036  \n  "
		"</gd:postalAddress>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_GD_POSTAL_ADDRESS (postal));
	g_clear_error (&error);

	/* Check the properties */
	g_assert_cmpstr (gdata_gd_postal_address_get_address (postal), ==, "500 West 45th Street\nNew York, NY 10036");
	g_assert_cmpstr (gdata_gd_postal_address_get_relation_type (postal), ==, "http://schemas.google.com/g/2005#home");
	g_assert_cmpstr (gdata_gd_postal_address_get_label (postal), ==, "Home & Safe House");
	g_assert (gdata_gd_postal_address_is_primary (postal) == TRUE);

	/* Compare it against another identical address */
	postal2 = gdata_gd_postal_address_new ("500 West 45th Street\nNew York, NY 10036", "http://schemas.google.com/g/2005#home",
					       "Home & Safe House", TRUE);
	g_assert_cmpint (gdata_gd_postal_address_compare (postal, postal2), ==, 0);

	/* …and a different one */
	gdata_gd_postal_address_set_address (postal2, "Atlas Mountains");
	g_assert_cmpint (gdata_gd_postal_address_compare (postal, postal2), !=, 0);
	g_object_unref (postal2);

	/* Check the outputted XML is the same */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (postal));
	g_assert_cmpstr (xml, ==,
			 "<gd:postalAddress xmlns='http://www.w3.org/2005/Atom' xmlns:gd='http://schemas.google.com/g/2005' "
			 	"rel='http://schemas.google.com/g/2005#home' label='Home &amp; Safe House' primary='true'>"
				"500 West 45th Street\nNew York, NY 10036"
			 "</gd:postalAddress>");
	g_free (xml);
	g_object_unref (postal);

	/* Now parse an address with less information available */
	postal = GDATA_GD_POSTAL_ADDRESS (gdata_parsable_new_from_xml (GDATA_TYPE_GD_POSTAL_ADDRESS,
		"<gd:postalAddress xmlns:gd='http://schemas.google.com/g/2005'>f</gd:postalAddress>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_GD_POSTAL_ADDRESS (postal));
	g_clear_error (&error);

	/* Check the properties */
	g_assert_cmpstr (gdata_gd_postal_address_get_address (postal), ==, "f");
	g_assert (gdata_gd_postal_address_get_relation_type (postal) == NULL);
	g_assert (gdata_gd_postal_address_get_label (postal) == NULL);
	g_assert (gdata_gd_postal_address_is_primary (postal) == FALSE);

	/* Check the outputted XML contains the unknown XML */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (postal));
	g_assert_cmpstr (xml, ==,
			 "<gd:postalAddress xmlns='http://www.w3.org/2005/Atom' xmlns:gd='http://schemas.google.com/g/2005' primary='false'>"
				 "f</gd:postalAddress>");
	g_free (xml);
	g_object_unref (postal);
}

static void
test_gd_reminder (void)
{
	GDataGDReminder *reminder, *reminder2;
	gchar *xml;
	GTimeVal time_val;
	GError *error = NULL;

	reminder = GDATA_GD_REMINDER (gdata_parsable_new_from_xml (GDATA_TYPE_GD_REMINDER,
		"<gd:reminder xmlns:gd='http://schemas.google.com/g/2005' days='15'/>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_GD_REMINDER (reminder));
	g_clear_error (&error);

	/* Check the properties */
	g_assert (gdata_gd_reminder_get_method (reminder) == NULL);
	g_assert (gdata_gd_reminder_is_absolute_time (reminder) == FALSE);
	g_assert_cmpint (gdata_gd_reminder_get_relative_time (reminder), ==, 15 * 24 * 60);

	/* Check the outputted XML */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (reminder));
	g_assert_cmpstr (xml, ==,
			 "<gd:reminder xmlns='http://www.w3.org/2005/Atom' xmlns:gd='http://schemas.google.com/g/2005' minutes='21600'/>");
	g_free (xml);
	g_object_unref (reminder);

	/* Try again with a different property */
	reminder = GDATA_GD_REMINDER (gdata_parsable_new_from_xml (GDATA_TYPE_GD_REMINDER,
		"<gd:reminder xmlns:gd='http://schemas.google.com/g/2005' hours='15'/>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_GD_REMINDER (reminder));
	g_clear_error (&error);

	/* Check the properties */
	g_assert (gdata_gd_reminder_get_method (reminder) == NULL);
	g_assert (gdata_gd_reminder_is_absolute_time (reminder) == FALSE);
	g_assert_cmpint (gdata_gd_reminder_get_relative_time (reminder), ==, 15 * 60);

	/* Compare to another reminder */
	reminder2 = gdata_gd_reminder_new (NULL, NULL, 15 * 60);
	g_assert_cmpint (gdata_gd_reminder_compare (reminder, reminder2), ==, 0);
	g_object_unref (reminder2);
	g_object_unref (reminder);

	/* …and another */
	reminder = GDATA_GD_REMINDER (gdata_parsable_new_from_xml (GDATA_TYPE_GD_REMINDER,
		"<gd:reminder xmlns:gd='http://schemas.google.com/g/2005' minutes='15'/>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_GD_REMINDER (reminder));
	g_clear_error (&error);

	/* Check the properties */
	g_assert (gdata_gd_reminder_get_method (reminder) == NULL);
	g_assert (gdata_gd_reminder_is_absolute_time (reminder) == FALSE);
	g_assert_cmpint (gdata_gd_reminder_get_relative_time (reminder), ==, 15);
	g_object_unref (reminder);

	/* Try again with an absolute time and a method */
	reminder = GDATA_GD_REMINDER (gdata_parsable_new_from_xml (GDATA_TYPE_GD_REMINDER,
		"<gd:reminder xmlns:gd='http://schemas.google.com/g/2005' method='alert' absoluteTime='2005-06-06T16:55:00-08:00'/>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_GD_REMINDER (reminder));
	g_clear_error (&error);

	/* Check the properties */
	g_assert_cmpstr (gdata_gd_reminder_get_method (reminder), ==, "alert");
	g_assert (gdata_gd_reminder_is_absolute_time (reminder) == TRUE);
	gdata_gd_reminder_get_absolute_time (reminder, &time_val);
	g_assert_cmpint (time_val.tv_sec, ==, 1118105700);
	g_assert_cmpint (time_val.tv_usec, ==, 0);

	/* Compare to another reminder */
	reminder2 = gdata_gd_reminder_new ("alert", &time_val, -1);
	g_assert_cmpint (gdata_gd_reminder_compare (reminder, reminder2), ==, 0);
	g_object_unref (reminder2);

	/* Check the outputted XML */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (reminder));
	g_assert_cmpstr (xml, ==,
			 "<gd:reminder xmlns='http://www.w3.org/2005/Atom' xmlns:gd='http://schemas.google.com/g/2005' "
			 	"absoluteTime='2005-06-07T00:55:00Z' method='alert'/>");
	g_free (xml);
	g_object_unref (reminder);
}

static void
test_gd_when (void)
{
	GDataGDWhen *when, *when2;
	gchar *xml;
	GList *reminders;
	GTimeVal time_val, time_val2;
	GError *error = NULL;

	when = GDATA_GD_WHEN (gdata_parsable_new_from_xml (GDATA_TYPE_GD_WHEN,
		"<gd:when xmlns:gd='http://schemas.google.com/g/2005' startTime='2005-06-06T17:00:00-08:00' endTime='2005-06-06T18:00:00-08:00'/>",
		-1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_GD_WHEN (when));
	g_clear_error (&error);

	/* Check the properties */
	gdata_gd_when_get_start_time (when, &time_val);
	g_assert_cmpint (time_val.tv_sec, ==, 1118106000);
	g_assert_cmpint (time_val.tv_usec, ==, 0);
	gdata_gd_when_get_end_time (when, &time_val2);
	g_assert_cmpint (time_val2.tv_sec, ==, 1118109600);
	g_assert_cmpint (time_val2.tv_usec, ==, 0);
	g_assert (gdata_gd_when_is_date (when) == FALSE);
	g_assert (gdata_gd_when_get_value_string (when) == NULL);
	g_assert (gdata_gd_when_get_reminders (when) == NULL);

	/* Compare it against another identical time */
	when2 = gdata_gd_when_new (&time_val, &time_val2, FALSE);
	g_assert_cmpint (gdata_gd_when_compare (when, when2), ==, 0);

	/* …and a different one */
	time_val2.tv_usec = 100;
	gdata_gd_when_set_end_time (when2, &time_val2);
	g_assert_cmpint (gdata_gd_when_compare (when, when2), !=, 0);
	g_object_unref (when2);

	/* Check the outputted XML is the same */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (when));
	g_assert_cmpstr (xml, ==,
			 "<gd:when xmlns='http://www.w3.org/2005/Atom' xmlns:gd='http://schemas.google.com/g/2005' startTime='2005-06-07T01:00:00Z' "
			 	"endTime='2005-06-07T02:00:00Z'/>");
	g_free (xml);
	g_object_unref (when);

	/* Now parse a time with different information */
	when = GDATA_GD_WHEN (gdata_parsable_new_from_xml (GDATA_TYPE_GD_WHEN,
		"<gd:when xmlns:gd='http://schemas.google.com/g/2005' startTime='2005-06-06' endTime='2005-06-08' valueString='This weekend'>"
			"<gd:reminder minutes='15'/>"
			"<foobar/>"
		"</gd:when>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_GD_WHEN (when));
	g_clear_error (&error);

	/* Check the properties */
	gdata_gd_when_get_start_time (when, &time_val);
	g_assert_cmpint (time_val.tv_sec, ==, 1118016000);
	g_assert_cmpint (time_val.tv_usec, ==, 0);
	gdata_gd_when_get_end_time (when, &time_val2);
	g_assert_cmpint (time_val2.tv_sec, ==, 1118188800);
	g_assert_cmpint (time_val2.tv_usec, ==, 0);
	g_assert (gdata_gd_when_is_date (when) == TRUE);
	g_assert_cmpstr (gdata_gd_when_get_value_string (when), ==, "This weekend");

	reminders = gdata_gd_when_get_reminders (when);
	g_assert (reminders != NULL);
	g_assert (GDATA_IS_GD_REMINDER (reminders->data));
	g_assert (reminders->next == NULL);
	g_assert (gdata_gd_reminder_is_absolute_time (GDATA_GD_REMINDER (reminders->data)) == FALSE);
	g_assert_cmpint (gdata_gd_reminder_get_relative_time (GDATA_GD_REMINDER (reminders->data)), ==, 15);

	/* Check the outputted XML is correct */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (when));
	g_assert_cmpstr (xml, ==,
			 "<gd:when xmlns='http://www.w3.org/2005/Atom' xmlns:gd='http://schemas.google.com/g/2005' startTime='2005-06-06' "
			 	"endTime='2005-06-08' valueString='This weekend'>"
				"<gd:reminder minutes='15'/>"
				"<foobar/>"
			 "</gd:when>");
	g_free (xml);
	g_object_unref (when);
}

static void
test_gd_where (void)
{
	GDataGDWhere *where, *where2;
	gchar *xml;
	GError *error = NULL;

	where = GDATA_GD_WHERE (gdata_parsable_new_from_xml (GDATA_TYPE_GD_WHERE,
		"<gd:where xmlns:gd='http://schemas.google.com/g/2005' rel='http://schemas.google.com/g/2005#event.alternate' "
			"label='New York Location &lt;videoconference&gt;' valueString='Metropolis'/>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_GD_WHERE (where));
	g_clear_error (&error);

	/* Check the properties */
	g_assert_cmpstr (gdata_gd_where_get_relation_type (where), ==, "http://schemas.google.com/g/2005#event.alternate");
	g_assert_cmpstr (gdata_gd_where_get_value_string (where), ==, "Metropolis");
	g_assert_cmpstr (gdata_gd_where_get_label (where), ==, "New York Location <videoconference>");

	/* Compare it against another identical place */
	where2 = gdata_gd_where_new ("http://schemas.google.com/g/2005#event.alternate", "Metropolis", "New York Location <videoconference>");
	g_assert_cmpint (gdata_gd_where_compare (where, where2), ==, 0);

	/* …and a different one */
	gdata_gd_where_set_label (where2, "Atlas Mountains");
	g_assert_cmpint (gdata_gd_where_compare (where, where2), !=, 0);
	g_object_unref (where2);

	/* Check the outputted XML is the same */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (where));
	g_assert_cmpstr (xml, ==,
			 "<gd:where xmlns='http://www.w3.org/2005/Atom' xmlns:gd='http://schemas.google.com/g/2005' "
			 	"label='New York Location &lt;videoconference&gt;' rel='http://schemas.google.com/g/2005#event.alternate' "
			 	"valueString='Metropolis'/>");
	g_free (xml);
	g_object_unref (where);

	/* Now parse a place with less information available */
	where = GDATA_GD_WHERE (gdata_parsable_new_from_xml (GDATA_TYPE_GD_WHERE,
		"<gd:where xmlns:gd='http://schemas.google.com/g/2005' valueString='Google Cafeteria &lt;Building 40&gt;'/>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_GD_WHERE (where));
	g_clear_error (&error);

	/* Check the properties */
	g_assert_cmpstr (gdata_gd_where_get_value_string (where), ==, "Google Cafeteria <Building 40>");
	g_assert (gdata_gd_where_get_relation_type (where) == NULL);
	g_assert (gdata_gd_where_get_label (where) == NULL);

	/* Check the outputted XML contains the unknown XML */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (where));
	g_assert_cmpstr (xml, ==,
			 "<gd:where xmlns='http://www.w3.org/2005/Atom' xmlns:gd='http://schemas.google.com/g/2005' "
			 	"valueString='Google Cafeteria &lt;Building 40&gt;'/>");
	g_free (xml);
	g_object_unref (where);
}

static void
test_gd_who (void)
{
	GDataGDWho *who, *who2;
	gchar *xml;
	GError *error = NULL;

	who = GDATA_GD_WHO (gdata_parsable_new_from_xml (GDATA_TYPE_GD_WHO,
		"<gd:who xmlns:gd='http://schemas.google.com/g/2005' rel='http://schemas.google.com/g/2005#message.to' valueString='Elizabeth' "
			"email='liz@example.com'/>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_GD_WHO (who));
	g_clear_error (&error);

	/* Check the properties */
	g_assert_cmpstr (gdata_gd_who_get_relation_type (who), ==, "http://schemas.google.com/g/2005#message.to");
	g_assert_cmpstr (gdata_gd_who_get_value_string (who), ==, "Elizabeth");
	g_assert_cmpstr (gdata_gd_who_get_email_address (who), ==, "liz@example.com");

	/* Compare it against another identical person */
	who2 = gdata_gd_who_new ("http://schemas.google.com/g/2005#message.to", "Elizabeth", "liz@example.com");
	g_assert_cmpint (gdata_gd_who_compare (who, who2), ==, 0);

	/* …and a different one */
	gdata_gd_who_set_email_address (who2, "john@example.com");
	g_assert_cmpint (gdata_gd_who_compare (who, who2), !=, 0);
	g_object_unref (who2);

	/* Check the outputted XML is the same */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (who));
	g_assert_cmpstr (xml, ==,
			 "<gd:who xmlns='http://www.w3.org/2005/Atom' xmlns:gd='http://schemas.google.com/g/2005' email='liz@example.com' "
			 	"rel='http://schemas.google.com/g/2005#message.to' valueString='Elizabeth'/>");
	g_free (xml);
	g_object_unref (who);

	/* Now parse a place with less information available */
	who = GDATA_GD_WHO (gdata_parsable_new_from_xml (GDATA_TYPE_GD_WHO,
		"<gd:who xmlns:gd='http://schemas.google.com/g/2005'/>", -1, &error));
	g_assert_no_error (error);
	g_assert (GDATA_IS_GD_WHO (who));
	g_clear_error (&error);

	/* Check the properties */
	g_assert (gdata_gd_who_get_value_string (who) == NULL);
	g_assert (gdata_gd_who_get_relation_type (who) == NULL);
	g_assert (gdata_gd_who_get_email_address (who) == NULL);

	/* Check the outputted XML contains the unknown XML */
	xml = gdata_parsable_get_xml (GDATA_PARSABLE (who));
	g_assert_cmpstr (xml, ==,
			 "<gd:who xmlns='http://www.w3.org/2005/Atom' xmlns:gd='http://schemas.google.com/g/2005'/>");
	g_free (xml);
	g_object_unref (who);
}

int
main (int argc, char *argv[])
{
	g_type_init ();
	g_test_init (&argc, &argv, NULL);
	g_test_bug_base ("http://bugzilla.gnome.org/show_bug.cgi?id=");

	g_test_add_func ("/entry/get_xml", test_entry_get_xml);
	g_test_add_func ("/entry/parse_xml", test_entry_parse_xml);
	g_test_add_func ("/query/categories", test_query_categories);
	g_test_add_func ("/color/parsing", test_color_parsing);
	g_test_add_func ("/color/output", test_color_output);

	g_test_add_func ("/atom/author", test_atom_author);
	g_test_add_func ("/atom/category", test_atom_category);
	g_test_add_func ("/atom/generator", test_atom_generator);
	g_test_add_func ("/atom/link", test_atom_link);

	g_test_add_func ("/gd/email_address", test_gd_email_address);
	g_test_add_func ("/gd/im_address", test_gd_im_address);
	g_test_add_func ("/gd/organization", test_gd_organization);
	g_test_add_func ("/gd/phone_number", test_gd_phone_number);
	g_test_add_func ("/gd/postal_address", test_gd_postal_address);
	g_test_add_func ("/gd/reminder", test_gd_reminder);
	g_test_add_func ("/gd/when", test_gd_when);
	g_test_add_func ("/gd/where", test_gd_where);
	g_test_add_func ("/gd/who", test_gd_who);

	/*g_test_add_data_func ("/media/thumbnail/parse_time", "", test_media_thumbnail_parse_time);
	g_test_add_data_func ("/media/thumbnail/parse_time", "de_DE", test_media_thumbnail_parse_time);*/

	return g_test_run ();
}
