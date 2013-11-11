// This is brcv/rec/dbskr/vis/dbskr_shock_patch_tableau.cxx

//:
// \file

#include <float.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_sstream.h>
#include <vcl_cstdlib.h>
#include <vgui/vgui.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_utils.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_distance.h>
#include <bvis1/bvis1_manager.h>

#include "dbskr_shock_patch_tableau.h"

#include <dbskr/dbskr_shock_patch.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/vidpro1_repository.h>

#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include <dbsk2d/pro/dbsk2d_shock_storage.h>

#include <dbsk2d/dbsk2d_shock_graph_sptr.h>

#include <bil/algo/bil_color_conversions.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vil/vil_image_resource.h>

#define DIST_THRESHOLD  1

//: Constructor
dbskr_shock_patch_tableau::dbskr_shock_patch_tableau() 
{
  gl_mode = GL_RENDER;

  bpro1_storage_sptr input_storage_sptr = bvis1_manager::instance()->repository()->get_data("image",0,0);
  vidpro1_image_storage_sptr frame;
  frame.vertical_cast(input_storage_sptr);
  vil_image_resource_sptr image_sptr = frame->get_image();
  I_ = image_sptr->get_view(0, image_sptr->ni(), 0, image_sptr->nj() );

  //make sure these images are one plane images
  if (I_.nplanes() != 3){
    color_image_ = false;
  } else {
    convert_RGB_to_Lab(I_, L_, A_, B_);
    color_image_ = true;
  }

  //display flags
  display_outer_boundary_ = true;
  display_all_ = false;

  display_real_boundaries_ = true;
  display_traced_boudary_ = true;

  gesture_select_point_ = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  next_patch_ = vgui_event_condition(vgui_MIDDLE, vgui_MODIFIER_NULL, true);
  create_storage_ = vgui_event_condition(vgui_MIDDLE, vgui_SHIFT, true);
  current_patch_ = 0;
  shock_pruning_threshold_ = 0.8;

  //fill in the randomized color table
  for (int i=0; i<100; i++){
    for (int j=0; j<3;j++)
      rnd_color[i][j] = (vcl_rand() % 256)/256.0;
  }
  
}

