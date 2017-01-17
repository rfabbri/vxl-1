// This is lemsvxlsrc/contrib/dbrec_lib/dbrec/pro/processes/dbrec_image_rot_inv_processes.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Processes that use rotationally invariant parts to parse images and learn part hiearachies to do so
//
// \author Ozge Can Ozcanli
// \date Nov 16, 2009
//
// \verbatim
//  Modifications
//   none yet
// \endverbatim

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <dbrec/dbrec_part.h>
#include <dbrec/dbrec_image_visitors.h>
#include <dbrec/dbrec_image_learner_visitors.h>
#include <dbrec/dbrec_visitor_sptr.h>
#include <dbrec/dbrec_part_context.h>

#include <vil/vil_new.h>
#include <vil/vil_convert.h>
#include <vil/vil_save.h>

#include <vnl/vnl_random.h>
#include <dbrec/dbrec_image_pairwise_models.h>

//: Constructor
// create hierarchy process may use the builder class or create an empty one to be passed to training processes to learn the hierarchy
bool dbrec_sample_rot_inv_parts_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_hierarchy_sptr");      // hierarchy
  input_types.push_back("int");                   // type id of the composition in the hierarchy to sample from, it could also be one of the root nodes
  input_types.push_back("int");  // ni , dimensions of the output image
  input_types.push_back("int");  // nj
  input_types.push_back("float");  // mean strength,  the strength of the output part will be sampled from a normal distribution with this mean and variance, it will be truncated to [0,1]
  input_types.push_back("float");  // strength variance
  input_types.push_back("int");  // pass 1 for a white background image and 0 for a black background image
  input_types.push_back("int");  // pass 1 to sample a rotation angle in [0,359] and rotate the sample, otherwise sample is drawn to the image with no rotation
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_sample_rot_inv_parts_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 7) {
    vcl_cerr << "dbrec_sample_rot_inv_parts_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_hierarchy_sptr h = pro.get_input<dbrec_hierarchy_sptr>(i++);
  int composition_type_id = pro.get_input<int>(i++);
  int ni = pro.get_input<int>(i++); int nj = pro.get_input<int>(i++);
  float mean_str = pro.get_input<float>(i++); float var_str = pro.get_input<float>(i++);
  double std_str = vcl_sqrt(var_str);

  bool white_bg = pro.get_input<int>(i++) == 1 ? true : false;

  bool rotate = pro.get_input<int>(i++) == 1 ? true : false;
  
  dbrec_part_sptr p = h->get_part(composition_type_id);

  vil_image_view<vxl_byte> img(ni, nj, 1);  
  if (white_bg) img.fill(255); else img.fill(0);

  //: use the global rng of the type id generator
  dbrec_type_id_factory* ins = dbrec_type_id_factory::instance();

  vnl_random& rng = ins->rng();
  int angle = 0;
  if (rotate)
    angle = rng.lrand32(0, 359);
  vcl_cout << "sampled angle: " << angle << " for rotation!\n";
  double str = rng.normal(); // samples a normally distributed random value with mean 0 and variance 1
  str = std_str*str + mean_str;    // gives a normally distributed random value with mean mean_str and variance var_str
  str = str > 1.0 ? 1.0 : str;
  str = str < 0.0 ? 0.0 : str;
  vcl_cout << "sampled str: " << str << " for strength!\n";
  dbrec_sample_and_draw_part_visitor sdpv(img, ni/2, nj/2, angle, (float)str, rng);
  p->accept(&sdpv);

  vil_image_view_base_sptr img_sptr = new vil_image_view<vxl_byte>(img);
  pro.set_output_val<vil_image_view_base_sptr>(0, img_sptr);

  return true;
}

