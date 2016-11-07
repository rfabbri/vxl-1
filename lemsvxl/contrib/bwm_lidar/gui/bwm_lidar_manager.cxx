// This is contrib/bwm_lidar/gui/bwm_lidar_manager.cxx
#include "bwm_lidar_manager.h"
//:
// \file
// \author Matt Leotta

#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_range_map_params.h>
#include <bgui/bgui_image_tableau.h>

#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_crop.h>
#include <vil/vil_convert.h>

#include <imesh/imesh_fileio.h>

#include <bgui/bgui_selector_tableau.h>

#include <bmdl/bmdl_classify.h>
#include <bmdl/bmdl_mesh.h>
#include <bwm_lidar/algo/world_modeler.h>

//static manager instance
bwm_lidar_manager *bwm_lidar_manager::instance_ = 0;

//===============================================================
//: The singleton pattern - only one instance of the manager can occur
//==============================================================
bwm_lidar_manager *bwm_lidar_manager::instance()
{
  if (!instance_)
  {
    instance_ = new bwm_lidar_manager();
    instance_->init();
  }
  return bwm_lidar_manager::instance_;
}

//==================================================================
//: constructors/destructor
//==================================================================
bwm_lidar_manager::
bwm_lidar_manager() : vgui_wrapper_tableau()
{
}

bwm_lidar_manager::~bwm_lidar_manager()
{
}

//======================================================================
//: set up the tableaux at each grid cell
//  the vtol2D_tableaux have been initialized in the constructor
//======================================================================
void bwm_lidar_manager::init()
{

  
  vgui_grid_tableau_sptr grid = vgui_grid_tableau_new(2,1);
  grid->set_uses_paging_events(false);
  first_return_tab_ = bgui_image_tableau_new();
  last_return_tab_ = bgui_image_tableau_new();
  color_img_tab_ = bgui_image_tableau_new();
  labels_img_tab_ = bgui_image_tableau_new();
  boundaries_tab_ = vgui_easy2D_tableau_new();

  bgui_selector_tableau_new sel_tab;
  sel_tab->add(color_img_tab_, "Color");
  sel_tab->add(last_return_tab_, "Last Returns");
  sel_tab->add(first_return_tab_, "First Returns");
  sel_tab->add(labels_img_tab_, "Object Labels");
  sel_tab->add(boundaries_tab_, "Boundaries");
  grid->add_at(vgui_viewer2D_tableau_new(sel_tab),0,0);
  //grid->add_at(vgui_viewer2D_tableau_new(img2_tab),1,0);
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(grid);
  this->add_child(shell);

}


//=========================================================================
//: make an event handler
// note that we have to get an adaptor and set the tableau to receive events
// this handler does nothing but is a place holder for future event processing
// For now, just pass the events down to the child tableaux
//=========================================================================
bool bwm_lidar_manager::handle(const vgui_event &e)
{

  return this->child.handle(e);
}


//=========================================================================
//: Quit
//=========================================================================
void bwm_lidar_manager::quit()
{
  vgui::quit();
}


//=========================================================================
//: save LIDAR Images
//=========================================================================
void bwm_lidar_manager::save_images()
{
  vgui_dialog save_image_dlg("Save LIDAR Images");
  static vcl_string image1_filename = "first_return.tiff";
  static vcl_string image2_filename = "last_return.tiff";
  static vcl_string ext = "*.*";
  static bool save_float = false;
  
  save_image_dlg.file("First Returns:", ext, image1_filename);
  save_image_dlg.file("Last Returns:", ext, image2_filename);
  save_image_dlg.checkbox("Save in 32-bit float", save_float);
  if (!save_image_dlg.ask())
    return;

  if(save_float){
    vil_image_view<float> fr;
    vil_image_view<float> lr;
    vil_convert_cast(classifier_.first_return(),fr);
    vil_convert_cast(classifier_.last_return(),lr);
    
    vil_save(fr, image1_filename.c_str());
    vil_save(lr, image2_filename.c_str());
  }
  else{
    vil_image_view<double> fr = classifier_.first_return();
    vil_image_view<double> lr = classifier_.last_return();
    vil_save(fr, image1_filename.c_str());
    vil_save(lr, image2_filename.c_str());
  }

}


