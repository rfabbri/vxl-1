//:
// \file
// \author Isabel Restrepo
// \date 28-Oct-2010

#include "dbrec3d_histogram_widgets.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QCheckBox>
#include <QDial>
#include <QPainter>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_interval_data.h>

/********************* histogram image widget *************************/

void dbrec3d_histogram_image_widget::paintEvent(QPaintEvent*)
{
  QPainter p(this);
  
  p.setRenderHint(QPainter::Antialiasing);
  p.setRenderHint(QPainter::SmoothPixmapTransform);
  p.setCompositionMode(QPainter::CompositionMode_SourceOver);
  
  QImage bgImage;
  // bgImage = m_histogramImage1D.scaled(m_bounds.width(),
  //                                        m_bounds.height());
  
  p.drawImage(rect().x(), rect().y(), hist_image_);
}

const QSize dbrec3d_histogram_image_widget::sizeHint()
{
  return QSize(this->hist_image_.width(),this->hist_image_.height());
}

/********************* histogram  widget *************************/
dbrec3d_histogram_widget::dbrec3d_histogram_widget(dbrec3d_scene_model *scene_data)
{
  hist_ = scene_data->hist_;
  init_hist_plot();
  set_layout();
  linear_scale_ = true;
}

//: Initialize from a vnl_vector
dbrec3d_histogram_widget::dbrec3d_histogram_widget(const vnl_vector<double> &scene_data, float min, float max, unsigned int nbins)
{  
  hist_ = bsta_histogram<float>(min, max, nbins);
  
  for(unsigned i = 0; i < scene_data.size(); ++i)
    hist_.upcount(scene_data[i], 1.0f);
  
  init_hist_plot();
  set_layout();
  linear_scale_ = true;
  
  
}

//: Initialize from a bvpl_kernel
dbrec3d_histogram_widget::dbrec3d_histogram_widget(bvpl_kernel_sptr kernel, float min, float max, unsigned int nbins)
{  
  hist_ = bsta_histogram<float>(min, max, nbins);
  
  
  bvpl_kernel_iterator kernel_iter = kernel->iterator();
  
  kernel_iter.begin(); // reset the kernel iterator
  while (!kernel_iter.isDone())
  {
    vgl_point_3d<int> kernel_idx = kernel_iter.index();
    hist_.upcount((*kernel_iter).c_, 1.0f);
    ++kernel_iter;
  }
  
  init_hist_plot();
  set_layout();
  linear_scale_ = true;
  
  
}

void dbrec3d_histogram_widget::set_layout()
{
  
  // linear/log check boxes
  QCheckBox *linear_button = new QCheckBox("Linear");
  linear_button->setCheckState(Qt::Checked);
  QCheckBox *log_button = new QCheckBox("Log");
  log_button->setCheckState(Qt::Unchecked);
  
  QButtonGroup *button_grp = new QButtonGroup(this);
  button_grp->addButton(linear_button);
  button_grp->addButton(log_button);
  button_grp->setExclusive(true);
  
  QHBoxLayout *buttons_layout = new QHBoxLayout;
  buttons_layout->addItem(new QSpacerItem(2,20,QSizePolicy::Expanding,QSizePolicy::Fixed));
  buttons_layout->addWidget(linear_button);
  buttons_layout->setSpacing(10);
  buttons_layout->addWidget(log_button);
  
  //put all widgets together
  QVBoxLayout *layout = new QVBoxLayout;
  
  layout->setSpacing(0);
  layout->addWidget(hist_plot_);
  layout->setSpacing(5);
  layout->addItem(buttons_layout);
  
  setLayout(layout);
  
  connect(linear_button, SIGNAL(stateChanged(int)), this, SLOT(update_histogram_state(int)));

}


void dbrec3d_histogram_widget::init_hist_plot()
{
  hist_plot_ = new QwtPlot;
  hist_plot_->setCanvasBackground(QColor(Qt::white));
  hist_plot_->setTitle("Histogram");
  
  QwtPlotGrid *grid = new QwtPlotGrid;
  grid->enableXMin(true);
  grid->enableYMin(true);
  grid->setMajPen(QPen(Qt::black, 0, Qt::DotLine));
  grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
  grid->attach(hist_plot_);
  
  qwt_histogram_= new HistogramItem;
  qwt_histogram_->setColor(Qt::red);
  
  init_hist_vals();
  
  qwt_histogram_->setData(QwtIntervalData(hist_intervals_, hist_values_));
  qwt_histogram_->attach(hist_plot_);
  
  hist_plot_->setAxisScale(QwtPlot::yLeft, min_y_, max_y_);
  hist_plot_->setAxisScale(QwtPlot::xBottom, hist_.min(), hist_.max());
  hist_plot_->replot();
}                                                

