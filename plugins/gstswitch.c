/* GStreamer
 * Copyright (C) 2012 Duzy Chan <code@duzy.info>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Suite 500,
 * Boston, MA 02110-1335, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstswitch.h"
#include "../logutils.h"

GST_DEBUG_CATEGORY_STATIC (gst_switch_debug);
#define GST_CAT_DEFAULT gst_switch_debug

#define GST_SWITCH_LOCK(obj) (g_mutex_lock (&(obj)->lock))
#define GST_SWITCH_UNLOCK(obj) (g_mutex_unlock (&(obj)->lock))

/**
 *  The Case Element has to be a N-to-1 fitter, and must always have a static
 *  "src" pad, e.g. funnel.
 *
 *  TODO: make it configurable, e.g. case=funnel.
 */
#define CASE_ELEMENT_NAME "funnel"

static GstStaticPadTemplate gst_switch_sink_factory =
  GST_STATIC_PAD_TEMPLATE ("sink_%u",
      GST_PAD_SINK,
      GST_PAD_REQUEST,
      GST_STATIC_CAPS_ANY);

static GstStaticPadTemplate gst_switch_src_factory =
  GST_STATIC_PAD_TEMPLATE ("src_%u",
      GST_PAD_SRC,
      GST_PAD_REQUEST,
      GST_STATIC_CAPS_ANY);

enum
{
  PROP_0,
  PROP_CASES,
};

enum
{
  GST_SWITCH_PAD_FLAG_GHOSTED	= (GST_PAD_FLAG_LAST << 1),
  GST_SWITCH_PAD_FLAG_LAST	= (GST_PAD_FLAG_LAST << 2)
};

G_DEFINE_TYPE (GstSwitch, gst_switch, GST_TYPE_BIN);

static void 
gst_switch_init (GstSwitch *swit)
{
  /*
  GstElement * default_case;
  GstPad * basepad, * pad;
  */

  swit->cases_string = NULL;

  g_mutex_init (&swit->lock);

  /*
  default_case = gst_element_factory_make (CASE_ELEMENT_NAME,
      "default");

  if (!gst_bin_add (GST_BIN (swit), default_case))
    goto error_bin_add_default_case;

  basepad = gst_element_get_static_pad (default_case, "src");
  pad = gst_ghost_pad_new_from_template ("default", basepad,
      gst_static_pad_template_get (&gst_switch_case_factory));

  //gst_pad_set_active (pad, TRUE);

  if (!gst_element_add_pad (GST_ELEMENT (swit), pad))
    goto error_add_default_pad;

  GST_OBJECT_FLAG_SET (basepad, GST_SWITCH_PAD_FLAG_GHOSTED);
  return;

 error_bin_add_default_case:
  {
    GST_ERROR_OBJECT (swit, "Failed to add default case");
    return;
  }

 error_add_default_pad:
  {
    GST_ERROR_OBJECT (swit, "Failed to add default pad");
    return;
  }
  */
}

static void
gst_switch_set_property (GstSwitch *swit, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  switch (prop_id) {
  case PROP_CASES:
    g_free (swit->cases_string);
    swit->cases_string = g_strdup (g_value_get_string (value));
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (swit), prop_id, pspec);
    break;
  }
}

static void
gst_switch_get_property (GstSwitch *swit, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  switch (prop_id) {
  case PROP_CASES:
    g_value_set_string (value, swit->cases_string);
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (swit), prop_id, pspec);
    break;
  }
}

static void
gst_switch_finalize (GstSwitch *swit)
{
  if (swit->cases_string) {
    g_free (swit->cases_string);
    swit->cases_string = NULL;
  }

  g_mutex_clear (&swit->lock);

  G_OBJECT_CLASS (swit)->finalize (G_OBJECT (swit));
}

static GstElement *
gst_switch_request_new_case (GstSwitch * swit, GstPadTemplate * templ,
    const GstCaps * caps)
{
  GList *item = GST_BIN_CHILDREN (GST_BIN (swit));
  gchar * name = GST_PAD_TEMPLATE_NAME_TEMPLATE (templ);
  GstElement * swcase = item ? GST_ELEMENT (item->data) : NULL;
  gint num;

  for (num = 0; item; item = g_list_next (item)) ++num;

  name = g_strdup_printf ("case_%d", num);
  swcase = gst_element_factory_make (CASE_ELEMENT_NAME, name);
  g_free(name);

  if (!gst_bin_add (GST_BIN (swit), swcase))
    goto error_bin_add_case;

  return swcase;

 error_bin_add_case:
  {
    GST_ERROR_OBJECT (swit, "Bin add failed for %s.%s",
	GST_ELEMENT_NAME (swit), GST_PAD_TEMPLATE_NAME_TEMPLATE (templ));
    if (swcase)
      gst_object_unref (GST_OBJECT (swcase));
    return NULL;
  }
}

