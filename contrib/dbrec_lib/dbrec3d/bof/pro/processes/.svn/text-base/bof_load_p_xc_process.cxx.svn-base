//:
// \brief A process to load a joint probability P(X,C) between class labels and keypoints
// \file
// \author Isabel Restrepo
// \date 11-Apr-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bof/bof_labels_keypoint_joint.h>
#include <bof/bof_codebook.h>

//:global variables
namespace bof_load_p_xc_process_globals 
{
  const unsigned n_inputs_ = 3;
  const unsigned n_outputs_ = 1;
}


//:sets input and output types
bool bof_load_p_xc_process_cons(bprb_func_process& pro)
{
  using namespace bof_load_p_xc_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "bof_codebook_sptr" ;  //class containing the means
  input_types_[i++] = "vcl_string"; // path to xml files containing the clusters for each classes
  input_types_[i++] = "unsigned"; //number of categories
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "bof_p_xc_sptr" ;
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bof_load_p_xc_process(bprb_func_process& pro)
{
  using namespace bof_load_p_xc_process_globals;
  
  //get inputs
  unsigned i = 0;
  bof_codebook_sptr codebook = pro.get_input<bof_codebook_sptr>(i++);
  vcl_string xml_path = pro.get_input<vcl_string>(i++);
  unsigned ncategories = pro.get_input<unsigned>(i++);
  
  bof_labels_keypoint_joint *p_cx = new bof_labels_keypoint_joint(ncategories, codebook->means_.size());
  p_cx->xml_read(xml_path, codebook->means_);
  
  //store output
  pro.set_output_val<bof_p_xc_sptr>(0, p_cx);
  
  return true;
}