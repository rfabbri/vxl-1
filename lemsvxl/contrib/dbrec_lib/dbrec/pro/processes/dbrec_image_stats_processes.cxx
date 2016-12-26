// This is lemsvxlsrc/contrib/dbrec_lib/dbrec/pro/processes/dbrec_image_stats_processes.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Processes to collect stats
//
// \author Ozge Can Ozcanli
// \date May 06, 2009
//
// \verbatim
//  Modifications
//   none yet
// \endverbatim

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_new.h>
#include <vil/vil_convert.h>
#include <vil/vil_save.h>
#include <vil/algo/vil_threshold.h>
#include <dbrec/dbrec_image_learner_visitors.h>
#include <dbrec/dbrec_visitor_sptr.h>
#include <dbrec/dbrec_part_selector.h>
#include <dbrec/dbrec_type_id_factory.h>
#include <dbfs/dbfs_selector.h>
#include <dbfs/dbfs_selector_sptr.h>

//: Constructor
bool dbrec_image_weibull_model_learner_init_process_cons(bprb_func_process& pro)
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
bool dbrec_image_weibull_model_learner_init_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 1) {
    vcl_cerr << "dbrec_image_parse_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_hierarchy_sptr h = pro.get_input<dbrec_hierarchy_sptr>(i++);
  dbrec_visitor_sptr lv = new dbrec_gaussian_weibull_model_learner_visitor(h);
  pro.set_output_val<dbrec_visitor_sptr>(0, lv);
  return true;
}

//: Constructor
bool dbrec_image_weibull_model_update_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_visitor_sptr");      
  input_types.push_back("dbrec_context_factory_sptr");    // current parse of the image
  input_types.push_back("vil_image_view_base_sptr");      // fg map (values in range [0,1] where 1 denotes foreground) or BOOL image as a ground truth map 
  input_types.push_back("vil_image_view_base_sptr");      // image mask to indicate valid regions in the image to collect stats
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_image_weibull_model_update_process_init(bprb_func_process& pro)
{
  vil_image_view_base_sptr mask_ptr; // initialize to zero
  pro.set_input(3, new brdb_value_t<vil_image_view_base_sptr>(mask_ptr));
  return true;
}

bool dbrec_image_weibull_model_update_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 4) {
    vcl_cerr << "dbrec_image_weibull_model_update_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  dbrec_visitor_sptr v = pro.get_input<dbrec_visitor_sptr>(i++);
  dbrec_gaussian_weibull_model_learner_visitor* lv = dynamic_cast<dbrec_gaussian_weibull_model_learner_visitor*>(v.ptr());
 
  if (!lv) {
    vcl_cerr << "dbrec_image_weibull_model_update_process - cannot cast input pointer!\n";
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
    vcl_cout << "In dbrec_image_weibull_model_update_process() -- the obj/fg map is neither a BOOL nor a float img!\n";
    return false;
  }
  vil_image_view<float> fg_map(fg_map_sptr);

  vil_image_view_base_sptr mask_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  if (!mask_sptr) {  // initialized to zero, set it to all true
    vil_image_view<bool> tmp(fg_map.ni(), fg_map.nj()); tmp.fill(true);
    mask_sptr = new vil_image_view<bool>(tmp);
  }
  if (mask_sptr->pixel_format() != VIL_PIXEL_FORMAT_BOOL) {
    vcl_cout << "In dbrec_image_weibull_model_update_process() - the valid regions mask passed to the process is not a BOOL image!\n";
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
//: Constructor
bool dbrec_image_weibull_model_learner_print_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_visitor_sptr");      
  input_types.push_back("vcl_string");                    // path to output current histograms and models in the learners
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_image_weibull_model_learner_print_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 2) {
    vcl_cerr << "dbrec_image_weibull_model_learner_print_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_visitor_sptr v = pro.get_input<dbrec_visitor_sptr>(i++);
  dbrec_gaussian_weibull_model_learner_visitor* lv = dynamic_cast<dbrec_gaussian_weibull_model_learner_visitor*>(v.ptr());
  vcl_string path = pro.get_input<vcl_string>(i++);
  lv->print_current_histograms(path);
  lv->print_current_models(path);
  return true;
}

// Hierarchy Construction Processes, a bunch of class-labeled training images are required

//: Constructor
bool dbrec_hierarchy_learner_init_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_hierarchy_sptr");      
  input_types.push_back("int");  // depth of the parts at the hierarchy that will be used for initialization
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  output_types.push_back("dbrec_part_selection_measure_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_hierarchy_learner_init_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    vcl_cerr << "dbrec_hierarchy_learner_init_process - invalid inputs\n";
    return false;
  }
  // get input
  unsigned i = 0;
  dbrec_hierarchy_sptr h = pro.get_input<dbrec_hierarchy_sptr>(i++);
  int depth = pro.get_input<int>(i++);

  vcl_vector<dbrec_part_sptr> parts;
  h->get_parts(depth, parts);

  dbrec_part_selection_measure_sptr sm = new dbrec_part_selection_measure(parts, h->class_cnt());
  pro.set_output_val<dbrec_part_selection_measure_sptr>(0, sm);
  return true;
}

