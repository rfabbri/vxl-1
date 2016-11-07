// This is contrib/biotree/biocts/biocts_ctsim.h

#ifndef biocts_ctsim_h
#define biocts_ctsim_h

//:
// \file
// \brief
//
// \author
// Rahul is the author.
// Can added the comments.
//
// \date
// 02/01/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <vil/vil_image_view.h>
#include <vcl_cmath.h>
#include <vcl_vector.h>
#include "biocts_Volume3D.h"

#ifndef PI
#define PI  3.1415926535897932384626433832795
#endif

class biocts_ctsim
{
public:
  int lambda;            //
  int Backgrnd_Noise;    //
  int gain;

  int xdim, ydim, zdim, Dia;
  int NViews;                //number of views

  biocts_Volume3D* orig_vol;       // 
  biocts_Volume3D* rot_vol;       //

  vcl_vector<vil_image_view<vxl_byte> > screens;

  biocts_ctsim(int _lambda, int _backgrnd_noise, int _gain, int _NViews, biocts_Volume3D* volume);
  ~biocts_ctsim();

};

#endif
