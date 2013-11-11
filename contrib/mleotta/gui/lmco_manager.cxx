// This is contrib/mleotta/gui/lmco_manager.cxx
#include "lmco_manager.h"
//:
// \file
// \author Matt Leotta

#include "lmco_compute_mi.h"
#include <vcl_sstream.h>
#include <bgui/bgui_image_tableau.h>
#include <bgui3d/bgui3d_project2d_tableau.h>
#include <bgui3d/bgui3d_examiner_tableau.h>
#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_error_dialog.h>
#include <vgui/vgui_range_map_params.h>
#include <vgui/vgui_composite_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_projection_inspector.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vil/vil_copy.h>
#include <vil/vil_plane.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/algo/vil_orientations.h>
#include <vil/algo/vil_gauss_reduce.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_math.h>
#include <imesh/imesh_mesh.h>
#include <imesh/imesh_fileio.h>
#include <imesh/algo/imesh_project.h>
#include <vnl/algo/vnl_amoeba.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>

#include <vcl_fstream.h>

class mesh_placer_tableau : public vgui_tableau
{
  public:
  enum mode_t {NONE, POSITION, ANGLE};
  
  mesh_placer_tableau(lmco_manager::view* v) : view(v), mode(NONE), 
                                               live_mi(false), draw_tex(false) {}

  bool handle(const vgui_event &e)
  {
    if(e.type == vgui_DRAW && draw_tex){
      vgui_soview2D_image *vi = lmco_manager::instance()->build_texture_image(view,0);
      if(vi)
        vi->draw();
      delete vi;
      return true;
    }
    
    if(e.type == vgui_KEY_PRESS && e.key == 'p')
    {
      mode = POSITION;
      return true;
    }    
    if(e.type == vgui_KEY_PRESS && e.key == 'a')
    {
      mode = ANGLE;
      return true;
    }  
    if(e.type == vgui_BUTTON_DOWN && mode != NONE)
    {
      mode = NONE;
      return true;
    }
    if(e.type == vgui_KEY_PRESS && e.key == 'm')
    {
      live_mi = !live_mi;
      return true;
    } 
    if(e.type == vgui_KEY_PRESS && e.key == 't')
    {
      draw_tex = !draw_tex;
      return true;
    } 
    if(e.type == vgui_MOUSE_MOTION && mode != NONE){
      vgui_projection_inspector p_insp;
      float ix, iy;
      p_insp.window_to_image_coordinates(e.wx, e.wy,ix, iy);
      double gx,gy, x,y,a;
      view->image_to_ground(ix,iy,gx,gy);
      if(mode == POSITION){
        view->set_pos(gx,gy);
      }
      x = view->pos_x();
      y = view->pos_y();
      if(mode == ANGLE){
        double dx = gx - x, dy = gy - y;
        view->set_angle(vcl_atan2(dy,dx));
      }
      post_redraw();
      a = view->angle();
      if(live_mi){
        double mi = lmco_manager::instance()->compute_mi();
        vgui::out << "MI = " << mi << "\n";
      }
      else
        vgui::out << "Position: "<< x<<", "<<y<<" angle "<< a <<"\n";
      return true;
    }
    return false;
  }
  
  lmco_manager::view* view;

  mode_t mode; 
  bool live_mi;
  bool draw_tex;
};

//=============================================================================

lmco_manager::view::view()
{
  img_to_gnd_.set_identity();

  mesh_xform_ = new SoTransform;
  mesh_xform_->ref();
  
  display_mode_ = ORIG;
  data_scale_ = 1.0;
  
  cam_ = vpgl_perspective_camera<double>();
  
  material_ = new SoMaterial;
  material_->ref();
  material_->ambientColor.set1Value(0, SbColor(1, 0, 0));
  material_->transparency.setValue(0.5);

  set_scale(3.2808399); // convert meters to feet
  set_pos(0.0,0.0);
  set_angle(0.0);

  mesh_xform_ = new SoTransform;
  mesh_xform_->ref();
  mesh_xform_->scaleFactor.setValue(mesh_scale_,mesh_scale_,mesh_scale_);
  mesh_xform_->translation.setValue(mesh_pos_x_,mesh_pos_y_,0);
  
  itab_ = bgui_image_tableau_new();
  ptab_ = bgui3d_project2d_tableau_new(cam_, new SoSeparator);
}

