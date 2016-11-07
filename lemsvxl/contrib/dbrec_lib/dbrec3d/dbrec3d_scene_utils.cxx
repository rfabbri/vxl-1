//:
// \file
// \author Isabel Restrepo
// \date 6-Sep-2010

#include "dbrec3d_scene_utils.h"
#include <boxm/boxm_scene_parser.h>
#include "dbrec3d_part_instance.h"

boxm_scene_base_sptr dbrec3d_load_scene(vcl_string filename)
{
  boxm_scene_parser parser;

  boxm_scene_base_sptr scene_ptr=new boxm_scene_base();
  scene_ptr->load_scene(filename, parser);
  if (scene_ptr->appearence_model() == DBREC3D_PAIR_COMPOSITE_INSTANCE) {
    typedef boct_tree<short, dbrec3d_pair_composite_instance  > tree_type;
    boxm_scene<tree_type>* scene = new boxm_scene<tree_type>();
    scene->load_scene(parser);
    scene_ptr = scene;
  }
  else if (scene_ptr->appearence_model() == DBREC3D_PART_INSTANCE) {
    typedef boct_tree<short, dbrec3d_part_instance  > tree_type;
    boxm_scene<tree_type>* scene = new boxm_scene<tree_type>();
    scene->load_scene(parser);
    scene_ptr = scene;
  }
  else {
    vcl_cerr << "dbrec3d_create_scene_process: undefined datatype\n";
    return false;
  }
  return 0;
}