//: Constructor
bool dbrec_image_parse_rot_inv_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");      // input image
  input_types.push_back("vil_image_view_base_sptr");      // image mask to indicate valid regions in the image to create instances in the output context
  input_types.push_back("dbrec_hierarchy_sptr");
  input_types.push_back("dbrec_context_factory_sptr");      // if a context factory is available for the image pass it if you want to re-use previous parses of the parts which exist in this context factory
                                                            // otherwise call the initialization method of the process which sets this input to zero
  input_types.push_back("float");      // class prior for primitive parts' posterior calculations
  input_types.push_back("float");    // the increments in the angle for rotational extrema operator (picks best orientation for each pixel)
  input_types.push_back("vcl_string");  // model path which contains fg appearance model parameters
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("dbrec_context_factory_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_image_parse_rot_inv_process_init(bprb_func_process& pro)
{
  dbrec_context_factory_sptr cf; // initialize to zero
  pro.set_input(3, new brdb_value_t<dbrec_context_factory_sptr>(cf));
  return true;
}
bool dbrec_image_parse_rot_inv_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 7) {
    vcl_cerr << "dbrec_image_parse_rot_inv_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  vil_image_view_base_sptr inp_img = pro.get_input<vil_image_view_base_sptr>(i++);
  if (inp_img->pixel_format() != VIL_PIXEL_FORMAT_BYTE)
    return false;

  vil_image_view<vxl_byte> orig_img(inp_img);
  if (orig_img.nplanes() == 3) {
    vcl_cout << "In dbrec_image_parse_rot_inv_process() -- converting input RGB image to grey scale!\n";
    vil_image_view<vxl_byte> *out_img = new vil_image_view<vxl_byte>(inp_img->ni(),inp_img->nj());
    vil_convert_planes_to_grey<vxl_byte, vxl_byte>(*(inp_img.as_pointer()),*out_img);
    inp_img = out_img;
    vil_save(*inp_img, "./test_grey.png");
  }
  
  vil_image_view<float> img = *vil_convert_cast(float(), inp_img);
  vil_math_scale_values(img,1.0/255.0);

  vil_image_resource_sptr input_sptr = vil_new_image_resource_of_view(img);

  vil_image_view_base_sptr mask_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  if (!mask_sptr) {  // initialized to zero, set it to all true
    vil_image_view<bool> tmp(img.ni(), img.nj()); tmp.fill(true);
    mask_sptr = new vil_image_view<bool>(tmp);
  }
  if (mask_sptr->pixel_format() != VIL_PIXEL_FORMAT_BOOL) {
    vcl_cout << "In dbrec_image_parse_rot_inv_process() - the valid regions mask passed to the process is not a BOOL image!\n";
    return false;
  }
  vil_image_view<bool> mask(mask_sptr);

  dbrec_hierarchy_sptr h = pro.get_input<dbrec_hierarchy_sptr>(i++);
  dbrec_context_factory_sptr cf = pro.get_input<dbrec_context_factory_sptr>(i++);
  
  float class_prior = pro.get_input<float>(i++);
  float theta_inc = pro.get_input<float>(i++);
  vcl_string model_path = pro.get_input<vcl_string>(i++);

  if (!cf) {
    dbrec_parse_image_rot_inv_visitor pv(h, input_sptr, mask, class_prior, theta_inc, model_path);
    for (unsigned i = 0; i < h->class_cnt(); i++) {
      h->root(i)->accept(&pv);
    }
    pro.set_output_val<dbrec_context_factory_sptr>(0, pv.context_factory());
  } else {
    dbrec_parse_image_rot_inv_visitor pv(h, cf, input_sptr, mask, class_prior, theta_inc, model_path);
    for (unsigned i = 0; i < h->class_cnt(); i++) {
      h->root(i)->accept(&pv);
    }
    pro.set_output_val<dbrec_context_factory_sptr>(0, pv.context_factory());
  }
  return true;
}

