// This is file seg/dbsks/dbsks_xnode_grid.cxx

//:
// \file

#include "dbsks_xnode_grid.h"
#include <dbnl/dbnl_angle.h>
#include <vnl/vnl_math.h>



// ============================================================================
// dbsks_xnode_grid
// ============================================================================


//: Compute from grid parameters
bool dbsks_xnode_grid::
compute(const dbsks_xnode_grid_params& p)
{
  // x
  this->step_x_ = p.step_x;
  this->x_.set_size(p.num_x);
  for (int i =0; i < p.num_x; ++i)
  {
    this->x_[i] = p.min_x + i * p.step_x;
  }

  // y
  this->step_y_ = p.step_y;
  this->y_.set_size(p.num_y);
  for (int i =0; i < p.num_y; ++i)
  {
    this->y_[i] = p.min_y + i * p.step_y;
  }

  // psi
  this->step_psi_ = p.step_psi;
  this->psi_.set_size(p.num_psi);
  for (int i =0; i < p.num_psi; ++i)
  {
    this->psi_[i] = p.min_psi + i * p.step_psi;
  }

  // phi0
  this->step_phi0_ = p.step_phi0;
  this->phi0_.set_size(p.num_phi0);
  for (int i =0; i < p.num_phi0; ++i)
  {
    this->phi0_[i] = p.min_phi0 + i* p.step_phi0;
  }

  // r
  this->step_r_ = p.step_r;
  this->r_.set_size(p.num_r);
  for (int i =0; i < p.num_r; ++i)
  {
    this->r_[i] = p.min_r + i*p.step_r;
  }

  // phi1
  this->step_phi1_ = p.step_phi1;
  this->phi1_.set_size(p.num_phi1);
  for (int i =0; i < p.num_phi1; ++i)
  {
    this->phi1_[i] = p.min_phi1 + i* p.step_phi1;
  }


  return true;
}





//------------------------------------------------------------------------------
//: Set psi vector
void dbsks_xnode_grid::
set_psi_vec(double min_psi, double max_psi, double step_psi)
{
  assert(max_psi >= min_psi);
  assert(step_psi > 0);

  this->step_psi_ = step_psi;

  // make sure the range does not exceed 2pi
  int num_psi = 2*vnl_math_floor((max_psi-min_psi)/2 / step_psi) + 1;
  num_psi = vnl_math_min(num_psi, vnl_math_floor(2*vnl_math::pi / step_psi) + 1);

  // center the vector around the mean value
  double xmin = (max_psi+min_psi)/2 - step_psi*(num_psi-1)/2;

  this->psi_.set_size(num_psi);
  for (int i =0; i < num_psi; ++i)
  {
    this->psi_[i] = xmin + i * step_psi;
  }

  return;
}




//------------------------------------------------------------------------------
//: Set phi0 vector
void dbsks_xnode_grid::
set_phi0_vec(double min_phi0, double max_phi0, double step_phi0)
{
  assert(max_phi0 >= min_phi0);
  assert(step_phi0 > 0);

  min_phi0 = vnl_math_max(vnl_math::pi/36, min_phi0); // phi0 is always >= 0
  max_phi0 = vnl_math_min(vnl_math::pi* (1-1.0/36), max_phi0); // phi0 is always < pi;
  
  this->step_phi0_ = step_phi0;

  // make sure the range does not exceed pi
  int num_phi0 = 2*vnl_math_rnd((max_phi0-min_phi0)/2 / step_phi0) + 1;
  num_phi0 = vnl_math_min(num_phi0, vnl_math_floor(vnl_math::pi / step_phi0) + 1);

  // center the vector around the mean value
  double xmin = (max_phi0+min_phi0)/2 - step_phi0*(num_phi0-1)/2;

  this->phi0_.set_size(num_phi0);
  for (int i =0; i < num_phi0; ++i)
  {
    this->phi0_[i] = xmin + i * step_phi0;
  }

  return;
}


//------------------------------------------------------------------------------
//: Return an xnode descriptor
dbsksp_xshock_node_descriptor dbsks_xnode_grid::
xdesc(int i_x, int i_y, int i_psi, int i_phi0, int i_r) const
{
  return dbsksp_xshock_node_descriptor(this->x_[i_x], this->y_[i_y], 
    this->psi_[i_psi], this->phi0_[i_phi0], this->r_[i_r]);
}



