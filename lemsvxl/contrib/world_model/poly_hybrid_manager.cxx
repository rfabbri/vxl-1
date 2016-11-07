#include "poly_hybrid_manager.h"

#include <vgui/vgui_grid_tableau.h> 
#include <vgui/vgui_image_tableau.h> 
#include <vgui/vgui_viewer2D_tableau.h> 
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_range_map_params.h>

#include <vnl/vnl_quaternion.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_math.h>

#include <vgl/vgl_homg_plane_3d.h> 
#include <vgl/vgl_homg_line_3d_2_points.h> 
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_clip.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_intersection.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>

#include <vil/vil_load.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_3d_sptr.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_box_2d.h>

#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/vis/dbmsh3d_vis_backpt.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>
#include <dbmsh3d/vis/dbmsh3d_vis_face.h>

#include <bgeo/bgeo_lvcs.h>

poly_hybrid_manager* poly_hybrid_manager::instance_ = 0;

poly_hybrid_manager* poly_hybrid_manager::instance() {
  if (!instance_) 
    instance_ = new poly_hybrid_manager();
  return poly_hybrid_manager::instance_;
   
}

poly_hybrid_manager::poly_hybrid_manager(void)
: my_obj(0), my_polygon(0), my_polygon_copy(0)
{
  //dbmsh3dvis_init_vispt_OpenInventor_classes ();
}

poly_hybrid_manager::~poly_hybrid_manager(void)
{
  
  if (my_polygon != 0)
    delete my_polygon;
  if (my_polygon_copy != 0)
    delete my_polygon_copy;
  vcl_list<obj_observable*>::iterator iter=objects.begin();
  while (iter != objects.end()) {
    delete (*iter);
    iter++;
  }
  delete observer_right_;
  delete observer_left_;
}

void poly_hybrid_manager::setup_scene (vgui_grid_tableau_sptr grid)
{
  grid_tableau_ = grid;

  grid_tableau_->add_at(this, 0, 0);



#define RATIONAL_CAM_TEST
#ifdef RATIONAL_CAM_TEST
 
  // create a LIDAR observer on the right
  //create a vgui range paramaters to be able to see float values
  float min_val = 20;
  float max_val = 100;
  float gamma = 1.0;
  bool invert = false;
  bool use_glPixelMap = false;
  bool cache_buffer = true;

  vgui_range_map_params_sptr params = new vgui_range_map_params(min_val, max_val, gamma,
                                invert, use_glPixelMap, cache_buffer);
 

  vcl_string image_filename = "C:\\test_images\\BaghdadLIDAR\\dem_1m_a1_baghdad_tile39.tif";
  
  vil_image_resource_sptr res = vil_load_image_resource(image_filename.c_str());
  img_right_ = bgui_image_tableau_new(res, params);
  observer_right_ = new poly_lidar_observer(img_right_);
  right_ = bgui_picker_tableau_new(observer_right_);
  vgui_viewer2D_tableau_new r(right_);
  grid_tableau_->add_at(r, 2, 0);

  // load left image
  

  bgui_image_tableau_sptr img_left = bgui_image_tableau_new("C:\\test_images\\providence1\\06MAR27155829-S2AS-005580823020_01_P001.TIF");

  observer_left_ = new poly_cam_observer(img_left);
  left_ = bgui_picker_tableau_new(observer_left_);
  vgui_viewer2D_tableau_new l(left_);
  grid_tableau_->add_at(l, 1, 0);

  vcl_string cam_path("C:\\test_images\\providence2\\06MAY28155032-S2AS-005580823010_01_P001.RPB");
  vpgl_rational_camera<double> cam = read_rational_camera(cam_path);
  vcl_cout << "right camera offset = <"<<cam.offset(cam.X_INDX)<<", "<<cam.offset(cam.Y_INDX)<<", "<<cam.offset(cam.Z_INDX)<<"> "<<vcl_endl;
  observer_left_->set_camera(cam.clone());
  vgl_point_3d<double> cam_center;
  //observer_left_->camera_center(cam_center);
  vcl_cout << "left camera center = "<<cam_center<<vcl_endl;
  
#else
  //this->add_child(vgui_viewer2D_tableau_new(vgui_image_tableau_new("C:\\test_images\\mesh\\pollard\\shortseq2\\reg00010.tif")));
  vgui_image_tableau_sptr img_right = vgui_image_tableau_new("C:\\test_images\\mesh\\pollard\\shortseq2\\reg00010.tif");
  
  observer_right_ = new poly_cam_observer(img_right);
  vgui_viewer2D_tableau_new r(static_cast<vgui_easy2D_tableau* const&> (observer_right_));
  grid_tableau_->add_at(r, 2, 0);
  vcl_string cam_path("C:\\test_images\\mesh\\pollard\\cameras\\camera10.txt");
  vpgl_proj_camera<double> cam = read_projective_camera(cam_path);
  //vcl_cout << cam << vcl_endl;
  observer_right_->set_camera(cam.clone());

  vgui_image_tableau_sptr img_left = vgui_image_tableau_new("C:\\test_images\\mesh\\pollard\\shortseq2\\reg00019.tif");
  observer_left_ = new poly_cam_observer(img_left);
  left_ = bgui_picker_tableau_new(observer_left_);
  vgui_viewer2D_tableau_new l(left_);
  grid_tableau_->add_at(l, 1, 0);

  vcl_string cam_path2("C:\\test_images\\mesh\\pollard\\cameras\\camera19.txt");
  vpgl_proj_camera<double> cam2 = read_projective_camera(cam_path2);

  observer_left_->set_camera(cam2.clone());
#endif
}

void poly_hybrid_manager::load_left_image()
{
  vcl_string filename = select_file();
  if (!filename.empty())
  {
    vil_image_view_base_sptr img = vil_load( filename.data() );
    observer_left_->add_image(0, 0, *img);
  }
} 
  
