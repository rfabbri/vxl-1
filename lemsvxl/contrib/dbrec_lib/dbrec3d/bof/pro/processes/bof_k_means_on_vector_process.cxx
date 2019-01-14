//:
// \brief A process to compute k-means on a set of pre-computed features
// \file
// \author Isabel Restrepo
// \date 4-Apr-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bof/bof_util.h>

#include <dbcll/dbcll_k_means.h>
#include <dbcll/dbcll_euclidean_cluster_light.h>

#include <fstream>
#include <vul/vul_file.h>

//:global variables
namespace bof_k_means_on_vector_process_globals 
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bof_k_means_on_vector_process_cons(bprb_func_process& pro)
{
  using namespace bof_k_means_on_vector_process_globals ;
  
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = vcl_string"; //path to means
  input_types_[1] = "bof_feature_vector_sptr"; //features to cluster
  input_types_[2] = "unsigned"; //maximum number or iterations
  input_types_[3] = vcl_string"; //file path to save updated (after convergance means)
  
  
  std::vector<std::string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bof_k_means_on_vector_process(bprb_func_process& pro)
{
  using namespace bof_k_means_on_vector_process_globals;
  
  //get inputs
  std::string CM_i_path = pro.get_input<std::string>(0);
  bof_feature_vector_sptr CM = pro.get_input<bof_feature_vector_sptr>(1);
  unsigned max_it = pro.get_input<unsigned>(2);
  std::string FM_i_path = pro.get_input<std::string>(3);
  
  
  //read the initial means
  std::ifstream mean_ifs(CM_i_path.c_str());
  if(!mean_ifs.is_open()){
    std::cerr << "Error: Could not open mean_ifs: " << CM_i_path <<  "\n";
    return false;
  }
  
  std::vector<vnl_vector_fixed<double,10> > means;
  unsigned num_means;
  mean_ifs >> num_means;
  
  std::cout << "Parsing: " << num_means << " means \n";
  
  for(unsigned i=0; i<num_means; i++){
    vnl_vector_fixed<double,10> mean;
    mean_ifs >> mean;
    means.push_back(mean);
  }
  
  mean_ifs.close();
  
  
  std::vector<std::vector<unsigned> > clusters;
  unsigned n_iterations = dbcll_fast_k_means(CM->features_, clusters, means, max_it);
  std::cout <<" Number of iterationsfor fast-k means is: " << n_iterations << std::endl;
  
  std::vector<dbcll_euclidean_cluster_light<10> > all_clusters;
  dbcll_init_euclidean_clusters(CM->features_, clusters, means, all_clusters);
  
  std::cout << "Means size(): " << means.size() << "\n";
  std::cout << "Points size(): " << CM->features_.size() << "\n";
  std::cout << "Clusters size(): " << clusters.size() << "\n";
  
  std::string info_path = (vul_file::strip_extension(FM_i_path)) + "_info.xml";
  
  std::cout <<" Writing cluster info to file : " << info_path << std::endl;
  dbcll_xml_write(all_clusters, info_path);
  
  //write new means to file
  std::ofstream means_ofs(FM_i_path.c_str());
  means_ofs.precision(15);
  means_ofs << means.size() << "\n";
  if(means_ofs.is_open())
    for (unsigned i =0; i<means.size(); i++) {
      means_ofs << means[i] << "\n";
    }
  
  else
    std::cerr << "Could not open file: " << FM_i_path << "\n";
  
  means_ofs.close();
  
  
  return true;
}