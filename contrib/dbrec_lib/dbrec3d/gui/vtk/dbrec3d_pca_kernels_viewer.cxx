//:
// \file
// \author Isabel Restrepo
// \date 11-Nov-2010

#include "dbrec3d_pca_kernels_viewer.h"
#include <QKeyEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDockWidget>
#include <vnl/vnl_vector.h>
#include <QStatusBar>


dbrec3d_pca_kernels_viewer::dbrec3d_pca_kernels_viewer(const vnl_matrix<double> &PC,const vnl_vector<double> &weights,
                                                       unsigned dimx, unsigned dimy, unsigned dimz, float tf_min, float tf_max)
{
  //initialize the widget to display the first principal component
  PC_=PC;
  curr_pc_ = 0;
  pc_widget_ = new dbrec3d_filter_grid_widget(PC_.get_column(curr_pc_), dimx, dimy, dimz, tf_min, tf_max);
  hist_widget_ = new dbrec3d_histogram_widget(PC_.get_column(curr_pc_), tf_min, tf_max, 5);
  weights_widget_ = new dbrec3d_line_plot(weights.data_block(), weights.size());
  component_label_ = new QLabel;
  component_label_->setNum((int)curr_pc_);
    
  QDockWidget *hist_dock = new QDockWidget(tr("Histogram Principal Component"), this);
  hist_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  hist_dock->setWidget(hist_widget_);
  hist_dock->setFloating(false);
  addDockWidget(Qt::RightDockWidgetArea, hist_dock);
  
  QDockWidget *weight_dock = new QDockWidget(tr("Weight of Principal Component"), this);
  weight_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  weight_dock->setWidget(weights_widget_);
  weight_dock->setFloating(false);
  addDockWidget(Qt::RightDockWidgetArea, weight_dock);
  
  //setLayout(layout);
  setCentralWidget(pc_widget_);
  statusBar()->addWidget(new QLabel("Principal Component No: "));
  statusBar()->addWidget(component_label_);
  setGeometry(300,200,1200,1000);
  setWindowTitle("PCA Visualizer");
  
  
  //connect signals and slots
  connect(pc_widget_->grid_widget_, SIGNAL(request_next_volume()), this, SLOT(next_volume()));
  connect(pc_widget_->grid_widget_, SIGNAL(request_prev_volume()), this, SLOT(prev_volume()));

}

void dbrec3d_pca_kernels_viewer::next_volume()
{
  if(curr_pc_ < PC_.cols() - 1)
    curr_pc_++;
  else
    curr_pc_ = 0; 
  
  component_label_->setNum((int)curr_pc_);
  vnl_vector<double> pc = PC_.get_column(curr_pc_);
  pc_widget_->grid_widget_->update_volume_data(pc);
  hist_widget_->update_histogram_data(pc);
  weights_widget_->set_active_datapoint(curr_pc_);
  
  vcl_cout << "Principal Component: " << curr_pc_ << ":\n"
  << "Magnitude: " << pc.two_norm() << ", Mean: " << pc.mean() << ", Min: " << pc.min_value() << ", Max: " << pc.max_value() << "\n"
  << pc << vcl_endl;
  
}

void dbrec3d_pca_kernels_viewer::prev_volume()
{
  if(curr_pc_ > 0)
    curr_pc_--;
  else
    curr_pc_ = PC_.cols(); 
  
  component_label_->setNum((int)curr_pc_);
  vnl_vector<double> pc = PC_.get_column(curr_pc_);
  pc_widget_->grid_widget_->update_volume_data(pc);
  hist_widget_->update_histogram_data(pc);
  weights_widget_->set_active_datapoint(curr_pc_);

  vcl_cout << "Principal Component: " << curr_pc_ << ":\n"
  << "Magnitude: " << pc.two_norm() << ", Mean: " << pc.mean() << ", Min: " << pc.min_value() << ", Max: " << pc.max_value() << "\n"
  << pc << vcl_endl;
}