//: Constructor
bool dbrec_image_parse_rot_inv_with_fg_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");      // input image
  input_types.push_back("vil_image_view_base_sptr");      // image mask to indicate valid regions in the image to create instances in the output context
  input_types.push_back("dbrec_hierarchy_sptr");
  input_types.push_back("dbrec_context_factory_sptr");      // if a context factory is available for the image pass it if you want to re-use previous parses of the parts which exist in this context factory
                                                            // otherwise call the initialization method of the process which sets this input to zero
  input_types.push_back("vil_image_view_base_sptr");      // fg map from the bakcground model for the input img
  input_types.push_back("float");      // class prior for primitive parts' posterior calculations
  input_types.push_back("float");    // the increments in the angle for rotational extrema operator (picks best orientation for each pixel)
  input_types.push_back("vcl_string");  // model path which contains fg appearance model parameters
  input_types.push_back("vcl_string");  // model path for background image mean and std dev of operator responses
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  output_types.push_back("dbrec_context_factory_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_image_parse_rot_inv_with_fg_process_init(bprb_func_process& pro)
{
  dbrec_context_factory_sptr cf; // initialize to zero
  pro.set_input(3, new brdb_value_t<dbrec_context_factory_sptr>(cf));
  return true;
}
bool dbrec_image_parse_rot_inv_with_fg_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 9) {
    vcl_cerr << "dbrec_image_parse_rot_inv_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  vil_image_view_base_sptr inp_img = pro.get_input<vil_image_view_base_sptr>(i++);
  if (inp_img->pixel_format() != VIL_PIXEL_FORMAT_BYTE)
    return false;

  vil_image_view<vxl_byte> orig_img(inp_img);
  if (orig_img.nplanes() == 3) {
    vcl_cout << "In dbrec_image_parse_rot_inv_process() -- converting input RGB image to grey scale!\n";
    vil_image_view<vxl_byte> *out_img = new vil_image_view<vxl_byte>(inp_img->ni(),inp_img->nj());
    vil_convert_planes_to_grey<vxl_byte, vxl_byte>(*(inp_img.as_pointer()),*out_img);
    inp_img = out_img;
    vil_save(*inp_img, "./test_grey.png");
  }
  
  vil_image_view<float> img = *vil_convert_cast(float(), inp_img);
  vil_math_scale_values(img,1.0/255.0);

  vil_image_resource_sptr input_sptr = vil_new_image_resource_of_view(img);

  vil_image_view_base_sptr mask_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  if (!mask_sptr) {  // initialized to zero, set it to all true
    vil_image_view<bool> tmp(img.ni(), img.nj()); tmp.fill(true);
    mask_sptr = new vil_image_view<bool>(tmp);
  }
  if (mask_sptr->pixel_format() != VIL_PIXEL_FORMAT_BOOL) {
    vcl_cout << "In dbrec_image_parse_rot_inv_process() - the valid regions mask passed to the process is not a BOOL image!\n";
    return false;
  }
  vil_image_view<bool> mask(mask_sptr);

  dbrec_hierarchy_sptr h = pro.get_input<dbrec_hierarchy_sptr>(i++);
  dbrec_context_factory_sptr cf = pro.get_input<dbrec_context_factory_sptr>(i++);

  vil_image_view_base_sptr fg_map_sptr = pro.get_input<vil_image_view_base_sptr>(i++);

  if (fg_map_sptr->pixel_format() == VIL_PIXEL_FORMAT_BOOL) { 
    vcl_cout << "In dbrec_image_parse_with_fg_process() - passed the ground truth map as the prob map so casting BOOL img to FLOAT img, assuming true pixels have prob 1.0\n";
    fg_map_sptr = vil_convert_cast(float(), fg_map_sptr);
  } else if (fg_map_sptr->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;
  vil_image_view<float> fg_map(fg_map_sptr);
  
  float class_prior = pro.get_input<float>(i++);
  float theta_inc = pro.get_input<float>(i++);
  vcl_string fg_model_path = pro.get_input<vcl_string>(i++);
  vcl_string bg_model_path = pro.get_input<vcl_string>(i++);

  if (!cf) {
    dbrec_parse_image_rot_inv_with_fg_map_visitor pv(h, input_sptr, mask, fg_map, class_prior, theta_inc, fg_model_path, bg_model_path);
    for (unsigned i = 0; i < h->class_cnt(); i++) {
      h->root(i)->accept(&pv);
    }
    pro.set_output_val<dbrec_context_factory_sptr>(0, pv.context_factory());
  } else {
    dbrec_parse_image_rot_inv_with_fg_map_visitor pv(h, cf, input_sptr, mask, fg_map, class_prior, theta_inc, fg_model_path, bg_model_path);
    for (unsigned i = 0; i < h->class_cnt(); i++) {
      h->root(i)->accept(&pv);
    }
    pro.set_output_val<dbrec_context_factory_sptr>(0, pv.context_factory());
  }
  return true;
}

//: Weibull model learners for rot invariant primitives
bool dbrec_image_rot_inv_weibull_model_learner_init_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_hierarchy_sptr");      
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  output_types.push_back("dbrec_visitor_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_image_rot_inv_weibull_model_learner_init_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 1) {
    vcl_cerr << "dbrec_image_parse_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_hierarchy_sptr h = pro.get_input<dbrec_hierarchy_sptr>(i++);
  dbrec_visitor_sptr lv = new dbrec_rot_inv_gaussian_weibull_model_learner_visitor(h);
  pro.set_output_val<dbrec_visitor_sptr>(0, lv);
  return true;
}
bool dbrec_image_rot_inv_weibull_model_update_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_visitor_sptr");      
  input_types.push_back("dbrec_context_factory_sptr");    // current parse of the image
  input_types.push_back("vil_image_view_base_sptr");      // fg map (values in range [0,1] where 1 denotes foreground) or BOOL image as a ground truth map 
  input_types.push_back("vil_image_view_base_sptr");      // image mask to indicate valid regions in the image to collect stats
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_image_rot_inv_weibull_model_update_process_init(bprb_func_process& pro)
{
  vil_image_view_base_sptr mask_ptr; // initialize to zero
  pro.set_input(3, new brdb_value_t<vil_image_view_base_sptr>(mask_ptr));
  return true;
}

bool dbrec_image_rot_inv_weibull_model_update_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 4) {
    vcl_cerr << "dbrec_image_rot_inv_weibull_model_update_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_visitor_sptr v = pro.get_input<dbrec_visitor_sptr>(i++);
  dbrec_rot_inv_gaussian_weibull_model_learner_visitor* lv = dynamic_cast<dbrec_rot_inv_gaussian_weibull_model_learner_visitor*>(v.ptr());
 
  if (!lv) {
    vcl_cerr << "dbrec_image_rot_inv_weibull_model_update_process - cannot cast input pointer!\n";
    return false;
  }
  dbrec_context_factory_sptr cf = pro.get_input<dbrec_context_factory_sptr>(i++);

  vil_image_view_base_sptr fg_map_sptr = pro.get_input<vil_image_view_base_sptr>(i++);

  if (fg_map_sptr->pixel_format() == VIL_PIXEL_FORMAT_BOOL) {
    fg_map_sptr = vil_convert_cast(float(), fg_map_sptr);

    vil_image_view<float> fg_map_temp(fg_map_sptr);
    vil_image_view<vxl_byte> temp(fg_map_sptr->ni(), fg_map_sptr->nj());
    vil_convert_stretch_range_limited(fg_map_temp, temp, 0.0f, 1.0f);
    vil_save(temp, "./using_this_mask.png");

  } else if (fg_map_sptr->pixel_format() != VIL_PIXEL_FORMAT_FLOAT) {
    vcl_cout << "In dbrec_image_rot_inv_weibull_model_update_process() -- the obj/fg map is neither a BOOL nor a float img!\n";
    return false;
  }
  vil_image_view<float> fg_map(fg_map_sptr);

  vil_image_view_base_sptr mask_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  if (!mask_sptr) {  // initialized to zero, set it to all true
    vil_image_view<bool> tmp(fg_map.ni(), fg_map.nj()); tmp.fill(true);
    mask_sptr = new vil_image_view<bool>(tmp);
  }
  if (mask_sptr->pixel_format() != VIL_PIXEL_FORMAT_BOOL) {
    vcl_cout << "In dbrec_image_rot_inv_weibull_model_update_process() - the valid regions mask passed to the process is not a BOOL image!\n";
    return false;
  }
  vil_image_view<bool> mask(mask_sptr);

  lv->set_data(cf, fg_map, mask);

  dbrec_hierarchy_sptr h = lv->h();
  for (unsigned i = 0; i < h->class_cnt(); i++) {
    h->root(i)->accept(lv);
  }

  return true;
}
bool dbrec_image_rot_inv_weibull_model_learner_print_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_visitor_sptr");      
  input_types.push_back("vcl_string");                    // path to output current histograms and models in the learners
  input_types.push_back("float");  // pass the angle increments of the experiment
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_image_rot_inv_weibull_model_learner_print_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 3) {
    vcl_cerr << "dbrec_image_weibull_model_learner_print_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_visitor_sptr v = pro.get_input<dbrec_visitor_sptr>(i++);
  dbrec_rot_inv_gaussian_weibull_model_learner_visitor* lv = dynamic_cast<dbrec_rot_inv_gaussian_weibull_model_learner_visitor*>(v.ptr());
  vcl_string path = pro.get_input<vcl_string>(i++);
  float angle_inc = pro.get_input<float>(i++);
  lv->print_current_histograms(path);
  lv->print_current_models_at_each_orientation(path, angle_inc);
  return true;
}

