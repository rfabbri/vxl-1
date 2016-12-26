// This is lemsvxlsrc/contrib/dbrec_lib/dbrec/pro/processes/dbrec_image_parse_processes.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Processes to parse images
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

#include <dbrec/dbrec_part.h>
#include <dbrec/dbrec_image_visitors.h>
#include <dbrec/dbrec_part_context.h>
#include <dbrec/dbrec_gaussian_primitive.h>

#include <vil/vil_new.h>
#include <vil/vil_convert.h>
#include <vil/vil_save.h>

#include <vnl/vnl_random.h>
#include <brip/brip_vil_float_ops.h>
#include <bsta/bsta_histogram.h>
#include <bsta/vis/bsta_svg_tools.h>

//: Constructor
bool dbrec_image_parse_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");      // input image
  input_types.push_back("dbrec_hierarchy_sptr");
  input_types.push_back("dbrec_context_factory_sptr");      // if a context factory is available for the image pass it if you want to re-use previous parses of the parts which exist in this context factory
                                                            // otherwise call the initialization method of the process which sets this input to zero
  input_types.push_back("float");      // class prior for primitive parts' posterior calculations
  input_types.push_back("vcl_string");  // model path which contains fg appearance model parameters
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("dbrec_context_factory_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_image_parse_process_init(bprb_func_process& pro)
{
  dbrec_context_factory_sptr cf; // initialize to zero
  pro.set_input(2, new brdb_value_t<dbrec_context_factory_sptr>(cf));
  return true;
}
bool dbrec_image_parse_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 5) {
    vcl_cerr << "dbrec_image_parse_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  vil_image_view_base_sptr inp_img = pro.get_input<vil_image_view_base_sptr>(i++);
  if (inp_img->pixel_format() != VIL_PIXEL_FORMAT_BYTE)
    return false;

  vil_image_view<vxl_byte> orig_img(inp_img);
  if (orig_img.nplanes() == 3) {
    vcl_cout << "In dbrec_image_parse_process() -- converting input RGB image to grey scale!\n";
    vil_image_view<vxl_byte> *out_img = new vil_image_view<vxl_byte>(inp_img->ni(),inp_img->nj());
    vil_convert_planes_to_grey<vxl_byte, vxl_byte>(*(inp_img.as_pointer()),*out_img);
    inp_img = out_img;
    vil_save(*inp_img, "./test_grey.png");
  }
  
  vil_image_view<float> img = *vil_convert_cast(float(), inp_img);
  vil_math_scale_values(img,1.0/255.0);

  vil_image_resource_sptr input_sptr = vil_new_image_resource_of_view(img);

  dbrec_hierarchy_sptr h = pro.get_input<dbrec_hierarchy_sptr>(i++);
  dbrec_context_factory_sptr cf = pro.get_input<dbrec_context_factory_sptr>(i++);
  
  float class_prior = pro.get_input<float>(i++);
  vcl_string model_path = pro.get_input<vcl_string>(i++);

  //: for now use the same class_prior for both compositional parts and primitive parts
  vcl_vector<float> comp_priors; comp_priors.push_back(class_prior); comp_priors.push_back(1.0f-class_prior);

  if (!cf) {
    dbrec_parse_image_visitor pv(h, input_sptr, class_prior, comp_priors, model_path);
    for (unsigned i = 0; i < h->class_cnt(); i++) {
      h->root(i)->accept(&pv);
    }
    pro.set_output_val<dbrec_context_factory_sptr>(0, pv.context_factory());
  } else {
    dbrec_parse_image_visitor pv(h, cf, input_sptr, class_prior, comp_priors, model_path);
    for (unsigned i = 0; i < h->class_cnt(); i++) {
      h->root(i)->accept(&pv);
    }
    pro.set_output_val<dbrec_context_factory_sptr>(0, pv.context_factory());
  }
  return true;
}

