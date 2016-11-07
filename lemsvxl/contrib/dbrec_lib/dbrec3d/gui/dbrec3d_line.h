// This is dbrec3d_gui_dbrec3d_line_h
#ifndef dbrec3d_line_h
#define dbrec3d_line_h

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

#include <QObject>
#include <QGLWidget>
#include <QWidget>
#include <QtGui>
#include <QColor>
#include <QPoint>

class dbrec3d_line : public QGLWidget
{
  Q_OBJECT
  
public:
  dbrec3d_line(QWidget* parent=0);
  ~dbrec3d_line();
  
protected:
  //: Inherited form QGLWidget
  void initializeGL();
  //: Inherited form QGLWidget
  void resizeGL(int width, int height);
  //: Inherited form QGLWidget
  void paintGL();
  
  
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseDoubleClickEvent(QMouseEvent *event){}
  
private:
  void draw();
  int line_at_position(const QPoint &pos);
  
  GLfloat rotation_x_;
  GLfloat rotation_y_;
  GLfloat rotation_z_;
  QColor line_color_;
  QPoint last_pos_;
};

#endif
