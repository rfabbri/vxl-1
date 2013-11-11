// This is lemsvxlsrc/contrib/dbrec_lib/dbrec/pro/processes/dbrec_hierarchy_processes.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Processes to run PCA based recognition algorithms
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

//: new classes of dbrec
#include <dbrec_mic/dbrec_pca_utils.h>
#include <dbrec_mic/dbrec_pca_utils_sptr.h>

#include <vil/vil_convert.h>
#include <vil/vil_math.h>

#include <bvgl/bvgl_changes.h>
#include <bvgl/bvgl_changes_sptr.h>

//: Constructor
//  initialize the dbrec_pca instance to store the data
bool dbrec_pca_initialize_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("dbrec_pca_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dbrec_pca_initialize_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() != 0) {
    vcl_cerr << "brec_create_hierarchy_process - invalid inputs\n";
    return false;
  }
  dbrec_pca_sptr ins = new dbrec_pca();
  pro.set_output_val<dbrec_pca_sptr>(0, ins);
  return true;
}

//: Constructor
//  do the data convertion of images and add to the data set 
bool dbrec_pca_add_data_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_pca_sptr");
  input_types.push_back("vil_image_view_base_sptr");
  input_types.push_back("vcl_string");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dbrec_pca_add_data_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 3) {
    vcl_cerr << "dbrec_pca_add_data_process - invalid inputs\n";
    return false;
  }

  // get the inputs
  unsigned i = 0;
  dbrec_pca_sptr ps = pro.get_input<dbrec_pca_sptr>(i++);
  vil_image_view_base_sptr temp = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<float> img = *vil_convert_cast(float(), temp);
  vil_math_scale_values(img, 1.0/255.0);
  if (temp->pixel_format() != VIL_PIXEL_FORMAT_BYTE)
    return false;
  
  vcl_string class_name = pro.get_input<vcl_string>(i++);
  ps->add_image(img, class_name);
  if (!ps->check_data()) {
    vcl_cout << "In dbrec_pca_add_data_process() -- the data is not consistent!\n";
    return false;
  }

  vcl_cout << "In dbrec_pca_add_data_process() -- added the image with no problems!\n";

  return true;
}

//: Constructor
//  do the basis construction
bool dbrec_pca_construct_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_pca_sptr");
  input_types.push_back("double"); // variance_proportion to explain, used to determine number of eigen values to return
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dbrec_pca_construct_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    vcl_cerr << "dbrec_pca_construct_process - invalid inputs\n";
    return false;
  }

  // get the inputs
  unsigned i = 0;
  dbrec_pca_sptr ps = pro.get_input<dbrec_pca_sptr>(i++);
  double var_pro = pro.get_input<double>(i++);
  
  if (!ps->construct_basis_vectors(var_pro)) {
    vcl_cout << "In dbrec_pca_construct_process() -- the basis vectors cannot be constructed!\n";
    return false;
  }

  return true;
}

//  do the basis construction
bool dbrec_pca_classify_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok = false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_pca_sptr");
  input_types.push_back("vil_image_view_base_sptr");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vcl_string");
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dbrec_pca_classify_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    vcl_cerr << "dbrec_pca_construct_process - invalid inputs\n";
    return false;
  }

  // get the inputs
  unsigned i = 0;
  dbrec_pca_sptr ps = pro.get_input<dbrec_pca_sptr>(i++);
  vil_image_view_base_sptr temp = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<float> img = *vil_convert_cast(float(), temp);
  vil_math_scale_values(img, 1.0/255.0);
  if (temp->pixel_format() != VIL_PIXEL_FORMAT_BYTE)
    return false;
 
  vcl_string out_str;
  if (!ps->classify_nn(img, out_str)) {
    vcl_cout << "In dbrec_pca_classify_process() -- the input img cannot be classified!\n";
    return false;
  }

  vcl_cout << "----- classified as: " << out_str << " ----\n"; 
  pro.set_output_val<vcl_string>(0, out_str);
  return true;
}


