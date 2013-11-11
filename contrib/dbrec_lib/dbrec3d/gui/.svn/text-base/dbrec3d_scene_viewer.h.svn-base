// This is dbrec3d_main_viewer.h
#ifndef dbrec3d_main_viewer_h
#define dbrec3d_main_viewer_h

//:
// \file
// \brief A class that implements the dbrec3d_part concept for composite parts (i.e non-leafs). 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  25-Oct-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <QMainWindow>


#include "dbrec3d_scene_model.h"
#include "dbrec3d_scene_widgets.h"
#include "dbrec3d_histogram_widgets.h"


//: The main widget that contains, the scene widget and its histogram
class dbrec3d_main_viewer: public QMainWindow
{
  Q_OBJECT
  
public:
  dbrec3d_main_viewer(dbrec3d_scene_model *scene_data);
 
private:
  dbrec3d_scene_model *scene_data_;
  dbrec3d_gradient_scene_widget *scene_widget_;
  dbrec3d_scene_info_widget *scene_info_widget_;
};


//: The main widget that contains, the scene widget and its histogram
class dbrec3d_id_main_viewer: public QMainWindow
{
  Q_OBJECT
  
public:
  dbrec3d_id_main_viewer(dbrec3d_scene_model *scene_data);
  
private:
  dbrec3d_scene_model *scene_data_;
  dbrec3d_id_scene_widget *scene_widget_;
  dbrec3d_id_scene_info_widget *scene_info_widget_;
};

#endif
