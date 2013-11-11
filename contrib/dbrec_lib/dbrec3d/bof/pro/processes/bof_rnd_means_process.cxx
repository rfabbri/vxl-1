//:
// \brief: A process to sample k means from all available training data
// \file
// \author Isabel Restrepo
// \date 2-Apr-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bof/bof_codebook.h>

//:global variables
namespace bof_rnd_means_process_globals 
{
  const unsigned n_inputs_ = 3;
  const unsigned n_outputs_ = 0 ;
}


//:sets input and output types
bool bof_rnd_means_process_cons(bprb_func_process& pro)
{
  using namespace bof_rnd_means_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; //bof_path
  input_types_[1] = "int"; //number of means
  input_types_[2] = "vcl_string"; //file to save the random means
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bof_rnd_means_process(bprb_func_process& pro)
{
  using namespace bof_rnd_means_process_globals;
  
  //get inputs
  vcl_string bof_path = pro.get_input<vcl_string>(0);
  int nmeans = pro.get_input<int>(1);
  vcl_string file_out = pro.get_input<vcl_string>(2);

  bof_codebook_utils codebook_utils(bof_path);
  vcl_vector<vnl_vector_fixed<double,10> > rnd_means;
  codebook_utils.sample_rnd_k_means(rnd_means, nmeans);
  
  //write means to file
  vcl_ofstream ofs(file_out.c_str());
  ofs.precision(15);
  if(ofs.is_open()){
    ofs << rnd_means.size() << "\n";
    for (unsigned i =0; i<rnd_means.size(); i++) {
      ofs << rnd_means[i] << "\n";
    }
  }  
  else
    vcl_cerr << "Could not open rnd_means file: " << file_out << "\n";
  
  ofs.close();
  
  
  return true;
}