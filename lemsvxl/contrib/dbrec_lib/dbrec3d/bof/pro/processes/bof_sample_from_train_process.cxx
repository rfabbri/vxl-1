//:
// \brief  A process that samples a fraction of the total training samples. It also samples k- means for each block
// \file
// \author Isabel Restrepo
// \date 2-Apr-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <dbrec3d/bof/bof_util.h>

//:global variables
namespace bof_sample_from_train_process_globals 
{
  const unsigned n_inputs_ = 7;
  const unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bof_sample_from_train_process_cons(bprb_func_process& pro)
{
  using namespace bof_sample_from_train_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i =0;
  input_types_[i++] = "vcl_string" ; //bag of featutes info file
  input_types_[i++] = "int"; //scene_id (this can be confirmed in xml file pca_info.xml)
  input_types_[i++] = "int";   //block Indeces
  input_types_[i++] = "int";
  input_types_[i++] = "int";
  input_types_[i++] = "double"; //fraction to sample
  input_types_[i++] = "vcl_string"; //path to file to hold features of this block

  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bof_sample_from_train_process(bprb_func_process& pro)
{
  using namespace bof_sample_from_train_process_globals;
  
  //get inputs
  unsigned i = 0;
  vcl_string bof_file = pro.get_input<vcl_string>(i++);
  int scene_id = pro.get_input<int>(i++);
  int block_i = pro.get_input<int>(i++);
  int block_j = pro.get_input<int>(i++);
  int block_k = pro.get_input<int>(i++);
  double fraction = pro.get_input<double>(i++);
  vcl_string features_file = pro.get_input<vcl_string>(i++);  
    
  bof_util util(bof_file);
  
  vcl_vector<vnl_vector_fixed<double,10> > features;
  
  util.sample_from_train(scene_id, block_i, block_j, block_k, fraction, features);
  
  vcl_cout << "Number of sampled features:" << features.size() << "\n";

  //Warning: Platform dependent binary write
  //vcl_ofstream ofs(features_file.c_str(), ios::out | ios::binary);
  vcl_ofstream ofs(features_file.c_str());
  ofs.precision(15);
  if(ofs.is_open())
    for (unsigned i =0; i<features.size(); i++) {
      ofs << features[i] << "\n";
    }
    
  else
    vcl_cerr << "Could not open features file \n";

  ofs.close();
  
  return true;
}