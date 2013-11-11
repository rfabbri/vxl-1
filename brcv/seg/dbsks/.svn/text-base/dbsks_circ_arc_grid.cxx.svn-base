// This is file seg/dbsks/dbsks_circ_arc_grid.cxx

//:
// \file

#include "dbsks_circ_arc_grid.h"
#include <vnl/vnl_math.h>

// ============================================================================
// dbsks_circ_arc_grid
// ============================================================================



//dbsks_circ_arc_grid::
//dbsks_circ_arc_grid(const dbsks_circ_arc_grid_params& params)
//{
//  // x
//  this->min_x_ = params.center_x_ - params.step_x_ * params.half_num_x_;
//  this->max_x_ = params.center_x_ + params.step_x_ * params.half_num_x_;;
//  this->num_x_ = 2* params.half_num_x_ + 1;
//
//  // y
//  this-> min_y_ = params.center_y_ - params.step_y_ * params.half_num_y_;
//  this-> max_y_ = params.center_y_ + params.step_y_ * params.half_num_y_;
//  this-> num_y_ = 2* params.half_num_y_ + 1;
//
//  // theta
//  this->min_theta_ = 0;
//  this->num_theta_ = 16;
//  this->max_theta_ = this->min_theta_+(this->num_theta_-1)*(vnl_math::pi*2/this->num_theta_);
//
//  // chord length (width of the bounding box)
//  this->min_chord_ = 32;
//  this->max_chord_ = 160;
//  this->num_chord_ = 17; //33;
//
//  // height of the bounding box of the arc
//  this->min_height_ = -64;
//  this->max_height_ = 64;
//  this->num_height_  = 17; //33;
//}


//: Set grid parameters
void dbsks_circ_arc_grid::
set_grid_params()
{
  // x
  this->min_x_ = 100; //x0 - 100;
  this-> max_x_ = 400; //x0 + 100;
  this-> num_x_ = 76; // 151;

  // y
  this-> min_y_ = 100; // y0 - 100;
  this-> max_y_ = 400; //y0 + 100;
  this-> num_y_ = 76; //151;

  // theta
  this->min_theta_ = 0;
  this->num_theta_ = 16;
  this->max_theta_ = this->min_theta_+(this->num_theta_-1)*(vnl_math::pi*2/this->num_theta_);

  // chord length (width of the bounding box)
  this->min_chord_ = 32;
  this->max_chord_ = 160;
  this->num_chord_ = 17; //33;

  // height of the bounding box of the arc
  this->min_height_ = -64;
  this->max_height_ = 64;
  this->num_height_  = 17; //33;

  this->compute_grid();
  
  return;
}




// Set grid parameters from a user-input parameter set
void dbsks_circ_arc_grid::
set_grid(const dbsks_circ_arc_grid_params& params)
{

  // x
  this->min_x_ = params.center_x - params.half_num_x * params.step_x;
  this->max_x_ = params.center_x + params.half_num_x * params.step_x;
  this-> num_x_ = 2 * params.half_num_x + 1;

  // y
  this-> min_y_ = params.center_y - params.half_num_y * params.step_y;
  this-> max_y_ = params.center_y + params.half_num_y * params.step_y;
  this-> num_y_ = 2 * params.half_num_y + 1;

  // theta
  this->min_theta_ = 0;
  this->num_theta_ = params.num_theta;
  this->step_theta_ = vnl_math::pi*2/this->num_theta_;
  this->max_theta_ = this->min_theta_+(this->num_theta_-1)*this->step_theta_;

  // chord length (width of the bounding box)
  this->min_chord_ = params.min_chord;
  this->max_chord_ = params.min_chord + (params.num_chord-1) * params.step_chord;
  this->num_chord_ = params.num_chord;

  // height of the bounding box of the arc
  this->min_height_ = -params.half_num_height * params.step_height;
  this->max_height_ = params.half_num_height * params.step_height;
  this->num_height_  = 2*params.half_num_height + 1;

  this->compute_grid();
}






// ----------------------------------------------------------------------------
//: Compute step for each dimension given min, max, and number of values
void dbsks_circ_arc_grid::
compute_grid()
{
  // x and y are distributed linearly
  // x
  this->step_x_ = (this->max_x_-this->min_x_) / (this->num_x_-1);
  this->x_.set_size(this->num_x_);
  for (int i=0; i<this->num_x_; ++i)
  {
    this->x_[i] = this->min_x_ + i*this->step_x_;
  }

  // y
  this->step_y_ = (this->max_y_-this->min_y_) / (this->num_y_-1);
  this->y_.set_size(this->num_y_);
  for (int i=0; i<this->num_y_; ++i)
  {
    this->y_[i] = this->min_y_ + i*this->step_y_;
  }

  // theta - the range is 2 pi
  this->step_theta_ = vnl_math::pi*2 / this->num_theta_;
  this->theta_.set_size(this->num_theta_);
  for (int i=0; i<this->num_theta_; ++i)
  {
    this->theta_[i] = this->min_theta_ + i*this->step_theta_;
  }

  // chord length
  this->step_chord_ = (this->max_chord_-this->min_chord_) / (this->num_chord_-1);
  this->chord_.set_size(this->num_chord_);
  for (int i=0; i<this->num_chord_; ++i)
  {
    this->chord_[i] = this->min_chord_ + i*this->step_chord_;
  }

  // height
  this->step_height_ = (this->max_height_-this->min_height_) / (this->num_height_-1);
  this->height_.set_size(this->num_height_);
  for (int i=0; i<this->num_height_; ++i)
  {
    this->height_[i] = this->min_height_ + i*this->step_height_;
  }

  return;  
}


