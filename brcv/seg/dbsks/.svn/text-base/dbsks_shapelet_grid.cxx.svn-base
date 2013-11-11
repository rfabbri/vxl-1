// This is file seg/dbsks/dbsks_shapelet_grid.cxx

//:
// \file

#include "dbsks_shapelet_grid.h"
#include <vnl/vnl_math.h>
#include <dbsksp/dbsksp_shapelet.h>




// ============================================================================
// struct dbsks_shapelet_grid_params
// ============================================================================






// ============================================================================
// dbsks_shapelet_grid
// ============================================================================


dbsks_shapelet_grid::
dbsks_shapelet_grid(const dbsksp_shapelet_sptr& s0) : 
has_front_arc_(false), has_rear_arc_(false)
{
  // x
  this->min_xA_ = 200; 
  this->max_xA_ = 400;
  this->num_xA_ = 51;

  // y
  this->min_yA_ = 200;
  this->max_yA_ = 400;
  this->num_yA_ = 51;

  // theta
  this->min_psiA_ = 0;
  this->num_psiA_ = 16;
  this->max_psiA_ = this->min_psiA_+(this->num_psiA_-1)*(vnl_math::pi*2/this->num_psiA_);

  // phiA
  this->min_phiA_ = vnl_math::pi_over_4;
  this->max_phiA_ = 3*vnl_math::pi_over_4;
  this->num_phiA_ = 5;

  // phiB
  this->min_phiB_ = vnl_math::pi_over_4;
  this->max_phiB_ = 3*vnl_math::pi_over_4;
  this->num_phiB_ = 5;

  // m
  this->max_m_ = 0.2;
  this->min_m_ = -0.2;
  this->num_m_ = 3;

  // rA
  this->ref_rA_ = s0->radius_start();
  this->min_log2_rA_ = -0.5;
  this->max_log2_rA_ = 0.5;
  this->num_rA_ = 3;

  // len
  this->ref_len_ = s0->chord_length();
  this->min_log2_len_ = -0.3;
  this->max_log2_len_ = 0.3;
  this->num_len_ = 3;

  this->compute_grid();

  return;
}








// ----------------------------------------------------------------------------
//: Set parameters of the grid
void dbsks_shapelet_grid::
set_grid(const dbsksp_shapelet_sptr& sref, bool has_front_arc, 
         bool has_rear_arc,
         const dbsks_shapelet_grid_params& params)
{
  this->has_front_arc_ = has_front_arc;
  this->has_rear_arc_ = has_rear_arc;


  // x
  this->min_xA_ = params.center_x - params.half_num_x * params.step_x; 
  this->max_xA_ = params.center_x + params.half_num_x * params.step_x;
  this->num_xA_ = 2*params.half_num_x + 1;

  // y
  this->min_yA_ = params.center_y - params.half_num_y * params.step_y;
  this->max_yA_ = params.center_y + params.half_num_y * params.step_y;
  this->num_yA_ = 2 * params.half_num_y + 1;

  // theta
  this->min_psiA_ = 0;
  this->num_psiA_ = params.num_psi;
  this->step_psiA_ = vnl_math::pi*2/this->num_psiA_;
  this->max_psiA_ = this->min_psiA_+ (this->num_psiA_-1)* this->step_psiA_;

  // phiA
  float center_phiA = float(vnl_math::pi_over_2);
  if (! params.force_phi_centered_at_pi_over_2)
  {
    center_phiA = float(sref->phi_start());
  }
  this->min_phiA_ = center_phiA - params.half_range_phiA;
  this->max_phiA_ = center_phiA + params.half_range_phiA;
  this->num_phiA_ = 2* params.half_num_phiA + 1;

  // phiB
  float center_phiB = float(vnl_math::pi_over_2);
  if (! params.force_phi_centered_at_pi_over_2)
  {
    center_phiB = float(sref->phi_end());
  }


  this->min_phiB_ = center_phiB - params.half_range_phiB;
  this->max_phiB_ = center_phiB + params.half_range_phiB;
  this->num_phiB_ = 2*params.half_num_phiB + 1;

  // m
  if (params.half_num_m == 0)
  {
    this->min_m_ = 0;
    this->max_m_ = 0;
  }
  else
  {
    this->min_m_ = -params.max_m; //;-0.3;
    this->max_m_ = params.max_m; //0.3;
  }
  this->num_m_ = 2*params.half_num_m + 1;


  // rA
  this->ref_rA_ = sref->radius_start();
  this->min_log2_rA_ = - params.max_log2_rA; //-0.5;
  this->max_log2_rA_ = params.max_log2_rA; //0.5;
  this->num_rA_ = 2*params.half_num_rA + 1;

  // len
  this->ref_len_ = sref->chord_length();
  this->min_log2_len_ = -params.max_log2_len; //-0.3;
  this->max_log2_len_ = params.max_log2_len; // 0.3;
  this->num_len_ = 2*params.half_num_len + 1;

  this->compute_grid();
  return;
}