vgui_tableau_sptr lmco_manager::view::tableau_root()
{
  vgui_tableau_sptr placer = new mesh_placer_tableau(this);
  return vgui_composite_tableau_new(itab_, ptab_, placer);
}


void lmco_manager::view::set_pos(double x, double y)
{
  mesh_pos_x_ = x;
  mesh_pos_y_ = y;
  mesh_xform_->translation.setValue(mesh_pos_x_,mesh_pos_y_,0);
}

void lmco_manager::view::set_scale(double s)
{
  mesh_scale_ = s;
  mesh_xform_->scaleFactor.setValue(mesh_scale_,mesh_scale_,mesh_scale_);
}

void lmco_manager::view::set_angle(double a)
{
  mesh_angle_ = a;
  mesh_xform_->rotation.setValue(SbVec3f(0, 0, 1), mesh_angle_);
}


//: map image points onto the ground plane
void lmco_manager::view::image_to_ground(double ix, double iy, double& gx, double& gy) const
{
  vnl_double_3 i(ix, iy, 1);
  vnl_double_3 g = img_to_gnd_*i;
  gx = g[0]/g[2];
  gy = g[1]/g[2];
}

void lmco_manager::view::set_camera(const vpgl_perspective_camera<double>& cam)
{
  cam_ = cam;
  
  bool use_scale = display_mode_ != ORIG;
  vpgl_perspective_camera<double> P = camera(use_scale);
  vnl_double_3x3 tmp = P.get_matrix().extract(3,3);
  tmp.set_column(2,P.get_matrix().get_column(3));
  if(vnl_det(tmp) != 0.0)
    img_to_gnd_ = vnl_inverse(tmp);
  
  ptab_->set_camera(P);
}

vpgl_perspective_camera<double> 
lmco_manager::view::camera(bool use_scale) const 
{ 
  if(use_scale){
    vpgl_perspective_camera<double> cam(cam_);
    vpgl_calibration_matrix<double> K = cam.get_calibration();
    K.set_x_scale(K.x_scale()/data_scale_);
    K.set_y_scale(K.y_scale()/data_scale_);
    vgl_point_2d<double> pp = K.principal_point();
    K.set_principal_point(vgl_point_2d<double>(pp.x()/data_scale_, 
                                               pp.y()/data_scale_));
    K.set_skew(K.skew()/data_scale_);
    cam.set_calibration(K);
    return cam; 
  }
  return cam_;
}

void lmco_manager::view::set_mesh(const imesh_mesh& mesh)
{
  SoNode * root = build_SoNode(mesh);
  ptab_->set_scene_root(root);
  ptab_->post_redraw();
  root->unref();
}


void lmco_manager::view::set_display_mode(display_img_t mode)
{ 
  display_mode_ = mode;
  
  if(!img_)
    return;
  
  vil_image_view<float> tmp;
  
  switch(display_mode_){
    case ORIG:
    {
      vgui_range_map_params_sptr rmps = lmco_manager::range_params(img_->get_view());
      itab_->set_image_resource(img_, rmps);
      set_camera(cam_);
      return;
    }
    case EO_INT:
      tmp = vil_plane(eo_data_,0);
      break;
    case EO_ORT:
      tmp = vil_plane(eo_data_,1);
      break;
    case EO_MAG:
      tmp = vil_plane(eo_data_,2);
      break;
    case IR_INT:
      tmp = vil_plane(ir_data_,0);
      break;
    case IR_ORT:
      tmp = vil_plane(ir_data_,1);
      break;
    case IR_MAG:
      tmp = vil_plane(ir_data_,2);
      break;
    default:
      return;
  }
  vil_image_view_base_sptr tmp_ptr = new vil_image_view<float>(tmp);
  vgui_range_map_params_sptr rmps = lmco_manager::range_params(tmp_ptr);
  itab_->set_image_view(tmp,rmps);
  set_camera(cam_);
}