//  initialize the dbrec_bayesian_pca instance to store the data
bool dbrec_bayesian_pca_initialize_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("int"); // n: nxn rectangular region will be extracted centered on each ground-truth polygon
  input_types.push_back("int"); // id of the descriptor to be used, pass 0 for normal PCA on 1-d coded image vector
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("dbrec_bayesian_pca_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dbrec_bayesian_pca_initialize_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() != 2) {
    vcl_cerr << "dbrec_bayesian_pca_initialize_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  int n = pro.get_input<int>(i++);
  int desc_id = pro.get_input<int>(i++);
  dbrec_bayesian_pca_sptr ins = new dbrec_bayesian_pca(n, desc_id);
  pro.set_output_val<dbrec_bayesian_pca_sptr>(0, ins);
  return true;
}

//  do the data convertion of images and add to the data set 
bool dbrec_bayesian_pca_add_data_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_bayesian_pca_sptr");
  input_types.push_back("vil_image_view_base_sptr");  // input image
  input_types.push_back("vcl_string"); // path of the ground-truth polygons in the image
  input_types.push_back("vcl_string"); // the type string of the polygons to be used for training
  input_types.push_back("int"); // id of the class (e.g. for a 2 class problem, 0 for class instances and 1 for non-class instances)
  input_types.push_back("int"); // n: nxn rectangular region will be extracted centered on each ground-truth polygon
  input_types.push_back("vcl_string"); // the output path to save the training image patches
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dbrec_bayesian_pca_add_data_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 7) {
    vcl_cerr << "dbrec_pca_add_data_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_bayesian_pca_sptr pca_s = pro.get_input<dbrec_bayesian_pca_sptr>(i++);
  
  vil_image_view_base_sptr inp_img = pro.get_input<vil_image_view_base_sptr>(i++);
  if (inp_img->pixel_format() != VIL_PIXEL_FORMAT_BYTE)
    return false;
   vil_image_view<vxl_byte> orig_img(inp_img);
  if (orig_img.nplanes() == 3) {
    vcl_cout << "In dbrec_bayesian_pca_add_data_process() -- converting input RGB image to grey scale!\n";
    vil_image_view<vxl_byte> *out_img = new vil_image_view<vxl_byte>(inp_img->ni(),inp_img->nj());
    vil_convert_planes_to_grey<vxl_byte, vxl_byte>(*(inp_img.as_pointer()),*out_img);
    inp_img = out_img;
  }
  
  vil_image_view<float> img = *vil_convert_cast(float(), inp_img);
  vil_math_scale_values(img,1.0/255.0);

  vcl_string gt_name = pro.get_input<vcl_string>(i++);
  vcl_string gt_type = pro.get_input<vcl_string>(i++);
  int class_id = pro.get_input<int>(i++);
  int n = pro.get_input<int>(i++);
  vcl_string output_path = pro.get_input<vcl_string>(i++);

  vsl_b_ifstream bif(gt_name);
  bvgl_changes_sptr gt_polys = new bvgl_changes();
  gt_polys->b_read(bif);
  bif.close();

  pca_s->add_image_rectangles(img, n, gt_polys, gt_type, (char)class_id);
  if (!pca_s->check_data()) {
    vcl_cout << "In dbrec_bayesian_pca_add_data_process() -- the data is not consistent!\n";
    return false;
  }
  pca_s->save_image_rectangles(output_path, n, (char)class_id);

  vcl_cout << "In dbrec_bayesian_pca_add_data_process() -- added the image with no problems!\n";

  return true;
}

