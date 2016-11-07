// This is dbrec3d_gui_dbrec3d_scene_h
#ifndef dbrec3d_scene_h
#define dbrec3d_scene_h

//:
// \file
// \brief A class that implements the dbrec3d_part concept for composite parts (i.e non-leafs). 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  14-Oct-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim


#include <QGLViewer/qglviewer.h>

#include <vnl/vnl_float_3.h>
#include <boxm/boxm_scene.h>

//: A class to draw a simple arrow
class dbrec3d_arrow
{
public:
  dbrec3d_arrow( vnl_float_3 pos, vnl_float_3 end_pos, float length): pos_(pos), end_pos_(end_pos), length_(length){}

  void draw();
  
  // Member variables
  vnl_float_3 pos_;
  vnl_float_3 end_pos_; 
  float length_;
};



//: A simple scene viewer with various visualization alternatives
class dbrec3d_scene_widget : public QGLViewer
{
  
public:
  
  dbrec3d_scene_widget(): scene_(NULL) {}
  dbrec3d_scene_widget(boxm_scene_base_sptr scene): scene_(scene){}
  
  
protected:
  
  virtual void draw();
  virtual void init();
  
  //: A function that iterates through leaf cells and creates arrows from edge normals
  void init_arrows();
  
  virtual QString helpString() const;

  void draw_spiral();
  void draw_arrow();
  
  QList<dbrec3d_arrow> arrows_;
  
  boxm_scene_base_sptr scene_;
};




//class dbrec3d_scene : public QGLWidget
//{
//  Q_OBJECT
//  
//public:
//  dbrec3d_scene(QWidget* parent=0);
//  
//  ~dbrec3d_scene();
//  
//protected:
//  //: Inherited form QGLWidget
//  void initializeGL();
//  //: Inherited form QGLWidget
//  void resizeGL(int width, int height);
//  //: Inherited form QGLWidget
//  void paintGL();
//    
//  void mousePressEvent(QMouseEvent *event);
//  void mouseMoveEvent(QMouseEvent *event);
//  void mouseDoubleClickEvent(QMouseEvent *event){}
//  
//private:
//  void draw();
//  
//  void draw_arrow();
//  
//  int line_at_position(const QPoint &pos);
//  
//  GLint display_list_;
//  float angle_;
//  float delta_angle_;
//  int delta_move_;
//  vnl_float_3 cam_pos_;
//  vnl_float_3 line_sight_;
//  
//  //: The canonical arrow axis is the default orientation of glutSolidCone - 0, 0, 1
//  //  And the canonical vertexs of the lines are match this direction
//  const vnl_float_3 canonical_arrow_axis_;
//  const vnl_float_3 line_p0_;
//  const vnl_float_3 line_p1_;
//  
//  GLfloat rotation_x_;
//  GLfloat rotation_y_;
//  GLfloat rotation_z_;
//  QColor line_color_;
//  QPoint last_pos_;
//};

#endif
