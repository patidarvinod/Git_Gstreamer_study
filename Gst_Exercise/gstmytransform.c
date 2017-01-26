/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2016 root <<user@hostname.org>>
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
 * SECTION:element-mytransform
 *
 * FIXME:Describe mytransform here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! mytransform ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>

#include "gstmytransform.h"

GST_DEBUG_CATEGORY_STATIC (gst_my_transform_debug);
#define GST_CAT_DEFAULT gst_my_transform_debug


/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("testcaps, "
		     "format =GST_FORMAT_BYTES,"
		     "width = (int){3, 4} ,"
		     "height = (int){4, 5}"
		    )
    );
//"format = (string){int, float} ,"

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("testcaps, "
		     "format = GST_FORMAT_BYTES ,"
		     "width = (int){3, 4} ,"
		     "height = (int){4, 5}"
		    )
    );

#define gst_my_transform_parent_class parent_class
G_DEFINE_TYPE (GstMyTransform, gst_my_transform, GST_TYPE_ELEMENT);

static gboolean gst_my_transform_sink_event (GstPad * pad, GstObject * parent, GstEvent * event);
static GstFlowReturn gst_my_transform_chain (GstPad * pad, GstObject * parent, GstBuffer * buf);
static gboolean gst_mytransform_activate_mode (GstPad * sinkpad, GstObject * parent, GstPadMode mode, gboolean active);
static gboolean gst_mytransform_activate (GstPad * sinkpad, GstObject * parent);
static GstFlowReturn gst_mytransform_loop (GstPad * pad);
static GstStateChangeReturn gst_mytransform_change_state (GstElement * element,GstStateChange transition);


static GstStateChangeReturn
gst_mytransform_change_state (GstElement * element,GstStateChange transition)
{
	GstStateChangeReturn ret;

  switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
        
      break;
    case GST_STATE_CHANGE_READY_TO_PAUSED:
           break;
    case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
      break;
    default:
      break;
  }

  ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);

  switch (transition) {
    case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
      break;
    case GST_STATE_CHANGE_PAUSED_TO_READY:
      break;
    case GST_STATE_CHANGE_READY_TO_NULL:
      break;
    default:
      break;
  }

  return ret;

	
}

/* GObject vmethod implementations */

/* initialize the mytransform's class */
static void
gst_my_transform_class_init (GstMyTransformClass * klass)
{
  GstElementClass *gstelement_class;
  gstelement_class = (GstElementClass *) klass;

  gst_element_class_set_details_simple(gstelement_class,
    "MyTransform",
    "Basic Transform Plugin",
    "Performs a basic transform",
    "Danish Rauf");

  gstelement_class->change_state = GST_DEBUG_FUNCPTR (gst_mytransform_change_state);
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&src_factory));
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&sink_factory));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_my_transform_init (GstMyTransform * filter)
{
  filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");

  filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");

  gst_pad_set_event_function (filter->sinkpad, GST_DEBUG_FUNCPTR(gst_my_transform_sink_event));
  gst_pad_set_chain_function (filter->sinkpad, GST_DEBUG_FUNCPTR(gst_my_transform_chain));
  gst_pad_set_activatemode_function (filter->sinkpad, gst_mytransform_activate_mode);
  gst_pad_set_activate_function (filter->sinkpad, gst_mytransform_activate);
  gst_element_add_pad (GST_ELEMENT_CAST (filter), filter->sinkpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);
  
}

/* GstElement vmethod implementations */

/* this function handles sink events */
static gboolean
gst_my_transform_sink_event (GstPad * pad, GstObject * parent, GstEvent * event)
{
  gboolean ret;
  GstMyTransform *filter;

  filter = GST_MYTRANSFORM (parent);
  GST_DEBUG_OBJECT(pad,"\n\n\nsink event in mytransform\n\n\n");

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_CAPS:
    {
      GstCaps * caps;

      gst_event_parse_caps (event, &caps);
      /* do something with the caps */

      /* and forward */
      ret = gst_pad_event_default (pad, parent, event);
      break;
    }
    case GST_EVENT_EOS:
    GST_DEBUG_OBJECT(pad,"\n\n  EOS...Event is generated \n\n");
            ret = gst_pad_push_event (pad, event);
      break;
    default:
      ret = gst_pad_event_default (pad, parent, event);
      break;
  }
  return ret;
}

