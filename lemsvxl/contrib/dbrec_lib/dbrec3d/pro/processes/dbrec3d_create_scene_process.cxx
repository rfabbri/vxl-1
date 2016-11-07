//This is brl/bseg/bvpl/bvpl_octree/pro/processes/dbrec3d_create_scene_process.cxx
#include <boxm/boxm_scene_base.h>
//:
// \file
// \brief A process loading xml parameters of a boxm_scene of types specific to dberc3d
//
// \author Isabel Restrepo
// \date Sept 6, 2010
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <vcl_string.h>

#include <boxm/boxm_scene.h>
#include <boxm/boxm_scene_parser.h>
#include <dbrec3d/dbrec3d_part_instance.h>

//:global variables
namespace dbrec3d_create_scene_process_globals
{
  //this process takes no inputs
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 1;
}

//:sets input and output types
bool dbrec3d_create_scene_process_cons(bprb_func_process& pro)
{
  using namespace dbrec3d_create_scene_process_globals;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "boxm_scene_base_sptr";
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//:creates a scene from parameters
bool dbrec3d_create_scene_process(bprb_func_process& pro)
{
  using namespace dbrec3d_create_scene_process_globals;
  vcl_string fname = pro.get_input<vcl_string>(0);
  
  boxm_scene_parser parser;
  
  boxm_scene_base_sptr scene_ptr=new boxm_scene_base();
  scene_ptr->load_scene(fname, parser);
  if (scene_ptr->appearence_model() == DBREC3D_PART_INSTANCE) 
  {
    typedef boct_tree<short, dbrec3d_part_instance  > tree_type;
    boxm_scene<tree_type>* scene = new boxm_scene<tree_type>(*scene_ptr);
    scene->load_scene(parser);
    //store output
    pro.set_output_val<boxm_scene_base_sptr>(0, scene);
    return true;
  }
 else if(scene_ptr->appearence_model() == DBREC3D_PAIR_COMPOSITE_INSTANCE) 
 {
    typedef boct_tree<short, dbrec3d_pair_composite_instance  > tree_type;
    boxm_scene<tree_type>* scene = new boxm_scene<tree_type>(*scene_ptr);
    scene->load_scene(parser);
    //store output
    pro.set_output_val<boxm_scene_base_sptr>(0, scene);
    return true;
  }
  else {
    vcl_cerr << "dbrec3d_create_scene_process: undefined datatype\n";
    return false;
 }
  
  
}