// ----------------------------------------------------------------------------
//: Compute step for each dimension given min, max, and number of values
void dbsks_shapelet_grid::
compute_grid()
{
  // x and y are distributed linearly
  // x
  this->step_xA_ = (this->max_xA_-this->min_xA_) / (this->num_xA_-1);
  this->xA_.set_size(this->num_xA_);
  for (int i=0; i<this->num_xA_; ++i)
  {
    this->xA_[i] = this->min_xA_ + i*this->step_xA_;
  }

  // y
  this->step_yA_ = (this->max_yA_-this->min_yA_) / (this->num_yA_-1);
  this->yA_.set_size(this->num_yA_);
  for (int i=0; i<this->num_yA_; ++i)
  {
    this->yA_[i] = this->min_yA_ + i*this->step_yA_;
  }

  // theta - the range is 2 pi
  this->step_psiA_ = vnl_math::pi*2 / this->num_psiA_;
  this->psiA_.set_size(this->num_psiA_);
  for (int i=0; i<this->num_psiA_; ++i)
  {
    this->psiA_[i] = this->min_psiA_ + i*this->step_psiA_;
  }

  // phiA - assume the range is [pi/4 3pi/4]
  this->step_phiA_ = (this->max_phiA_-this->min_phiA_)/(this->num_phiA_-1);
  this->phiA_.set_size(this->num_phiA_);
  for (int i=0; i<this->num_phiA_; ++i)
  {
    this->phiA_[i] = this->min_phiA_ + i*this->step_phiA_;
  }

  // phiB - assume the range is [pi/4 3pi/4]
  this->step_phiB_ = (this->max_phiB_-this->min_phiB_)/(this->num_phiB_-1);
  this->phiB_.set_size(this->num_phiB_);
  for (int i=0; i<this->num_phiB_; ++i)
  {
    this->phiB_[i] = this->min_phiB_ + i*this->step_phiB_;
  }

  // m
  if (this->num_m_ == 0)
  {
    this->step_m_ = 1;
  }
  else
  {
    this->step_m_ = (this->max_m_ - this->min_m_) / (this->num_m_-1);
  }

  this->m_.set_size(this->num_m_);
  for (int i=0; i<this->num_m_; ++i)
  {
    this->m_[i] = this->min_m_ + i*this->step_m_;
  }

  // rA - sampled at log scale
  this->step_log2_rA_ = (this->max_log2_rA_ - this->min_log2_rA_) / (this->num_rA_-1);
  this->rA_.set_size(this->num_rA_);
  for (int i=0; i<this->num_rA_; ++i)
  {
    double log2_ratio = this->min_log2_rA_ + i*this->step_log2_rA_;
    this->rA_[i] = this->ref_rA_ * vcl_exp(vnl_math::ln2 * log2_ratio);
  }


  // len - sampled at log scale
  this->step_log2_len_ = (this->max_log2_len_ - this->min_log2_len_) / (this->num_len_-1);
  this->len_.set_size(this->num_len_);
  for (int i=0; i<this->num_len_; ++i)
  {
    double log2_ratio = this->min_log2_len_ + i*this->step_log2_len_;
    this->len_[i] = this->ref_len_ * vcl_exp(vnl_math::ln2 * log2_ratio);
  }
}


// ----------------------------------------------------------------------------
//: Total number of points in the grid
int dbsks_shapelet_grid::
size() const
{
  int vol = this->num_xA_ * this->num_yA_ * this->num_psiA_ * this->num_len_ * 
    this->num_rA_ * this->num_phiA_ * this->num_phiB_ * this->num_m_;
  return vol;
}



//: Total number of "planes"
int dbsks_shapelet_grid::
num_planes() const
{
  return (this->num_psiA_ * this->num_len_ * 
    this->num_rA_ * this->num_phiA_ * this->num_phiB_ * this->num_m_);
}


