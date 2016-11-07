//:
// \file
// \author Isabel Restrepo
// \date 26-Oct-2010



#include "dbrec3d_scene_widgets.h"

#include <vnl/vnl_vector_fixed.h>
#include <boxm/boxm_scene.h>

#include <vcl_algorithm.h>
#include <vcl_iterator.h>

/********************* scene widget *************************/
//: Constructor - from scene. 
dbrec3d_gradient_scene_widget::dbrec3d_gradient_scene_widget(dbrec3d_scene_model *scene_data, QWidget* parent, const QGLWidget* shareWidget)//: QGLViewer(parent, shareWidget)
{
  // Set the scene parameters
  scene_data_ = scene_data;
  makeCurrent();
  filter_by_dir_ = false;
  dir_to_filter_ = vnl_vector_fixed<float, 3>(0.0f, 0.0f, 0.1f); 
  init_arrows();
  bbox_ = scene_data_->scene_base_->get_world_bbox();
  
}

dbrec3d_gradient_scene_widget::~dbrec3d_gradient_scene_widget()
{
  arrows_.clear();
}


//Draw one by one
void dbrec3d_gradient_scene_widget::draw()
{
  for(unsigned b = min_bin_; b< max_bin_; b++)
    for (QList<dbrec3d_arrow>::const_iterator it=arrows_[b].constBegin(), end=arrows_[b].constEnd(); it != end; ++it)
    {
      dbrec3d_arrow arrow = (*it);
      glLineWidth(10.0);
      vnl_float_3 color = (arrow.end_pos_ - arrow.pos_).normalize();
      if(filter_by_dir_ )
        if((dot_product(color, dir_to_filter_.normalize()) < vcl_cos(vnl_math::pi_over_4) ))
          continue;
      glColor3f(vcl_abs(color[0]), vcl_abs(color[1]), vcl_abs(color[2]));
      QGLViewer::drawArrow(qglviewer::Vec(arrow.pos_[0],arrow.pos_[1], arrow.pos_[2]), qglviewer::Vec(arrow.end_pos_[0],arrow.end_pos_[1], arrow.end_pos_[2]), arrow.length_/10.0);
    }
}

#if 0
//: Faster dawing by using glDrawArray - the problem is if the array is too large it may chuck
void dbrec3d_gradient_scene_widget::draw_fast()
{
  int value;
  glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &value);
  unsigned nvertices3d = vcl_floor(scene_data_->hist_.area())*2*3;
  vcl_cout <<value<< vcl_endl; 
  
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  
  glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	
  GLint f = (scene_data_->hist_.cumulative_area(min_bin_)) *2* 3 ;
	glColorPointer(3, GL_FLOAT, 0, &colors_[f]);
	glVertexPointer(3, GL_FLOAT, 0, &vertices_[f]);
  glLineWidth(2.0);
  
	for (unsigned bin = min_bin_; bin < max_bin_ -1 ; bin++) 
  {
    GLint first = (scene_data_->hist_.cumulative_area(bin)) * 2* 3 - f;
    GLint count = (scene_data_->hist_.counts(bin)) * 2 * 3;
    if (count > 0)
      //DrawArrays(mode, first[i], count[i]);
      //glDrawArrays(GL_LINES,0, 30000 );
      
      glDrawArrays(GL_LINES,first, count );
	}
  
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
  
}

#endif

void dbrec3d_gradient_scene_widget::init()
{
  makeCurrent();

  //init scene geometry
  setSceneBoundingBox(qglviewer::Vec(bbox_.min_x(), bbox_.min_y(), bbox_.min_z()), qglviewer::Vec(bbox_.max_x(), bbox_.max_y(), bbox_.max_z()));
  showEntireScene();
  setAxisIsDrawn();
  vcl_cout << "Scene Radius: " << sceneRadius() << vcl_endl;
  
  // Opens help window
  help();
  
}

