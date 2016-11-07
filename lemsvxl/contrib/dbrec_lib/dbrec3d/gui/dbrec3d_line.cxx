//:
// \file
// \author Isabel Restrepo
// \date 14-Oct-2010

#include "dbrec3d_line.h"
#ifdef __APPLE__
# include <glut.h>
#else
# include <GL/glut.h>
#endif

//: Contructor 
dbrec3d_line::dbrec3d_line(QWidget *parent):QGLWidget(parent)
{
  // Set OpenGL display context
  setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));
  
  // Initialize private variables
  rotation_x_ = -21.0;
  rotation_y_ = -57.0;
  rotation_z_ = 0.0;
  line_color_ = Qt::red;
  
}

dbrec3d_line::~dbrec3d_line(){}

//: Set up OpenGL rendering context, define display list - called once befor paintGL
void dbrec3d_line::initializeGL()
{
  qglClearColor(Qt::black);
  glShadeModel(GL_FLAT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}


//: Called whenever widget is resized - set up OpenGl viewport and projection
void dbrec3d_line::resizeGL(int width, int height)
{
  //Reset the coorsinate system
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  //Set viwport to entire window
  glViewport(0, 0, width, height);
  
  //Set the correct perspective
  GLfloat x = GLfloat(width) / height;
  //glFrustum(-x, +x, -1.0, +1.0, 4.0, 15.0);
  gluPerspective(45,(double)width/(double)height,1,1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0,0.0,0.0,  // eye position - gets mapped to the origing
            0.0,0.0,-1.0, //reference point - center of the scene
            0.0f,1.0f,0.0f);} //up vector

//: Called whenever the widget needs to be repainted
void dbrec3d_line::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  draw();
}


void dbrec3d_line::draw()
{
  // Start and End points
  static const GLfloat p0[3] = { 0.0, 0.0, 0.0 };
  static const GLfloat p1[3] = { 0.0, 0.0, -1.0  };
  
  
  //Draw the line
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0.0,0.0,10.0,  // eye position - gets mapped to the origing
            0.0,0.0,0.0, //reference point - center of the scene
            0.0f,1.0f,0.0f); //up vector
  //glTranslatef(0.0, 0.0, -5.0);
  glRotatef(rotation_x_, 1.0, 0.0, 0.0);
  glRotatef(rotation_y_, 0.0, 1.0, 0.0);
  glRotatef(rotation_z_, 0.0, 0.0, 1.0);
  
  glLoadName(0);
  glBegin(GL_LINES);
  qglColor(line_color_);
  glVertex3f(p0[0], p0[1], p0[2]);
  glVertex3f(p1[0], p1[1], p1[2]);
  glEnd();
  
  //Draw the cone
  // glTranslatef(p0[0], p0[1], p0[2]);
  //  glRotatef(rotation_x_, 1.0, 0.0, 0.0);
  //  glRotatef(rotation_y_, 0.0, 1.0, 0.0);
  //  glRotatef(rotation_z_, 0.0, 0.0, 1.0);
  glutSolidCone(0.08f,0.5f,10,2);
  
}
//: Handle Events
void dbrec3d_line::mousePressEvent(QMouseEvent *event)
{
  last_pos_ = event->pos();
}

//: Allow the user to rotate the view by clicking and dragging. 
//  The left mouse button allows the user to rotate around the x- and y-axes, the right mouse button around the x- and z-axes.
void dbrec3d_line::mouseMoveEvent(QMouseEvent *event)
{
  GLfloat dx = GLfloat(event->x() - last_pos_.x()) / width();
  GLfloat dy = GLfloat(event->y() - last_pos_.y()) / height();
  if (event->buttons() & Qt::LeftButton) {
    //update positions
    rotation_x_ += 180 * dy;
    rotation_y_ += 180 * dx;
    //redraw
    updateGL();
  } else if (event->buttons() & Qt::RightButton) {
    //update positions
    rotation_x_ += 180 * dy;
    rotation_z_ += 180 * dx;
    //redraw
    updateGL();
  }
  last_pos_ = event->pos();
}