//  do the data convertion of images and add to the data set 
bool dbrec_bayesian_pca_add_data_process2_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_bayesian_pca_sptr");
  input_types.push_back("vil_image_view_base_sptr");  // input image
  input_types.push_back("vil_image_view_base_sptr");  // input image mask (a bool image)
  input_types.push_back("int");  // the value in the mask to designate class (e.g. 0 for false, 1 for true)
  input_types.push_back("int"); // id of the class (e.g. for a 2 class problem, 0 for class instances and 1 for non-class instances)
  input_types.push_back("int"); // n: nxn rectangular region will be extracted centered on each ground-truth polygon
  input_types.push_back("int"); // the number of class rectangles to be randomly extracted from the designated pixels in the mask
  input_types.push_back("vcl_string"); // the output path to save the training image patches
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dbrec_bayesian_pca_add_data_process2(bprb_func_process& pro)
{
  if (pro.n_inputs() < 8) {
    vcl_cerr << "dbrec_pca_add_data_process2 - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_bayesian_pca_sptr pca_s = pro.get_input<dbrec_bayesian_pca_sptr>(i++);
  
  vil_image_view_base_sptr inp_img = pro.get_input<vil_image_view_base_sptr>(i++);
  if (inp_img->pixel_format() != VIL_PIXEL_FORMAT_BYTE)
    return false;
   vil_image_view<vxl_byte> orig_img(inp_img);
  if (orig_img.nplanes() == 3) {
    vcl_cout << "In dbrec_bayesian_pca_add_data_process2() -- converting input RGB image to grey scale!\n";
    vil_image_view<vxl_byte> *out_img = new vil_image_view<vxl_byte>(inp_img->ni(),inp_img->nj());
    vil_convert_planes_to_grey<vxl_byte, vxl_byte>(*(inp_img.as_pointer()),*out_img);
    inp_img = out_img;
  }
  
  vil_image_view<float> img = *vil_convert_cast(float(), inp_img);
  vil_math_scale_values(img,1.0/255.0);

  vil_image_view_base_sptr inp_img_mask = pro.get_input<vil_image_view_base_sptr>(i++);
  if (inp_img_mask->pixel_format() != VIL_PIXEL_FORMAT_BOOL)
    return false;
  vil_image_view<bool> img_mask(inp_img_mask);

  int class_value = pro.get_input<int>(i++);
  bool class_val = class_value == 0 ? false : true;

  int class_id = pro.get_input<int>(i++);
  int n = pro.get_input<int>(i++);
  int cnt = pro.get_input<int>(i++);
  vcl_string output_path = pro.get_input<vcl_string>(i++);

  pca_s->add_image_rectangles(img, n, img_mask, class_val, (char)class_id, cnt);
  if (!pca_s->check_data()) {
    vcl_cout << "In dbrec_bayesian_pca_add_data_process2() -- the data is not consistent!\n";
    return false;
  }
  pca_s->save_image_rectangles(output_path, n, (char)class_id);

  vcl_cout << "In dbrec_bayesian_pca_add_data_process2() -- added the image with no problems!\n";

  return true;
}

//  do the basis construction and gaussian modeling in the transformed space
bool dbrec_bayesian_pca_construct_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_bayesian_pca_sptr");
  input_types.push_back("double"); // variance_proportion to explain, used to determine number of eigen values to return
  input_types.push_back("int"); // n: nxn rectangular region used as the image patch size
  input_types.push_back("vcl_string"); // the output path to visualize the eigenvectors
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dbrec_bayesian_pca_construct_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 4) {
    vcl_cerr << "dbrec_pca_construct_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_bayesian_pca_sptr ps = pro.get_input<dbrec_bayesian_pca_sptr>(i++);
  double var_pro = pro.get_input<double>(i++);
  int n = pro.get_input<int>(i++);
  vcl_string output_path = pro.get_input<vcl_string>(i++);

  if (!ps->construct_basis_vectors(var_pro, n, output_path)) {
    vcl_cout << "In dbrec_bayesian_pca_construct_process() -- the basis vectors cannot be constructed!\n";
    return false;
  }
  
  return true;
}

//  visualize the gaussian modeling in the transformed space for the given class
bool dbrec_bayesian_pca_visualize_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_bayesian_pca_sptr");
  input_types.push_back("int"); // id of the class (e.g. for a 2 class problem, 0 for class instances and 1 for non-class instances)
  input_types.push_back("vcl_string"); // the output path to plot the histograms
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dbrec_bayesian_pca_visualize_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 3) {
    vcl_cerr << "dbrec_pca_construct_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_bayesian_pca_sptr ps = pro.get_input<dbrec_bayesian_pca_sptr>(i++);
  int class_id = pro.get_input<int>(i++);
  vcl_string output_path = pro.get_input<vcl_string>(i++);

  if (!ps->visualize_transformed_distributions(output_path, (char)class_id)) {
    vcl_cout << "In dbrec_bayesian_pca_construct_process() -- the distributions in the transformed space cannot be visualized!\n";
    return false;
  }

  return true;
}

