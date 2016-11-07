//:
// \file An executable to render objects in a scene
// \author Isabel Restrepo
// \date 20-Sep-2011

#include <QApplication>


#include <dbrec3d/gui/dbrec3d_scene_viewer.h>
#include <dbrec3d/gui/vtk/dbrec3d_region_viewer.h>
#include <boxm/boxm_scene_parser.h>
#include <vul/vul_arg.h>
#include <dbrec3d/bof/bof_scene_categories.h>

int main(int argc, char** argv)
{
  vul_arg<vcl_string> scene_file("-scene_file", "scene filename", "");
  vul_arg<int> scene_id("-scene_id", "scene id", 0);
  vul_arg<vcl_string> bof_dir("-bof_dir", "Bag Of Features xml path", "");
  vul_arg<float> tf_min("-tf_min", "minimum value of opacity function", 0.0);
  vul_arg<float> tf_max("-tf_max", "maximum value of opacity function", 1.0);
  vul_arg<unsigned> resolution_level("-resolution_level", "resolution level", 0);
  vul_arg_parse(argc, argv);
  
  // Read command lines arguments.
  QApplication application(argc,argv);
  
  // Create the scene
  boxm_scene_parser parser;
  
  boxm_scene_base_sptr scene_ptr=new boxm_scene_base();
  scene_ptr->load_scene(scene_file(), parser);
  
  if(scene_ptr->appearence_model() == BOXM_FLOAT){
    //cast the  scene
    typedef boct_tree<short, float > tree_type;
    
    boxm_scene<tree_type>* scene = new boxm_scene<tree_type>();
    scene->load_scene(parser);
    scene_ptr = scene;
    
    
    vcl_vector<vgl_box_3d<double> > bboxes;
    bof_scene_categories category_info(bof_dir());
    category_info.load_objects(scene_id(), bboxes);
    
      
    dbrec3d_region_viewer *main_viewer = new dbrec3d_region_viewer(dynamic_cast<boxm_scene<tree_type>*> (scene_ptr.as_pointer()), bboxes, tf_min(), tf_max(), resolution_level());
    
    main_viewer->show();
  }
    
  // Run main loop.
  return application.exec();
}


