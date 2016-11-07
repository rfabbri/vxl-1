// This is seg/dbsks/dbsks_dp_match_utils.h
#ifndef dbsks_dp_match_utils_h_
#define dbsks_dp_match_utils_h_

//:
// \file
// \brief A class to detect symmetry points from image evidence
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Feb 6, 2007
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
//#include <dbsksp/dbsksp_xshock_node.h>
//#include <vcl_vector.h>


#include <dbsks/dbsks_dp_match.h>
#include <dbsks/dbsks_dp_match_sptr.h>

//#include <dbsksp/dbsksp_shock_graph_sptr.h>


//: Construct a graph from a list of states for each of its edges
dbsksp_shock_graph_sptr dbsks_construct_graph(
  dbsks_dp_match_sptr dp_engine,
  vcl_map<unsigned int, vgl_point_2d<int > >& graph_i_state_map);

//: 
dbsksp_shock_graph_sptr dbsks_construct_graph(
  dbsks_dp_match_sptr dp_engine,
  vcl_map<dbsksp_shock_edge_sptr, vgl_point_2d<int > >& graph_i_state_map);




#endif // shp/dbsks/dbsks_dp_match_utils.h


