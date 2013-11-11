// This is lemsvxlsrc/contrib/dbrec_lib/dbrec/pro/processes/dbrec_image_hierarchy_processes.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Processes to create/save/load instances of  part hierarchies for 2D images.
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
#include <dbrec/dbrec_gaussian_primitive.h>
#include <dbrec/dbrec_image_hierarchy_factory.h>
#include <dbrec/dbrec_image_compositor.h>
#include <dbrec/dbrec_image_pairwise_models.h>

//: Constructor
// create hierarchy process may use the builder class or create an empty one to be passed to training processes to learn the hierarchy
bool dbrec_image_create_random_hierarchy_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("int");      // ndirs  // if this is zero, then it creates primitives with orientation = 0 so that rot inv creation is done
  input_types.push_back("float");      // lambda_min
  input_types.push_back("float");      // lambda_max
  input_types.push_back("float");      // lambda_inc
  input_types.push_back("int");    // use fast extractor for primitives or not, if 0: normal, 1: fast
  input_types.push_back("int");      // number of classes
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("dbrec_hierarchy_sptr");      // output hierarchy
  ok = pro.set_output_types(output_types);
  return ok;
}


bool dbrec_image_create_random_hierarchy_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 6) {
    vcl_cerr << "dbrec_image_create_random_hierarchy_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  int ndirs = pro.get_input<int>(i++);  // if zero initialize prims for rot inv hierarchy
  float lambda_min = pro.get_input<float>(i++);
  float lambda_max = pro.get_input<float>(i++);
  float lambda_inc = pro.get_input<float>(i++);
  int fast = pro.get_input<int>(i++);
  int n_classes = pro.get_input<int>(i++);

  dbrec_gaussian_factory* gf = new dbrec_gaussian_factory();
  bool use_fast = fast == 0 ? false : true;
  if (ndirs == 0) {
    gf->populate_rot_inv(lambda_min, lambda_max, lambda_inc, true, use_fast);
    gf->populate_rot_inv(lambda_min, lambda_max, lambda_inc, false, use_fast);
  } else {
    gf->populate(ndirs, lambda_min, lambda_max, lambda_inc, true, use_fast);  // populate with both bright and dark operators
    gf->populate(ndirs, lambda_min, lambda_max, lambda_inc, false, use_fast);
  }
  dbrec_hierarchy_sptr h = gf->construct_random_classifier(n_classes);
  pro.set_output_val<dbrec_hierarchy_sptr>(0, h);
  return true;
}

//: Constructor
// create hierarchy process may use the builder class or create an empty one to be passed to training processes to learn the hierarchy
bool dbrec_image_hierarchy_factory_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("int");      // id of the hierarchy in the factory class
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("dbrec_hierarchy_sptr");      // output hierarchy
  ok = pro.set_output_types(output_types);
  return ok;
}


bool dbrec_image_hierarchy_factory_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 1) {
    vcl_cerr << "dbrec_image_create_random_hierarchy_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  int id = pro.get_input<int>(i++);

  dbrec_hierarchy_sptr h;
  switch(id) {
    case 0: { h = dbrec_image_hierarchy_factory::construct_detector_steeple0(); break; } // Aerial Steeple Street data
    case 1: { h = dbrec_image_hierarchy_factory::construct_detector_roi1_0(); break; }   // satellite Baghdad sewage site ROI1, short vehicles
    case 2: { h = dbrec_image_hierarchy_factory::construct_detector_roi1_1(); break; }   // satellite Baghdad sewage site ROI1, long vehicles
    case 3: { h = dbrec_image_hierarchy_factory::construct_detector_roi1_0_downsampled_by_2(); break; } // ROI1, short vehicles, for images downsampled by 2 (ni -> ni/2, nj -> nj/2)
    case 4: { h = dbrec_image_hierarchy_factory::construct_detector_roi1_0_prims(); break; } // only the prims of the roi1_0 detector, required to train appearance models for roi1_0
    case 5: { h = dbrec_image_hierarchy_factory::construct_detector_roi1_0_1(); break; } // another version of roi1_0, has two compositions with a pair of prims, and a higher level that composes these two
    case 6: { h = dbrec_image_hierarchy_factory::construct_detector_roi1_0_1_part(); break; } // roi1_0_1's level 1 compositional part for training 
    case 7: { h = dbrec_image_hierarchy_factory::construct_detector_rot_inv_1(); break; }  // rotationally invariant parts to be used in sampling experiments
    case 8: { h = dbrec_image_hierarchy_factory::construct_detector_rot_inv_1_prims(); break; } // only the prims of the hierarchy in 7
    case 9: { h = dbrec_image_hierarchy_factory::construct_detector_rot_inv_roi1_0_level1(); break; } // rot inv hierarchy for satellite Baghdad sewage site ROI1, short vehicles
    case 10: { h = dbrec_image_hierarchy_factory::construct_detector_rot_inv_roi1_0(); break; } // rot inv hierarchy for satellite Baghdad sewage site ROI1, short vehicles
    case 11: { h = dbrec_image_hierarchy_factory::construct_detector_rot_inv_roi1_2(); break; } // a new detector for both short and longer vehicles
    case 12: { h = dbrec_image_hierarchy_factory::construct_detector_rot_inv_roi1_0_prims(); break; } // rot inv hierarchy for satellite Baghdad sewage site ROI1, short vehicles, only the prims of the hierarchy in 10
    case 13: { h = dbrec_image_hierarchy_factory::construct_detector_rot_inv_haifa_prims(); break; } // rot inv hierarchy for satellite Haifa site, short vehicles, only the prims
    case 14: { h = dbrec_image_hierarchy_factory::construct_detector_rot_inv_haifa_prims2(); break; } // rot inv hierarchy for satellite Haifa site, short vehicles, only the prims
    default: { vcl_cout << "In dbrec_image_hierarchy_factory_process() -- id: " << id << " not recognized!\n"; }
  }
  
  pro.set_output_val<dbrec_hierarchy_sptr>(0, h);
  return true;
}

