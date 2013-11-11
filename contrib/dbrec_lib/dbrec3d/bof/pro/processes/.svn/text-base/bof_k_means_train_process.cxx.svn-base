//:
// \brief Perform k-means on the training samples of a scene block
// \file
// \author Isabel Restrepo
// \date 4-Apr-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bof/bof_codebook.h>

#include <vcl_iostream.h>

//:global variables
namespace bof_k_means_train_process_globals 
{
  const unsigned n_inputs_ = 8;
  const unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bof_k_means_train_process_cons(bprb_func_process& pro)
{
  using namespace bof_k_means_train_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "vcl_string"; //path to bof_info_file
  input_types_[i++] = "vcl_string"; //file to initial means (refined using algorithm from Bradley98)
  input_types_[i++] = "int";   //scene_id (this can be confirmed in xml file pca_info.xml)
  input_types_[i++] = "int";   //block Indeces
  input_types_[i++] = "int";
  input_types_[i++] = "int";
  input_types_[i++] = "unsigned";   //maximum number of iterations
  input_types_[i++] = "vcl_string"; //file out for updated means for this block (after k-means)
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bof_k_means_train_process(bprb_func_process& pro)
{
  using namespace bof_k_means_train_process_globals;
  
  //get inputs
  unsigned i = 0;
  vcl_string bof_path = pro.get_input<vcl_string>(i++);
  vcl_string init_mean_file = pro.get_input<vcl_string>(i++);
  int scene_id = pro.get_input<int>(i++);
  int block_i = pro.get_input<int>(i++);
  int block_j = pro.get_input<int>(i++);
  int block_k = pro.get_input<int>(i++);
  unsigned max_it = pro.get_input<unsigned>(i++);
  vcl_string mean_out_file = pro.get_input<vcl_string>(i++);

 
  //read initial means
  vcl_ifstream mean_ifs(init_mean_file.c_str());
  if(!mean_ifs.is_open()){
    vcl_cerr << "Error: Could not open: " << init_mean_file << "\n";
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
  
  if (codebook_utils.fast_k_means_on_train_samples(means, scene_id, block_i, block_j, block_k, max_it))
  {
    //write means to file
    vcl_ofstream means_ofs(mean_out_file.c_str());
    means_ofs.precision(15);
    means_ofs << means.size() << "\n";
    if(means_ofs.is_open())
      for (unsigned i =0; i<means.size(); i++) {
        means_ofs << means[i] << "\n";
      }
    
    else{
      vcl_cerr << "Could not open file: " << mean_out_file << "\n";
      return false;
    }
    
    means_ofs.close();
  }
    
  return true;
}