#include "poly_coin3d_manager.h"
#include "poly_coin3d_proj_simple_observer.h"
#include "poly_menu.h"

#include <vgui/vgui_grid_tableau.h> 
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_viewer2D_tableau.h> 
#include <bgui/bgui_picker_tableau.h>
#include <vgui/vgui_deck_tableau.h>
#include <vgui/vgui_composite_tableau.h>
#include <bgui3d/bgui3d_examiner_tableau.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vgui/vgui_projection_inspector.h>
#include <bgui3d/bgui3d.h>
#include <vul/vul_file.h> 

#include <vnl/vnl_quaternion.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_math.h>

#include <vsol/vsol_point_2d.h>

#include <vgl/vgl_homg_plane_3d.h>  
#include <vgl/vgl_line_3d_2_points.h> 
#include <vgl/algo/vgl_homg_operators_3d.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vpgl/algo/vpgl_camera_compute.h>

#include <vsol/vsol_polygon_3d.h>

#include <vil/vil_load.h>

#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/vis/dbmsh3d_vis_backpt.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>
#include <dbmsh3d/vis/dbmsh3d_vis_face.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/SbVec3f.h>
#include <Inventor/SbRotation.h>

#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoMaterial.h>

#define CAMERA_TYPE 0 //0 for perspective, 1 for rational

static double ext_val = 20.0;
poly_coin3d_manager* poly_coin3d_manager::instance_ = 0;

poly_coin3d_manager* poly_coin3d_manager::instance() {
  if (!instance_) 
    instance_ = new poly_coin3d_manager();
  return poly_coin3d_manager::instance_;
   
}

poly_coin3d_manager::poly_coin3d_manager(void):my_obj(0), my_polygon(0)
{
  bgui3d_init();
}

poly_coin3d_manager::~poly_coin3d_manager(void)
{
}

void test(SoGroup *root)
{
  {
    SoSeparator *group = new SoSeparator;
    SoTransform *myTransform = new SoTransform;
    myTransform->scaleFactor.setValue(1.0f, 1.0f, 1.0f);
    //myTransform->translation.setValue(214.0f, -171.0f, 0.0f);
    group->addChild(myTransform);

    // Add a red material
    SoMaterial *myMaterial = new SoMaterial;
    myMaterial->diffuseColor.setValue(0.0f, 1.0f, 0.0f);  // Green
    group->addChild(myMaterial);

    // Add a cylinder
    group->addChild(new SoCylinder);

    root->addChild(group);
  }

}
void poly_coin3d_manager::setup_scene (vgui_grid_tableau_sptr grid, int argc, char** argv)
{
  // Load the first image
  vcl_string image_file = argv[1];
  vil_image_resource_sptr image = load_image(image_file.c_str());
  if(!image)
  {
    vcl_cout << "couldn't load image\n";
    return;
  }

  // Load the first image
  vcl_string image_file2 = argv[3];
  vil_image_resource_sptr image2 = load_image(image_file2.c_str());
  if(!image2)
  {
    vcl_cout << "couldn't load image\n";
    return;
  }
  // ################################################
  // Load the camera
  vcl_string cam_file1 = argv[2];
  vcl_string cam_file2 = argv[4];
  cam1=0; cam2=0;

  vpgl_rational_camera<double> *rat_cam1=0, *rat_cam2=0;
  if (CAMERA_TYPE == 1) {
    rat_cam1 = new vpgl_rational_camera<double>(cam_file1);
    rat_cam2 = new vpgl_rational_camera<double>(cam_file2);
  }
  
  load_cameras(cam_file1, cam_file2, cam1, cam2);

  grid_tableau_ = grid;

  master_root_ = new SoSelection;
  //SoSeparator* left_root_ = new SoSeparator;
  master_root_->ref();

  // load the examiner-observer-manager tableau
  left_ = new poly_coin3d_proj_simple_observer(*cam1, master_root_);
  vgui_image_tableau_new img_tab(image);
  vgui_composite_tableau_new comp(img_tab, left_);
  vgui_viewer2D_tableau_new viewer(comp);
  grid_tableau_->add_at(viewer, 0, 0);

  // load the examiner-observer-manager tableau
  right_ = new poly_coin3d_observer(master_root_);
  right_->set_camera(*cam1);
  if (CAMERA_TYPE == 1)
    right_->set_trans(rat_proj_cam1.norm_trans());
  
  right_->save_home_position();
  grid_tableau_->add_at(right_, 1, 0);

  // load the picker-observer-manager tableau
  bgui_image_tableau_sptr img_ = bgui_image_tableau_new();
  img_->set_image_resource(image);
  cam_observer_ = new poly_cam_observer(img_);
  if (CAMERA_TYPE == 1)
    cam_observer_->set_camera(rat_cam1);
  else 
    cam_observer_->set_camera(cam1->clone());
  picker_ = bgui_picker_tableau_new(cam_observer_);
  vgui_composite_tableau_sptr pick_mgr = new vgui_composite_tableau(picker_, this);
  vgui_viewer2D_tableau_new l(pick_mgr);
  
  grid_tableau_->add_at(l, 0, 1);
  
  // load the second image tableau, just an observer
  bgui_image_tableau_sptr img2 = bgui_image_tableau_new();
  img2->set_image_resource(image2);
  cam_observer2_ = new poly_cam_observer(img2);
  if (CAMERA_TYPE == 1) 
    cam_observer2_->set_camera(rat_cam2);
  else
    cam_observer2_->set_camera(cam2->clone());
  vgui_viewer2D_tableau_new right(cam_observer2_);
  grid_tableau_->add_at(right, 1, 1);
}

