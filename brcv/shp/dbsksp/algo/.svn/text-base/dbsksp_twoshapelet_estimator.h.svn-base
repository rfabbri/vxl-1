// This is shp/dbsksp/algo/dbsksp_twoshapelet_estimator.h
#ifndef dbsksp_twoshapelet_estimator_h_
#define dbsksp_twoshapelet_estimator_h_

//:
// \file
// \brief Cost functions to interpolate between two extrinsic nodes
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \data May 10, 2007
//
// \verbatim
//  Modifications
// \endverbatim


#include <dbsksp/dbsksp_shapelet.h>
#include <dbsksp/dbsksp_xshock_node.h>
#include <vnl/vnl_cost_function.h>
#include <vnl/vnl_matrix_fixed.h>


// ============================================================================
// dbsksp_estimate_twoshapelet
// ============================================================================
//: Estimate a twoshapet given a theta and a phi1 and two xnodes
// The only constraint that cannot be satisfied is the direction of the chord
// from the starting node to the end node
class dbsksp_twoshapelet_estimator
{
public:
  dbsksp_twoshapelet_estimator(){};
  ~dbsksp_twoshapelet_estimator(){};

  enum estimation_type{
    IGNORE_VERTICAL_DISCREPANCY = 0,
    IGNORE_RADIUS_DISCREPANCY = 1
  };

  static const double det_A_epsilon;

  //: Main function: estimate a twoshapelet given
  // start_xnode, end_xnode, alpha0, and phi1
  dbsksp_twoshapelet_sptr compute(
    const dbsksp_xshock_node_descriptor& start_xnode,
    const dbsksp_xshock_node_descriptor& end_xnode,
    double alpha0,
    double phi1,
    estimation_type type
    );

  //: determinant of matrix A used to compute chord lengths
  double det_A() const;
  double discrepancy_of_ignored_param() const 
  { return this->discrepancy_of_ignored_param_; }
  dbsksp_twoshapelet_sptr twoshapelet() const {return this->ss_; }

protected:
  //: Set the input data
  void set(const dbsksp_xshock_node_descriptor& new_start_xnode,
    const dbsksp_xshock_node_descriptor& new_end_xnode,
    double new_alpha0,
    double new_phi1)
  {
    this->start_xnode = new_start_xnode;
    this->end_xnode = new_end_xnode;
    this->alpha0 = new_alpha0;
    this->phi1 = new_phi1;
  }

  //: Do the actual computation for the over the constrained problem
  // ``type'' specifies the term that is ignored in the estimation
  dbsksp_twoshapelet_sptr compute(estimation_type type);


  // input values
  dbsksp_xshock_node_descriptor start_xnode;
  dbsksp_xshock_node_descriptor end_xnode;
  double alpha0; 
  double phi1;

  // cache values during computation
  vnl_matrix_fixed<double, 2, 2 > A;
  vnl_vector_fixed<double, 2> B;
  dbsksp_twoshapelet_sptr ss_;
  double discrepancy_of_ignored_param_;
};



////: Compute the value of phi1 at the discontinuity point on len0 and len1 as 
//// functions of phi1 when estimating a twoshapelet fitting to two extrinsic 
//// nodes
//double dbsksp_compute_phi1_where_len0_and_len1_discontinuous_at(const dbsksp_xshock_node_descriptor& start_xnode,
//  const dbsksp_xshock_node_descriptor& end_xnode,
//  double alpha0);









#endif // shp/dbsksp/dbsksp_twoshapelet_estimator.h









