// This is file shp/dbsksp/algo/dbsksp_shock_path.cxx

//:
// \file

#include "dbsksp_shock_path.h"


#include <vgl/vgl_distance.h>

//==============================================================================
// dbsksp_shock_path
//==============================================================================





//------------------------------------------------------------------------------
//: Compute cache data for fast access to the path's properties
void dbsksp_shock_path::
compute_cache_data()
{
  // mid-line
  this->mid_line_.resize(this->num_points_);
  for (unsigned i =0; i < this->num_points_; ++i)
  {
    this->mid_line_[i] = this->xdesc_[i].bnd_mid_pt();
  }

  // length along mid-line
  this->mid_line_lengths_.resize(this->num_points_);
  this->mid_line_lengths_[0] = 0;
  for (unsigned i =1; i < this->num_points_; ++i)
  {
    this->mid_line_lengths_[i] = this->mid_line_lengths_[i-1] + 
                                vgl_distance(this->mid_line_[i-1], this->mid_line_[i]);
  }

  // boundary length


  return;

}