//: Constructor
bool dbrec_image_parse_with_fg_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");      // input image
  input_types.push_back("dbrec_hierarchy_sptr");      
  input_types.push_back("dbrec_context_factory_sptr");      // if a context factory is available for the image pass it if you want to re-use previous parses of the parts which exist in this context factory
                                                            // otherwise call the initialization method of the process which sets this input to zero
  input_types.push_back("vil_image_view_base_sptr");      // fg map from the bakcground model for the input img
  input_types.push_back("float");      // class prior for primitive parts' posterior calculations
  input_types.push_back("float");      // prior for compositional parts' posterior calculations, prior for class-foreground composition
  input_types.push_back("float");      // prior for compositional parts' posterior calculations, prior for non-class-foreground composition
  input_types.push_back("float");      // prior for compositional parts' posterior calculations, prior for class-background composition
                                       // prior for non-class-background composition will be 1.0-(prior_cf + prior_ncf + prior_cb);

  input_types.push_back("vcl_string");  // model path which contains fg appearance model parameters
  input_types.push_back("vcl_string");  // model path for background image mean and std dev of operator responses
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("dbrec_context_factory_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_image_parse_with_fg_process_init(bprb_func_process& pro)
{
  dbrec_context_factory_sptr cf; // initialize to zero
  pro.set_input(2, new brdb_value_t<dbrec_context_factory_sptr>(cf));
  return true;
}
bool dbrec_image_parse_with_fg_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 10) {
    vcl_cerr << "dbrec_image_parse_with_fg_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  vil_image_view_base_sptr inp_img = pro.get_input<vil_image_view_base_sptr>(i++);
  if (inp_img->pixel_format() != VIL_PIXEL_FORMAT_BYTE)
    return false;

   vil_image_view<vxl_byte> orig_img(inp_img);
  if (orig_img.nplanes() == 3) {
    vcl_cout << "In dbrec_image_parse_process() -- converting input RGB image to grey scale!\n";
    vil_image_view<vxl_byte> *out_img = new vil_image_view<vxl_byte>(inp_img->ni(),inp_img->nj());
    vil_convert_planes_to_grey<vxl_byte, vxl_byte>(*(inp_img.as_pointer()),*out_img);
    inp_img = out_img;
    vil_save(*inp_img, "./test_grey.png");
  }
  
  vil_image_view<float> img = *vil_convert_cast(float(), inp_img);
  vil_math_scale_values(img,1.0/255.0);

  vil_image_resource_sptr input_sptr = vil_new_image_resource_of_view(img);

  dbrec_hierarchy_sptr h = pro.get_input<dbrec_hierarchy_sptr>(i++);
  dbrec_context_factory_sptr cf = pro.get_input<dbrec_context_factory_sptr>(i++);
  
  vil_image_view_base_sptr fg_map_sptr = pro.get_input<vil_image_view_base_sptr>(i++);

  if (fg_map_sptr->pixel_format() == VIL_PIXEL_FORMAT_BOOL) { 
    vcl_cout << "In dbrec_image_parse_with_fg_process() - passed the ground truth map as the prob map so casting BOOL img to FLOAT img, assuming true pixels have prob 1.0\n";
    fg_map_sptr = vil_convert_cast(float(), fg_map_sptr);
  } else if (fg_map_sptr->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;
  vil_image_view<float> fg_map(fg_map_sptr);

  float min, max;
  vil_math_value_range(fg_map, min, max);
  vcl_cout << "min value in the fg map: " << min << " max value: " << max << vcl_endl;
  
  float class_prior = pro.get_input<float>(i++);
  float prior_cf = pro.get_input<float>(i++);
  float prior_ncf = pro.get_input<float>(i++);
  float prior_cb = pro.get_input<float>(i++);
  float prior_ncb = 1.0f - (prior_cf + prior_ncf + prior_cb);
  vcl_string fg_model_path = pro.get_input<vcl_string>(i++);
  vcl_string bg_model_path = pro.get_input<vcl_string>(i++);

  vcl_vector<float> comp_priors; 
  comp_priors.push_back(prior_cf); comp_priors.push_back(prior_ncf); comp_priors.push_back(prior_cb); comp_priors.push_back(prior_ncb);

  if (!cf) {
    dbrec_parse_image_with_fg_map_visitor pv(h, input_sptr, fg_map, class_prior, comp_priors, fg_model_path, bg_model_path);
    for (unsigned i = 0; i < h->class_cnt(); i++) {
      h->root(i)->accept(&pv);
    }
    pro.set_output_val<dbrec_context_factory_sptr>(0, pv.context_factory());
  } else {
    dbrec_parse_image_with_fg_map_visitor pv(h, cf, input_sptr, fg_map, class_prior, comp_priors, fg_model_path, bg_model_path);
    for (unsigned i = 0; i < h->class_cnt(); i++) {
      h->root(i)->accept(&pv);
    }
    pro.set_output_val<dbrec_context_factory_sptr>(0, pv.context_factory());
  }
  
  return true;
}
//: Constructor
bool dbrec_image_train_composite_parts_process_cons(bprb_func_process& pro)
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
bool dbrec_image_train_composite_parts_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 4) {
    vcl_cerr << "In dbrec_image_train_composite_parts_process - invalid inputs!!!!!!!!!!\n";
    return false;
  }
  unsigned i = 0;
  dbrec_hierarchy_sptr h = pro.get_input<dbrec_hierarchy_sptr>(i++);
  int depth = pro.get_input<int>(i++);
  dbrec_context_factory_sptr cf = pro.get_input<dbrec_context_factory_sptr>(i++);
  int class_id = pro.get_input<int>(i++);
 
  dbrec_train_compositional_parts_visitor pv(h, depth, cf);
  dbrec_part_sptr cp = h->root(class_id);
  if (!cp) {
    vcl_cout << "In dbrec_image_train_parts_process() - cannot find the class root with id: " << class_id << "!\n";
    return false;
  }
  cp->accept(&pv);
  return true;
}

