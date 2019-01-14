//:
// \brief A process that transforms a point cloud such that it's XY axis correspond to eigen vectors of XY scatter
// \file
// \author Isabel Restrepo
// \date 11/22/11

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <dbrec3d/pcl/dbrec3d_pcl_transforms.h>

#include <pcl/io/pcd_io.h>


//:global variables
namespace pcl_trasform_xy_align_process_globals 
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 0;
}


//:sets input and output types
bool pcl_trasform_xy_align_process_cons(bprb_func_process& pro)
{
  using namespace pcl_trasform_xy_align_process_globals ;
  
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = vcl_string";
  input_types_[1] = vcl_string";

  
  std::vector<std::string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool pcl_trasform_xy_align_process(bprb_func_process& pro)
{
  using namespace pcl_trasform_xy_align_process_globals;
  
  //get inputs
  std::string cloud_in_file = pro.get_input<std::string>(0);
  std::string cloud_out_file = pro.get_input<std::string>(1);

  //read in the point cloud
  std::cout << "Loading: " << cloud_in_file <<std::endl;
  pcl::PointCloud<pcl::PointNormal>::Ptr cloud_in(new pcl::PointCloud<pcl::PointNormal>);
  if (pcl::io::loadPCDFile (cloud_in_file, *cloud_in) < 0)
    return (false);
  std::cout << " Done:" <<  cloud_in->width * cloud_in->height << " points\n";
  std::cout << "Available dimensions: " << getFieldsList(*cloud_in).c_str ();
  
  //transform point cloud
  pcl::PointCloud<pcl::PointNormal>::Ptr cloud_out(new pcl::PointCloud<pcl::PointNormal>);
  dbrec3d_pcl_transforms::xy_canonical(*cloud_in, *cloud_out);
  
  
  //write output point cloud
  pcl::io::savePCDFileBinary (cloud_out_file, *cloud_out);  

  
  
  
  return true;
}