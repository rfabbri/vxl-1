#include "poly_manager.h"
#include "texture_map_generator.h"
#include "poly_menu.h"

#include <vul/vul_file.h> 
#include <vgui/vgui_grid_tableau.h> 
#include <bgui/bgui_image_tableau.h> 
#include <vgui/vgui_viewer2D_tableau.h> 
#include <vgui/vgui_composite_tableau.h> 
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_range_map_params.h>
#include <vgui/vgui_popup_params.h>

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
#include <vgl/vgl_homg_plane_3d.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>

#include <vil/vil_load.h>
#include <vil/vil_blocked_image_resource.h>
#include <vil/vil_pyramid_image_resource.h>
#include <vil/vil_property.h>
#include <bgui/bgui_image_utils.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_3d_sptr.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_box_2d.h>

#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_triangulate.h>
#include <dbmsh3d/vis/dbmsh3d_vis_backpt.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>
#include <dbmsh3d/vis/dbmsh3d_vis_face.h>

#include <dbmsh3d/dbmsh3d_textured_mesh_mc.h>
#include <dbmsh3d/dbmsh3d_vertex.h>
#include <dbmsh3d/dbmsh3d_textured_face_mc.h>

#include <bgeo/bgeo_lvcs.h>

poly_manager* poly_manager::instance_ = 0;

poly_manager* poly_manager::instance() {
  if (!instance_) 
    instance_ = new poly_manager();
  return poly_manager::instance_;
   
}

poly_manager::poly_manager()
  : my_obj(0), my_polygon(0), my_polygon_copy(0), observer_left_(0), observer_right_(0), selected_vertex_(0)
{
}

poly_manager::~poly_manager(void)
{
  
  if (my_polygon != 0)
    delete my_polygon;
  if (my_polygon_copy != 0)
    delete my_polygon_copy;
  if (my_obj != 0)
    delete my_obj;
  std::list<obj_observable*>::iterator iter=objects.begin();
  while (iter != objects.end()) {
    if (*iter)
      delete (*iter);
    iter++;
  }
  
  delete observer_right_;
  delete observer_left_;

}

void poly_manager::setup_scene (vgui_grid_tableau_sptr grid)
{
  grid_tableau_ = grid;
}

void poly_manager::load_left_image()
{
  std::string filename = select_file();
  if (!filename.empty()) {
    load_left_image(filename);
  }
  return;
}

void poly_manager::load_left_image(std::string filename)
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
      std::cerr << "error loading image pyramid "<< filename << std::endl;
      return;
    }
  } 
  else {
    res = vil_load_image_resource(filename.c_str());
  }
  //vgui_range_map_params_sptr rmps = new vgui_range_map_params(min, max, min, max, min, max,
  //  gamma, gamma, gamma, invert,
  //  gl_map, cache);
  vgui_range_map_params_sptr rmps = range_params(res);


  bgui_image_tableau_sptr img_left = bgui_image_tableau_new(res,rmps);
  if (observer_left_)
    delete observer_left_;

  observer_left_ = new poly_cam_observer(img_left);
  observer_left_->set_line_width(2.0f);
  left_ = bgui_picker_tableau_new(observer_left_);
  vgui_composite_tableau_sptr comp = new vgui_composite_tableau(left_, this);
  vgui_viewer2D_tableau_new l(comp);
 // l->center_image(img_left->width(), img_left->height());
  
  grid_tableau_->add_at(l, 0, 0);
  return;
}
//: Calculate the range parameters for the input image
vgui_range_map_params_sptr poly_manager::
range_params(vil_image_resource_sptr const& image)
{
  float gamma = 1.0;
  bool invert = false;
  bool gl_map = false;
  bool cache = true;

  //Check if the image is blocked
  vil_blocked_image_resource_sptr bir = blocked_image_resource(image);
  if (bir){ 
    gl_map = true;
    cache = false;
    std::cout << "image is blocked.\n";
  }else {
    std::cout << "image is not blocked.\n";
  }

  //Check if the image is a pyramid
  bool pyr = image->get_property(vil_property_pyramid, 0);
  if (pyr){
    gl_map = true; 
    cache = false;
    std::cout << "image is a pyramid.\n";
  }else {
    std::cout << "image is not a pyramid\n.";
  }
  //Get max min parameters

  double min=0, max=0;
  unsigned n_components = image->nplanes();
  vgui_range_map_params_sptr rmps;
  if (n_components == 1)
  {
    bgui_image_utils iu(image);
    iu.range(min, max);
    rmps= new vgui_range_map_params(min, max, gamma, invert,
                                    gl_map, cache);
  }
  else if (n_components == 3)
  {
    min = 0; max = 255;//for now - ultimately need to compute color histogram
    rmps = new vgui_range_map_params(min, max, min, max, min, max,
                                     gamma, gamma, gamma, invert,
                                     gl_map, cache);
  }
  return rmps;
}
  
void poly_manager::load_left_camera()
{
  if (!observer_left_) {
    std::cerr << "error loading camera: no observer yet!" << std::endl;
    return;
  }
  vpgl_camera<double> *cam = select_camera();
  observer_left_->set_camera(cam);
}

void poly_manager::save_left_camera()
{
  std::string filename = select_file();
  if (filename.empty()) {
    std::cerr << "no filename selected\n";
    return;
  }
  if (vpgl_rational_camera<double> *rat_cam = dynamic_cast<vpgl_rational_camera<double>*>(observer_left_->get_camera())) {
    rat_cam->save(filename);
  }
  else {
    std::cerr << "error: can only save rational cameras\n";
  }
  return;
}

    
void poly_manager::load_right_image()
{
  std::string filename = select_file();
  if (!filename.empty()) {
    load_right_image(filename);
  }
  return;
}

void poly_manager::load_right_image(std::string filename)
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
      std::cerr << "error loading image pyramid "<< filename << std::endl;
      return;
    }
  } 
  else {
    res = vil_load_image_resource(filename.c_str());
  }
  //vgui_range_map_params_sptr rmps = new vgui_range_map_params(min, max, min, max, min, max,
  //  gamma, gamma, gamma, invert,
  //  gl_map, cache);
  vgui_range_map_params_sptr rmps = range_params(res);

  bgui_image_tableau_sptr img_right = bgui_image_tableau_new(res,rmps);
  if (observer_right_)
    delete observer_right_;
  observer_right_ = new poly_cam_observer(img_right);
  observer_right_->set_line_width(2.0f);
  right_ = bgui_picker_tableau_new(observer_right_);
  vgui_composite_tableau_sptr comp = new vgui_composite_tableau(right_, this);
  vgui_viewer2D_tableau_new r(comp);
  //r->center_image(img_right->width(), img_right->height());
  grid_tableau_->add_at(r, 1, 0);

  return;
}

  
void poly_manager::load_right_camera()
{
  if (!observer_right_) {
    std::cerr << "error loading camera: no observer yet!" << std::endl;
    return;
  }
  vpgl_camera<double> *cam = select_camera();
  observer_right_->set_camera(cam);
}

void poly_manager::save_right_camera()
{
  std::string filename = select_file();
  if (filename.empty()) {
    std::cerr << "no filename selected\n";
    return;
  }
  if (vpgl_rational_camera<double> *rat_cam = dynamic_cast<vpgl_rational_camera<double>*>(observer_right_->get_camera())) {
    rat_cam->save(filename);
  }
  else {
    std::cerr << "error: can only save rational cameras\n";
  }
  return;
}


void poly_manager::load_images_and_cameras()
{
  std::string filename = select_file();
  if (filename.empty())
    {
      std::cout << "Bad filename\n";
      return;
    }
  std::ifstream is(filename.data());
  if(!is.is_open())
    {
      std::cout << "Can't open file\n";
      return;
    }
  std::string name;
  is >> name;
  if(name != "LEFT:")
    {
      std::cout << "Bad file parse\n";
      return;
    }
  is >> name;
  if(name != "IMAGE:")
    {
      std::cout << "Bad file parse\n";
      return;
    }
  std::string left_image_path;
  is >> left_image_path;
  is >> name;
  if(name != "CAMERA_TYPE:")
    {
      std::cout << "Bad file parse\n";
      return;
    }
  std::string left_camera_type;
  is >> left_camera_type;
  is >> name;
  if(name != "CAMERA_PATH:")
    {
      std::cout << "Bad file parse\n";
      return;
    }
  std::string left_camera_path;
  is >> left_camera_path;
  is >> name;
  if(name != "RIGHT:")
    {
      std::cout << "Bad file parse\n";
      return;
    }
  is >> name;
  if(name != "IMAGE:")
    {
      std::cout << "Bad file parse\n";
      return;
    }
  std::string right_image_path;
  is >> right_image_path;
  is >> name;
  if(name != "CAMERA_TYPE:")
    {
      std::cout << "Bad file parse\n";
      return;
    }
  std::string right_camera_type;
  is >> right_camera_type;
  is >> name;
  if(name != "CAMERA_PATH:")
    {
      std::cout << "Bad file parse\n";
      return;
    }
  std::string right_camera_path;
  is >> right_camera_path;
  //parsing done

  load_left_image(left_image_path);

  //std::cout << limg.cols() << "  " << limg.rows();
  //observer_left_->add_image(0, 0, *(res->get_view()));
  //observer_left_->set_image(left_image_path);
  vpgl_camera<double> *left_camera;
  if(left_camera_type=="projective")
    left_camera = (read_projective_camera(left_camera_path)).clone();
  else if( left_camera_type=="rational") {
    left_camera = new vpgl_rational_camera<double>(left_camera_path);
    double zoff = ((vpgl_rational_camera<double>*)left_camera)->offset(vpgl_rational_camera<double>::Z_INDX);
    vgl_homg_plane_3d<double> plane(0,0,1,-zoff);
    observer_left_->set_proj_plane(plane);
  } else
    {
      std::cout << "Unsupported camera type" << left_camera_type << '\n';
      return;
    }
  observer_left_->set_camera(left_camera);

  load_right_image(right_image_path);

  vpgl_camera<double> *right_camera;
  if(right_camera_type=="projective")
    right_camera = (read_projective_camera(right_camera_path)).clone();
  else if( right_camera_type=="rational")
    right_camera = new vpgl_rational_camera<double>(right_camera_path);
  else
    {
      std::cout << "Unsupported camera type" << right_camera_type << '\n';
      return;
    }
  observer_right_->set_camera(right_camera);
}
void poly_manager::select_proj_plane()
{
  obj_observable* obs=0;
  unsigned face_id;
  get_selection(obs, face_id);
  if (obs) {
    vgl_homg_plane_3d<double> plane = obs->get_plane_of_face(face_id);
    observer_left_->set_proj_plane(plane);
  } else {
    std::cerr << "Selected plain is not valid" << std::endl;
  }
}

