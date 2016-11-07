//:
// \file An executable to render an id scene as different colored points
// \author Isabel Restrepo
// \date 20-Sep-2011

#include <QApplication>


#include <dbrec3d/gui/dbrec3d_scene_viewer.h>
#include <boxm/boxm_scene_parser.h>
#include <vul/vul_arg.h>


int main(int argc, char** argv)
{
  vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
  vul_arg_parse(argc, argv);
  
  // Create the scene
  boxm_scene_parser parser;
  
  boxm_scene_base_sptr scene_ptr=new boxm_scene_base();
  scene_ptr->load_scene(scene_file(), parser);
  
  if(scene_ptr->appearence_model() == BOXM_SHORT){
    //cast the  scene
    typedef boct_tree<short, short > tree_type;
    
    boxm_scene<tree_type>* scene = new boxm_scene<tree_type>();
    scene->load_scene(parser);
    scene_ptr = scene;
  }
  
  // Read command lines arguments.
  QApplication application(argc,argv);
  
  dbrec3d_scene_model *scene_model = new dbrec3d_scene_model(scene_ptr);
  dbrec3d_id_main_viewer *main_viewer = new dbrec3d_id_main_viewer(scene_model);
  
  main_viewer->show();
  
  // Run main loop.
  return application.exec();
}