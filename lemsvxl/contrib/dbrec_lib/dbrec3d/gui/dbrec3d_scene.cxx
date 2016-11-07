//:
// \file
// \author Isabel Restrepo
// \date 14-Oct-2010

#include "dbrec3d_scene.h"
#ifdef __APPLE__
# include <glut.h>
#else
# include <GL/glut.h>
#endif

#include <QGLViewer/vec.h>

#include <vgl/vgl_box_3d.h>

//: OpenGL code to paint a 3d arrow in its cannonical position
void dbrec3d_arrow::draw()
{
  //Draw the line
  glBegin(GL_LINES);
  glColor3f(1.0, 0.0f , 0.0f);
  glVertex3f(pos_[0], pos_[1], pos_[2]);
  glVertex3f(end_pos_[0], end_pos_[1], end_pos_[2]);
  glEnd();
  
  //Draw the cone
  glPushMatrix();
  glTranslatef(end_pos_[0], end_pos_[1], end_pos_[2]);
  glutSolidCone(length_/24.0f,length_/4.0f,10,2); //by default oriented alog z axis - out of the screen
  glPopMatrix();
  
}


//: OpenGL code to paint a 3d arrow in its cannonical position
void dbrec3d_scene_widget::draw_arrow()
{
  vnl_float_3 line_p0_(0.0f, 0.0f, 0.0f);
  vnl_float_3 line_p1_(0.0f, 0.0f, 1.0f);
  
  //Draw the line
  glBegin(GL_LINES);
  qglColor(Qt::red);
  glVertex3f(line_p0_[0], line_p0_[1], line_p0_[2]);
  glVertex3f(line_p1_[0], line_p1_[1], line_p1_[2]);
  glEnd();
  
  //Draw the cone
  glPushMatrix();
  glTranslatef(line_p1_[0], line_p1_[1], line_p1_[2]);
  glutSolidCone(0.08f,0.5f,10,2); //by default oriented alog z axis - out of the screen
  glPopMatrix();
  
}

// Draws a spiral
void dbrec3d_scene_widget::draw_spiral()
{
  const float nbSteps = 200.0;
  
  glBegin(GL_QUAD_STRIP);
  for (int i=0; i<nbSteps; ++i)
  {
    const float ratio = i/nbSteps;
    const float angle = 21.0*ratio;
    const float c = cos(angle);
    const float s = sin(angle);
    const float r1 = 1.0 - 0.8f*ratio;
    const float r2 = 0.8f - 0.8f*ratio;
    const float alt = ratio - 0.5f;
    const float nor = 0.5f;
    const float up = sqrt(1.0-nor*nor);
    glColor3f(1.0-ratio, 0.2f , ratio);
    glNormal3f(nor*c, up, nor*s);
    glVertex3f(r1*c, alt, r1*s);
    glVertex3f(r2*c, alt+0.05f, r2*s);
  }
  glEnd();
}

void dbrec3d_scene_widget::draw()
{
  for (QList<dbrec3d_arrow>::iterator it=arrows_.begin(), end=arrows_.end(); it != end; ++it){
    dbrec3d_arrow arrow = (*it);
    glLineWidth(10.0);
    vnl_float_3 color = (arrow.end_pos_ - arrow.pos_).normalize();
    glColor3f(color[0], color[1], color[2]);
    QGLViewer::drawArrow(qglviewer::Vec(arrow.pos_[0],arrow.pos_[1], arrow.pos_[2]), qglviewer::Vec(arrow.end_pos_[0],arrow.end_pos_[1], arrow.end_pos_[2]), arrow.length_/10.0);
    //(*it).draw();
  }
}

void dbrec3d_scene_widget::init()
{
  // Set the scene parameters
  if(!scene_)
    return;
  
  init_arrows();
  vgl_box_3d<double> bbox = scene_->get_world_bbox();
  
  //init scene geometry
  setSceneBoundingBox(qglviewer::Vec(bbox.min_x(), bbox.min_y(), bbox.min_z()), qglviewer::Vec(bbox.max_x(), bbox.max_y(), bbox.max_z()));
  showEntireScene();
  setAxisIsDrawn();
  vcl_cout << "Scene Radius: " << sceneRadius() << vcl_endl;
  
  
  
  // Opens help window
  help();
  
}

//: A function that iterates through leaf cells and creates arrows from edge normals
void dbrec3d_scene_widget::init_arrows()
{
  
  //cast the gradient scene
  typedef vnl_vector_fixed< float,3 > datatype;
  typedef boct_tree<short, datatype > grad_tree_type;
  if (boxm_scene<grad_tree_type> *grad_scene = dynamic_cast<boxm_scene< grad_tree_type >* >(scene_.as_pointer()))
  {
    //iterate through cells
    boxm_cell_iterator<grad_tree_type > iter = grad_scene->cell_iterator(&boxm_scene<grad_tree_type >::load_block);
    iter.begin();
    
    float min_cell_length = grad_scene->finest_cell_length();
    
    while(!iter.end())
    {
      boct_tree_cell<short,datatype> *cell = *iter;
      
      vnl_vector_fixed<float, 3 > p0(iter.global_origin().x(),iter.global_origin().y(), iter.global_origin().z());
      if(cell->data().two_norm() < 1e-9){
        ++iter;
        continue;
      }
      vnl_vector_fixed<float, 3 > nom_grad = cell->data().normalize();
      vnl_vector_fixed<float, 3 > p1 = p0 + (nom_grad * min_cell_length);
      
      // The cell data is a gradiant, we will normalize it. the magnitude/strength is ignored for now
      dbrec3d_arrow arrow(p0, p1, min_cell_length);
      arrows_.push_back(dbrec3d_arrow(p0, p1, min_cell_length));
      ++iter;
    }    
  }
  
}


