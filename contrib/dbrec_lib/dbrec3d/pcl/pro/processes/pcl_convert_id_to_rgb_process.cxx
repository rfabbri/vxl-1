//:
// \brief
// \file
// \author Isabel Restrepo
// \date 11/15/11

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <dbrec3d/pcl/dbrec3d_pcl_codebook_utils.h>
#include <dbrec3d/pcl/dbrec3d_pcl_point_types.h>

#include <pcl/io/pcd_io.h>

//:global variables
namespace pcl_convert_id_to_rgb_process_globals 
{
  const unsigned n_inputs_ = 3;
  const unsigned n_outputs_ = 0;
}


//:sets input and output types
bool pcl_convert_id_to_rgb_process_cons(bprb_func_process& pro)
{
  using namespace pcl_convert_id_to_rgb_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; //id cloud path
  input_types_[1] = "vcl_string"; //rgb cloud path
  input_types_[2] = "unsigned";

  vcl_vector<vcl_string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool pcl_convert_id_to_rgb_process(bprb_func_process& pro)
{
  using namespace pcl_convert_id_to_rgb_process_globals;
  using namespace pcl;
  
  //get inputs
  vcl_string id_cloud_pcd_file = pro.get_input<vcl_string>(0);
  vcl_string rgb_pcd_file = pro.get_input<vcl_string>(1);
  unsigned nmeans = pro.get_input<unsigned>(2);

  //read the id_cloud
  vcl_cout << "Loading: " << id_cloud_pcd_file <<vcl_endl;
  PointCloud<dbrec3d_pcl_point_types::PointClassId>::Ptr class_id_cloud(new PointCloud<dbrec3d_pcl_point_types::PointClassId>);
  if (pcl::io::loadPCDFile (id_cloud_pcd_file, *class_id_cloud) < 0)
    return (false);
  vcl_cout << " Done:" <<  class_id_cloud->width * class_id_cloud->height << " points\n";
  vcl_cout << "Available dimensions: " << getFieldsList(*class_id_cloud).c_str ();
  
  //creat an rgb cloud for visualization purposes
  PointCloud<PointXYZRGB>::Ptr rgb_cloud(new PointCloud<PointXYZRGB>);
  dbrec3d_pcl_codebook_utils::convert_id_to_rgb(class_id_cloud, nmeans, rgb_cloud);
  
  //save the rgb_cloud to memory
  pcl::io::savePCDFileBinary (rgb_pcd_file, *rgb_cloud);  
  
  return true;
}