void dbrec3d_histogram_widget::init_hist_vals()
{
 
  if(hist_.counts(0) < 1.0 - 1e-9)
    min_log_y_, max_log_y_ =vcl_log(1);
  else 
    min_log_y_, max_log_y_ = vcl_log(hist_.counts(0)); 
  
  min_y_, max_y_ = hist_.counts(0);
  for ( unsigned i = 0; i < hist_.nbins(); i++ )
  {
    float  min_pos, max_pos;
    hist_.value_range(i, min_pos, max_pos);
    hist_intervals_.push_back(QwtDoubleInterval(min_pos, max_pos));
    
    float this_val = hist_.counts(i);
    float this_log_val;
    
    if(this_val <  1.0 - 1e-9)
      this_log_val = vcl_log(1.0);
    else 
      this_log_val = vcl_log(this_val);
    
    hist_values_.push_back(this_val);
    hist_log_values_.push_back(this_log_val);
    
    
    if(this_val < min_y_) min_y_ = this_val;
    if(this_val > max_y_) max_y_ = this_val;
    
    if(this_log_val < min_log_y_) min_log_y_ = this_log_val;
    if(this_log_val > max_log_y_) max_log_y_ = this_log_val;
    
  }
}

void dbrec3d_histogram_widget::update_histogram_state(int linear_state)
{
  linear_scale_ = linear_state;
  if(linear_state){
    qwt_histogram_->setData(QwtIntervalData(hist_intervals_, hist_values_));
    qwt_histogram_->attach(hist_plot_);
    
    hist_plot_->setAxisScale(QwtPlot::yLeft, min_y_, max_y_);
    hist_plot_->setAxisScale(QwtPlot::xBottom, hist_.min(), hist_.max());
    hist_plot_->replot();
  }
  else {
    qwt_histogram_->setData(QwtIntervalData(hist_intervals_, hist_log_values_));
    qwt_histogram_->attach(hist_plot_);
    
    hist_plot_->setAxisScale(QwtPlot::yLeft, min_log_y_, max_log_y_);
    hist_plot_->setAxisScale(QwtPlot::xBottom, hist_.min(), hist_.max());
    hist_plot_->replot();
    
  }      
}

void dbrec3d_histogram_widget::update_histogram_data(bvpl_kernel_sptr kernel)
{
  hist_.clear();
  
  bvpl_kernel_iterator kernel_iter = kernel->iterator();
  
  kernel_iter.begin(); // reset the kernel iterator
  while (!kernel_iter.isDone())
  {
    vgl_point_3d<int> kernel_idx = kernel_iter.index();
    hist_.upcount((*kernel_iter).c_, 1.0f);
    ++kernel_iter;
  }
  
  hist_values_.clear();
  hist_log_values_.clear();
  
  init_hist_vals();
  
  update_histogram_state(linear_scale_);  
}



void dbrec3d_histogram_widget::update_histogram_data(const vnl_vector<double> &scene_data)
{
  hist_.clear();
  
  for(unsigned i = 0; i < scene_data.size(); ++i)
    hist_.upcount(scene_data[i], 1.0f);
  
  hist_values_.clear();
  hist_log_values_.clear();
  
  init_hist_vals();
  
  update_histogram_state(linear_scale_);  
}

QSize dbrec3d_histogram_widget::sizeHint()const
{
  return QSize(640,600);
}

/******************* Scene Info Widget *********************/

