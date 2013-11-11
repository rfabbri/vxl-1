//:
// \file
// \author Isabel Restrepo
// \date 11-Nov-2010

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>

#include "dbrec3d_double_slider.h"


dbrec3d_double_slider::dbrec3d_double_slider(float min_val, float max_val, vcl_string label)
{
  //floating-point correspondant values
  min_val_ = min_val;
  max_val_ = max_val;
  range_val_ = max_val-min_val;
  //vcl_assert(range_val_ > 0.0f);
  
  //min-threshold slider
  min_slider_ = new QSlider(Qt::Horizontal);
  min_val_label_ = new QLabel;
  init_slider(min_slider_, min_val_label_, 30);
  
  QWidget *min_slider_widget = assemble_slider_widget(min_slider_, min_val_label_ , ": Min " + label);
  
  //max-threshold slider
  max_slider_ = new QSlider(Qt::Horizontal);
  max_val_label_ = new QLabel;
  init_slider(max_slider_, max_val_label_, 60);
  
  QWidget *max_slider_widget = assemble_slider_widget(max_slider_, max_val_label_ , ": Max " + label);
  
  //Put sliders together
  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(min_slider_widget);
  layout->addWidget(max_slider_widget);
  
  setLayout(layout);
  
  //connect signals and slots
  connect(min_slider_, SIGNAL(valueChanged(int)), this, SLOT(update_min_threshold(int)));
  connect(max_slider_, SIGNAL(valueChanged(int)), this, SLOT(update_max_threshold(int)));
}

void dbrec3d_double_slider::init_slider(QSlider *slider, QLabel *label, int init_val)
{
  slider->setMinimum(0);
  slider->setMaximum(100);
  slider->setTickInterval(1);
  slider->setSingleStep(1);
  slider->setValue(init_val);
  label->setNum((float)init_val*range_val_/100.0f);
}

QWidget* dbrec3d_double_slider::assemble_slider_widget(QSlider *slider, QLabel *value_label, vcl_string name)
{
  QHBoxLayout *label_layout = new QHBoxLayout();
  label_layout->setSpacing(0);
  // Add a spacer to keep the widget on the left / right
  QSpacerItem *labelMinSpacer = new QSpacerItem(200,20,QSizePolicy::Expanding,QSizePolicy::Expanding);
  label_layout->addItem(labelMinSpacer);
  // Add the label to the H layout
  label_layout->addWidget(value_label);
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