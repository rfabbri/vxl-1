// This is seg/dbsks/dbsks_compute_ocm_cost.h
#ifndef dbsks_compute_ocm_cost_h_
#define dbsks_compute_ocm_cost_h_

//:
// \file
// \brief Functions to compute OCM cost for different entities
//        
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Oct 29, 2008
//
// \verbatim
//  Modifications
// \endverbatim



#include <dbsks/dbsks_shotton_ocm.h>
#include <dbsks/dbsks_gray_ocm.h>
#include <dbsks/dbsks_ccm.h>
#include <dbsks/dbsks_biarc_sampler.h>
#include <dbsks/dbsks_ocm_image_cost.h>
#include <dbsksp/dbsksp_xshock_fragment.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbgl/algo/dbgl_biarc.h>


//: Compute ocm cost of a biarc
bool dbsks_compute_ocm_cost(const dbsks_ocm_image_cost& ocm, 
                            const dbgl_biarc& biarc, 
                            float& cost, 
                            double ds = 1);


//: Compute ocm cost of an extrinsic fragment
bool dbsks_compute_ocm_cost(const dbsks_ocm_image_cost& ocm,
                            const dbsksp_xshock_fragment& xfrag,
                            float& cost,
                            double ds = 1);

//: Compute ocm cost using pre-computed biarc points
bool dbsks_compute_ocm_cost(const dbsks_ocm_image_cost& ocm,
                            dbsks_biarc_sampler* biarc_sampler,
                            const dbsksp_xshock_fragment& xfrag,
                            float& cost,
                            vcl_vector<vgl_point_2d<double > >& pts,
                            vcl_vector<vgl_vector_2d<double > >& tangents);

//: Compute shotton ocm cost using a biarc sampler
bool dbsks_compute_ocm_cost(dbsks_shotton_ocm* shotton_ocm,
                            dbsks_biarc_sampler* biarc_sampler,
                            const dbsksp_xshock_fragment& xfrag,
                            float& cost);



// -----------------------------------------------------------------------------
//: Compute gray OCM cost using biarc sampler
bool dbsks_compute_ocm_cost_biarc(dbsks_gray_ocm* gray_ocm,
                            dbsks_biarc_sampler* biarc_sampler,
                            const vgl_point_2d<double >& start_pt,
                            const vgl_vector_2d<double >& start_tangent,
                            const vgl_point_2d<double >& end_pt,
                            const vgl_vector_2d<double >& end_tangent,
                            float& cost);






//: Compute gray OCM cost using biarc sampler
bool dbsks_compute_ocm_cost(dbsks_gray_ocm* gray_ocm,
                            dbsks_biarc_sampler* biarc_sampler,
                            const dbsksp_xshock_fragment& xfrag,
                            float& cost);


//: Compute gray OCM cost of a graph
bool dbsks_compute_ocm_cost(dbsks_gray_ocm* gray_ocm,
                            dbsks_biarc_sampler* biarc_sampler,
                            const dbsksp_xshock_graph_sptr& xgraph,
                            float& cost,
                            const vcl_vector<unsigned >& ignored_edges,
                            bool verbose = false);






// -----------------------------------------------------------------------------
//: Compute gray OCM cost using biarc sampler
bool dbsks_compute_ocm_cost_biarc(dbsks_ccm* ccm,
                            dbsks_biarc_sampler* biarc_sampler,
                            const vgl_point_2d<double >& start_pt,
                            const vgl_vector_2d<double >& start_tangent,
                            const vgl_point_2d<double >& end_pt,
                            const vgl_vector_2d<double >& end_tangent,
                            float& cost);


//: Compute contour OCM cost using biarc sampler
bool dbsks_compute_ocm_cost(dbsks_ccm* ccm,
                            dbsks_biarc_sampler* biarc_sampler,
                            const dbsksp_xshock_fragment& xfrag,
                            float& cost);

//: Compute contour OCM cost using biarc sampler
bool dbsks_compute_ocm_cost(dbsks_ccm* ccm,
                            dbsks_biarc_sampler* biarc_sampler,
                            const dbsksp_xshock_fragment& xfrag,
                            float& cost_left, float& cost_right);



//: Compute contour OCM cost of a graph
bool dbsks_compute_ocm_cost(dbsks_ccm* gray_ocm,
                            dbsks_biarc_sampler* biarc_sampler,
                            const dbsksp_xshock_graph_sptr& xgraph,
                            float& cost,
                            const vcl_vector<unsigned >& ignored_edges,
                            bool verbose = false);



//: Compute CCM cost for every boundary contour fragment of a graph
// Assumption: root node has been chosen and vertex depths have been computed
// Return a map from contour fragment labels to their ccm costs
// Format of contour fragment label: 13-L for left fragment of edge_id=13
bool dbsks_compute_ccm_of_xgraph(dbsks_ccm* ccm,
                            dbsks_biarc_sampler* biarc_sampler,
                            const dbsksp_xshock_graph_sptr& xgraph,
                            vcl_vector<vcl_string >& cfrag_labels, 
                            vcl_vector<float >& cfrag_ccm_costs);






#endif // seg/dbsks/dbsks_compute_ocm_cost.h