// classify each pixel in the image as the given class or not by generating a class map as the posterior map of the class
bool dbrec_bayesian_pca_classify_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_bayesian_pca_sptr");
  input_types.push_back("vil_image_view_base_sptr");  // input image
  input_types.push_back("vil_image_view_base_sptr");  // input image mask (shows the valid regions in the image
  input_types.push_back("int"); // id of the class (e.g. for a 2 class problem, 0 for class instances and 1 for non-class instances)
  input_types.push_back("float"); // prior for the class
  input_types.push_back("int"); // n: use nxn rectangular regions
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");  // output posterior map
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dbrec_bayesian_pca_classify_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 6) {
    vcl_cerr << "dbrec_bayesian_pca_classify_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_bayesian_pca_sptr pca_s = pro.get_input<dbrec_bayesian_pca_sptr>(i++);
  
  vil_image_view_base_sptr inp_img = pro.get_input<vil_image_view_base_sptr>(i++);
  if (inp_img->pixel_format() != VIL_PIXEL_FORMAT_BYTE)
    return false;
   vil_image_view<vxl_byte> orig_img(inp_img);
  if (orig_img.nplanes() == 3) {
    vcl_cout << "In dbrec_bayesian_pca_classify_process() -- converting input RGB image to grey scale!\n";
    vil_image_view<vxl_byte> *out_img = new vil_image_view<vxl_byte>(inp_img->ni(),inp_img->nj());
    vil_convert_planes_to_grey<vxl_byte, vxl_byte>(*(inp_img.as_pointer()),*out_img);
    inp_img = out_img;
  }
  
  vil_image_view<float> img = *vil_convert_cast(float(), inp_img);
  vil_math_scale_values(img,1.0/255.0);

  vil_image_view_base_sptr inp_img_mask = pro.get_input<vil_image_view_base_sptr>(i++);
  if (inp_img_mask->pixel_format() != VIL_PIXEL_FORMAT_BOOL)
    return false;
  vil_image_view<bool> img_mask(inp_img_mask);

  int class_id = pro.get_input<int>(i++);
  float class_prior = pro.get_input<float>(i++);
  int n = pro.get_input<int>(i++);

  vil_image_view<float> output_map(img.ni(), img.nj(), 1);
  output_map.fill(0.0f);
  if (!pca_s->classify_image_rectangles(img, img_mask, n, (char)class_id, class_prior, output_map)) {
    vcl_cout << "In dbrec_bayesian_pca_classify_process() -- problems in classifying the image!\n";
    return false;
  }

  float min_value, max_value;
  vil_math_value_range(output_map, min_value, max_value);
  vcl_cout << "\t \t in the output map, min_value: " << min_value << " max_value: " << max_value << vcl_endl;
  
  vil_image_view_base_sptr output_map_sptr = new vil_image_view<float>(output_map);
  pro.set_output_val<vil_image_view_base_sptr>(0, output_map_sptr);
  return true;
}

/////////////////////////////////////////////////////////////////
///////////// PROCESSES for dbrec_template matching /////////////
/////////////////////////////////////////////////////////////////

//  initialize the dbrec_template_matching instance to store the data
bool dbrec_template_matching_initialize_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("dbrec_template_matching_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dbrec_template_matching_initialize_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() != 0) {
    vcl_cerr << "dbrec_template_matching_initialize_process - invalid inputs\n";
    return false;
  }
  dbrec_template_matching_sptr ins = new dbrec_template_matching();
  pro.set_output_val<dbrec_template_matching_sptr>(0, ins);
  return true;
}

