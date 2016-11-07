#include "epip_app.h"
#include "mw_app.h"
#include "mw_util.h"
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau_sptr.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vgui/vgui_projection_inspector.h>
#include <bvis1/bvis1_manager.h>
#include <vgui/vgui_find.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_tableau_sptr.h>
#include <vgui/vgui_tableau.h>

#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <vil/vil_load.h>
#include <vil/vil_image_resource_sptr.h>

static bool was_called_=false;

void
epip_interactive()
{
   // ========= READING IMAGE AND CAMERA INFO =============

   // dialog to ask for 2 filenames

   vcl_string fname1("tst1/p1010053.jpg");
   vcl_string fname2("tst1/p1010047.jpg");


   // Reading images
   vil_image_resource_sptr img1 = vil_load_image_resource( fname1.c_str() );
   if( !img1 ) {
     vcl_cerr << "Failed to load image file" << fname1 << vcl_endl;
     return;
   }

   vil_image_resource_sptr img2 = vil_load_image_resource( fname2.c_str() );
   if( !img2 ) {
     vcl_cerr << "Failed to load image file" << fname2 << vcl_endl;
     return;
   }
   
   // Create the storage data structures
   vcl_string itype("image");
   vcl_string iname("cur_image");
   bpro1_storage_sptr img_data = MANAGER->repository()->new_data(itype,iname);
   if (img_data) {
      vidpro1_image_storage_sptr img_storage;
      img_storage.vertical_cast(img_data);
      img_storage->set_image(img1);
      MANAGER->add_to_display(img_data);
   } else {
      vcl_cerr << "error: unable to register new data\n";
      return;
   }

   vcl_string iname2("nxt_image");
   img_data = MANAGER->repository()->new_data(itype,iname2);
   if (img_data) {
      vidpro1_image_storage_sptr img_storage;
      img_storage.vertical_cast(img_data);
      img_storage->set_image(img2);
      MANAGER->add_to_display(img_data);
      MANAGER->display_current_frame();
   } else {
      vcl_cerr << "error: unable to register new data\n";
      return;
   }


   // Reading cameras
   vpgl_perspective_camera <double> *P1,*P2;

   P1 = new vpgl_perspective_camera<double>;
   P2 = new vpgl_perspective_camera<double>;

   if (!read_cam(fname1,fname2,P1,P2)) {
      vcl_cerr << "epip_interactive: error reading cam\n";
      return;
   }

   // Fundamental matrix
   vpgl_fundamental_matrix<double> *fm = new vpgl_fundamental_matrix <double> (*P1,*P2);

   // ========= GUI STUFF =============

   // add grid if only one present
   vgui_grid_tableau_sptr pgrid;
   pgrid.vertical_cast(vgui_find_below_by_type_name(MANAGER,"vgui_grid_tableau"));

   if (pgrid->rows()*pgrid->cols() <= 1)
      MANAGER->add_new_view(0, false);
   MANAGER->display_current_frame();


   // ADD TABLEAUS TO DRAW ON

   vcl_string type("vsol2D");
   vcl_string name("cur_vsol2D");
   bpro1_storage_sptr n_data = MANAGER->repository()->new_data(type,name);
   if (n_data) {
      MANAGER->add_to_display(n_data);
      MANAGER->display_current_frame();
   } else {
      vcl_cerr << "error: unable to register new data\n";
      return;
   }

   vcl_string type2("vsol2D");
   vcl_string name2("nxt_vsol2D");
   n_data = MANAGER->repository()->new_data(type2,name2);
   if (n_data) {
      MANAGER->add_to_display(n_data);
      MANAGER->display_current_frame();
   } else {
      vcl_cerr << "error: unable to register new data\n";
      return;
   }
   
   // Add event-handling tableau
   vgui_shell_tableau_sptr psh;
   psh.vertical_cast(vgui_find_below_by_type_name(MANAGER,"vgui_shell_tableau"));

   epip_drawer_tableau *pdraw;
   vgui_tableau_sptr spdraw = epip_drawer_tableau_new();

   pdraw = dynamic_cast <epip_drawer_tableau *> (spdraw.ptr());

   psh->add(spdraw);
   
   vgui_viewer2D_tableau_sptr vt;
   vt.vertical_cast(pgrid->get_tableau_at(0,0));
   pdraw->set_l_view(vt);
   vt.vertical_cast(pgrid->get_tableau_at(1,0));
   pdraw->set_r_view(vt);

   // Makes curr vsol invisible in next view
   vgui_tableau_sptr ptab;
   ptab = vgui_find_below_by_type_name(pgrid->get_tableau_at(1,0),"bgui_selector_tableau");
   bgui_selector_tableau_sptr selector;
   selector.vertical_cast(ptab);
   selector->toggle(name);
   selector->toggle(iname);
   selector->set_active(name2);
   vgui_easy2D_tableau_sptr peasy;
   peasy.vertical_cast(selector->active_tableau());
   pdraw->set_r_tableau(peasy);

   // Makes nxt vsol invisible in curr view
   ptab = vgui_find_below_by_type_name(pgrid->get_tableau_at(0,0),"bgui_selector_tableau");
   selector.vertical_cast(ptab);
   selector->toggle(name2);
   selector->toggle(iname2);
   selector->set_active(name);
   peasy.vertical_cast(selector->active_tableau());
   pdraw->set_l_tableau(peasy);

   pdraw->set_l_camera(P1);
   pdraw->set_r_camera(P2);
   pdraw->set_f_matrix(fm);

   MANAGER->post_redraw();
}

