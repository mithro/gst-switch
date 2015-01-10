/* GStreamer
 * Copyright (C) 2011 David A. Schleef <ds@schleef.org>
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
/**
 * SECTION:element-gstintervideosink
 *
 * The intervideosink element is a video sink element.  It is used
 * in connection with an intervideosrc element in a different pipeline,
 * similar to interaudiosink and interaudiosrc.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v videotestsrc ! intervideosink
 * ]|
 * 
 * The intervideosink element cannot be used effectively with gst-launch,
 * as it requires a second pipeline in the application to send video to.
 * See the gstintertest.c example in the gst-plugins-bad source code for
 * more details.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/base/gstbasesink.h>
#include <gst/video/video.h>
#include "gstintervideosink.h"

#include <string.h>

GST_DEBUG_CATEGORY_STATIC (gsw_inter_video_sink_debug_category);
#define GST_CAT_DEFAULT gsw_inter_video_sink_debug_category

/* prototypes */
static void gsw_inter_video_sink_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec);
static void gsw_inter_video_sink_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec);
static void gsw_inter_video_sink_finalize (GObject * object);

static void gsw_inter_video_sink_get_times (GstBaseSink * sink,
    GstBuffer * buffer, GstClockTime * start, GstClockTime * end);
static gboolean gsw_inter_video_sink_start (GstBaseSink * sink);
static gboolean gsw_inter_video_sink_stop (GstBaseSink * sink);
static gboolean gsw_inter_video_sink_set_caps (GstBaseSink * sink,
    GstCaps * caps);
static GstFlowReturn gsw_inter_video_sink_render (GstBaseSink * sink,
    GstBuffer * buffer);

enum
{
  PROP_0,
  PROP_CHANNEL
};

/* pad templates */
static GstStaticPadTemplate gsw_inter_video_sink_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE (GST_VIDEO_FORMATS_ALL))
    );


/* class initialization */
G_DEFINE_TYPE (GswInterVideoSink, gsw_inter_video_sink, GST_TYPE_BASE_SINK);

