// This is file shp/dbsksp/algo/dbsksp_fit_one_shock_branch_cost_function.cxx

//:
// \file

#include "dbsksp_fit_one_shock_branch_cost_function.h"

#include <vnl/vnl_math.h>
#include <dbsksp/dbsksp_xshock_fragment.h>
#include <dbsksp/dbsksp_xshock_fragment_sptr.h>
#include <dbgl/algo/dbgl_closest_point.h>
#include <vgl/vgl_distance.h>




//==============================================================================
// dbsksp_fit_shock_branch_residual
//==============================================================================

//: Constructor
// Requirement: the number of intervals is one more than the number of init xnode descriptors
dbsksp_fit_shock_branch_residual::
dbsksp_fit_shock_branch_residual(const dbsksp_xshock_node_descriptor& start_xdesc, 
                                 const dbsksp_xshock_node_descriptor& end_xdesc, 
                                 const vcl_vector<dbsksp_xshock_node_descriptor >& list_init_xdesc,
                                 const vcl_vector<vcl_vector<const dbsksp_xshock_node_descriptor* > >& list_xsamples_per_interval,
                                 double total_left_bnd_length, double total_right_bnd_length)
{
  // Preliminary checks
  if ( (list_init_xdesc.size() +1) != list_xsamples_per_interval.size() )
  {
    vcl_cerr << "\nERROR: the number of xsample intervals should be 1 more than the number of unknown xshock nodes.\n";
    return;
  }

  this->num_residuals_ = 0;
  for (unsigned i =0; i < list_xsamples_per_interval.size(); ++i)
  {
    this->num_residuals_ += list_xsamples_per_interval[i].size() * 2;
  }
  // add 2 more for penalty terms when there is length difference
  this->num_residuals_ += 2;

  // List of xshock nodes for the whole branch, including the two ends
  this->list_xdesc_.reserve(2 + list_init_xdesc.size());
  this->list_xdesc_.push_back(start_xdesc);
  this->list_xdesc_.insert(this->list_xdesc_.end(), list_init_xdesc.begin(), list_init_xdesc.end());
  this->list_xdesc_.push_back(end_xdesc);

  // List of samples
  this->list_xsamples_per_interval_ = list_xsamples_per_interval;

  // boundary length
  this->orig_bnd_lengths_[0] = total_left_bnd_length;
  this->orig_bnd_lengths_[1] = total_right_bnd_length;
}


//: Main function
void dbsksp_fit_shock_branch_residual::
compute_residual( vnl_vector<double >& fx )
{
  // boundary length
  //double cur_bnd_lengths[] = {0,0}; 
  double long_bnd_penalty[] = {0,0};
  vnl_vector<double >::iterator iter = fx.begin();
  for (unsigned k =1; k < this->list_xdesc_.size(); ++k)
  {
    dbsksp_xshock_fragment xfrag(this->list_xdesc_[k-1], this->list_xdesc_[k]);
    dbgl_biarc left = xfrag.bnd_left_as_biarc();
    dbgl_biarc right = xfrag.bnd_right_as_biarc();

    // penalize for long boundary length
    double ds_left = left.len() - vnl_math::pi * vgl_distance(left.start(), left.end());
    long_bnd_penalty[0] += (ds_left < 0) ? 0 : ds_left;

    double ds_right = right.len() - vnl_math::pi * vgl_distance(right.start(), right.end());
    long_bnd_penalty[1] += (ds_right < 0) ? 0 : ds_right;

    ////
    //cur_bnd_lengths[0] += left.len();
    //cur_bnd_lengths[1] += right.len();

    // penalty for illegal fragment
    // \todo: 100 is an arbitrary choice. Find a more systematic way to avoid illegal fragments.
    double multiplier = xfrag.is_legal() ? 1 : 100; 

    // Retrieve xsamples corresponding to this interval
    vcl_vector<const dbsksp_xshock_node_descriptor* >& interval_xsamples = this->list_xsamples_per_interval_[k-1];

    // Computer error for each of these samples
    for (unsigned i =0; i < interval_xsamples.size(); ++i)
    {
      const dbsksp_xshock_node_descriptor* xdesc = interval_xsamples[i];
      double dummy;
      double dist_left = dbgl_closest_point::point_to_biarc(xdesc->bnd_pt_left(), left, dummy);
      double dist_right = dbgl_closest_point::point_to_biarc(xdesc->bnd_pt_right(), right, dummy);

      *(iter++) = multiplier * dist_left;
      *(iter++) = multiplier * dist_right;
    }
  }

  // Penalize if the fitting fragment has longer length than the original branch
  for (int side =0; side < 2; ++side)
  {
    ////
    //double delta_s = cur_bnd_lengths[side] - 1.2* this->orig_bnd_lengths_[side];
    //double cost = (delta_s < 0) ? 0 : (vcl_exp(delta_s)-1); 

    double cost = vcl_exp(long_bnd_penalty[side]) - 1;
    *(iter++) = cost;
  }

  assert(iter == fx.end());
  return;
}