void lmco_manager::view::set_image(const vil_image_resource_sptr& img, 
                                   double scale, bool rgb_to_eo)
{
  img_ = img;
  if(scale < 1.0)
    scale = 1.0;
  data_scale_ = scale;
  
  unsigned int ni = img->ni(), nj = img->nj();
  
  if(img && rgb_to_eo){
    //convert to double
    vil_image_view<float> rgb_img = vil_convert_cast(float(), img->get_view());

    // convert rgb to grey EO
    vil_image_view<float> eo_int(ni, nj);
    vil_convert_planes_to_grey(rgb_img, eo_int); 
    
    vil_convert_stretch_range(eo_int, eo_int, 0.0f, 1.0f); 
    
    vil_image_view<float> worka, workb;
    while(scale > 1.0){
      vil_image_view<float> dst;
      double s = (scale > 2.0)?2.0:scale;
      vil_gauss_reduce_general(eo_int, dst, worka, workb, 
                               vil_gauss_reduce_params(s) );
      scale /= s;
      eo_int = dst;
    }
    
    ni = eo_int.ni();
    nj = eo_int.nj();
    
    // resize EO data
    eo_data_ = vil_image_view<float>(ni, nj, 3);
    vil_plane(eo_data_,0).deep_copy(eo_int);
    
    // compute gradients
    compute_data_image(eo_data_);
    
    // copy EO data into IR data
    ir_data_.deep_copy(eo_data_);
  }
  
  else if(img){
    //convert to double
    vil_image_view<float> multi_img = vil_convert_cast(float(), img->get_view());
    
    vil_image_view<float> worka, workb;
    while(scale > 1.0){
      vil_image_view<float> dst;
      double s = (scale > 2.0)?2.0:scale;
      vil_gauss_reduce_general(multi_img, dst, worka, workb, 
                               vil_gauss_reduce_params(s) );
      scale /= s;
      multi_img = dst;
    }
    
    ni = multi_img.ni();
    nj = multi_img.nj();
    
    // resize EO and IR data
    eo_data_ = vil_image_view<float>(ni, nj, 3);
    ir_data_ = vil_image_view<float>(ni, nj, 3);
    
    vil_image_view<float> eo0 = vil_plane(eo_data_,0);
    vil_image_view<float> ir0 = vil_plane(ir_data_,0);
    vil_convert_stretch_range(vil_plane(multi_img,1), eo0, 0.0f, 1.0f);
    vil_convert_stretch_range(vil_plane(multi_img,0), ir0, 0.0f, 1.0f); 
    
    // compute gradients
    compute_data_image(eo_data_);
    compute_data_image(ir_data_);
  }

  set_display_mode(display_mode_);
}

//: compute gradient orientation (plane 1) and magnitude (plane 2)
// from intensity data in plane 0
void lmco_manager::view::compute_data_image(vil_image_view<float>& data_img)
{
  assert(data_img.nplanes() == 3);
  unsigned int ni = data_img.ni(), nj = data_img.nj();

  // compute gradients
  vil_image_view<float> grad_i(ni,nj), grad_j(ni,nj);
  vil_sobel_3x3(vil_plane(data_img,0), grad_i, grad_j);
  vil_image_view<float> orient = vil_plane(data_img, 1);
  vil_image_view<float> mag = vil_plane(data_img, 2);
  vil_orientations(grad_i,grad_j, orient,mag);
  const float pi2 = static_cast<float>(vnl_math::pi_over_2 + 1e-8);
  vil_convert_stretch_range_limited(orient, orient, 
                                    -pi2, pi2, 0.0f, 1.0f);
}


void lmco_manager::view::set_style(double r, double g, double b, double a)
{
  material_->ambientColor.set1Value(0, SbColor(r, g, b));
  material_->transparency.setValue(a);
}

void lmco_manager::view::get_style(double& r, double& g, double& b, double& a)
{
  r = material_->ambientColor[0][0];
  g = material_->ambientColor[0][1];
  b = material_->ambientColor[0][2];
  a = material_->transparency[0];
}


vgl_h_matrix_3d<double> lmco_manager::view::homography() const
{
  vgl_h_matrix_3d<double> h;
  h.set_identity();
  h.set_rotation_euler(0,0,mesh_angle_);
  h.set_translation(mesh_pos_x_, mesh_pos_y_, 0);
  h.set(2,2,mesh_scale_);
  h.set(0,0,mesh_scale_*h.get(0,0));
  h.set(0,1,mesh_scale_*h.get(0,1));
  h.set(1,0,mesh_scale_*h.get(1,0));
  h.set(1,1,mesh_scale_*h.get(1,1));
  return h;
}


