// This is shp/dbsksp/algo/dbsksp_interp_xshock_fragment.h
#ifndef dbsksp_interp_xshock_fragment_h_
#define dbsksp_interp_xshock_fragment_h_

//:
// \file
// \brief Functions to interpolate an extrinsic shock fragment
//        
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Nov 20, 2009
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbsksp/dbsksp_shapelet_sptr.h>
#include <dbsksp/dbsksp_xshock_node_descriptor.h>
#include <dbgl/algo/dbgl_circ_arc.h>
#include <dbsksp/dbsksp_xshock_fragment.h>


//: Interpolate an extrinsic fragment using at most 3 shapelets
void dbsksp_interp_xfrag_with_max_three_shapelets(const dbsksp_xshock_node_descriptor& start,
                                        const dbsksp_xshock_node_descriptor& end,
                                        vcl_vector<dbsksp_shapelet_sptr >& list_shapelet);


// Should move this function to dbgl/algo
//: Compute a biarc from a two point-tangents. Assume the curvature of the first arc is known.
bool dbsksp_compute_biarc_given_k1(const vgl_point_2d<double >& pt1, 
                                   const vgl_vector_2d<double >& t1,
                                   const vgl_point_2d<double >& pt2,
                                   const vgl_vector_2d<double >& t2,
                                   double k1, // curvature of the first arc
                                   dbgl_circ_arc& arc1,  // returned pair of circular arc
                                   dbgl_circ_arc& arc2);

//: Fit a shapelet between two descriptors with the same shock point
// This is a degenerate case, often find is A1-A_\infty node
dbsksp_shapelet_sptr dbsksp_interp_xfrag_with_zero_chord_using_one_shapelet(const dbsksp_xshock_node_descriptor& start,
                                        const dbsksp_xshock_node_descriptor& end);


//: Compute extrinsic samples of a shock fragment
// The samples are equally spaced along the left boundary biarc
void dbsksp_compute_middle_xsamples_by_sampling_longer_bnd_biarc(int num_intervals,
                                                               const dbsksp_xshock_node_descriptor& start_xdesc,
                                                               const dbsksp_xshock_node_descriptor& end_xdesc,
                                                               vcl_vector<dbsksp_xshock_node_descriptor >& list_xsample);

//: Compute extrinsic samples of a shock fragment
// The samples are equally spaced along the left boundary biarc
void dbsksp_compute_middle_xsamples_by_sampling_left_bnd_biarc(int num_intervals,
                                                               const dbsksp_xshock_node_descriptor& start_xdesc,
                                                               const dbsksp_xshock_node_descriptor& end_xdesc,
                                                               vcl_vector<dbsksp_xshock_node_descriptor >& list_xsample);


//: Compute sample of an xshock fragment given a ratio parameter [0, 1]
// The sample is picked to minimize sum of curvature difference on the boundary
bool dbsksp_compute_xfrag_sample_with_min_kdiff(const dbsksp_xshock_fragment& xfrag, double t,
                                                dbsksp_xshock_node_descriptor& xsample );


//: Divide an xshock fragment into 2^n fragments
// require n > 0
// the return sample list includes start and end-samples
bool dbsksp_divide_xfrag_into_2_power_n_fragments(const dbsksp_xshock_fragment& xfrag, int n,
                                                vcl_vector<dbsksp_xshock_node_descriptor >& list_xsample);




#endif // shp/dbsksp/dbsksp_interp_xshock_fragment.h









