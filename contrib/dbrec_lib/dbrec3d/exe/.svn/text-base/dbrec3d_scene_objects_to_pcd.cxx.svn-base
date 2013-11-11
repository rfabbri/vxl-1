//:
// \file
// \author Isabel Restrepo
// \date 11/3/11


#include <boxm/boxm_scene_parser.h>
#include <vul/vul_arg.h>
#include <dbrec3d/bof/bof_scene_categories.h>
#include <dbrec3d/pcl/dbrec3d_scene_to_pc_utils.h>

#include <pcl/io/pcd_io.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/features/fpfh.h>



int main(int argc, char** argv)
{
  vul_arg<vcl_string> scene_file("-scene_file", "scene filename", "");
  vul_arg<int> scene_id("-scene_id", "scene id", 0);
  vul_arg<vcl_string> bof_dir("-bof_dir", "Bag Of Features xml path", "");
  vul_arg<vcl_string> pcd_dir("-pcd_dir", "Dir to write the pcd files to");
  vul_arg_parse(argc, argv);
  
 
  // Create the scene
  boxm_scene_parser parser;
  
  boxm_scene_base_sptr scene_ptr=new boxm_scene_base();
  scene_ptr->load_scene(scene_file(), parser);
  
  if(scene_ptr->appearence_model() == VNL_FLOAT_10){
    //cast the  scene
    typedef boct_tree<short, vnl_vector_fixed<float,10> > tree_type;
    
    boxm_scene<tree_type>* scene = new boxm_scene<tree_type>();
    scene->load_scene(parser);
    scene_ptr = scene;
        
    vcl_vector<vgl_box_3d<double> > bboxes;
    bof_scene_categories category_info(bof_dir());
    category_info.load_objects(scene_id(), bboxes);
    
    vcl_vector<vgl_box_3d<double> >::iterator bboxes_it= bboxes.begin();
    
    //iterate through the objects
    unsigned object_id = 0;
    for (; bboxes_it != bboxes.end(); bboxes_it++) {
      vcl_stringstream pcd_file;
      
      pcd_file << pcd_dir() << "/object_" << object_id++ << ".pcd"; 
      
      vcl_vector<boct_cell_data<short, vnl_vector_fixed<float,10> > > cell_data;
      scene->cell_data_in_region(*bboxes_it, cell_data, 0);
      
      //convert the cell_data into point cloud
      pcl::PointCloud<pcl::PointNormal>::Ptr cloud (new pcl::PointCloud<pcl::PointNormal>);
      
      boxm_cell_data_to_pcd(cell_data, cloud);
      
      pcl::io::savePCDFileASCII (pcd_file.str(), *cloud); 

#if 0
      /********* Cumpute FPFH *****************/
      
      // Create the FPFH estimation class, and pass the input dataset+normals to it
      pcl::FPFHEstimation<pcl::PointNormal, pcl::PointNormal, pcl::FPFHSignature33> fpfh;
      fpfh.setInputCloud (cloud);
      fpfh.setInputNormals (cloud);
      
      // Create an empty kdtree representation, and pass it to the FPFH estimation object.
      // Its content will be filled inside the object, based on the given input dataset (as no other search surface is given).
      pcl::search::KdTree<pcl::PointNormal>::Ptr tree (new pcl::search::KdTree<pcl::PointNormal> ());
      fpfh.setSearchMethod (tree);
      
      // Output datasets
      pcl::PointCloud<pcl::FPFHSignature33>::Ptr fpfhs (new pcl::PointCloud<pcl::FPFHSignature33> ());
      
      // Use all neighbors in a sphere of radius 5cm
      // IMPORTANT: the radius used here has to be larger than the radius used to estimate the surface normals!!!
      fpfh.setRadiusSearch (7.5);
      
      // Compute the features
      fpfh.compute (*fpfhs); 

      vcl_stringstream pcd_file_out;
      pcd_file_out << pcd_dir() << "/fpfh_object_" << object_id++ << ".pcd"; 

      pcl::io::savePCDFileASCII (pcd_file_out.str(), *fpfhs);     
      
#endif
   }
  }
 
  return 0;

}