//==============================================================================
// dbsksp_vectorize_xshock_sample
//==============================================================================




//------------------------------------------------------------------------------
//: Convert xshock sample to vector
vnl_vector<double > dbsksp_vectorize_xshock_sample::
forward(const dbsksp_xshock_node_descriptor& xdesc)
{
  vnl_vector<double > x(5, 0);
  vgl_point_2d<double > bnd_centroid = centre(xdesc.bnd_pt_left(), xdesc.bnd_pt_right());
  x[0] = bnd_centroid.x();
  x[1] = bnd_centroid.y();

  // Compute shock tangent by rotating the line connecting two boundary points
  vgl_vector_2d<double > n0 = (xdesc.bnd_pt_right() - xdesc.bnd_pt_left()) / 2;
  vgl_vector_2d<double > t0 = rotated(n0, vnl_math::pi_over_2);
  // note that t0 encodes both orientation of the shock tangent as well as distance
  // between two boundary points
  x[2] = t0.x();
  x[3] = t0.y();

  // (signed) distance from boundary centroid to the shock point
  x[4] = -xdesc.radius() * vcl_cos(xdesc.phi());

  return x;
}

//------------------------------------------------------------------------------
//: Convert vector (length = 5) to xshock sample
dbsksp_xshock_node_descriptor dbsksp_vectorize_xshock_sample::
backward(const vnl_vector<double >& x)
{
  vgl_point_2d<double > bnd_center(x[0], x[1]);
  vgl_vector_2d<double > t0(x[2], x[3]); // shock tangent
  double b = x[4];
  
  // half distance between two boundary points
  double H = t0.length();

  // shock radius
  double R = vcl_sqrt(H*H + b*b);

  // shock point
  vgl_point_2d<double > pt = bnd_center + (b/H) * t0;

  // phi angle
  double phi = vcl_acos(-b / R);

  // shock tangent angle
  double psi = vcl_atan2(t0.y(), t0.x());

  dbsksp_xshock_node_descriptor xdesc(pt.x(), pt.y(), psi, phi, R);
  return xdesc;
}







//==============================================================================
// dbsksp_fit_one_shock_branch_cost_function
//==============================================================================

//------------------------------------------------------------------------------
//: Constructor
// Requirement: the number of intervals is one more than the number of init xnode descriptors
dbsksp_fit_one_shock_branch_cost_function::
dbsksp_fit_one_shock_branch_cost_function(const dbsksp_xshock_node_descriptor& start_xdesc, 
    const dbsksp_xshock_node_descriptor& end_xdesc, 
    const vcl_vector<dbsksp_xshock_node_descriptor >& list_init_xdesc,
    const vcl_vector<vcl_vector<const dbsksp_xshock_node_descriptor* > >& list_xsamples_per_interval,
    double total_left_bnd_length, double total_right_bnd_length):
dbsksp_fit_shock_branch_residual(start_xdesc, end_xdesc, list_init_xdesc, list_xsamples_per_interval,
                                 total_left_bnd_length, total_right_bnd_length),
