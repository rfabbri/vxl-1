//:
// \brief
// \file
// \author Isabel Restrepo
// \date 11/8/11

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <dbrec3d/pcl/dbrec3d_pcl_features_util.h>

#include <pcl/io/pcd_io.h>

//:global variables
namespace pcl_compute_fphf_process_globals 
{
  using namespace pcl;
  using namespace pcl::io;
  
  const unsigned n_inputs_ = 3;
  const unsigned n_outputs_ = 0;
  
  
  bool load_cloud (const vcl_string &filename, pcl::PointCloud<pcl::PointNormal>::Ptr cloud)
  {
    vcl_cout << "Loading: " << filename <<vcl_endl;
    
    if (loadPCDFile (filename, *cloud) < 0)
      return (false);
    vcl_cout << " Done:" <<  cloud->width * cloud->height << " points\n";
    vcl_cout << "Available dimensions: " << getFieldsList(*cloud).c_str ();
    
    // Check if the dataset has normals
    vcl_vector<pcl::traits::fieldList<PointNormal>::type > field_list;

    return true;
  }
}


//:sets input and output types
bool pcl_compute_fphf_process_cons(bprb_func_process& pro)
{
  using namespace pcl_compute_fphf_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "vcl_string"; //point cloud input path
  input_types_[i++] = "double";  //radius for neighborhood computation
  input_types_[i++] = "vcl_string"; //point cloud output path
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool pcl_compute_fphf_process(bprb_func_process& pro)
{
  using namespace pcl_compute_fphf_process_globals;
  using namespace pcl;

  //get inputs
  unsigned i = 0;
  vcl_string cloud_in_file = pro.get_input<vcl_string>(i++);
  double radius = pro.get_input<double>(i++);
  vcl_string cloud_out_file = pro.get_input<vcl_string>(i++);
  
  //create and read from file the input poin cloud
  PointCloud<PointNormal>::Ptr cloud_in(new PointCloud<PointNormal>);
  load_cloud(cloud_in_file, cloud_in);
  
  PointCloud<FPFHSignature33>::Ptr cloud_out(new PointCloud<FPFHSignature33>);
  
  //compute the features
  dbrec3d_compute_pcl_fpfh_features(cloud_in, cloud_out, radius);
  
  //save ouput point cloud to file
  pcl::io::savePCDFileASCII (cloud_out_file, *cloud_out);   

  
  return true;
}