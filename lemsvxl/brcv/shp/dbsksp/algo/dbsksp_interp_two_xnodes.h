// This is shp/dbsksp/algo/dbsksp_interp_two_xnodes.h
#ifndef dbsksp_interp_two_xnodes_h_
#define dbsksp_interp_two_xnodes_h_

//:
// \file
// \brief Interpolation between two xnodes using shape fragments (shapelets)
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Mar 5, 2007
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbsksp/dbsksp_shapelet.h>
#include <dbsksp/dbsksp_xshock_node.h>
#include <dbgl/algo/dbgl_biarc.h>



// ============================================================================
// dbsksp_optimal_interp_2_extrinsic_nodes
// ============================================================================
class dbsksp_optimal_interp_two_xnodes
{
public:

  // constructors
  dbsksp_optimal_interp_two_xnodes(
    const dbsksp_xshock_node_descriptor& start_xnode,
    const dbsksp_xshock_node_descriptor& end_xnode);
  ~dbsksp_optimal_interp_two_xnodes(){};

  // --------------------------------------------------------------------------
  
  //: set the two extrinsic nodes
  void set(const dbsksp_xshock_node_descriptor& start_xnode,
    const dbsksp_xshock_node_descriptor& end_xnode);

  // --------------------------------------------------------------------------
  // DATA ACCESS
  // --------------------------------------------------------------------------
  dbgl_biarc shock_estimate() const {return this->shock_estimate_; }
  dbgl_biarc left_bnd_estimate() const {return this->left_bnd_estimate_; }
  dbgl_biarc right_bnd_estimate() const {return this->right_bnd_estimate_; }

  // Global optimum

  //: optimize to fit the extrinsic nodes
  dbsksp_twoshapelet_sptr optimize();

  //: General case optimize over alpha0
  dbsksp_twoshapelet_sptr optimize_over_alpha0_method_1();

  //: General case optimize over alpha0 - method 2
  dbsksp_twoshapelet_sptr optimize_over_alpha0_method_2();

  //: Special case: alpha0 = 0 constant, optimize over phi1
  dbsksp_twoshapelet_sptr optimize_over_phi1();

  // Local optimum (fit) to the end conditions given a parameter t \in [0..1]
  // Each t corresponds to an initial angle theta (the reverse is not true)
  // t represents the location of the initial 
  // shock-meet-point along the estimated biarc for the shock
  // the biarc naturally limits the range of theta.
  // As t varies from 0 to 1, theta varies inside that range
  dbsksp_twoshapelet_sptr init(double t);

  // fit a shape given t
  dbsksp_twoshapelet_sptr optimize(double t);


  // errors
  double start_error_;
  double end_error_;


protected:
  dbsksp_xshock_node_descriptor start_xnode_;
  dbsksp_xshock_node_descriptor end_xnode_;

  // estimation of boundary and shocks
  dbgl_biarc shock_estimate_;
  dbgl_biarc left_bnd_estimate_;
  dbgl_biarc right_bnd_estimate_;
};


#endif // shp/dbsksp/dbsksp_interp_two_xnodes.h









