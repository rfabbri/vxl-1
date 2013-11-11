// This is brcv/mvg/dbvrl/vis/dbvrl_region_tools.cxx
//:
// \file

#include "dbvrl_region_tools.h"
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <dbvrl/pro/dbvrl_region_storage.h>
#include <vidpro1/vidpro1_repository.h>
#include <vgui/vgui_projection_inspector.h>
#include "dbvrl_region_tableau.h"
#include <vcl_iostream.h>
#include <vcl_algorithm.h>
#include <vil/vil_image_resource.h>

#include <vil/vil_convert.h>
#include <vgui/vgui.h> 
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_4point.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/algo/vnl_svd.h>


#include <dbvrl/dbvrl_region.h>
#include <dbvrl/dbvrl_transform_2d.h>

#include <dbvrl/dbvrl_super_res.h>

#include <vidpro1/storage/vidpro1_image_storage.h>





//: Constructor
dbvrl_region_transform_tool::dbvrl_region_transform_tool()
 : tableau_(NULL), xform_storage_(NULL),
   state_(TRANSLATE),
   init_x_(0.0), init_y_(0.0),
   last_x_(0.0), last_y_(0.0),
   active_(false),
   active_corner_(0),
   curr_region_(new dbvrl_region(100, 100)),
   vertices_(4,vgl_point_2d<double>(0.0,0.0)),
   active_tableau_(NULL)
{
  curr_xform_.set_zoom_only(100.0, 0.0, 0.0);
}


//: Destructor
dbvrl_region_transform_tool::~dbvrl_region_transform_tool()
{
}


//: Set the tableau to work with
bool
dbvrl_region_transform_tool::set_tableau ( const vgui_tableau_sptr& tableau )
{
  if(!tableau)
    return false;

  if( tableau->type_name() == "dbvrl_region_tableau" ){
    if( !this->set_storage(bvis1_manager::instance()->storage_from_tableau(tableau)) )
      return false;
    tableau_.vertical_cast(tableau);
    return true;
  }

  tableau_ = NULL;
  return false;
}


//: Set the storage class for the active tableau
bool
dbvrl_region_transform_tool::set_storage ( const bpro1_storage_sptr& storage )
{
  if (!storage)
    return false;

  if ( xform_storage_ && 
       xform_storage_->name() == storage->name() &&
       xform_storage_->frame() == storage->frame() )
    return true;
  if (storage->type() == "bvrl"){
    xform_storage_.vertical_cast(storage);
    dbvrl_region_sptr reg = xform_storage_->region();
    if(reg){
      vertices_ = reg->corners();
      for(int i=0; i<4; ++i)
        vertices_[i] = (*xform_storage_->transform())(vertices_[i]);
    }
    return true; 
  }
  xform_storage_ = NULL;
  return false;
}



//: Return the name of this tool
vcl_string
dbvrl_region_transform_tool::name() const
{
  return "Region Transform"; 
}