QString dbrec3d_scene_widget::helpString() const
{
  QString text("<h2>S i m p l e V i e w e r</h2>");
  text += "Use the mouse to move the camera around the object. ";
  text += "You can respectively revolve around, zoom and translate with the three mouse buttons. ";
  text += "Left and middle buttons pressed together rotate around the camera view direction axis<br><br>";
  text += "Pressing <b>Alt</b> and one of the function keys (<b>F1</b>..<b>F12</b>) defines a camera keyFrame. ";
  text += "Simply press the function key again to restore it. Several keyFrames define a ";
  text += "camera path. Paths are saved when you quit the application and restored at next start.<br><br>";
  text += "Press <b>F</b> to display the frame rate, <b>A</b> for the world axis, ";
  text += "<b>Alt+Return</b> for full screen mode and <b>Control+S</b> to save a snapshot. ";
  text += "See the <b>Keyboard</b> tab in this window for a complete shortcut list.<br><br>";
  text += "Double clicks automates single click actions: A left button double click aligns the closer axis with the camera (if close enough). ";
  text += "A middle button double click fits the zoom of the camera and the right button re-centers the scene.<br><br>";
  text += "A left button double click while holding right button pressed defines the camera <i>Revolve Around Point</i>. ";
  text += "See the <b>Mouse</b> tab and the documentation web pages for details.<br><br>";
  text += "Press <b>Escape</b> to exit the viewer.";
  return text;
}