//: manually pick point in image that should correspond with the lvcs origin,
//  and adjust camera model accordingly
void poly_manager::adjust_left_camera_offset()
{
  // make sure lvcs is defined
  if (!lvcs_) {
    std::cerr << "error: no lvcs defined!\n";
    return;
  }
  // make sure camera is rational


  float x0,y0;
  left_->pick_point(&x0,&y0);
  vsol_point_2d_sptr img_point = new vsol_point_2d(x0,y0);

  // get projection of lvcs origin
  double lat,lon,elev;
  lvcs_->local_to_global(0,0,0,bgeo_lvcs::wgs84,lon,lat,elev,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
  vgl_point_3d<double> world_pt(lon,lat,elev);
  vgl_point_2d<double> image_pt;
  observer_left_->proj_point(world_pt,image_pt);

  // shift camera translation to line up points
  if (observer_left_->shift_rational_camera(x0 - image_pt.x(),y0 - image_pt.y())) {
    std::cout << "shifted left camera offset by [" << x0-image_pt.x() <<", " << y0-image_pt.y() <<"]\n";
  }else {
    std::cerr << " error shifting camera offset\n";
  }

  return;
}

//: manually pick point in image that should correspond with the lvcs origin,
//  and adjust camera model accordingly
void poly_manager::adjust_right_camera_offset()
{
  // make sure lvcs is defined
  if (!lvcs_) {
    std::cerr << "error: no lvcs defined!\n";
    return;
  }

  float x0,y0;
  right_->pick_point(&x0,&y0);
  vsol_point_2d_sptr img_point = new vsol_point_2d(x0,y0);

  // get projection of lvcs origin
  double lat,lon,elev;
  lvcs_->local_to_global(0,0,0,bgeo_lvcs::wgs84,lon,lat,elev,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
  vgl_point_3d<double> world_pt(lon,lat,elev);
  vgl_point_2d<double> image_pt;
  observer_right_->proj_point(world_pt,image_pt);

  // shift camera translation to line up points
  if (observer_right_->shift_rational_camera(x0 - image_pt.x(),y0 - image_pt.y())) {
    std::cout << "shifted right camera offset by [" << x0-image_pt.x() <<", " << y0-image_pt.y() <<"]\n";
  }else {
    std::cerr << " error shifting camera offset\n";
  }

  return;
}

void poly_manager::triangulate_meshes()
{
  std::list<obj_observable*>::iterator it;
  int obj_count = 0;
  for (it = objects.begin(); it != objects.end(); it++, obj_count++) {
    std::printf("triangulating mesh %d\n",obj_count);
    obj_observable* obj = *it;
    // assume object is texture mapped
    dbmsh3d_mesh* mesh = (dbmsh3d_mesh*)obj->get_object();
    ///mesh->build_IFS_mesh();
    dbmsh3d_mesh* tri_mesh = generate_tri_mesh(mesh);
    //tri_mesh->build_IFS_mesh();
    ///tri_mesh->IFS_to_MHE();
    //tri_mesh->build_IFS_mesh();
    dbmsh3d_mesh_mc* tri_mesh_mc = new dbmsh3d_mesh_mc(tri_mesh);

    obj->replace(tri_mesh_mc);
    delete tri_mesh;
  }
}

void poly_manager::define_proj_plane()
{
  // pick the ground truth line
  float x1, y1, x2, y2;
  left_->pick_line(&x1, &y1, &x2, &y2);
  std::cout << "(" << x1 << "," << y1 << ")" << "(" << x2 << "," << y2 << ")" << std::endl;
  observer_left_->set_ground_plane(x1, y1, x2, y2);
}

void poly_manager::define_xy_proj_plane()
{
  static double z = 0;
  vgui_dialog zval("Define XY plane");
  zval.field("Elevation (meters)", z);
  if (!zval.ask())
    return;
  vgl_homg_plane_3d<double> plane(0,0,1,-z);
  observer_left_->set_proj_plane(plane);
}

void poly_manager::define_yz_proj_plane()
{
  static double x = 0;
  vgui_dialog xval("Define YZ plane");
  xval.field("X (meters)", x);
  if (!xval.ask())
    return;
  vgl_homg_plane_3d<double> plane(1,0,0,-x);
  observer_left_->set_proj_plane(plane);
}

void poly_manager::define_xz_proj_plane()
{
  static double y= 0;
  vgui_dialog yval("Define XZ plane");
  yval.field("Y (meters)", y);
  if (!yval.ask())
    return;
  vgl_homg_plane_3d<double> plane(0,1,0,-y);
  observer_left_->set_proj_plane(plane);
}

void poly_manager::save()
{
  vgui_dialog params("File Save");
  std::string ext, file, empty="";

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

  std::list<obj_observable*>::iterator it = objects.begin();
  while (it != objects.end()) {
    obj_observable* o = *it;
    dbmsh3d_mesh_mc* mesh = o->get_object();
    save_mesh(mesh,file.data(),use_lvcs);
    it++;
  }
}

void poly_manager::save_x3d()
{
  if (!lvcs_) {
    std::cerr << "Error: lvcs not defined.\n";
    return;
  }
  vgui_dialog params("File Save");
  std::string ext, x3d_filename, empty="";

  params.file("Save...",ext,x3d_filename);
  if (!params.ask())
    return;

  if (x3d_filename == "") {
    std::cerr << "Error: no filename selected.\n";
    return;
  }

  FILE* fp;
  if ((fp = std::fopen(x3d_filename.c_str(), "w")) == NULL) {
    std::fprintf (stderr, "Can't open x3d file %s to write.\n", x3d_filename.c_str());
    return; 
  }

  
  std::fprintf(fp, "#VRML V2.0 utf8\n");
  //std::fprintf(fp, "PROFILE Immersive\n\n");

  std::list<obj_observable*>::iterator it;
  int obj_count = 0;
  for (it = objects.begin(); it != objects.end(); it++, obj_count++) {
    obj_observable* obj = *it;
    // assume object is texture mapped
    dbmsh3d_textured_mesh_mc* mesh = (dbmsh3d_textured_mesh_mc*)obj->get_object();

    // texture image is actually defined per face in the dbmsh3d structure, but we will 
    // assume that it is the same for every face of the mesh here.
    dbmsh3d_textured_face_mc* first_face = (dbmsh3d_textured_face_mc*)mesh->facemap().begin()->second;
    // just want filename, not full path
    std::string texmap_url = vul_file::strip_directory(first_face->tex_map_uri());
  
    std::fprintf(fp, "Transform {\n");
    std::fprintf(fp, "  children\n");
    std::fprintf(fp, "  Shape {\n");
    std::fprintf(fp, "    appearance Appearance {\n");
    std::fprintf(fp, "      material Material{}\n");
    std::fprintf(fp, "      texture ImageTexture {\n");
    std::fprintf(fp, "        url \"%s\"\n",texmap_url.c_str());
    std::fprintf(fp, "      }\n");
    std::fprintf(fp, "    }\n");
    std::fprintf(fp, "    geometry IndexedFaceSet {\n");
    std::fprintf(fp, "      coord Coordinate {\n");
    std::fprintf(fp, "        point [\n");

    // map vertex ID's to indices.
    std::map<int,int> vert_indices;

    std::map<int, dbmsh3d_vertex*>::iterator vit;
    int idx = 0;
    for (vit = mesh->vertexmap().begin(); vit != mesh->vertexmap().end(); vit++, idx++) {
      dbmsh3d_vertex* v = (dbmsh3d_vertex*)vit->second;
      vert_indices[v->id()] = idx;
      double x,y,z;
      lvcs_->global_to_local(v->pt().x(),v->pt().y(),v->pt().z(),bgeo_lvcs::wgs84,x,y,z,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
      std::fprintf(fp,"       %0.8f %0.8f %0.8f,\n",x,y,z);
    }
    std::fprintf(fp, "        ]\n");
    std::fprintf(fp, "      }\n");
    std::fprintf(fp, "      coordIndex[\n");
    
    std::map<int, dbmsh3d_face*>::iterator fit;
    for (fit = mesh->facemap().begin(); fit!= mesh->facemap().end(); fit++) {
      dbmsh3d_textured_face_mc* face = (dbmsh3d_textured_face_mc*)fit->second;
      std::fprintf(fp, "             ");   
      for (unsigned j=0; j<face->vertices().size(); j++) {
        dbmsh3d_vertex* v = (dbmsh3d_vertex*) face->vertices(j);
        std::fprintf( fp, "%d ",vert_indices[v->id()]);
      }
      std::fprintf(fp, "-1,\n");
    }
    std::fprintf(fp, "      ]\n\n");

    std::fprintf(fp, "      texCoord TextureCoordinate {\n");
    std::fprintf(fp, "        point [\n");

    for (fit = mesh->facemap().begin(); fit!= mesh->facemap().end(); fit++) {
      dbmsh3d_textured_face_mc* face = (dbmsh3d_textured_face_mc*)fit->second;
      for (unsigned j=0; j<face->vertices().size(); j++) {
        dbmsh3d_vertex* v = (dbmsh3d_vertex*) face->vertices(j);
        vgl_point_2d<double> pt = face->tex_coords(v->id());
        std::fprintf(fp, "           %0.8f %0.8f,\n",pt.x(),pt.y());
      }
    }
    std::fprintf(fp, "        ]\n");
    std::fprintf(fp, "      }\n\n");  

    std::fprintf(fp, "      texCoordIndex[\n");
    int tex_coord_idx = 0;
    for (fit = mesh->facemap().begin(); fit!= mesh->facemap().end(); fit++) {
      dbmsh3d_textured_face_mc* face = (dbmsh3d_textured_face_mc*)fit->second;
      std::fprintf(fp, "                ");    
      for (unsigned j=0; j < face->vertices().size(); j++) {
        std::fprintf(fp, "%d ",tex_coord_idx++);
      }
      std::fprintf(fp, "-1,\n");
    }
    std::fprintf(fp, "      ]\n\n");
    std::fprintf(fp, "      solid TRUE\n");
    std::fprintf(fp, "      convex FALSE\n");
    std::fprintf(fp, "      creaseAngle 0\n");
    std::fprintf(fp, "    }\n");
    std::fprintf(fp, "  }\n");
    std::fprintf(fp, "}\n\n\n");
  }

  std::fclose(fp);
  return;
}
  
void poly_manager::save_gml()
{
  if (!lvcs_) {
    std::cerr << "Error: lvcs not defined.\n";
    return;
  }
  vgui_dialog params("File Save");
  std::string ext, gml_filename, empty="";
  std::string model_name;

  params.field("model name",model_name);

  params.file("Save...",ext,gml_filename);
  if (!params.ask())
    return;

  if (gml_filename == "") {
    std::cerr << "Error: no filename selected.\n";
    return;
  }

  FILE* fp;
  if ((fp = std::fopen(gml_filename.c_str(), "w")) == NULL) {
    std::fprintf (stderr, "Can't open xml file %s to write.\n", gml_filename.c_str());
    return; 
  }


  std::fprintf (fp, "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>\n");
  std::fprintf (fp, "<CityModel xmlns=\"http://www.citygml.org/citygml/1/0/0\" xmlns:gml=\"http://www.opengis.net/gml\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.citygml.org/citygml/1/0/0 http://www.citygml.org/citygml/1/0/0/CityGML.xsd\">\n");
  std::fprintf (fp, "<gml:description>%s</gml:description>\n",model_name.c_str());
  std::fprintf (fp, "<gml:name>%s</gml:name>\n",model_name.c_str());

  std::list<obj_observable*>::iterator it;
  int obj_count = 0;
  for (it = objects.begin(); it != objects.end(); it++, obj_count++) {
    obj_observable* obj = *it;
    // assume object is texture mapped
    dbmsh3d_textured_mesh_mc* mesh = (dbmsh3d_textured_mesh_mc*)obj->get_object();

    //mesh->IFS_to_MHE();
    //mesh->orient_face_normals();
    //mesh->build_face_IFS();
  
    std::fprintf (fp, "<cityObjectMember>\n");
    std::fprintf (fp, "<Building>\n");
    std::fprintf (fp, "<gml:description>Building #%d</gml:description>\n",obj_count);
    std::fprintf (fp, "<gml:name>Building #%d</gml:name>\n",obj_count);
    //std::fprintf (fp, "<consistsOfBuildingPart>");
    
    //std::fprintf (fp, "<BuildingPart>");
    //std::fprintf (fp, " <gml:description>BuildingPart Description</gml:description>");
    //std::fprintf (fp, "<gml:name>BuildingPart Name</gml:name>");
    
    std::map<int, dbmsh3d_face*>::iterator fit;
    for (fit = mesh->facemap().begin(); fit!= mesh->facemap().end(); fit++) {
    dbmsh3d_textured_face_mc* face = (dbmsh3d_textured_face_mc*)fit->second;
    
    std::fprintf (fp, "<boundedBy>");
    std::fprintf (fp, "<WallSurface>");
    std::fprintf (fp, "<lod4MultiSurface><gml:MultiSurface>\n");
    std::fprintf (fp, "<gml:surfaceMember>");
    std::fprintf (fp, "<TexturedSurface orientation=\"+\">");
    std::fprintf (fp, "<gml:baseSurface>");
    std::fprintf (fp, "<gml:Polygon>");
    std::fprintf (fp, "<gml:exterior>");
    std::fprintf (fp, "<gml:LinearRing>\n");
    
    for (unsigned j=0; j<face->vertices().size(); j++) {
      dbmsh3d_vertex* v = (dbmsh3d_vertex*) face->vertices(j);
      double x,y,z;
      lvcs_->global_to_local(v->pt().x(),v->pt().y(),v->pt().z(),bgeo_lvcs::wgs84,x,y,z,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
      std::fprintf (fp, "<gml:pos srsDimension=\"3\">");
      std::fprintf (fp, "%.16f ", x);
      std::fprintf (fp, "%.16f ", y);
      std::fprintf (fp, "%.16f ", z);
      std::fprintf (fp, "</gml:pos>\n");
    }
    //Now print the first vertex again to close the polygon
    dbmsh3d_vertex* v = (dbmsh3d_vertex*) face->vertices(0);  
    double x,y,z;
    lvcs_->global_to_local(v->pt().x(),v->pt().y(),v->pt().z(),bgeo_lvcs::wgs84,x,y,z,bgeo_lvcs::DEG,bgeo_lvcs::METERS); 
    std::fprintf (fp, "<gml:pos srsDimension=\"3\">");
    std::fprintf (fp, "%.16f ", x);
    std::fprintf (fp, "%.16f ", y);
    std::fprintf (fp, "%.16f ", z);
    std::fprintf (fp, "</gml:pos>\n");

    std::fprintf (fp, "</gml:LinearRing>");
    std::fprintf (fp, "</gml:exterior>");
    std::fprintf (fp, "</gml:Polygon>");
    std::fprintf (fp, "</gml:baseSurface>");

    // texture
    std::fprintf(fp, "<appearance><SimpleTexture>\n");
    std::fprintf(fp, "<textureMap>%s</textureMap>\n",vul_file::strip_directory(face->tex_map_uri().c_str()));
    std::fprintf(fp, "<textureCoordinates>");
    for (unsigned j=0; j<face->vertices().size(); j++) {
      v = (dbmsh3d_vertex*) face->vertices(j);
      //dbmsh3d_textured_vertex_3d* tv = (dbmsh3d_textured_vertex_3d*) face->vertices(j);  
      vgl_point_2d<double> pt_tex = face->tex_coords(v->id());
      std::fprintf (fp, "%.8f ", pt_tex.x());
      std::fprintf (fp, "%.8f ", pt_tex.y());
    }
    //Now print the first vertex again to close the polygon
    v = (dbmsh3d_vertex*) face->vertices(0);
    vgl_point_2d<double> pt_tex = face->tex_coords(v->id());
    std::fprintf (fp, "%.8f ", pt_tex.x());
    std::fprintf (fp, "%.8f ", pt_tex.y());

    std::fprintf(fp,"</textureCoordinates>\n");
    std::fprintf(fp,"<textureType>specific</textureType>\n");
    std::fprintf(fp,"</SimpleTexture></appearance>\n");

    std::fprintf (fp, "</TexturedSurface>\n");     
    std::fprintf (fp, "</gml:surfaceMember>");
    std::fprintf (fp, "</gml:MultiSurface></lod4MultiSurface>");
    std::fprintf (fp, "</WallSurface>");
    std::fprintf (fp, "</boundedBy>\n");         

  }
  //std::fprintf (fp, "       </BuildingPart>");
  //std::fprintf (fp, "     </consistsOfBuildingPart>");
  std::fprintf (fp, "   </Building>");
  std::fprintf (fp, "  </cityObjectMember>");       
  }
  std::fprintf (fp, " </CityModel>");

  std::fclose (fp);

}

void poly_manager::save_kml_collada()
{
  if (!lvcs_) {
    std::cerr << "Error: lvcs not defined.\n";
    return;
  }
  double origin_lat = 0,origin_lon = 0, origin_elev = 0;
  lvcs_->get_origin(origin_lat,origin_lon,origin_elev);

  vgui_dialog params("File Save");
  std::string ext, kmz_dir, empty="";

  // guess at ground height = lowest vertex
  double minz = 1e6;
  std::list<obj_observable*>::iterator it;
  std::map<int, dbmsh3d_vertex*>::iterator vit;
  for (it = objects.begin(); it != objects.end(); it++) {
    obj_observable* obj = *it;
    // assume object is texture mapped
    dbmsh3d_mesh_mc* mesh = (dbmsh3d_mesh_mc*)obj->get_object();
    for (vit = mesh->vertexmap().begin(); vit!= mesh->vertexmap().end(); vit++) {
      dbmsh3d_vertex* v = (dbmsh3d_vertex*)vit->second;
      double x,y,z;
      lvcs_->global_to_local(v->pt().x(),v->pt().y(),v->pt().z(),bgeo_lvcs::wgs84,x,y,z,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
      if (z < minz) {
        minz = z;
      }
    }
  }

  double ground_height = minz;
  double lat_offset = 0.0;
  double lon_offset = 0.0;
  std::string model_name;

  params.field("model name",model_name);
  params.field("ground height",ground_height);
  params.field("lattitude offset",lat_offset);
  params.field("longitude offset",lon_offset);

  params.file("Save...",ext,kmz_dir);
  if (!params.ask())
    return;

  if (kmz_dir == "") {
    std::cerr << "Error: no filename selected.\n";
    return;
  }

  if (!vul_file::is_directory(kmz_dir)) {
    std::cerr << "Error: Select a directory name.\n";
    return;
  }

  std::ostringstream dae_fname;
  dae_fname << kmz_dir << "/models/mesh.dae";

  FILE* dae_fp;
  if ((dae_fp = std::fopen(dae_fname.str().data(), "w")) == NULL) {
    std::fprintf (stderr, "Can't open .dae file %s to write.\n", dae_fname.str().data());
    return; 
  }

  std::vector<std::string> image_names;
  std::vector<std::string> image_fnames;
  std::vector<std::string> material_ids;
  std::vector<std::string> material_names;
  std::vector<std::string> effect_ids;
  std::vector<std::string> surface_ids;
  std::vector<std::string> image_sampler_ids;
  std::vector<std::string> geometry_ids;
  std::vector<std::string> geometry_position_ids;
  std::vector<std::string> geometry_position_array_ids;
  std::vector<std::string> geometry_uv_ids;
  std::vector<std::string> geometry_uv_array_ids;
  std::vector<std::string> geometry_vertex_ids;
  std::vector<std::string> mesh_ids;

  int nobjects = 0;
  unsigned min_faces = 3;
  //std::list<obj_observable*>::iterator it;
  for (it = objects.begin(); it != objects.end(); it++) {
    obj_observable* obj = *it;
    // assume object is texture mapped
    dbmsh3d_textured_mesh_mc* mesh = (dbmsh3d_textured_mesh_mc*)obj->get_object();

    //mesh->build_IFS_mesh();
    //mesh->IFS_to_MHE();
   // mesh->orient_face_normals();
    //mesh->build_IFS_mesh();

    if (mesh->facemap().size() <  min_faces) {
      // single mesh face is probably ground plane, which we do not want to render
      continue;
    }

    dbmsh3d_textured_face_mc* face0 = (dbmsh3d_textured_face_mc*)mesh->facemap(0);
    std::string image_fname = vul_file::strip_directory(face0->tex_map_uri()); // assume all faces have same texmap img
    std::string image_name = vul_file::strip_extension(image_fname);
    
    std::ostringstream image_path;
    image_path << "../images/" << image_fname;

    std::ostringstream objname;
    objname << "object_"<<nobjects;

    std::ostringstream material_id;
    material_id << objname.str() <<"_materialID";

    std::ostringstream material_name;
    material_name << objname.str() <<"_material";

    std::ostringstream effect_id;
    effect_id << objname.str() << "_effect";

    std::ostringstream surface_id;
    surface_id << objname.str() << "_surface";
   
    std::ostringstream image_sampler_id;
    image_sampler_id << objname.str() << "_sampler";

    std::ostringstream geometry_id;
    geometry_id << objname.str() << "_geometry";

    std::ostringstream geometry_position_id;
    geometry_position_id << objname.str() << "_geometry_position";

    std::ostringstream geometry_position_array_id;
    geometry_position_array_id << objname.str() <<"_geometry_position_array";


    std::ostringstream geometry_uv_id;
    geometry_uv_id << objname.str() << "_geometry_uv";

    std::ostringstream geometry_uv_array_id;
    geometry_uv_array_id << objname.str() << "_geometry_uv_array";

    std::stringstream geometry_vertex_id;
    geometry_vertex_id << objname.str() << "_geometry_vertex";

    mesh_ids.push_back(objname.str());
    image_names.push_back(image_name);
    image_fnames.push_back(image_path.str());
    material_names.push_back(material_name.str());
    material_ids.push_back(material_id.str());
    effect_ids.push_back(effect_id.str());
    surface_ids.push_back(surface_id.str());
    image_sampler_ids.push_back(image_sampler_id.str());
    geometry_ids.push_back(geometry_id.str());
    geometry_position_ids.push_back(geometry_position_id.str());
    geometry_position_array_ids.push_back(geometry_position_array_id.str());
    geometry_uv_ids.push_back(geometry_uv_id.str());
    geometry_uv_array_ids.push_back(geometry_uv_array_id.str());
    geometry_vertex_ids.push_back(geometry_vertex_id.str());

    nobjects++;
  }

  std::fprintf(dae_fp,"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
  std::fprintf(dae_fp,"<COLLADA xmlns=\"http://www.collada.org/2005/11/COLLADASchema\" version=\"1.4.1\">\n");

  std::fprintf(dae_fp,"  <asset>\n");
  std::fprintf(dae_fp,"    <contributor>\n");
  std::fprintf(dae_fp,"      <authoring_tool> Brown University World Modeler </authoring_tool>\n");
  std::fprintf(dae_fp,"    </contributor>\n");
  std::fprintf(dae_fp,"    <unit name=\"meters\" meter=\"1\"/>\n");
  std::fprintf(dae_fp,"    <up_axis>Z_UP</up_axis>\n");
  std::fprintf(dae_fp,"  </asset>\n");

  std::fprintf(dae_fp,"  <library_images>\n");
  for (int i=0; i<nobjects; i++) {
    std::fprintf(dae_fp,"    <image id=\"%s\" name=\"%s\">\n",image_names[i].c_str(),image_names[i].c_str());
    std::fprintf(dae_fp,"      <init_from>%s</init_from>\n",image_fnames[i].c_str());
    std::fprintf(dae_fp,"    </image>\n");
  }
  std::fprintf(dae_fp,"  </library_images>\n");

  std::fprintf(dae_fp,"  <library_materials>\n");
  for (int i=0; i<nobjects; i++) {
    std::fprintf(dae_fp,"    <material id=\"%s\" name=\"%s\">\n",material_ids[i].c_str(),material_names[i].c_str());
    std::fprintf(dae_fp,"      <instance_effect url=\"#%s\"/>\n",effect_ids[i].c_str());
    std::fprintf(dae_fp,"    </material>\n");
  }
  std::fprintf(dae_fp,"  </library_materials>\n");

  std::fprintf(dae_fp,"  <library_effects>\n");
  for (int i=0; i<nobjects; i++) {
    std::fprintf(dae_fp,"    <effect id=\"%s\" name=\"%s\">\n",effect_ids[i].c_str(),effect_ids[i].c_str());
    std::fprintf(dae_fp,"      <profile_COMMON>\n");
    std::fprintf(dae_fp,"        <newparam sid=\"%s\">\n",surface_ids[i].c_str());
    std::fprintf(dae_fp,"          <surface type=\"2D\">\n");
    std::fprintf(dae_fp,"            <init_from>%s</init_from>\n",image_names[i].c_str());
    std::fprintf(dae_fp,"          </surface>\n");
    std::fprintf(dae_fp,"        </newparam>\n");
    std::fprintf(dae_fp,"        <newparam sid=\"%s\">\n",image_sampler_ids[i].c_str());
    std::fprintf(dae_fp,"          <sampler2D>\n");
    std::fprintf(dae_fp,"            <source>%s</source>\n",surface_ids[i].c_str());
    std::fprintf(dae_fp,"          </sampler2D>\n");
    std::fprintf(dae_fp,"        </newparam>\n");
    std::fprintf(dae_fp,"        <technique sid=\"COMMON\">\n");
    std::fprintf(dae_fp,"          <phong>\n");
    std::fprintf(dae_fp,"            <emission>\n");
    std::fprintf(dae_fp,"              <color>0.0 0.0 0.0 1</color>\n");
    std::fprintf(dae_fp,"            </emission>\n");
    std::fprintf(dae_fp,"            <ambient>\n");
    std::fprintf(dae_fp,"              <color>0.0 0.0 0.0 1</color>\n");
    std::fprintf(dae_fp,"            </ambient>\n");
    std::fprintf(dae_fp,"            <diffuse>\n");
    std::fprintf(dae_fp,"              <texture texture=\"%s\" texcoord=\"UVSET0\"/>\n",image_sampler_ids[i].c_str());
    std::fprintf(dae_fp,"            </diffuse>\n");
    std::fprintf(dae_fp,"            <specular>\n");
    std::fprintf(dae_fp,"              <color>0.33 0.33 0.33 1</color>\n");
    std::fprintf(dae_fp,"            </specular>\n");
    std::fprintf(dae_fp,"            <shininess>\n");
    std::fprintf(dae_fp,"              <float>20.0</float>\n");
    std::fprintf(dae_fp,"            </shininess>\n");
    std::fprintf(dae_fp,"            <reflectivity>\n");
    std::fprintf(dae_fp,"              <float>0.1</float>\n");
    std::fprintf(dae_fp,"            </reflectivity>\n");
    std::fprintf(dae_fp,"            <transparent>\n");
    std::fprintf(dae_fp,"              <color>1 1 1 1</color>\n");
    std::fprintf(dae_fp,"            </transparent>\n");
    std::fprintf(dae_fp,"            <transparency>\n");
    std::fprintf(dae_fp,"              <float>0.0</float>\n");
    std::fprintf(dae_fp,"            </transparency>\n");
    std::fprintf(dae_fp,"          </phong>\n");
    std::fprintf(dae_fp,"        </technique>\n");
    std::fprintf(dae_fp,"      </profile_COMMON>\n");
    std::fprintf(dae_fp,"    </effect>\n");
  }
  std::fprintf(dae_fp,"  </library_effects>\n");


  std::fprintf(dae_fp,"  <library_geometries>\n");
  int idx = 0;
  for (it = objects.begin(); it != objects.end(); it++) {
    obj_observable* obj = *it;
    // assume object is texture mapped
    dbmsh3d_textured_mesh_mc* mesh = (dbmsh3d_textured_mesh_mc*)obj->get_object();
    if (mesh->facemap().size() < min_faces) {
      // single mesh face is probably ground plane, which we do not want to render
      continue;
    }
    int nverts = mesh->vertexmap().size();
    int nfaces = mesh->facemap().size();

    std::fprintf(dae_fp,"    <geometry id=\"%s\" name=\"%s\">\n",geometry_ids[idx].c_str(),geometry_ids[idx].c_str());
    std::fprintf(dae_fp,"      <mesh>\n");
    std::fprintf(dae_fp,"        <source id=\"%s\">\n",geometry_position_ids[idx].c_str());
    std::fprintf(dae_fp,"        <float_array id=\"%s\" count=\"%d\">\n"          ,geometry_position_array_ids[idx].c_str(),nverts*3);
    // map vertex ID's to indices.
    std::map<int,int> vert_indices;
    //std::map<int, dbmsh3d_vertex*>::iterator vit;
    int vert_idx = 0;
    for (vit = mesh->vertexmap().begin(); vit != mesh->vertexmap().end(); vit++, vert_idx++) {
      dbmsh3d_vertex* v = (dbmsh3d_vertex*)vit->second;
      vert_indices[v->id()] = vert_idx;
      double x,y,z;
      lvcs_->global_to_local(v->pt().x(),v->pt().y(),v->pt().z(),bgeo_lvcs::wgs84,x,y,z,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
      std::fprintf(dae_fp,"%f %f %f ",x,y,z);
    }
    std::fprintf(dae_fp,"\n        </float_array>\n");
    std::fprintf(dae_fp,"        <technique_common>\n");
    std::fprintf(dae_fp,"          <accessor source=\"#%s\" count=\"%d\" stride=\"3\">\n",geometry_position_array_ids[idx].c_str(),nverts);
    std::fprintf(dae_fp,"            <param name=\"X\" type=\"float\"/>\n");
    std::fprintf(dae_fp,"            <param name=\"Y\" type=\"float\"/>\n");
    std::fprintf(dae_fp,"            <param name=\"Z\" type=\"float\"/>\n");
    std::fprintf(dae_fp,"          </accessor>\n");
    std::fprintf(dae_fp,"        </technique_common>\n");
    std::fprintf(dae_fp,"      </source>\n");
    std::fprintf(dae_fp,"      <source id=\"%s\">\n",geometry_uv_ids[idx].c_str());
    // determine total number of corners in mesh
    int ncorners = 0;
    std::map<int, dbmsh3d_face*>::iterator fit;
    for (fit = mesh->facemap().begin(); fit!= mesh->facemap().end(); fit++) {
      dbmsh3d_textured_face_mc* face = (dbmsh3d_textured_face_mc*)fit->second;
      ncorners += face->vertices().size();
    }
    std::fprintf(dae_fp,"        <float_array id=\"%s\" count=\"%d\">\n"          ,geometry_uv_array_ids[idx].c_str(),ncorners*2);
    for (fit = mesh->facemap().begin(); fit!= mesh->facemap().end(); fit++) {
      dbmsh3d_textured_face_mc* face = (dbmsh3d_textured_face_mc*)fit->second;
      for (unsigned j=0; j<face->vertices().size(); j++) {
        dbmsh3d_vertex* v = (dbmsh3d_vertex*) face->vertices(j);
        vgl_point_2d<double> pt = face->tex_coords(v->id());
        std::fprintf(dae_fp,"%f %f ",pt.x(),pt.y());
      }
    }
    std::fprintf(dae_fp,"\n        </float_array>\n");
    std::fprintf(dae_fp,"        <technique_common>\n");
    std::fprintf(dae_fp,"          <accessor source=\"#%s\" count=\"%d\" stride=\"2\">\n",geometry_uv_array_ids[idx].c_str(),ncorners);
    std::fprintf(dae_fp,"            <param name=\"S\" type=\"float\"/>\n");
    std::fprintf(dae_fp,"            <param name=\"T\" type=\"float\"/>\n");
    std::fprintf(dae_fp,"          </accessor>\n");
    std::fprintf(dae_fp,"        </technique_common>\n");
    std::fprintf(dae_fp,"      </source>\n");

    std::fprintf(dae_fp,"      <vertices id=\"%s\">\n",geometry_vertex_ids[idx].c_str());
    std::fprintf(dae_fp,"        <input semantic=\"POSITION\" source=\"#%s\"/>\n",geometry_position_ids[idx].c_str());
    std::fprintf(dae_fp,"      </vertices>\n");
    std::fprintf(dae_fp,"      <triangles material=\"%s\" count=\"%d\">\n",material_names[idx].c_str(),nfaces);
    std::fprintf(dae_fp,"        <input semantic=\"VERTEX\" source=\"#%s\" offset=\"0\"/>\n",geometry_vertex_ids[idx].c_str());
    std::fprintf(dae_fp,"        <input semantic=\"TEXCOORD\" source=\"#%s\" offset=\"1\" set=\"0\"/>\n",geometry_uv_ids[idx].c_str());

    int tex_idx = 0;
    std::fprintf(dae_fp,"        <p>");
    for (fit = mesh->facemap().begin(); fit!= mesh->facemap().end(); fit++) {
      dbmsh3d_textured_face_mc* face = (dbmsh3d_textured_face_mc*)fit->second;

      if (face->vertices().size() != 3) {
        std::cerr << "ERROR! only triangle meshes are supported. Face has "<<face->vertices().size()<<" vertices.\n";
      }
      for (unsigned j=0; j< 3; j++) {
        dbmsh3d_vertex* v = (dbmsh3d_vertex*) face->vertices(j);
        std::fprintf(dae_fp,"%d %d ",vert_indices[v->id()],tex_idx++);
      }
    }
    std::fprintf(dae_fp,"</p>\n");
    std::fprintf(dae_fp,"      </triangles>\n");
    std::fprintf(dae_fp,"    </mesh>\n");
    std::fprintf(dae_fp,"  </geometry>\n");
    idx++;
  }
  std::fprintf(dae_fp,"</library_geometries>\n");

  std::fprintf(dae_fp,"<library_nodes>\n");
  for (int i=0; i<nobjects; i++) {
    std::fprintf(dae_fp,"  <node id=\"Component_%d\" name=\"Component_%d\">\n",i,i);
    std::fprintf(dae_fp,"    <node id=\"%s\" name=\"%s\">\n",mesh_ids[i].c_str(),mesh_ids[i].c_str());
    std::fprintf(dae_fp,"      <instance_geometry url=\"#%s\">\n",geometry_ids[i].c_str());
    std::fprintf(dae_fp,"        <bind_material>\n");
    std::fprintf(dae_fp,"          <technique_common>\n");
    std::fprintf(dae_fp,"            <instance_material symbol=\"%s\" target=\"#%s\">\n",material_names[i].c_str(),material_ids[i].c_str());
    std::fprintf(dae_fp,"              <bind_vertex_input semantic=\"UVSET0\" input_semantic=\"TEXCOORD\" input_set=\"0\"/>\n");
    std::fprintf(dae_fp,"            </instance_material>\n");
    std::fprintf(dae_fp,"          </technique_common>\n");
    std::fprintf(dae_fp,"        </bind_material>\n");
    std::fprintf(dae_fp,"      </instance_geometry>\n");
    std::fprintf(dae_fp,"    </node>\n");
    std::fprintf(dae_fp,"  </node>\n");
  }
  std::fprintf(dae_fp,"</library_nodes>\n");

  std::fprintf(dae_fp,"<library_visual_scenes>\n");
  std::fprintf(dae_fp,"  <visual_scene id=\"WorldModelerScene\" name=\"WorldModelerScene\">\n");
  std::fprintf(dae_fp,"    <node id=\"Model\" name=\"Model\">\n");
  for (int i=0; i<nobjects; i++) {
    std::fprintf(dae_fp,"      <node id=\"Component_%d_1\" name=\"Component_%d_1\">\n",i,i);
    std::fprintf(dae_fp,"        <instance_node url=\"#Component_%d\"/>\n",i);
    std::fprintf(dae_fp,"      </node>\n");
  }
  std::fprintf(dae_fp,"    </node>\n");
  std::fprintf(dae_fp,"  </visual_scene>\n");
  std::fprintf(dae_fp,"</library_visual_scenes>\n");

  std::fprintf(dae_fp,"<scene>\n");
  std::fprintf(dae_fp,"  <instance_visual_scene url=\"#WorldModelerScene\"/>\n");
  std::fprintf(dae_fp,"</scene>\n");
  std::fprintf(dae_fp,"</COLLADA>\n");

  fclose(dae_fp);

  std::ostringstream textures_fname;
  textures_fname << kmz_dir << "/textures.txt";

  FILE* tex_fp;
  if ((tex_fp = std::fopen(textures_fname.str().data(), "w")) == NULL) {
    std::fprintf (stderr, "Can't open .dae file %s to write.\n", textures_fname.str().data());
    return; 
  }
  
  for (int i=0; i<nobjects; i++) {
    std::fprintf(tex_fp,"<%s> <%s>\n",image_fnames[i].c_str(),image_fnames[i].c_str());
  }
  std::fclose(tex_fp);

  std::ostringstream kml_fname;
  kml_fname << kmz_dir << "/doc.kml";
  FILE* kml_fp;
  if ((kml_fp = std::fopen(kml_fname.str().data(), "w")) == NULL) {
    std::fprintf (stderr, "Can't open .kml file %s to write.\n", kml_fname.str().data());
    return; 
  }

  std::fprintf(kml_fp,"<?xml version='1.0' encoding='UTF-8'?>\n");
  std::fprintf(kml_fp,"<kml xmlns='http://earth.google.com/kml/2.1'>\n");
  std::fprintf(kml_fp,"<Folder>\n");
  std::fprintf(kml_fp,"  <name>%s</name>\n",model_name.c_str());
  std::fprintf(kml_fp,"  <description><![CDATA[Created with <a href=\"http://www.lems.brown.edu\">Brown University World Modeler</a>]]></description>\n");
  std::fprintf(kml_fp,"  <DocumentSource>Brown University World Modeler</DocumentSource>\n");
  std::fprintf(kml_fp,"  <visibility>1</visibility>\n");
  std::fprintf(kml_fp,"  <LookAt>\n");
  std::fprintf(kml_fp,"    <heading>0</heading>\n");
  std::fprintf(kml_fp,"    <tilt>45</tilt>\n");
  std::fprintf(kml_fp,"    <longitude>%f</longitude>\n",origin_lon);
  std::fprintf(kml_fp,"    <latitude>%f</latitude>\n",origin_lat);
  std::fprintf(kml_fp,"    <range>200</range>\n");
  std::fprintf(kml_fp,"    <altitude>%f</altitude>\n",0.0f);
  std::fprintf(kml_fp,"  </LookAt>\n");
  std::fprintf(kml_fp,"  <Folder>\n");
  std::fprintf(kml_fp,"    <name>Tour</name>\n");
  std::fprintf(kml_fp,"    <Placemark>\n");
  std::fprintf(kml_fp,"      <name>Camera</name>\n");
  std::fprintf(kml_fp,"      <visibility>1</visibility>\n");
  std::fprintf(kml_fp,"    </Placemark>\n");
  std::fprintf(kml_fp,"  </Folder>\n");
  std::fprintf(kml_fp,"  <Placemark>\n");
  std::fprintf(kml_fp,"    <name>Model</name>\n");
  std::fprintf(kml_fp,"    <description><![CDATA[]]></description>\n");
  std::fprintf(kml_fp,"    <Style id='default'>\n");
  std::fprintf(kml_fp,"    </Style>\n");
  std::fprintf(kml_fp,"    <Model>\n");
  std::fprintf(kml_fp,"      <altitudeMode>relativeToGround</altitudeMode>\n");
  std::fprintf(kml_fp,"      <Location>\n");
  std::fprintf(kml_fp,"        <longitude>%f</longitude>\n",origin_lon + lon_offset);
  std::fprintf(kml_fp,"        <latitude>%f</latitude>\n",origin_lat + lat_offset);
  std::fprintf(kml_fp,"        <altitude>%f</altitude>\n",origin_elev - ground_height);
  std::fprintf(kml_fp,"      </Location>\n");
  std::fprintf(kml_fp,"      <Orientation>\n");
  std::fprintf(kml_fp,"        <heading>0</heading>\n");
  std::fprintf(kml_fp,"        <tilt>0</tilt>\n");
  std::fprintf(kml_fp,"        <roll>0</roll>\n");
  std::fprintf(kml_fp,"      </Orientation>\n");
  std::fprintf(kml_fp,"      <Scale>\n");
  std::fprintf(kml_fp,"        <x>1.0</x>\n");
  std::fprintf(kml_fp,"        <y>1.0</y>\n");
  std::fprintf(kml_fp,"        <z>1.0</z>\n");
  std::fprintf(kml_fp,"      </Scale>\n");
  std::fprintf(kml_fp,"      <Link>\n");
  std::fprintf(kml_fp,"        <href>models/mesh.dae</href>\n");
  std::fprintf(kml_fp,"      </Link>\n");
  std::fprintf(kml_fp,"    </Model>\n");
  std::fprintf(kml_fp,"  </Placemark>\n");
  std::fprintf(kml_fp,"</Folder>\n");
  std::fprintf(kml_fp,"</kml>\n");

  std::fclose(kml_fp);

}


void poly_manager::save_kml()
{

  vgui_dialog params("File Save");
  std::string ext, kml_filename, empty="";
  std::string model_name;
  double ground_height = 0.0;
  double x_offset = 0.0;
  double y_offset = 0.0;

  params.field("model name",model_name);
  params.field("ground height",ground_height);
  params.field("x offset",x_offset);
  params.field("y offset",y_offset);

  params.file("Save...",ext,kml_filename);
  if (!params.ask())
    return;

  if (kml_filename == "") {
    std::cerr << "Error: no filename selected.\n";
    return;
  }

  FILE* fp;
  if ((fp = std::fopen(kml_filename.c_str(), "w")) == NULL) {
    std::fprintf (stderr, "Can't open xml file %s to write.\n", kml_filename.c_str());
    return; 
  }

  std::fprintf (fp, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
  std::fprintf (fp, "<kml xmlns=\"http://earth.google.com/kml/2.1\">\n");
  std::fprintf (fp, "<Document>\n");
  std::fprintf (fp, "  <name>%s</name>\n",vul_file::strip_directory(kml_filename.c_str()));
  std::fprintf (fp, "  <open>1</open>\n");
  std::fprintf (fp, "  <Placemark>\n");
  std::fprintf (fp, "    <name>%s</name>\n",model_name.c_str());
  std::fprintf (fp, "    <visibility>1</visibility>\n");
  
    std::fprintf (fp, "    <MultiGeometry>\n");

  std::list<obj_observable*>::iterator it;
  int obj_count = 0;
  for (it = objects.begin(); it != objects.end(); it++, obj_count++) {
    obj_observable* obj = *it;
    // assume object is texture mapped
    dbmsh3d_textured_mesh_mc* mesh = (dbmsh3d_textured_mesh_mc*)obj->get_object();

    mesh->build_IFS_mesh();
    mesh->IFS_to_MHE();
    mesh->orient_face_normals();
    mesh->build_IFS_mesh();



  
    int poly_count = 0;
    if (mesh->facemap().size() <= 1) {
      // single mesh face is probably ground plane, which we do not want to render
      continue;
    }
    std::map<int, dbmsh3d_face*>::iterator fit;
    for (fit = mesh->facemap().begin(); fit!= mesh->facemap().end(); fit++) {
      dbmsh3d_textured_face_mc* face = (dbmsh3d_textured_face_mc*)fit->second;

      std::fprintf(fp, "      <Polygon id=\"building%d_face%d\">\n",obj_count,poly_count);
      std::fprintf(fp, "        <extrude>0</extrude>\n");
      std::fprintf(fp, "        <tessellate>0</tessellate>\n");
      std::fprintf(fp, "        <altitudeMode>relativeToGround</altitudeMode>\n");
      std::fprintf(fp, "        <outerBoundaryIs>\n");
      std::fprintf(fp, "          <LinearRing>\n");
      std::fprintf(fp, "            <coordinates>\n");
    
      for (unsigned j=0; j<face->vertices().size(); j++) {
        dbmsh3d_vertex* v = (dbmsh3d_vertex*) face->vertices(j);
        std::fprintf (fp, "             %.16f, ", v->pt().x()+x_offset);
        std::fprintf (fp, "%.16f, ", v->pt().y()+y_offset);
        std::fprintf (fp, "%.16f \n", v->pt().z() - ground_height);
      }
      //Now print the first vertex again to close the polygon
      dbmsh3d_vertex* v = (dbmsh3d_vertex*) face->vertices(0);  
      std::fprintf (fp, "             %.16f, ", v->pt().x()+x_offset);
      std::fprintf (fp, "%.16f, ", v->pt().y()+y_offset);
      std::fprintf (fp, "%.16f \n", v->pt().z() - ground_height);

      std::fprintf(fp, "            </coordinates>\n");
      std::fprintf(fp, "          </LinearRing>\n");
      std::fprintf(fp, "        </outerBoundaryIs>\n");
      std::fprintf(fp, "      </Polygon>\n");
    }
   
  }
   std::fprintf(fp, "    </MultiGeometry>\n");
  std::fprintf(fp, "  </Placemark>\n");
  std::fprintf(fp, "</Document>\n");
  std::fprintf(fp, "</kml>\n");

  std::fclose (fp);
}

void poly_manager::generate_textures()
{
  if (!lvcs_) {
    std::cerr << "Error: must define LVCS coordinate system before generating tex coords.\n";
    return;
  }
  vgui_dialog params("Texture Map filename");
  std::string ext, tex_filename, empty = "";
  params.file("Texture map...",ext,tex_filename);

  if (!params.ask())
    return;

  std::vector<poly_cam_observer*> observers;
  observers.push_back(observer_left_);
  observers.push_back(observer_right_);
  texture_map_generator tex_generator(observers);

  // each object independantly for now.
  std::list<obj_observable*>::iterator it;
  int mesh_idx = 0;
  for (it = objects.begin(); it != objects.end(); it++, mesh_idx++) {
    obj_observable* o = *it;

    // orient faces
    dbmsh3d_mesh_mc* mesh = o->get_object();

    std::ostringstream tex_fullpath;
    tex_fullpath << tex_filename << "." << mesh_idx << ".jpg";
    tex_generator.generate_texture_map(o,tex_fullpath.str(),*lvcs_);
  }
  return;
}

void poly_manager::save_all()
{
  vgui_dialog params("File Save");
  std::string ext, list_name, empty="";

  params.file ("Filename...", ext, list_name);  
  bool use_lvcs = false;
  params.checkbox("use lvcs",use_lvcs);

  if (!params.ask())
    return;

  if (list_name == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify a filename." );
    error.ask();
    return;
  }

  std::string directory_name = vul_file::dirname(list_name);

  std::ofstream list_out(list_name.data());
  if (!list_out.good()) {
    std::cerr << "error opening file "<< list_name <<std::endl;
    return;
  }

  std::list<obj_observable*>::iterator it = objects.begin();
  int mesh_idx = 0;
  while (it != objects.end()) {
    std::ostringstream meshname;
    std::ostringstream fullpath;
    meshname << "mesh" << mesh_idx <<".ply2";
    fullpath << directory_name << "/" << meshname.str();

    list_out << meshname.str() << std::endl;
    obj_observable* o = *it;

    // generate texture map
    //if (do_tex) {
    //  std::ostringstream tex_fullpath;
    //  tex_fullpath << tex_filename << "." << mesh_idx << ".tiff";
    //  tex_generator.generate_texture_map(o,tex_fullpath.str());
    //}
    dbmsh3d_mesh_mc* mesh = o->get_object();
    save_mesh(mesh,fullpath.str().data(),use_lvcs);
    it++;
    mesh_idx++;
  }
}

void poly_manager::save_lvcs()
{
  std::string filename = select_file();
  // just save origin for now
  std::ofstream os(filename.data());
  double lat,lon,elev;
  lvcs_->get_origin(lat,lon,elev);
  os.precision(12);
  os << lat << " "<< lon << " " << elev << std::endl;

  return;
}

void poly_manager::load_lvcs()
{
  std::string filename = select_file();
  // just load origin for now
  std::ifstream is(filename.data());
  double lat, lon, elev;
  is >> lat;
  is >> lon;
  is >> elev;

  lvcs_ = new bgeo_lvcs(lat,lon,elev,bgeo_lvcs::wgs84,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
  std::cout << "loaded lvcs with origin "<<lat<<", "<<lon<<", "<<elev<<std::endl;

  return;
}

void poly_manager::convert_file_to_lvcs()
{
  if (!lvcs_) {
    std::cerr << "error: lvcs is not defined!"<<std::endl;
    return;
  }
  // expects simple text file with each line being of the form "lat lon z"
  std::string filename_in = select_file();
  std::string filename_out = filename_in + ".lvcs";
  std::ifstream is(filename_in.data());
  std::ofstream os(filename_out.data());
  
  double lat,lon,elev;
  double x,y,z;

  while (!is.eof()) {
    is >> lat;
    is >> lon;
    is >> elev;
    lvcs_->global_to_local(lon,lat,elev,bgeo_lvcs::wgs84,x,y,z,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
    os << x <<" "<<y<<" "<<z<<std::endl;
  }

  return;
}

void poly_manager::save_mesh(dbmsh3d_mesh_mc* mesh, const char* filename, bool use_lvcs)
{
  mesh->build_IFS_mesh();
  dbmsh3d_mesh_mc* mesh2 = mesh->clone();

  if (use_lvcs) {
    if (!lvcs_){
      std::cerr << "error: lvcs == null" <<std::endl;
      return;
    }
    std::map<int, dbmsh3d_vertex*>::iterator it = mesh2->vertexmap().begin();
    for (; it != mesh2->vertexmap().end(); it++) {
      dbmsh3d_vertex* V = (dbmsh3d_vertex*) (*it).second;
      double x=0,y=0,z=0;
      lvcs_->global_to_local(V->pt().x(),V->pt().y(),V->pt().z(),bgeo_lvcs::wgs84,x,y,z,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
      vgl_point_3d<double> new_pt(x,y,z);
      std::cout << "converted global <"<<V->pt().x() <<", "<< V->pt().y() <<", "<< V->pt().z() <<"> to <" <<x<< ", "<<y<<" ,"<<z<<"> "<<std::endl;
      V->set_pt(new_pt);
    }
  }
  mesh2->IFS_to_MHE();
  mesh2->orient_face_normals();
  mesh2->build_IFS_mesh();
  dbmsh3d_save_ply2(mesh2, filename);
  return;
}


void poly_manager::load_mesh_single()
{
  std::string filename = select_file();
  load_mesh(filename);
  return;
}

void poly_manager::load_mesh_multiple()
{
  // read txt file
  std::string master_filename = select_file();
  std::ifstream file_inp(master_filename.data());
  if (!file_inp.good()) {
    std::cerr << "error opening file "<< master_filename <<std::endl;
    return;
  }

  std::string directory_name = vul_file::dirname(master_filename);
  while(!file_inp.eof()){
    std::ostringstream fullpath;
    std::string mesh_fname;
    file_inp >> mesh_fname;
    if (!mesh_fname.empty() && (mesh_fname[0] != '#')) {
      fullpath << directory_name << "/" << mesh_fname;
      load_mesh(fullpath.str());
    }
  }
  file_inp.close();

  return;
}

void poly_manager::load_mesh(std::string filename)
{
  dbmsh3d_mesh_mc mesh_mc;

  if (!dbmsh3d_load_ply2(&mesh_mc,filename.data())) {
    std::cerr << "Error loading mesh "<<filename<< std::endl;
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
void poly_manager::define_lvcs()
{
  float x1,y1;
  left_->pick_point(&x1,&y1);
  vsol_point_2d_sptr img_point = new vsol_point_2d(x1,y1);

  std::vector<vsol_point_2d_sptr> points2d;
  std::vector<vsol_point_3d_sptr> points3d;
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
  std::cout << "defining lvcs with origin = <"<<origin_poly3d->vertex(0)->x() <<", "<<origin_poly3d->vertex(0)->y() <<", "<<origin_poly3d->vertex(0)->z() <<">"<<std::endl;
}
  
//: Combines the the first and the last points to create a polygon
// on the screen
void poly_manager::create_poly()
{
  vsol_polygon_2d_sptr poly;
  left_->set_color(1, 0, 0);
  left_->pick_polygon(poly);
  if(!poly)
  {std::cout << "Null polygon - no creation in world\n";
  return;
  }
  poly2d = poly->cast_to_polygon();
  unsigned n = poly2d->size();
  // make sure that the last two vertices of the polygon are not the same
  if (*(poly2d->vertex(n-1)) == *(poly2d->vertex(n-2))) {
    std::vector<vsol_point_2d_sptr> vertices;
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

void poly_manager::extrude_poly()
{
  obj_observable* obs=0;
  unsigned face_id;

  get_selection(obs, face_id);
  if (obs) {
    obs->extrude(face_id);
    my_obj = obs;
  }
}

void poly_manager::draw_line() 
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

void poly_manager::label_roof() 
{
  obj_observable* obs=0;
  unsigned face_id;
  get_selection(obs, face_id);
  obs->label_roof(face_id);
}

void poly_manager::label_wall() 
{
  obj_observable* obs=0;
  unsigned face_id;
  get_selection(obs, face_id);
  obs->label_wall(face_id);
}
void poly_manager::create_inner_face() 
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

void poly_manager::create_interior() 
{
  obj_observable* obs=0;
  unsigned face_id;
  get_selection(obs, face_id);
  obs->create_interior();
}
void poly_manager::get_selection(obj_observable* &obs, unsigned &face_id)
{

  std::vector<vgui_soview*> select_list = observer_left_->get_selected_soviews();
  if (select_list.size() == 0 || select_list.size() > 1) {
    std::cerr << "Select only ONE face" << std::endl;
  } else {
    unsigned sel_id = select_list[0]->get_id();
    vgui_soview2D_polygon* polygon = static_cast<vgui_soview2D_polygon *> (select_list[0]);
    observer_left_->deselect_all(); //>deselect(sel_id);

    // find the 3D polygon with this ID
    vsol_polygon_3d_sptr poly3d;
    obs = find_polygon_in_3D(sel_id, poly3d, face_id);
  }
}
void poly_manager::deselect_all()
{
  observer_left_->deselect_all();
  observer_right_->deselect_all();
}

void poly_manager::set_selected_vertex()
{
  //Now find out which polygon is selected (if any)
  std::vector<vgui_soview*> select_list = observer_left_->get_selected_soviews();
  //Check if there is at least one selected polygon
  if (select_list.size() > 0) {

      unsigned list_index = 0;
      bool found = false;
      for(std::vector<vgui_soview*>::iterator sit = select_list.begin();
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
      std::cout << "Is a face selected?\n";
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
      std::cout << "Is a face selected?\n";
  return;
 }
    unsigned vindex =observer_left_->get_selected_3d_vertex_index(id);
    if(vindex<poly3d->size())
      selected_vertex_ = poly3d->vertex(vindex);
    else
      selected_vertex_ = (vsol_point_3d*)0;
  }
  else
    selected_vertex_ = (vsol_point_3d*)0;
}

void poly_manager::print_selected_vertex()
{
  this->set_selected_vertex();
  if(!selected_vertex_)
    return;
  std::cout.precision(10);
    std::cout << "Lat: " << selected_vertex_->y() << ' '
             << "Lon: " << selected_vertex_->x() << ' '
             << "Elv: " << selected_vertex_->z() << '\n';
}
void poly_manager::set_lvcs_at_selected_vertex()
{
  this->set_selected_vertex();
  if(!selected_vertex_)
    return;
  lvcs_ = new bgeo_lvcs(selected_vertex_->y(),selected_vertex_->x(),selected_vertex_->z(),
                        bgeo_lvcs::wgs84,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
  std::cout << "defining lvcs with origin = <"<<selected_vertex_->x() <<", "<<selected_vertex_->y() <<", "<<selected_vertex_->z() <<">"<<std::endl;
}

void poly_manager::clear_poly()
{
  obj_observable* obs=0;
  unsigned face_id;
  get_selection(obs, face_id);
  if (obs) {
    delete_observable(obs);
  }
}

void poly_manager::clear_all()
{
  std::list<obj_observable*>::iterator it = objects.begin();
  while (it != objects.end()) {
    delete (*it);
    it++;
  }
  objects.clear();
}

bool poly_manager::handle(const vgui_event &e) 
{
    //Events report the position of the mouse in screen coordinates.
    //The projection inspector transforms from screen coordinates to image
    //coordinates. 
    vgui_projection_inspector pi;
    //std::cout << "poly_manager::handle() - " << e.type << " " << e.modifier << std::endl; //X=" << e.wx << " Y=" << e.wy << std::endl;
    //Here the case of pressing the left mouse button with the shift key down
    //is checked
    if (e.type == vgui_BUTTON_DOWN && e.button == vgui_LEFT && e.modifier == vgui_SHIFT) {
      //set the state as left button operating
      left_button_down = true;
      // take the position of the point when the left button is first pressed
      //  as the start position
      pi.window_to_image_coordinates(e.wx, e.wy, mouse_start_x, mouse_start_y);
      //std::cout << "1............................... "  << mouse_start_x << " " << mouse_start_x << std::endl;

      //Now find out which polygon is selected (if any)
      std::vector<vgui_soview*> select_list = observer_left_->get_selected_soviews();
      //std::cout << select_list.size();
      //Check if there is at least one selected polygon
      if (select_list.size() > 0) {
        // TODO: if there are more than one polygon, move all of them
        //select_list[0]->print(std::cout);
        //Here we get the first polygon, no matter how many are selected
        unsigned list_index = 0;
        bool found = false;
        for(std::vector<vgui_soview*>::iterator sit = select_list.begin();
            sit!= select_list.end(); ++sit,++list_index)
          if((*sit)->type_name()!="vgui_soview2D_polygon")
            continue;
          else{
            found = true;
            break;
          }
        if(!found) return true;
        unsigned id = select_list[list_index]->get_id();
        vgui_soview2D_polygon* polygon = static_cast<vgui_soview2D_polygon *> (select_list[0]);
        //deselect the polygon since we have it 
        observer_left_->deselect(id);
        vsol_polygon_3d_sptr poly3d;
        unsigned face_id;
        //Get the 3d polygon which has been projected onto the construction
        //plane 
        observable *obs = find_polygon_in_3D(id, poly3d, face_id);
        //and make it an observable, i.e. can be watched in 
        //multiple images at the same time
        my_polygon_copy = new obj_observable();
        my_polygon_copy->attach(observer_right_);
        my_polygon_copy->attach(observer_left_);
        my_polygon_copy->set_object(poly3d);
        observer_left_->proj_poly(poly3d,poly2d);

        vgl_homg_plane_3d<double> poly_plane(vgl_homg_point_3d<double> (poly3d->vertex(0)->x(), poly3d->vertex(0)->y(), poly3d->vertex(0)->z()), 
                                        vgl_homg_point_3d<double> (poly3d->vertex(1)->x(), poly3d->vertex(1)->y(), poly3d->vertex(1)->z()),
                                        vgl_homg_point_3d<double> (poly3d->vertex(2)->x(), poly3d->vertex(2)->y(), poly3d->vertex(2)->z()));
        vgl_vector_3d<double> normal(poly_plane.a(),poly_plane.b(),poly_plane.c());
        double normlen = normal.length();
        poly_plane.set(poly_plane.a()/normlen, poly_plane.b()/normlen, poly_plane.c()/normlen, poly_plane.d()/normlen);
        observer_left_->set_proj_plane(poly_plane);
      }
      //report that the event has been handled
      return true;
    } 
    //if the mouse is moving and the left button is down and the shift key is
    //pressed 
    else if (e.type == vgui_MOTION && left_button_down == true && e.modifier == vgui_SHIFT) {
      //std::cout << "3..............................." << std::endl;
      // update these only if there is motion event
      //If there is a polygon to move
      if (my_polygon_copy != 0) {
        float wx = e.wx;
        float wy = e.wy;     
        //transform the mouse position
        pi.window_to_image_coordinates(e.wx, e.wy, cur_pointx, cur_pointy);
        //get the amount the mouse has moved in the y direction
        //the sensitivity factor is 0.5 this could be changed to give 
        //more more or less world motion for a given mouse motion
        double diff = (cur_pointy - mouse_start_y)*0.5;

        //std::cout << diff << std::endl;
        //Actually move the polygon by moving the construction plane 
        //along its normal by diff and backproject the 2-d polygon along
        //the camera ray.
        vsol_polygon_3d_sptr poly3d;
        observer_left_->backproj_poly(poly2d, poly3d, diff);
        my_polygon_copy->move(poly3d);
      }
      //mark the event as used
      return true;
    }
    //Or if the shift key is pressed and the left button comes up
    else if (e.type == vgui_BUTTON_UP && e.button == vgui_LEFT && e.modifier == vgui_SHIFT){
      left_button_down = false;
      //if the polygon exists then replace the original polygon
      //with the moved polygon and make it permanent
      if (my_polygon_copy) {
        delete_observable(my_polygon);
        my_polygon = my_polygon_copy;
        my_polygon_copy = 0;
        objects.push_back(my_polygon);
        my_polygon->attach(observer_left_);
        //print the vertices of the polygon
        vsol_polygon_3d_sptr poly3d = my_polygon->extract_face(0);
        std::cout << "moving poly verts\n";
        for(unsigned i = 0; i<poly3d->size(); ++i)
          poly3d->vertex(i)->describe(std::cout, 3);
      }
      //mark the event as used
      return true;
    } 
    //if the middle mouse button is down along with the shift key
    else if (e.type == vgui_BUTTON_DOWN && e.button == vgui_MIDDLE && e.modifier == vgui_SHIFT) {
      //set the state as middle button operating
      middle_button_down = true;
      //set the start x and y 
      pi.window_to_image_coordinates(e.wx, e.wy, mouse_start_x, mouse_start_y);
      //mark the event as used
      return true;
    }
    //If there is mouse motion and we are in the middle button state
    // and the shift key is pressed
    else if (e.type == vgui_MOTION && middle_button_down == true && e.modifier == vgui_SHIFT) {
      // update these only if there is motion event
      float wx = e.wx;
      float wy = e.wy;
      //convert to image coordinates
      pi.window_to_image_coordinates(e.wx, e.wy, cur_pointx, cur_pointy);
      //get the difference in mouse position from the start
      double diff = -1*(mouse_start_y - cur_pointy)*0.5;
      std::cout << mouse_start_y  << "-" << cur_pointy << "=" << diff << std::endl;
      //reset the start point. This strategy could be used in the 
      //move along optical ray section.
      mouse_start_y = cur_pointy;
      //extrude the face
      if (my_obj != 0)
        my_obj->move_extr_face(diff);
      return true;
    } 
    //If the middle button is raised with the shift key pressed
    else if (e.type == vgui_BUTTON_UP && e.button == vgui_MIDDLE && e.modifier == vgui_SHIFT){
      //std::cout << "2..............................." << std::endl;
      //leave the middle button state
      middle_button_down = false;
      //mark the event as used
      return true;
    }
 
    // otherwise we are not interested in other events,
    // so pass event to base class:
    return vgui_wrapper_tableau::handle(e);  
}

//////////////////////////////////////////////////////////////////////////////////
// Private Methods
std::string poly_manager::select_file()
{
  vgui_dialog params ("File Open");
  std::string ext, file, empty="";

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

vpgl_camera<double>* poly_manager::select_camera()
{
  vgui_dialog params ("Camera File Open");
  std::string ext, file, empty="";
  std::vector<std::string> camera_types;
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
    camera = new vpgl_rational_camera<double>(file);
    break;
  default:
    std::cout << "Error: unknown camera type "<<camera_type<< std::endl;
  }
  return camera;
}

void poly_manager::read_world_points(std::string fname)
{
  std::ifstream points_file(fname.data());
  int x, y, z;
  while (!points_file.eof()) {
    points_file >> x;
    points_file >> y;
    points_file >> z;
    std::cout << x << " " << y << " " << z << std::endl;
    vgl_point_3d<double> p(x, y, z);
    //world_points_.push_back(p);
  }
}
  
obj_observable* poly_manager::find_polygon_in_3D(unsigned id, 
                                                 vsol_polygon_3d_sptr& poly,
                                                 unsigned& index)
{ 
  std::list<obj_observable*>::iterator it = objects.begin();
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

void poly_manager::delete_observable(observable* obs)
{
  std::list<obj_observable*>::iterator it = objects.begin();
  while (it != objects.end()) {
    if (*it == obs) {
      delete (*it);
      objects.erase(it);
      return;
    }
    it++;
  }
}

vpgl_proj_camera<double>
poly_manager::read_projective_camera(std::string cam_path){

  std::ifstream cam_stream(cam_path.data());
  vpgl_proj_camera<double> cam;
  cam_stream >> cam;
  std::cout << cam << std::endl;
  
  return cam;
}

