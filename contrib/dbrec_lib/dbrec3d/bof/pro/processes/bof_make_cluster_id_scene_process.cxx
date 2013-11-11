//:
// \brief
// \file
// \author Isabel Restrepo
// \date 19-Sep-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bof/bof_codebook.h>



//:global variables
namespace bof_make_cluster_id_scene_process_globals 
{
  const unsigned n_inputs_ = 6;
  const unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bof_make_cluster_id_scene_process_cons(bprb_func_process& pro)
{
  using namespace bof_make_cluster_id_scene_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "vcl_string"; //path to bof_info_file
  input_types_[i++] = "bof_codebook_sptr" ;  //class containing the means
  input_types_[i++] = "int";   //scene_id 
  input_types_[i++] = "int";   //block Indeces
  input_types_[i++] = "int";
  input_types_[i++] = "int";
  
  vcl_vector<vcl_string> output_types_(n_outputs_);

  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bof_make_cluster_id_scene_process(bprb_func_process& pro)
{
  using namespace bof_make_cluster_id_scene_process_globals;
  
  //get inputs
  unsigned i = 0;
  vcl_string bof_path = pro.get_input<vcl_string>(i++);
  bof_codebook_sptr codebook = pro.get_input<bof_codebook_sptr>(i++);
  int scene_id = pro.get_input<int>(i++);
  int block_i = pro.get_input<int>(i++);
  int block_j = pro.get_input<int>(i++);
  int block_k = pro.get_input<int>(i++);
  
  
  bof_codebook_utils codebook_utils(bof_path);
  codebook_utils.assign_cluster_id(codebook->means_, scene_id, block_i, block_j, block_k);
 
  
  return true;
}