static GstPad *
gst_switch_request_new_pad (GstElement * element,
    GstPadTemplate * templ, const gchar * unused, const GstCaps * caps)
{
  GstSwitch * swit = GST_SWITCH (element);
  GList *item = GST_BIN_CHILDREN (GST_BIN (swit));
  GstPadDirection dir = GST_PAD_TEMPLATE_DIRECTION (templ);
  GstPad * pad = NULL, * basepad = NULL;
  gchar * name, * dirname;
  GstElement * swcase;
  gint num;

  GST_SWITCH_LOCK (swit);

  switch (dir) {
  case GST_PAD_SRC:
    dirname = "src";
    //swcase = gst_switch_request_new_case (swit, templ, caps);
    INFO ("requesting source\n");
    break;
  case GST_PAD_SINK:
    dirname = "sink";
    /*
    if (item) {
      swcase = GST_ELEMENT (item->data);
    } else {
      swcase = gst_switch_request_new_case (swit, templ, caps);
    }
    */
    break;
  default:
    dirname = NULL;
    swcase = NULL;
    break;
  }

  if (item) {
    swcase = GST_ELEMENT (item->data);
  } else {
    swcase = gst_switch_request_new_case (swit, templ, caps);
    INFO ("new case %s for %s.%s\n", GST_ELEMENT_NAME (swcase),
	GST_ELEMENT_NAME (swit), GST_PAD_TEMPLATE_NAME_TEMPLATE (templ));
  }

  /*
  INFO ("requesting: %s.%s (%s) on %s\n",
      GST_ELEMENT_NAME (element),
      GST_PAD_TEMPLATE_NAME_TEMPLATE (templ),
      dirname,
      swcase ? GST_ELEMENT_NAME (swcase) : "(null)");
  */

  if (!swcase)
    goto error_no_case;

  switch (dir) {
  case GST_PAD_SRC:
    basepad = gst_element_get_static_pad (swcase, "src");
    break;
  case GST_PAD_SINK:
    for (item = GST_ELEMENT_PADS (swcase), num = 0;
	 item; item = g_list_next (item)) {
      if (gst_pad_get_direction (GST_PAD (item->data)) == dir)
	++num;
    }
    name = g_strdup_printf ("sink_%u", num);
    basepad = gst_element_request_pad (swcase, templ, name, caps);
    g_free (name);
    break;
  default:
    basepad = NULL;
    break;
  }

  if (!basepad)
    goto error_no_basepad;

  if (gst_pad_is_linked (basepad))
    goto error_basepad_already_linked;

  for (item = GST_ELEMENT_PADS (swit), num = 0;
       item; item = g_list_next (item)) {
    if (gst_pad_get_direction (GST_PAD (item->data)) == dir)
      ++num;
  }

  name = g_strdup_printf ("%s_%u", dirname, num);
  pad = gst_ghost_pad_new (name, basepad);
  g_free (name);

  if (pad) {
    gst_pad_set_active (pad, TRUE);
    if (gst_element_add_pad (GST_ELEMENT (swit), pad)) {
      GST_OBJECT_FLAG_SET (basepad, GST_SWITCH_PAD_FLAG_GHOSTED);
      GST_DEBUG_OBJECT (swit, "New pad %s.%s (%d) on %s.%s",
	  GST_ELEMENT_NAME (swit), GST_PAD_NAME (pad), GST_PAD_IS_SRC (pad),
	  GST_ELEMENT_NAME (swcase), GST_PAD_NAME (basepad));
      INFO ("requested %s.%s on %s.%s\n",
	  GST_ELEMENT_NAME (swit), GST_PAD_NAME (pad),
	  GST_ELEMENT_NAME (swcase), GST_PAD_NAME (basepad));
    }
  }

  GST_SWITCH_UNLOCK (swit);

  gst_object_unref (basepad);
  return pad;

 error_no_case:
  {
    GST_SWITCH_UNLOCK (swit);
    GST_ERROR_OBJECT (swit, "Failed to request new case for %s.%s",
	GST_ELEMENT_NAME (swit), GST_PAD_TEMPLATE_NAME_TEMPLATE (templ));
    return NULL;
  }

 error_no_basepad:
  {
    GST_SWITCH_UNLOCK (swit);
    GST_ERROR_OBJECT (swit, "Failed to request new pad on %s",
	GST_ELEMENT_NAME (swcase));
    return NULL;
  }

 error_basepad_already_linked:
  {
    GST_SWITCH_UNLOCK (swit);
    GstPad *pp = GST_PAD_PEER (basepad);
    GstElement *ppp = GST_PAD_PARENT (pp);
    gst_object_unref (basepad);
    GST_ERROR_OBJECT (swit, "Pad %s.%s already linked with %s.%s",
	GST_ELEMENT_NAME (swcase), GST_PAD_NAME (basepad),
	GST_ELEMENT_NAME (ppp), GST_PAD_NAME (pp));
    return NULL;
  }

 error_no_pad:
  {
    GST_SWITCH_UNLOCK (swit);
    gst_object_unref (basepad);
    GST_ERROR_OBJECT (swit, "Failed to request new pad");
    return NULL;
  }
}

static void
gst_switch_release_pad (GstElement * element, GstPad * pad)
{
  GstSwitch * swit = GST_SWITCH (element);
  GST_SWITCH_LOCK (swit);
  gst_pad_set_active (pad, FALSE);
  gst_element_remove_pad (element, pad);
  GST_SWITCH_UNLOCK (swit);
}

static void
gst_switch_class_init (GstSwitchClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

  object_class->set_property = (GObjectSetPropertyFunc) gst_switch_set_property;
  object_class->get_property = (GObjectGetPropertyFunc) gst_switch_get_property;
  object_class->finalize = (GObjectFinalizeFunc) gst_switch_finalize;

  g_object_class_install_property (object_class, PROP_CASES,
      g_param_spec_string ("cases", "Switch Cases",
          "The cases for switching", "",
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  gst_element_class_set_static_metadata (element_class,
      "Stream Switch", "Element",
      "Switch within streams",
      "Duzy Chan <code@duzy.info>");

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&gst_switch_sink_factory));

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&gst_switch_src_factory));

  element_class->request_new_pad = gst_switch_request_new_pad;
  element_class->release_pad = gst_switch_release_pad;

  GST_DEBUG_CATEGORY_INIT (gst_switch_debug, "switch", 0, "Switch");
}