//: Contructor 
//dbrec3d_scene::dbrec3d_scene(QWidget *parent):QGLWidget(parent),canonical_arrow_axis_(vnl_float_3(0.0f, 0.0f, 1.0f)),
//line_p0_(vnl_float_3(0.0f, 0.0f, 0.0f)),
//line_p1_(vnl_float_3(0.0f, 0.0f, 1.0f))
//{
//  // Set OpenGL display context
//  setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));
//  
//  // Initialize private variables
//  rotation_x_ = 0.0;
//  rotation_y_ = 0.0;
//  rotation_z_ = 0.0;
//  angle_= 0.0f;
//  delta_angle_ = 0.0f;
//  delta_move_ = 0;
//  cam_pos_ = vnl_float_3(0.0f,0.0f,10.0f);
//  line_sight_ = vnl_float_3(0.0f,0.0f,-1.0f);
//  line_color_ = Qt::red;
//  
//}
//
//dbrec3d_scene::~dbrec3d_scene(){}
//
//: Set up OpenGL rendering context, define display list - called once befor paintGL
//void dbrec3d_scene::initializeGL()
//{
//  qglClearColor(Qt::black);
//  glShadeModel(GL_FLAT);
//  glEnable(GL_DEPTH_TEST);
//  glEnable(GL_CULL_FACE);
//}
//
//
//: Called whenever widget is resized - set up OpenGl viewport and projection
//void dbrec3d_scene::resizeGL(int width, int height)
//{
//  //Reset the coorsinate system
//  glMatrixMode(GL_PROJECTION);
//  glLoadIdentity();
//  
//  //Set viwport to entire window
//  glViewport(0, 0, width, height);
//  
//  //Set the correct perspective and clipping volume
//  GLfloat x = GLfloat(width) / height;
//  gluPerspective(45,(double)width/(double)height,1,1000);
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//	gluLookAt(cam_pos_[0], cam_pos_[1], cam_pos_[2],  // eye position - gets mapped to the origing
//            cam_pos_[0]+line_sight_[0], cam_pos_[1]+line_sight_[1], cam_pos_[2]+line_sight_[2], //reference point - center of the scene
//            0.0f,1.0f,0.0f);} //up vector
//
//: Called whenever the widget needs to be repainted
//void dbrec3d_scene::paintGL()
//{
//  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//  draw();
//}
//
//: OpenGL code to paint a 3d arrow in its cannonical position
//void dbrec3d_scene::draw_arrow()
//{
//  //Draw the line
//  glBegin(GL_LINES);
//  qglColor(line_color_);
//  glVertex3f(line_p0_[0], line_p0_[1], line_p0_[2]);
//  glVertex3f(line_p1_[0], line_p1_[1], line_p1_[2]);
//  glEnd();
//  
//  //Draw the cone
//  glPushMatrix();
//  glTranslatef(line_p1_[0], line_p1_[1], line_p1_[2]);
//  glutSolidCone(0.08f,0.5f,10,2); //by default oriented alog z axis - out of the screen
//  glPopMatrix();
//  
//}
//
//
//: OpenGL code to paint the 3d line
//void dbrec3d_scene::draw()
//{
//  //Draw the arrows:
//  glPushMatrix();
//  glMatrixMode(GL_MODELVIEW);
//  glLoadIdentity();
//  //update camera
//  vcl_cout << "cam: " <<cam_pos_ << " lines: " << line_sight_ + cam_pos_ << vcl_endl;
//  gluLookAt(cam_pos_[0], cam_pos_[1], cam_pos_[2],  // eye position - gets mapped to the origing
//            cam_pos_[0]+line_sight_[0], cam_pos_[1]+line_sight_[1], cam_pos_[2]+line_sight_[2], //reference point - center of the scene
//            0.0f,1.0f,0.0f);//up vector
//  
//  draw_arrow();
//  glPopMatrix();
//  
//}
//
//void dbrec3d_scene::draw()
//{
//  // Start and End points
//  static const GLfloat p0[3] = { 0.0, 0.0, 0.0 };
//  static const GLfloat p1[3] = { 0.0, 0.0, -1.0  };
//  
//  
//  //Draw the line
//  glMatrixMode(GL_MODELVIEW);
//  glLoadIdentity();
//  gluLookAt(0.0,0.0,10.0,  // eye position - gets mapped to the origing
//            0.0,0.0,0.0, //reference point - center of the scene
//            0.0f,1.0f,0.0f); //up vector
//  //glTranslatef(0.0, 0.0, -5.0);
//  glRotatef(rotation_x_, 1.0, 0.0, 0.0);
//  glRotatef(rotation_y_, 0.0, 1.0, 0.0);
//  glRotatef(rotation_z_, 0.0, 0.0, 1.0);
//  
//  glLoadName(0);
//  glBegin(GL_LINES);
//  qglColor(line_color_);
//  glVertex3f(p0[0], p0[1], p0[2]);
//  glVertex3f(p1[0], p1[1], p1[2]);
//  glEnd();
//  
//  //Draw the cone
//  // glTranslatef(p0[0], p0[1], p0[2]);
//  //  glRotatef(rotation_x_, 1.0, 0.0, 0.0);
//  //  glRotatef(rotation_y_, 0.0, 1.0, 0.0);
//  //  glRotatef(rotation_z_, 0.0, 0.0, 1.0);
//  glutSolidCone(0.08f,0.5f,10,2);
//  
//}
//
//: Handle Events
//void dbrec3d_scene::mousePressEvent(QMouseEvent *event)
//{
//  last_pos_ = event->pos();
//}
//
////: Allow the user to rotate  and translate the camera by clicking and dragging. 
////  The left mouse button allows the user to rotate around the x- and y-axes, the right mouse button around the x- and z-axes.
//void dbrec3d_scene::mouseMoveEvent(QMouseEvent *event)
//{
//  GLfloat dx = GLfloat(event->x() - last_pos_.x()) / width();
//  GLfloat dy = GLfloat(event->y() - last_pos_.y()) / height();
//  
//  GLdouble x = event->x(); 
//  GLdouble y = event->y();
//  
//  if (event->buttons() & Qt::LeftButton) {
//    //update positions
//    float angle = vnl_math::pi * dx;
//    rotation_x_ += vnl_math::pi * dx;
//    rotation_y_ += 180 * dx;
//        
//    line_sight_[0]= vcl_sin(angle);
//    line_sight_[2]= line_sight_[2] - vcl_cos(angle) ;
//    
//    
//  
//    float wscale = 2.0f / width();
//    float hscale = 2.0f / height();
//    float delta_r[4];
//    
//    //redraw
//    updateGL();
//    
//    
//  } else if (event->buttons() & Qt::RightButton) {
//    //update positions
//    cam_pos_[0]= cam_pos_[0] - dx;
//    cam_pos_[1]= cam_pos_[1] + dy;
//    //redraw
//    updateGL();
//  }
//  last_pos_ = event->pos();
//}
//
////: Allow the user to rotate the view by clicking and dragging. 
////  The left mouse button allows the user to rotate around the x- and y-axes, the right mouse button around the x- and z-axes.
////void dbrec3d_scene::mouseMoveEvent(QMouseEvent *event)
////{
////  GLfloat dx = GLfloat(event->x() - last_pos_.x()) / width();
////  GLfloat dy = GLfloat(event->y() - last_pos_.y()) / height();
////  if (event->buttons() & Qt::LeftButton) {
////    //update positions
////    rotation_x_ += 180 * dy;
////    rotation_y_ += 180 * dx;
////    //redraw
////    updateGL();
////  } else if (event->buttons() & Qt::RightButton) {
////    //update positions
////    rotation_x_ += 180 * dy;
////    rotation_z_ += 180 * dx;
////    //redraw
////    updateGL();
////  }
////  last_pos_ = event->pos();
////}
//
//
