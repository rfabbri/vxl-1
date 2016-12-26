// This is shp/dbsksp/algo/dbsksp_fit_one_shock_branch_cost_function.h
#ifndef dbsksp_fit_one_shock_branch_cost_function_h_
#define dbsksp_fit_one_shock_branch_cost_function_h_

//:
// \file
// \brief Cost function to fit a generative xshock model to an existing shock graph
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu )
// \date Oct 26, 2009
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbsksp/dbsksp_xshock_node_descriptor.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/vnl_matrix.h>





//==============================================================================
// dbsksp_vectorize_xshock_sample
//==============================================================================

//: Convert an xshock sample to vector-form
class dbsksp_vectorize_xshock_sample
{
private:
  dbsksp_vectorize_xshock_sample(){};

public:
  ~dbsksp_vectorize_xshock_sample(){};

  // Main functions

  //: Convert xshock sample to vector
  static vnl_vector<double > forward(const dbsksp_xshock_node_descriptor& xdesc);

  //: Convert vector (length = 5) to xshock sample
  static dbsksp_xshock_node_descriptor backward(const vnl_vector<double >& x);
};




//==============================================================================
// dbsksp_fit_shock_branch_residual
//==============================================================================

//: Cost function for a hypothesized set of A12 shock nodes
class dbsksp_fit_shock_branch_residual
{
public:
  //: Constructor
  // Requirement: the number of intervals is one more than the number of init xnode descriptors
  dbsksp_fit_shock_branch_residual(const dbsksp_xshock_node_descriptor& start_desc, 
    const dbsksp_xshock_node_descriptor& end_desc, 
    const vcl_vector<dbsksp_xshock_node_descriptor >& list_init_middle_xdesc,
    const vcl_vector<vcl_vector<const dbsksp_xshock_node_descriptor* > >& list_xsamples_per_interval,
    double total_left_bnd_length, double total_right_bnd_length);

  //: Destructor
  virtual ~dbsksp_fit_shock_branch_residual(){};

  //: Main function
  void compute_residual( vnl_vector<double >& fx );

  ////: Last value of the unknown vector
  //void last_x(vnl_vector<double >& x);

  ////: Return a list of xshock nodes corresponding to a given value of the unknown "x"
  //bool compute_list_middle_xdesc(const vnl_vector<double >& x, 
  //  vcl_vector<dbsksp_xshock_node_descriptor >& list_middle_xdesc);


protected:

  // list of xshock nodes, including the two ends
  vcl_vector<dbsksp_xshock_node_descriptor > list_xdesc_;

  ////: a matrix holding all info of the unknown xnodes
  //vnl_matrix<double > list_unknown_xdesc_;

  //: List of samples corresponding to the shock intervals
  vcl_vector<vcl_vector<const dbsksp_xshock_node_descriptor* > > list_xsamples_per_interval_;

  double orig_bnd_lengths_[2];

  //: Number of residuals
  unsigned num_residuals_;
};




//==============================================================================
// dbsksp_fit_one_shock_branch_cost_function
//==============================================================================

//: Cost function for a hypothesized set of A12 shock nodes
class dbsksp_fit_one_shock_branch_cost_function : public dbsksp_fit_shock_branch_residual,
                                                  public vnl_least_squares_function
{
public:
  //: Constructor
  // Requirement: the number of intervals is one more than the number of init xnode descriptors
  dbsksp_fit_one_shock_branch_cost_function(const dbsksp_xshock_node_descriptor& start_desc, 
    const dbsksp_xshock_node_descriptor& end_desc, 
    const vcl_vector<dbsksp_xshock_node_descriptor >& list_init_middle_xdesc,
    const vcl_vector<vcl_vector<const dbsksp_xshock_node_descriptor* > >& list_xsamples_per_interval,
    double total_left_bnd_length, double total_right_bnd_length);

  //: Destructor
  virtual ~dbsksp_fit_one_shock_branch_cost_function(){};

  //: Main function
  virtual void f(const vnl_vector<double >& x, vnl_vector<double >& fx);

  //: Last value of the unknown vector
  void cur_x(vnl_vector<double >& x);

  //: Return a list of xshock nodes corresponding to a given value of the unknown "x"
  bool x_to_xsamples(const vnl_vector<double >& x, 
    vcl_vector<dbsksp_xshock_node_descriptor >& xsamples);


protected:
  //: a matrix holding all info of the unknown xnodes
  vnl_matrix<double > list_unknown_xdesc_;
};





//==============================================================================
// dbsksp_fit_shock_branch_with_fixed_shock_points_cost_function
//==============================================================================

//: Cost function for a hypothesized set of A12 shock nodes
class dbsksp_fit_shock_branch_with_fixed_shock_points_cost_function : 
  public dbsksp_fit_shock_branch_residual,
  public vnl_least_squares_function
{
public:
  //: Constructor
  // Requirement: the number of intervals is one more than the number of init xnode descriptors
  dbsksp_fit_shock_branch_with_fixed_shock_points_cost_function(const dbsksp_xshock_node_descriptor& start_desc, 
    const dbsksp_xshock_node_descriptor& end_desc, 
    const vcl_vector<dbsksp_xshock_node_descriptor >& list_init_middle_xdesc,
    const vcl_vector<vcl_vector<const dbsksp_xshock_node_descriptor* > >& list_xsamples_per_interval,
    double total_left_bnd_length, double total_right_bnd_length);

  //: Destructor
  virtual ~dbsksp_fit_shock_branch_with_fixed_shock_points_cost_function(){};

  //: Main function
  virtual void f(const vnl_vector<double >& x, vnl_vector<double >& fx);

  //: Value of unknown vector represent current state of the cost function
  void cur_x(vnl_vector<double >& x);

  //: Return a list of xshock nodes corresponding to a given value of the unknown "x"
  bool x_to_xsamples(const vnl_vector<double >& x, 
    vcl_vector<dbsksp_xshock_node_descriptor >& xsamples) const;

protected:
  unsigned num_pts_;
  vnl_vector<double > interior_shock_x_;
  vnl_vector<double > interior_shock_y_;
  dbsksp_xshock_node_descriptor start_;
  dbsksp_xshock_node_descriptor end_;
};






#endif // shp/dbsksp/algo/dbsksp_fit_one_shock_branch_cost_function.h