dbrec3d_scene_info_widget::dbrec3d_scene_info_widget(dbrec3d_scene_model *scene_data)
{
  hist_ = scene_data->hist_;
  
  //histogram_widget_ = new dbrec3d_histogram_widget(scene_data->histogram_image());
  //histogram_widget_->setGeometry(0,0,700,400);
  
  init_hist_plot();
  
  // linear/log check boxes
  QCheckBox *linear_button = new QCheckBox("Linear");
  linear_button->setCheckState(Qt::Checked);
  QCheckBox *log_button = new QCheckBox("Log");
  log_button->setCheckState(Qt::Unchecked);
  
  QButtonGroup *button_grp = new QButtonGroup(this);
  button_grp->addButton(linear_button);
  button_grp->addButton(log_button);
  button_grp->setExclusive(true);
  
  QHBoxLayout *buttons_layout = new QHBoxLayout;
  buttons_layout->addItem(new QSpacerItem(2,20,QSizePolicy::Expanding,QSizePolicy::Fixed));
  buttons_layout->addWidget(linear_button);
  buttons_layout->setSpacing(10);
  buttons_layout->addWidget(log_button);
   
  
  int init_min_val = hist_.nbins() - hist_.nbins()/10*9;
  int init_max_val = hist_.nbins() - hist_.nbins()/10*8;
  
  //min-threshold slider
  hist_min_slider_ = new QSlider(Qt::Horizontal);
  hist_min_value_ = new QLabel;
  init_slider(hist_min_slider_, hist_min_value_, init_min_val, hist_.nbins());
  
  QWidget *min_slider_widget = assemble_slider_widget(hist_min_slider_, hist_min_value_, ": Min Magnitude");
  
  //max-threshold slider
  hist_max_slider_ = new QSlider(Qt::Horizontal);
  hist_max_value_ = new QLabel;
  init_slider(hist_max_slider_, hist_max_value_, init_max_val, hist_.nbins());
  
  QWidget *max_slider_widget = assemble_slider_widget(hist_max_slider_, hist_max_value_, ": Max Magnitude");
  
  QCheckBox *enable_dir_filter = new QCheckBox("Disable Direction Filtering");
  enable_dir_filter->setCheckState(Qt::Checked);
  disable_dir_filtering_ = true;
  QHBoxLayout *filter_layout = new QHBoxLayout;
  filter_layout->addItem(new QSpacerItem(2,20,QSizePolicy::Expanding,QSizePolicy::Fixed));
  filter_layout->addWidget(enable_dir_filter);

  
  //polar-threshold slider
  polar_slider_ = new QSlider(Qt::Horizontal);
  polar_value_ = new QLabel;
  init_slider(polar_slider_, polar_value_, 0, 180);
  QWidget *polar_slider_widget = assemble_slider_widget(polar_slider_, polar_value_, ": Polar-Degrees");
  
  //azimutal-threshold slider
  azimuth_slider_ = new QSlider(Qt::Horizontal);
  azimuth_value_ = new QLabel;
  init_slider(azimuth_slider_, azimuth_value_, 0, 360);
  QWidget *azimutal_slider_widget = assemble_slider_widget(azimuth_slider_, azimuth_value_, ": azimuth - Degrees");
  
  //put all widgets together
  QVBoxLayout *info_layout = new QVBoxLayout;
  
  info_layout->setSpacing(0);
  info_layout->addWidget(hist_plot_);
  info_layout->setSpacing(5);
  info_layout->addItem(buttons_layout);
  info_layout->setSpacing(0);
  info_layout->addWidget(min_slider_widget);
  info_layout->addWidget(max_slider_widget);
  info_layout->addWidget(polar_slider_widget);
  info_layout->addWidget(azimutal_slider_widget);
  info_layout->addItem(filter_layout);
  
  setLayout(info_layout);
  
  //connect signals and slots
  //treshold-related signals
  connect(hist_min_slider_, SIGNAL(valueChanged(int)), this, SLOT(update_min_threshold(int)));
  connect(hist_max_slider_, SIGNAL(valueChanged(int)), this, SLOT(update_max_threshold(int)));
  connect(polar_slider_, SIGNAL(valueChanged(int)), this, SLOT(update_polar(int)));
  connect(azimuth_slider_, SIGNAL(valueChanged(int)), this, SLOT(update_azimuth(int)));
  //log-linear signals
  connect(linear_button, SIGNAL(stateChanged(int)), this, SLOT(update_histogram_state(int)));
  connect(enable_dir_filter, SIGNAL(stateChanged(int)), this, SLOT(update_dir_filtering_state(int)));
}
dbrec3d_scene_info_widget::~dbrec3d_scene_info_widget()
{
  hist_values_.clear();
  hist_intervals_.clear();
  hist_log_values_.clear();
  delete hist_min_slider_;
  delete hist_min_value_;
  delete hist_max_slider_;
  delete hist_max_value_;
  delete qwt_histogram_;
}

