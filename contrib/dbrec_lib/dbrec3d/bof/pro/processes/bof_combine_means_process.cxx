//:
// \brief A process to combine the means found in separate datasets
// \file
// \author Isabel Restrepo
// \date 3-Apr-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bof/bof_util.h>

#include <vcl_sstream.h>

#include <vul/vul_file_iterator.h>

//:global variables
namespace bof_combine_means_process_globals 
{
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 1;
}


//:sets input and output types
bool bof_combine_means_process_cons(bprb_func_process& pro)
{
  using namespace bof_combine_means_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; //suffix of means to be added
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "bof_feature_vector_sptr" ;  //the vector of the combined means
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bof_combine_means_process(bprb_func_process& pro)
{
  using namespace bof_combine_means_process_globals;
  
  //get inputs
  vcl_string means_sfx = pro.get_input<vcl_string>(0);

  bof_feature_vector_sptr fv = new bof_feature_vector;
  fv->features_.clear();

  for (vul_file_iterator fn= (means_sfx + "*"); fn; ++fn) {

   
    vcl_ifstream mean_ifs(fn());
    if(!mean_ifs.is_open()){
      vcl_cerr << "Error: Could not open mean_ifs \n";
      return false;
    }
    
    unsigned num_means;
    mean_ifs >> num_means;
    
    vcl_cout << "In file: " << fn() << "Parsing: " << num_means << " means \n";
    
    for(unsigned i=0; i<num_means; i++){
      vnl_vector_fixed<double,10> mean;
      mean_ifs >> mean;
      fv->features_.push_back(mean);
    }
    
    mean_ifs.close();
    
  }
  
  
  
  vcl_cout << "Number of combined means: " << fv->features_.size() << " means \n";

  
  //store output
  pro.set_output_val<bof_feature_vector_sptr>(0, fv);
  
  return true;
}