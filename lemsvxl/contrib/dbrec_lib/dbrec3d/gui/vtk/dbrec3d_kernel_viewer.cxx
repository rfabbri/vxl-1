//:
// \file
// \author Isabel Restrepo
// \date 27-Jan-2011

#include "dbrec3d_kernel_viewer.h"

//:
// \file
// \author Isabel Restrepo
// \date 11-Nov-2010

#include "dbrec3d_kernel_viewer.h"
#include <QKeyEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDockWidget>
#include <vnl/vnl_vector.h>
#include <QStatusBar>

dbrec3d_kernel_viewer::dbrec3d_kernel_viewer(vcl_map<vcl_string, bvpl_kernel_sptr> &kernels, float tf_min, float tf_max)
{
  //initialize the widget to display the first principal component
  kernels_= kernels;
  curr_kernel_it_ = kernels_.begin();
  curr_kernel_it_->second->print();
  kernel_widget_ = new dbrec3d_filter_grid_widget((*curr_kernel_it_).second, tf_min, tf_max);
  hist_widget_ = new dbrec3d_histogram_widget((*curr_kernel_it_).second, tf_min, tf_max, 5);
  component_label_ = new QLabel;
  component_label_->setText((*curr_kernel_it_).first.c_str());
  
  QDockWidget *hist_dock = new QDockWidget(tr("Histogram Kernel Values"), this);
  hist_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  hist_dock->setWidget(hist_widget_);
  hist_dock->setFloating(false);
  addDockWidget(Qt::RightDockWidgetArea, hist_dock);
  
  //setLayout(layout);
  setCentralWidget(kernel_widget_);
  statusBar()->addWidget(new QLabel("Kernel: "));
  statusBar()->addWidget(component_label_);
  setGeometry(300,200,1200,1000);
  setWindowTitle("Kernel Visualizer");
  
  
  //connect signals and slots
  connect(kernel_widget_->grid_widget_, SIGNAL(request_next_volume()), this, SLOT(next_volume()));
  connect(kernel_widget_->grid_widget_, SIGNAL(request_prev_volume()), this, SLOT(prev_volume()));
  
}

void dbrec3d_kernel_viewer::next_volume()
{
  curr_kernel_it_++;
  if(curr_kernel_it_== kernels_.end())
    curr_kernel_it_ = kernels_.begin();

  
  component_label_->setText((*curr_kernel_it_).first.c_str());
  kernel_widget_->grid_widget_->update_volume_data((*curr_kernel_it_).second);
  hist_widget_->update_histogram_data((*curr_kernel_it_).second);
  
}

void dbrec3d_kernel_viewer::prev_volume()
{
  if(curr_kernel_it_==kernels_.begin())
    curr_kernel_it_ = kernels_.end();
  
  curr_kernel_it_--;
   
  component_label_->setText((*curr_kernel_it_).first.c_str());
  kernel_widget_->grid_widget_->update_volume_data((*curr_kernel_it_).second);
  hist_widget_->update_histogram_data((*curr_kernel_it_).second);
}


/****************************** for vectors ********************************/

dbrec3d_vector_kernel_viewer::dbrec3d_vector_kernel_viewer(vcl_vector< vnl_vector<double> > &kernels, unsigned dimx, unsigned dimy, unsigned dimz,float tf_min, float tf_max)
{
  //initialize the widget to display the first principal component
  kernels_= kernels;
  curr_kernel_it_ = kernels_.begin();
  vcl_cout << *curr_kernel_it_ << vcl_endl;
  curr_kernel_idx_ = 0;
  kernel_widget_ = new dbrec3d_filter_grid_widget((*curr_kernel_it_), dimx,  dimy,  dimz, tf_min, tf_max);
  hist_widget_ = new dbrec3d_histogram_widget((*curr_kernel_it_), tf_min, tf_max, 10);
  component_label_ = new QLabel;
  component_label_->setNum((int)curr_kernel_idx_); 
  
  QDockWidget *hist_dock = new QDockWidget(tr("Histogram Kernel Values"), this);
  hist_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  hist_dock->setWidget(hist_widget_);
  hist_dock->setFloating(false);
  addDockWidget(Qt::RightDockWidgetArea, hist_dock);
  
  //setLayout(layout);
  setCentralWidget(kernel_widget_);
  statusBar()->addWidget(new QLabel("Kernel No: "));
  statusBar()->addWidget(component_label_);
  setGeometry(300,200,1200,1000);
  setWindowTitle("Kernel Visualizer");
  
  
  //connect signals and slots
  connect(kernel_widget_->grid_widget_, SIGNAL(request_next_volume()), this, SLOT(next_volume()));
  connect(kernel_widget_->grid_widget_, SIGNAL(request_prev_volume()), this, SLOT(prev_volume()));
  
}

