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
 * SECTION:element-mysink
 *
 * FIXME:Describe mysink here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! mysink ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>

#include "gstmysink.h"

GST_DEBUG_CATEGORY_STATIC (gst_my_sink_debug);
#define GST_CAT_DEFAULT gst_my_sink_debug

guint8 outputBuffer[12]={0};

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("testcaps, "
		     "format = GST_FORMAT_BYTES ,"
		     "width = (int)3 ,"
		     "height = (int)4"
		    )
    );

#define gst_my_sink_parent_class parent_class
G_DEFINE_TYPE (GstMySink, gst_my_sink, GST_TYPE_BASE_SINK);

static gboolean gst_my_sink_sink_event (GstBaseSink * bsink, GstEvent * event);
//static GstFlowReturn gst_my_sink_chain (GstPad * pad, GstObject * parent, GstBuffer * buf);
static GstFlowReturn gst_my_sink_preroll (GstBaseSink * bsink, GstBuffer * buffer);
static GstFlowReturn gst_my_sink_render (GstBaseSink * bsink,GstBuffer * buffer);
/* initialize the mysink's class */
static void
gst_my_sink_class_init (GstMySinkClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  GstBaseSinkClass *gstbasesink_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;
  gstbasesink_class = (GstBaseSinkClass *) klass;
  gstbasesink_class->preroll = GST_DEBUG_FUNCPTR (gst_my_sink_preroll);
  gstbasesink_class->render = GST_DEBUG_FUNCPTR (gst_my_sink_render);
 // gstbasesink_class->event = GST_DEBUG_FUNCPTR (gst_my_sink_sink_event);
  gst_element_class_set_details_simple(gstelement_class,
    "MySink",
    "Sink Plugin",
    "dump the data into array",
    "Prashant Ghode <<user@hostname.org>>");

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&sink_factory));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_my_sink_init (GstMySink * filter)
{
  filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");

  //gst_pad_set_event_function (filter->sinkpad,
    //                          GST_DEBUG_FUNCPTR(gst_my_sink_sink_event));
//  gst_pad_set_chain_function (filter->sinkpad,
  //                            GST_DEBUG_FUNCPTR(gst_my_sink_chain));
  gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);
}

/* this function handles sink events */
//static gboolean
//gst_my_sink_sink_event (GstPad * pad, GstObject * parent, GstEvent * event)
static gboolean
gst_my_sink_sink_event (GstBaseSink * bsink, GstEvent * event)

{
  gboolean ret;
  GstMySink *mysink;
    mysink = GST_MYSINK (bsink);


  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_CAPS:
    {
      GstCaps * caps;

      gst_event_parse_caps (event, &caps);
      /* do something with the caps */

      /* and forward */
      ret = gst_pad_event_default (bsink->sinkpad, (GstObject*)bsink, event);
      break;
    }
    case GST_EVENT_EOS:
    {
#if 0		
		GstMessage *message;
      guint32 seqnum;

    GST_DEBUG_OBJECT(bsink,"\n\n  EOS...Event is generated \n\n");
            //ret = gst_pad_push_event (pad, event);
      GST_DEBUG_OBJECT (bsink, "Now posting EOS");

      seqnum = gst_event_get_seqnum (event);
      GST_DEBUG_OBJECT (bsink, "Got seqnum #%" ,G_GUINT32_FORMAT, seqnum);

      message = gst_message_new_eos ((GstObject*)bsink);
      gst_message_set_seqnum (message, seqnum);
      gst_element_post_message (GST_ELEMENT_CAST (bsink), message);
#endif      
      break;        

    }
    case GST_EVENT_SEGMENT_DONE:
     {
		 ret = gst_pad_event_default (mysink->sinkpad, (GstObject*)bsink, gst_event_new_eos ());
#if 0		
		GstMessage *message;
      guint32 seqnum;

    GST_DEBUG_OBJECT(bsink,"\n\n  SEGMENT_DONE is generated \n\n");
            //ret = gst_pad_push_event (pad, event);
      GST_DEBUG_OBJECT (bsink, "Now posting SEGMENT_DONE");

      seqnum = gst_event_get_seqnum (event);
      GST_DEBUG_OBJECT (bsink, "Got seqnum #%" ,G_GUINT32_FORMAT, seqnum);

      message = gst_message_new_eos ((GstObject*)bsink);
      gst_message_set_seqnum (message, seqnum);
      gst_element_post_message (GST_ELEMENT_CAST (bsink), message);
#endif      
      break;        

    }  
      default:
      ret = gst_pad_event_default (bsink->sinkpad, (GstObject*)bsink, event);
      break;
  }
  return ret;
}
static GstFlowReturn
gst_my_sink_preroll(GstBaseSink * bsink, GstBuffer * buffer) {
	GST_DEBUG_OBJECT (bsink, "preroll buffer %p", buffer);
//	gst_my_sink_render(bsink,buffer);
}
/* chain function	
 * this function does the actual processing
 */
static GstFlowReturn
gst_my_sink_render (GstBaseSink * bsink, GstBuffer * buf)
{
  GstFlowReturn ret = GST_FLOW_OK;
  guint8 *data = outputBuffer;
  guint size, i,length;
  static int j=0;
  GstMapInfo minfo;
  GST_DEBUG("\n\n######gst_my_sink_chain enter sink chain  #############\n\n");
  gst_buffer_map (buf, &minfo, GST_MAP_READ);
  size = minfo.size;
  //gst_buffer_extract (buf, 0, data, size);
  printf("\nminfo.data in sink= %u     minfo.data= %u\n",*minfo.data,minfo.data);
  GST_DEBUG("\nminfo.data in sink= %u\n",*minfo.data);
  GST_DEBUG("\n\n######gst_my_sink_chain extracted buffer  #############\n\n");
  length=size/4;
  gst_buffer_unmap (buf, &minfo);
  //for(i=0; i<length; i++)
  {
	  //printf("\noutputBuffer[%d] = %d", j, (guint)outputBuffer[j]);
	  //j++;
	  
  }
  	
  return ret;
}


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean
mysink_init (GstPlugin * mysink)
{
  /* debug category for fltering log messages
   *
   * exchange the string 'Template mysink' with your description
   */
  GST_DEBUG_CATEGORY_INIT (gst_my_sink_debug, "mysink", 0, "Template mysink");

  return gst_element_register (mysink, "mysink", GST_RANK_NONE, GST_TYPE_MYSINK);
}


/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstmysink"
#endif

/* gstreamer looks for this structure to register mysinks
 *
 * exchange the string 'Template mysink' with your mysink description
 */
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    mysink,
    "Template mysink",
    mysink_init,
    VERSION,
    "LGPL",
    "GStreamer",
    "http://gstreamer.net/"
)
