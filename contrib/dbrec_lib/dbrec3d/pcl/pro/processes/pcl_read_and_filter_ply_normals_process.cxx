//:
// \brief
// \file
// \author Isabel Restrepo
// \date 11/30/11

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>

//:global variables
namespace pcl_read_and_filter_ply_normals_process_globals 
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 0;
}


//:sets input and output types
bool pcl_read_and_filter_ply_normals_process_cons(bprb_func_process& pro)
{
  using namespace pcl_read_and_filter_ply_normals_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; //input .ply file
  input_types_[1] = "vcl_string"; //output .pcl file

  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool pcl_read_and_filter_ply_normals_process(bprb_func_process& pro)
{
  using namespace pcl_read_and_filter_ply_normals_process_globals;
  
  //get inputs
  vcl_string ply_fname = pro.get_input<vcl_string>(0);
  
  pcl::PointCloud<pcl::PointNormal> point_cloud;
  
  pcl::PLYReader reader;
  reader.read (ply_fname, point_cloud);

  vcl_cout << " Done:" <<  point_cloud.width * point_cloud.height << " points\n";
  vcl_cout << "Available dimensions: " << getFieldsList(point_cloud).c_str ();
  
   
  return true;
}