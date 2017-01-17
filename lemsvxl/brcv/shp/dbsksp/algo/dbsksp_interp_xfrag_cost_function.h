// This is shp/dbsksp/algo/dbsksp_interp_xfrag_cost_function.h
#ifndef dbsksp_interp_xfrag_cost_function_h_
#define dbsksp_interp_xfrag_cost_function_h_

//:
// \file
// \brief Cost functions to support interpolating inside an xshock fragment
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Mar 10, 2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbsksp/dbsksp_xshock_fragment.h>
#include <vnl/vnl_cost_function.h>
#include <vnl/vnl_least_squares_function.h>

//: Coarse interpolation between two xsample
bool dbsksp_xfrag_coarse_interp_along_shock_curve(const dbsksp_xshock_node_descriptor& start,
                                             const dbsksp_xshock_node_descriptor& end,
                                             double t,
                                             dbsksp_xshock_node_descriptor& xsample);


//==============================================================================
// dbsksp_compute_shapelet_chord_cost_function
//==============================================================================
//: A cost function to compute the chord orientation for the degenerate shapelet
class dbsksp_compute_shapelet_chord_cost_function: public vnl_cost_function
{
public:
  dbsksp_compute_shapelet_chord_cost_function(double start_psi, double start_phi,
    double end_psi, double end_phi): vnl_cost_function(1),
    start_psi_(start_psi), sin_start_phi_(vcl_sin(start_phi)), 
    end_psi_(end_psi), sin_end_phi_(vcl_sin(end_phi))
    {}

  //: Main function - evaluate the cost
  double f(const vnl_vector<double >& x);

protected:
  double start_psi_;
  double sin_start_phi_;
  double end_psi_;
  double sin_end_phi_;
};





//==============================================================================
// dbsksp_xfrag_sample_kdiff_cost_function
//==============================================================================

//: Cost function for a hypothesized set of A12 shock nodes
class dbsksp_xfrag_sample_kdiff_cost_function : public vnl_least_squares_function
{
public:
  //: Constructor
  dbsksp_xfrag_sample_kdiff_cost_function(const dbsksp_xshock_fragment& xfrag, 
    double t);

  //: Destructor
  virtual ~dbsksp_xfrag_sample_kdiff_cost_function(){};

  //: Main function
  virtual void f(const vnl_vector<double >& x, vnl_vector<double >& fx);

  //: Convert from 'x' to xnode descriptor
  dbsksp_xshock_node_descriptor x_to_xsample(const vnl_vector<double >& x) const;

  //: Initial value for 'x'
  vnl_vector<double > initial_x() const;

protected:
  // User-input-----------------------------------------------------------------

  //: The fragment to compute sample from
  dbsksp_xshock_fragment xfrag_;

  // Parameter of the sample, between [0, 1]
  double t_; 

  // Intermediate variables-----------------------------------------------------
  dbsksp_xshock_node_descriptor start_;
  dbsksp_xshock_node_descriptor end_;
  dbgl_biarc bnd_biarc[2];
  vgl_point_2d<double > bnd_pt[2]; //0: left, 1: right
  double half_width; // half distance between two boundary points
};




//==============================================================================
// dbsksp_xfrag_sample_along_shock_biarc_cost_function
//==============================================================================

//: Cost function for a hypothesized set of A12 shock nodes
class dbsksp_xfrag_sample_along_shock_biarc_cost_function : public vnl_least_squares_function
{
public:
  //: Constructor
  dbsksp_xfrag_sample_along_shock_biarc_cost_function(const dbsksp_xshock_fragment& xfrag, 
    double t);

  //: Destructor
  virtual ~dbsksp_xfrag_sample_along_shock_biarc_cost_function(){};

  //: Main function
  virtual void f(const vnl_vector<double >& x, vnl_vector<double >& fx);

  //: Convert from 'x' to xnode descriptor
  dbsksp_xshock_node_descriptor x_to_xsample(const vnl_vector<double >& x) const;

  //: Initial value for 'x'
  vnl_vector<double > initial_x() const;

protected:
  // User-input-----------------------------------------------------------------

  //: The fragment to compute sample from
  dbsksp_xshock_fragment xfrag_;

  // Parameter of the sample, between [0, 1]
  double t_; 

  // Intermediate variables-----------------------------------------------------
  dbsksp_xshock_node_descriptor start_;
  dbsksp_xshock_node_descriptor end_;
  //dbgl_biarc bnd_biarc[2];
  dbsksp_xshock_node_descriptor init_xsample_;
};



#endif // shp/dbsksp/dbsksp_interp_xfrag_cost_function.h