//: Constructor
bool dbrec_context_factory_get_map_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_context_factory_sptr");      // input context factory
  input_types.push_back("dbrec_hierarchy_sptr");      // input hierarchy
  input_types.push_back("vil_image_view_base_sptr");      // input image
  input_types.push_back("int");      // type id of the part to generate output maps for
  input_types.push_back("int");      // id of the posterior map to output, for instance primitive parts possibly have 4 maps: class-foreground map, non-class fg map, class bg map, non-class bg map
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");      // output prob map
  output_types.push_back("vil_image_view_base_sptr");      // output map overlayed on orig view as a byte image
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_context_factory_get_map_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 5) {
    vcl_cerr << "dbrec_image_parse_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_context_factory_sptr cf = pro.get_input<dbrec_context_factory_sptr>(i++);
  dbrec_hierarchy_sptr h = pro.get_input<dbrec_hierarchy_sptr>(i++);

  vil_image_view_base_sptr orig_view = pro.get_input<vil_image_view_base_sptr>(i++);
  if (orig_view->pixel_format() != VIL_PIXEL_FORMAT_BYTE)
    return false;
  vil_image_view<vxl_byte> orig_img(orig_view);

  int type_id = pro.get_input<int>(i++);
  int map_id = pro.get_input<int>(i++);

  dbrec_part_context_sptr pc = cf->get_context(type_id);
  dbrec_part_sptr p = h->get_part(type_id);

  if (!pc || !p) {
    vcl_cout << "In dbrec_context_factory_get_map_process() -- The context of type: " << type_id << " is not created or a part with this id does not exist!\n";
    return false;
  }

  vil_image_resource_sptr cfm = pc->get_posterior_map(map_id, orig_img.ni(), orig_img.nj());
  if (!cfm) {
    vcl_cout << "In dbrec_context_factory_get_map_process() -- Problems in retrieving posterior map in the context of type: " << type_id << " with map id: " << map_id << "!\n";
    return false;
  }
  vil_image_view<float> out = cfm->get_view();
 
  dbrec_mark_receptive_fields_visitor mrfv(pc, out);
  p->accept(&mrfv);

  vil_image_view<float> out_with_receptive_fields = mrfv.get_rec_field_map();
  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(out_with_receptive_fields));

  //: get the colored output byte img
  vil_image_view<vxl_byte>* out_colored_img = new vil_image_view<vxl_byte>(orig_img.ni(), orig_img.nj(), 3);
  mrfv.get_colored_img(orig_img, *out_colored_img);
  vil_image_view_base_sptr out_colored_img_sptr = out_colored_img;

  pro.set_output_val<vil_image_view_base_sptr>(1, out_colored_img_sptr);

  //: min and max values
  float min, max;
  vil_math_value_range(out, min, max);
  vcl_cout << "min value in the output map: " << min << " max value: " << max << vcl_endl;

  return true;
}
//: Constructor
bool dbrec_context_factory_write_maps_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_context_factory_sptr");      // input context factory
  input_types.push_back("dbrec_hierarchy_sptr");      // input hierarchy
  input_types.push_back("vil_image_view_base_sptr");      // input image
  input_types.push_back("vcl_string");      // prefix (includes the path) to write the output maps to
  input_types.push_back("vcl_string");      // suffix to write the output maps to
  input_types.push_back("int");      // id of the posterior map to output, for instance primitive parts possibly have 4 maps: class-foreground map, non-class fg map, class bg map, non-class bg map
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_context_factory_write_maps_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 6) {
    vcl_cerr << "dbrec_image_parse_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_context_factory_sptr cf = pro.get_input<dbrec_context_factory_sptr>(i++);
  dbrec_hierarchy_sptr h = pro.get_input<dbrec_hierarchy_sptr>(i++);

  vil_image_view_base_sptr orig_view = pro.get_input<vil_image_view_base_sptr>(i++);
  if (orig_view->pixel_format() != VIL_PIXEL_FORMAT_BYTE)
    return false;
  vil_image_view<vxl_byte> orig_img(orig_view);

  vcl_string prefix = pro.get_input<vcl_string>(i++);
  vcl_string suffix = pro.get_input<vcl_string>(i++);
  int map_id = pro.get_input<int>(i++);

  vcl_vector<dbrec_part_sptr> all_parts;
  h->get_all_parts(all_parts);

  for (unsigned i = 0; i < all_parts.size(); i++) {
    dbrec_part_sptr p = all_parts[i];
    dbrec_part_context_sptr pc = cf->get_context(p->type());
    if (!pc) {
      vcl_cout << "In dbrec_context_factory_write_map_process() -- The context of type: " << p->type() << " is not created or a part with this id does not exist!\n";
      return false;
    }
    vil_image_resource_sptr cfm = pc->get_posterior_map(map_id, orig_img.ni(), orig_img.nj());
    if (!cfm) {
      vcl_cout << "In dbrec_context_factory_write_maps_process() -- Problems in retrieving posterior map in the context of type: " << p->type() << " with map id: " << map_id << "!\n";
      return true;
    }
    vil_image_view<float> out = cfm->get_view();
 
    dbrec_mark_receptive_fields_visitor mrfv(pc, out);
    p->accept(&mrfv);

    vil_image_view<float> out_with_receptive_fields = mrfv.get_rec_field_map();
    
    //: get the colored output byte img
    vil_image_view<vxl_byte>* out_colored_img = new vil_image_view<vxl_byte>(orig_img.ni(), orig_img.nj(), 3);
    mrfv.get_colored_img(orig_img, *out_colored_img);
    //vil_image_view_base_sptr out_colored_img_sptr = out_colored_img;
    vcl_stringstream typess; typess << p->type();
    vcl_string name = prefix + "part_"+ typess.str() + "_" +  suffix;
    vil_save(*out_colored_img, name.c_str());

  }
  return true;
}