void poly_hybrid_manager::load_left_camera()
{
  vpgl_camera<double> *cam = select_camera();
  observer_left_->set_camera(cam);
}
    
void poly_hybrid_manager::load_right_image()
{
  vcl_string filename = select_file();
  if (!filename.empty())
  {
    vil_image_view_base_sptr img = vil_load( filename.data() );
    observer_right_->add_image(0, 0, *img);
  }
}  
  
//sets the vgui range params from a dialog
void poly_hybrid_manager::set_range_params()
{
  vgui_range_map_params_sptr rmps = img_right_->map_params();
  
  unsigned nc = rmps->n_components_;
  static double min = static_cast<double>(rmps->min_L_),
    max = static_cast<double>(rmps->max_L_);
  static float gamma = rmps->gamma_L_;
  static bool invert = rmps->invert_;
  static bool gl_map = rmps->use_glPixelMap_;
  static bool cache = rmps->cache_mapped_pix_;
  if(nc==3)
    {
      min = static_cast<double>(rmps->min_R_);
      max = static_cast<double>(rmps->max_R_);
      gamma = rmps->gamma_R_;
    }
  vgui_dialog range_dlg("Set Range Map Params");
  range_dlg.field("Range min:", min);
  range_dlg.field("Range max:", max);
  range_dlg.field("Gamma:", gamma);
  range_dlg.checkbox("Invert:", invert);
  range_dlg.checkbox("Use GL Mapping", gl_map);
  range_dlg.checkbox("Cache Pixels", cache);
  if (!range_dlg.ask())
    return;
  if(nc==1)
    rmps= new vgui_range_map_params(min, max, gamma, invert,
                                    gl_map, cache);
  else if (nc == 3)
    rmps = new vgui_range_map_params(min, max, min, max, min, max,
                                     gamma, gamma, gamma, invert,
                                     gl_map, cache);
  else
    rmps = 0;
  img_right_->set_mapping(rmps);
}


void poly_hybrid_manager::load_images_and_cameras()
{
  vcl_string filename = select_file();
  if (filename.empty())
    {
      vcl_cout << "Bad filename\n";
      return;
    }
  vcl_ifstream is(filename.data());
  if(!is.is_open())
    {
      vcl_cout << "Can't open file\n";
      return;
    }
  vcl_string name;
  is >> name;
  if(name != "LEFT:")
    {
      vcl_cout << "Bad file parse\n";
      return;
    }
  is >> name;
  if(name != "IMAGE:")
    {
      vcl_cout << "Bad file parse\n";
      return;
    }
  vcl_string left_image_path;
  is >> left_image_path;
  is >> name;
  if(name != "CAMERA_TYPE:")
    {
      vcl_cout << "Bad file parse\n";
      return;
    }
  vcl_string left_camera_type;
  is >> left_camera_type;
  is >> name;
  if(name != "CAMERA_PATH:")
    {
      vcl_cout << "Bad file parse\n";
      return;
    }
  vcl_string left_camera_path;
  is >> left_camera_path;
  is >> name;
  if(name != "RIGHT:")
    {
      vcl_cout << "Bad file parse\n";
      return;
    }
  is >> name;
  if(name != "IMAGE:")
    {
      vcl_cout << "Bad file parse\n";
      return;
    }
  vcl_string right_image_path;
  is >> right_image_path;
  is >> name;
  if(name != "CAMERA_TYPE:")
    {
      vcl_cout << "Bad file parse\n";
      return;
    }
  vcl_string right_camera_type;
  is >> right_camera_type;
  is >> name;
  if(name != "CAMERA_PATH:")
    {
      vcl_cout << "Bad file parse\n";
      return;
    }
  vcl_string right_camera_path;
  is >> right_camera_path;
  //parsing done
  //vil_image limg = vil_load( left_image_path.c_str() );
  vil_image_resource_sptr res = vil_load_image_resource(left_image_path.c_str());
  //vcl_cout << limg.cols() << "  " << limg.rows();
  observer_left_->add_image(0, 0, *(res->get_view()));
  vpgl_camera<double> *left_camera;
  if(left_camera_type=="projective")
    left_camera = (read_projective_camera(left_camera_path)).clone();
  else if( left_camera_type=="rational")
    left_camera = (read_rational_camera(left_camera_path)).clone();
  else
    {
      vcl_cout << "Unsupported camera type" << left_camera_type << '\n';
      return;
    }
  observer_left_->set_camera(left_camera);
  res = vil_load_image_resource( right_image_path.c_str() );
  //vcl_cout << limg.cols() << "  " << limg.rows();
  observer_right_->add_image(0, 0, *(res->get_view()));
  /*vpgl_camera<double> *right_camera;
  if(right_camera_type=="projective")
    right_camera = (read_projective_camera(right_camera_path)).clone();
  else if( right_camera_type=="rational")
    right_camera = (read_rational_camera(right_camera_path)).clone();
  else
    {
      vcl_cout << "Unsupported camera type" << right_camera_type << '\n';
      return;
    }
  observer_right_->set_camera(right_camera);*/
}
void poly_hybrid_manager::select_proj_plane()
{
  obj_observable* obs=0;
  unsigned face_id;
  get_selection(obs, face_id);
  if (obs) {
    vgl_homg_plane_3d<double> plane = obs->get_plane_of_face(face_id);
    observer_left_->set_proj_plane(plane);
  } else {
    vcl_cerr << "Selected plain is not valid" << vcl_endl;
  }
}

