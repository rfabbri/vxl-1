// This is seg/dbsks/dbsks_hog.h
#ifndef dbsks_hog_h_
#define dbsks_hog_h_

//:
// \file
// \brief Functions for computing histogram of orientation gradient
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date September 26, 2008
//
// \verbatim
//  Modifications
// \endverbatim


//#include <vbl/vbl_ref_count.h>
//#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <vnl/vnl_vector.h>
#include <dbsksp/dbsksp_xshock_fragment.h>

//#include <vgl/vgl_point_2d.h>
//#include <vgl/vgl_vector_3d.h>
//#include <vcl_string.h>

//#include <vcl_vector.h>
//#include <vcl_map.h>
//#include <vsol/vsol_box_2d_sptr.h>
//#include <vsol/vsol_spatial_object_2d_sptr.h>
//#include <dbgl/algo/dbgl_circ_arc.h>
//
//#include <dbsksp/dbsksp_shapelet_sptr.h>
//#include <dbsksp/dbsksp_shock_node_sptr.h>
//#include <dbsksp/dbsksp_xshock_node.h>
//#include <dbsksp/dbsksp_shock_graph_sptr.h>
#include <vil/vil_image_view.h>

//: Compute histogram of orientation gradient (HOG) along the boundary of an
// extrinsic fragment
bool dbsks_hog_boundary(vnl_vector<double >& hog,
                        const vil_image_view<double >& Gx,
                        const vil_image_view<double >& Gy,
                        const dbsksp_xshock_fragment& xfrag, 
                        int num_segs = 4, 
                        double patch_width = 8, 
                        int num_orientation_bins = 9);



#endif // shp/dbsks/dbsks_hog.h


