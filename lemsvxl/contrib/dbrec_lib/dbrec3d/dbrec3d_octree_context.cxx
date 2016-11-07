//:
// \file
// \author Isabel Restrepo
// \date 16-Jun-2010

#include "dbrec3d_octree_context.h"
#include "dbrec3d_parts_manager.h"

#include <vsl/vsl_binary_io.h>


/************* Binary IO ***********************************/
void vsl_b_write(vsl_b_ostream & os, dbrec3d_context const &c){}


void vsl_b_write(vsl_b_ostream & os, dbrec3d_context const * &c){}


void vsl_b_read(vsl_b_istream & is, dbrec3d_context &c){}

void vsl_b_read(vsl_b_istream & is, dbrec3d_context *c){}


vcl_ostream& operator << (vcl_ostream& os, const dbrec3d_context &c){ return os; }

/*************** XML IO *************************************/

//: XML read
dbrec3d_context_sptr dbrec3d_context_parse_xml_element(bxml_data_sptr d)
{
  bxml_element query("dbrec3d_octree_context");
  bxml_data_sptr root = bxml_find_by_name(d, query);
  if (!root || root->type() != bxml_data::ELEMENT) {
    return NULL;
  }
  bxml_element* root_elm = dynamic_cast<bxml_element*>(root.ptr());
  
  //get the variables
  vcl_string scene_path = "", scene_file = "";
  bool exclusive = false;
  int hierarchy_level = -1, part_id = -1;
  root_elm->get_attribute("scene_file", scene_file);
  root_elm->get_attribute("exclusive", exclusive);
  root_elm->get_attribute("hierarchy_level", hierarchy_level);
  root_elm->get_attribute("part_id", part_id);

  dbrec3d_part_sptr part = PARTS_MANAGER->get_part(part_id);
  
  //parse the scene
  boxm_scene_parser parser;
  
  boxm_scene_base_sptr scene_ptr=new boxm_scene_base();
  scene_ptr->load_scene(scene_file, parser);
  if (scene_ptr->appearence_model() == DBREC3D_PAIR_COMPOSITE_INSTANCE) {
    typedef boct_tree<short, dbrec3d_pair_composite_instance  > tree_type;
    boxm_scene<tree_type>* scene = new boxm_scene<tree_type>(*scene_ptr);
    scene->load_scene(parser);
    if(scene)
      return new dbrec3d_octree_context<dbrec3d_pair_composite_instance>(*scene, exclusive, part,hierarchy_level);
  }
  else if (scene_ptr->appearence_model() == DBREC3D_PART_INSTANCE) {
    typedef boct_tree<short, dbrec3d_part_instance  > tree_type;
    boxm_scene<tree_type>* scene = new boxm_scene<tree_type>(*scene_ptr);
    scene->load_scene(parser);
    if(scene)
      return new dbrec3d_octree_context<dbrec3d_part_instance>(*scene, exclusive, part,hierarchy_level);
    
  }
  else {
    vcl_cerr << "dbrec3d_create_scene_process: undefined datatype\n";
    return NULL;
  }  
  
}
