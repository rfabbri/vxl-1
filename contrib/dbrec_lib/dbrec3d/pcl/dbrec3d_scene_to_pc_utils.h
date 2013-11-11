// This is dbrec3d_scene_to_pc_utils.h
#ifndef dbrec3d_scene_to_pc_utils_h
#define dbrec3d_scene_to_pc_utils_h

//:
// \file
// \brief Tools to converts boxm_scenes to point clouds (pcl)
// \author Isabel Restrepo mir@lems.brown.edu
// \date  25-Oct-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <pcl/common/common_headers.h>
#include <boxm/boxm_scene.h>


//: Converts a boxm_scene containing gradient information to a point cloud of normals
//  If resolution level isn't 0, it assumes that intermediate levels have been filled
void boxm_gradient_scene_to_pc(boxm_scene<boct_tree<short, vnl_vector_fixed< float,10 > > > *scene, 
                               boxm_scene<boct_tree<short,bool > > *valid_scene, 
                               pcl::PointCloud<pcl::PointNormal>::Ptr cloud,
                               short resolution_level,
                               float norm_thresh = 0.0);

//: Converts a vector of cells containing gradient information to a point cloud of normals
void boxm_cell_data_to_pcd(vcl_vector<boct_cell_data<short, vnl_vector_fixed<float,10> > > const &cells, 
                           pcl::PointCloud<pcl::PointNormal>::Ptr cloud);

#endif
