#ifndef _baml_prob_img_h_
#define _baml_prob_img_h_

//:
// \file
// \brief An image with a mog at each pixel.
// \author Thomas Pollard
// \date 10/22/07


#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vil/vil_image_view.h>

#include "baml_mog.h"


class baml_prob_img {

public:

  baml_prob_img();
  baml_prob_img( unsigned ni, unsigned nj );
  ~baml_prob_img(){ delete[] d_; }

  void initialize( unsigned ni, unsigned nj );

  float prob(
    const vil_image_view<float>& img );

  // Calculate optimal a and b such that a*img+b is most probable.
  void normalize_img(
    const vil_image_view<float>& img,
    float& a, float& b );

  // Get a mog for the given index, must be deleted.
  baml_mog* operator()( unsigned i, unsigned j );

  void write( vcl_string file_name );
  void read( vcl_string file_name );

  void draw_best( vcl_string img_file );
  void write_pixel(
    int i, int j,
    vcl_string file );

protected:


  float* d_;
  unsigned ni_, nj_, mog_size_, num_mixtures_;
  unsigned d_size(){ return nj_*ni_*mog_size_; }

  baml_prob_img( baml_prob_img& img ){}

  static vcl_ofstream norm_record; // TEMP HACK
};

#endif // _baml_prob_img_h_