//: Handle events
bool
dbvrl_region_transform_tool::handle( const vgui_event & e, 
                                     const bvis1_view_tableau_sptr& view )
{
  if( e.type == vgui_KEY_PRESS && e.key == 'b'){
    vidpro1_repository_sptr rep = bvis1_manager::instance()->repository();
    //vcl_vector<vcl_string> names = rep->get_all_storage_class_names("dbvrl", frame);

    vgui_dialog reg_dialog("Create New Region");
    static vcl_string name = "region0";
    reg_dialog.field("Region Name: ", name);
    static bool copy = true;
    reg_dialog.checkbox("Copy Last Modified",copy);
    if( !reg_dialog.ask() )
      return false;


    dbvrl_transform_2d_sptr xform;
    dbvrl_region_sptr region;
    if(copy){
      xform = new dbvrl_transform_2d(curr_xform_);
      region = curr_region_;
    }
    else{
      xform = new dbvrl_transform_2d();
      xform->set_zoom_only(100.0, 0.0, 0.0);
      region = new dbvrl_region(100, 100);
    }
    dbvrl_region_storage_new output(region, xform);
    output->set_name(name);
    rep->store_data_at(output, view->frame());
    bvis1_manager::instance()->add_to_display(output);
    bvis1_manager::instance()->display_current_frame();
    vgui_selector_tableau_sptr active_tab = 
      bvis1_manager::instance()->active_selector();

    if(active_tab)
      active_tab->set_active(name);
    return true;
  }

  if(!tableau_)
    return false;

  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

  if( e.type == vgui_KEY_PRESS ) {
    switch(e.key){
    case 't':
      state_ = TRANSLATE;
      update_xform();
      break;
    case 'r':
      state_ = ROTATE;
      update_xform();
      break;
    case 's':
      state_ = SCALE;
      update_xform();
      break;
    case 'a':
      state_ = SKEW;
      update_xform();
      break;
    case 'm':
      state_ = PROJECT;
      update_xform();
      break;
    case 'd':
      // reset to the default transform
      xform_storage_->transform()->set_zoom_only( xform_storage_->region()->ni(),
                                                  xform_storage_->region()->nj(), 
                                                  0.0, 0.0 );
      tableau_->post_redraw();
      active_ = false;
      break;
    default:
      break;
    }
    return true;
  }

  
  if( e.type == vgui_OVERLAY_DRAW ) {
      
    if(tableau_ == active_tableau_){    
      //this->draw_grid(vertices_, ni_, nj_);
    }
    return true;
  }

  if( e.type == vgui_MOTION) {
    // last known position
    last_x_ = ix;
    last_y_ = iy;
    if(active_){ 
      vimt_transform_2d update;
      switch(state_){
      case TRANSLATE:
        {
          update.set_translation(ix - init_x_, iy - init_y_);
        }
        break;
      case SCALE:
        {
          vimt_transform_2d center_xform;
          vgl_point_2d<double> center = min_bound_ + (max_bound_ - min_bound_)/2.0;
          center_xform.set_translation(center.x(), center.y());
          double sx = (ix - center.x())/(init_x_ - center.x());
          double sy = (iy - center.y())/(init_y_ - center.y());
          update.set_zoom_only(sx, sy, 0.0, 0.0);
          update = center_xform * update * center_xform.inverse();
        }
        break;
      case ROTATE:
        {
          vimt_transform_2d center_xform;
          vgl_point_2d<double> center = min_bound_ + (max_bound_ - min_bound_)/2.0;
          center_xform.set_translation(center.x(), center.y());
          double theta = atan2(iy - center.y(), ix - center.x());
          double init_t = atan2(init_y_ - center.y(), init_x_ - center.x());
          update.set_rigid_body(theta-init_t, 0.0, 0.0);
          update = center_xform * update * center_xform.inverse();
        }
        break;
      case SKEW:
        {
          update.set_identity();
        }
        break;
      case PROJECT:
        {
          new_corners_[active_corner_].set(ix, iy);
          vcl_vector< vgl_homg_point_2d< double > > pts1, pts2;
          pts1.push_back(vgl_homg_point_2d<double>(vertices_[0]));
          pts1.push_back(vgl_homg_point_2d<double>(vertices_[1]));
          pts1.push_back(vgl_homg_point_2d<double>(vertices_[2]));
          pts1.push_back(vgl_homg_point_2d<double>(vertices_[3]));

          pts2.push_back(vgl_homg_point_2d<double>(new_corners_[0]));
          pts2.push_back(vgl_homg_point_2d<double>(new_corners_[1]));
          pts2.push_back(vgl_homg_point_2d<double>(new_corners_[2]));
          pts2.push_back(vgl_homg_point_2d<double>(new_corners_[3]));
          vgl_h_matrix_2d<double> homog = vgl_h_matrix_2d_compute_4point().compute(pts1,pts2);
          vnl_matrix<double> H;
          homog.get(&H);
          update.set_projective(H);
        }
        break;
      }
      vimt_transform_2d new_xform(update*curr_xform_);
      vnl_vector< double > params;
      new_xform.params(params);
      xform_storage_->transform()->set(params, new_xform.form()); 
      vgui::out << "Form: " << new_xform.form() << '\n';
      tableau_->post_redraw();
      return true;
    }
  }

  if( e.type == vgui_BUTTON_DOWN && e.button == vgui_LEFT ) {
    if(!active_){
      // make closest corner active
      active_corner_ = 0;
      vgl_point_2d<double> mouse(ix,iy);
      double min_dist = vgl_distance(mouse, vertices_[0]);
      for(int i=1; i<4; ++i){
        double dist = vgl_distance(mouse, vertices_[i]);
        if(dist < min_dist){
          active_corner_ = i;
          min_dist = dist;
        }
      }
      // last known position
      last_x_ = ix;
      last_y_ = iy;
      update_xform();
      active_ = true;
    }
    else{
      active_ = false;
    }
    return true;
  } 
 
  return false;

}