SoNode* lmco_manager::view::build_SoNode(const imesh_mesh& mesh)
{
  SoSeparator *root = new SoSeparator;
  root->ref();

  root->addChild(mesh_xform_);

  SoSeparator *group = new SoSeparator;
  root->addChild(group);
  
  SoShapeHints * hints = new SoShapeHints;
  group->addChild( hints );
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  hints->shapeType = SoShapeHints::SOLID;
  hints->faceType = SoShapeHints::CONVEX;


  group->addChild( material_ );

  // coordset
  SoCoordinate3 * coords = new SoCoordinate3;
  group->addChild( coords );
  typedef imesh_vertex_array<3>::const_iterator vitr;
  const imesh_vertex_array<3>& verts3d = mesh.vertices<3>();
  unsigned int idx = 0;
  for(vitr v = verts3d.begin(); v!=verts3d.end(); ++v)
  {
    coords->point.set1Value(idx++, SbVec3f((*v)[0], (*v)[1], (*v)[2]));
  }

  // indexed face set
  SoIndexedFaceSet * ifs = new SoIndexedFaceSet;
  group->addChild( ifs );
  const imesh_face_array_base& faces = mesh.faces();
  idx = 0;
  for(unsigned int f=0; f<faces.size(); ++f)
  {
    for(unsigned int i=0; i<faces.num_verts(f); ++i)
      ifs->coordIndex.set1Value(idx++, faces(f,i));
    ifs->coordIndex.set1Value(idx++, -1);
  }


  return root;
}

//=============================================================================


//static manager instance
lmco_manager *lmco_manager::instance_ = 0;

//===============================================================
//: The singleton pattern - only one instance of the manager can occur
//==============================================================
lmco_manager *lmco_manager::instance()
{
  if (!instance_)
  {
    instance_ = new lmco_manager();
    instance_->init();
  }
  return lmco_manager::instance_;
}

//==================================================================
//: constructors/destructor
//==================================================================
lmco_manager::
lmco_manager() : vgui_wrapper_tableau()
{
}

lmco_manager::~lmco_manager()
{
  delete instance_;
}

//======================================================================
//: set up the tableaux at each grid cell
//  the vtol2D_tableaux have been initialized in the constructor
//======================================================================
void lmco_manager::init()
{
  active_method_ = SUM;
  use_gradients_ = true;
  grid_ = vgui_grid_tableau_new(2,1);
  grid_->set_uses_paging_events(false);
  grid_->add_at(vgui_viewer2D_tableau_new(v[0].tableau_root()),0,0);
  grid_->add_at(vgui_viewer2D_tableau_new(v[1].tableau_root()),1,0);
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(grid_);
  this->add_child(shell);

}


//=========================================================================
//: make an event handler
// note that we have to get an adaptor and set the tableau to receive events
// this handler does nothing but is a place holder for future event processing
// For now, just pass the events down to the child tableaux
//=========================================================================
bool lmco_manager::handle(const vgui_event &e)
{

  return this->child.handle(e);
}

unsigned int lmco_manager::active_view() const
{
  unsigned int r,c;
  grid_->get_last_selected_position(&c, &r);
  return c;
}

//=========================================================================
//: Quit
//=========================================================================
void lmco_manager::quit()
{
  vgui::quit();
}



//=========================================================================
//: load an image 
//=========================================================================
void lmco_manager::load_image()
{
  unsigned int i = active_view();
  vcl_stringstream title;
  title << "Load Image " << i+1;
  vgui_dialog load_image_dlg(title.str().c_str());
  static vcl_string image_filename = "";
  static vcl_string ext = "*.*";
  bool rgb_to_eo = false;
  double scale = v[i].data_scale();
  load_image_dlg.file("Image Filename:", ext, image_filename);
  load_image_dlg.field("Down Sample Factor", scale);
  load_image_dlg.checkbox("Convert RGB to EO", rgb_to_eo);
  if (!load_image_dlg.ask())
    return;

  vil_image_resource_sptr img = vil_load_image_resource(image_filename.c_str());
  
  v[i].set_image(img, scale, rgb_to_eo);
}