void dbrec3d_scene_info_widget::init_slider(QSlider *slider, QLabel *label , int init_val, int max_val)
{
  
  slider->setMinimum(0);
  slider->setMaximum(max_val);
  slider->setTickInterval(1);
  slider->setSingleStep(1);
  
  slider->setValue(init_val);
  
  label->setNum(init_val);
}

QWidget* dbrec3d_scene_info_widget::assemble_slider_widget(QSlider *slider, QLabel *value, vcl_string name)
{
  QHBoxLayout *label_layout = new QHBoxLayout();
  label_layout->setSpacing(0);
  // Add a spacer to keep the widget on the left / right
  QSpacerItem *labelMinSpacer = new QSpacerItem(200,20,QSizePolicy::Expanding,QSizePolicy::Expanding);
  label_layout->addItem(labelMinSpacer);
  // Add the label to the H layout
  label_layout->addWidget(value);
  label_layout->addWidget(new QLabel(name.c_str()));
  
  
  QWidget *label_container = new QWidget(this);
  label_container->setLayout(label_layout);
  label_container->setGeometry(0,0,700,50);
  label_container->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
  
  QWidget *main_container = new QWidget(this);
  
  QVBoxLayout *main_layout = new QVBoxLayout();
  main_layout->setSpacing(0);
  
  main_layout->addWidget(label_container);
  main_layout->addWidget(slider);
  main_container->setGeometry(0,0,700,100);
  
  main_container->setLayout(main_layout);
  main_container->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
  
  return main_container;
  
}

void dbrec3d_scene_info_widget::init_hist_plot()
{
  hist_plot_ = new QwtPlot;
  hist_plot_->setCanvasBackground(QColor(Qt::white));
  hist_plot_->setTitle("Histogram");
  
  QwtPlotGrid *grid = new QwtPlotGrid;
  grid->enableXMin(true);
  grid->enableYMin(true);
  grid->setMajPen(QPen(Qt::black, 0, Qt::DotLine));
  grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
  grid->attach(hist_plot_);
  
  qwt_histogram_= new HistogramItem;
  qwt_histogram_->setColor(Qt::darkCyan);
  
  init_hist_vals();
  
  qwt_histogram_->setData(QwtIntervalData(hist_intervals_, hist_values_));
  qwt_histogram_->attach(hist_plot_);
  
  hist_plot_->setAxisScale(QwtPlot::yLeft, min_y_, max_y_);
  hist_plot_->setAxisScale(QwtPlot::xBottom, hist_.min(), hist_.max());
  //plot.replot();
}

void dbrec3d_scene_info_widget::init_hist_vals()
{
  //hist_intervals_.reserve(hist_.nbins());
  //  hist_values_.reserve(hist_.nbins());
  //  hist_log_values_.reserve(hist_.nbins());
  
  if(hist_.counts(0) < 1.0 - 1e-9)
    min_log_y_, max_log_y_ =vcl_log(1);
  else 
    min_log_y_, max_log_y_ = vcl_log(hist_.counts(0)); 
  
  min_y_, max_y_ = hist_.counts(0);
  for ( unsigned i = 0; i < hist_.nbins(); i++ )
  {
    float  min_pos, max_pos;
    hist_.value_range(i, min_pos, max_pos);
    hist_intervals_.push_back(QwtDoubleInterval(min_pos, max_pos));
    
    float this_val = hist_.counts(i);
    float this_log_val;
    
    if(this_val <  1.0 - 1e-9)
      this_log_val = vcl_log(1.0);
    else 
      this_log_val = vcl_log(this_val);
    
    hist_values_.push_back(this_val);
    hist_log_values_.push_back(this_log_val);
    
    
    if(this_val < min_y_) min_y_ = this_val;
    if(this_val > max_y_) max_y_ = this_val;
    
    if(this_log_val < min_log_y_) min_log_y_ = this_log_val;
    if(this_log_val > max_log_y_) max_log_y_ = this_log_val;
    
  }
}

void dbrec3d_scene_info_widget::update_histogram_state(int linear_state)
{
  if(linear_state){
    qwt_histogram_->setData(QwtIntervalData(hist_intervals_, hist_values_));
    qwt_histogram_->attach(hist_plot_);
    
    hist_plot_->setAxisScale(QwtPlot::yLeft, min_y_, max_y_);
    hist_plot_->setAxisScale(QwtPlot::xBottom, hist_.min(), hist_.max());
    hist_plot_->replot();
  }
  else {
    qwt_histogram_->setData(QwtIntervalData(hist_intervals_, hist_log_values_));
    qwt_histogram_->attach(hist_plot_);
    
    hist_plot_->setAxisScale(QwtPlot::yLeft, min_log_y_, max_log_y_);
    hist_plot_->setAxisScale(QwtPlot::xBottom, hist_.min(), hist_.max());
    hist_plot_->replot();
    
  }      
}