// ----------------------------------------------------------------------------
//: Total number of points in the grid
int dbsks_circ_arc_grid::
size() const
{
  int vol = this->num_x_ * this->num_y_ * this->num_theta_ * 
    this->num_chord_ * this->num_height_;
  return vol;
}






//: Total number of planes in the grid (size() / (num_x * num_y))
int dbsks_circ_arc_grid::
num_planes() const
{
  return (this->num_theta_ * this->num_chord_ * this->num_height_);
}








// ----------------------------------------------------------------------------
// convert array (linear) index to grid index (multiple params)
// all exclude x andy y
void dbsks_circ_arc_grid::
linear_to_grid(int i_plane, int& i_chord, int& i_height, int& i_theta) const
{
  i_theta = i_plane % this->num_theta_;
  i_plane /= this->num_theta_;

  i_height = i_plane % this->num_height_;
  i_plane /= this->num_height_;

  i_chord = i_plane % this->num_chord_;
  i_plane /= this->num_chord_;

  assert(i_plane == 0);
  return;
}


// ----------------------------------------------------------------------------
// convert grid index to linear index
// all exclude x and y
int dbsks_circ_arc_grid::
grid_to_linear(int i_chord, int i_height, int i_theta) const
{
  // compute the linear index state from the grid-index state
  int i_plane = i_theta + this->num_theta_*(
    i_height + this->num_height_*( i_chord ));
  return i_plane;
}



// ----------------------------------------------------------------------------
//: Return (approximate) index given parameter value
int dbsks_circ_arc_grid::
i_x(double x) const
{
  return vnl_math_rnd((x-this->min_x_) / this->step_x_);
}


// ----------------------------------------------------------------------------
int dbsks_circ_arc_grid::
i_y(double y) const
{
  return vnl_math_rnd((y-this->min_y_) / this->step_y_);
}


// ----------------------------------------------------------------------------
int dbsks_circ_arc_grid::
i_theta(double theta) const
{
  double diff = theta - this->min_theta_;

  // convert this to [0, 2pi] range
  diff = vcl_fmod(diff, 2*vnl_math::pi);
  diff = (diff < 0) ? (diff + 2*vnl_math::pi) : diff;
  
  int ind = vnl_math_rnd(diff / this->step_theta_);
  return (ind < this->num_theta_) ? ind : 0;
}


// ----------------------------------------------------------------------------
int dbsks_circ_arc_grid::
i_chord(double chord) const
{
  return vnl_math_rnd((chord-this->min_chord_) / this->step_chord_);;
}


// ----------------------------------------------------------------------------
int dbsks_circ_arc_grid::
i_height(double height) const
{
  return vnl_math_rnd((height-this->min_height_) / this->step_height_);;
}




// ----------------------------------------------------------------------------
//: Find grid indices of the arc closest to a given circular arc
void dbsks_circ_arc_grid::
arc_to_grid(const dbgl_circ_arc& arc, int& i_x, int& i_y,
            int& i_chord, int& i_height, int& i_theta) const
{
  // indices of left arc
  vgl_point_2d<double > pt = arc.point_at(0.5);
  vgl_vector_2d<double > tangent = arc.chord_dir();
  double theta = vcl_atan2(tangent.y(), tangent.x());
  double chord = arc.chord_len();
  double height = arc.height() * vnl_math_sgn0(arc.k());

  i_x = this->i_x(pt.x());
  i_y = this->i_y(pt.y());
  i_chord = this->i_chord(chord);
  i_height = this->i_height(height);
  i_theta = this->i_theta(theta);
}





// ----------------------------------------------------------------------------
//: Print grid information
void dbsks_circ_arc_grid::
print(vcl_ostream& str) const
{
  str << "Circular arc grid info: \n"
    << "  num_x = " << this->num_x_ << "\n"
    << "  num_y = " << this->num_y_ << "\n"
    << "  num_theta = " << this->num_theta_ << "\n"
    << "  num_chord = " << this->num_chord_ << "\n"
    << "  num_height = " << this->num_height_ << "\n"
    << "  Total number of arcs = " << this->size() << "\n";


  str << "Individual vectors: \n"
    << "  [x] " << this->min_x_ 
    << " : " << this->step_x_ 
    << " : " << this->max_x_ << "\n"

    << "  [y] " << this->min_y_
    << " : " << this->step_y_
    << " : " << this->max_y_ << "\n" 

    << "  [theta] " << this->min_theta_
    << " : " << this->step_theta_
    << " : " << this->max_theta_ << "\n"

    << "  [chord] " << this->min_chord_
    << " : " << this->step_chord_
    << " : " << this->max_chord_ << "\n"

    << "  [height] " << this->min_height_
    << " : " << this->step_height_
    << " : " << this->max_height_ << "\n";

  ////
  //  << "  x: " << this->x_ << "\n"
  //  << "  y: " << this->y_ << "\n" 
  //  << "  theta: " << this->theta_ << "\n"
  //  << "  chord: " << this->chord_ << "\n"
  //  << "  height: " << this->height_ << "\n";
}