//: Calculate the range parameters for the input image
vgui_range_map_params_sptr lmco_manager::
range_params(vil_image_view_base_sptr const& image)
{
  double gamma = 1.0;
  bool invert = false;
  bool gl_map = false;
  bool cache = true;
  double min = 0, max = 255;
  if (image->pixel_format()==VIL_PIXEL_FORMAT_BYTE)
  {
    vil_image_view<unsigned char> temp = image;
    unsigned char vmin=0, vmax=255;
    vil_math_value_range<unsigned char>(temp, vmin, vmax);
    min = static_cast<double>(vmin);
    max = static_cast<double>(vmax);
    if(image->nplanes() == 3)
      return new  vgui_range_map_params(min, max, min, max, min, max,
                                        gamma, gamma, gamma, invert,
                                        gl_map, cache);

    return  new vgui_range_map_params(min, max, gamma, invert,
                                      gl_map, cache);

  }
  if (image->pixel_format()==VIL_PIXEL_FORMAT_UINT_16)
  {
    vil_image_view<unsigned short> temp = image;
    unsigned short vmin=0, vmax=60000;
    vil_math_value_range<unsigned short>(temp, vmin, vmax);
    min = static_cast<double>(vmin);
    max = static_cast<double>(vmax);
    gl_map = true;
    if(image->nplanes() == 3)
      return new  vgui_range_map_params(min, max, min, max, min, max,
                                        gamma, gamma, gamma, invert,
                                        gl_map, cache);

    return  new vgui_range_map_params(min, max, gamma, invert,
                                      gl_map, cache);
  }
  if (image->pixel_format()==VIL_PIXEL_FORMAT_FLOAT)
  {
    vil_image_view<float> temp = image;
    float vmin=0.0f, vmax=1.0f;
    vil_math_value_range<float>(temp, vmin, vmax);
    min = static_cast<double>(vmin);
    max = static_cast<double>(vmax);
    gl_map = true;
    if(image->nplanes() == 3)
      return new  vgui_range_map_params(min, max, min, max, min, max,
                                        gamma, gamma, gamma, invert,
                                        gl_map, cache);
    
    return  new vgui_range_map_params(min, max, gamma, invert,
                                      gl_map, cache);
  }
vcl_cout << "Image pixel format not handled\n";
 return new vgui_range_map_params(0, 255, gamma, invert,
                                      gl_map, cache);
}

//=========================================================================
//: load a mesh
//=========================================================================
void lmco_manager::load_mesh()
{
  vgui_dialog load_mesh_dlg("Load Mesh");
  static vcl_string filename = "";
  static vcl_string ext = "*.*";
  load_mesh_dlg.file("Mesh Filename:", ext, filename);
  if (!load_mesh_dlg.ask())
    return;


  if(!imesh_read_ply2(filename,mesh_))
    vgui_error_dialog(("Could not load file: "+filename).c_str());

  if(!mesh_.faces().has_normals())
    mesh_.compute_face_normals();

  v[0].set_mesh(mesh_);
  v[1].set_mesh(mesh_);
}




//=========================================================================
//: load a camera
//=========================================================================
void lmco_manager::load_camera()
{
  unsigned int i = active_view();
  vcl_stringstream title;
  title << "Load Camera " << i+1;
  vgui_dialog load_camera_dlg(title.str().c_str());
  static vcl_string filename = "";
  static vcl_string ext = "*.*";
  load_camera_dlg.file("Camera Filename:", ext, filename);
  if (!load_camera_dlg.ask())
    return;

  vnl_double_3x4 camera;
  vcl_fstream fh(filename.c_str());
  fh >> camera;
  fh.close();

  vpgl_perspective_camera<double> cam;
  vpgl_perspective_decomposition(camera, cam);

  v[i].set_camera(cam);

}

//=========================================================================
//: set the scale of the mesh
//=========================================================================
void lmco_manager::set_scale()
{
  unsigned int i = active_view();
  vcl_stringstream title;
  title << "Set Scale " << i+1;
  vgui_dialog scale_dlg(title.str().c_str());
  static vcl_string filename = "";
  static vcl_string ext = "*.*";

  double scale = v[i].scale();
  scale_dlg.field("Scale", scale);
  static bool all = true;
  scale_dlg.checkbox("Apply to all", all);
  if (!scale_dlg.ask())
    return;
  
  if(all){
    v[0].set_scale(scale);
    v[1].set_scale(scale);
  }
  else
    v[i].set_scale(scale);
}

