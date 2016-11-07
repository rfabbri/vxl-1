//:
// \brief
// \file
// \author Isabel Restrepo
// \date 11-Apr-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bof/bof_scene_categories.h>

//:global variables
namespace bof_classify_process_globals 
{
  const unsigned n_inputs_ = 5;
  const unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bof_classify_process_cons(bprb_func_process& pro)
{
  using namespace bof_classify_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "bof_codebook_sptr" ;  //class containing the means
  input_types_[i++] = "bof_p_xc_sptr";  // the class keypoint joint distribution
  input_types_[i++] = "int";  //id of scene to process. if scene is not label for training. it won't be processed
  input_types_[i++] = "vcl_string"; // bof_path, where bof_info.xml and bof_category_info.xml are
  input_types_[i++] = "vcl_string"; //path to save the results for this scene
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bof_classify_process(bprb_func_process& pro)
{
  using namespace bof_classify_process_globals;
  
  //get inputs
  unsigned i = 0;
  bof_codebook_sptr codebook = pro.get_input<bof_codebook_sptr>(i++);
  bof_p_xc_sptr p_xc = pro.get_input<bof_p_xc_sptr>(i++);
  int scene_id = pro.get_input<int>(i++);
  vcl_string bof_dir = pro.get_input<vcl_string>(i++);
  vcl_string path_out = pro.get_input<vcl_string>(i++);
  
  bof_scene_categories categories(bof_dir);
  
  categories.classify(codebook, scene_id, *p_xc, path_out);
  
  return true;
}