//  do the data convertion of images and add to the data set 
bool dbrec_template_matching_add_data_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_template_matching_sptr");
  input_types.push_back("vil_image_view_base_sptr");  // input image
  input_types.push_back("vcl_string"); // path of the ground-truth polygons in the image
  input_types.push_back("vcl_string"); // the type string of the polygons to be used for training
  input_types.push_back("int"); // id of the class (e.g. for a 2 class problem, 0 for class instances and 1 for non-class instances)
  input_types.push_back("int"); // n: nxn rectangular region will be extracted centered on each ground-truth polygon
  input_types.push_back("vcl_string"); // the output path to save the training image patches
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dbrec_template_matching_add_data_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 7) {
    vcl_cerr << "dbrec_template_matching_add_data_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_template_matching_sptr tm_s = pro.get_input<dbrec_template_matching_sptr>(i++);
  
  vil_image_view_base_sptr inp_img = pro.get_input<vil_image_view_base_sptr>(i++);
  if (inp_img->pixel_format() != VIL_PIXEL_FORMAT_BYTE)
    return false;
   vil_image_view<vxl_byte> orig_img(inp_img);
  if (orig_img.nplanes() == 3) {
    vcl_cout << "In dbrec_template_matching_add_data_process() -- converting input RGB image to grey scale!\n";
    vil_image_view<vxl_byte> *out_img = new vil_image_view<vxl_byte>(inp_img->ni(),inp_img->nj());
    vil_convert_planes_to_grey<vxl_byte, vxl_byte>(*(inp_img.as_pointer()),*out_img);
    inp_img = out_img;
  }
  
  vil_image_view<float> img = *vil_convert_cast(float(), inp_img);
  vil_math_scale_values(img,1.0/255.0);

  vcl_string gt_name = pro.get_input<vcl_string>(i++);
  vcl_string gt_type = pro.get_input<vcl_string>(i++);
  int class_id = pro.get_input<int>(i++);
  int n = pro.get_input<int>(i++);
  vcl_string output_path = pro.get_input<vcl_string>(i++);

  vsl_b_ifstream bif(gt_name);
  bvgl_changes_sptr gt_polys = new bvgl_changes();
  gt_polys->b_read(bif);
  bif.close();

  tm_s->add_image_rectangles(img, n, gt_polys, gt_type, (char)class_id);
  if (!tm_s->check_data()) {
    vcl_cout << "In dbrec_template_matching_add_data_process() -- the data is not consistent!\n";
    return false;
  }
  tm_s->save_image_rectangles(output_path, n, (char)class_id);

  vcl_cout << "In dbrec_template_matching_add_data_process() -- added the image with no problems!\n";

  return true;
}