//=========================================================================
//: set the draw style of the mesh
//=========================================================================
void lmco_manager::set_style()
{
  unsigned int i = active_view();
  vcl_stringstream title;
  title << "Set Style " << i+1;
  vgui_dialog scale_dlg(title.str().c_str());
  static vcl_string filename = "";
  static vcl_string ext = "*.*";

  double r,g,b,a;
  v[i].get_style(r,g,b,a);
  vcl_stringstream color_stm;
  color_stm << r<<' '<<g<<' '<<b<<' '<<a;
  vcl_string color = color_stm.str();
  scale_dlg.inline_color("Mesh Color", color);
  static bool all = true;
  scale_dlg.checkbox("Apply to all", all);
  if (!scale_dlg.ask())
    return;
  
  color_stm.str(color);
  color_stm >> r >> g >> b >> a;
  vcl_cout << "R: "<<r<<" G: "<<g<<" B: "<<b<<" A: "<<a<<vcl_endl;
  
  if(all){
    v[0].set_style(r,g,b,a);
    v[1].set_style(r,g,b,a);
  }
  else
    v[i].set_style(r,g,b,a);
}


//=========================================================================
//: set the draw style of the mesh
//=========================================================================
void lmco_manager::adjust_view()
{
  unsigned int i = active_view();
  vcl_stringstream title;
  title << "Display Options for View " << i+1;
  vgui_dialog view_dlg(title.str().c_str());
  
  vcl_vector<vcl_string> mode_names(7);
  mode_names[view::ORIG] = "Orginal Image";
  mode_names[view::EO_INT] = "EO Intensity";
  mode_names[view::EO_ORT] = "EO Gradient Orientation";
  mode_names[view::EO_MAG] = "EO Gradient Magnitude";
  mode_names[view::IR_INT] = "IR Intensity";
  mode_names[view::IR_ORT] = "IR Gradient Orientation";
  mode_names[view::IR_MAG] = "IR Gradient Magnitude";
  
  unsigned int choice = v[i].display_mode();
  view_dlg.choice("View Mode", mode_names, choice);
  static bool all = true;
  view_dlg.checkbox("Apply to all", all);
  
  if (!view_dlg.ask())
    return;
  
  if(all){
    v[0].set_display_mode(static_cast<view::display_img_t>(choice));
    v[1].set_display_mode(static_cast<view::display_img_t>(choice));
  }
  else
    v[i].set_display_mode(static_cast<view::display_img_t>(choice));
  
}


//=========================================================================
//: Create an overlay of the texture mapping
//=========================================================================
vgui_soview2D_image* lmco_manager::build_texture_image(const view* const vw, unsigned mode) const
{
  unsigned int i1 = 0, i2 = 1;
  if(&v[0] == vw){
    i1 = 1; i2 = 0;
  }
  
  // this is slow and ugly, but vpgl_perspective_camera<T>::postmultiply can not handle scaling
  vpgl_perspective_camera<double> cam1;
  vpgl_perspective_decomposition(postmultiply(vpgl_proj_camera<double>(v[i2].camera()),v[i2].homography()).get_matrix(), cam1);
  vpgl_perspective_camera<double> cam2;
  vpgl_perspective_decomposition(postmultiply(vpgl_proj_camera<double>(v[i1].camera()),v[i1].homography()).get_matrix(), cam2);
  
  
  unsigned int ni = v[i2].image()->ni();
  unsigned int nj = v[i2].image()->nj();
  double step = 1.0;
  
  vcl_vector<vgl_point_2d<double> > img_pts1, img_pts2;
  compute_point_mapping(cam1, cam2, ni, nj, step, 
                        mesh_, mesh_.faces().normals(), img_pts1, img_pts2);
     
  if(img_pts1.empty())
     return 0;
  
  unsigned int min_x = ni, max_x = 0, min_y = nj, max_y = 0;
  for(unsigned k=0; k<img_pts1.size(); ++k){
    if(img_pts1[k].x() < min_x) min_x = img_pts1[k].x();
    if(img_pts1[k].x() > max_x) max_x = img_pts1[k].x();
    if(img_pts1[k].y() < min_y) min_y = img_pts1[k].y();
    if(img_pts1[k].y() > max_y) max_y = img_pts1[k].y();
  }
  

  vil_image_view<vxl_byte> img(max_x - min_x+1, max_y - min_y+1, 4);
  img.fill(0);
  
  vcl_vector<double> d_eo;
  bilin_sample(img_pts2, v[i1].eo_data(), d_eo, mode);
  
  for(unsigned int k=0; k<d_eo.size(); ++k){
    unsigned i = static_cast<unsigned>(img_pts1[k].x()) - min_x;
    unsigned j = static_cast<unsigned>(img_pts1[k].y()) - min_y;
    vxl_byte v = static_cast<vxl_byte>(d_eo[k]*255.0);
    img(i,j,0) = img(i,j,1) = img(i,j,2) = v;
    img(i,j,3) = 255;
  }
  
  return new vgui_soview2D_image(min_x, min_y, img, true);
}


