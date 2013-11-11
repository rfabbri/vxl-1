// This is dbrec3d_histogram_widgets.h
#ifndef dbrec3d_histogram_widgets_h
#define dbrec3d_histogram_widgets_h

//:
// \file
// \brief A collection of classes tha presents histogram-info of a boxm_scene
// \author Isabel Restrepo mir@lems.brown.edu
// \date  28-Oct-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <QImage>
#include <QSlider>
#include <QLabel>
#include <qwt_plot.h>
#include <qwt_double_interval.h>
#include "qwt_histogram.h"

#include <bvpl/kernels/bvpl_kernel.h>

#include "dbrec3d_scene_model.h"

//: A widget to display a custom histogram image of data of boxm_scene
class dbrec3d_histogram_image_widget : public QWidget
{
  Q_OBJECT
public:
  dbrec3d_histogram_image_widget(QImage hist_image): hist_image_(hist_image){} 
  virtual void paintEvent(QPaintEvent*);
  
private:
  QImage hist_image_;
  
protected:
  const QSize sizeHint();
};

//: A class to hold a qwt histogram
class dbrec3d_histogram_widget : public QWidget
{
  Q_OBJECT
  
public:
  dbrec3d_histogram_widget(dbrec3d_scene_model *scene_data);
  dbrec3d_histogram_widget(const vnl_vector<double> &scene_data,float min, float max,  unsigned int nbins );
  dbrec3d_histogram_widget(bvpl_kernel_sptr kernel, float min, float max, unsigned int nbins);
  ~dbrec3d_histogram_widget(){}
  
protected:
  
  //: Initializes qwt histogram plot: look and feel
  void init_hist_plot();
  
  //: Initializes histogram data - which is kept in memory
  void init_hist_vals();
  
  //: Size properties
  virtual QSize sizeHint() const;
  
  void set_layout();
  
public slots:
  
  //: Updated the histogram to be linear scale or log scale, depending on state
  void update_histogram_state(int linear_state);
  
  void update_histogram_data(const vnl_vector<double> &scene_data);

  void update_histogram_data(bvpl_kernel_sptr kernel);
  
private:
  
  //: The histogram data
  bsta_histogram<float> hist_;
  
  //: Basic plot container
  QwtPlot *hist_plot_;
  //: Histogram plot  
  HistogramItem *qwt_histogram_;
  
  //:Histogram related data
  QwtArray<QwtDoubleInterval> hist_intervals_;
  QwtArray<double> hist_values_;
  QwtArray<double> hist_log_values_;
  float min_y_, max_y_, min_log_y_, max_log_y_;   
  bool linear_scale_;
};

//: A widget to display a qwt-histogram and other info boxm_scene
class dbrec3d_scene_info_widget : public QWidget
{
  Q_OBJECT
  
public:
  dbrec3d_scene_info_widget(dbrec3d_scene_model *scene_data);
  ~dbrec3d_scene_info_widget();
  
  int current_low_threshold() {return hist_min_slider_->value(); }
  int current_high_threshold() {return hist_max_slider_->value(); }
  int current_polar() {return polar_slider_->value(); }
  int current_azimuth() {return azimuth_slider_->value();}
  int is_dir_filter_disable() { return disable_dir_filtering_; }
  
protected:
  //: Initilize sliders look and position
  void init_slider(QSlider *slider, QLabel *label , int init_val, int max_val);
  
  //: Insert sliders and labels into layouts
  QWidget* assemble_slider_widget(QSlider *slider, QLabel *value, vcl_string name);
  
  //: Initializes qwt histogram plot: look and feel
  void init_hist_plot();
  
  //: Initializes histogram data - which is kept in memory
  void init_hist_vals();
  
  //: Size properties
  virtual QSize sizeHint() const;
  
signals:
  
  //: Signals used the min and max values of sliders and labels have changed
  void min_threshold_changed(int);
  void max_threshold_changed(int);
  void polar_changed(int);
  void azimuth_changed(int);
  //: Signals true if filtering is disabled
  void dir_filter_state_changed(bool);
  
  protected slots:
  
  //: Slots used the min and max values of sliders and labels have changed
  void update_min_threshold(int t)
  {
    //update the value label
    hist_min_value_->setNum(t);
    
    //update the max_slider    
    if(t > hist_max_slider_->value())
      hist_max_slider_->setValue(t);
    
    emit min_threshold_changed(t);
  }
  