//: Constructor
// create hierarchy process may use the builder class or create an empty one to be passed to training processes to learn the hierarchy
bool dbrec_image_hierarchy_factory_rotated_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("int");      // id of the hierarchy in the factory class
  input_types.push_back("float");      // orientation angle, the factory generates the primitives in the hierarchy with the given orientation angle
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  output_types.push_back("dbrec_hierarchy_sptr");      // output hierarchy
  ok = pro.set_output_types(output_types);
  return ok;
}


bool dbrec_image_hierarchy_factory_rotated_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 2) {
    vcl_cerr << "dbrec_image_create_random_hierarchy_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  int id = pro.get_input<int>(i++);
  float or_angle = pro.get_input<float>(i++);

  dbrec_hierarchy_sptr h;
  switch(id) {
    case 0: { h = dbrec_image_hierarchy_factory::construct_detector_haifa(or_angle); break; } //same as roi1_0 detector, the direction for the primitives is set to the passed angle
    default: { vcl_cout << "In dbrec_image_hierarchy_factory_process() -- id: " << id << " not recognized!\n"; }
  }
  
  pro.set_output_val<dbrec_hierarchy_sptr>(0, h);
  return true;
}



//: Constructor
// create a new hierarchy from a given hierarchy by generating all pairwise combinations of parts at the level right below the root nodes for each class 
// treat parts of different classes differently, and generate compositional part instances from the parts of the class only
// construct an OR node for each class using all the parts at its new pool
bool dbrec_image_populate_hierarchy_pairwise_central_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_hierarchy_sptr");      // original hierarchy
  input_types.push_back("float");      // radius of the new compositional parts (usually depends on depth but may also be customized.)
  input_types.push_back("unsigned");  // number of bins on the angle axis of the joint histogram in the pairwise model
  input_types.push_back("unsigned");  // number of bins on the distance axis of the joint histogram in the pairwise model
  input_types.push_back("float");  // minimum distance of the joint histogram in the pairwise model
  input_types.push_back("float");  // maximum distance of the joint histogram in the pairwise model
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  output_types.push_back("dbrec_hierarchy_sptr");      // output hierarchy
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_image_populate_hierarchy_pairwise_central_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 6) {
    vcl_cerr << "dbrec_image_create_random_hierarchy_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_hierarchy_sptr orig_h = pro.get_input<dbrec_hierarchy_sptr>(i++);
  float radius = pro.get_input<float>(i++);
  unsigned nbins_a = pro.get_input<unsigned>(i++);
  unsigned nbins_dist = pro.get_input<unsigned>(i++);
  float min_dist = pro.get_input<float>(i++);
  float max_dist = pro.get_input<float>(i++);

  dbrec_pairwise_discrete_model_factory mf(nbins_a, min_dist, max_dist, nbins_dist);
  dbrec_hierarchy_sptr new_h = populate_class_based_central_compositor(orig_h, radius, mf);
  pro.set_output_val<dbrec_hierarchy_sptr>(0, new_h);
  return true;
}

//: Constructor
// create a new hierarchy from a given hierarchy by generating sampled compositional parts from the existing ones at the level right below the root nodes for each class 
// treat parts of different classes differently, and generate compositional part instances from the parts of the class only
// construct an OR node for each class using all the parts at its new pool
bool dbrec_image_populate_sample_hierarchy_pairwise_central_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_hierarchy_sptr");      // original hierarchy
  input_types.push_back("unsigned");  // number of compositional parts to create from an existing compositional part through sampling from its distribution
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  output_types.push_back("dbrec_hierarchy_sptr");      // output hierarchy
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_image_populate_sample_hierarchy_pairwise_central_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 2) {
    vcl_cerr << "dbrec_image_populate_sample_hierarchy_pairwise_central_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_hierarchy_sptr orig_h = pro.get_input<dbrec_hierarchy_sptr>(i++);
  unsigned n = pro.get_input<unsigned>(i++);
  
  dbrec_hierarchy_sptr new_h = populate_class_based_central_compositor_by_sampling(orig_h, n);
  pro.set_output_val<dbrec_hierarchy_sptr>(0, new_h);
  return true;
}

bool dbrec_image_populate_sample_indep_gaus_hierarchy_pairwise_central_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_hierarchy_sptr");      // original hierarchy
  input_types.push_back("unsigned");  // number of compositional parts to create from an existing compositional part through sampling from its distribution
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  output_types.push_back("dbrec_hierarchy_sptr");      // output hierarchy
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_image_populate_sample_indep_gaus_hierarchy_pairwise_central_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 2) {
    vcl_cerr << "dbrec_image_populate_sample_indep_gaus_hierarchy_pairwise_central_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_hierarchy_sptr orig_h = pro.get_input<dbrec_hierarchy_sptr>(i++);
  unsigned n = pro.get_input<unsigned>(i++);
  
  dbrec_hierarchy_sptr new_h = populate_class_based_central_compositor_by_indep_gaussian_model(orig_h, n);
  pro.set_output_val<dbrec_hierarchy_sptr>(0, new_h);
  return true;
}