vnl_least_squares_function(0, 0, vnl_least_squares_function::no_gradient)
{
  // Compute the number of unknown and residuals
  unsigned num_unknowns = 3*list_init_xdesc.size();
  this->init(num_unknowns, this->num_residuals_);

  // Construct matrix holding the varying xshock nodes
  this->list_unknown_xdesc_.set_size(list_init_xdesc.size(), 5);
  for (unsigned i =0; i < list_init_xdesc.size(); ++i)
  {
    this->list_unknown_xdesc_.set_row(i, dbsksp_vectorize_xshock_sample::forward(list_init_xdesc[i]));
  }
}


//------------------------------------------------------------------------------
//: Main function
void dbsksp_fit_one_shock_branch_cost_function::
f(const vnl_vector<double >& x, vnl_vector<double >& fx)
{
  // x is organized in tuples of 3 where each tuple corresponds to unknowns of an xshock node

  // list_unknown_xdesc_ has size : num_unknown_xnodes \times 5
  // Each row in list_unknown_xdesc_ corresponds to an unknown xshock node
  // The 5 variables: Bx, By, Hx, Hy, b, which correspond to boundary center, tangent vector,
  // and signed distance from shock point to the center of two boundary points
  // We fix Bx and By during  optimization, leaving 3 varying parameters.

  // Procedure:
  //   - update "list_unknown_xdesc_" using x
  //   - update the "list_xdesc_" using "list_unknown_xdesc_"
  //   - compute the residuals using list_xdesc_ and the samples

  // Update list_unknown_xdesc
  for (unsigned r =0; r < this->list_unknown_xdesc_.rows(); ++r)
  {
    int idx = 3*r;
    this->list_unknown_xdesc_(r, 2) = x[idx];
    this->list_unknown_xdesc_(r, 3) = x[idx+1];
    this->list_unknown_xdesc_(r, 4) = x[idx+2];
  }

  // Update list_xdesc_
  for (unsigned r =0; r < this->list_unknown_xdesc_.rows(); ++r)
  {
    this->list_xdesc_[r+1] = dbsksp_vectorize_xshock_sample::backward(this->list_unknown_xdesc_.get_row(r));
  }

  // Compute residuals
  this->compute_residual(fx);
}



//------------------------------------------------------------------------------
//: Last value of the unknown vector
void dbsksp_fit_one_shock_branch_cost_function::
cur_x(vnl_vector<double >& x)
{
  x.set_size(this->get_number_of_unknowns());
  for (unsigned row =0; row < this->list_unknown_xdesc_.rows(); ++row)
  {
    unsigned idx = 3*row;
    x[idx] = this->list_unknown_xdesc_(row, 2);
    x[idx+1] = this->list_unknown_xdesc_(row, 3);
    x[idx+2] = this->list_unknown_xdesc_(row, 4);
  }
  return;
}



//------------------------------------------------------------------------------
//: Return a list of xshock nodes corresponding to a given value of the unknown "x"
bool dbsksp_fit_one_shock_branch_cost_function::
x_to_xsamples(const vnl_vector<double >& x, 
              vcl_vector<dbsksp_xshock_node_descriptor >& xsamples)
{
  // Preliminary check for vector size
  if (x.size() != this->get_number_of_unknowns())
    return false;

  // Clean up old data and allocate space
  xsamples.clear();
  xsamples.reserve(this->list_unknown_xdesc_.rows());
  
  // Compute the middle xnode descriptors, one by one
  for (unsigned row =0; row < this->list_unknown_xdesc_.rows(); ++row)
  {
    unsigned idx = 3*row;
    vnl_vector<double > sample = this->list_unknown_xdesc_.get_row(row);
    sample[2] = x[idx];
    sample[3] = x[idx+1];
    sample[4] = x[idx+2];

    xsamples.push_back(dbsksp_vectorize_xshock_sample::backward(sample));
  }

  return true;
}







//==============================================================================
// dbsksp_fit_shock_branch_with_fixed_shock_points_cost_function
//==============================================================================