bool dbskr_shock_patch_tableau::handle( const vgui_event & e )
{
  if( e.type == vgui_DRAW )
      draw_render();

  if (gesture_select_point_(e)) {

    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix_, iy_);
    //vcl_cout << "ix: " << ix_ << " iy: " << iy_ << " ";
    mouse_pt_ = vgl_point_2d<double>(ix_, iy_);
    display_all_ = false;

    bool found_it = false;
    current_patch_ = 0;
    for (unsigned i = 0; i < patches_.size(); i++) {
      if (patches_[i]->inside(ix_, iy_)) {
        current_patch_ = patches_[i];
        found_it = true;
        break;
      }
    }
    if (!found_it)
      vcl_cout << "Current mouse position is not covered by any of the shock patches!\n";

    if (current_patch_) {
      if (!current_patch_->get_traced_boundary())
        current_patch_->trace_outer_boundary();
      if (color_image_ && current_patch_->color_contrast() < 0) 
        current_patch_->find_color_contrast(L_, A_, B_, 5.0);
      else if (current_patch_->app_contrast() < 0)
        current_patch_->find_grey_contrast(I_, 5.0);
      current_patch_->output_info();
    }
   
    bvis1_manager::instance()->post_redraw();
  }

  if (next_patch_(e)) {
    
    int ii = -1;
    for (unsigned i = 0; i < patches_.size(); i++) {
      if (current_patch_ == patches_[i]) {
        ii = i;
        break;
      }
    }

    bool found_it = false;
    for (int i = ii+1; i < int(patches_.size()); i++) {
      if (patches_[i]->inside(ix_, iy_)) {
        current_patch_ = patches_[i];
        found_it = true;
        break;
      }
    }
    if (!found_it) {
      for (int i = 0; i < ii; i++) {
        if (patches_[i]->inside(ix_, iy_)) {
          current_patch_ = patches_[i];
          found_it = true;
          break;
        }
      }
    }
    if (!found_it)
      vcl_cout << "No next!\n";

    if (current_patch_) {
      if (!current_patch_->get_traced_boundary())
        current_patch_->trace_outer_boundary();
      if (color_image_ && current_patch_->color_contrast() < 0) 
        current_patch_->find_color_contrast(L_, A_, B_, 5.0);
      else if (current_patch_->app_contrast() < 0)
        current_patch_->find_grey_contrast(I_, 5.0);
      current_patch_->output_info();
    }

    bvis1_manager::instance()->post_redraw();
  }

  if (create_storage_(e) && current_patch_) {
     vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
      if(!res) {
        vcl_cout << "Could not access repository!\n";
        return false;
      }

      if (!current_patch_->get_traced_boundary())
        current_patch_->trace_outer_boundary();
        
      current_patch_->shock_pruning_threshold_ = shock_pruning_threshold_;
      
      if (!current_patch_->shock_graph())
        current_patch_->extract_simple_shock();

      if (current_patch_->get_traced_boundary() && current_patch_->shock_graph()) {
      
        vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
        vcl_set<bpro1_storage_sptr> st_set = res->get_all_storage_classes(res->current_frame());
        vcl_string name_initial = "patch_vsol";
        int len = name_initial.length();
        int max = 0;
        for (vcl_set<bpro1_storage_sptr>::iterator iter = st_set.begin();
          iter != st_set.end(); iter++) {
            if ((*iter)->type() == output_vsol->type() && 
                (*iter)->name().find(name_initial) != vcl_string::npos) {
              vcl_string name = (*iter)->name();
              vcl_string numbr = name.substr(len, 3);
              int n = vcl_atoi(numbr.c_str());
              if (n > max)
                max = n;
            }
        }
        vcl_ostringstream oss;
        oss.width(3);
        oss.fill('0');
        oss << name_initial << max+1;
        name_initial = oss.str();
        output_vsol->set_name(name_initial);
        output_vsol->add_object(current_patch_->get_traced_boundary()->cast_to_spatial_object());
        res->store_data(output_vsol);

        // add the shock as well 
        dbsk2d_shock_graph_sptr sg = current_patch_->shock_graph();  // forces computation of the shock graph if not available
        dbsk2d_shock_storage_sptr output_shock = dbsk2d_shock_storage_new();
        name_initial = "patch_shock";
        len = name_initial.length();
        max = 0;
        for (vcl_set<bpro1_storage_sptr>::iterator iter = st_set.begin();
          iter != st_set.end(); iter++) {
            if ((*iter)->type() == output_shock->type() && 
                (*iter)->name().find(name_initial) != vcl_string::npos) {
              vcl_string name = (*iter)->name();
              vcl_string numbr = name.substr(len, 3);
              int n = vcl_atoi(numbr.c_str());
              if (n > max)
                max = n;
            }
        }
        vcl_ostringstream oss2;
        oss2.width(3);
        oss2.fill('0');
        oss2 << name_initial << max+1;
        name_initial = oss2.str();
        output_shock->set_name(name_initial);
        output_shock->set_shock_graph(sg);
        res->store_data(output_shock);
#if 0 
        dbsk2d_shock_graph_sptr sg2 = current_patch_->trimmed_shock_graph();
        dbsk2d_shock_storage_sptr output_shock2 = dbsk2d_shock_storage_new();
        name_initial = "patch_shock";
        oss2 << name_initial << max+2;
        name_initial = oss2.str();
        output_shock2->set_name(name_initial);
        output_shock2->set_shock_graph(sg2);
        res->store_data(output_shock2);
#endif
        bvis1_manager::instance()->add_to_display(output_vsol);
        bvis1_manager::instance()->add_to_display(output_shock);
        bvis1_manager::instance()->display_current_frame();
      } else {
        vcl_cout << "Problems in tracing or shock extraction!\n";
        return false;
      }
  }

  return false;
}

