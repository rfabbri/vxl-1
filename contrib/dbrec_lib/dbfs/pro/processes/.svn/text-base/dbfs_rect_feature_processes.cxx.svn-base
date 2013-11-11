// This is lemsvxlsrc/contrib/dbrec_lib/dbrec/pro/processes/dbrec_hierarchy_processes.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Processes to create/save/load instances of  part hierarchies.
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

#include <dbfs/dbfs_rect_feature.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>

//: Constructor
bool dbfs_create_rect_features_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  output_types.push_back("dbfs_rect_feature_set_sptr");      // output hierarchy
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbfs_create_rect_features_process(bprb_func_process& pro)
{
  if (pro.n_inputs() != 0) {
    vcl_cerr << "dbfs_create_rect_features_process - invalid inputs\n";
    return false;
  }
  dbfs_rect_feature_set_sptr fs = new dbfs_rect_feature_set();
  pro.set_output_val<dbfs_rect_feature_set_sptr>(0, fs);
  return true;
}

//: Constructor
// extract rectangular features from the given image
bool dbfs_populate_rect_features_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");
  input_types.push_back("vil_image_view_base_sptr");  // ground truth map for foreground or foreground map, a float img
  input_types.push_back("dbfs_rect_feature_set_sptr");    
  input_types.push_back("int");
  input_types.push_back("int");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbfs_populate_rect_features_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 5) {
    vcl_cerr << "dbfs_populate_rect_features_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  vcl_cout << "In populate features!\n";
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  if (img->pixel_format() != VIL_PIXEL_FORMAT_BYTE) {
    vcl_cerr << "dbfs_populate_rect_features_process - Img is not in BYTE format!\n";
    return false;
  }
  vcl_cout << "In populate features!\n";
  if (img->nplanes() == 3) {
    vcl_cerr << "dbfs_populate_rect_features_process - Img is not a Grey img!\n";
    return false;
  }
vcl_cout << "In populate features!\n";
  vil_image_view_base_sptr img_f = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<float> img_fg;
  if (img_f->pixel_format() == VIL_PIXEL_FORMAT_BOOL) {
    img_fg = *(vil_convert_cast(float(), img_f));
  } else if (img_f->pixel_format() != VIL_PIXEL_FORMAT_FLOAT) 
    return false;
  else
    img_fg = img_f;
  
vcl_cout << "In populate features!\n";
  dbfs_rect_feature_set_sptr fs = pro.get_input<dbfs_rect_feature_set_sptr>(i++);
  int w = pro.get_input<int>(i++);
  int h = pro.get_input<int>(i++);
  fs->populate(img, img_fg, w, h);
  return true;
}

// create image patches of the features in the feature set
bool dbfs_display_rect_features_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbfs_rect_feature_set_sptr");    
  input_types.push_back("vcl_string");    // output prefix
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbfs_display_rect_features_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 2) {
    vcl_cerr << "dbfs_display_rect_features_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbfs_rect_feature_set_sptr fs = pro.get_input<dbfs_rect_feature_set_sptr>(i++);
  vcl_string prefix = pro.get_input<vcl_string>(i++);
  vcl_cout << "------Displaying " << fs->size() << " patches!\n";
  for (unsigned i = 0; i < fs->size(); i++) {
    dbfs_rect_feature_sptr f = fs->get_feature(i);
    vil_image_resource_sptr img_r = f->create_patch();
    vcl_stringstream ids; ids << i;
    vcl_string name = prefix + "_" + ids.str() + ".png";
    vil_save_image_resource(img_r, name.c_str());
  }
  return true;
}