//: update the current transform
void 
dbvrl_region_transform_tool::update_xform()
{
  init_x_ = last_x_;
  init_y_ = last_y_;
  curr_xform_ = *xform_storage_->transform();
  curr_region_ = xform_storage_->region();
  vertices_ = xform_storage_->region()->corners();
  for(int i=0; i<4; ++i){
    vertices_[i] = curr_xform_(vertices_[i]); 
    if(i==0)
      min_bound_ = max_bound_ = vertices_[0];
    else{
      min_bound_.x() = vcl_min(min_bound_.x(), vertices_[i].x());
      min_bound_.y() = vcl_min(min_bound_.y(), vertices_[i].y());
      max_bound_.x() = vcl_max(max_bound_.x(), vertices_[i].x());
      max_bound_.y() = vcl_max(max_bound_.y(), vertices_[i].y());
    }
  }
  new_corners_ = vertices_;
}




//=======================================================
// Super resolution


dbvrl_region_super_res_tool::dbvrl_region_super_res_tool()
{
}


dbvrl_region_super_res_tool::~dbvrl_region_super_res_tool()
{
}


//: Set the tableau to work with
bool 
dbvrl_region_super_res_tool::set_tableau ( const vgui_tableau_sptr& tableau )
{
  return true;
}


//: Set the storage class for the active tableau
//bool 
//dbvrl_region_super_res_tool::set_storage ( const bpro1_storage_sptr& storage);


//: Return the name of this tool
vcl_string 
dbvrl_region_super_res_tool::name() const
{
  return "Super Resolution";
}


//: Handle events
bool 
dbvrl_region_super_res_tool::handle( const vgui_event & e, 
                                   const bvis1_view_tableau_sptr& view )
{
  if( e.type == vgui_KEY_PRESS && e.key == 's' ){
    vidpro1_repository_sptr rep = bvis1_manager::instance()->repository();

    //Dialog to select the image and transforms
    vgui_dialog in_dialog("Select Images and Transforms");

    vcl_vector<vcl_string> images = rep->get_all_storage_class_names("image");

    static int img_index = 0;
    in_dialog.choice("Select Image:", images, img_index);

    vcl_vector<vcl_string> xforms = rep->get_all_storage_class_names("dbvrl");

    static int xform_index = 0;
    in_dialog.choice("Select Image:", xforms, xform_index);

    static int block_size = 10;
    in_dialog.field("Block Size",block_size);

    static double blur = 0.25;
    in_dialog.field("Weight Blur",blur);

    static double area_mag = 4.0;
    in_dialog.field("Area Magnification",area_mag);


    static vcl_string out_path = "";
    static vcl_string out_ext = "*.png";
    in_dialog.file("Output Image Path",out_ext,out_path);

    if(!in_dialog.ask())
      return false;

    images_.clear();
    for(int f=0; f<rep->num_frames(); ++f){
      vidpro1_image_storage_sptr img_stg;
      img_stg.vertical_cast(rep->get_data_at("image",f,img_index));
      dbvrl_region_storage_sptr dbvrl_stg;
      dbvrl_stg.vertical_cast(rep->get_data_at("dbvrl",f,xform_index));
      if( img_stg && dbvrl_stg ){
        vil_image_view<vxl_byte> curr_image = img_stg->get_image()->get_view();
        vimt_transform_2d xform = *dbvrl_stg->transform();
        vimt_image_2d_of<vxl_byte> v_img(curr_image, xform);
        images_.push_back(v_img);
      }
    }

    dbvrl_region_storage_sptr dbvrl_stg;
    dbvrl_stg.vertical_cast(rep->get_data("dbvrl",0,xform_index));
    dbvrl_super_res super_resolution(images_, block_size);
    dbvrl_region_sptr reg = super_resolution.compute_region(area_mag);
    vcl_cout << "(ni, nj) = " << reg->ni()<<","<<reg->nj()<< vcl_endl;
    dbvrl_stg->region()->set_world_size(reg->ni(), reg->nj());
    vil_image_view<double> s_img = super_resolution.compute_high_res(dbvrl_stg->region(),
                                                                     out_path, blur);

    vgui_dialog mydialog("Super Resolved Image");
    vgui_image_tableau_new image(s_img);
    vgui_viewer2D_tableau_new viewer(image);
    mydialog.inline_tableau(viewer, reg->ni(), reg->nj());

    mydialog.set_ok_button("close");
    mydialog.set_cancel_button(0);
    mydialog.ask();

    return true;
  }
  return false;
}
