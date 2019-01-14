// This is brl/bmvl/brct/brct_volume_processor_params.cxx
#include "brct_volume_processor_params.h"
//:
// \file
// See brct_volume_processor_params.h
//
//-----------------------------------------------------------------------------
#include <sstream>
#include <iostream>

//------------------------------------------------------------------------
// Constructors
//

brct_volume_processor_params::
brct_volume_processor_params(brct_volume_processor_params const& vp)
  : gevd_param_mixin()
{
  InitParams(vp.xmin_,
             vp.xmax_,
             vp.ymin_,
             vp.ymax_,
             vp.zmin_,
             vp.zmax_,
             vp.cube_edge_length_,
             vp.cell_thresh_,
             vp.dummy2_,
             vp.dummy3_
            );
}

brct_volume_processor_params::
brct_volume_processor_params(const float xmin,
                             const float xmax,
                             const float ymin,
                             const float ymax,
                             const float zmin,
                             const float zmax,
                             const int cube_edge_length,
                             const float cell_thresh,
                             const float dummy2,
                             const float dummy3)
{
  InitParams(xmin, xmax, ymin, ymax, zmin, zmax, cube_edge_length,
             cell_thresh,  dummy2,  dummy3);
}

void brct_volume_processor_params::InitParams(float xmin,
                                              float xmax,
                                              float ymin,
                                              float ymax,
                                              float zmin,
                                              float zmax,
                                              int cube_edge_length,
                                              float cell_thresh,
                                              float dummy2,
                                              float dummy3)
{
  xmin_ = xmin;
  xmax_ = xmax;
  ymin_ = ymin;
  ymax_ = ymax;
  zmin_ = zmin;
  zmax_ = zmax;
  cube_edge_length_ = cube_edge_length;
  cell_thresh_= cell_thresh;
  dummy2_= dummy2;
  dummy3_= dummy3;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    std::endl otherwise.
bool brct_volume_processor_params::SanityCheck()
{
  std::stringstream msg;
  bool valid = true;

  msg << std::ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

std::ostream& operator<< (std::ostream& os, const brct_volume_processor_params& vp)
{
  return
  os << "brct_volume_processor_params:\n[---\n"
     << "xmin " << vp.xmin_ << std::endl
     << "xmax " << vp.xmax_ << std::endl
     << "ymin " << vp.ymin_ << std::endl
     << "ymax " << vp.ymax_ << std::endl
     << "zmin " << vp.zmin_ << std::endl
     << "zmax " << vp.zmax_ << std::endl
     << "cube_edge_length " << vp.cube_edge_length_ << std::endl
     << "cell thresh " << vp.cell_thresh_ << std::endl
     << "---]" << std::endl;
}