vil_image_resource_sptr poly_coin3d_manager::load_image(vcl_string filename)
{
  vil_image_resource_sptr res;
  float gamma = 1.0;
  bool invert = false;
  bool gl_map = false;
  bool cache = true;
  double min = 0, max = 255; //for now - ultimately need to compute color histogram


  // if filename is a directory, assume pyramid image
  if (vul_file::is_directory(filename)) {
    vil_pyramid_image_resource_sptr pyr = vil_load_pyramid_resource(filename.c_str());
    if (pyr) {
      cache = false;
      gl_map = true;
      res = pyr.ptr();
    }
    else {
      vcl_cerr << "error loading image pyramid "<< filename << vcl_endl;
      return 0;
    }
  } 
  else {
    res = vil_load_image_resource(filename.c_str());
  }
  /*vgui_range_map_params_sptr rmps = new vgui_range_map_params(min, max, min, max, min, max,
    gamma, gamma, gamma, invert,
    gl_map, cache);*/

  return res;
}

void poly_coin3d_manager::load_cameras(vcl_string cam_file1, vcl_string cam_file2,
                                       vpgl_proj_camera<double> * &cam1, 
                                       vpgl_proj_camera<double> * &cam2)
{
  if (CAMERA_TYPE == 0) { //PERSPECTIVE_CAMERA
    vpgl_proj_camera<double> camera;
    vcl_ifstream cam_stream1(cam_file1.data());
    cam_stream1 >> camera;
    cam1 = camera.clone();
   // vcl_cout << *cam1 << vcl_endl;

    vcl_ifstream cam_stream2(cam_file2.data());
    cam_stream2 >> camera;
    cam2 = camera.clone();
   // vcl_cout << *cam2 << vcl_endl;

  } else { // RATIONAL CAMERA
    vpgl_rational_camera<double> rat_cam1;
    vpgl_rational_camera<double> rat_cam2;
    rat_cam1 = vpgl_rational_camera<double>(cam_file1);
    rat_cam2 = vpgl_rational_camera<double>(cam_file2);
    /////////////////////////
    /*vpgl_proj_camera<double> camera;
    vcl_ifstream cam_stream1("C:\\test_images\\providence1\\cam.txt");
    cam_stream1 >> camera;
    cam1 = camera.clone();
    vcl_cout << *cam1 << vcl_endl;*/
    /////////////////////////

    rat_proj_cam1.set_rat_cam(rat_cam1);
    rat_proj_cam1.compute();
   // rat_proj_cam2.set_rat_cam(rat_cam2);
   // rat_proj_cam2.compute();

    vpgl_perspective_camera<double> pcam1, pcam2;
    pcam1 = rat_proj_cam1.get_persp_cam();
    //pcam2 = rat_proj_cam2.get_persp_cam();

    vcl_cout << pcam1 << vcl_endl;
    //vcl_cout << pcam2 << vcl_endl;

    cam1 = new vpgl_proj_camera<double> (pcam1);
    //cam2 = new vpgl_proj_camera<double> (pcam2);
  }

  // create perspective cameras out of projective ones
  /*bool ok = vpgl_perspective_decomposition<double>(proj_cam1.get_matrix(), persp_cam1); 
    if (!ok)
      vcl_cout << "Cannot create a perspective camera" << vcl_endl;
    vcl_cout << "Center " << persp_cam1.camera_center() << vcl_endl;
    vcl_cout << "Rotation " << persp_cam1.get_rotation() << vcl_endl;
    vcl_cout << "Matrix " << persp_cam1.get_matrix()<< vcl_endl;
    vpgl_calibration_matrix<double> calib = persp_cam1.get_calibration();
    vcl_cout << "Calib Matrix=  " << calib.get_matrix() << vcl_endl;
    vcl_cout << "Focal Length=  " << calib.focal_length() << vcl_endl;

    ok = vpgl_perspective_decomposition<double>(proj_cam2.get_matrix(), persp_cam2); 
    if (!ok)
      vcl_cout << "Cannot create a perspective camera" << vcl_endl;*/
}