// -----------------------------------------------------------------------------
//: Compute indices of a node descriptor. Return false if out of range
bool dbsks_xnode_grid::
xdesc_to_grid(const dbsksp_xshock_node_descriptor& xdesc, 
              int& i_x, int& i_y, int& i_psi, int& i_phi0, int& i_r) const
{
  //> parameter values 
  double x, y, psi, phi, r;
  xdesc.get(x, y, psi, phi, r);

  //> parameter indices
  i_x    = this->i_x(x);
  i_y    = this->i_y(y);
  i_psi  = this->i_psi(psi);
  i_phi0 = this->i_phi0(phi);
  i_r    = this->i_r(r);
  
  // legality check
  if (i_x<0 || i_x>= (int)this->x_.size() || 
    i_y<0 || i_y>=(int)this->y_.size() ||
    i_psi<0 || i_psi>=(int)this->psi_.size() ||
    i_phi0<0 || i_phi0>=(int)this->phi0_.size() ||
    i_r<0 || i_r>=(int)this->r_.size())
  {
    return false;
  }
  
  return true;
}







// -----------------------------------------------------------------------------
//: Compute indices of a node descriptor. Return false if out of range
int dbsks_xnode_grid::
grid_to_linear(int i_x, int i_y, int i_psi, int i_phi0, int i_r) const
{
  assert(i_x >= 0 && i_x < (int)this->x_.size());
  assert(i_y >= 0 && i_y < (int)this->y_.size());
  assert(i_psi >= 0 && i_psi < (int)this->psi_.size());
  assert(i_phi0 >= 0 && i_phi0 < (int)this->phi0_.size());
  assert(i_r >= 0 && i_r < (int)this->r_.size());

  return (i_r + this->r_.size() * 
    (i_phi0 + this->phi0_.size() *
    (i_psi + this->psi_.size() *
    (i_y + this->y_.size() * 
    i_x))));
}



// -----------------------------------------------------------------------------
//: Compute indices of a node descriptor. Return false if out of range
int dbsks_xnode_grid::
grid_to_linear(int i_x, int i_y, int i_psi, int i_phi0, int i_r, int i_phi1) const
{
  assert(i_phi1 >= 0 && i_phi1 < (int)this->phi1_.size());
  return i_phi1 + this->phi1_.size()*this->grid_to_linear(i_x, i_y, i_psi, i_phi0, i_r);
}











// -----------------------------------------------------------------------------
//: Compute indices of a node descriptor. Return false if out of range
bool dbsks_xnode_grid::
linear_to_grid(int i_linear, int& i_x, int& i_y, int& i_psi, int& i_phi0, int& i_r)
{
  if (i_linear < 0) return false;

  i_r = i_linear % this->r_.size();
  i_linear /= this->r_.size();

  i_phi0 = i_linear % this->phi0_.size();
  i_linear /= this->phi0_.size();

  i_psi = i_linear % this->psi_.size();
  i_linear /= this->psi_.size();

  i_y = i_linear % this->y_.size();
  i_linear /= this->y_.size();

  i_x = i_linear % this->x_.size();
  i_linear /= this->x_.size();

  return (i_linear == 0);
}


// -----------------------------------------------------------------------------
//: Compute indices of a node descriptor. Return false if out of range
bool dbsks_xnode_grid::
linear_to_grid(int i_linear, int& i_x, int& i_y, int& i_psi, int& i_phi0, int& i_r, int& i_phi1)
{
  if (i_linear < 0) return false;
  
  i_phi1 = i_linear % this->phi1_.size();
  i_linear /= this->phi1_.size();

  return this->linear_to_grid(i_linear, i_x, i_y, i_psi, i_phi0, i_r);
}



//------------------------------------------------------------------------------
//: Convert position within a cell (in x-y plane) from grid index to linear index
int dbsks_xnode_grid::
cell_grid_to_linear(int i_psi, int i_phi0, int i_r) const
{
  return (i_r + this->r_.size() * (i_phi0  + this->phi0_.size()*i_psi));
}


//------------------------------------------------------------------------------
//: Convert position within a cell (in x-y plane) from linear index to grid index
bool dbsks_xnode_grid::
cell_linear_to_grid(int i_linear, int& i_psi, int& i_phi0, int& i_r)
{
  i_r = i_linear % this->r_.size();
  i_linear /= this->r_.size();

  i_phi0 = i_linear % this->phi0_.size();
  i_linear /= this->phi0_.size();

  i_psi = i_linear % this->psi_.size();
  i_linear /= this->psi_.size();

  return (i_linear == 0);
}


// -----------------------------------------------------------------------------
//:
int dbsks_xnode_grid::
i_x(double x) const
{
  return vnl_math_rnd((x-this->x_[0]) / this->step_x_);
}