//: A process to generate response map image for primitive parts whose context contains a response map and a mask image 
bool dbrec_context_factory_get_response_map_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_context_factory_sptr");      // input context factory
  input_types.push_back("vil_image_view_base_sptr");      // input image
  input_types.push_back("int");      // type id of the part to generate output maps for
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");      // output map overlayed on orig view as a byte image
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_context_factory_get_response_map_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 3) {
    vcl_cerr << "dbrec_context_factory_get_response_map_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_context_factory_sptr cf = pro.get_input<dbrec_context_factory_sptr>(i++);
  vil_image_view_base_sptr orig_view = pro.get_input<vil_image_view_base_sptr>(i++);
  if (orig_view->pixel_format() != VIL_PIXEL_FORMAT_BYTE)
    return false;
  vil_image_view<vxl_byte> orig_img(orig_view);

  int type_id = pro.get_input<int>(i++);

  dbrec_part_context_sptr pc = cf->get_context(type_id);

  if (!pc || pc->maps_size() < 2) {
    vcl_cout << "In dbrec_context_factory_get_response_map_process() -- The context of type: " << type_id << " is not created or it does not contains both a response map and a mask img!\n";
    return false;
  }

  vil_image_resource_sptr res_sptr = pc->get_map(0);
  vil_image_view<float> res(res_sptr->get_view());
  vil_image_resource_sptr res_mask_sptr = pc->get_map(1);
  vil_image_view<float> res_mask(res_mask_sptr->get_view());

  vil_image_resource_sptr res_resc = vil_new_image_resource_of_view(res);
  vil_image_resource_sptr msk_resc = vil_new_image_resource_of_view(res_mask);
  vil_image_resource_sptr img_resc = vil_new_image_resource_of_view(orig_img);
  vil_image_view<vil_rgb<vxl_byte> > rgb = brip_vil_float_ops::combine_color_planes(img_resc, res_resc, msk_resc);
  vil_image_view_base_sptr out_colored_img_sptr = new vil_image_view<vil_rgb<vxl_byte> > (rgb);
  pro.set_output_val<vil_image_view_base_sptr>(0, out_colored_img_sptr);
  return true;
}

