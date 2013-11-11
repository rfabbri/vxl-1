//:
// \brief A process to perform quatiatization given k-means and data
// \file
// \author Isabel Restrepo
// \date 5-Apr-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bof/bof_codebook.h>
#include <dbcll/dbcll_euclidean_cluster.h>

//:global variables
namespace bof_euclidean_cluster_quantization_process_globals 
{
  const unsigned n_inputs_ = 9;
  const unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bof_euclidean_cluster_quantization_process_cons(bprb_func_process& pro)
{
  using namespace bof_euclidean_cluster_quantization_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "vcl_string"; //path to bof_info_file
  input_types_[i++] = "vcl_string"; //path to the means
  input_types_[i++] = "bool"; // Training Data? Yes=True, false otherwise
  input_types_[i++] = "int"; //category id, use < 0 for all categories
  input_types_[i++] = "int";   //scene_id (this can be confirmed in xml file pca_info.xml)
  input_types_[i++] = "int";   //block Indeces
  input_types_[i++] = "int";
  input_types_[i++] = "int";
  input_types_[i++] = "vcl_string"; //output path to xml info file for clusters
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bof_euclidean_cluster_quantization_process(bprb_func_process& pro)
{
  using namespace bof_euclidean_cluster_quantization_process_globals;
  
  //get inputs
  unsigned i =0;
  vcl_string bof_path = pro.get_input<vcl_string>(i++);
  vcl_string mean_file = pro.get_input<vcl_string>(i++);
  bool is_train = pro.get_input<bool>(i++);
  int class_id = pro.get_input<int>(i++);
  int scene_id = pro.get_input<int>(i++);
  int block_i = pro.get_input<int>(i++);
  int block_j = pro.get_input<int>(i++);
  int block_k = pro.get_input<int>(i++);
  vcl_string cluster_out_file = pro.get_input<vcl_string>(i++);  
 
  //read means
  vcl_ifstream mean_ifs(mean_file.c_str());
  if(!mean_ifs.is_open()){
    vcl_cerr << "Error: Could not open: " << mean_file << "\n";
    return false;
  }
  
  vcl_vector<vnl_vector_fixed<double,10> > means;
  unsigned num_means;
  mean_ifs >> num_means;
  
  vcl_cout << "Parsing: " << num_means << " means \n";
  
  for(unsigned i=0; i<num_means; i++){
    vnl_vector_fixed<double,10> mean;
    mean_ifs >> mean;
    means.push_back(mean);
  }
  
  mean_ifs.close();
  
  bof_codebook_utils codebook_utils(bof_path);
  
  vcl_vector<dbcll_euclidean_cluster_light<10> >  all_clusters;
  codebook_utils.compute_euclidean_clusters(means, is_train, class_id, scene_id, block_i, block_j, block_k, all_clusters);
  
  dbcll_xml_write(all_clusters, cluster_out_file);
  
  return true;
}