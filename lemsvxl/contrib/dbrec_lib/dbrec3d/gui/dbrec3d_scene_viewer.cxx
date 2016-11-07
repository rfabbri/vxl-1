//:
// \file
// \author Isabel Restrepo
// \date 25-Oct-2010

#include <QHBoxLayout>
#include <QDockWidget>
#include <QSplitter>
#include "dbrec3d_scene_viewer.h"

/********************* main viewer *************************/

dbrec3d_main_viewer::dbrec3d_main_viewer(dbrec3d_scene_model *scene_data)
{
  //Initilized "child" widgets
  
  //Scene widget
  scene_data_ = scene_data;  
  scene_info_widget_ = new dbrec3d_scene_info_widget(scene_data_);
  
//  dbrec3d_filter_dir_widget *dir_widget = new dbrec3d_filter_dir_widget(scene_data_,this, NULL);
//  QDockWidget *dir_dock = new QDockWidget(tr("Dir Info"), this);
//  dir_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
//  dir_dock->setWidget(dir_widget);
//  dir_dock->setFloating(true);
//  addDockWidget(Qt::LeftDockWidgetArea, dir_dock);
  
  
  QDockWidget *info_dock = new QDockWidget(tr("Scene Info"), this);
  info_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  info_dock->setWidget(scene_info_widget_);
  info_dock->setFloating(true);
  addDockWidget(Qt::RightDockWidgetArea, info_dock);
  
  
  scene_widget_ = new dbrec3d_gradient_scene_widget(scene_data_,this, NULL);
  setCentralWidget(scene_widget_);
 
  setGeometry(300,200,1200,1000);
  setWindowTitle("Boxm Scene Visualizer");
  
  //Missing: Connect signals and slots
  connect(scene_info_widget_, SIGNAL(min_threshold_changed(int)), scene_widget_, SLOT(set_low_thresh(int)));
  connect(scene_info_widget_, SIGNAL(max_threshold_changed(int)), scene_widget_, SLOT(set_high_thresh(int)));
  connect(scene_info_widget_, SIGNAL(polar_changed(int)), scene_widget_, SLOT(set_polar(int)));
  connect(scene_info_widget_, SIGNAL(azimuth_changed(int)), scene_widget_, SLOT(set_azimuth(int)));
  connect(scene_info_widget_, SIGNAL(dir_filter_state_changed(bool)), scene_widget_, SLOT(set_filter_by_dir(bool)));
  
  scene_widget_->set_low_thresh(scene_info_widget_->current_low_threshold());
  scene_widget_->set_high_thresh(scene_info_widget_->current_high_threshold());
  
}


dbrec3d_id_main_viewer::dbrec3d_id_main_viewer(dbrec3d_scene_model *scene_data)
{
  //Initilized "child" widgets
  
  //Scene widget
  scene_data_ = scene_data;  
  scene_info_widget_ = new dbrec3d_id_scene_info_widget(scene_data);
  
  QDockWidget *info_dock = new QDockWidget(tr("Scene Info"), this);
  info_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  info_dock->setWidget(scene_info_widget_);
  info_dock->setFloating(true);
  addDockWidget(Qt::RightDockWidgetArea, info_dock);
  
  
  scene_widget_ = new dbrec3d_id_scene_widget(scene_data_,this, NULL);
  setCentralWidget(scene_widget_);
  
  setGeometry(300,200,1200,1000);
  setWindowTitle("Boxm Scene Visualizer");
  
  //Missing: Connect signals and slots
  connect(scene_info_widget_, SIGNAL(min_threshold_changed(int)), scene_widget_, SLOT(set_low_thresh(int)));
  connect(scene_info_widget_, SIGNAL(max_threshold_changed(int)), scene_widget_, SLOT(set_high_thresh(int)));
  
  scene_widget_->set_low_thresh(scene_info_widget_->current_low_threshold());
  scene_widget_->set_high_thresh(scene_info_widget_->current_high_threshold());
  
}
