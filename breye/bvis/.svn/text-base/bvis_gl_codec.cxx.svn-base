// This is breye/bvis/bvis_gl_codec.cxx
#include "bvis_gl_codec.h"
//:
// \file

#include <vcl_cassert.h>
#include <vcl_iostream.h>

#include <vul/vul_sprintf.h>

#include <vidl1/vidl1_codec.h>
#include <vidl1/vidl1_movie.h>
#include <vidl1/vidl1_frame.h>
#include <vil/vil_image_view_base.h>

#include <bvis/bvis_video_manager.h>
#include <vidpro/vidpro_repository.h>
#include <vgui/vgui_utils.h>
#include <vil/vil_new.h>
#include <vil/vil_image_resource.h>
#include <vgui/vgui_gl.h>



//: Basic constructor. 
bvis_gl_codec::bvis_gl_codec(int start_frame, int end_frame)
 : start_(start_frame), end_(end_frame)
{
  int num_frames = bvis_video_manager::instance()->repository()->num_frames();
  if(start_<1)
    start_ = 1;
  if(end_ > num_frames)
    end_ = num_frames;
  init();
}

// Destructor
bvis_gl_codec::~bvis_gl_codec()
{
}

//: Initialize
bool bvis_gl_codec::init()
{

  set_number_frames(end_ - start_ + 1);
  
  // get viewport size
  GLint vp[4]; // x,y,w,h
  glGetIntegerv(GL_VIEWPORT, vp);
  unsigned w = vp[2];
  unsigned h = vp[3];

  set_bits_pixel(sizeof(GLubyte) * 8);
  set_width(w);
  set_height(h);

  return true;
}


//: Get a section of pixels in function of the frame number, position and size.
vil_image_view_base_sptr bvis_gl_codec::get_view(int position, int x0, int w, int y0, int h) const
{
  bvis_video_manager::instance()->repository()->go_to_frame( start_ + position - 1);
  bvis_video_manager::instance()->display_current_frame();
  vil_image_view<vxl_byte> buf = vgui_utils::colour_buffer_to_view();
  vil_image_resource_sptr res = vil_new_image_resource_of_view(buf); 
  return res->get_view(x0, w, y0, h);
}


//: Put a section of pixels in function of the frame number, position and size.
bool bvis_gl_codec::put_view(int /*position*/, const vil_image_view_base & /*im*/, int /*x0*/, int /*y0*/)
{
  vcl_cerr << "bvis_gl_codec::put_section not implemented\n";
  return false;
}


//: Load from a file name.
vidl1_codec_sptr bvis_gl_codec::load(vcl_string const& fname, char mode)
{
  return NULL;
}


//: Supposed to check the validity of this codec for a special filename.
// Not so well implemented for this codec.
bool bvis_gl_codec::probe(vcl_string const& fname)
{
  return false;
}


//: Save the given video as a set of images of the default set type.
bool bvis_gl_codec::save(vidl1_movie* movie, vcl_string const& fname)
{
  return false;
}


