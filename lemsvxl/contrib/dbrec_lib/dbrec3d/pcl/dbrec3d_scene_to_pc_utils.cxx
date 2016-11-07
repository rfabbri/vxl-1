//:
// \file
// \author Isabel Restrepo
// \date 25-Oct-2011

#include "dbrec3d_scene_to_pc_utils.h"


//: Converts a boxm_scene containing gradient information to a point cloud of normals
//  If resolution level isn't 0, it assumes that intermediate levels have been filled
void boxm_gradient_scene_to_pc(boxm_scene<boct_tree<short, vnl_vector_fixed<float, 10> > > *scene, 
                               boxm_scene<boct_tree<short,bool > > *valid_scene, 
                               pcl::PointCloud<pcl::PointNormal>::Ptr cloud,
                               short resolution_level,
                               float norm_thresh)
{
    
    //cast the scene with steerable filters
    typedef vnl_vector_fixed<float,10 > sf_datatype;
    typedef boct_tree<short, sf_datatype > sf_tree_type;
    
    //iterate through the cells at the given resolution
    if(resolution_level !=0){
    vcl_cerr << "Case not implemented yet" << vcl_endl;
    return;
    }

    //Traverse leaf cells
    boxm_cell_iterator<sf_tree_type > cell_it = scene->cell_iterator(&boxm_scene<sf_tree_type >::load_block_and_neighbors);
    cell_it.begin();

    boxm_cell_iterator<boct_tree<short, bool> > valid_cell_it = valid_scene->cell_iterator(&boxm_scene<boct_tree<short, bool> >::load_block);
    valid_cell_it.begin();

    while ( !(cell_it.end() || valid_cell_it.end()) )
    {  

    boct_tree_cell<short, sf_datatype> *grad_cell = *cell_it;
    boct_tree_cell<short, bool> *valid_cell = *valid_cell_it;

    if (grad_cell->level()!= resolution_level) {
      ++cell_it; ++valid_cell_it;
      continue;
    }
    if(!valid_cell->data())
    {
      ++cell_it; ++valid_cell_it;
      continue;
    }


    sf_datatype cell_data = grad_cell->data();

    float norm = vcl_sqrt(cell_data[1]*cell_data[1] + cell_data[2]*cell_data[2] + cell_data[3]*cell_data[3]);
       
    if (norm < norm_thresh) {
      ++cell_it; ++valid_cell_it;
      continue;
    }


    pcl::PointNormal p;
    p.x = cell_it.global_centroid().x();
    p.y = cell_it.global_centroid().y();
    p.z = cell_it.global_centroid().z();
    p.normal_x =cell_data[1]/norm;
    p.normal_y =cell_data[2]/norm;
    p.normal_z =cell_data[3]/norm;
        
    //dummy curvature for now
    p.curvature = 0.0f;

    cloud->push_back(p);
    ++cell_it; ++valid_cell_it;
    }
    
  
}

//: Converts a vector of cells containing gradient information to a point cloud of normals
void boxm_cell_data_to_pcd(vcl_vector<boct_cell_data<short, vnl_vector_fixed<float,10> > > const &cells, 
                           pcl::PointCloud<pcl::PointNormal>::Ptr cloud)
{
  vcl_vector<boct_cell_data<short, vnl_vector_fixed<float,10> > >::const_iterator cell_data_it = cells.begin();
  
  for (; cell_data_it!=cells.end(); cell_data_it++) {
    
    vnl_vector_fixed<float,10> cell_data = cell_data_it->data_;
    
    float norm = vcl_sqrt(cell_data[1]*cell_data[1] + cell_data[2]*cell_data[2] + cell_data[3]*cell_data[3]);
    
    pcl::PointNormal p;
    p.x = cell_data_it->centroid_.x();
    p.y = cell_data_it->centroid_.y();
    p.z = cell_data_it->centroid_.z();
    p.normal_x =cell_data[1]/norm;
    p.normal_y =cell_data[2]/norm;
    p.normal_z =cell_data[3]/norm;
    
    //dummy curvature for now
    p.curvature = norm;
    
    cloud->push_back(p);
  }

}