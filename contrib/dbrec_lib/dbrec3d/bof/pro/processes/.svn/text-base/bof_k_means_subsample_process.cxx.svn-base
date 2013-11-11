//:
// \brief A process to run (fast) k-means on a subsample of the training features
// \file
// \author Isabel Restrepo
// \date 3-Apr-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bof/bof_codebook.h>

#include <vcl_iostream.h>

//:global variables
namespace bof_k_means_subsample_process_globals 
{
  const unsigned n_inputs_ = 5;
  const unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bof_k_means_subsample_process_cons(bprb_func_process& pro)
{
  using namespace bof_k_means_subsample_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "vcl_string"; //path to bof_info_file
  input_types_[i++] = "vcl_string"; //file to initial random means
  input_types_[i++] = "double"; //fraction of training samples to use as subsamples
  input_types_[i++] = "unsigned"; //maximum number of iterations
  input_types_[i++] = "vcl_string"; //file out for final converged means for this subsample 
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bof_k_means_subsample_process(bprb_func_process& pro)
{
  using namespace bof_k_means_subsample_process_globals;
  
  //get inputs
  unsigned i = 0;
  vcl_string bof_path = pro.get_input<vcl_string>(i++);
  vcl_string rnd_mean_file = pro.get_input<vcl_string>(i++);
  double subsample_fraction = pro.get_input<double>(i++);
  unsigned max_it = pro.get_input<unsigned>(i++);
  vcl_string mean_out_file = pro.get_input<vcl_string>(i++);

  //read the random initial means
  vcl_ifstream mean_ifs(rnd_mean_file.c_str());
  if(!mean_ifs.is_open()){
    vcl_cerr << "Error: Could not open mean_ifs \n";
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
  
  if(codebook_utils.fast_k_means_for_train_subsample(means, subsample_fraction, max_it)){
  
    //write means to file
    vcl_ofstream means_ofs(mean_out_file.c_str());
    means_ofs.precision(15);
    means_ofs << means.size() << "\n";
    if(means_ofs.is_open())
      for (unsigned i =0; i<means.size(); i++) {
        means_ofs << means[i] << "\n";
      }
    
    else
      vcl_cerr << "Could not open rnd_means file: " << mean_out_file << "\n";
    
    means_ofs.close();
    
  }
  
  
  
  return true;
}