//: A process to generate response map image for primitive parts whose context contains a response map and a mask image 
bool dbrec_context_factory_create_response_histogram_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_context_factory_sptr");      // input context factory
  input_types.push_back("dbrec_hierarchy_sptr");      // input hierarchy
  input_types.push_back("vil_image_view_base_sptr");      // input gt image, two response histograms will be created, one of them will use this gt
  input_types.push_back("int");      // type id of the part to generate output maps for
  input_types.push_back("vcl_string"); // output prefix for the histograms
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_context_factory_create_response_histogram_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 5) {
    vcl_cerr << "dbrec_context_factory_get_response_map_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_context_factory_sptr cf = pro.get_input<dbrec_context_factory_sptr>(i++);
  dbrec_hierarchy_sptr h = pro.get_input<dbrec_hierarchy_sptr>(i++);
  vil_image_view_base_sptr gt_view = pro.get_input<vil_image_view_base_sptr>(i++);
  if (gt_view->pixel_format() != VIL_PIXEL_FORMAT_BOOL)
    return false;
  vil_image_view<bool> gt_img(gt_view);

  int type_id = pro.get_input<int>(i++);
  vcl_string output_prefix = pro.get_input<vcl_string>(i++);

  dbrec_part_context_sptr pc = cf->get_context(type_id);
  dbrec_part_sptr part = h->get_part(type_id);
  dbrec_gaussian* gpart = dynamic_cast<dbrec_gaussian*>(part.ptr());
  vil_image_view<bool> gt_img_dilated(gt_img.ni(), gt_img.nj());
  gt_img_dilated.fill(false);
  gpart->mask_dilate(gt_img, gt_img_dilated);

  if (!pc || pc->maps_size() < 2) {
    vcl_cout << "In dbrec_context_factory_get_response_map_process() -- The context of type: " << type_id << " is not created or it does not contains both a response map and a mask img!\n";
    return false;
  }

  vil_image_resource_sptr res_sptr = pc->get_map(0);
  vil_image_view<float> res(res_sptr->get_view());
  
  //float max_response = 0.1f;
  //unsigned int nbins = 100;
  //unsigned int nbins = 50;
  //bsta_histogram<float> gt_hist(0.0f, max_response, nbins);
  //bsta_histogram<float> hist(0.0f, max_response, nbins);

  bsta_histogram<float> gt_hist(-7.0f, 1.0f, 32);
  bsta_histogram<float> hist(-7.0f, 1.0f, 32);

  //bsta_joint_histogram<float> joint_hist(0.0f,2.0f,(unsigned int)2,0.0f,max_response, nbins);
  bsta_joint_histogram<float> joint_hist(0.0f,2.0f,(unsigned int)2,-7.0f,1.0f, 32);

  //float min_response = 0.003f;
  float min_response = 1.0e-3f;
  //float min_response = 0.001f;
  
  for (unsigned i = 0; i < res.ni(); i++)
    for (unsigned j = 0; j < res.nj(); j++) {
      if (!gt_img_dilated(i,j) && res(i,j) >= min_response) { 
        hist.upcount(vcl_log10(res(i,j)),1.0);
        joint_hist.upcount(0.0f,0.0f, vcl_log10(res(i,j)), 1.0f);
      }
      if (gt_img_dilated(i,j) && res(i,j) >= min_response)
        joint_hist.upcount(2.0f,0.0f, vcl_log10(res(i,j)), 1.0f);
    }

  for (unsigned i = 0; i < res.ni(); i++)
    for (unsigned j = 0; j < res.nj(); j++) {
      if (gt_img_dilated(i,j) && res(i,j) >= min_response)
        gt_hist.upcount(vcl_log10(res(i,j)),1.0);
    }
  
  float width = 600.0, height = 600.0, margin = 30.0;
  int font_size = 15;
  vcl_string name = output_prefix + "_response_hist.svg";
  write_svg<float>(hist, name, width, height, margin, font_size);
  name = output_prefix + "_gt_response_hist.svg";
  write_svg<float>(gt_hist, name, width, height, margin, font_size);
  vcl_cout << output_prefix << " joint histogram entropy: " << joint_hist.entropy() << " marginal a: " << joint_hist.entropy_marginal_a() << "\n";
  name = output_prefix + "_joint_response_hist.vrml";
  vcl_ofstream os(name.c_str());
  joint_hist.print_to_vrml(os);
  os.close();

  name = output_prefix + "_joint_response_hist.txt";
  vcl_ofstream os2(name.c_str());
  joint_hist.print_to_text(os2);
  os2.close();
  return true;
}