void poly_hybrid_manager::define_proj_plane()
{
  // pick the ground truth line
  float x1, y1, x2, y2;
  left_->pick_line(&x1, &y1, &x2, &y2);
  vcl_cout << "(" << x1 << "," << y1 << ")" << "(" << x2 << "," << y2 << ")" << vcl_endl;
  //observer_left_->add_line(x1, y1, x2, y2);
  observer_left_->set_ground_plane(x1, y1, x2, y2);

  // pick the orthogonal line
  /*float x3, y3, x4, y4;
  left_->pick_line(&x3, &y3, &x4, &y4);
  vcl_cout << "(" << x3 << "," << y3 << ")" << "(" << x4 << "," << y4 << ")" << vcl_endl;
  observer_left_->add_line(x3,y3,x4,y4);*/
}

void poly_hybrid_manager::define_ground_plane()
{
  // pick the ground truth line
  float x1, y1, x2, y2;
  right_->pick_line(&x1, &y1, &x2, &y2);
  vcl_cout << "(" << x1 << "," << y1 << ")" << "(" << x2 << "," << y2 << ")" << vcl_endl;
  observer_right_->add_line(x1, y1, x2, y2);
  observer_right_->set_ground_plane(x1, y1, x2, y2);
}

void poly_hybrid_manager::define_xy_proj_plane()
{
  static double z = 0;
  vgui_dialog zval("Define XY plane");
  zval.field("Elevation (meters)", z);
  if (!zval.ask())
    return;
  vgl_homg_plane_3d<double> plane(0,0,1,-z);
  observer_left_->set_proj_plane(plane);
}

void poly_hybrid_manager::define_yz_proj_plane()
{
  static double x = 0;
  vgui_dialog xval("Define YZ plane");
  xval.field("X (meters)", x);
  if (!xval.ask())
    return;
  vgl_homg_plane_3d<double> plane(1,0,0,-x);
  observer_left_->set_proj_plane(plane);
}

void poly_hybrid_manager::define_xz_proj_plane()
{
  static double y= 0;
  vgui_dialog yval("Define XZ plane");
  yval.field("Y (meters)", y);
  if (!yval.ask())
    return;
  vgl_homg_plane_3d<double> plane(0,1,0,-y);
  observer_left_->set_proj_plane(plane);
}

void poly_hybrid_manager::save()
{
  vgui_dialog params("File Save");
  vcl_string ext, file, empty="";

  params.file ("Save...", ext, file);  
  bool use_lvcs = false;
  params.checkbox("use lvcs",use_lvcs);
  if (!params.ask())
    return;

  if (file == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify a filename (prefix)." );
    error.ask();
    return;
  }

  vcl_list<obj_observable*>::iterator it = objects.begin();
  while (it != objects.end()) {
    obj_observable* o = *it;
    dbmsh3d_mesh_mc* mesh = o->get_object();
    save_mesh(mesh,file.data(),use_lvcs);
    it++;
  }
}

void poly_hybrid_manager::save_all()
{
  vgui_dialog params("File Save");
  vcl_string ext, master_filename, empty="";

  params.file ("Save...", ext, master_filename);  
  bool use_lvcs = false;
  params.checkbox("use lvcs",use_lvcs);
  if (!params.ask())
    return;

  if (master_filename == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify a filename (prefix)." );
    error.ask();
    return;
  }

  vcl_ofstream list_out(master_filename.data());
  if (!list_out.good()) {
    vcl_cerr << "error opening file "<<master_filename<<vcl_endl;
    return;
  }

  vcl_list<obj_observable*>::iterator it = objects.begin();
  int mesh_idx = 0;
  while (it != objects.end()) {
    vcl_ostringstream meshname;
    vcl_ostringstream fullpath;
    meshname << "mesh" << mesh_idx <<".ply2";
    fullpath << master_filename << "." << meshname.str();

    list_out << meshname.str() << vcl_endl;
    obj_observable* o = *it;
    dbmsh3d_mesh_mc* mesh = o->get_object();
    save_mesh(mesh,fullpath.str().data(),use_lvcs);
    it++;
    mesh_idx++;
  }
}

void poly_hybrid_manager::save_lvcs()
{
  vcl_string filename = select_file();
  // just save origin for now
  vcl_ofstream os(filename.data());
  double lat,lon,elev;
  lvcs_->get_origin(lat,lon,elev);
  os.precision(12);
  os << lat << " "<< lon << " " << elev << vcl_endl;

  return;
}