//: Processes for learning and stat collection

//: Constructor
bool dbrec_image_collect_stats_rot_inv_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_hierarchy_sptr");
  input_types.push_back("int");  // the depth of the parts in the hierarchy, that we'll collect pairwise stats from
  input_types.push_back("dbrec_context_factory_sptr");      // current parse of the image
  input_types.push_back("float"); // radius 
  input_types.push_back("int");  // class id: the id of the class that the training image belongs to
  input_types.push_back("vcl_string");  // path and prefix for the output binary file
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_image_collect_stats_rot_inv_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 6) {
    vcl_cerr << "dbrec_image_collect_stats_rot_inv_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_hierarchy_sptr h = pro.get_input<dbrec_hierarchy_sptr>(i++);
  int depth = pro.get_input<int>(i++);
  dbrec_context_factory_sptr cf = pro.get_input<dbrec_context_factory_sptr>(i++);
  float radius = pro.get_input<float>(i++);
  
  int class_id = pro.get_input<int>(i++);
  vcl_string stat_file_path = pro.get_input<vcl_string>(i++);

  vcl_vector<dbrec_part_sptr> parts;
  h->get_parts(depth, parts);

  vcl_stringstream class_id_str; class_id_str << class_id;
  for (unsigned i = 0; i < parts.size(); i++) {
    dbrec_part_sptr p1 = parts[i];
    vcl_stringstream p1_id_str; p1_id_str << p1->type();
    for (unsigned j = 0; j < parts.size(); j++) {
      dbrec_part_sptr p2 = parts[j];
      vcl_stringstream p2_id_str; p2_id_str << p2->type();
      vcl_string output_file = stat_file_path + "_class_" + class_id_str.str() + "_" + p1_id_str.str() + "_" + p2_id_str.str() + "_class_stats.txt";
      dbrec_pairwise_compositor::collect_rot_inv_stats(cf, 0, p1, p2, radius, output_file);
      output_file = stat_file_path + "_class_" + class_id_str.str() + "_" + p1_id_str.str() + "_" + p2_id_str.str() + "_non_class_stats.txt";
      dbrec_pairwise_compositor::collect_rot_inv_stats(cf, 3, p1, p2, radius, output_file);
    }
  }
  return true;
}