//  do the data convertion of images and add to the data set 
bool dbrec_template_matching_add_data_process2_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_template_matching_sptr");
  input_types.push_back("vil_image_view_base_sptr");  // input image
  input_types.push_back("vil_image_view_base_sptr");  // input image mask (a bool image)
  input_types.push_back("int");  // the value in the mask to designate class (e.g. 0 for false, 1 for true)
  input_types.push_back("int"); // id of the class (e.g. for a 2 class problem, 0 for class instances and 1 for non-class instances)
  input_types.push_back("int"); // n: nxn rectangular region will be extracted centered on each ground-truth polygon
  input_types.push_back("int"); // the number of class rectangles to be randomly extracted from the designated pixels in the mask
  input_types.push_back("vcl_string"); // the output path to save the training image patches
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dbrec_template_matching_add_data_process2(bprb_func_process& pro)
{
  if (pro.n_inputs() < 8) {
    vcl_cerr << "dbrec_template_matching_add_data_process2 - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_template_matching_sptr tm_s = pro.get_input<dbrec_template_matching_sptr>(i++);
  
  vil_image_view_base_sptr inp_img = pro.get_input<vil_image_view_base_sptr>(i++);
  if (inp_img->pixel_format() != VIL_PIXEL_FORMAT_BYTE)
    return false;
   vil_image_view<vxl_byte> orig_img(inp_img);
  if (orig_img.nplanes() == 3) {
    vcl_cout << "In dbrec_template_matching_add_data_process2() -- converting input RGB image to grey scale!\n";
    vil_image_view<vxl_byte> *out_img = new vil_image_view<vxl_byte>(inp_img->ni(),inp_img->nj());
    vil_convert_planes_to_grey<vxl_byte, vxl_byte>(*(inp_img.as_pointer()),*out_img);
    inp_img = out_img;
  }
  
  vil_image_view<float> img = *vil_convert_cast(float(), inp_img);
  vil_math_scale_values(img,1.0/255.0);

  vil_image_view_base_sptr inp_img_mask = pro.get_input<vil_image_view_base_sptr>(i++);
  if (inp_img_mask->pixel_format() != VIL_PIXEL_FORMAT_BOOL)
    return false;
  vil_image_view<bool> img_mask(inp_img_mask);

  int class_value = pro.get_input<int>(i++);
  bool class_val = class_value == 0 ? false : true;

  int class_id = pro.get_input<int>(i++);
  int n = pro.get_input<int>(i++);
  int cnt = pro.get_input<int>(i++);
  vcl_string output_path = pro.get_input<vcl_string>(i++);

  tm_s->add_image_rectangles(img, n, img_mask, class_val, (char)class_id, cnt);
  if (!tm_s->check_data()) {
    vcl_cout << "In dbrec_template_matching_add_data_process2() -- the data is not consistent!\n";
    return false;
  }
  tm_s->save_image_rectangles(output_path, n, (char)class_id);

  vcl_cout << "In dbrec_template_matching_add_data_process2() -- added the image with no problems!\n";

  return true;
}

// classify each pixel in the image as the given class or not by generating a normalized cross correlation value of the nearest neighbor in the training set
bool dbrec_template_matching_classify_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_template_matching_sptr");
  input_types.push_back("vil_image_view_base_sptr");  // input image
  input_types.push_back("vil_image_view_base_sptr");  // input image mask (shows the valid regions in the image
  input_types.push_back("int"); // id of the class (e.g. for a 2 class problem, 0 for class instances and 1 for non-class instances)
  input_types.push_back("int"); // n: use nxn rectangular regions
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");  // output posterior map
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dbrec_template_matching_classify_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 5) {
    vcl_cerr << "dbrec_bayesian_pca_classify_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_template_matching_sptr tm_s = pro.get_input<dbrec_template_matching_sptr>(i++);
  
  vil_image_view_base_sptr inp_img = pro.get_input<vil_image_view_base_sptr>(i++);
  if (inp_img->pixel_format() != VIL_PIXEL_FORMAT_BYTE)
    return false;
   vil_image_view<vxl_byte> orig_img(inp_img);
  if (orig_img.nplanes() == 3) {
    vcl_cout << "In dbrec_template_matching_classify_process() -- converting input RGB image to grey scale!\n";
    vil_image_view<vxl_byte> *out_img = new vil_image_view<vxl_byte>(inp_img->ni(),inp_img->nj());
    vil_convert_planes_to_grey<vxl_byte, vxl_byte>(*(inp_img.as_pointer()),*out_img);
    inp_img = out_img;
  }
  
  vil_image_view<float> img = *vil_convert_cast(float(), inp_img);
  vil_math_scale_values(img,1.0/255.0);

  vil_image_view_base_sptr inp_img_mask = pro.get_input<vil_image_view_base_sptr>(i++);
  if (inp_img_mask->pixel_format() != VIL_PIXEL_FORMAT_BOOL)
    return false;
  vil_image_view<bool> img_mask(inp_img_mask);

  int class_id = pro.get_input<int>(i++);
  int n = pro.get_input<int>(i++);

  vil_image_view<float> output_map(img.ni(), img.nj(), 1);
  output_map.fill(0.0f);
  if (!tm_s->classify_image_rectangles(img, img_mask, n, (char)class_id, output_map)) {
    vcl_cout << "In dbrec_bayesian_pca_classify_process() -- problems in classifying the image!\n";
    return false;
  }

  float min_value, max_value;
  vil_math_value_range(output_map, min_value, max_value);
  vcl_cout << "\t \t in the output map, min_value: " << min_value << " max_value: " << max_value << vcl_endl;
  
  vil_image_view_base_sptr output_map_sptr = new vil_image_view<float>(output_map);
  pro.set_output_val<vil_image_view_base_sptr>(0, output_map_sptr);
  return true;
}