epip_drawer_tableau::
epip_drawer_tableau() : pt_(0), soview_r_(0), tab_l_(0),tab_r_(0),activated_(false),first_time_(true)
{
  gesture0 = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
}

bool epip_drawer_tableau::
handle(const vgui_event &e)
{
   if (!tab_l_)
     return vgui_tableau::handle(e);


   if (first_time_) {
      first_time_ = false;
      // project world origin into both images
      vgl_homg_point_3d<double> orig(0,0,0);
      vgl_point_2d<double> gama1 = p_l_->project(orig);
      vcl_cout << "Gama1:" << gama1 << vcl_endl;
      vgl_point_2d<double> gama2 = p_r_->project(orig);
      vcl_cout << "Gama2:" << gama2 << vcl_endl;

      vcl_cout << "\nProjective matrices:\n";
      vcl_cout << *p_l_ << vcl_endl << vcl_endl;
      vcl_cout << p_l_->get_matrix() << vcl_endl;

      vcl_cout << *p_r_ << vcl_endl;
      vcl_cout << p_r_->get_matrix() << vcl_endl;

      // add gama1 and gama2 as blue points
      tab_l_->set_foreground(0,0,1);
      tab_l_->set_point_radius(5);
      tab_l_->set_current_grouping( "Drawing" );
      tab_l_->add_point(gama1.x(),gama1.y());
      tab_l_->set_current_grouping( "default" );
      tab_l_->post_redraw();

      tab_r_->set_foreground(0,0,1);
      tab_r_->set_point_radius(5);
      tab_r_->set_current_grouping( "Drawing" );

      tab_r_->add_point(gama2.x(),gama2.y());
      tab_r_->set_current_grouping( "default" );
      tab_r_->post_redraw();
   }

   float ix, iy;
   vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
   ix = e.wx - view_r_->token.offsetX;

   if (e.button  == vgui_LEFT) {
      activated_ = !activated_;
      vcl_cout << "Offset: " << view_r_->token.offsetX << " " << view_r_->token.offsetY << vcl_endl;
      vcl_cout << "My point: " << ix << ", " << iy << vcl_endl;
      vcl_cout << "Event: " << e.wx << ", " << e.wy << vcl_endl;
   }

   if (gesture0(e) || (activated_ && e.type == vgui_MOTION)) {
      if (pt_) {
         tab_l_->remove(pt_);
         tab_r_->remove(soview_r_);
      }
      tab_l_->set_foreground(1,0.2,0.2);
      tab_l_->set_point_radius(5);
      tab_l_->set_current_grouping( "Drawing" );
      pt_ = tab_l_->add_point(ix,iy);
      tab_l_->set_current_grouping( "default" );
      tab_l_->post_redraw();

      vgl_homg_point_2d<double> img1_pt(ix,iy);
      vgl_homg_line_2d<double> l;
      l = fm_->image2_epipolar_line(img1_pt);
      tab_r_->set_foreground(1,0,0);
      tab_r_->set_current_grouping( "Drawing" );
      soview_r_ = tab_r_->add_infinite_line(l.a(),l.b(),l.c());
      tab_r_->set_current_grouping( "default" );
      tab_r_->post_redraw();
      return true;
   }

   //  We are not interested in other events,
   //  so pass event to base class:
   return vgui_tableau::handle(e);
}



//: just a simple test to see if pt gets transferred
void
point_transfer_app()
{
  vgl_homg_point_2d<double> p1,p2,p3;

  vcl_string fname1("curr/p1010049.jpg");
  vcl_string fname2("curr/p1010053.jpg");
  vcl_string fname3("curr/p1010069.jpg");

  // Reading cameras
  vpgl_perspective_camera <double> Pr1,Pr2,Pr3;

  if (!read_cam(fname1,fname2,&Pr1,&Pr2)) {
     vcl_cerr << "epip_interactive: error reading cam\n";
     return;
  }

  if (!read_cam(fname3,&Pr3)) {
     vcl_cerr << "epip_interactive: error reading cam\n";
     return;
  }

  vpgl_fundamental_matrix<double> f13(Pr1,Pr3);
  vpgl_fundamental_matrix<double> f23(Pr2,Pr3);


  p1.set(1395,211);
  p2.set(1811,319);

  p3 = mw_epipolar_point_transfer(p1,p2,f13,f23);

  vcl_cout << "P3: " << p3.x()/p3.w() << "  " << p3.y()/p3.w() <<  vcl_endl;
}