// -----------------------------------------------------------------------------
//:
int dbsks_xnode_grid::
i_y(double y) const
{
  return vnl_math_rnd((y-this->y_[0]) / this->step_y_);
}


// -----------------------------------------------------------------------------
//: Return index in the grid. No range check
int dbsks_xnode_grid::
i_psi(double psi) const
{
  double psi_0to2pi = dbnl_angle_0to2pi(psi-this->psi_[0]);
  int i_psi = vnl_math_rnd(psi_0to2pi / this->step_psi_);

  // just in case psi_0to2pi is very close to 2pi
  if (i_psi >= (int)this->psi_.size())
    i_psi = vnl_math_rnd((psi_0to2pi -vnl_math::pi*2) / this->step_psi_);
  return i_psi;
}




// -----------------------------------------------------------------------------
//: index of phi0
int dbsks_xnode_grid::
i_phi0(double phi0) const
{
  return vnl_math_rnd((phi0-this->phi0_[0]) / this->step_phi0_);
}

// -----------------------------------------------------------------------------
//: index of r
int dbsks_xnode_grid::
i_r(double r) const
{
  return vnl_math_rnd( (r - this->r_[0]) / this->step_r_ );
}


// -----------------------------------------------------------------------------
//: index of phi1
int dbsks_xnode_grid::
i_phi1(double phi1) const
{
  return vnl_math_rnd((phi1-this->phi1_[0]) / this->step_phi1_);
}


// -----------------------------------------------------------------------------
//: Generate a grid of points surrounding a reference location
// The generated points are guaranteed to lie inside the grid
bool dbsks_xnode_grid::
sample_xy(double x0, double y0, int num_x_backward, int num_x_forward,
          int num_y_backward, int num_y_forward, 
          vcl_vector<double >& xs, vcl_vector<double >& ys,
          vcl_vector<int >& index_xs, vcl_vector<int >& index_ys)
{
  // index of reference point
  int i_x0 = this->i_x(x0);
  int i_y0 = this->i_y(y0);

  // bounds of indices of the grid
  int min_i_x = vnl_math_max(i_x0 - num_x_backward, 0);
  int max_i_x = vnl_math_min(i_x0 + num_x_forward, (int)(this->x_.size())-1);
  int min_i_y = vnl_math_max(i_y0 - num_y_backward, 0);
  int max_i_y = vnl_math_min(i_y0 + num_y_forward, (int)(this->y_.size())-1);

  // total number of grid points
  int num_pts = vnl_math_max(max_i_x-min_i_x+1, 0) * vnl_math_max(max_i_y-min_i_y+1, 0);

  // allocate memory
  xs.resize(num_pts);
  ys.resize(num_pts);
  index_xs.resize(num_pts);
  index_ys.resize(num_pts);

  // coordinate of each point
  int idx = 0;
  for (int i_x = min_i_x; i_x <= max_i_x; ++i_x)
  {
    for (int i_y = min_i_y; i_y <= max_i_y; ++i_y)
    {
      xs[idx] = this->x_[i_x];
      ys[idx] = this->y_[i_y];
      index_xs[idx] = i_x;
      index_ys[idx] = i_y;
      ++idx;
    }
  }
  return true;
}







// -----------------------------------------------------------------------------
//: sample x
bool dbsks_xnode_grid::
sample_x(double ref_x, int num_x_backward, int num_x_forward,
         vcl_vector<double >& x_vec, vcl_vector<int >& index_x_vec) const
{
  // clear existing elements
  x_vec.clear();
  x_vec.reserve(num_x_backward+num_x_forward+1);

  index_x_vec.clear();
  index_x_vec.reserve(num_x_backward+num_x_forward+1);

  // index of reference point
  int i0 = this->i_x(ref_x);
  int min_index = vnl_math_max(i0 - num_x_backward, 0);
  int max_index = vnl_math_min(i0 + num_x_forward, int(this->x_.size())-1);

  // compute index and value for each point
  for (int i = min_index; i <= max_index; ++i)
  {
    x_vec.push_back(this->x_[i]);
    index_x_vec.push_back(i);
  }
  return true;
}


// -----------------------------------------------------------------------------
//: sample y
bool dbsks_xnode_grid::
sample_y(double ref_y, int num_y_backward, int num_y_forward,
         vcl_vector<double >& y_vec, vcl_vector<int >& index_y_vec) const
{
  // clear existing elements
  y_vec.clear();
  y_vec.reserve(num_y_backward+num_y_forward+1);

  index_y_vec.clear();
  index_y_vec.reserve(num_y_backward+num_y_forward+1);

  // index of reference point
  int i0 = this->i_y(ref_y);
  int min_index = vnl_math_max(i0 - num_y_backward, 0);
  int max_index = vnl_math_min(i0 + num_y_forward, int(this->y_.size())-1);

  // compute index and value for each point
  for (int i = min_index; i <= max_index; ++i)
  {
    y_vec.push_back(this->y_[i]);
    index_y_vec.push_back(i);
  }
  return true;
}