//: Combines the the first and the last points to create a polygon
// on the screen
void poly_coin3d_manager::create_poly()
{
  
  vsol_polygon_2d_sptr poly;
  picker_->set_color(1, 0, 0);
  picker_->pick_polygon(poly);
  if(!poly)
  {vcl_cout << "Null polygon - no creation in world\n";
  return;
  }
  poly2d = poly->cast_to_polygon();
  unsigned n = poly2d->size();
  
    /*vcl_vector<vsol_point_2d_sptr> vertices;
    // erase the last vertex
    for (unsigned i=0; i<poly2d->size(); i++) {
      vertices.push_back(new vsol_point_2d(poly2d->vertex(i)->x()/0.0219, poly2d->vertex(i)->y()/0.0347));
    }
    
    vsol_polygon_2d* temp = new vsol_polygon_2d(vertices);
    poly2d = temp;*/

  // reset the cameras
 /* if (CAMERA_TYPE == 1) {
    vsol_point_2d_sptr p2 = poly2d->centroid();
    vsol_point_3d_sptr p3;
    cam_observer_->backproj_point(p2,p3);
    rat_proj_cam1->shift_camera(p3->get_p());
    cam_observer_->set_camera(&(rat_proj_cam1->get_proj_cam()));
    right_->set_camera(proj_cam1->get_matrix());
    right_->set_trans(rat_proj_cam1->norm_trans());
    left_->set_camera(*proj_cam1);
  }*/

  vsol_polygon_3d_sptr poly3d;
  cam_observer_->backproj_poly(poly2d, poly3d);
  my_polygon = new obj_observable();
  my_polygon->attach(right_);
  my_polygon->attach(left_);
  my_polygon->attach(cam_observer2_);
  my_polygon->attach(cam_observer_);
  my_polygon->set_object(poly3d);
  objects.push_back(my_polygon);
} 

void poly_coin3d_manager::extrude_poly()
{
  obj_observable* obs=0;
  unsigned face_id;

  get_selection(obs, face_id);
  if (obs) {
    obs->extrude(face_id);
    my_obj = obs;
  }
}

void poly_coin3d_manager::draw_line() 
{
  obj_observable* obs=0;
  unsigned face_id;
  get_selection(obs, face_id);
  if (obs) {
    // pick the line
    float x1, y1, x2, y2;
    picker_->set_color(0, 1, 0);
    picker_->pick_line(&x1, &y1, &x2, &y2);
    cam_observer_->set_foreground(0, 1, 0);
    cam_observer_->intersect(obs, face_id, x1, y1, x2, y2);
  }
  
}

//: Creates a polygon from the given vertex list and adds it to the mesh
dbmsh3d_face*
poly_coin3d_manager::create_face(dbmsh3d_mesh& mesh, vcl_vector<vgl_point_3d<double> > &vertex_list)
{
  for(unsigned int i=0; i<vertex_list.size(); i++) {
    double x = vertex_list[i].x();
    double y = vertex_list[i].y();
    double z = vertex_list[i].z();
    dbmsh3d_vertex* point = (dbmsh3d_vertex*) mesh._new_vertex ();
    point->get_pt().set(x, y, z);
    mesh._add_vertex(point);
  }

  // create a polygon, there is only one face
  dbmsh3d_face* face = mesh._new_face ();
  for(unsigned int i=0; i<vertex_list.size(); i++) {
    dbmsh3d_vertex* vertex = (dbmsh3d_vertex*) mesh.vertexmap(i);
    face->_ifs_add_bnd_V (vertex);
  }
  
  return face;
  //mesh._add_face(face);

} 