//=========================================================================
//: save mesh
//=========================================================================
void bwm_lidar_manager::save_mesh()
{
  vgui_dialog save_image_dlg("Save Mesh");
  static vcl_string filename = "";
  static vcl_string ext = "*.obj";
  
  save_image_dlg.file("Mesh File:", ext, filename);
  if (!save_image_dlg.ask())
    return;
  
  imesh_write_obj(filename,mesh_);
}



//=========================================================================
//: load LIDAR Images
//=========================================================================
void bwm_lidar_manager::load_images()
{
  vgui_dialog load_image_dlg("Load LIDAR Images");
  static vcl_string image1_filename = "";
  static vcl_string image2_filename = "";
  static vcl_string ext = "*.*";
  static bool crop = false;
  static int min_x=0;
  static int min_y=0;
  static int max_x=1000;
  static int max_y=1000;

  load_image_dlg.file("First Returns:", ext, image1_filename);
  load_image_dlg.file("Last Returns:", ext, image2_filename);
  load_image_dlg.checkbox("Crop",crop);
  load_image_dlg.field("Min X",min_x);
  load_image_dlg.field("Min Y",min_y);
  load_image_dlg.field("Max X",max_x);
  load_image_dlg.field("Max Y",max_y);
  if (!load_image_dlg.ask())
    return;

  vil_image_view<double> fr, lr;
  if(crop)
  {
    fr = vil_convert_cast(double(),vil_crop(vil_load_image_resource(image1_filename.c_str()),
                                                       min_x, max_x-min_x,
                                                       min_y, max_y-min_y)->get_view());
    lr = vil_convert_cast(double(),vil_crop(vil_load_image_resource(image2_filename.c_str()),
                                                      min_x, max_x-min_x,
                                                      min_y, max_y-min_y)->get_view());
  }
  else
  {
    fr = vil_convert_cast(double(),vil_load(image1_filename.c_str()));
    lr = vil_convert_cast(double(),vil_load(image2_filename.c_str()));
  }
  classifier_.set_lidar_data(fr,lr);
  double ground = classifier_.estimate_bare_earth();
  vcl_cout << "ground estimated at "<<ground <<vcl_endl;
  

  double minv = classifier_.last_min();
  double maxv = classifier_.first_max();

  vcl_cout << "range "<<minv<<" to "<<maxv<<vcl_endl;

  first_return_tab_->set_image_view(classifier_.first_return(), 
                                    new vgui_range_map_params(minv,maxv));
  last_return_tab_->set_image_view(classifier_.last_return(), 
                                   new vgui_range_map_params(minv,maxv));

  this->post_redraw();
}


//=========================================================================
//: load FLIMAP Data
//=========================================================================
void bwm_lidar_manager::load_flimap()
{
  vgui_dialog load_dlg("Load FLIMAP Data");
  static vcl_string glob = "";
  static vcl_string ext = "*";
  static double min_x=594025.0, max_x=594450.0, min_y=3343770.0, max_y=3344020.0;

  load_dlg.file("File Glob:", ext, glob);
  load_dlg.field("Min X", min_x);
  load_dlg.field("Max X", max_x);
  load_dlg.field("Min Y", min_y);
  load_dlg.field("Max Y", max_y);
  if (!load_dlg.ask())
    return;

  vgl_box_2d<double> bbox(min_x,max_x,min_y,max_y);
  
  vil_image_view<double> fr,lr;

  world_modeler::generate_lidar_images(glob, bbox,
                                       fr,
                                       lr,
                                       color_img_);
  classifier_.set_lidar_data(fr,lr);

  double minv = classifier_.last_min();
  double maxv = classifier_.first_max();

  vcl_cout << "range "<<minv<<" to "<<maxv<<vcl_endl;

  first_return_tab_->set_image_view(classifier_.first_return(), 
                                    new vgui_range_map_params(minv,maxv));
  last_return_tab_->set_image_view(classifier_.last_return(), 
                                   new vgui_range_map_params(minv,maxv));
  color_img_tab_->set_image_view(color_img_);

  this->post_redraw();
}