// -----------------------------------------------------------------------------
//: samples of psi on the grid around a given psi value "central_psi"
bool dbsks_xnode_grid::
sample_psi(double ref_psi, int num_psi_backward, int num_psi_forward, 
           vcl_vector<double >& psi_vec, vcl_vector<int >& index_psi_vec) const
{
  // clear existing elements
  psi_vec.clear();
  index_psi_vec.clear();

  // index of reference point
  int i0 = this->i_psi(ref_psi);

  // compute index and value for each point
  for (int i = -num_psi_backward; i <= num_psi_forward; ++i)
  {
    int idx = i + i0; // index within the grid
    if (idx >= 0 && idx < int(this->psi_.size()) )
    {
      psi_vec.push_back(this->psi_[idx]);
      index_psi_vec.push_back(idx);
    }
    else // if out of range, wrap around and check the index again
    {
      idx = this->i_psi(ref_psi + i*this->step_psi_);
      if (idx >= 0 && idx < int(this->psi_.size()))
      {
        psi_vec.push_back(this->psi_[idx]);
        index_psi_vec.push_back(idx);
      }
    }
  }
  return true;
}






// -----------------------------------------------------------------------------
//: sample phi0
bool dbsks_xnode_grid::
sample_phi0(double ref_phi0, int num_phi0_backward, int num_phi0_forward,
            vcl_vector<double >& phi0_vec, vcl_vector<int >& index_phi0_vec) const
{
  // clear existing elements
  phi0_vec.clear();
  phi0_vec.reserve(num_phi0_backward+num_phi0_forward+1);

  index_phi0_vec.clear();
  index_phi0_vec.reserve(num_phi0_backward+num_phi0_forward+1);

  // index of reference point
  int i0 = this->i_phi0(ref_phi0);
  int min_index = vnl_math_max(i0 - num_phi0_backward, 0);
  int max_index = vnl_math_min(i0 + num_phi0_forward, int(this->phi0_.size())-1);

  // compute index and value for each point
  for (int i = min_index; i <= max_index; ++i)
  {
    phi0_vec.push_back(this->phi0_[i]);
    index_phi0_vec.push_back(i);
  }
  return true;
}


// -----------------------------------------------------------------------------
//: sample r
bool dbsks_xnode_grid::
sample_r(double ref_r, int num_r_backward, int num_r_forward,
         vcl_vector<double >& r_vec, vcl_vector<int >& index_r_vec) const
{
  // clear existing elements
  r_vec.clear();
  r_vec.reserve(num_r_backward+num_r_forward+1);

  index_r_vec.clear();
  index_r_vec.reserve(num_r_backward+num_r_forward+1);

  // index of reference point
  int i0 = this->i_r(ref_r);
  int min_index = vnl_math_max(i0 - num_r_backward, 0);
  int max_index = vnl_math_min(i0 + num_r_forward, int(this->r_.size())-1);

  // compute index and value for each point
  for (int i = min_index; i <= max_index; ++i)
  {
    r_vec.push_back(this->r_[i]);
    index_r_vec.push_back(i);
  }
  return true;
}



// -----------------------------------------------------------------------------
//: sample phi1
bool dbsks_xnode_grid::
sample_phi1(double ref_phi1, int num_phi1_backward, int num_phi1_forward,
            vcl_vector<double >& phi1_vec, vcl_vector<int >& index_phi1_vec) const
{
  // clear existing elements
  phi1_vec.clear();
  phi1_vec.reserve(num_phi1_backward+num_phi1_forward+1);

  index_phi1_vec.clear();
  index_phi1_vec.reserve(num_phi1_backward+num_phi1_forward+1);

  // index of reference point
  int i0 = this->i_phi1(ref_phi1);
  int min_index = vnl_math_max(i0 - num_phi1_backward, 0);
  int max_index = vnl_math_min(i0 + num_phi1_forward, int(this->phi1_.size())-1);

  // compute index and value for each point
  for (int i = min_index; i <= max_index; ++i)
  {
    phi1_vec.push_back(this->phi1_[i]);
    index_phi1_vec.push_back(i);
  }
  return true;
}




