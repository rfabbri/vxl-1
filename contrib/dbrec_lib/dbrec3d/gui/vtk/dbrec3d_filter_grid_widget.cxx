//:
// \file
// \author Isabel Restrepo
// \date 11-Nov-2010

#include "dbrec3d_filter_grid_widget.h"
#include <QVBoxLayout>

dbrec3d_filter_grid_widget::dbrec3d_filter_grid_widget(const vnl_vector<double> &vector, unsigned dimx, unsigned dimy, unsigned dimz,
                                                       float tf_min, float tf_max)
{
  slider_widget_ = new dbrec3d_double_slider(tf_min,tf_max, "TF");
  slider_widget_->resize(400,100);
  
  vtkRenderWindow *window = vtkRenderWindow::New();
  grid_widget_ = new dbrec3d_regular_grid_widget(window, vector, dimx, dimy, dimz, tf_min, tf_max);
  grid_widget_->interactor_->Initialize();
  
  
  //Put widgets together
  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(grid_widget_);
  layout->addWidget(slider_widget_);
  
  setLayout(layout);
  
  //connect signals and slots
  connect(slider_widget_, SIGNAL(min_threshold_changed(float)), grid_widget_, SLOT(set_min_tf_point(float)));
  connect(slider_widget_, SIGNAL(max_threshold_changed(float)), grid_widget_, SLOT(set_max_tf_point(float)));

  
}

dbrec3d_filter_grid_widget::dbrec3d_filter_grid_widget(bvpl_kernel_sptr kernel, float tf_min, float tf_max)
{
  slider_widget_ = new dbrec3d_double_slider(tf_min,tf_max, "TF");
  slider_widget_->resize(400,100);
  
  vtkRenderWindow *window = vtkRenderWindow::New();
  grid_widget_ = new dbrec3d_regular_grid_widget(window, kernel, tf_min, tf_max);
  grid_widget_->interactor_->Initialize();
  
  
  //Put widgets together
  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(grid_widget_);
  layout->addWidget(slider_widget_);
  
  setLayout(layout);
  
  //connect signals and slots
  connect(slider_widget_, SIGNAL(min_threshold_changed(float)), grid_widget_, SLOT(set_min_tf_point(float)));
  connect(slider_widget_, SIGNAL(max_threshold_changed(float)), grid_widget_, SLOT(set_max_tf_point(float)));
}


dbrec3d_filter_grid_widget::dbrec3d_filter_grid_widget(vnl_vector_fixed<float,5> filter_x, 
                                                       vnl_vector_fixed<float,5> filter_y, 
                                                       vnl_vector_fixed<float,5> filter_z, float tf_min, float tf_max)
{
  slider_widget_ = new dbrec3d_double_slider(tf_min,tf_max, "TF");
  slider_widget_->resize(400,100);
  
  vtkRenderWindow *window = vtkRenderWindow::New();
  grid_widget_ = new dbrec3d_regular_grid_widget(window, filter_x, filter_y, filter_z, tf_min, tf_max);
  grid_widget_->interactor_->Initialize();
  
  
  //Put widgets together
  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(grid_widget_);
  layout->addWidget(slider_widget_);
  
  setLayout(layout);
  
  //connect signals and slots
  connect(slider_widget_, SIGNAL(min_threshold_changed(float)), grid_widget_, SLOT(set_min_tf_point(float)));
  connect(slider_widget_, SIGNAL(max_threshold_changed(float)), grid_widget_, SLOT(set_max_tf_point(float)));
  
  
}


dbrec3d_filter_grid_widget::dbrec3d_filter_grid_widget(vgl_box_3d<double> const &bbox,
                                                       double cell_length,
                                                       vcl_vector<boct_cell_data<short, float> > const &cell_data,
                                                       float min_val, float max_val, unsigned resolution_level)
{
  slider_widget_ = new dbrec3d_double_slider(min_val,max_val, "TF");
  slider_widget_->resize(400,100);
  
  vtkRenderWindow *window = vtkRenderWindow::New();
  grid_widget_ = new dbrec3d_regular_grid_widget(window, bbox, cell_length, cell_data, min_val, max_val, resolution_level);
  grid_widget_->interactor_->Initialize();
  
  
  //Put widgets together
  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(grid_widget_);
  layout->addWidget(slider_widget_);
  
  setLayout(layout);
  
  //connect signals and slots
  connect(slider_widget_, SIGNAL(min_threshold_changed(float)), grid_widget_, SLOT(set_min_tf_point(float)));
  connect(slider_widget_, SIGNAL(max_threshold_changed(float)), grid_widget_, SLOT(set_max_tf_point(float)));
}

dbrec3d_filter_grid_widget::~dbrec3d_filter_grid_widget()
{
  delete slider_widget_;
  delete grid_widget_;
}