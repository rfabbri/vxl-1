// This is lemsvxlsrc/contrib/dbrec_lib/dbrec/pro/processes/dbrec_image_model_processes.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Processes to fit models
//
// \author Ozge Can Ozcanli
// \date May 06, 2009
//
// \verbatim
//  Modifications
//     none yet
// \endverbatim

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>
#include <dbrec/dbrec_part_sptr.h>
#include <dbrec/dbrec_part.h>
#include <dbrec/dbrec_image_learner_visitors.h>
#include <vil/vil_image_view.h>

//: Constructor
bool dbrec_construct_bg_op_models_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_hierarchy_sptr"); // h
  input_types.push_back("vcl_string"); // output directory to save the models
  input_types.push_back("vil_image_view_base_sptr");      // bg model mean img to construct response models, float img with values in [0,1] range
  input_types.push_back("vil_image_view_base_sptr");      // bg model std dev img to construct response models, float img with values in [0,1] range
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dbrec_construct_bg_op_models_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 4) {
    vcl_cerr << "brec_construct_bg_op_models_process - invalid inputs\n";
    return false;
  }

  // get the inputs
  unsigned i=0;
  dbrec_hierarchy_sptr h = pro.get_input<dbrec_hierarchy_sptr>(i++);
  vcl_string output_dir = pro.get_input<vcl_string>(i++);
  vil_image_view_base_sptr inp_mean = pro.get_input<vil_image_view_base_sptr>(i++);

  if (inp_mean->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;

  vil_image_view_base_sptr inp_std_dev = pro.get_input<vil_image_view_base_sptr>(i++);

  if (inp_std_dev->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;

  vil_image_view<float> mean_img(inp_mean);
  vil_image_view<float> std_dev_img(inp_std_dev);

  dbrec_image_construct_bg_model_visitor bgmv(mean_img, std_dev_img, output_dir);
  for (unsigned i = 0; i < h->class_cnt(); i++) {
    h->root(i)->accept(&bgmv);
  }

  return true;
}

//: Constructor
bool dbrec_construct_bg_op_models_rot_inv_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_hierarchy_sptr"); // h
  input_types.push_back("float"); // angle increments
  input_types.push_back("vcl_string"); // output directory to save the models
  input_types.push_back("vil_image_view_base_sptr");      // bg model mean img to construct response models, float img with values in [0,1] range
  input_types.push_back("vil_image_view_base_sptr");      // bg model std dev img to construct response models, float img with values in [0,1] range
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dbrec_construct_bg_op_models_rot_inv_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 5) {
    vcl_cerr << "brec_construct_bg_op_models_process - invalid inputs\n";
    return false;
  }
  unsigned i=0;
  dbrec_hierarchy_sptr h = pro.get_input<dbrec_hierarchy_sptr>(i++);
  float angle_inc = pro.get_input<float>(i++);
  vcl_string output_dir = pro.get_input<vcl_string>(i++);
  vil_image_view_base_sptr inp_mean = pro.get_input<vil_image_view_base_sptr>(i++);

  if (inp_mean->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;

  vil_image_view_base_sptr inp_std_dev = pro.get_input<vil_image_view_base_sptr>(i++);

  if (inp_std_dev->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;

  vil_image_view<float> mean_img(inp_mean);
  vil_image_view<float> std_dev_img(inp_std_dev);

  dbrec_image_construct_bg_model_rot_inv_visitor bgmv(mean_img, std_dev_img, angle_inc, output_dir);
  for (unsigned i = 0; i < h->class_cnt(); i++) {
    h->root(i)->accept(&bgmv);
  }

  return true;
}
