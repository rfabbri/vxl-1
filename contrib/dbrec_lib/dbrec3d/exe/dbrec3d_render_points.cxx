//:
// \brief An executable to render points in a 3d scene
// \file
// \author Isabel Restrepo
// \date 14-Jan-2011

#include <QApplication>


#include <dbrec3d/gui/dbrec3d_scene_widgets.h>
#include <boxm/boxm_scene_parser.h>
#include <vul/vul_arg.h>


int main(int argc, char** argv)
{
  vul_arg<vcl_string> loc_file("-loc_file", "locations filename", "");
  vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
  vul_arg_parse(argc, argv);
  
  // Create the scene
  boxm_scene_parser parser;
  
  boxm_scene_base_sptr scene_ptr=new boxm_scene_base();
  scene_ptr->load_scene(scene_file(), parser);
  
  // Read command lines arguments.
  QApplication application(argc,argv);
  
  dbrec3d_points_scene_widget *points_viewer = new dbrec3d_points_scene_widget(loc_file(), scene_ptr);
  
  points_viewer->show();
  
  // Run main loop.
  return application.exec();
}