// convert array (linear) index to grid index (multiple params)
// all exclude x andy y
void dbsks_shapelet_grid::
linear_to_grid(int i_plane, 
               int& i_phiA, int& i_phiB, int& i_m, int& i_rA, 
               int& i_len, int& i_psiA) const
{
  i_psiA = i_plane % this->num_psiA_;
  i_plane /= this->num_psiA_;

  i_len = i_plane % this->num_len_;
  i_plane /= this->num_len_;

  i_rA = i_plane % this->num_rA_;
  i_plane /= this->num_rA_;

  i_m = i_plane % this->num_m_;
  i_plane /= this->num_m_;

  i_phiB = i_plane % this->num_phiB_;
  i_plane /= this->num_phiB_;

  i_phiA = i_plane % this->num_phiA_;
  i_plane /= this->num_phiA_;
  assert(i_plane == 0);

  return;
}



//: convert grid index to linear index
// all exclude x and y
int dbsks_shapelet_grid::
grid_to_linear(int i_phiA, int i_phiB, int i_m, int i_rA, int i_len, int i_psiA) const
{
  // compute the linear index state from the grid-index state
  int i_array = i_psiA + this->num_psiA_*(
    i_len + this->num_len_*(
    i_rA + this->num_rA_*(
    i_m + this->num_m_*(
    i_phiB + this->num_phiB_*(
    i_phiA)))));
  return i_array;

}






//: convert between plane grid index (x,y)  and linear index (y + x*num_y)
int dbsks_shapelet_grid::
grid_to_linear(int i_xA, int i_yA) const
{
  return (i_yA + this->num_yA_ * i_xA);
}






void dbsks_shapelet_grid::
linear_to_grid(int i_xy, int& i_xA, int& i_yA) const
{
  i_yA = i_xy % this->num_yA_;
  i_xy /= this->num_yA_;

  i_xA = i_xy % this->num_xA_;
  i_xy /= this->num_xA_;

  assert(i_xy == 0);
}



// Properties wrt the starting xnode -----------------------------------------

//: Total number of planes to contain all xnodes
int dbsks_shapelet_grid::
sxnode_num_planes() const
{
  return (this->num_phiA_ * this->num_rA_ * this->num_psiA_);
}



// ----------------------------------------------------------------------------
//: linear index to grid index (for the starting xnode)
void dbsks_shapelet_grid::
sxnode_linear_to_grid(int i_plane, int& i_phiA, int& i_rA, int& i_psiA) const
{
  i_psiA = i_plane % this->num_psiA_;
  i_plane /= this->num_psiA_;

  i_rA = i_plane % this->num_rA_;
  i_plane /= this->num_rA_;

  i_phiA = i_plane % this->num_phiA_;
  i_plane /= this->num_phiA_;
  assert(i_plane == 0);
}


// ----------------------------------------------------------------------------
//: grid index to linear index (for the starting xnode)
int dbsks_shapelet_grid::
sxnode_grid_to_linear(int i_phiA, int i_rA, int i_psiA) const
{
  return (i_psiA + this->num_psiA_*(i_rA + this->num_rA_* i_phiA));
}



// Invididual parameters
// ----------------------------------------------------------------------------
//:
int dbsks_shapelet_grid::
i_xA(double xA) const
{
  int temp = vnl_math_rnd((xA-this->min_xA_) / this->step_xA_);
  //return dbsks_clip(temp, 0, this->xA_.size()-1);
  return temp;
}

//:
int dbsks_shapelet_grid::
i_yA(double yA) const
{
  int temp = vnl_math_rnd((yA-this->min_yA_) / this->step_yA_);
  //return dbsks_clip(temp, 0, this->yA_.size()-1);
  return temp;
}


int dbsks_shapelet_grid::
i_psiA(double psiA) const
{
  double diff = psiA - this->min_psiA_;

  // convert this to [0, 2pi] range
  diff = vcl_fmod(diff, 2*vnl_math::pi);
  diff = (diff < 0) ? (diff + 2*vnl_math::pi) : diff;

  int ind = vnl_math_rnd(diff / this->step_psiA_);
  return (ind < this->num_psiA_) ? ind : 0;
}



int dbsks_shapelet_grid::
i_psiA(const vgl_vector_2d<double >& tangentA) const
{
  double psiA = vcl_atan2(tangentA.y(), tangentA.x());
  return this->i_psiA(psiA);
}