//=========================================================================
//: Label buildings, ground, and vegetation in lidar images
//=========================================================================
void bwm_lidar_manager::label_lidar()
{
  classifier_.segment();
  building_heights_.clear();
  building_sizes_.clear();
  classifier_.cluster_buildings(building_heights_, building_sizes_);
  classifier_.refine_buildings(building_heights_, building_sizes_);
  labels_img_tab_->set_image_view(classifier_.labels(), 
                                  new vgui_range_map_params(0,building_heights_.size()));
  labels_img_tab_->post_redraw();
}


//=========================================================================
//: Label buildings, ground, and vegetation in lidar images
//=========================================================================
void bwm_lidar_manager::segment_lidar()
{
  classifier_.segment();
  labels_img_tab_->set_image_view(classifier_.labels(), new vgui_range_map_params(0,2));
  labels_img_tab_->post_redraw();
}


//=========================================================================
//: Cluster pixels into groups of buildings
//=========================================================================
void bwm_lidar_manager::cluster_buildings()
{
  building_heights_.clear();
  building_sizes_.clear();
  classifier_.cluster_buildings(building_heights_, building_sizes_);
  labels_img_tab_->set_image_view(classifier_.labels(), 
                                  new vgui_range_map_params(0,building_heights_.size()));
  labels_img_tab_->post_redraw();
}


//=========================================================================
//: Clean up the building clusters
//=========================================================================
void bwm_lidar_manager::refine_buildings()
{
  classifier_.refine_buildings(building_heights_, building_sizes_);
  labels_img_tab_->set_image_view(classifier_.labels(), 
                                  new vgui_range_map_params(0,building_heights_.size()));
  labels_img_tab_->post_redraw();
}


//=========================================================================
//: Construct a mesh from the labels and lidar data
//=========================================================================
void bwm_lidar_manager::construct_mesh()
{
  vcl_vector<vgl_polygon<double> > boundaries = bmdl_mesh::trace_boundaries(classifier_.labels());
  bmdl_mesh::simplify_boundaries(boundaries);
  
  bmdl_mesh::mesh_lidar(boundaries, classifier_.labels(), 
                        classifier_.heights(), classifier_.bare_earth(), mesh_);
  
  unsigned int ni = classifier_.bare_earth().ni();
  unsigned int nj = classifier_.bare_earth().nj();
  // create the ground plane
  imesh_vertex_array<3>& verts = mesh_.vertices<3>();
  unsigned int nv = verts.size();
  verts.push_back(imesh_vertex<3>(0,0,classifier_.bare_earth()(0,0)));
  verts.push_back(imesh_vertex<3>(0,nj,classifier_.bare_earth()(0,nj-1)));
  verts.push_back(imesh_vertex<3>(ni,nj,classifier_.bare_earth()(ni-1,nj-1)));
  verts.push_back(imesh_vertex<3>(ni,0,classifier_.bare_earth()(ni-1,0)));
  imesh_face_array& faces = static_cast<imesh_face_array&>(mesh_.faces());
  faces.push_back(imesh_quad(nv,nv+1,nv+2,nv+3));

  boundaries_tab_->set_foreground(1.0,0.0,0.0);
  for(unsigned i=0; i<boundaries.size(); ++i){
    vgl_polygon<double>& poly = boundaries[i];

    for(unsigned j=0; j<poly.num_sheets(); ++j){
      unsigned psize = poly[j].size();
      float x[psize], y[psize];
      boundaries_tab_->set_foreground(0.0,0.0,1.0);
      boundaries_tab_->set_point_radius(2.0);
      for(unsigned k=0; k<psize; ++k){
        x[k] = poly[j][k].x();
        y[k] = poly[j][k].y();
        boundaries_tab_->add_point(x[k],y[k]);
      }
      if(j==0)
        boundaries_tab_->set_foreground(1.0,0.0,0.0);
      else
        boundaries_tab_->set_foreground(0.0,1.0,0.0);
      boundaries_tab_->add_polygon(psize,x,y);
    }
  }
  
  boundaries_tab_->post_redraw();
}