//=========================================================================
//: compute mutual information
//=========================================================================
double lmco_manager::compute_mi()
{
  unsigned int i1 = active_view();
  unsigned int i2 = 1-i1;
  
  // this is slow and ugly, but vpgl_perspective_camera<T>::postmultiply can not handle scaling
  vpgl_perspective_camera<double> cam1;
  vpgl_perspective_decomposition(postmultiply(vpgl_proj_camera<double>(v[i1].camera()),v[i1].homography()).get_matrix(), cam1);
  vpgl_perspective_camera<double> cam2;
  vpgl_perspective_decomposition(postmultiply(vpgl_proj_camera<double>(v[i2].camera()),v[i2].homography()).get_matrix(), cam2);
  

  unsigned int ni = v[i1].eo_data().ni();
  unsigned int nj = v[i1].eo_data().nj();
  double step = 1.0;
  
  vcl_vector<vgl_point_2d<double> > img_pts1, img_pts2;
  compute_point_mapping(cam1, cam2, ni, nj, step, 
                        mesh_, mesh_.faces().normals(), img_pts1, img_pts2);
  
  vcl_vector<vcl_vector<double> > d1_ir(3), d2_ir(3), d1_eo(3), d2_eo(3);
  for(unsigned i=0; i<3; ++i){
    bilin_sample(img_pts1, v[i1].ir_data(), d1_ir[i], i);
    bilin_sample(img_pts1, v[i1].eo_data(), d1_eo[i], i);
    bilin_sample(img_pts2, v[i2].ir_data(), d2_ir[i], i);
    bilin_sample(img_pts2, v[i2].eo_data(), d2_eo[i], i);
  }
  
  //vcl_cout << "MI_IR = "<<MI_IR<<"\nMI_EO = "<<MI_EO<<vcl_endl;
  
  double mi = compute_mi(d1_ir, d2_ir, d1_eo, d2_eo);
  //vcl_cout << "MI = " << mi << vcl_endl;
  return mi;
}

//: compute mutual info of the data using the active method
double lmco_manager::compute_mi(const vcl_vector<vcl_vector<double> >& d1_ir, 
                                const vcl_vector<vcl_vector<double> >& d2_ir, 
                                const vcl_vector<vcl_vector<double> >& d1_eo, 
                                const vcl_vector<vcl_vector<double> >& d2_eo)
{
  double mi = 0.0;
  switch(active_method_){
    case EO:
    {
      mi = mutual_info(d1_eo[0], d2_eo[0]);
      if(use_gradients_)
        mi += mutual_info_weighted(d1_eo[1], d2_eo[1], d1_eo[2], d2_eo[2]);
    }
      break;
    case IR:
    {
      mi = mutual_info(d1_ir[0], d2_ir[0]);
      if(use_gradients_)
        mi += mutual_info_weighted(d1_ir[1], d2_ir[1], d1_ir[2], d2_ir[2]);
    }
      break;
    case SUM:
    {
      double MI_IR = mutual_info(d1_ir[0], d2_ir[0]);
      double MI_EO = mutual_info(d1_eo[0], d2_eo[0]);
      if(use_gradients_){
        MI_IR += mutual_info_weighted(d1_ir[1], d2_ir[1], d1_ir[2], d2_ir[2]);
        MI_EO += mutual_info_weighted(d1_eo[1], d2_eo[1], d1_eo[2], d2_eo[2]);
      }
      mi = MI_IR + MI_EO;
    }
      break;
    case POLAR:
    {
      vcl_vector<double> ang1, ang2, mag1, mag2;
      for(unsigned int i=0; i<d1_ir.size(); ++i){
        ang1.push_back(vcl_atan2(d1_eo[0][i], d1_ir[0][i])/1.5707963);
        ang2.push_back(vcl_atan2(d2_eo[0][i], d2_ir[0][i])/1.5707963);
        mag1.push_back(vcl_sqrt(d1_eo[0][i]*d1_eo[0][i] + d1_ir[0][i]*d1_ir[0][i]));
        mag2.push_back(vcl_sqrt(d2_eo[0][i]*d2_eo[0][i] + d2_ir[0][i]*d2_ir[0][i]));
      }
      mi = mutual_info_weighted(ang1, ang2, mag1, mag2);
    }
      break;
  }
  return mi;
}




