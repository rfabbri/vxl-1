//:
// \brief
// \file
// \author Isabel Restrepo
// \date 11/10/11

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <dbrec3d/pcl/dbrec3d_pcl_codebook_utils.h>
#include <dbrec3d/pcl/dbrec3d_pcl_point_types.h>

#include <pcl/io/pcd_io.h>


//:global variables
namespace pcl_k_means_assign_process_globals 
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 0;
}


//:sets input and output types
bool pcl_k_means_assign_process_cons(bprb_func_process& pro)
{
  using namespace pcl_k_means_assign_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "vcl_string"; //path to xyznormal .pcd file
  input_types_[i++] = "vcl_string"; //path to fpfh .pcd file
  input_types_[i++] = "vcl_string"; //path to the means
  input_types_[i++] = "vcl_string"; //output dir


  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool pcl_k_means_assign_process(bprb_func_process& pro)
{
  using namespace pcl_k_means_assign_process_globals;
  using namespace pcl;

  
  //get inputs
  unsigned i = 0;
  vcl_string xyznormal_pcd_file = pro.get_input<vcl_string>(i++);
  vcl_string fpfh_pcd_file = pro.get_input<vcl_string>(i++);
  vcl_string means_file = pro.get_input<vcl_string>(i++);
  vcl_string output_dir = pro.get_input<vcl_string>(i++); 
  
  //read in the features cloud
  vcl_cout << "Loading: " << fpfh_pcd_file <<vcl_endl;
  PointCloud<FPFHSignature33>::Ptr fpfh_cloud(new PointCloud<FPFHSignature33>);
  if (pcl::io::loadPCDFile (fpfh_pcd_file, *fpfh_cloud) < 0)
    return (false);
  vcl_cout << " Done:" <<  fpfh_cloud->width * fpfh_cloud->height << " points\n";
  vcl_cout << "Available dimensions: " << getFieldsList(*fpfh_cloud).c_str ();
  
  
  //read in the point cloud
  vcl_cout << "Loading: " << xyznormal_pcd_file <<vcl_endl;
  PointCloud<PointNormal>::Ptr point_cloud(new PointCloud<PointNormal>);
  if (pcl::io::loadPCDFile (xyznormal_pcd_file, *point_cloud) < 0)
    return (false);
  vcl_cout << " Done:" <<  point_cloud->width * point_cloud->height << " points\n";
  vcl_cout << "Available dimensions: " << getFieldsList(*point_cloud).c_str ();
  
  PointCloud<dbrec3d_pcl_point_types::PointClassId>::Ptr class_id_cloud(new PointCloud<dbrec3d_pcl_point_types::PointClassId>);
  
  
  //read the means
  vcl_ifstream mean_ifs(means_file.c_str());
  if(!mean_ifs.is_open()){
    vcl_cerr << "Error: Could not open mean_ifs: " << means_file <<  "\n";
    return false;
  }
  
  vcl_vector<vnl_vector_fixed<double,33> > means;
  unsigned num_means;
  mean_ifs >> num_means;
  
  vcl_cout << "Parsing: " << num_means << " means \n";
  
  for(unsigned i=0; i<num_means; i++){
    vnl_vector_fixed<double,33> mean;
    mean_ifs >> mean;
    means.push_back(mean);
  }
  
  mean_ifs.close();
  
  dbrec3d_pcl_codebook_utils::assign_features_to_cluster(fpfh_cloud, point_cloud, means, class_id_cloud );
  
  //save the id_cloud
  vcl_stringstream id_pcd_file;
  id_pcd_file << output_dir << "/class_id.pcd"; 
  pcl::io::savePCDFileASCII (id_pcd_file.str(), *class_id_cloud);   
  
  //clean memory
  point_cloud->clear();
  fpfh_cloud->clear();
  class_id_cloud->clear();
  
//  //creat an rgb cloud for visualization purposes
//  PointCloud<PointXYZRGB>::Ptr rgb_cloud(new PointCloud<PointXYZRGB>);
//  dbrec3d_pcl_codebook_utils::convert_id_to_rgb(class_id_cloud, means.size(), rgb_cloud);
//  
//  //save the rgb_cloud to memory
//  vcl_stringstream rgb_pcd_file;
//  rgb_pcd_file << output_dir << "/class_id_rgb.pcd"; 
//  pcl::io::savePCDFileASCII (rgb_pcd_file.str(), *rgb_cloud);  
    
  return true;
}