  void update_max_threshold(int t)
  {
    //update the value label
    hist_max_value_->setNum(t);
    
    //update the max_slider    
    if(t < hist_min_slider_->value())
      hist_min_slider_->setValue(t);
    
    emit max_threshold_changed(t);
  }
  
  //: Slots used the min and max values of sliders and labels have changed
  void update_polar(int t)
  {
    //update the value label
    polar_value_->setNum(t);
    
    if(!disable_dir_filtering_)
      emit polar_changed(t);
  }
  
  void update_azimuth(int t)
  {
    //update the value label
    azimuth_value_->setNum(t);
   
    if(!disable_dir_filtering_)
      emit azimuth_changed(t);
  }
  
  //: Updated the histogram to be linear scale or log scale, depending on state
  void update_histogram_state(int linear_state);
  
  void update_dir_filtering_state(int disable)
  {
    disable_dir_filtering_ = disable;
    emit dir_filter_state_changed(disable);
  }
  
  
private:
  //: The histogram data
  bsta_histogram<float> hist_;
  //: Sliders and labels
  QSlider *hist_min_slider_;
  QLabel *hist_min_value_;
  QSlider *hist_max_slider_;
  QLabel *hist_max_value_;
  QSlider *polar_slider_;
  QLabel *polar_value_;
  QSlider *azimuth_slider_;
  QLabel *azimuth_value_;
  bool disable_dir_filtering_;
  
  //: Basic plot container
  QwtPlot *hist_plot_;
  //: Histogram plot  
  HistogramItem *qwt_histogram_;

  //:Histogram related data
  QwtArray<QwtDoubleInterval> hist_intervals_;
  QwtArray<double> hist_values_;
  QwtArray<double> hist_log_values_;
  float min_y_, max_y_, min_log_y_, max_log_y_;
};



//: A widget to display a qwt-histogram and other info boxm_scene
class dbrec3d_id_scene_info_widget : public QWidget
{
  Q_OBJECT
  
public:
  dbrec3d_id_scene_info_widget(dbrec3d_scene_model *scene_data);
  ~dbrec3d_id_scene_info_widget();
  
  int current_low_threshold() {return hist_min_slider_->value(); }
  int current_high_threshold() {return hist_max_slider_->value(); }

  
protected:
  //: Initilize sliders look and position
  void init_slider(QSlider *slider, QLabel *label , int init_val, int max_val);
  
  //: Insert sliders and labels into layouts
  QWidget* assemble_slider_widget(QSlider *slider, QLabel *value, vcl_string name);
  
  //: Initializes qwt histogram plot: look and feel
  void init_hist_plot();
  
  //: Initializes histogram data - which is kept in memory
  void init_hist_vals();
  
  //: Size properties
  virtual QSize sizeHint() const;
  
signals:
  
  //: Signals used the min and max values of sliders and labels have changed
  void min_threshold_changed(int);
  void max_threshold_changed(int);
  
  protected slots:
  
  //: Slots used the min and max values of sliders and labels have changed
  void update_min_threshold(int t)
  {
    //update the value label
    hist_min_value_->setNum(t);
    
    //update the max_slider    
    if(t > hist_max_slider_->value())
      hist_max_slider_->setValue(t);
    
    emit min_threshold_changed(t);
  }
  
  void update_max_threshold(int t)
  {
    //update the value label
    hist_max_value_->setNum(t);
    
    //update the max_slider    
    if(t < hist_min_slider_->value())
      hist_min_slider_->setValue(t);
    
    emit max_threshold_changed(t);
  }
  
  //: Updated the histogram to be linear scale or log scale, depending on state
  void update_histogram_state(int linear_state);
  
  
private:
  //: The histogram data
  bsta_histogram<float> hist_;
  //: Sliders and labels
  QSlider *hist_min_slider_;
  QLabel *hist_min_value_;
  QSlider *hist_max_slider_;
  QLabel *hist_max_value_;
  
  //: Basic plot container
  QwtPlot *hist_plot_;
  //: Histogram plot  
  HistogramItem *qwt_histogram_;
  
  //:Histogram related data
  QwtArray<QwtDoubleInterval> hist_intervals_;
  QwtArray<double> hist_values_;
  QwtArray<double> hist_log_values_;
  float min_y_, max_y_, min_log_y_, max_log_y_;
};
#endif
