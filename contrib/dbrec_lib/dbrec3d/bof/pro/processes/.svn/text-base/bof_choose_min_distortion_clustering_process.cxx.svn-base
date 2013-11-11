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

#include <vcl_limits.h>

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
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; //suffix of means to be added

  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "vcl_string"; // the index of the mean with that minimizes sse
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bof_choose_min_distortion_clustering_process(bprb_func_process& pro)
{
  using namespace bof_choose_min_distortion_clustering_process_globals;
  
  //get inputs
  vcl_string means_sfx = pro.get_input<vcl_string>(0);
  double min_sse = vcl_numeric_limits<double>::max();
  vcl_string best_cluster;
  for (vul_file_iterator fn= (means_sfx + "*.xml"); fn; ++fn) {
    vcl_cout << "Parsing file: " << fn() <<"\n";
    double sse  = bof_util::parse_variance_from_xml_cluster(fn());
    if(sse < min_sse){
      min_sse = sse;
      best_cluster = fn.filename();
    }
    vcl_cout << "sse: " << sse << "\n";
  }
    
  //store output
  vcl_string output =  vul_file::strip_extension(best_cluster);
  vcl_cout << " Lowest sse error file : " << output << vcl_endl;
  pro.set_output_val<vcl_string>(0,output);
  
  return true;
}