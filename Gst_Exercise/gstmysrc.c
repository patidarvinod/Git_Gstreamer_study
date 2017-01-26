/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2016 Prashant Ghode <<user@hostname.org>>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
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
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-mysrc
 *
 * FIXME:Describe mysrc here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! mysrc ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>

#include "gstmysrc.h"

GST_DEBUG_CATEGORY_STATIC (gst_mysrc_debug);
#define GST_CAT_DEFAULT gst_mysrc_debug

gint8 Inputbuffer[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 1, 0};

guint8 *data,j=0;


/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("testcaps, "
		     "format = GST_FORMAT_BYTES ,"
		     "width = (int)3 ,"
		     "height = (int)4"
		    )
    );

#define gst_mysrc_parent_class parent_class
G_DEFINE_TYPE (GstMysrc, gst_mysrc, GST_TYPE_BASE_SRC);

static GstFlowReturn gst_mysrc_fill (GstBaseSrc * src, guint64 offset,guint length, GstBuffer * buf);

static gboolean gst_mysrc_is_seekable (GstBaseSrc * src);

static gboolean
gst_mysrc_is_seekable (GstBaseSrc * basesrc)
{
  GstMysrc *src = GST_MYSRC (basesrc);

  return src->seekable;
}

static GstFlowReturn gst_mysrc_fill (GstBaseSrc * src, guint64 offset,guint length, GstBuffer * buf){
  GstMapInfo info;
  
  guint to_read,i,bytes_read=0;
  data = &Inputbuffer[0];

  GST_DEBUG("\n\n######gst_mysrc_fill enter fill src  #############\n\n");
  gst_buffer_map (buf, &info, GST_MAP_WRITE);

  to_read = length / 4;
  GST_DEBUG_OBJECT(src,"\n\n######to_read =%u   #############\n\n",to_read);

  for(i=0; i<to_read && j<12; i++)
  {
	  
		if(*(data + j)==0){
				GST_DEBUG("\n\n######info.data EOS reached =%u  #############\n\n",*info.data);   
				gst_buffer_unmap (buf, &info);
				return GST_FLOW_EOS;
		}
		
    *(info.data) = *(data + j);
    j++;
    GST_DEBUG_OBJECT(src,"\n\n######*info.data =%u   #############\n\n",*info.data);
  }
#if 0
   if(*(info.data)==0){
	GST_DEBUG("\n\n######minfo.data =%u  #############\n\n",*info.data);   
	  gst_buffer_unmap (buf, &info);
   return GST_FLOW_EOS;
}
#endif
  gst_buffer_unmap (buf, &info);
 
  return GST_FLOW_OK;
}

/* GObject vmethod implementations */

/* initialize the mysrc's class */
static void
gst_mysrc_class_init (GstMysrcClass * klass)
{
  GstElementClass *gstelement_class;
  GstBaseSrcClass *gstbasesrc_class;

  gstelement_class = (GstElementClass *) klass;
  gstbasesrc_class = (GstBaseSrcClass *)(klass);

//gobject_class->finalize = GST_DEBUG_FUNCPTR (gst_mysrc_finalize);

  gst_element_class_set_details_simple(gstelement_class,
    "Mysrc",
    "Source",
    "Read from buffer",
    "Prashant Ghode <<user@hostname.org>>");

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&src_factory));

  gstbasesrc_class->fill = GST_DEBUG_FUNCPTR (gst_mysrc_fill);
  gstbasesrc_class->is_seekable = GST_DEBUG_FUNCPTR (gst_mysrc_is_seekable);
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_mysrc_init (GstMysrc * filter)
{
  filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
  gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);
  filter->seekable = TRUE;
}


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean
mysrc_init (GstPlugin * mysrc)
{
  /* debug category for fltering log messages
   *
   * exchange the string 'Template mysrc' with your description
   */
  GST_DEBUG_CATEGORY_INIT (gst_mysrc_debug, "mysrc",
      0, "Template mysrc");

  return gst_element_register (mysrc, "mysrc", GST_RANK_NONE,
      GST_TYPE_MYSRC);
}
#if 0
static void gst_mysrc_finalize(GObject * object)
{
   GST_DEBUG_OBJECT (sink, "Finalizing the sorce."); 
   G_OBJECT_CLASS (parent_class)->finalize (object);
}
#endif
/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstmysrc"
#endif

/* gstreamer looks for this structure to register mysrcs
 *
 * exchange the string 'Template mysrc' with your mysrc description
 */
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    mysrc,
    "Template mysrc",
    mysrc_init,
    VERSION,
    "LGPL",
    "GStreamer",
    "http://gstreamer.net/"
)