//: optimize the position of the vehicle for maximum MI
void lmco_manager::optimize_position()
{
  unsigned int i1 = active_view();
  unsigned int i2 = 1-i1;
  
  vpgl_perspective_camera<double> cam1;
  vpgl_perspective_decomposition(postmultiply(vpgl_proj_camera<double>(v[i1].camera()),v[i1].homography()).get_matrix(), cam1);
  vpgl_perspective_camera<double> cam2;
  vpgl_perspective_decomposition(postmultiply(vpgl_proj_camera<double>(v[i2].camera()),v[i2].homography()).get_matrix(), cam2);
  
  vgui_dialog optimize_dlg("Optimize Parameters");
  static bool joint = false;
  static bool use_rotation = true;
  static double trans_step = 3.0;
  static double grid_step = 1.0;
  optimize_dlg.checkbox("Joint Optimization",joint);
  optimize_dlg.checkbox("Include Rotation",use_rotation);
  optimize_dlg.field("Simplex Step Size",trans_step);
  optimize_dlg.field("Grid Step Size",grid_step);
  if(!optimize_dlg.ask())
    return;
  
  if(joint){
    mesh_joint_mi_cost_func mi_func(v[i1].eo_data(),v[i1].ir_data(),
                                    v[i2].eo_data(),v[i2].ir_data(),
                                    cam1,cam2,mesh_,grid_step,use_rotation);
    
    vnl_amoeba::default_verbose = true;
    vnl_vector<double> x(4,0.0), dx(4,trans_step);
    if(use_rotation){
      x.set_size(6);  x.fill(0.0);
      dx.set_size(6); dx.fill(trans_step); 
      dx[2]=dx[5]=0.5;
    }
    vnl_amoeba::minimize(mi_func, x, dx);
    
    vcl_cout << "min x = " << x << vcl_endl;
    
    vgl_homg_point_3d<double> p = v[i1].homography()*vgl_homg_point_3d<double>(x[0],x[1],0);
    v[i1].set_pos(p.x()/p.w(), p.y()/p.w());
    if(use_rotation)
      v[i1].set_angle(v[i1].angle()+x[2]);
    
    if(use_rotation){
      p = v[i2].homography()*vgl_homg_point_3d<double>(x[3],x[4],0);
      v[i2].set_angle(v[i2].angle()+x[5]);
    }
    else
      p = v[i2].homography()*vgl_homg_point_3d<double>(x[2],x[3],0); 
    v[i2].set_pos(p.x()/p.w(), p.y()/p.w());
  }
  else{
    mesh_mi_cost_func mi_func(v[i1].eo_data(),v[i1].ir_data(),
                              v[i2].eo_data(),v[i2].ir_data(),
                              cam1,cam2,mesh_,grid_step,use_rotation);
    
    vnl_amoeba::default_verbose = true;
    vnl_vector<double> x(2,0.0), dx(2,trans_step);
    if(use_rotation){
      x.set_size(3);  x.fill(0.0);
      dx.set_size(3); dx.fill(trans_step); 
      dx[2]=0.5;
    }
    vnl_amoeba::minimize(mi_func, x, dx);
    
    vcl_cout << "min x = " << x << vcl_endl;
    
    vgl_homg_point_3d<double> p = v[i1].homography()*vgl_homg_point_3d<double>(x[0],x[1],0);
    v[i1].set_pos(p.x()/p.w(), p.y()/p.w());
    if(use_rotation)
      v[i1].set_angle(v[i1].angle()+x[2]);
    
  }
  
  post_redraw();
}


//: set options
void lmco_manager::set_options()
{
  vgui_dialog options_dlg("Options");
  unsigned int choice = static_cast<unsigned int>(active_method_);
  vcl_vector<vcl_string> choices;
  choices.push_back("EO only");
  choices.push_back("IR only");
  choices.push_back("Sum EO and IR");
  choices.push_back("Polar Coordinates");
  options_dlg.choice("MI method",choices,choice);
  options_dlg.checkbox("Use Gradients",use_gradients_);
  if(!options_dlg.ask())
    return;
  
  active_method_ = static_cast<mi_method>(choice);
}