vpgl_perspective_camera<double> 
poly_coin3d_manager::get_camera(SoCamera* camera,
                        SbViewportRegion viewport)
{
  SoSFVec3f pos = camera->position;
  float pos_x, pos_y, pos_z;
  pos.getValue().getValue(pos_x, pos_y, pos_z);

  SbVec3f axis;
  float angle;
  float axis_x, axis_y, axis_z;
  camera->orientation.getValue(axis, angle);
  axis.getValue(axis_x, axis_y, axis_z);

  float aspect_ratio = camera->aspectRatio.getValue();
  float focal_length = camera->focalDistance.getValue();

  // find the center of the viewport as the principal point
//  viewport = cam_observer_->getViewport();
  SbVec2s window_size = viewport.getWindowSize();
  short x, y;
  window_size.getValue(x, y);
  vgl_point_2d<double> pp(x/2, y/2);
  vpgl_calibration_matrix<double> calib_matrix(focal_length, pp);

  vnl_vector_fixed<double,3> q_axis(axis_x, axis_y, axis_z);
  vnl_quaternion<double> quatern(q_axis, vnl_math::pi - angle);
  //vnl_quaternion<double> quatern(vnl_vector_fixed<double,3> (0, 0, 1), 0);

  quatern.normalize();
  vnl_matrix_fixed<double, 4, 4> R = quatern.rotation_matrix_transpose_4();
  vgl_rotation_3d<double> rot(R);
  vpgl_perspective_camera<double> vpgl_camera(calib_matrix, 
    vgl_point_3d<double> (pos_x, pos_y, pos_z), rot);

  return vpgl_camera;
}

bool poly_coin3d_manager::handle(const vgui_event &e) 
  {
    
    vgui_projection_inspector pi;
    //vcl_cout << "X=" << e.wx << " Y=" << e.wy << vcl_endl;
    if (e.type == vgui_BUTTON_DOWN && e.button == vgui_LEFT && e.modifier == vgui_SHIFT) {
      left_button_down = true;
      // take the position of the first point
      pi.window_to_image_coordinates(e.wx, e.wy, mouse_start_x, mouse_start_y);
      //vcl_cout << "1............................... "  << mouse_start_x << " " << mouse_start_x << vcl_endl;

      vcl_vector<vgui_soview*> select_list = cam_observer_->get_selected_soviews();
      //vcl_cout << select_list.size();
      if (select_list.size() > 0) {
        // TODO: if there are more than one polygon, move all of them
        //select_list[0]->print(vcl_cout);
        unsigned id = select_list[0]->get_id();
        vgui_soview2D_polygon* polygon = static_cast<vgui_soview2D_polygon *> (select_list[0]);
        cam_observer_->deselect(id);
        
        vsol_polygon_3d_sptr poly3d;
        unsigned face_id;
        observable *obs = find_polygon_in_3D(id, poly3d, face_id);
        //my_polygon_copy = new obj_observable();
        //my_polygon_copy->attach(right_);
        //my_polygon_copy->attach(left_);
        //my_polygon_copy->attach(cam_observer_);
        //my_polygon_copy->attach(cam_observer2_);
        //my_polygon_copy->set_object(poly3d);
      }
      return true;
    } 
    else if (e.type == vgui_MOTION && left_button_down == true && e.modifier == vgui_SHIFT) {
      // update these only if there is motion event
      if (my_polygon != 0) {
        float wx = e.wx;
        float wy = e.wy;     
        pi.window_to_image_coordinates(e.wx, e.wy, cur_pointx, cur_pointy);
        double diff = -1 * (cur_pointy - mouse_start_y)*1.0;
        //vcl_cout << diff << vcl_endl;
        vsol_polygon_3d_sptr poly3d;
        cam_observer_->backproj_poly(poly2d, poly3d, diff);
        my_polygon->move(poly3d);
      }
      return true;
    }
    else if (e.type == vgui_BUTTON_UP && e.button == vgui_LEFT && e.modifier == vgui_SHIFT){
      left_button_down = false;
      return true;
      /*if (my_polygon_copy) {
        delete_observable(my_polygon);
        my_polygon = my_polygon_copy;
        //delete my_polygon_copy;
        my_polygon_copy = 0;
        objects.push_back(my_polygon);
        my_polygon_copy->attach(right_);
        my_polygon_copy->attach(left_);
        my_polygon_copy->attach(cam_observer_);
        my_polygon_copy->attach(cam_observer2_);
      }*/
    } 
    else if (e.type == vgui_BUTTON_DOWN && e.button == vgui_MIDDLE && e.modifier == vgui_SHIFT) {
      middle_button_down = true;
      pi.window_to_image_coordinates(e.wx, e.wy, mouse_start_x, mouse_start_y);
      return true;
    }
    else if (e.type == vgui_MOTION && middle_button_down == true) {
      // update these only if there is motion event
      float wx = e.wx;
      float wy = e.wy;
      
      pi.window_to_image_coordinates(e.wx, e.wy, cur_pointx, cur_pointy);
      double diff = (mouse_start_y - cur_pointy)*1.0;
      //vcl_cout << mouse_start_y  << "-" << cur_pointy << "=" << diff << vcl_endl;
      mouse_start_y = cur_pointy;
      if (my_obj != 0)
        my_obj->move_extr_face(diff);
      return true;
    } 
    else if (e.type == vgui_BUTTON_UP && e.button == vgui_MIDDLE && e.modifier == vgui_SHIFT){
      middle_button_down = false;
      return true;
    }
 
    // We are not interested in other events, so pass event to base class:
    return vgui_wrapper_tableau::handle(e);  
  }