void poly_hybrid_manager::load_lvcs()
{
  vcl_string filename = select_file();
  // just load origin for now
  vcl_ifstream is(filename.data());
  double lat, lon, elev;
  is >> lat;
  is >> lon;
  is >> elev;

  lvcs_ = new bgeo_lvcs(lat,lon,elev,bgeo_lvcs::wgs84,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
  vcl_cout << "loaded lvcs with origin "<<lat<<", "<<lon<<", "<<elev<<vcl_endl;

  return;
}

void poly_hybrid_manager::convert_file_to_lvcs()
{
  if (!lvcs_) {
    vcl_cerr << "error: lvcs is not defined!"<<vcl_endl;
    return;
  }
  // expects simple text file with each line being of the form "lat lon z"
  vcl_string filename_in = select_file();
  vcl_string filename_out = filename_in + ".lvcs";
  vcl_ifstream is(filename_in.data());
  vcl_ofstream os(filename_out.data());
  
  double lat,lon,elev;
  double x,y,z;

  while (!is.eof()) {
    is >> lat;
    is >> lon;
    is >> elev;
    lvcs_->global_to_local(lon,lat,elev,bgeo_lvcs::wgs84,x,y,z,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
    os << x <<" "<<y<<" "<<z<<vcl_endl;
  }

  return;
}

void poly_hybrid_manager::save_mesh(dbmsh3d_mesh_mc* mesh, const char* filename, bool use_lvcs)
{
  mesh->build_IFS_mesh();
  //dbmsh3d_mesh mesh2(*mesh);
  dbmsh3d_mesh_mc* mesh2 = mesh->clone();
  if (use_lvcs) {
    if (!lvcs_){
      vcl_cerr << "error: lvcs == null" <<vcl_endl;
      return;
    }
    vcl_map<int, dbmsh3d_vertex*>::iterator it = mesh2->vertexmap().begin();
    for (; it != mesh2->vertexmap().end(); it++) {
      dbmsh3d_vertex* V = (dbmsh3d_vertex*) (*it).second;
      double x=0,y=0,z=0;
      lvcs_->global_to_local(V->pt().x(),V->pt().y(),V->pt().z(),bgeo_lvcs::wgs84,x,y,z,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
      vgl_point_3d<double> new_pt(x,y,z);
      vcl_cout << "converted global <"<<V->pt().x() <<", "<< V->pt().y() <<", "<< V->pt().z() <<"> to <" <<x<< ", "<<y<<" ,"<<z<<"> "<<vcl_endl;
      V->set_pt(new_pt);
    }
  }
  mesh2->orient_face_normals();
  mesh2->build_IFS_mesh();
  dbmsh3d_save_ply2(mesh2, filename);
  return;
}


void poly_hybrid_manager::load_mesh_single()
{
  vcl_string filename = select_file();
  load_mesh(filename);
  return;
}

void poly_hybrid_manager::load_mesh_multiple()
{
  // read txt file
  vcl_string master_filename = select_file();
  vcl_ifstream file_inp(master_filename.data());
  if (!file_inp.good()) {
    vcl_cerr << "error opening file "<< master_filename <<vcl_endl;
    return;
  }

  while(!file_inp.eof()){
    vcl_ostringstream fullpath;
    vcl_string mesh_fname;
    file_inp >> mesh_fname;
    if (!mesh_fname.empty() && (mesh_fname[0] != '#')) {
      fullpath << master_filename << "." << mesh_fname;
      load_mesh(fullpath.str());
    }
  }
  file_inp.close();

  return;
}

void poly_hybrid_manager::load_mesh(vcl_string filename)
{
  dbmsh3d_mesh_mc mesh_mc;

  if (!dbmsh3d_load_ply2(&mesh_mc,filename.data())) {
    vcl_cerr << "Error loading mesh "<<filename<< vcl_endl;
    return;
  }

  // build half-edge structure
  mesh_mc.IFS_to_MHE();

  obj_observable* obj = new obj_observable();
  obj->attach(observer_right_);
  obj->attach(observer_left_);
  obj->set_object(mesh_mc.clone());
  objects.push_back(obj);

  return;
}

//: Define a local vertical coordinate system by choosing a single point as
// the origin.  Elevation is set to the height of the current projection plane. 
void poly_hybrid_manager::define_lvcs()
{
  float x1,y1;
  left_->pick_point(&x1,&y1);
  vsol_point_2d_sptr img_point = new vsol_point_2d(x1,y1);

  vcl_vector<vsol_point_2d_sptr> points2d;
  vcl_vector<vsol_point_3d_sptr> points3d;
  // push img_point 3x to create degenerate polygon
  for (int i=0;i<3;i++) {
    points2d.push_back(img_point);
    points3d.push_back(new vsol_point_3d(0,0,0));
  }
  vsol_polygon_2d_sptr origin_poly2d = new vsol_polygon_2d(points2d);
  vsol_polygon_3d_sptr origin_poly3d = new vsol_polygon_3d(points3d);

  observer_left_->backproj_poly(origin_poly2d,origin_poly3d);

  // note constructor takes lat, long (as opposed to long, lat) so switch x and y
  lvcs_ = new bgeo_lvcs(origin_poly3d->vertex(0)->y(),origin_poly3d->vertex(0)->x(),origin_poly3d->vertex(0)->z(),
                        bgeo_lvcs::wgs84,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
  observer_right_->set_lvcs(origin_poly3d->vertex(0)->y(),origin_poly3d->vertex(0)->x(),origin_poly3d->vertex(0)->z());
  vcl_cout << "defining lvcs with origin = <"<<origin_poly3d->vertex(0)->x() <<", "<<origin_poly3d->vertex(0)->y() <<", "<<origin_poly3d->vertex(0)->z() <<">"<<vcl_endl;
}

 //: Define a local vertical coordinate system by choosing a single point as
// the origin.  Elevation is set to the height of the current projection plane. 
void poly_hybrid_manager::define_lidar_lvcs()
{
  float x1,y1;
  right_->pick_point(&x1,&y1);
  vsol_point_2d_sptr img_point = new vsol_point_2d(x1,y1);

  vcl_vector<vsol_point_2d_sptr> points2d;
  vcl_vector<vsol_point_3d_sptr> points3d;
  // push img_point 3x to create degenerate polygon
  for (int i=0;i<3;i++) {
    points2d.push_back(img_point);
    points3d.push_back(new vsol_point_3d(0,0,0));
  }
  vsol_polygon_2d_sptr origin_poly2d = new vsol_polygon_2d(points2d);
  vsol_polygon_3d_sptr origin_poly3d = new vsol_polygon_3d(points3d);

  observer_right_->backproj_poly(origin_poly2d,origin_poly3d);

  // note constructor takes lat, long (as opposed to long, lat) so switch x and y
  lvcs_ = new bgeo_lvcs(origin_poly3d->vertex(0)->y(),origin_poly3d->vertex(0)->x(),origin_poly3d->vertex(0)->z(),
                        bgeo_lvcs::wgs84,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
  observer_right_->set_lvcs(origin_poly3d->vertex(0)->y(),origin_poly3d->vertex(0)->x(),origin_poly3d->vertex(0)->z());
  vcl_cout << "defining lvcs with origin = <"<<origin_poly3d->vertex(0)->x() <<", "<<origin_poly3d->vertex(0)->y() <<", "<<origin_poly3d->vertex(0)->z() <<">"<<vcl_endl;
} 
//: Combines the the first and the last points to create a polygon
// on the screen
void poly_hybrid_manager::create_poly()
{
  vsol_polygon_2d_sptr poly;
  left_->set_color(1, 0, 0);
  left_->pick_polygon(poly);
  if(!poly)
  {vcl_cout << "Null polygon - no creation in world\n";
  return;
  }
  poly2d = poly->cast_to_polygon();
  unsigned n = poly2d->size();
  // make sure that the last two vertices of the polygon are not the same
  if (*(poly2d->vertex(n-1)) == *(poly2d->vertex(n-2))) {
    vcl_vector<vsol_point_2d_sptr> vertices;
    // erase the last vertex
    for (unsigned i=0; i<poly2d->size()-1; i++) {
      vertices.push_back(poly2d->vertex(i));
    }
    
    vsol_polygon_2d* temp = new vsol_polygon_2d(vertices);
    //delete poly2d->;
    poly2d = temp;
  }
  vsol_polygon_3d_sptr poly3d;
  observer_left_->backproj_poly(poly2d, poly3d);
  my_polygon = new obj_observable();
  my_polygon->attach(observer_right_);
  my_polygon->attach(observer_left_);
  my_polygon->set_object(poly3d);
  objects.push_back(my_polygon);
  
} 

// create Polygon on LIDAR image
void poly_hybrid_manager::create_lidar_poly()
{
  vsol_polygon_2d_sptr poly;
  right_->set_color(1, 0, 0);
  right_->pick_polygon(poly);

  poly2d = poly->cast_to_polygon();
  unsigned n = poly2d->size();
  // make sure that the last two vertices of the polygon are not the same
  if (*(poly2d->vertex(n-1)) == *(poly2d->vertex(n-2))) {
    vcl_vector<vsol_point_2d_sptr> vertices;
    // erase the last vertex
    for (unsigned i=0; i<poly2d->size()-1; i++) {
      vertices.push_back(poly2d->vertex(i));
    }
    
    vsol_polygon_2d* temp = new vsol_polygon_2d(vertices);
    //delete poly2d->;
    poly2d = temp;
  }
  vsol_polygon_3d_sptr poly3d;
  observer_right_->backproj_poly(poly2d, poly3d);
  float elev = observer_right_->ground_elev();
  my_polygon = new obj_observable();
  my_polygon->attach(observer_right_);
  my_polygon->set_object(poly3d, elev);
  objects.push_back(my_polygon);
  
} 

void poly_hybrid_manager::extrude_poly()
{
  obj_observable* obs=0;
  unsigned face_id;

  get_selection(obs, face_id);
  if (obs) {
    obs->extrude(face_id);
    my_obj = obs;
  }
}

void poly_hybrid_manager::draw_line() 
{
  obj_observable* obs=0;
  unsigned face_id;
  get_selection(obs, face_id);
  if (obs) {
    // pick the line
    float x1, y1, x2, y2;
    left_->set_color(0, 1, 0);
    left_->pick_line(&x1, &y1, &x2, &y2);
    observer_left_->set_foreground(0, 1, 0);
    observer_left_->intersect(obs, face_id, x1, y1, x2, y2);
  }
  
}

void poly_hybrid_manager::label_roof() 
{
  obj_observable* obs=0;
  unsigned face_id;
  get_selection(obs, face_id);
  obs->label_roof(face_id);
}

void poly_hybrid_manager::label_wall() 
{
  obj_observable* obs=0;
  unsigned face_id;
  get_selection(obs, face_id);
  obs->label_wall(face_id);
}
void poly_hybrid_manager::create_inner_face() 
{
  // first get the selection face
  obj_observable* obs=0;
  unsigned face_id;
  get_selection(obs, face_id);
  if (obs) {
    vsol_polygon_2d_sptr poly;
    left_->set_color(1, 0, 0);
    left_->pick_polygon(poly);

    poly2d = poly->cast_to_polygon();
    observer_left_->connect_inner_face(obs, face_id, poly2d);
  }
}

void poly_hybrid_manager::create_interior() 
{
  obj_observable* obs=0;
  unsigned face_id;
  get_selection(obs, face_id);
  obs->create_interior();
}
void poly_hybrid_manager::get_selection(obj_observable* &obs, unsigned &face_id)
{

  vcl_vector<vgui_soview*> select_list = observer_left_->get_selected_soviews();
  if (select_list.size() == 0 || select_list.size() > 1) {
    vcl_cerr << "Select only ONE face" << vcl_endl;
  } else {
    unsigned sel_id = select_list[0]->get_id();
    vgui_soview2D_polygon* polygon = static_cast<vgui_soview2D_polygon *> (select_list[0]);
    observer_left_->deselect_all(); //>deselect(sel_id);

    // find the 3D polygon with this ID
    vsol_polygon_3d_sptr poly3d;
    obs = find_polygon_in_3D(sel_id, poly3d, face_id);
  }
}

void poly_hybrid_manager::clear_poly()
{
}

bool poly_hybrid_manager::handle(const vgui_event &e) 
  {
    
    vgui_projection_inspector pi;
    //vcl_cout << "X=" << e.wx << " Y=" << e.wy << vcl_endl;
    if (e.type == vgui_BUTTON_DOWN && e.button == vgui_LEFT && e.modifier == 0) {
      left_button_down = true;
      // take the position of the first point
      pi.window_to_image_coordinates(e.wx, e.wy, mouse_start_x, mouse_start_y);
      //vcl_cout << "1............................... "  << mouse_start_x << " " << mouse_start_x << vcl_endl;

      vcl_vector<vgui_soview*> select_list = observer_left_->get_selected_soviews();
      //vcl_cout << select_list.size();
      if (select_list.size() > 0) {
        // TODO: if there are more than one polygon, move all of them
        //select_list[0]->print(vcl_cout);
        unsigned id = select_list[0]->get_id();
        vgui_soview2D_polygon* polygon = static_cast<vgui_soview2D_polygon *> (select_list[0]);
        observer_left_->deselect(id);
        
        vsol_polygon_3d_sptr poly3d;
        unsigned face_id;
        observable *obs = find_polygon_in_3D(id, poly3d, face_id);
        my_polygon_copy = new obj_observable();
        my_polygon_copy->attach(observer_right_);
        my_polygon_copy->attach(observer_left_);
        my_polygon_copy->set_object(poly3d);
      }
    } 
    else if (e.type == vgui_MOTION && left_button_down == true) {
      //vcl_cout << "3..............................." << vcl_endl;
      // update these only if there is motion event
      if (my_polygon_copy != 0) {
        float wx = e.wx;
        float wy = e.wy;     
        pi.window_to_image_coordinates(e.wx, e.wy, cur_pointx, cur_pointy);
        double diff = -1 * (cur_pointy - mouse_start_y)/10.0;
        vcl_cout << diff << vcl_endl;
        vsol_polygon_3d_sptr poly3d;
        observer_left_->backproj_poly(poly2d, poly3d, diff);
        my_polygon_copy->move(poly3d);
      }
    }
    else if (e.type == vgui_BUTTON_UP && e.button == vgui_LEFT && e.modifier == 0){
      left_button_down = false;
      if (my_polygon_copy) {
        delete_observable(my_polygon);
        my_polygon = my_polygon_copy;
        my_polygon_copy = 0;
        objects.push_back(my_polygon);
        my_polygon->attach(observer_left_);
      }
    } 
    else if (e.type == vgui_BUTTON_DOWN && e.button == vgui_MIDDLE && e.modifier == 0) {
      middle_button_down = true;
      pi.window_to_image_coordinates(e.wx, e.wy, mouse_start_x, mouse_start_y);
    }
    else if (e.type == vgui_MOTION && middle_button_down == true) {
      // update these only if there is motion event
      float wx = e.wx;
      float wy = e.wy;
      
      pi.window_to_image_coordinates(e.wx, e.wy, cur_pointx, cur_pointy);
      double diff = (mouse_start_y - cur_pointy)/10.0;
      vcl_cout << mouse_start_y  << "-" << cur_pointy << "=" << diff << vcl_endl;
      mouse_start_y = cur_pointy;
      if (my_obj != 0)
        my_obj->move_extr_face(diff);
    } 
    else if (e.type == vgui_BUTTON_UP && e.button == vgui_MIDDLE && e.modifier == 0){
      //vcl_cout << "2..............................." << vcl_endl;
      middle_button_down = false;
    }
 
    // We are not interested in other events, so pass event to base class:
    return true; //vgui_tableau::handle(e);  
  }


//////////////////////////////////////////////////////////////////////////////////
// Private Methods
vcl_string poly_hybrid_manager::select_file()
{
  vgui_dialog params ("File Open");
  vcl_string ext, file, empty="";

  params.file ("Open...", ext, file);  
  if (!params.ask())
    return empty;

  if (file == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify a input file (prefix)." );
    error.ask();
    return empty;
  }
  return file;
}

vpgl_camera<double>* poly_hybrid_manager::select_camera()
{
  vgui_dialog params ("Camera File Open");
  vcl_string ext, file, empty="";
  vcl_vector<vcl_string> camera_types;
  int camera_type = 0;

  vpgl_camera<double> *camera = (vpgl_camera<double>*)0;

  camera_types.push_back("projective");
  camera_types.push_back("rational");

  params.choice("Camera Type", camera_types, camera_type);
  params.file ("Open...", ext, file);  
  if (!params.ask())
    return camera;

  if (file == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify a input file (prefix)." );
    error.ask();
    return camera;
  }
  // load camera
  switch(camera_type) {
  case 0:
    // projective
    camera = (read_projective_camera(file)).clone();
    break;
  case 1:
    // rational
    camera = (read_rational_camera(file)).clone();
    break;
  default:
    vcl_cout << "Error: unknown camera type "<<camera_type<< vcl_endl;
  }
  return camera;
}

void poly_hybrid_manager::read_world_points(vcl_string fname)
{
  vcl_ifstream points_file(fname.data());
  int x, y, z;
  while (!points_file.eof()) {
    points_file >> x;
    points_file >> y;
    points_file >> z;
    vcl_cout << x << " " << y << " " << z << vcl_endl;
    vgl_point_3d<double> p(x, y, z);
    //world_points_.push_back(p);
  }
}

obj_observable* poly_hybrid_manager::find_polygon_in_3D(unsigned id, 
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

void poly_hybrid_manager::delete_observable(observable* obs)
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

void poly_hybrid_manager::find_closest_indices(vgui_soview2D_polygon* poly2d, 
                                        vsol_polygon_3d_sptr poly3d,
                                        float x1, float y1,
                                        float x2, float y2,
                                        unsigned &index1, unsigned &index2,
                                        unsigned &index3, unsigned &index4)
{
  vgl_homg_point_2d<double> p1(x1, y1);
  vgl_homg_point_2d<double> p2(x2, y2);
  //vgl_homg_line_3d_2_points<double> line(p1, p2);

  unsigned n = poly2d->n;
  float *x_list, *y_list;
  x_list = poly2d->x;
  y_list = poly2d->y;
  vgl_polygon<float> polygon(x_list, y_list, n);

  for (unsigned j=0; j<n; j++) {
    unsigned next_index = (j==n-1) ? 0 : j+1;
    vgl_homg_point_2d<double> v1(x_list[j], y_list[j]);
    vgl_homg_point_2d<double> v2(x_list[next_index], y_list[next_index]);
    if (collinear(v1, p1, v2)) {
      index1 = j;
      index2 = next_index;
    } else if (collinear(v1, p2, v2)) {
      index3 = j;
      index4 = next_index;
    }
    //vgl_homg_line_3d_2_points<double> v_line(v1, v2);
    //vgl_homg_point_3d<double> intersect = intersection(line, v_line);
  }
}


void poly_hybrid_manager::divide_polygon(vgui_soview2D_polygon* poly2d, 
                                        vsol_polygon_3d_sptr poly3d,
                                        float x1, float y1, float x2, float y2,
                                        vsol_polygon_3d_sptr& poly1,
                                        vsol_polygon_3d_sptr& poly2)
{
  vsol_polygon_2d_sptr p = observer_left_->get_vsol_polygon_2d(poly2d);
  vsol_box_2d_sptr bb = p->get_bounding_box();

  vgl_line_2d<float> line(vgl_point_2d<float>(x1,y1), vgl_point_2d<float>(x2,y2));
  
  vgl_box_2d<float> bbox(bb->get_min_x(), bb->get_max_x() , bb->get_min_y(), bb->get_max_y());

  vgl_point_2d<float> p1, p2;
  bool inter = vgl_intersection<float>(bbox, line, p1, p2);
    
  // find the edge index of the points
  //           2
  //    ----------------
  //    -              -
  //  3 -              - 1
  //    -              -
  //    ----------------
  //            0

  float xmin = bbox.min_x(), xmax = bbox.max_x(), ymin = bbox.min_y(), ymax = bbox.max_y();
  int p1_index, p2_index;
  if (p1.y() == ymin)
    p1_index = 0;
  else if (p1.x() == xmax)
    p1_index = 1;
  else if (p1.y() == ymax)
    p1_index = 2;
  else if (p1.x() == xmin)
    p1_index = 3;
  else
    p1_index = -1;

  if (p2.y() == ymin)
    p2_index = 0;
  else if (p2.x() == xmax)
    p2_index = 1;
  else if (p2.y() == ymax)
    p2_index = 2;
  else if (p2.x() == xmin)
    p2_index = 3;
  else
    p2_index = -1;

  // swap the points if p2 is lowe than p1 in Y direction
  if (p1_index > p2_index) {
    vgl_point_2d<float> temp(p1.x(), p1.y());
    p1.set(p2.x(), p2.y());
    p2.set(temp.x(), temp.y());
    int temp_ind = p2_index;
    p2_index = p1_index;
    p1_index = temp_ind;
  }

  //create a polygon from the box based on the intersection points
  // start with (xmin, ymin)
  
  vgl_polygon<float> bbox_poly(1);
  bbox_poly.push_back(p1.x(), p1.y());
  bbox_poly.push_back(p2.x(), p2.y());
  if ((p1_index == 0) && (p2_index == 1)) {
    bbox_poly.push_back(xmax, ymin);
  } else if ((p1_index == 0) && (p2_index == 2)) {
    bbox_poly.push_back(xmax, ymax);
    bbox_poly.push_back(xmax, ymin);
  } else if ((p1_index == 0) && (p2_index == 3)) {
    bbox_poly.push_back(xmin, ymin);
  } else if ((p1_index == 1) && (p2_index == 2)) {
    bbox_poly.push_back(xmax, ymax);
  } else if ((p1_index == 1) && (p2_index == 3)) {
    bbox_poly.push_back(xmin, ymin);
    bbox_poly.push_back(xmax, ymin);
  } else if ((p1_index == 2) && (p2_index == 3)) {
    bbox_poly.push_back(xmin, ymax);
  }

  vcl_cout << bbox_poly;

  vgl_polygon<float> my_poly(poly2d->x, poly2d->y, poly2d->n);
  vcl_cout << my_poly << vcl_endl;

  // intersect the two polygon
  vgl_polygon<float> new_poly1 = vgl_clip(my_poly, bbox_poly, vgl_clip_type_intersect);
  vgl_polygon<float> new_poly2 = vgl_clip(my_poly, new_poly1, vgl_clip_type_difference);

  vcl_cout << new_poly1 << vcl_endl;
  vcl_cout << new_poly2 << vcl_endl;

  vcl_vector<vsol_point_2d_sptr> vlist1;
  if (new_poly1.num_sheets() > 1)
    vcl_cerr << "More than one polygon found!!!!!!" << vcl_endl;
  //for (unsigned int s = 0; s < new_poly1.num_sheets(); ++s) {
    for (unsigned int p = 0; p < new_poly1[0].size(); ++p) {
      vlist1.push_back(new vsol_point_2d(new_poly1[0][p].x(), new_poly1[0][p].y())); 
  }
 // }

  vcl_vector<vsol_point_2d_sptr> vlist2;
  if (new_poly2.num_sheets() > 1)
    vcl_cerr << "More than one polygon found!!!!!!" << vcl_endl;
  //for (unsigned int s = 0; s < new_poly2.num_sheets(); ++s) {
    for (unsigned int p = 0; p < new_poly2[0].size(); ++p) {
      vlist2.push_back(new vsol_point_2d(new_poly2[0][p].x(), new_poly2[0][p].y())); 
    }
  //}
  observer_left_->backproj_poly(new vsol_polygon_2d(vlist1),poly1, 0);
  observer_left_->backproj_poly(new vsol_polygon_2d(vlist2),poly2, 0);


  vgl_plane_3d<double> plane(vgl_point_3d<double> (poly3d->vertex(0)->x(), poly3d->vertex(0)->y(), poly3d->vertex(0)->z()), 
        vgl_point_3d<double> (poly3d->vertex(1)->x(), poly3d->vertex(1)->y(), poly3d->vertex(1)->z()),
        vgl_point_3d<double> (poly3d->vertex(2)->x(), poly3d->vertex(2)->y(), poly3d->vertex(2)->z()));
  double dist1 = vgl_distance(plane, vgl_point_3d<double>(poly1->vertex(0)->x(), poly1->vertex(0)->y(), poly1->vertex(0)->z())); 
  //double dist2 = vgl_distance(plane, vgl_point_3d<double>(poly22->vertex(0)->x(), poly_new2->vertex(0)->y(), poly_new2->vertex(0)->z())); 

  observer_left_->backproj_poly(new vsol_polygon_2d(vlist1),poly1, dist1);
  observer_left_->backproj_poly(new vsol_polygon_2d(vlist2),poly2, dist1);
}

void poly_hybrid_manager::move_polygon(vsol_polygon_3d_sptr &polygon, double dist)
{
  unsigned n = polygon->size();
  vcl_vector<vsol_point_3d_sptr> vlist(n);
  for(unsigned i=0; i<n; i++) {
    vsol_point_3d_sptr p = polygon->vertex(i);
    vcl_cout << "point before: " << p << vcl_endl;
    vgl_vector_3d<double> normal = polygon->normal_at_point(p);
    vcl_cout << "Normal==>" << normal << vcl_endl;
    double fact = -1*dist;
    vsol_point_3d_sptr ptr = new vsol_point_3d(p->x() + fact*normal.x() , 
      p->y() + fact*normal.y(), 
      p->z() + fact*normal.z());
    vcl_cout << "point after: ";
    ptr->print();
    vlist[i] = ptr;
  }

  polygon = new vsol_polygon_3d(vlist);
}

vpgl_proj_camera<double>
poly_hybrid_manager::read_projective_camera(vcl_string cam_path){

  vcl_ifstream cam_stream(cam_path.data());
  vpgl_proj_camera<double> cam;
  cam_stream >> cam;
  vcl_cout << cam << vcl_endl;
  
  return cam;
}

vpgl_rational_camera<double>
poly_hybrid_manager::read_rational_camera(vcl_string cam_path){
  vcl_ifstream file_inp;
  file_inp.open(cam_path.c_str());
  if (!file_inp.good()) {
    vcl_cout << "error: bad filename: " << cam_path << vcl_endl;
    vpgl_rational_camera<double> dummy_camera;
    return dummy_camera;
  }

  vcl_vector<double> neu_u;
  vcl_vector<double> den_u;
  vcl_vector<double> neu_v;
  vcl_vector<double> den_v;
  double x_scale,x_off,y_scale,y_off,z_scale,z_off,u_scale,u_off,v_scale,v_off;

  vcl_string input;
  char bulk[100];

  while(!file_inp.eof()){
    file_inp >> input;

    if (input=="sampScale") {
      file_inp >> input;
      file_inp >> u_scale;
    }
    if (input=="sampOffset") {
      file_inp >> input;
      file_inp >> u_off;
    }

    if (input=="lineScale") {
      file_inp >> input;
      file_inp >> v_scale;
    }
    if (input=="lineOffset") {
      file_inp >> input;
      file_inp >> v_off;
    }

    if (input=="longScale") {
      file_inp >> input;
      file_inp >> x_scale;
    }
    if (input=="longOffset") {
      file_inp >> input;
      file_inp >> x_off;
    }

    if (input=="latScale") {
      file_inp >> input;
      file_inp >> y_scale;
    }
    if (input=="latOffset") {
      file_inp >> input;
      file_inp >> y_off;
    }

    if (input=="heightScale") {
      file_inp >> input;
      file_inp >> z_scale;
    }
    if (input=="heightOffset") {
      file_inp >> input;
      file_inp >> z_off;
    }

    double temp_dbl;
    if (input=="lineNumCoef") {
      file_inp >> input;
      file_inp >> input;
      for (int i=0; i<20; i++) {
        file_inp >> temp_dbl;
        neu_v.push_back(temp_dbl);
        file_inp.getline(bulk,200);
      }
    }

    if (input=="lineDenCoef") {
      file_inp >> input;
      file_inp >> input;
      for (int i=0; i<20; i++) {
        file_inp >> temp_dbl;
        den_v.push_back(temp_dbl);
        file_inp.getline(bulk,200);
      }
    }

    if (input=="sampNumCoef") {
      file_inp >> input;
      file_inp >> input;
      for (int i=0; i<20; i++) {
        file_inp >> temp_dbl;
        neu_u.push_back(temp_dbl);
        file_inp.getline(bulk,200);
      }
    }

    if (input=="sampDenCoef") {
      file_inp >> input;
      file_inp >> input;
      for (int i=0; i<20; i++) {
        file_inp >> temp_dbl;
        den_u.push_back(temp_dbl);
        file_inp.getline(bulk,200);
      }
    }

  }
  file_inp.close();

  int map[20];
  map[0]=19;
  map[1]=9;
  map[2]=15;
  map[3]=18;
  map[4]=6;
  map[5]=8;
  map[6]=14;
  map[7]=3;
  map[8]=12;
  map[9]=17;
  map[10]=5;
  map[11]=0;
  map[12]=4;
  map[13]=7;
  map[14]=1;
  map[15]=10;
  map[16]=13;
  map[17]=2;
  map[18]=11;
  map[19]=16;

  double temp_vector[20];

  for (int j=0; j<20; j++) {
    temp_vector[j] = neu_u[j];
  }
  for (int j=0; j<20; j++) {
    neu_u[map[j]] = temp_vector[j];
  }
  for (int j=0; j<20; j++) {
    temp_vector[j] = den_u[j];
  }
  for (int j=0; j<20; j++) {
    den_u[map[j]] = temp_vector[j];
  }
  for (int j=0; j<20; j++) {
    temp_vector[j] = neu_v[j];
  }
  for (int j=0; j<20; j++) {
    neu_v[map[j]] = temp_vector[j];
  }
  for (int j=0; j<20; j++) {
    temp_vector[j] = den_v[j];
  }
  for (int j=0; j<20; j++) {
    den_v[map[j]] = temp_vector[j];
  }

  vpgl_rational_camera<double> return_val(neu_u,den_u,neu_v,den_v,x_scale,x_off,y_scale,y_off,z_scale,z_off,u_scale,u_off,v_scale,v_off);
  return return_val;

}