#if 0
//: Initializes arrays to set of lines in the direction of the gradient
void dbrec3d_gradient_scene_widget::init_gradient_lines()
{
  //allocate the arrays
  unsigned nvertices3d = vcl_floor(scene_data_->hist_.area())*2*3;
  //vertices_.reserve(nvertices3d);
  //colors_.reserve(nvertices3d);
  
  //iterate throught the scene organizing vertices so that tresholding becomes easier
  vcl_vector<vcl_vector<float> > binned_vertices;
  
  for(unsigned i = 0; i < scene_data_->hist_.nbins(); i++)
    binned_vertices.push_back(vcl_vector<float>());
  
  //cast the gradient scene
  typedef vnl_vector_fixed< float,3 > datatype;
  typedef boct_tree<short, datatype > grad_tree_type;
  if (boxm_scene<grad_tree_type> *grad_scene = dynamic_cast<boxm_scene< grad_tree_type >* >(scene_data_->scene().as_pointer()))
  {
    //iterate through cells
    boxm_cell_iterator<grad_tree_type > iter = grad_scene->cell_iterator(&boxm_scene<grad_tree_type >::load_block);
    iter.begin();
    
    float min_cell_length = grad_scene->finest_cell_length();
    
    while(!iter.end())
    {
      boct_tree_cell<short,datatype> *cell = *iter;
      
      vnl_vector_fixed<float, 3 > p0(iter.global_centroid().x(),iter.global_centroid().y(), iter.global_centroid().z());
      
      vnl_vector_fixed<float, 3 > nom_grad = cell->data().normalize();
      
      
      vnl_vector_fixed<float, 3 > p1 = p0 + (nom_grad * min_cell_length);
      
        
      //determine the bin
      int bin = scene_data_->hist_.bin_at_val(cell->data().magnitude());
      if(bin >= 0){
        binned_vertices[bin].push_back(p0[0]); binned_vertices[bin].push_back(p0[1]); binned_vertices[bin].push_back(p0[2]);
        binned_vertices[bin].push_back(p1[0]); binned_vertices[bin].push_back(p1[1]); binned_vertices[bin].push_back(p1[2]);
      }
      
      ++iter;
    }
    
  }
  
  //: Now assemble the vertices such that they are orginez by bins - release the memory occuppyed by the temp stl-vectors
  for(unsigned i = 0; i < scene_data_->hist_.nbins(); i++)
  {
    vcl_copy(binned_vertices[i].begin(),binned_vertices[i].end(),vcl_back_inserter(vertices_));
    vcl_copy(binned_vertices[i].begin(),binned_vertices[i].end(),vcl_back_inserter(colors_));
    binned_vertices[i].clear();
  }
  
  min_bin_ = 1;
  max_bin_ = 10;
  //max_bin_ = scene_data_->hist_.nbins() - 1;
  
  for(unsigned i = 0; i < vertices_.size(); i++)
  {
    vcl_sqrt(vertices_[i]);
  }
  
  vcl_cout << " Array is allocated \n" ; 
  
    
}
#endif