obj_observable* poly_coin3d_manager::find_polygon_in_3D(unsigned id, 
                                                 vsol_polygon_3d_sptr& poly,
                                                 unsigned& index)
{ 
  vcl_list<obj_observable*>::iterator it = objects.begin();
  while (it != objects.end()) {
    obj_observable* o = *it;
    if (o->is_poly_in(id, index)) {
        poly = o->extract_face(index);
        return o;
    }
    it++;
  }  

  // not found, return NULL
  return 0;
}

void poly_coin3d_manager::delete_observable(observable* obs)
{
  vcl_list<obj_observable*>::iterator it = objects.begin();
  while (it != objects.end()) {
    if (*it == obs) {
      delete (*it);
      objects.erase(it);
      return;
    }
    it++;
  }
}

void poly_coin3d_manager::get_selection(obj_observable* &obs, unsigned &face_id)
{

  vcl_vector<vgui_soview*> select_list = cam_observer_->get_selected_soviews();
  if (select_list.size() == 0 || select_list.size() > 1) {
    vcl_cerr << "Select only ONE face" << vcl_endl;
  } else {
    unsigned sel_id = select_list[0]->get_id();
    vgui_soview2D_polygon* polygon = static_cast<vgui_soview2D_polygon *> (select_list[0]);
    cam_observer_->deselect_all(); 

    // find the 3D polygon with this ID
    vsol_polygon_3d_sptr poly3d;
    obs = find_polygon_in_3D(sel_id, poly3d, face_id);
  }
}

void poly_coin3d_manager::deselect_all()
{
  cam_observer_->deselect_all();
  cam_observer2_->deselect_all();
}

void poly_coin3d_manager::set_selected_vertex()
{
  //Now find out which polygon is selected (if any)
  vcl_vector<vgui_soview*> select_list = cam_observer_->get_selected_soviews();
  //Check if there is at least one selected polygon
  if (select_list.size() > 0) {

      unsigned list_index = 0;
      bool found = false;
      for(vcl_vector<vgui_soview*>::iterator sit = select_list.begin();
          sit != select_list.end(); ++sit,++list_index)
        if((*sit)->type_name()!= "vgui_soview2D_polygon")
          continue;
  else{
          found = true;
    break;
  }
      if(!found)
        {
          selected_vertex_ = (vsol_point_3d*)0;
          return;
        }
      unsigned id = select_list[list_index]->get_id();          
    //Here we get the first polygon, no matter how many are selected

    vgui_soview2D_polygon* polygon = static_cast<vgui_soview2D_polygon *> (select_list[list_index]);
 if(!polygon)
 {
      vcl_cout << "Is a face selected?\n";
  return;
 }
    //deselect the polygon since we have it 
    vsol_polygon_3d_sptr poly3d;
    unsigned face_id;
        //Get the 3d polygon which has been projected onto the construction
        //plane 
    observable *obs = find_polygon_in_3D(id, poly3d, face_id);
 if(!poly3d)
 {
      vcl_cout << "Is a face selected?\n";
  return;
 }
    unsigned vindex =cam_observer_->get_selected_3d_vertex_index(id);
    if(vindex<poly3d->size())
      selected_vertex_ = poly3d->vertex(vindex);
    else
      selected_vertex_ = (vsol_point_3d*)0;
  }
  else
    selected_vertex_ = (vsol_point_3d*)0;
}
void poly_coin3d_manager::print_selected_vertex()
{
  this->set_selected_vertex();
  if(!selected_vertex_)
    return;
  vcl_cout.precision(10);
    vcl_cout << "Lat: " << selected_vertex_->y() << ' '
             << "Lon: " << selected_vertex_->x() << ' '
             << "Elv: " << selected_vertex_->z() << '\n';
}
void poly_coin3d_manager::clear_poly()
{
  obj_observable* obs=0;
  unsigned face_id;
  get_selection(obs, face_id);
  if (obs) {
    delete_observable(obs);
  }
}

void poly_coin3d_manager::clear_all()
{
  vcl_list<obj_observable*>::iterator it = objects.begin();
  while (it != objects.end()) {
    delete (*it);
    it++;
  }
  objects.clear();
}