int dbsks_shapelet_grid::
i_m(double m) const
{
  int temp = vnl_math_rnd((m-this->min_m_) / this->step_m_);
  //return dbsks_clip(temp, 0, this->m_.size()-1);
  return temp;
}


//:
int dbsks_shapelet_grid::
i_rA(double rA) const
{
  double dev = vcl_log(rA / this->ref_rA_) / vnl_math::ln2;
  int temp = vnl_math_rnd((dev - this->min_log2_rA_) / this->step_log2_rA_);
  //return dbsks_clip(temp, 0, this->rA_.size()-1);
  return temp;
}

//:
int dbsks_shapelet_grid::
i_len(double len) const
{
  double dev = vcl_log(len / this->ref_len_) / vnl_math::ln2;
  int temp = vnl_math_rnd((dev-this->min_log2_len_) / this->step_log2_len_);
  //return dbsks_clip(temp, 0, this->len_.size()-1);
  return temp;
}


int dbsks_shapelet_grid::
i_phiA(double phiA) const
{
  int temp = vnl_math_rnd((phiA-this->min_phiA_) / this->step_phiA_);
  //return dbsks_clip(temp, 0, this->phiA_.size()-1);
  return temp;
}


int dbsks_shapelet_grid::
i_phiB(double phiB) const
{
  int temp = vnl_math_rnd((phiB-this->min_phiB_) / this->step_phiB_);
  //return dbsks_clip(temp, 0, this->phiB_.size()-1);
  return temp;
}


//: Check if a configuration is legal (within bounds of the allowed range
bool dbsks_shapelet_grid::
is_legal(int i_phiA, int i_phiB, int i_m, int i_rA, int i_len, int i_psiA) const
{
  return (i_phiA >= 0 && i_phiA < this->num_phiA_ &&
    i_phiB >= 0 && i_phiB < this->num_phiB_ &&
    i_m >= 0 && i_m < this->num_m_ &&
    i_rA >= 0 && i_rA < this->num_rA_ &&
    i_len >= 0 && i_len < this->num_len_ &&
    i_psiA >= 0 && i_psiA < this->num_psiA_);
}




//: Check if the position of a shapelet within bounds of the allowed range
bool dbsks_shapelet_grid::
is_legal(int i_xA, int i_yA) const
{
  return (i_xA >= 0 && i_xA < this->num_xA_ &&
    i_yA >= 0 && i_yA < this->num_yA_);
}




// -----------------------------------------------------------------------------
//: Compute grid parameters of the discretized shapelet closest to a given shapelet
void dbsks_shapelet_grid::
approx_shapelet_on_grid(const dbsksp_shapelet_sptr& s, int& i_xy, int& i_plane) const
{
  // Computed indices of the approximated shapelet
  int i_xA = this->i_xA(s->start().x());
  int i_yA = this->i_yA(s->start().y());
  int i_psiA = this->i_psiA(s->tangent_start());
  int i_rA = this->i_rA(s->radius_start());
  int i_phiA = this->i_phiA(s->phi_start());
  int i_len = this->i_len(s->chord_length());
  int i_m = this->i_m(s->m_start());
  int i_phiB = this->i_phiB(s->phi_end());

  i_xy = this->grid_to_linear(i_xA, i_yA);
  i_plane = this->grid_to_linear(i_phiA, i_phiB, i_m, i_rA, i_len, i_psiA);  
}



// -----------------------------------------------------------------------------
//: Compute the discretized shapelet closest to a given shapelet
dbsksp_shapelet_sptr dbsks_shapelet_grid::
approx_shapelet_on_grid(const dbsksp_shapelet_sptr& s) const
{
  int i_xy;
  int i_plane;
  this->approx_shapelet_on_grid(s, i_xy, i_plane);
  
  int i_xA, i_yA, i_phiA, i_phiB, i_m, i_rA, i_len, i_psiA;
  this->linear_to_grid(i_xy, i_xA, i_yA);
  this->linear_to_grid(i_plane, i_phiA, i_phiB, i_m, i_rA, i_len, i_psiA);

  if (!this->is_legal(i_phiA, i_phiB, i_m, i_rA, i_len, i_psiA) ||
      !this->is_legal(i_xA, i_yA))
      return 0;

  return this->shapelet(i_xy, i_plane);
}