/* chain function
 * this function does the actual processing
 */
static GstFlowReturn
gst_my_transform_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
  GstMyTransform *filter;

  filter = GST_MYTRANSFORM (parent);

  /* just push out the incoming buffer without touching it */
  return gst_pad_push (filter->srcpad, buf);
}

static gboolean
gst_mytransform_activate (GstPad * sinkpad, GstObject * parent)
{
	gboolean ret;
       GST_DEBUG("\n\n######Before gst_mytransform_activate #############\n\n");
	ret = gst_pad_activate_mode(sinkpad, GST_PAD_MODE_PULL, TRUE);
       GST_DEBUG("\n\n######After gst_mytransform_activate ############# ret=%d\n\n",ret);
	return ret;
}

static gboolean
gst_mytransform_activate_mode (GstPad * sinkpad, GstObject * parent,
							   GstPadMode mode, gboolean active)
{
     GST_DEBUG("\n\n######gst_mytransform_activate_mode #############\n\n");
  gboolean res;
  if (active) {
           GST_DEBUG("\n\n######gst_mytransform_activate_mode start task #############\n\n");
    res = gst_pad_start_task (sinkpad, (GstTaskFunction) gst_mytransform_loop, sinkpad, NULL);
           GST_DEBUG("\n\n######gst_mytransform_activate_mode started task #############\n\n");
  } else {
	res = gst_pad_stop_task (sinkpad);
  }
  return res;
}

static GstFlowReturn
gst_mytransform_loop (GstPad * pad)
{
  GstFlowReturn ret = GST_FLOW_OK;
  GstMyTransform *mytransform;
  GstBuffer *buf = NULL;
  GstMapInfo minfo;
  guint size;
  guint8 i=0;
  mytransform = GST_MYTRANSFORM (gst_pad_get_parent (pad));

  GST_DEBUG("\n\n######gst_mytransform_loop enter loop  #############\n\n");
  ret = gst_pad_pull_range (mytransform->sinkpad, 0, 4, &buf);
  
  if(ret == GST_FLOW_OK) {
	  gst_buffer_map (buf, &minfo, GST_MAP_READ);
      size = minfo.size;

      gst_buffer_unmap (buf, &minfo);
      GST_DEBUG("\n\n###### *minfo.data =%u  minfo.data = %u #############\n\n",*minfo.data,minfo.data); 
      printf("\n\n###### *minfo.data =%u  minfo.data = %u #############\n\n",*minfo.data,minfo.data); 
      gst_pad_push(mytransform->srcpad, buf);
      gst_buffer_unref(buf);
    
    return ret;
  } else if (ret == GST_FLOW_EOS){
	  printf("\n\nEntered EOS...");
	  GST_DEBUG("\n\nEntered EOS...");
	  
	  gst_pad_pause_task (mytransform->sinkpad);
	  	  
	 // gst_element_post_message(GST_ELEMENT_CAST (mytransform),gst_message_new_segment_done ((GstObject*) (mytransform),GST_FORMAT_TIME, -1));
     // gst_pad_push_event (mytransform->srcpad,gst_event_new_segment_done (GST_FORMAT_TIME, -1));



	 gst_pad_push_event (mytransform->srcpad, gst_event_new_eos ());	    

	return ret;
  }
  else{
	    return ret;
	  }
  
}

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean
mytransform_init (GstPlugin * mytransform)
{
  /* debug category for fltering log messages
   *
   * exchange the string 'Template mytransform' with your description
   */
  GST_DEBUG_CATEGORY_INIT (gst_my_transform_debug, "mytransform",
      0, "Template mytransform");

  return gst_element_register (mytransform, "mytransform", GST_RANK_NONE,
      GST_TYPE_MYTRANSFORM);
}

/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstmytransform"
#endif

/* gstreamer looks for this structure to register mytransforms
 *
 * exchange the string 'Template mytransform' with your mytransform description
 */
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    mytransform,
    "Template mytransform",
    mytransform_init,
    VERSION,
    "LGPL",
    "GStreamer",
    "http://gstreamer.net/"
)
