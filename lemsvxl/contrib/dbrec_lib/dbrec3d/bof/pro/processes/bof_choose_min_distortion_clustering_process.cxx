//:
// \brief A process to read several clustering options and pick the set of means that minimize the sse
// \file
// \author Isabel Restrepo
// \date 4-Apr-2011
//
//  This process conresponds to step 4 in the algorith proposed by
//  Bradley P., Fayyad U. Refining inittial points for K-Means Clustering
//

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <limits>

#include <bof/bof_util.h>

#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>

//:global variables
namespace bof_choose_min_distortion_clustering_process_globals 
{
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 1;
}


//:sets input and output types
bool bof_choose_min_distortion_clustering_process_cons(bprb_func_process& pro)
{
  using namespace bof_choose_min_distortion_clustering_process_globals ;
  
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = vcl_string"; //suffix of means to be added

  
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = vcl_string"; // the index of the mean with that minimizes sse
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bof_choose_min_distortion_clustering_process(bprb_func_process& pro)
{
  using namespace bof_choose_min_distortion_clustering_process_globals;
  
  //get inputs
  std::string means_sfx = pro.get_input<std::string>(0);
  double min_sse = std::numeric_limits<double>::max();
  std::string best_cluster;
  for (vul_file_iterator fn= (means_sfx + "*.xml"); fn; ++fn) {
    std::cout << "Parsing file: " << fn() <<"\n";
    double sse  = bof_util::parse_variance_from_xml_cluster(fn());
    if(sse < min_sse){
      min_sse = sse;
      best_cluster = fn.filename();
    }
    std::cout << "sse: " << sse << "\n";
  }
    
  //store output
  std::string output =  vul_file::strip_extension(best_cluster);
  std::cout << " Lowest sse error file : " << output << std::endl;
  pro.set_output_val<std::string>(0,output);
  
  return true;
}