//: Constructor
bool dbrec_hierarchy_learner_update_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_part_selection_measure_sptr");      
  input_types.push_back("dbrec_context_factory_sptr");    // current parse of the image
  input_types.push_back("vil_image_view_base_sptr");      // fg map or ground truth map
  input_types.push_back("vil_image_view_base_sptr");      // image mask to indicate valid regions in the image to collect stats
  input_types.push_back("int");  // class id: the id of the class that the training image belongs to
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  output_types.push_back("dbrec_part_selection_measure_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_hierarchy_learner_update_process_init(bprb_func_process& pro)
{
  vil_image_view_base_sptr mask_ptr; // initialize to zero
  pro.set_input(3, new brdb_value_t<vil_image_view_base_sptr>(mask_ptr));
  return true;
}
bool dbrec_hierarchy_learner_update_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 5) {
    vcl_cerr << "dbrec_hierarchy_learner_update_process - invalid inputs\n";
    return false;
  }
  // get input
  unsigned i = 0;
  dbrec_part_selection_measure_sptr sm = pro.get_input<dbrec_part_selection_measure_sptr>(i++);
  dbrec_context_factory_sptr cf = pro.get_input<dbrec_context_factory_sptr>(i++);

  vil_image_view_base_sptr fg_map_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  if (fg_map_sptr->pixel_format() == VIL_PIXEL_FORMAT_BOOL) {
    fg_map_sptr = vil_convert_cast(float(), fg_map_sptr);
  } else if (fg_map_sptr->pixel_format() != VIL_PIXEL_FORMAT_FLOAT) {
    vcl_cout << "In dbrec_hierarchy_learner_update_process() -- the obj/fg map is neither a BOOL nor a float img!\n";
    return false;
  }
  vil_image_view<float> fg_map(fg_map_sptr);

  vil_image_view_base_sptr mask_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  if (!mask_sptr) {  // initialized to zero, set it to all true
    vil_image_view<bool> tmp(fg_map.ni(), fg_map.nj()); tmp.fill(true);
    mask_sptr = new vil_image_view<bool>(tmp);
  }
  if (mask_sptr->pixel_format() != VIL_PIXEL_FORMAT_BOOL) {
    vcl_cout << "In dbrec_hierarchy_learner_update_process() - the valid regions mask passed to the process is not a BOOL image!\n";
    return false;
  }
  vil_image_view<bool> mask(mask_sptr);

  int class_id = pro.get_input<int>(i++);

  sm->measure_training_image(cf, mask, fg_map, class_id);

  return true;
}

//: Constructor
bool dbrec_hierarchy_learner_construct_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_part_selection_measure_sptr");  
  input_types.push_back("dbrec_hierarchy_sptr");  // we need old hierarchy in order to pick unique type ids for the new parts
  input_types.push_back("int");                   // N: top N parts will be selected to construct an OR node with them for each class
  input_types.push_back("float");                 // radius, the radius of the new parts will be set as this radius
  input_types.push_back("unsigned");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  output_types.push_back("dbrec_hierarchy_sptr");      // new hierarchy
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_hierarchy_learner_construct_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 5) {
    vcl_cerr << "dbrec_hierarchy_learner_construct_process - invalid inputs\n";
    return false;
  }
  // get input
  unsigned i = 0;
  dbrec_part_selection_measure_sptr sm = pro.get_input<dbrec_part_selection_measure_sptr>(i++);
  dbrec_hierarchy_sptr old_hierarchy = pro.get_input<dbrec_hierarchy_sptr>(i++);
  int N = pro.get_input<int>(i++);
  float radius = pro.get_input<float>(i++);
  unsigned algo_type = pro.get_input<unsigned>(i++);

  vcl_vector<dbrec_part_sptr>& parts = sm->get_parts();
  int class_cnt = sm->get_class_cnt();

  //: create a new hierarchy
  dbrec_hierarchy_sptr h = new dbrec_hierarchy();

  //: use an id factory and register the parts to create unique ids for the new parts
  dbrec_type_id_factory* ins = dbrec_type_id_factory::instance();
  old_hierarchy->register_parts(ins);
  
  //: select the parts
  dbrec_part_selector sel(sm, algo_type, parts, old_hierarchy, class_cnt);

  for (int c = 0; c < class_cnt; c++) {
    vcl_cout << "class: " << c << " ";
    vcl_vector<dbrec_part_sptr> best_parts;
    sel.get_top_features(c, N, best_parts);
    if (!best_parts.size()) {
      vcl_cout << "In dbrec_hierarchy_learner_construct_process() -- no parts can be selected for class: " << c << vcl_endl;
      throw 0;
    }
    //: create an OR node with these parts
    dbrec_compositor_sptr cc_or = new dbrec_or_compositor();
    dbrec_part_sptr new_part = new dbrec_composition(ins->new_type(), best_parts, cc_or, radius);
    h->add_root(new_part);
  }

  pro.set_output_val<dbrec_hierarchy_sptr>(0, h);
  return true;
}
