// This is file seg/dbsks/dbsks_biarc_grid.cxx

//:
// \file

#include "dbsks_biarc_grid.h"
#include <vnl/vnl_math.h>

// ============================================================================
// dbsks_biarc_grid
// ============================================================================



//: Compute the grid from a parameter set
bool dbsks_biarc_grid::
compute(const dbsks_biarc_grid_params& p)
{
  // x
  this->step_x_ = p.step_x;
  this->x_.set_size(p.num_x);
  for (int i =0; i < p.num_x; ++i)
  {
    this->x_[i] = p.min_x + i*p.step_x;
  }

  // y
  this->step_y_ = p.step_y;
  this->y_.set_size(p.num_y);
  for (int i =0; i < p.num_y; ++i)
  {
    this->y_[i] = p.min_y + i * p.step_y;
  }

  // dx
  this->step_dx_ = p.step_dx;
  this->dx_.set_size(p.num_dx);
  for (int i =0; i < p.num_dx; ++i)
  {
    this->dx_[i] = p.min_dx + i*p.step_dx;
  }


  // dy
  this->step_dy_ = p.step_dy;
  this->dy_.set_size(p.num_dy);
  for (int i =0; i < p.num_dy; ++i)
  {
    this->dy_[i] = p.min_dy + i*p.step_dy;
  }

  // alpha0
  this->step_alpha0_ = p.step_alpha0;
  this->alpha0_.set_size(p.num_alpha0);
  for (int i =0; i < p.num_alpha0; ++i)
  {
    this->alpha0_[i] = p.min_alpha0 + i*p.step_alpha0;
  }


  // alpha2
  this->step_alpha2_ = p.step_alpha2;
  this->alpha2_.set_size(p.num_alpha2);
  for (int i =0; i < p.num_alpha2; ++i)
  {
    this->alpha2_[i] = p.min_alpha2 + i*p.step_alpha2;
  }

  return true;
}



