//:
// \brief
// \file
// \author Isabel Restrepo
// \date 11/10/11

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <dbrec3d/pcl/dbrec3d_pcl_codebook_utils.h>

#include <pcl/io/pcd_io.h>


//:global variables
namespace pcl_k_means_learning_process_globals 
{
  const unsigned n_inputs_ = 6;
  const unsigned n_outputs_ = 0;
}


//:sets input and output types
bool pcl_k_means_learning_process_cons(bprb_func_process& pro)
{
  using namespace pcl_k_means_learning_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "vcl_string"; //path to .pcd file
  input_types_[i++] = "unsigned"; //K, number of means
  input_types_[i++] = "double"; //fraction of samples to use during initialization refinement
  input_types_[i++] = "unsigned"; //number of iterations to use during initialization refinement
  input_types_[i++] = "unsigned"; //max number of iterations during k-means
  input_types_[i++] = "vcl_string"; //k-means dir/where results are saved to 
 
  vcl_vector<vcl_string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool pcl_k_means_learning_process(bprb_func_process& pro)
{
  using namespace pcl_k_means_learning_process_globals;
  using namespace pcl;

  
  //get inputs
  unsigned i =0;
  vcl_string pcd_file = pro.get_input<vcl_string>(i++);
  unsigned K = pro.get_input<unsigned>(i++);
  double fraction = pro.get_input<double>(i++);
  unsigned J = pro.get_input<unsigned>(i++);
  unsigned max_it = pro.get_input<unsigned>(i++);
  vcl_string k_means_dir = pro.get_input<vcl_string>(i++);
   
  
  //read in the point cloud
  vcl_cout << "Loading: " << pcd_file <<vcl_endl;
  PointCloud<FPFHSignature33>::Ptr cloud(new PointCloud<FPFHSignature33>);
  if (pcl::io::loadPCDFile (pcd_file, *cloud) < 0)
    return (false);
  vcl_cout << " Done:" <<  cloud->width * cloud->height << " points\n";
  vcl_cout << "Available dimensions: " << getFieldsList(*cloud).c_str ();
  
  //run k-means clustering
  vcl_vector<vnl_vector_fixed<double,33> > means;
  vcl_vector<dbcll_euclidean_cluster_light<33> > all_clusters;
  dbrec3d_pcl_codebook_utils::learn_codebook (K, fraction, J, max_it, cloud, means, all_clusters);
  
  //save clusters information to file
  dbcll_xml_write(all_clusters, k_means_dir + "/lowest_sse_means_info.xml");
  
  //save new means to file
  vcl_ofstream means_ofs((k_means_dir + "/lowest_sse_means.txt").c_str());
  means_ofs.precision(15);
  means_ofs << means.size() << "\n";
  if(means_ofs.is_open())
    for (unsigned i =0; i<means.size(); i++) {
      means_ofs << means[i] << "\n";
    }
  
  else
    vcl_cerr << "Could not open file: " << (k_means_dir + "/lowest_sse_means.txt") << "\n";
  
  means_ofs.close();
  
  return true;
}