//: Constructor, trains parts using dbrec_pairwise_discrete_compositor
bool dbrec_image_train_rot_inv_composite_parts_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_hierarchy_sptr");
  input_types.push_back("int");  // depth of the compositional parts to be trained
  input_types.push_back("dbrec_context_factory_sptr");      // pass the context factory of the image
  input_types.push_back("int");  // class id of the training img
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_image_train_rot_inv_composite_parts_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 4) {
    vcl_cerr << "In dbrec_image_train_rot_inv_composite_parts_process - invalid inputs!!!!!!!!!!\n";
    return false;
  }
  unsigned i = 0;
  dbrec_hierarchy_sptr h = pro.get_input<dbrec_hierarchy_sptr>(i++);
  int depth = pro.get_input<int>(i++);
  dbrec_context_factory_sptr cf = pro.get_input<dbrec_context_factory_sptr>(i++);
  int class_id = pro.get_input<int>(i++);
 
  dbrec_train_rot_inv_compositional_parts_visitor pv(h, depth, cf);
  dbrec_part_sptr cp = h->root(class_id);
  if (!cp) {
    vcl_cout << "In dbrec_image_train_rot_inv_composite_parts_process() - cannot find the class root with id: " << class_id << "!\n";
    return false;
  }
  cp->accept(&pv);
  return true;
}

//: Constructor, generate a bunch of parts with pairwise discrete models of rot invariant parts
bool dbrec_image_populate_hierarchy_pairwise_discrete_process_cons(bprb_func_process& pro)
{
  bool ok = false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_hierarchy_sptr");
  input_types.push_back("int");
  //input_types.push_back("int");
  input_types.push_back("float"); // pass the angle increments used to extract primitives, the gamma intervals will be adjusted wrt this
  input_types.push_back("int");
  input_types.push_back("int");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  output_types.push_back("dbrec_hierarchy_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_image_populate_hierarchy_pairwise_discrete_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 5) {
    vcl_cerr << "In dbrec_image_populate_hierarchy_pairwise_discrete_process - invalid inputs!!!!!!!!!!\n";
    return false;
  }
  unsigned i = 0; 
  dbrec_hierarchy_sptr h = pro.get_input<dbrec_hierarchy_sptr>(i++);
  int radius = pro.get_input<int>(i++);
  //int gamma_interval = pro.get_input<int>(i++);
  float angle_increment = pro.get_input<float>(i++);
  int d_interval = pro.get_input<int>(i++);
  int rho_interval = pro.get_input<int>(i++);

  //dbrec_hierarchy_sptr h_new = populate_class_based_pairwise_discrete_compositor(h, radius, gamma_interval, d_interval, rho_interval);
  dbrec_hierarchy_sptr h_new = populate_class_based_pairwise_discrete_compositor(h, radius, angle_increment, d_interval, rho_interval);
  pro.set_output(0, new brdb_value_t<dbrec_hierarchy_sptr>(h_new));
  return true;
}

