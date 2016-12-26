// This is shp/dbsksp/algo/dbsksp_fit_shock_branch.h
#ifndef dbsksp_fit_shock_branch_h_
#define dbsksp_fit_shock_branch_h_

//:
// \file
// \brief Constains list of functions to interpolate and sample a shock branch
//        
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Mar 13, 2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbsksp/algo/dbsksp_shock_path_sptr.h>
#include <dbsksp/dbsksp_xshock_node_descriptor.h>



//: Fit a shock branch to a list of xshock samples
// \todo merge this function with the functio below "dbsksp_fit_shock_path_using_min_num_pts"
// since they do the same thing
bool dbsksp_fit_one_shock_branch_with_power_of_2_intervals(const dbsksp_xshock_node_descriptor& start_desc, 
                            const dbsksp_xshock_node_descriptor& end_desc, 
                            const vcl_vector<dbsksp_xshock_node_descriptor >& xsamples, 
                            double error_threshold,
                            vcl_vector<dbsksp_xshock_node_descriptor >& list_intermediate_xdesc);







//: Fit a shock branch using minimum number of points
// `rms_tol' is tolerance for RMS of distance error
bool dbsksp_fit_shock_path_using_min_num_pts(const dbsksp_shock_path_sptr& in_path,
                                             const dbsksp_xshock_node_descriptor& start_xdesc, 
                                             const dbsksp_xshock_node_descriptor& end_xdesc, 
                                             double rms_tol,
                                             dbsksp_shock_path_sptr& out_path,
                                             int min_num_intervals = 1,
                                             int max_num_intervals = 10);


//: Coarse interpolation of between two xsamples
dbsksp_xshock_node_descriptor dbsksp_coarse_interp_btw_xsamples(const dbsksp_xshock_node_descriptor& xdesc0,
                                                                const dbsksp_xshock_node_descriptor& xdesc1,
                                                                double t);


#endif // shp/dbsksp/dbsksp_fit_shock_branch.h









