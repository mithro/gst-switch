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
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _GST_INTER_VIDEO_SINK_H_
#define _GST_INTER_VIDEO_SINK_H_

#include <gst/base/gstbasesink.h>
#include "gstintersurface.h"

G_BEGIN_DECLS

#define GSW_TYPE_INTER_VIDEO_SINK   (gsw_inter_video_sink_get_type())
#define GST_INTER_VIDEO_SINK(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj),GSW_TYPE_INTER_VIDEO_SINK,GswInterVideoSink))
#define GST_INTER_VIDEO_SINK_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST((klass),GSW_TYPE_INTER_VIDEO_SINK,GswInterVideoSinkClass))
#define GST_IS_INTER_VIDEO_SINK(obj)   (G_TYPE_CHECK_INSTANCE_TYPE((obj),GSW_TYPE_INTER_VIDEO_SINK))
#define GST_IS_INTER_VIDEO_SINK_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE((klass),GSW_TYPE_INTER_VIDEO_SINK))

typedef struct _GswInterVideoSink GswInterVideoSink;
typedef struct _GswInterVideoSinkClass GswInterVideoSinkClass;

struct _GswInterVideoSink
{
  GstBaseSink base_intervideosink;

  GswInterSurface *surface;
  char *channel;

  GstVideoInfo info;
};

struct _GswInterVideoSinkClass
{
  GstBaseSinkClass base_intervideosink_class;
};

GType gsw_inter_video_sink_get_type (void);

G_END_DECLS

#endif