//: pick top N parts based on the entropy and volume of the pairwise discrete parts at the given depth
bool dbrec_image_entropy_selector_pairwise_discrete_process_cons(bprb_func_process& pro)
{
  bool ok = false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_hierarchy_sptr");
  input_types.push_back("int");
  input_types.push_back("int"); // if 1: set class prior of the compositions based on volumes in the histograms, if 0: set them to the constant value passed as the next input
  input_types.push_back("float");  // the const composition class prior if to be set to this constant
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  output_types.push_back("dbrec_hierarchy_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}
bool volume_greater(const vcl_pair<float, dbrec_part_sptr>& pa1, const vcl_pair<float, dbrec_part_sptr>& pa2)
{
  return pa1.first > pa2.first;
}
bool dbrec_image_entropy_selector_pairwise_discrete_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 4) {
    vcl_cerr << "In dbrec_image_populate_hierarchy_pairwise_discrete_process - invalid inputs!!!!!!!!!!\n";
    return false;
  }
  unsigned i = 0; 
  dbrec_hierarchy_sptr h = pro.get_input<dbrec_hierarchy_sptr>(i++);
  int N = pro.get_input<int>(i++);
  int val = pro.get_input<int>(i++);
  bool set_using_volume = val == 1 ? true : false;
  float const_comp_class_prior = pro.get_input<float>(i++);

  //: use an id factory and register the parts to create unique ids for the new parts
  dbrec_type_id_factory* ins = dbrec_type_id_factory::instance();
  h->register_parts(ins);
  
  dbrec_hierarchy_sptr new_h = new dbrec_hierarchy();
  for (unsigned c = 0; c < h->class_cnt(); c++) {
    dbrec_part_sptr c_node = h->root(c);
    dbrec_composition* c_node_comp = dynamic_cast<dbrec_composition*>(c_node.ptr());
    vcl_vector<dbrec_part_sptr> parts = c_node_comp->children();  // this is an OR composition, use all of its parts
    //: first prune the pairs so that the one with less entropy stays
    
    vcl_vector<vcl_pair<float, dbrec_part_sptr> > part_pool;
    for (unsigned i = 0; i < parts.size()-1; i+=2) {
      dbrec_part_sptr p1 = parts[i];
      dbrec_part_sptr p2 = parts[i+1];
      dbrec_composition* p1c = dynamic_cast<dbrec_composition*>(p1.ptr());
      dbrec_pairwise_discrete_compositor* pdc1 = dynamic_cast<dbrec_pairwise_discrete_compositor*>(p1c->compositor().ptr());

      dbrec_composition* p2c = dynamic_cast<dbrec_composition*>(p2.ptr());
      dbrec_pairwise_discrete_compositor* pdc2 = dynamic_cast<dbrec_pairwise_discrete_compositor*>(p2c->compositor().ptr());

      float e1 = pdc1->model_->class_entropy();
      float e2 = pdc2->model_->class_entropy();
      float v1 = pdc1->model_->class_volume(); float v1nc = pdc1->model_->non_class_volume();
      float v2 = pdc2->model_->class_volume(); float v2nc = pdc2->model_->non_class_volume();
      if (set_using_volume) {
        p1c->set_class_prior(v1/(v1+v1nc));
        p2c->set_class_prior(v2/(v2+v2nc));
      } else {
        p1c->set_class_prior(const_comp_class_prior);
        p2c->set_class_prior(const_comp_class_prior);
      }
      if (e1 < e2) 
        part_pool.push_back(vcl_pair<float, dbrec_part_sptr>(v1, p1));
      else
        part_pool.push_back(vcl_pair<float, dbrec_part_sptr>(v2, p2));
    }
    vcl_sort(part_pool.begin(), part_pool.end(), volume_greater);
    vcl_vector<dbrec_part_sptr> part_pool2;
    int upper_limit = int(part_pool.size()) < N ? part_pool.size() : N;
    for (int k = 0; k < upper_limit; k++) { // get top N 
      part_pool2.push_back(part_pool[k].second);
    }

    dbrec_compositor_sptr oc = new dbrec_or_compositor();
    dbrec_part_sptr class_composition = new dbrec_composition(ins->new_type(), part_pool2, oc, c_node_comp->radius()); // the OR node is higher in the hierarchy but has the same radius
    new_h->add_root(class_composition);
  }
  
  pro.set_output(0, new brdb_value_t<dbrec_hierarchy_sptr>(new_h));
  return true;
}