QSize dbrec3d_scene_info_widget::sizeHint()const
{
  return QSize(640,600);
}

/******************* ID Scene Info Widget *********************/

dbrec3d_id_scene_info_widget::dbrec3d_id_scene_info_widget(dbrec3d_scene_model *scene_data)
{
  hist_ = scene_data->hist_;
  

  init_hist_plot();
  
  // linear/log check boxes
  QCheckBox *linear_button = new QCheckBox("Linear");
  linear_button->setCheckState(Qt::Checked);
  QCheckBox *log_button = new QCheckBox("Log");
  log_button->setCheckState(Qt::Unchecked);
  
  QButtonGroup *button_grp = new QButtonGroup(this);
  button_grp->addButton(linear_button);
  button_grp->addButton(log_button);
  button_grp->setExclusive(true);
  
  QHBoxLayout *buttons_layout = new QHBoxLayout;
  buttons_layout->addItem(new QSpacerItem(2,20,QSizePolicy::Expanding,QSizePolicy::Fixed));
  buttons_layout->addWidget(linear_button);
  buttons_layout->setSpacing(10);
  buttons_layout->addWidget(log_button);
  
  
  int init_min_val = hist_.nbins() - hist_.nbins()/10*9;
  int init_max_val = hist_.nbins() - hist_.nbins()/10*8;
  
  //min-threshold slider
  hist_min_slider_ = new QSlider(Qt::Horizontal);
  hist_min_value_ = new QLabel;
  init_slider(hist_min_slider_, hist_min_value_, init_min_val, hist_.nbins());
  
  QWidget *min_slider_widget = assemble_slider_widget(hist_min_slider_, hist_min_value_, ": Min Magnitude");
  
  //max-threshold slider
  hist_max_slider_ = new QSlider(Qt::Horizontal);
  hist_max_value_ = new QLabel;
  init_slider(hist_max_slider_, hist_max_value_, init_max_val, hist_.nbins());
  
  QWidget *max_slider_widget = assemble_slider_widget(hist_max_slider_, hist_max_value_, ": Max Magnitude");
 
  
  //put all widgets together
  QVBoxLayout *info_layout = new QVBoxLayout;
  
  info_layout->setSpacing(0);
  info_layout->addWidget(hist_plot_);
  info_layout->setSpacing(5);
  info_layout->addItem(buttons_layout);
  info_layout->setSpacing(0);
  info_layout->addWidget(min_slider_widget);
  info_layout->addWidget(max_slider_widget);

  
  setLayout(info_layout);
  
  //connect signals and slots
  //treshold-related signals
  connect(hist_min_slider_, SIGNAL(valueChanged(int)), this, SLOT(update_min_threshold(int)));
  connect(hist_max_slider_, SIGNAL(valueChanged(int)), this, SLOT(update_max_threshold(int)));
  //log-linear signals
  connect(linear_button, SIGNAL(stateChanged(int)), this, SLOT(update_histogram_state(int)));
}
dbrec3d_id_scene_info_widget::~dbrec3d_id_scene_info_widget()
{
  hist_values_.clear();
  hist_intervals_.clear();
  hist_log_values_.clear();
  delete hist_min_slider_;
  delete hist_min_value_;
  delete hist_max_slider_;
  delete hist_max_value_;
  delete qwt_histogram_;
}

void dbrec3d_id_scene_info_widget::init_slider(QSlider *slider, QLabel *label ,  int init_val,  int max_val)
{
  
  slider->setMinimum(0);
  slider->setMaximum(max_val);
  slider->setTickInterval(1);
  slider->setSingleStep(1);
  
  slider->setValue(init_val);
  
  label->setNum(init_val);
}

