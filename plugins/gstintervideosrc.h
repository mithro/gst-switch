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

#ifndef _GST_INTER_VIDEO_SRC_H_
#define _GST_INTER_VIDEO_SRC_H_

#include <gst/base/gstbasesrc.h>
#include <gst/video/video.h>
#include "gstintersurface.h"

G_BEGIN_DECLS

#define GSW_TYPE_INTER_VIDEO_SRC   (gsw_inter_video_src_get_type())
#define GST_INTER_VIDEO_SRC(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj),GSW_TYPE_INTER_VIDEO_SRC,GswInterVideoSrc))
#define GST_INTER_VIDEO_SRC_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST((klass),GSW_TYPE_INTER_VIDEO_SRC,GswInterVideoSrcClass))
#define GST_IS_INTER_VIDEO_SRC(obj)   (G_TYPE_CHECK_INSTANCE_TYPE((obj),GSW_TYPE_INTER_VIDEO_SRC))
#define GST_IS_INTER_VIDEO_SRC_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE((klass),GSW_TYPE_INTER_VIDEO_SRC))

typedef struct _GswInterVideoSrc GswInterVideoSrc;
typedef struct _GswInterVideoSrcClass GswInterVideoSrcClass;

struct _GswInterVideoSrc
{
  GstBaseSrc base_intervideosrc;

  GswInterSurface *surface;

  char *channel;

  GstVideoInfo info;
  GstBuffer *black_frame;
  int n_frames;
  GstClockTime timestamp_offset;
};

struct _GswInterVideoSrcClass
{
  GstBaseSrcClass base_intervideosrc_class;
};

GType gsw_inter_video_src_get_type (void);

G_END_DECLS

#endif