//: Return a shapelet given its indices
dbsksp_shapelet_sptr dbsks_shapelet_grid::
shapelet(int i_xy, int i_plane) const
{
  int i_xA, i_yA, i_psiA, i_m, i_rA, i_len, i_phiA, i_phiB;
  this->linear_to_grid(i_xy, i_xA, i_yA);
  this->linear_to_grid(i_plane, i_phiA, i_phiB, i_m, i_rA, i_len, i_psiA);

  double xA = this->xA_[i_xA];
  double yA = this->yA_[i_yA];
  double psiA = this->psiA_[i_psiA];
  double m = this->m_[i_m];
  double rA = this->rA_[i_rA];
  double len = this->len_[i_len];
  double phiA = this->phiA_[i_phiA];
  double phiB = this->phiB_[i_phiB];

  vgl_point_2d<double > ptA(xA, yA);
  vgl_vector_2d<double > tA(vcl_cos(psiA), vcl_sin(psiA));

  dbsksp_shapelet_sptr s0 = new dbsksp_shapelet;
  s0->set_from(ptA, rA, tA, phiA, m, len, phiB);
  return s0;
}




//: Translate grid
void dbsks_shapelet_grid::
translate_grid_center_to(double center_x, double center_y)
{
  double dx = center_x - (this->max_xA_ + this->min_xA_)/2;
  this->max_xA_ += dx;
  this->min_xA_ += dx;
  for (unsigned i =0; i < this->xA_.size(); ++i)
  {
    this->xA_[i] += dx;
  }
  
  double dy = center_y - (this->max_yA_ + this->min_yA_)/2;
  this->min_yA_ += dy;
  this->max_yA_ += dy;
  for (unsigned i =0; i < this->yA_.size(); ++i)
  {
    this->yA_[i] += dy;
  }
  return;
}


// ----------------------------------------------------------------------------
//: Change number of data points for xA
void dbsks_shapelet_grid::
change_num_xA(int num_xA)
{
  this->num_xA_ = num_xA;

  // update other parameters, keep step_xA unchanged
  double center_x = (this->min_xA_ + this->max_xA_) / 2;

  double half_num_xA = double(num_xA-1) / 2;
  this->min_xA_ = center_x - half_num_xA * this->step_xA_;
  this->max_xA_ = center_x + half_num_xA * this->step_xA_;
  for (int i=0; i<this->num_xA_; ++i)
  {
    this->xA_[i] = this->min_xA_ + i*this->step_xA_;
  }
  return;
}


// ----------------------------------------------------------------------------
//: Chang number of data points for yA
void dbsks_shapelet_grid::
change_num_yA(int num_yA)
{
  this->num_yA_ = num_yA;

  // update other parameters, keep step_yA unchanged
  double center_y = (this->min_yA_ + this->max_yA_) / 2;
  double half_num_yA = double(num_yA-1) / 2;
  this->min_yA_ = center_y - half_num_yA * this->step_yA_;
  this->max_yA_ = center_y + half_num_yA * this->step_yA_;
  for (int i=0; i<this->num_yA_; ++i)
  {
    this->yA_[i] = this->min_yA_ + i*this->step_yA_;
  }
  return;
}



// ----------------------------------------------------------------------------
//: Print grid information
void dbsks_shapelet_grid::
print(vcl_ostream& str)
{
  str << "Shapelet grid info: \n"
    << "  num_x = " << this->num_xA_ << "\n"
    << "  num_y = " << this->num_yA_ << "\n"
    << "  num_theta = " << this->num_psiA_ << "\n"
    << "  num_rA = " << this->num_rA_ << "\n"
    << "  num_len = " << this->num_len_ << "\n"
    << "  num_phiA = " << this->num_phiA_ << "\n"
    << "  num_phiB = " << this->num_phiB_ << "\n"
    << "  num_m = " << this->num_m_ << "\n"
    << "  Total number of points = " << this->size() << "\n";

  str << "xA: \n" << this->xA_ << "\n";
  str << "yA: \n" << this->yA_ << "\n";
  str << "psiA: \n" << this->psiA_ << "\n";
  str << "rA: \n" << this->rA_ << "\n";
  str << "len: \n" << this->len_ << "\n";
  str << "phiA: \n" << this->phiA_ << "\n";
  str << "phiB: \n" << this->phiB_ << "\n";
  str << "m: \n" << this->m_ << "\n";
  
}



