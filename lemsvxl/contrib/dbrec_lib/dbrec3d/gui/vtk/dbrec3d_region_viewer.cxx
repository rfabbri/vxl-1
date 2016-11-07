//:
// \file
// \author Isabel Restrepo
// \date 4-Oct-2011

#include "dbrec3d_region_viewer.h"


dbrec3d_region_viewer::  dbrec3d_region_viewer(boxm_scene<boct_tree< short, float > > *scene, vcl_vector<vgl_box_3d<double> > bboxes, float tf_min, float tf_max, unsigned resolution_level)
{
  scene_= scene;
  cell_length_ = scene_->cell_length(resolution_level);
  resolution_level_ = resolution_level;
  
  bboxes_= bboxes;
  curr_bboxes_it_ = bboxes_.begin();
  vcl_vector<boct_cell_data<short, float> > cell_data;
  scene_->cell_data_in_region(*curr_bboxes_it_, cell_data,resolution_level_);
  region_widget_ = new dbrec3d_filter_grid_widget(*curr_bboxes_it_, cell_length_, cell_data, tf_min, tf_max, resolution_level_);
  
  //setLayout(layout);
  setCentralWidget(region_widget_);
  setGeometry(300,200,1200,1000);
  setWindowTitle("Kernel Visualizer");
  
  
  //connect signals and slots
  connect(region_widget_->grid_widget_, SIGNAL(request_next_volume()), this, SLOT(next_volume()));
  connect(region_widget_->grid_widget_, SIGNAL(request_prev_volume()), this, SLOT(prev_volume()));
  
  
}

void dbrec3d_region_viewer::next_volume()
{
  curr_bboxes_it_++;
  if(curr_bboxes_it_== bboxes_.end())
    curr_bboxes_it_ = bboxes_.begin();
  
  
  vcl_vector<boct_cell_data<short, float> > cell_data;
  //scene_->leaves_data_in_region(*curr_bboxes_it_, cell_data);
  scene_->cell_data_in_region(*curr_bboxes_it_, cell_data,resolution_level_);
  
  region_widget_->grid_widget_->update_volume_data(*curr_bboxes_it_, cell_length_, cell_data,resolution_level_);
  
}

void dbrec3d_region_viewer::prev_volume()
{
  if(curr_bboxes_it_==bboxes_.begin())
    curr_bboxes_it_ = bboxes_.end();
  
  curr_bboxes_it_--;
  
  vcl_vector<boct_cell_data<short, float> > cell_data;
  //scene_->leaves_data_in_region(*curr_bboxes_it_, cell_data);
  scene_->cell_data_in_region(*curr_bboxes_it_, cell_data,resolution_level_);

  region_widget_->grid_widget_->update_volume_data(*curr_bboxes_it_, cell_length_, cell_data,resolution_level_);
}