//: Constructor
//: this process returns a single name (classification output) for the whole image by looking at the parse given by the hierarchy
//  this process is to be used in comparisons with PCA to classify images being known to contain a single object from one of the categories in the hierarchy
bool dbrec_classify_image_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_context_factory_sptr");      // input context factory
  input_types.push_back("dbrec_hierarchy_sptr");      // input hierarchy
  input_types.push_back("vil_image_view_base_sptr");      // input image
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  output_types.push_back("int");      // output class id
  output_types.push_back("vil_image_view_base_sptr");      // output map overlayed on orig view as a byte image
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dbrec_classify_image_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 3) {
    vcl_cerr << "dbrec_classify_image_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  dbrec_context_factory_sptr cf = pro.get_input<dbrec_context_factory_sptr>(i++);
  dbrec_hierarchy_sptr h = pro.get_input<dbrec_hierarchy_sptr>(i++);

  vil_image_view_base_sptr orig_view = pro.get_input<vil_image_view_base_sptr>(i++);
  if (orig_view->pixel_format() != VIL_PIXEL_FORMAT_BYTE)
    return false;
  vil_image_view<vxl_byte> orig_img(orig_view);

  vnl_random rng;

  int out_class = -1;
  double max_class_posterior = -1.0f;
  dbrec_part_ins_sptr ins_max_class;
  //:get the posterior maps for each class node (root node of the hierarhcy)
  for (unsigned i = 0; i < h->class_cnt(); i++) {
    dbrec_part_sptr p = h->root(i);
    dbrec_part_context_sptr pc = cf->get_context(p->type());
    if (!pc) {
      vcl_cout << "In dbrec_context_factory_get_map_process() -- The context of class: " << i << " is not created in the passed factory!\n";
      return false;
    }
    //: find the instance with the largest posterior in the image
    double max = -1.0f;  // posteriors are larger than 0
    dbrec_part_ins_sptr ins_max;
    dbrec_part_ins_sptr ins = pc->first();
    while (ins) {
      double post = ins->posterior(0);
      if (post > max) {
        max = post;
        ins_max = ins;
      }
      ins = pc->next();
    }
    
    if ((max > max_class_posterior) ||
        (vcl_abs(max-max_class_posterior) < 0.0001 && rng.drand32() <= 0.5) ) { // decide randomly which class to pick in the case of ties
      max_class_posterior = max;
      ins_max_class = ins_max;
      out_class = i;
    }
  }
  if (out_class < 0 || !ins_max_class) {
    vcl_cout << "In dbrec_classify_image_process() - the max posterior class cannot be found!\n";
    pro.set_output_val<int>(0, out_class);
    return false;
  }

  pro.set_output_val<int>(0, out_class);

  //: create a center map from this instance  
  vil_image_view<float> out_center_map(orig_view->ni(), orig_view->nj());
  unsigned ii = (unsigned)ins_max_class->pt().x();
  unsigned jj = (unsigned)ins_max_class->pt().y();
  if (ii < orig_view->ni() && jj < orig_view->nj() && ii >= 0 && jj >= 0)
    out_center_map(ii,jj) = (float)ins_max_class->posterior(0);
  
  //: create an receptive field map from this instance  
  vil_image_view<float> out_rec_map(orig_view->ni(), orig_view->nj());
  dbrec_mark_receptive_field_visitor mrfv(ins_max_class, (float)ins_max_class->posterior(0), out_rec_map);
  (h->root(out_class))->accept(&mrfv);

  vil_image_view<vxl_byte> out(orig_view->ni(), orig_view->nj(), 3);
  dbrec_mark_receptive_fields_visitor::get_colored_img(orig_img, out_center_map, out_rec_map, out);
  vil_image_view_base_sptr out_sptr = new vil_image_view<vxl_byte>(out);

  pro.set_output_val<vil_image_view_base_sptr>(1, out_sptr);
  
  return true;
}