//: A function that iterates through leaf cells and creates arrows from edge normals
void dbrec3d_gradient_scene_widget::init_arrows()
{
  
  // initialize related class variables
  min_bin_ = 1;
  max_bin_ = scene_data_->hist_.nbins() - 1;
  
  for(unsigned i = 0; i < scene_data_->hist_.nbins(); i++)
    arrows_.push_back(QList<dbrec3d_arrow>());
  
  
  boxm_scene_base_sptr scene_base = scene_data_->scene();
  
  //cast the gradient scene
  typedef vnl_vector_fixed< float,3 > datatype;
  typedef boct_tree<short, datatype > grad_tree_type;
  if (boxm_scene<grad_tree_type> *grad_scene = dynamic_cast<boxm_scene< grad_tree_type >* >(scene_base.as_pointer()))
  {
    float min_cell_length = grad_scene->finest_cell_length();

    //iterate through cells
    boxm_cell_iterator<grad_tree_type > iter = grad_scene->cell_iterator(&boxm_scene<grad_tree_type >::load_block);
    iter.begin();
       
    while(!iter.end())
    {
      boct_tree_cell<short,datatype> *cell = *iter;
      
      vnl_vector_fixed<float, 3 > p0(iter.global_centroid().x(),iter.global_centroid().y(), iter.global_centroid().z());
      
      vnl_vector_fixed<float, 3 > nom_grad = cell->data().normalize();

      vnl_vector_fixed<float, 3 > p1 = p0 + (nom_grad * min_cell_length);
      
      // The cell data is a gradiant, we will normalize it. the magnitude/strength is ignored for now
      dbrec3d_arrow arrow(p0, p1, min_cell_length);
      
      //determine the bin
      int bin = scene_data_->hist_.bin_at_val(cell->data().magnitude());
      if(bin >= 0)
        arrows_[bin].push_back(dbrec3d_arrow(p0, p1, min_cell_length));
      ++iter;
    }    
  }
  
  //cast the scene with steerable filters
  typedef vnl_vector_fixed< float,10 > sf_datatype;
  typedef boct_tree<short, sf_datatype > sf_tree_type;
  if (boxm_scene<sf_tree_type> *sf_scene = dynamic_cast<boxm_scene< sf_tree_type >* >(scene_base.as_pointer()))
  {
    float min_cell_length = sf_scene->finest_cell_length();
    
    //iterate through cells
    boxm_cell_iterator<sf_tree_type > iter = sf_scene->cell_iterator(&boxm_scene<sf_tree_type >::load_block);
    iter.begin();
    
    while(!iter.end())
    {
    
      boct_tree_cell<short, sf_datatype> *cell = *iter;
      
      if (cell->level() != 0) {
        ++iter;
        continue;
      }
      
      vnl_vector_fixed<float, 3 > p0(iter.global_centroid().x(),iter.global_centroid().y(), iter.global_centroid().z());
      
      vnl_vector_fixed<float, 10 > cell_data = cell->data();
      vnl_vector_fixed<float, 3 > nom_grad(cell_data[1],cell_data[2],cell_data[3]);
      
      
      vnl_vector_fixed<float, 3 > p1 = p0 + (nom_grad.normalize() * min_cell_length);
      
      // The cell data is a gradiant, we will normalize it. the magnitude/strength is ignored for now
      dbrec3d_arrow arrow(p0, p1, min_cell_length);
      
      //determine the bin
      int bin = scene_data_->hist_.bin_at_val(cell->data().magnitude());
      if(bin >= 0)
        arrows_[bin].push_back(dbrec3d_arrow(p0, p1, min_cell_length));
      ++iter;
    }    
  }
  
}

QString dbrec3d_gradient_scene_widget::helpString() const
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

QSize dbrec3d_gradient_scene_widget::sizeHint()const
{
  return QSize(640,600);
}

void dbrec3d_gradient_scene_widget::update_dir_to_filter()
{
  float x = vcl_cos((float)azimuth_);
  float y = vcl_sin((float)azimuth_);
  float z = vcl_cos((float)polar_);
  
  dir_to_filter_.put(0,x);
  dir_to_filter_.put(1,y);
  dir_to_filter_.put(2,z);
  
}

/******************* Filter dir widget *****************************/

void dbrec3d_filter_dir_widget::draw()
{
  //vcl_cout << this->camera()->orientation() << vcl_endl;
  
//  glColor3f(0, 0, 1);
//  QGLViewer::drawArrow(qglviewer::Vec(0,0,0), qglviewer::Vec(0,0,1));
  makeCurrent();
  glBegin(GL_LINES);
    glColor3f(0,0,1);
    glVertex3f(0,0,0);
    glVertex3f(0,0,sceneRadius());
  
  glEnd();
  
}

void dbrec3d_filter_dir_widget::init()
{
  makeCurrent();
  setSceneBoundingBox(qglviewer::Vec(bbox_.min_x(), bbox_.min_y(), bbox_.min_z()), qglviewer::Vec(bbox_.max_x(), bbox_.max_y(), bbox_.max_z()));
  showEntireScene();
  setAxisIsDrawn();
  vcl_cout << "Scene Radius: " << sceneRadius() << vcl_endl;
}

/********************* Points Scene Widget **********************************/

dbrec3d_points_scene_widget::dbrec3d_points_scene_widget(vcl_string file, boxm_scene_base_sptr scene_base)
{
  init_points(file);
  bbox_ = scene_base->get_world_bbox();
}

void dbrec3d_points_scene_widget::init_points(vcl_string filename)
{
  vcl_ifstream loc_ifs(filename.c_str());
  
  unsigned long i;
  while (!loc_ifs.eof()) {
    //if(i%2 ==0 )
    {
      vgl_point_3d<double> this_loc;
      loc_ifs >> this_loc;
      points_.push_back(this_loc);
    }
    i++;
  }
  vcl_cout << " Out of while loop" <<vcl_endl;
}