void dbrec3d_vector_kernel_viewer::next_volume()
{
  curr_kernel_it_++;
  curr_kernel_idx_++;
  if(curr_kernel_it_== kernels_.end()){
    curr_kernel_it_ = kernels_.begin();
    curr_kernel_idx_=0;
  }
  
  
  component_label_->setNum((int)curr_kernel_idx_); 
  kernel_widget_->grid_widget_->update_volume_data((*curr_kernel_it_));
  hist_widget_->update_histogram_data((*curr_kernel_it_));
  
}

void dbrec3d_vector_kernel_viewer::prev_volume()
{
  if(curr_kernel_it_==kernels_.begin()){
    curr_kernel_it_ = kernels_.end();
    curr_kernel_idx_ = kernels_.size();
  }
  
  curr_kernel_it_--;
  curr_kernel_idx_--;
  
  component_label_->setNum((int)curr_kernel_idx_); 
  kernel_widget_->grid_widget_->update_volume_data((*curr_kernel_it_));
  hist_widget_->update_histogram_data((*curr_kernel_it_));
}


/****************************** for steerable basis ********************************/

dbrec3d_steerable_basis_viewer::dbrec3d_steerable_basis_viewer(vcl_vector<vcl_vector<vcl_string> > basis,
                                                               vcl_vector<vcl_string> basis_names,
                                                               vcl_map<vcl_string, vnl_vector_fixed<float,5> > separable_taps, 
                                                               float tf_min, float tf_max)
{
  //initialize the widget to display the first principal component
  basis_= basis;
  basis_names_ = basis_names;
  curr_bases_it_ = basis_.begin();
  curr_name_it_ = basis_names_.begin();
  separable_taps_=separable_taps;
  vcl_vector<vcl_string> filter_names = *curr_bases_it_;
  if (filter_names.size() !=3) {
    vcl_cerr <<"Error creating dbrec3d_steerable_basis_viewer\n"; 
    throw;
  }
  kernel_widget_ = new dbrec3d_filter_grid_widget(separable_taps[filter_names[0]], 
                                                  separable_taps[filter_names[1]],
                                                  separable_taps[filter_names[2]], tf_min, tf_max);
  
  //hist_widget_ = new dbrec3d_histogram_widget((*curr_kernel_it_).second, tf_min, tf_max, 5);
  component_label_ = new QLabel;
  component_label_->setText(curr_name_it_->c_str());
  
//  QDockWidget *hist_dock = new QDockWidget(tr("Histogram Kernel Values"), this);
//  hist_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
//  hist_dock->setWidget(hist_widget_);
//  hist_dock->setFloating(false);
//  addDockWidget(Qt::RightDockWidgetArea, hist_dock);
  
  //setLayout(layout);
  setCentralWidget(kernel_widget_);
  statusBar()->addWidget(new QLabel("Kernel: "));
  statusBar()->addWidget(component_label_);
  setGeometry(300,200,1200,1000);
  setWindowTitle("Kernel Visualizer");
  
  
  //connect signals and slots
  connect(kernel_widget_->grid_widget_, SIGNAL(request_next_volume()), this, SLOT(next_volume()));
  connect(kernel_widget_->grid_widget_, SIGNAL(request_prev_volume()), this, SLOT(prev_volume()));
  
}

void dbrec3d_steerable_basis_viewer::next_volume()
{
  curr_bases_it_++;
  curr_name_it_++;
  if(curr_bases_it_== basis_.end()){
    curr_bases_it_ = basis_.begin();
    curr_name_it_ = basis_names_.begin();
  }
  
  
  component_label_->setText(curr_name_it_->c_str());
  vcl_vector<vcl_string> filter_names = *curr_bases_it_;
  kernel_widget_->grid_widget_->update_volume_data(separable_taps_[filter_names[0]],
                                                   separable_taps_[filter_names[1]],
                                                   separable_taps_[filter_names[2]]);
  
}

void dbrec3d_steerable_basis_viewer::prev_volume()
{
  if(curr_bases_it_==basis_.begin()){
    curr_bases_it_ = basis_.end();
    curr_name_it_ = basis_names_.end();
  }
  
  curr_bases_it_--;
  curr_name_it_--;
  
  component_label_->setText(curr_name_it_->c_str());
  vcl_vector<vcl_string> filter_names = *curr_bases_it_;
  kernel_widget_->grid_widget_->update_volume_data(separable_taps_[filter_names[0]],
                                                   separable_taps_[filter_names[1]],
                                                   separable_taps_[filter_names[2]]);
}
