//:
// \brief A process to learn the joint distribution of class and keypoints for a given scene
// \file
// \author Isabel Restrepo
// \date 8-Apr-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bof/bof_scene_categories.h>

//:global variables
namespace bof_learn_categories_process_globals 
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bof_learn_categories_process_cons(bprb_func_process& pro)
{
  using namespace bof_learn_categories_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bof_codebook_sptr" ;  //class containing the means
  input_types_[1] = "int";  //id of scene to process. if scene is not label for training. it won't be processed
  input_types_[2] = "vcl_string"; // bof_path, where bof_info.xml and bof_category_info.xml are
  input_types_[3] = "vcl_string"; //path to save the results for this scene
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bof_learn_categories_process(bprb_func_process& pro)
{
  using namespace bof_learn_categories_process_globals;
  
  //get inputs
  bof_codebook_sptr codebook = pro.get_input<bof_codebook_sptr>(0);
  int scene_id = pro.get_input<int>(1);
  vcl_string bof_dir = pro.get_input<vcl_string>(2);
  vcl_string path_out = pro.get_input<vcl_string>(3);

  bof_scene_categories categories(bof_dir);
  
  bof_labels_keypoint_joint p_cx(categories.nclasses(), codebook->means_.size());
  
  categories.learn_categories(codebook, scene_id, p_cx, path_out);
  p_cx.xml_write(path_out);

  return true;
}