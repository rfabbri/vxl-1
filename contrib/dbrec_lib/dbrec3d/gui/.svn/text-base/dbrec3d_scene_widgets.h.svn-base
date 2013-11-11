// This is dbrec3d_scene_widgets.h
#ifndef dbrec3d_scene_widgets_h
#define dbrec3d_scene_widgets_h

//:
// \file
// \brief Widgets that display the information in a boxm_scene in different ways. 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  26-Oct-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <QList>
#include <QGLViewer/qglviewer.h>

#include "dbrec3d_scene_model.h"
#include <vgl/vgl_box_3d.h>

//: A simple scene widget with various visualization alternatives
class dbrec3d_gradient_scene_widget : public QGLViewer
{
  Q_OBJECT
  
public:
  
  dbrec3d_gradient_scene_widget(dbrec3d_scene_model *scene_data, QWidget* parent, const QGLWidget* shareWidget);
  ~dbrec3d_gradient_scene_widget();
  
  public slots:
  
  //: Filters values above given treshold
  void set_high_thresh(int b) 
  { 
    if (b >= 0) max_bin_ = b; 
    update();
  }
  
  //: Filters values below given treshold
  void set_low_thresh(int b)
  { 
    min_bin_ = b;
    update();
  }
  
  //: Enable/Disable direction filtering
  void set_filter_by_dir(bool disable)
  {
    filter_by_dir_ = !disable;
    update();
  }
  
  void set_polar(int polar)
  {
    polar_ = polar;
    update_dir_to_filter();
    update();
  }
  
  void set_azimuth( int azimuth)
  {
    azimuth_ = azimuth;
    update_dir_to_filter();
    update();
  }
  
protected:
  
  void draw();
  void init();
  virtual QSize sizeHint() const;
  
  
  //: A function that iterates through leaf cells and creates arrows from edge normals
  void init_arrows();
  
  void update_dir_to_filter();
  
  virtual QString helpString() const;
  
  void draw_spiral();
  void draw_arrow();
  
  QList<QList<dbrec3d_arrow> > arrows_;
  
  //: Bounding box of the world
  vgl_box_3d<double> bbox_; 
  //: Determines the type of the scene, and how it is rendered
  //dbrec3d_scene_type scene_type_;
  
  //: Thresholds for displayed data
  unsigned min_bin_;
  unsigned max_bin_;
  
  //: Length of the arrow
  float cell_length_;
  
  //: Variables to filter by direction
  bool filter_by_dir_;
  vnl_vector_fixed<float,3> dir_to_filter_;
  int polar_;
  int azimuth_;
  
  
  //: The array to store the points and colors to be passed to opengl
  //vcl_vector<float> vertices_;
  //vcl_vector<float> colors_;
  
  
  dbrec3d_scene_model *scene_data_;
  
};


class dbrec3d_filter_dir_widget: public QGLViewer
{
  
public:
  dbrec3d_filter_dir_widget(dbrec3d_scene_model *scene_data,QWidget* parent, const QGLWidget* shareWidget) //QGLViewer(parent, shareWidget)
  {
    //init scene geometry
    bbox_ = scene_data->scene_base_->get_world_bbox();
    makeCurrent();
       
  }
  
protected:
  
  void draw();
  void init();
  vgl_box_3d<double> bbox_;
};



//: A scene widget that keeps a list of 3d locations to be visualized as spheres
class dbrec3d_points_scene_widget : public QGLViewer
{
  Q_OBJECT
  
public:
  //: Constructor from a file location where points where saved a vgl_point_3d, using << stream operator.
  //  The base scene is necessary to finf scene dimensions
  dbrec3d_points_scene_widget(vcl_string file, boxm_scene_base_sptr scene_base);
  ~dbrec3d_points_scene_widget(){}
  
  
protected:
  
  void draw();
  void init();
  virtual QSize sizeHint() const { return QSize(640,600); }
  
  
  //: A function that reads point locations from file
  void init_points(vcl_string file);
   
  virtual QString helpString() const;
  
  QList<vgl_point_3d<double> > points_;
  
  //: Bounding box of the world
  vgl_box_3d<double> bbox_; 
  
};

//: A scene widget that keeps a list of 3d locations to be visualized as spheres
class dbrec3d_id_scene_widget : public QGLViewer
{
  Q_OBJECT
  
public:
  
  dbrec3d_id_scene_widget(dbrec3d_scene_model *scene_data, QWidget* parent, const QGLWidget* shareWidget);
  ~dbrec3d_id_scene_widget();
  
  public slots:
  
  //: Filters values above given treshold
  void set_high_thresh(int b) 
  { 
    if (b >= 0) max_bin_ = b; 
    update();
  }
  
  //: Filters values below given treshold
  void set_low_thresh(int b)
  { 
    min_bin_ = b;
    update();
  }
  
  
protected:
  
  void draw();
  void init();
  virtual QSize sizeHint() const;
  
  
  //: A function that iterates through leaf cells and creates arrows from edge normals
  void init_data();
  
  
  virtual QString helpString() const;
  
  
  QList<QList<vgl_point_3d<float> > > points_;
  QList<vnl_vector_fixed<char,3> > colors_;
  
  //: Bounding box of the world
  vgl_box_3d<double> bbox_; 

  //: Thresholds for displayed data
  unsigned min_bin_;
  unsigned max_bin_;
  
  dbrec3d_scene_model *scene_data_;
  
};
#endif
