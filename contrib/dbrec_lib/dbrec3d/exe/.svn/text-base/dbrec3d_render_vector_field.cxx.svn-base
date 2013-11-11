//:
// \file
// \brief An executable to render a bvpl_response scene as a vector field
// \author Isabel Restrepo mir@lems.brown.edu
// \date  Octorber 12, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <QApplication>
#include <QGLFormat>
#include <QWidget>
#include <QLabel>
#include <QMainWindow>
#include <QHBoxLayout>
#include <vcl_iostream.h>

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
 
  if(scene_ptr->appearence_model() == VNL_FLOAT_3){
    //cast the gradient scene
    typedef vnl_vector_fixed< float,3 > datatype;
    typedef boct_tree<short, datatype > tree_type;
    
    boxm_scene<tree_type>* scene = new boxm_scene<tree_type>();
    scene->load_scene(parser);
    scene_ptr = scene;
  }
  
  if(scene_ptr->appearence_model() == VNL_FLOAT_10){
    //cast the gradient scene
    typedef vnl_vector_fixed< float,10 > datatype;
    typedef boct_tree<short, datatype > tree_type;
    
    boxm_scene<tree_type>* scene = new boxm_scene<tree_type>();
    scene->load_scene(parser);
    scene_ptr = scene;
  }
  
  // Read command lines arguments.
  QApplication application(argc,argv);
  
  dbrec3d_scene_model *scene_model = new dbrec3d_scene_model(scene_ptr);
  dbrec3d_main_viewer *main_viewer = new dbrec3d_main_viewer(scene_model);
  
  main_viewer->show();
  
  // Run main loop.
  return application.exec();
}