void dbrec3d_points_scene_widget::init()
{
 
  //init scene geometry
  setSceneBoundingBox(qglviewer::Vec(bbox_.min_x(), bbox_.min_y(), bbox_.min_z()), qglviewer::Vec(bbox_.max_x(), bbox_.max_y(), bbox_.max_z()));
  showEntireScene();
  setAxisIsDrawn();
  vcl_cout << "Scene Radius: " << sceneRadius() << vcl_endl;
  
  // Opens help window
  help();
  
}
void dbrec3d_points_scene_widget::draw()
{
  makeCurrent();
  glBegin(GL_POINTS);
  for (QList<vgl_point_3d<double> >::const_iterator it=points_.constBegin(), end=points_.constEnd(); it != end; ++it)
  {   
    vgl_point_3d<double> point = *it;
    glVertex3f(point.x(),point.y(),point.z());
  
  }
  glEnd();
}

QString dbrec3d_points_scene_widget::helpString() const
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

/********************* ID Scene Widget **********************************/
//: Constructor - from scene. 
dbrec3d_id_scene_widget::dbrec3d_id_scene_widget(dbrec3d_scene_model *scene_data, QWidget* parent, const QGLWidget* shareWidget)//: QGLViewer(parent, shareWidget)
{
  // Set the scene parameters
  scene_data_ = scene_data;
  makeCurrent();
  init_data();
  bbox_ = scene_data_->scene_base_->get_world_bbox();
  
}

dbrec3d_id_scene_widget::~dbrec3d_id_scene_widget()
{
  points_.clear();
  colors_.clear();
}


//Draw one by one
void dbrec3d_id_scene_widget::draw()
{
  makeCurrent();
  glBegin(GL_POINTS);
  for(unsigned b = min_bin_; b< max_bin_; b++)
    for (QList<vgl_point_3d<float> >::const_iterator it=points_[b].constBegin(), end=points_[b].constEnd(); it != end; ++it)
    {
      glColor3ub(colors_[b][0],colors_[b][1],colors_[b][2]);
      vgl_point_3d<float> point = *it;
      glVertex3f(point.x(),point.y(),point.z());    
    }
  glEnd();
}


void dbrec3d_id_scene_widget::init()
{
  makeCurrent();
  
  //init scene geometry
  setSceneBoundingBox(qglviewer::Vec(bbox_.min_x(), bbox_.min_y(), bbox_.min_z()), qglviewer::Vec(bbox_.max_x(), bbox_.max_y(), bbox_.max_z()));
  showEntireScene();
  setAxisIsDrawn();
  vcl_cout << "Scene Radius: " << sceneRadius() << vcl_endl;
  
  // Opens help window
  help();
  
}


//: A function that iterates through leaf cells and creates arrows from edge normals
void dbrec3d_id_scene_widget::init_data()
{
  
  // initialize related class variables
  min_bin_ = 0;
  max_bin_ = scene_data_->hist_.nbins() - 1;
  vnl_random rand;
  
  for(unsigned i = 0; i < scene_data_->hist_.nbins(); i++){
    points_.push_back(QList<vgl_point_3d<float> >());
    char r=rand.lrand32(128,255);                             // Pick A Random Red Color (Bright)
    char g=rand.lrand32(128,255);                            // Pick A Random Green Color (Bright)
    char b=rand.lrand32(128,255); 
    colors_.push_back(vnl_vector_fixed<char, 3>(r, g, b));
  }
  
  
  boxm_scene_base_sptr scene_base = scene_data_->scene();
  
  //cast the id scene
  typedef boct_tree<short, short > id_tree_type;
  if (boxm_scene<id_tree_type> *id_scene = dynamic_cast<boxm_scene< id_tree_type >* >(scene_base.as_pointer()))
  {
    
    //iterate through cells
    boxm_cell_iterator<id_tree_type > iter = id_scene->cell_iterator(&boxm_scene<id_tree_type >::load_block, true);
    iter.begin();
    
    while(!iter.end())
    {  
     
      // The cell data is a gradiant, we will normalize it. the magnitude/strength is ignored for now
      vgl_point_3d<double> centroid = iter.global_centroid();
      
      //determine the bin
      short bin = iter->data();
      if(bin >= 0)
        points_[bin].push_back(vgl_point_3d<float>((float)centroid.x(), (float)centroid.y(), (float)centroid.z()));
      ++iter;
    }    
  }
  
}

QString dbrec3d_id_scene_widget::helpString() const
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

QSize dbrec3d_id_scene_widget::sizeHint()const
{
  return QSize(640,600);
}