void dbskr_shock_patch_tableau::draw_render()
{
  if (display_all_) {
    for (unsigned i = 0; i < patches_.size(); i++)
      draw_patch(patches_[i]);
  } else if (current_patch_) {
      draw_patch(current_patch_);
  }

  glColor3f( 0 , 1 , 1 );
  glPointSize( 30.0 );
  glBegin( GL_POINTS );
  glVertex2f( mouse_pt_.x(), mouse_pt_.y() );
  glEnd();
}



void dbskr_shock_patch_tableau::
draw_patch(dbskr_shock_patch_sptr shock_patch)
{
  if (display_outer_boundary_) {
    vsol_polygon_2d_sptr poly = shock_patch->get_outer_boundary();
    if (poly) {
      //glColor3f( rnd_colormap[color][0] , rnd_colormap[color][1] , rnd_colormap[color][2] );
      glColor3f( 1.0 , 1.0 , 0.0 );
      glLineWidth (3.0);
      glBegin( GL_LINE_STRIP );
      for( unsigned int i = 0 ; i < poly->size() ; i++ )
        glVertex2f( poly->vertex(i)->x(), poly->vertex(i)->y() );
      glVertex2f( poly->vertex(0)->x(), poly->vertex(0)->y() );
      glEnd();
    }
  }

  if (display_real_boundaries_) {
    vcl_vector<vsol_polyline_2d_sptr>& rbs = shock_patch->get_real_boundaries();
    int color = 0;
    for (unsigned i = 0; i < rbs.size(); i++) {
      vsol_polyline_2d_sptr poly = rbs[i];
      //glColor3f( 0.0 , 0.0 , 1.0 );
      if (i == 0)
        glColor3f( 1.0 , 0.0 , 0.0 );
      else if (i%2 == 0)
        glColor3f( 0.0 , 1.0 , 0.0 );
      else
        glColor3f( 0.0 , 0.0 , 1.0 );
      //glColor3f( rnd_color[color][0] , rnd_color[color][1] , rnd_color[color][2] );
      glLineWidth (4.0);
      glBegin( GL_LINE_STRIP );
      for( unsigned int i = 0 ; i < poly->size() ; i++ )
        glVertex2f( poly->vertex(i)->x(), poly->vertex(i)->y() );
      glEnd();
      color = (color + 1)%100;
    }
  }

  if (display_traced_boudary_) {
    vsol_polygon_2d_sptr poly = shock_patch->get_traced_boundary();
    if (poly) {
      //glColor3f( rnd_colormap[color][0] , rnd_colormap[color][1] , rnd_colormap[color][2] );
      glColor3f( 1.0 , 0.0 , 1.0 );
      glLineWidth (3.0);
      glBegin( GL_LINE_STRIP );
      for( unsigned int i = 0 ; i < poly->size() ; i++ )
        glVertex2f( poly->vertex(i)->x(), poly->vertex(i)->y() );
      glVertex2f( poly->vertex(0)->x(), poly->vertex(0)->y() );
      glEnd();
    }
  }
}

void 
dbskr_shock_patch_tableau::get_popup(const vgui_popup_params& params, vgui_menu &menu)
{
  vgui_menu submenu;
  vcl_string on = "[x] ", off = "[ ] ";

  submenu.add( ((display_all_)?on:off)+"Display all patches", 
               new dbskr_sp_tableau_toggle_command(this, &display_all_));

  submenu.add( ((display_outer_boundary_)?on:off)+"Display outer boundary", 
               new dbskr_sp_tableau_toggle_command(this, &display_outer_boundary_));

  submenu.add( ((display_real_boundaries_)?on:off)+"Display real boundaries", 
               new dbskr_sp_tableau_toggle_command(this, &display_real_boundaries_));

  submenu.add( ((display_traced_boudary_)?on:off)+"Display traced boundary", 
               new dbskr_sp_tableau_toggle_command(this, &display_traced_boudary_));

  submenu.add( "Set shock pruning threshold", 
               new dbskr_sp_tableau_set_display_params_command(this, "Pruning threshold ", &shock_pruning_threshold_));

  //add this submenu to the popup menu
  menu.add("Matching Display Options", submenu);
}
