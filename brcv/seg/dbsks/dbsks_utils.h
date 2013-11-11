// This is seg/dbsks/dbsks_utils.h
#ifndef dbsks_utils_h_
#define dbsks_utils_h_

//:
// \file
// \brief Utilities used for both dbsks_dp_match and dbsks_local_match
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date March 2, 2007
//
// \verbatim
//  Modifications
// \endverbatim


//#include <vbl/vbl_ref_count.h>
//#include <dbdet/edge/dbdet_edgemap_sptr.h>
//#include <vnl/vnl_vector.h>
//#include <vgl/vgl_point_2d.h>
//#include <vgl/vgl_vector_3d.h>
//#include <vcl_string.h>

#include <vcl_vector.h>
#include <vcl_map.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <dbgl/algo/dbgl_circ_arc.h>

#include <dbsksp/dbsksp_shapelet_sptr.h>
#include <dbsksp/dbsksp_shock_node_sptr.h>
#include <dbsksp/dbsksp_xshock_node.h>
#include <dbsksp/dbsksp_shock_graph_sptr.h>



//: Rotate a set of point-tangents 
bool dbsks_rotate_point_set(const vcl_vector<vgl_point_2d<double > >& source_pts,                                      
                            const vcl_vector<vgl_vector_2d<double > >& source_tangents,
                            const vgl_point_2d<double >& origin, double angle,
                            vcl_vector<vgl_point_2d<double > >& target_pts,
                            vcl_vector<vgl_vector_2d<double > >& target_tangents);

//: Translate a set of point-tangents
bool dbsks_translate_point_set(const vcl_vector<vgl_point_2d<double > >& source_pts,
                               const vcl_vector<vgl_vector_2d<double > >& source_tangents,
                               const vgl_vector_2d<double >& v,
                               vcl_vector<vgl_point_2d<double > >& target_pts,
                               vcl_vector<vgl_vector_2d<double > >& target_tangents);





//: Compute bounding box of a set of shapelets
vsol_box_2d_sptr dbsks_compute_bounding_box(
  const vcl_vector<dbsksp_shapelet_sptr >& shapelet_list);

//: Compute bounding box of a set of xnodes
vsol_box_2d_sptr dbsks_compute_bounding_box(const
  vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor >& xnode_map);


//: Collect the the vertices of same depths and put them into bins
void dbsks_collect_shock_nodes_by_depth(const dbsksp_shock_graph_sptr& graph,
    vcl_vector<vcl_vector<dbsksp_shock_node_sptr > >& vertex_bins);


//: compute deformation cost between two shape fragments
float dbsks_deform_cost_shock_edit(const dbsksp_shapelet_sptr& s_ref, 
  bool include_front_arc,
  bool include_rear_arc,
  const dbsksp_shapelet_sptr& s_target);



//: Compute the size of a fragment (square root of the total occupying area)
double dbsks_fragment_size(const dbsksp_shapelet_sptr& s0,
  bool include_front_arc,
  bool include_rear_arc);



//: Compute size of the working graph by summing the size of the fragments
double dbsks_compute_graph_size(const dbsksp_shock_graph_sptr& graph);



//: compute deformation cost between two shape fragments
float dbsks_deform_cost_log2_scale_diff(const dbsksp_shapelet_sptr& s_ref, 
  bool include_front_arc,
  bool include_rear_arc,
  const dbsksp_shapelet_sptr& s_target);


//: compute deformation cost between two shape fragments
float dbsks_deform_cost(const dbsksp_shapelet_sptr& s_ref,
  bool include_front_arc,
  bool include_rear_arc,
  const dbsksp_shapelet_sptr& s_target,
  float sigma_deform);

//: Trace out the boundary of a one-branch graph with its extrinsic nodes
vcl_vector<dbgl_circ_arc > dbsks_bnd_arc_list(const dbsksp_shock_graph_sptr& graph,
  const vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor >& xnode_map);

//: Trace boundary of a one-branch graph
vcl_vector<vsol_spatial_object_2d_sptr > dbsks_trace_boundary(const dbsksp_shock_graph_sptr& graph,
  const vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor >& xnode_map);


//: Trace contact shocks
vcl_vector<vsol_spatial_object_2d_sptr > dbsks_trace_contact_shocks(const dbsksp_shock_graph_sptr& graph,
  const vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor >& xnode_map);


//: Determine whether two xnodes correspond to a LEGAL shape fragment
bool dbsks_is_legal_xfrag(const dbsksp_xshock_node_descriptor& xnode_parent,
                          const dbsksp_xshock_node_descriptor& xnode_child);




//: Append a file to an out stream
vcl_ostream& dbsks_append_text_file(vcl_ostream& os, const vcl_string& filename);

//: Helper function to avoid the assertion failure reported by the 'set' method of 'pdf1d_flat' class due to equality of min and max values
void dbsks_regularize_min_max_values(double& min_val, double& max_val);


#endif // shp/dbsks/dbsks_utils.h


