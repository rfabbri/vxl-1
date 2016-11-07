//:
// \brief An executable that shows the histogram of a scene
// \file
// \author Isabel Restrepo
// \date 29-Nov-2010

#include <QApplication>
#include <QWidget>

#include <vcl_iostream.h>

#include <dbrec3d/gui/dbrec3d_scene_model.h>
#include <dbrec3d/gui/dbrec3d_histogram_widgets.h>

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
  
  //cast the gradient scene
  typedef vnl_vector_fixed< float,3 > datatype;
  typedef boct_tree<short, datatype > tree_type;
  boxm_scene<tree_type>* scene = new boxm_scene<tree_type>();
  scene->load_scene(parser);
  scene_ptr = scene;
  
  // Read command lines arguments.
  QApplication application(argc,argv);
  
  dbrec3d_scene_model *scene_model = new dbrec3d_scene_model(scene_ptr);
  dbrec3d_histogram_widget *histogram_widget = new dbrec3d_histogram_widget(scene_model);
  
  histogram_widget->show();
  
  // Run main loop.
  return application.exec();
}