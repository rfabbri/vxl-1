// This is dbrec3d_double_slider.h
#ifndef dbrec3d_double_slider_h
#define dbrec3d_double_slider_h

//:
// \file
// \brief A generic QWidget with two sliders to control min/max ranges. 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  11-Nov-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <QWidget>
#include <QLabel>
#include <QSlider>

#include <vcl_string.h>

class dbrec3d_double_slider : public QWidget
{
  Q_OBJECT
  
public:
  
  dbrec3d_double_slider(float min_val, float max_val, vcl_string label);
  
signals:
  
  //: Signals used the min and max values of sliders and labels have changed
  void min_threshold_changed(float);
  void max_threshold_changed(float);
  
  protected slots:
  
  //: Slots used the min and max values of sliders and labels have changed
  void update_min_threshold(int t)
  {
    //update the value label
    float fval = min_val_ + (float)t*(range_val_)/100.0f;
    min_val_label_->setNum(fval);
    
    //update the max_slider    
    if(t > max_slider_->value())
      max_slider_->setValue(t);
    
    emit min_threshold_changed(fval);
  }
  
  void update_max_threshold(int t)
  {
    float fval = min_val_ + (float)t*(range_val_)/100.0f;

    //update the value label
    max_val_label_->setNum(fval);
    
    //update the max_slider    
    if(t < min_slider_->value())
      min_slider_->setValue(t);
    
    emit max_threshold_changed(fval);
  }
  
protected:
  QWidget* assemble_slider_widget(QSlider *slider, QLabel *value_label, vcl_string name);
  void init_slider(QSlider *slider, QLabel *label, int init_val);
  
  
protected:
  
  //: Sliders and labels
  QSlider *min_slider_;
  QLabel *min_val_label_;
  QSlider *max_slider_;
  QLabel *max_val_label_;
  
  //: The slider size is fixed from 0-100. However, in order to represent floating point values we keep the true range of slider
  float min_val_;
  float max_val_;
                  float range_val_;
};

#endif