QWidget* dbrec3d_id_scene_info_widget::assemble_slider_widget(QSlider *slider, QLabel *value, vcl_string name)
{
  QHBoxLayout *label_layout = new QHBoxLayout();
  label_layout->setSpacing(0);
  // Add a spacer to keep the widget on the left / right
  QSpacerItem *labelMinSpacer = new QSpacerItem(200,20,QSizePolicy::Expanding,QSizePolicy::Expanding);
  label_layout->addItem(labelMinSpacer);
  // Add the label to the H layout
  label_layout->addWidget(value);
  label_layout->addWidget(new QLabel(name.c_str()));
  
  
  QWidget *label_container = new QWidget(this);
  label_container->setLayout(label_layout);
  label_container->setGeometry(0,0,700,50);
  label_container->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
  
  QWidget *main_container = new QWidget(this);
  
  QVBoxLayout *main_layout = new QVBoxLayout();
  main_layout->setSpacing(0);
  
  main_layout->addWidget(label_container);
  main_layout->addWidget(slider);
  main_container->setGeometry(0,0,700,100);
  
  main_container->setLayout(main_layout);
  main_container->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
  
  return main_container;
  
}

void dbrec3d_id_scene_info_widget::init_hist_plot()
{
  hist_plot_ = new QwtPlot;
  hist_plot_->setCanvasBackground(QColor(Qt::white));
  hist_plot_->setTitle("Histogram");
  
  QwtPlotGrid *grid = new QwtPlotGrid;
  grid->enableXMin(true);
  grid->enableYMin(true);
  grid->setMajPen(QPen(Qt::black, 0, Qt::DotLine));
  grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
  grid->attach(hist_plot_);
  
  qwt_histogram_= new HistogramItem;
  qwt_histogram_->setColor(Qt::darkCyan);
  
  init_hist_vals();
  
  qwt_histogram_->setData(QwtIntervalData(hist_intervals_, hist_values_));
  qwt_histogram_->attach(hist_plot_);
  
  hist_plot_->setAxisScale(QwtPlot::yLeft, min_y_, max_y_);
  hist_plot_->setAxisScale(QwtPlot::xBottom, hist_.min(), hist_.max());
  //plot.replot();
}

void dbrec3d_id_scene_info_widget::init_hist_vals()
{
  //hist_intervals_.reserve(hist_.nbins());
  //  hist_values_.reserve(hist_.nbins());
  //  hist_log_values_.reserve(hist_.nbins());
  
  if(hist_.counts(0) < 1.0 - 1e-9)
    min_log_y_, max_log_y_ =vcl_log(1);
  else 
    min_log_y_, max_log_y_ = vcl_log(hist_.counts(0)); 
  
  min_y_, max_y_ = hist_.counts(0);
  for ( unsigned i = 0; i < hist_.nbins(); i++ )
  {
    float  min_pos, max_pos;
    hist_.value_range(i, min_pos, max_pos);
    hist_intervals_.push_back(QwtDoubleInterval(min_pos, max_pos));
    
    float this_val = hist_.counts(i);
    float this_log_val;
    
    if(this_val <  1.0 - 1e-9)
      this_log_val = vcl_log(1.0);
    else 
      this_log_val = vcl_log(this_val);
    
    hist_values_.push_back(this_val);
    hist_log_values_.push_back(this_log_val);
    
    
    if(this_val < min_y_) min_y_ = this_val;
    if(this_val > max_y_) max_y_ = this_val;
    
    if(this_log_val < min_log_y_) min_log_y_ = this_log_val;
    if(this_log_val > max_log_y_) max_log_y_ = this_log_val;
    
  }
}

void dbrec3d_id_scene_info_widget::update_histogram_state(int linear_state)
{
  if(linear_state){
    qwt_histogram_->setData(QwtIntervalData(hist_intervals_, hist_values_));
    qwt_histogram_->attach(hist_plot_);
    
    hist_plot_->setAxisScale(QwtPlot::yLeft, min_y_, max_y_);
    hist_plot_->setAxisScale(QwtPlot::xBottom, hist_.min(), hist_.max());
    hist_plot_->replot();
  }
  else {
    qwt_histogram_->setData(QwtIntervalData(hist_intervals_, hist_log_values_));
    qwt_histogram_->attach(hist_plot_);
    
    hist_plot_->setAxisScale(QwtPlot::yLeft, min_log_y_, max_log_y_);
    hist_plot_->setAxisScale(QwtPlot::xBottom, hist_.min(), hist_.max());
    hist_plot_->replot();
    
  }      
}

QSize dbrec3d_id_scene_info_widget::sizeHint()const
{
  return QSize(640,600);
}