//: Constructor
// Requirement: the number of intervals is one more than the number of init xnode descriptors
dbsksp_fit_shock_branch_with_fixed_shock_points_cost_function::
dbsksp_fit_shock_branch_with_fixed_shock_points_cost_function(
  const dbsksp_xshock_node_descriptor& start_xdesc, 
  const dbsksp_xshock_node_descriptor& end_xdesc, 
  const vcl_vector<dbsksp_xshock_node_descriptor >& list_init_middle_xdesc,
  const vcl_vector<vcl_vector<const dbsksp_xshock_node_descriptor* > >& list_xsamples_per_interval,
  double total_left_bnd_length, double total_right_bnd_length):
dbsksp_fit_shock_branch_residual(start_xdesc, end_xdesc, list_init_middle_xdesc, list_xsamples_per_interval,
                                 total_left_bnd_length, total_right_bnd_length),
vnl_least_squares_function(0, 0, vnl_least_squares_function::no_gradient)
{
  // Save the list of shock points (fixed during optimization)
  // The list_xdesc_ consists of all descriptors, including the two end points
  // We only to vary the interior descriptors, keeping the two end points fixed.
  this->num_pts_ = this->list_xdesc_.size()-2;
  
  this->interior_shock_x_.set_size(this->num_pts_);
  this->interior_shock_y_.set_size(this->num_pts_);
  for (unsigned i =0; i < this->num_pts_; ++i)
  {
    this->interior_shock_x_[i] = this->list_xdesc_[i+1].x();
    this->interior_shock_y_[i] = this->list_xdesc_[i+1].y();
  }

  // the two end points
  this->start_ = this->list_xdesc_.front();
  this->end_ = this->list_xdesc_.back();

  // Compute the number of unknown and residuals
  unsigned num_unknowns = 3*this->num_pts_;
  this->init(num_unknowns, this->num_residuals_);
}



//------------------------------------------------------------------------------
//: Main function
void dbsksp_fit_shock_branch_with_fixed_shock_points_cost_function::
f(const vnl_vector<double >& x, vnl_vector<double >& fx)
{
  this->x_to_xsamples(x, this->list_xdesc_);
  this->compute_residual(fx);
  return;
}

//------------------------------------------------------------------------------
//: Value of unknown vector represent current state of the cost function
void dbsksp_fit_shock_branch_with_fixed_shock_points_cost_function::
cur_x(vnl_vector<double >& x)
{
  x.set_size(this->get_number_of_unknowns());

  vnl_vector<double >::iterator x_iter = x.begin();
  for (unsigned i =0; i < this->num_pts_; ++i)
  {
    *(x_iter++) = this->list_xdesc_[i+1].psi();
    *(x_iter++) = this->list_xdesc_[i+1].phi();
    *(x_iter++) = this->list_xdesc_[i+1].radius();
  }

  assert(x_iter == x.end());
  return;
}


//------------------------------------------------------------------------------
//: Return list of xsamples corresponding to a given value of the unknown "x"
bool dbsksp_fit_shock_branch_with_fixed_shock_points_cost_function::
x_to_xsamples(const vnl_vector<double >& x, 
              vcl_vector<dbsksp_xshock_node_descriptor >& xsamples) const
{
  // We fix the position of the shock points and allow the other three variables 
  // (psi, phi, radius) to change
  assert(x.size() == 3 * this->num_pts_);

  vnl_vector<double >::const_iterator x_iter = x.begin();  
  xsamples.resize(this->num_pts_+2);

  // start and end are fixed
  xsamples.front() = this->start_;
  xsamples.back() = this->end_;
  for (unsigned i =0; i < this->num_pts_; ++i)
  {
    double psi = *(x_iter++);
    double phi = *(x_iter++);
    double radius = *(x_iter++);
    xsamples[i+1].set(this->interior_shock_x_[i], this->interior_shock_y_[i], psi, phi, radius);
  }
 

  assert(x_iter == x.end());
  return true;
}