static void
gsw_inter_video_sink_class_init (GswInterVideoSinkClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);
  GstBaseSinkClass *base_sink_class = GST_BASE_SINK_CLASS (klass);

  GST_DEBUG_CATEGORY_INIT (gsw_inter_video_sink_debug_category,
      "intervideosink", 0, "debug category for intervideosink element");

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&gsw_inter_video_sink_sink_template));

  gst_element_class_set_static_metadata (element_class,
      "Internal video sink",
      "Sink/Video",
      "Virtual video sink for internal process communication",
      "David Schleef <ds@schleef.org>");

  gobject_class->set_property = gsw_inter_video_sink_set_property;
  gobject_class->get_property = gsw_inter_video_sink_get_property;
  gobject_class->finalize = gsw_inter_video_sink_finalize;
  base_sink_class->get_times =
      GST_DEBUG_FUNCPTR (gsw_inter_video_sink_get_times);
  base_sink_class->start = GST_DEBUG_FUNCPTR (gsw_inter_video_sink_start);
  base_sink_class->stop = GST_DEBUG_FUNCPTR (gsw_inter_video_sink_stop);
  base_sink_class->render = GST_DEBUG_FUNCPTR (gsw_inter_video_sink_render);
  base_sink_class->set_caps = GST_DEBUG_FUNCPTR (gsw_inter_video_sink_set_caps);

  g_object_class_install_property (gobject_class, PROP_CHANNEL,
      g_param_spec_string ("channel", "Channel",
          "Channel name to match inter src and sink elements",
          "default", G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
gsw_inter_video_sink_init (GswInterVideoSink * intervideosink)
{
  intervideosink->channel = g_strdup ("default");
}

void
gsw_inter_video_sink_set_property (GObject * object, guint property_id,
    const GValue * value, GParamSpec * pspec)
{
  GswInterVideoSink *intervideosink = GST_INTER_VIDEO_SINK (object);

  switch (property_id) {
    case PROP_CHANNEL:
      g_free (intervideosink->channel);
      intervideosink->channel = g_value_dup_string (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gsw_inter_video_sink_get_property (GObject * object, guint property_id,
    GValue * value, GParamSpec * pspec)
{
  GswInterVideoSink *intervideosink = GST_INTER_VIDEO_SINK (object);

  switch (property_id) {
    case PROP_CHANNEL:
      g_value_set_string (value, intervideosink->channel);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gsw_inter_video_sink_finalize (GObject * object)
{
  GswInterVideoSink *intervideosink = GST_INTER_VIDEO_SINK (object);

  /* clean up object here */
  g_free (intervideosink->channel);

  G_OBJECT_CLASS (gsw_inter_video_sink_parent_class)->finalize (object);
}


static void
gsw_inter_video_sink_get_times (GstBaseSink * sink, GstBuffer * buffer,
    GstClockTime * start, GstClockTime * end)
{
  GswInterVideoSink *intervideosink = GST_INTER_VIDEO_SINK (sink);

  if (GST_BUFFER_TIMESTAMP_IS_VALID (buffer)) {
    *start = GST_BUFFER_TIMESTAMP (buffer);
    if (GST_BUFFER_DURATION_IS_VALID (buffer)) {
      *end = *start + GST_BUFFER_DURATION (buffer);
    } else {
      if (intervideosink->info.fps_n > 0) {
        *end = *start +
            gst_util_uint64_scale_int (GST_SECOND, intervideosink->info.fps_d,
            intervideosink->info.fps_n);
      }
    }
  }
}

static gboolean
gsw_inter_video_sink_start (GstBaseSink * sink)
{
  GswInterVideoSink *intervideosink = GST_INTER_VIDEO_SINK (sink);

  intervideosink->surface = gsw_inter_surface_get (intervideosink->channel);
  g_mutex_lock (&intervideosink->surface->mutex);
  memset (&intervideosink->surface->video_info, 0, sizeof (GstVideoInfo));
  g_mutex_unlock (&intervideosink->surface->mutex);

  return TRUE;
}

static gboolean
gsw_inter_video_sink_stop (GstBaseSink * sink)
{
  GswInterVideoSink *intervideosink = GST_INTER_VIDEO_SINK (sink);

  g_mutex_lock (&intervideosink->surface->mutex);
  if (intervideosink->surface->video_buffer) {
    gst_buffer_unref (intervideosink->surface->video_buffer);
  }
  intervideosink->surface->video_buffer = NULL;
  memset (&intervideosink->surface->video_info, 0, sizeof (GstVideoInfo));
  g_mutex_unlock (&intervideosink->surface->mutex);

  gsw_inter_surface_unref (intervideosink->surface);
  intervideosink->surface = NULL;

  return TRUE;
}

static gboolean
gsw_inter_video_sink_set_caps (GstBaseSink * sink, GstCaps * caps)
{
  GswInterVideoSink *intervideosink = GST_INTER_VIDEO_SINK (sink);
  GstVideoInfo info;

  if (!gst_video_info_from_caps (&info, caps)) {
    GST_ERROR_OBJECT (sink, "Failed to parse caps %" GST_PTR_FORMAT, caps);
    return FALSE;
  }

  g_mutex_lock (&intervideosink->surface->mutex);
  intervideosink->surface->video_info = info;
  intervideosink->info = info;
  g_mutex_unlock (&intervideosink->surface->mutex);

  return TRUE;
}

static GstFlowReturn
gsw_inter_video_sink_render (GstBaseSink * sink, GstBuffer * buffer)
{
  GswInterVideoSink *intervideosink = GST_INTER_VIDEO_SINK (sink);

  GST_DEBUG_OBJECT (intervideosink, "render ts %" GST_TIME_FORMAT,
      GST_TIME_ARGS (GST_BUFFER_PTS (buffer)));

  g_mutex_lock (&intervideosink->surface->mutex);
  if (intervideosink->surface->video_buffer) {
    gst_buffer_unref (intervideosink->surface->video_buffer);
  }
  intervideosink->surface->video_buffer = gst_buffer_ref (buffer);
  intervideosink->surface->video_buffer_count = 0;
  g_mutex_unlock (&intervideosink->surface->mutex);

  return GST_FLOW_OK;
}
