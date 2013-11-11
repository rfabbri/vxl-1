#include "mw_correspond_point_tool.h"

#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <bgui/bgui_vsol_soview2D.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vil/vil_image_resource.h>

#include <vidpro1/vidpro1_repository.h>
#include <bpro1/bpro1_storage_sptr.h>
#define MANAGER bvis1_manager::instance()

#include <dvpgl/pro/dvpgl_camera_storage.h>

#include <vgui/vgui.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <bgui/bgui_vsol_soview2D.h>

#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>

#include <mw/mw_util.h>
#include <mw/mw_subpixel_point_set.h>
#include <mw/mw_epi_interceptor.h>
//#include <mw/algo/mw_point_matcher.h>

#include <mw/pro/mw_discrete_corresp_storage.h>

#include <mw/algo/dbdif_data.h>

#include <dbgl/algo/dbgl_eulerspiral.h>



mw_correspond_point_tool::
mw_correspond_point_tool()
{
  gesture_query_corresp_= vgui_event_type(vgui_MOUSE_MOTION);
  corresp_edges_style_  = vgui_style::new_style(0.5, 0.5, 1, 
                                    5.0f/*ptsize*/, 3.0f/*linesize*/); // light Blue

  wrongly_matched_edgels_style_ = vgui_style::new_style(1, 0, 1, 
                                    5.0f/*ptsize*/, 3.0f/*linesize*/); // Magenta


  best_match_style_.push_back(vgui_style::new_style(1, 1, 0, 1.0f, 3.0f));   // Yellow
  best_match_style_.push_back(vgui_style::new_style(1, 0.5, 0, 1.0f, 3.0f)); // Orange
  best_match_style_.push_back(vgui_style::new_style(1, 0, 1, 1.0f, 3.0f));   // Magenta
  best_match_style_.push_back(vgui_style::new_style(0, 1, 1, 1.0f, 3.0f));   // Green blue

  p0_query_style_ = vgui_style::new_style(0, 0, 1, 
                                    5.0f/*ptsize*/, 8.0f/*linesize*/); // Blue

  p1_query_style_ = vgui_style::new_style(0, 0, 1, 
                                    5.0f/*ptsize*/, 8.0f/*linesize*/); // Blue

  es_style_ = vgui_style::new_style(1, 1, 1, 
                                    5.0f/*ptsize*/, 2.0f/*linesize*/); // White

  es_style_gt_ = vgui_style::new_style(0.7, 0.7, 0.7, 
                                    5.0f/*ptsize*/, 2.0f/*linesize*/); // Gray

  srm_allcrvs_style_ = vgui_style::new_style(7, 0, 0, 
                                    5.0f/*ptsize*/, 1.5f/*linesize*/); // White

  //: basic stuff is in layer 50
  //: higher numbers mean higher layers
  best_match_layer_ = "layer91";
  corresp_edges_layer_ = "layer60";
  p0_query_layer_ = "layer85";
  p1_query_layer_ = "layer85";
  es_layer_ = "layer70";
  es_layer_gt_ = "layer69";
  srm_allcrvs_layer_ = "layer65";

  angle_cam_.resize(nviews_);

  es_gt_ = 0;


//  es_.resize(nviews_,0);
  
}

vcl_string mw_correspond_point_tool::
name() const
{
  return "Multiview point correspond";
}

void   
mw_correspond_point_tool::
activate ()
{
  mw_correspond_point_tool_basic::activate();

  //----------
  for (unsigned i=0; i < es_.size(); ++i)  {
    if (es_[i]) {
      delete es_[i];
    }
  }
  es_.clear();

  if (es_gt_) {
    delete es_gt_;
    tab_[2]->remove(es_so_gt_);
    es_gt_=0;
  }

  lock_corresp_query_ = false;
  p0_query_soview_ = 0;
  p1_query_soview_ = 0;
  p1_query_is_candidate_ = false;
  synthetic_ = false;
  synthetic_olympus_ = false;
  srm_display_full_ = false;
  srm_display_es_   = true;

  p0_corresp_soview_.resize(vsols_[0].size(),0);



  { // get correspondence storage at view 1


    //Prompt the user to select input/output variable
    vgui_dialog io_dialog("Select Inputs" );


    vcl_vector< vcl_string > input_type_list;
    input_type_list.push_back("mw pt corresp");

    io_dialog.message("Select Input(s) From Available ones:");
    vcl_vector<int> input_choices(input_type_list.size());
    vcl_vector< vcl_vector <vcl_string> > available_storage_classes(input_type_list.size());
    vcl_vector< vcl_string > input_names(input_type_list.size());
         
    for( unsigned int i = 0 ; i < input_type_list.size(); i++ )
    {
      //for this input type allow user to select from available storage classes in the repository
      available_storage_classes[i] = MANAGER->repository()->get_all_storage_class_names(input_type_list[i]);
      available_storage_classes[i].push_back("(NONE)");

      //Multiple choice - with the list of options.
      io_dialog.choice(input_type_list[i].c_str(), available_storage_classes[i], input_choices[i]);
    }

    io_dialog.checkbox("Deal with synthetic data         CTSPHERES", synthetic_);
    io_dialog.checkbox("Deal with synthetic data DIGICAM TURNTABLE", synthetic_olympus_);

    if (!io_dialog.ask()) {
      vcl_cout << "Canceled\n";
      return;
    } else {
      vgui_dialog null_inputs("Name Missing Inputs");
      bool found_missing = false;
      for ( unsigned int i=0; i < input_choices.size(); i++) {
        if( input_choices[i]+1 < (int)available_storage_classes[i].size() ) {
          input_names[i] = available_storage_classes[i][input_choices[i]];
          null_inputs.message((input_type_list[i]+" : "+input_names[i]).c_str());
        }
        else{
          null_inputs.field(input_type_list[i].c_str(), input_names[i]);
          found_missing = true;
        }
      }
      if(found_missing)
        null_inputs.ask();
    }

    vcl_cout << "Selected input: " << input_names[0] << " ";
    
    bpro1_storage_sptr 
      p = MANAGER->repository()->get_data_by_name_at(input_names[0],frame_v_[0]);

    mw_discrete_corresp_storage_sptr p_sto;

    p_sto.vertical_cast(p);
    if(!p) {
      vcl_cerr << "Error: tool requires a valid correspondence storage" << vcl_endl;
      return;
    }
    
    corr_ = p_sto->corresp();
    if(!corr_) {
      vcl_cerr << "Empty storage - allocating data" << vcl_endl;
      corr_ = new mw_discrete_corresp(vsols_[0].size(),vsols_[1].size());
      p_sto->set_corresp(corr_); // storage deletes it
    } else {
      if (corr_->size() != vsols_[0].size()) {
        vcl_cerr << "Error: input correspondence is not valid for current edgels\n" 
          << vcl_endl;
        vcl_cerr << "corresp size: " << corr_->size() << vcl_endl;
        vcl_cerr << "vsols size: " << vsols_[0].size() << vcl_endl;
        return;
      }
    }

    vcl_cout << "Corresp NAME: " << p_sto->name() << vcl_endl;
    // vcl_cout << "Corresp: " << " : \n" << *corr_ << vcl_endl;
  }

  // get image storage at all 3 views
  /*
  for (unsigned i=0; i < nviews_; ++i) {
    bpro1_storage_sptr 
      p = MANAGER->repository()->get_data_at("image",frame_v_[i]);

    vidpro1_image_storage_sptr frame_image;
    frame_image.vertical_cast(p);

    if (!frame_image) {
      if (!synthetic_ && !synthetic_olympus_) {
        vcl_cout << "ERROR: no images in view " << i << vcl_endl;
        return;
      }
    } else {
      images_.push_back(frame_image->get_image());
      ncols_ = images_[i]->ni();
      nrows_ = images_[i]->nj();

      vcl_cout << "Image view # " << i << "  ";
      vcl_cout << "Nrows: " << nrows_;
      vcl_cout << "  Ncols: " << ncols_ << vcl_endl;
    }
  }
  */


  // Generate synthetic ground-truth for inspection, if requested
  if (synthetic_ || synthetic_olympus_) {
    // generate spc curve & store it

    vcl_cout << "Generating curves..."; vcl_cout.flush();

    if (synthetic_) {
      dbdif_data::space_curves_ctspheres( crv3d_gt_ );
      // hardcode nrows, ncols:
      nrows_ = 314;
      ncols_ = 600;
    } else  {
      dbdif_data::space_curves_olympus_turntable( crv3d_gt_ );
      nrows_ = 400;
      ncols_ = 500;
      angle_cam_[0] = 0;
      angle_cam_[1] = 5;
      angle_cam_[2] = 60;
      unsigned  crop_origin_x_ = 450;
      unsigned  crop_origin_y_ = 1750;
      dbdif_turntable::internal_calib_olympus(srm_K_, ncols_, crop_origin_x_, crop_origin_y_);
      vpgl_calibration_matrix<double> K(srm_K_);
      cam_gt_.resize(nviews_);
      for (unsigned i=0; i < nviews_; ++i) {
        vpgl_perspective_camera<double> *P = 
              dbdif_turntable::camera_olympus(angle_cam_[i], K);
        cam_gt_[i].set_p(*P);
      }
    }


    dbdif_data::project_into_cams_without_epitangency(crv3d_gt_, cam_gt_, crv2d_gt_, vnl_math::pi/6.0);
    vcl_cout << "done.\n";

    gt_.set_size(crv2d_gt_[0].size(), crv2d_gt_[1].size());
    for (unsigned i=0; i < crv2d_gt_[0].size(); ++i) {
      gt_.corresp_[i].push_back(mw_attributed_object(i));
    }
  }

  // coloring of points in image 0 having any correspondents

  tab_[0]->set_current_grouping(corresp_edges_layer_.c_str());
  for (unsigned i=0; i < corr_->corresp_.size(); ++i) {
    if (! corr_->corresp_[i].empty() ) {
      p0_corresp_soview_[i] = 
        tab_[0]->add_vsol_line_2d(vsols_orig_cache_[0][i]);

      if (!gt_.is_empty()) {
        if (corr_->is_gt_among_top5(i, &gt_)) {
          p0_corresp_soview_[i]->set_style(corresp_edges_style_);
        } else {
          p0_corresp_soview_[i]->set_style(wrongly_matched_edgels_style_);
        }
      } 
    }
  }
  tab_[0]->post_redraw();
}

void   
mw_correspond_point_tool::
deactivate ()
{
  vcl_cout << "mw_correspond_point_tool OFF\n";
  mw_correspond_point_tool_basic::deactivate ();
}

bool mw_correspond_point_tool::
handle( const vgui_event & e, 
        const bvis1_view_tableau_sptr& view )
{

  if (e.type == vgui_KEY_PRESS) {
    vcl_cout << "Frame index: " << view->frame() << vcl_endl;
    return handle_key(e.key);
  }

  //: mouse motion
  if (e.type == gesture_query_corresp_) {
    if ( handle_corresp_query_whatever_view(e,view) ) {
      if ( view->frame() == frame_v_[0] ) {
        if (!lock_corresp_query_)
          handle_corresp_query_at_view_0(e,view);
      }
    }
  }


  if (gesture0_(e)) {
    if ( !handle_mouse_event_whatever_view(e,view) )
      return false;


    if (view->frame() == frame_v_[0]) {
      handle_mouse_event_at_view_0(e,view);

    } else if (view->frame() == frame_v_[1]) {
      handle_mouse_event_at_view_1(e,view);
    } else { // view 3
      vcl_cout << "Processing a point in third view" << vcl_endl;
    }

  }

  /*
  if( e.type == vgui_DRAW && es_.size())
  {
    for (unsigned ie=0; ie < es_.size(); ++ie) {
      glLineWidth(2);
      glColor3f(1.0f,1.0f,1.0f);
      glBegin(GL_LINE_STRIP);
      for (double s=-4.0; s<0; s+=0.1)
        glVertex2f(es_[ie]->point_at_length(s).x(), es_[ie]->point_at_length(s).y());
      glEnd();

      glColor3f(1.0f,1.0f,1.0f);
      glBegin(GL_LINE_STRIP);
      for (double s=0; s<4.0; s+=0.1)
        glVertex2f(es_[ie]->point_at_length(s).x(), es_[ie]->point_at_length(s).y());
      glEnd();
    }
    */

  //  We are not interested in other events,
  //  so pass event to base class:
  return false;
}

bool mw_correspond_point_tool::
handle_key(vgui_key key)
{
  bool base_stat = mw_correspond_point_tool_basic::handle_key(key);

  switch (key) {

    case 277: // Del
      if (p1_query_is_candidate_) {
        vcl_cout << "Removing " <<  p0_query_idx_ << ",  " << *p1_query_itr_ <<  vcl_endl;
        corr_->corresp_[p0_query_idx_].erase(p1_query_itr_);
        p1_query_is_candidate_ = false;

        // - delete this guy from correspondents soview list

        tab_[1]->remove(*(correspondents_idx_[p1_query_idx_]));

        correspondents_soview_.erase(correspondents_idx_[p1_query_idx_]);
        correspondents_idx_.erase(p1_query_idx_);
        tab_[1]->remove(p1_query_soview_);
        p1_query_soview_ = 0;

        if (corr_->corresp_[p0_query_idx_].empty() && 
            p0_corresp_soview_[p0_query_idx_] != 0) {
          tab_[0]->remove(p0_corresp_soview_[p0_query_idx_]);
          p0_corresp_soview_[p0_query_idx_] = 0;
        }

        tab_[1]->post_redraw();
        tab_[0]->post_redraw();
      } else {
        vcl_cout << "Selected correspondence inexist\n";
      }

      return true;
      break;

    case 278: // Ins
      if (!p1_query_is_candidate_) {

        vcl_list<mw_attributed_object>::iterator itr;
        bool  stat = 
          corr_->add_unique( mw_attributed_object(p1_query_idx_), p0_query_idx_, &itr);

        if (stat) {
          vcl_cout << "Inserting " << p0_query_idx_ << ",  " << *itr << vcl_endl;

          p1_query_is_candidate_ = true;

          p1_query_itr_ = itr;
          p1_query_idx_ = itr->obj_;

          tab_[1]->set_current_grouping( corresp_edges_layer_.c_str() );
          // - add this guy to correspondents soview list; breaking sort order
          correspondents_soview_.push_back(
              tab_[1]->add_vsol_line_2d(vsols_orig_cache_[1][itr->obj_]));
          correspondents_soview_.back()->set_style(corresp_edges_style_);
          correspondents_idx_[p1_query_idx_] = --correspondents_soview_.end();

          tab_[0]->set_current_grouping(corresp_edges_layer_.c_str() );
          //p1_query_soview_  remains the same as before
          if (p0_corresp_soview_[p0_query_idx_] == 0) {
            p0_corresp_soview_[p0_query_idx_] = tab_[0]->add_vsol_line_2d(
                vsols_orig_cache_[0][p0_query_idx_]);
            p0_corresp_soview_[p0_query_idx_]->set_style(corresp_edges_style_);
          }

          tab_[1]->post_redraw();
          tab_[0]->post_redraw();
        } else {
          vcl_cout << "Error/Insert: point is already in corresp datastructure\n";
          vcl_cout << "p1_query_idx_:" << p1_query_idx_ << "  p0_query_idx_:" 
            << p0_query_idx_ << vcl_endl;
        }
      } else {
        vcl_cout << "Selected correspondence already inserted\n";
      }
      break;

    case 'p': // print misc info / debug
      vcl_cout << *corr_ << vcl_endl;

      return true;
      break;
    case 's': // fill corresp. structure with stereo results
      vcl_cout << "Epipolar constraint";
      run_stereo_matcher();

      return true;
      break;
    case 'l': // misc. stuff
      toggle_lock_correspondence_query();
      return true;
      break;

    case '.': {
      srm_angle_+=1;
      vgui::out << "srm view angle: " << srm_angle_ << "deg\n";

      srm_draw_eulerspiral();
    }
    break;

    case ',': {
      srm_angle_-=1;
      vgui::out << "srm view angle: " << srm_angle_ << "deg\n";

      srm_draw_eulerspiral();
    }
    break;

    case '>': {
      srm_angle_ = angle_cam_[1];
      vgui::out << "srm view angle: " << srm_angle_ << "deg\n";

      srm_draw_eulerspiral();
    }
    break;

    case '<': {
      srm_angle_ = angle_cam_[0];
      vgui::out << "srm view angle: " << srm_angle_ << "deg\n";

      srm_draw_eulerspiral();
    }
    break;

    case 5: { /* Ctrl-E */
      srm_display_full_ = !srm_display_full_;
      vgui::out << "Display srm reproj " << ( (srm_display_full_)? "ON" : "OFF") << vcl_endl;
    }
    break;

    case '^': /* Shift-6 */
      // - Display eulerspiral off
      srm_display_es_ = !srm_display_es_;
      vgui::out << "Display Eulerspirals " << ( (srm_display_es_)? "ON" : "OFF") << vcl_endl;
    break;
    case '6':

      for (unsigned  i=0; i < es_.size(); ++i) {
        if (es_[i]) {
          delete es_[i];
          tab_[i]->remove(es_so_[i]);
        }
      }
      es_.clear();

      if (es_gt_) {
        delete es_gt_;
        tab_[2]->remove(es_so_gt_);
        es_gt_=0;
      }

      vcl_cout << "6 pressed\n";
      if (srm_display_es_) { // TODO: also make sure some point is selected 
        vcl_cout << "Drawing Eulerspirals\n";
        vgui::out << "Drawing Eulerspirals\n";
        srm_angle_= angle_cam_[2];

//        vpgl_calibration_matrix<double> K(srm_K_);
//        vpgl_perspective_camera<double> *P = 
//              dbdif_turntable::camera_olympus(srm_angle_, K);
        srm_cam_.set_p(cam_[2].Pr_);

        dbdif_3rd_order_point_2d p1,p2;
        if (crv2d_gt_.size()) {
          // get hold of p1, theta1, k1, kdot1
          p1 = crv2d_gt_[0][p0_query_idx_];

          // get hold of p2, theta2, k2, kdot2
          p2 = crv2d_gt_[1][p1_query_idx_];
        } else {
          // - get hold of p1, theta1
          // - set k1, kdot1 to zero

          p1.gama[0]  = vsols_[0][p0_query_idx_]->x();
          p1.gama[1]  = vsols_[0][p0_query_idx_]->y();
          p1.gama[2]  = 0;
          
          vgl_vector_2d<double> dir = vsols_orig_cache_[0][p0_query_idx_]->direction();

          p1.t[0] = dir.x();
          p1.t[1] = dir.y();
          p1.t[2] = 0;
          p1.n[0] = -dir.y();
          p1.n[1] = dir.x();
          p1.n[2] = 0;
          p1.k = 0; 
          p1.kdot = 0;
          p1.valid = true;

          p2.gama[0]  = vsols_[1][p1_query_idx_]->x();
          p2.gama[1]  = vsols_[1][p1_query_idx_]->y();
          p2.gama[2]  = 0;
          
          dir = vsols_orig_cache_[1][p1_query_idx_]->direction();

          p2.t[0] = dir.x();
          p2.t[1] = dir.y();
          p2.t[2] = 0;
          p2.n[0] = -dir.y();
          p2.n[1] = dir.x();
          p2.n[2] = 0;
          p2.k = 0; 
          p2.kdot = 0;
          p2.valid = true;
        }

        dbdif_3rd_order_point_2d p1_w, p2_w;

        // get hold of Prec
        dbdif_rig rig(cam_[0].Pr_,cam_[1].Pr_);

        rig.cam[0].img_to_world(&p1,&p1_w);
        rig.cam[1].img_to_world(&p2,&p2_w);

        rig.reconstruct_3rd_order(p1_w, p2_w, &srm_Prec_);

//        vcl_cout << "HERE: Prec.K: " << srm_Prec_.K
//                 << " Prec.T[0]: " << srm_Prec_.T[0] 
//                 << " Prec.T[1]: " << srm_Prec_.T[1]
//                 << " Prec.T[2]: " << srm_Prec_.T[2]
//                 << " Prec.Gama[0]: " << srm_Prec_.Gama[0] 
//                 << " Prec.Gama[1]: " << srm_Prec_.Gama[1]
//                 << " Prec.Gama[2]: " << srm_Prec_.Gama[2] << vcl_endl;
        
        // theta1 = theta(view3) 

        bool valid;
        dbdif_3rd_order_point_2d p_rec_reproj = srm_cam_.project_to_image(srm_Prec_,&valid);

//        vcl_cout << "HERE: p_rec_reproj.K: " << p_rec_reproj.k
//                 << " p_rec_reproj.T[0]: " << p_rec_reproj.t[0] 
//                 << " p_rec_reproj.T[1]: " << p_rec_reproj.t[1]
//                 << " p_rec_reproj.T[2]: " << p_rec_reproj.t[2]
//                 << " p_rec_reproj.Gama[0]: " << p_rec_reproj.gama[0] 
//                 << " p_rec_reproj.Gama[1]: " << p_rec_reproj.gama[1]
//                 << " p_rec_reproj.Gama[2]: " << p_rec_reproj.gama[2] << vcl_endl;

        dbdif_3rd_order_point_3d gt_Prec;
        dbdif_3rd_order_point_2d p_rec_reproj_gt;
        if (synthetic_ || synthetic_olympus_) {
          // same for g-t cameras
          //
          // get hold of Prec
          dbdif_rig rig_gt(cam_gt_[0].Pr_,cam_gt_[1].Pr_);

          rig_gt.cam[0].img_to_world(&p1,&p1_w);
          rig_gt.cam[1].img_to_world(&p2,&p2_w);

          rig_gt.reconstruct_3rd_order(p1_w, p2_w, &gt_Prec);
          
          // theta1 = theta(view3) 

          bool valid_gt;
          p_rec_reproj_gt = cam_gt_[2].project_to_image(gt_Prec,&valid_gt);

          if (valid_gt)
             es_gt_ = get_new_eulerspiral (p_rec_reproj_gt);

          { // construct eulerspirals for true cams
            vcl_vector<vsol_point_2d_sptr> vp;
            vp.reserve((unsigned)vcl_ceil(24.0/0.1));
            for (double s=-12.0; s<12.0; s+=0.1)
              vp.push_back(
                  new vsol_point_2d(es_gt_->point_at_length(s).x(), es_gt_->point_at_length(s).y()));

            vsol_polyline_2d_sptr vpoly = new vsol_polyline_2d(vp);

            tab_[2]->set_current_grouping(es_layer_gt_.c_str());
            es_so_gt_ = tab_[2]->add_vsol_polyline_2d(vpoly);
            es_so_gt_->set_style(es_style_gt_);
          }
        }
        
        // construct eulerspirals 
        es_.push_back(get_new_eulerspiral (p1)); 
        es_.push_back(get_new_eulerspiral (p2)); 
        if (valid)
           es_.push_back(get_new_eulerspiral (p_rec_reproj)); 

        es_so_.resize(es_.size());
        for (unsigned ie=0; ie < es_.size(); ++ie) {
          vcl_vector<vsol_point_2d_sptr> vp;

          vp.reserve((unsigned)vcl_ceil(24.0/0.1));
          for (double s=-12.0; s<12.0; s+=0.1)
            vp.push_back(
                new vsol_point_2d(es_[ie]->point_at_length(s).x(), es_[ie]->point_at_length(s).y()));

          vsol_polyline_2d_sptr vpoly = new vsol_polyline_2d(vp);

          tab_[ie]->set_current_grouping(es_layer_.c_str());
          es_so_[ie] = tab_[ie]->add_vsol_polyline_2d(vpoly);
          es_so_[ie]->set_style(es_style_);
          
        }
      }

      for (unsigned ie=0; ie < es_.size(); ++ie) {
        tab_[ie]->post_redraw();
      }

    break;

    case '7':
      vcl_cout << "Epipolar constraint TODO...";
//      TODO vcl_cout.flush();
//      get_epipolar_candidates();
//      vcl_cout << "done!\n";
    break;

    case '8':
      vcl_cout << "Writing energies to file...";
      vgui::out << "Writing energies to file...";
      write_energies();
      vcl_cout << "done!\n";
      vgui::out << "done!\n";
    break;

    case '9':
      vcl_cout << "Trimming any ones bellow cost threshold...";
      corr_->threshold_by_cost(500);
      vcl_cout << "done!\n";
    break;

    case '0': // misc. stuff
      vcl_cout << "Sorting corrresps....";
      corr_->sort();
      vcl_cout << "done!\n";
      break;

    default:
      if (!base_stat)
        vcl_cout << "(corresp_point_tool) Unassigned key: " << key << " pressed.\n";
      return base_stat;
      break;
  }
  return true;
}


bool mw_correspond_point_tool::
handle_mouse_event_whatever_view( 
    const vgui_event & e, 
    const bvis1_view_tableau_sptr& view )
{
  return mw_correspond_point_tool_basic::handle_mouse_event_whatever_view(e,view);
}

bool mw_correspond_point_tool::
handle_mouse_event_at_view_0( 
    const vgui_event & e, 
    const bvis1_view_tableau_sptr& view )
{
  return mw_correspond_point_tool_basic::handle_mouse_event_at_view_0(e,view);
}

bool mw_correspond_point_tool::
handle_mouse_event_at_view_1( 
    const vgui_event & e, 
    const bvis1_view_tableau_sptr& view )
{
  bool base_stat = 
    mw_correspond_point_tool_basic::handle_mouse_event_at_view_1(e,view);
  
  // identify the index of the selected edgel in our data
  unsigned idx;
  bool stat  = get_index_of_point( selected_edgel_corresp_, vsols_orig_cache_[1], &idx);

  if (stat) {
     vcl_cout << "View 2: mouse click on point number: (" << idx+1 << ") out of " 
               << vsols_[1].size() << vcl_endl;
     p1_query_idx_ = idx;
  } else {
     vcl_cout << "View 2: mouse click on mysterious pont\n";
     return false;
  }

  if (p1_query_soview_)
    tab_[1]->remove(p1_query_soview_);

  tab_[1]->set_current_grouping(p1_query_layer_.c_str());
  p1_query_soview_ 
    = tab_[1]->add_vsol_line_2d(vsols_orig_cache_[1][idx]);

  p1_query_soview_->set_style(p1_query_style_);
  
  // find clicked point among candidates + print info

  vcl_list<mw_attributed_object>::iterator itr;

  itr = corr_->corresp_[p0_query_idx_].begin();  unsigned  ii=0;
  for (; itr != corr_->corresp_[p0_query_idx_].end(); ++itr, ++ii) {
    if (itr->obj_ == idx)
      break;
  }

  if (itr != corr_->corresp_[p0_query_idx_].end()) {
    vcl_cout << "Clicked on candidate (" <<  ii+1 << ")"  << " out of " 
      << corr_->corresp_[p0_query_idx_].size() << vcl_endl;
    vcl_cout << "    " << *itr << vcl_endl;
    p1_query_itr_ = itr;
    p1_query_is_candidate_ = true;

    if (synthetic_ || synthetic_olympus_) {
      dbdif_3rd_order_point_2d &p1 = crv2d_gt_[0][p0_query_idx_];
      dbdif_3rd_order_point_2d &p2 = crv2d_gt_[1][idx];

      dbdif_3rd_order_point_2d p1_w, p2_w;
      dbdif_3rd_order_point_3d P_rec;

      dbdif_rig rig(cam_[0].Pr_,cam_[1].Pr_);

      rig.cam[0].img_to_world(&p1,&p1_w);
      rig.cam[1].img_to_world(&p2,&p2_w);

      rig.reconstruct_3rd_order(p1_w, p2_w, &P_rec);

      //: todo: print theta
      printf("view 1(pixels) -- k1: %8g\t(r=%8g),\tkdot1:%8g\n", p1.k, 1./p1.k, p1.kdot);
      printf("view 2(pixels) -- k2: %8g\t(r=%8g),\tkdot2:%8g\n", p2.k, 1./p2.k, p2.kdot);
      printf("view 1(mm)     -- k1: %8g\t(r=%8g),\tkdot1:%8g\n", p1_w.k, 1./p1_w.k, p1_w.kdot);
      printf("view 2(mm)     -- k2: %8g\t(r=%8g),\tkdot2:%8g\n", p2_w.k, 1./p2_w.k, p2_w.kdot);
      printf("reconstr       --  K: %8g\t(R=%8g),\tKdot :%8g,\tTau:%8g\tSpeed:%8g\tGamma3dot:%8g\n", P_rec.K, 1./P_rec.K, P_rec.Kdot, P_rec.Tau, 1.0/rig.cam[0].speed(P_rec), P_rec.Gamma_3dot_abs());
      double depth_rec = dot_product(P_rec.Gama - cam_[0].c, cam_[0].F);
      vcl_cout << "reconstr       --  Point(world coords): " << P_rec.Gama << "\tnorm: "  << P_rec.Gama.two_norm() << "\tdepth: " << depth_rec << vcl_endl;
      bool is_the_match = (idx == p0_query_idx_);
      if (is_the_match) {
        unsigned  i_crv, i_pt;
        bool found = find_crv3d_idx(idx, i_crv, i_pt);
        if (found) {
          assert(i_crv < crv3d_gt_.size() && i_pt < crv3d_gt_[i_crv].size());
          dbdif_3rd_order_point_3d &P = crv3d_gt_[i_crv][i_pt];
          printf("gnd-truth      --  K: %8g\t(R=%8g),\tKdot:%8g,\tTau:%8g\tSpeed:%8g\tGamma3dot:%8g\n", P.K, 1./P.K, P.Kdot, P.Tau, 1.0/rig.cam[0].speed(P), P.Gamma_3dot_abs());
          double depth_gnd = dot_product(P.Gama - cam_[0].c, cam_[0].F);
          vcl_cout << "gnd-truth      --  Point(world coords): " << P.Gama << "\tnorm: "  << P.Gama.two_norm()<< "\tdepth: " << depth_gnd << vcl_endl;

          // angle of reprojection with the edgel in 3rd view
          bool valid;
          dbdif_3rd_order_point_2d p3_reproj = cam_[2].project_to_image(P_rec,&valid);
          vcl_cout << "gnd-truth      --  reproj tangent: " << p3_reproj.t <<"\tview3 tangent: " << crv2d_gt_[2][p0_query_idx_].t 
            << "\tangle(rad): " << vcl_acos(mw_util::clump_to_acos(p3_reproj.t[0]*(crv2d_gt_[2][p0_query_idx_].t[0]) + p3_reproj.t[1]*(crv2d_gt_[2][p0_query_idx_].t[1]))) << vcl_endl;
          double dotprod = p3_reproj.t[0]*(crv2d_gt_[2][p0_query_idx_].t[0]) + p3_reproj.t[1]*(crv2d_gt_[2][p0_query_idx_].t[1]);
          vcl_cout << "gnd-truth      --  dotprod tangent clumped: " << mw_util::clump_to_acos(dotprod) << "\t> one?" << ((dotprod > 1)?"yes":"no") << vcl_endl;
          
        } else {
          vcl_cout << "WARNING: Not found among ground-truth!\n";
        }
      }
    }
  } else {
    vcl_cout << "Clicked point NOT found among candidates\n";
    p1_query_is_candidate_ = false;
  }

  // reconstruct+reproject + add soview

  return base_stat;
}

/*
//: output: corr_
// Fill up correspondence structure with epipolar candidates.
void mw_correspond_point_tool::
get_epipolar_candidates()
{
  mw_point_matcher m(cam_);
  m.epipolar_constraint(vsols_, corr_, 0, 1, *fm_);
}
*/

/*
//: output: corr_
void mw_correspond_point_tool::
run_stereo_matcher_given_epipolar_candidates()
{
  mw_point_matcher m(cam_);
//  m.epipolar_constraint(vsols_, corr_, 0, 1);

  mw_subpixel_point_set p_img3(vsols_[2]);
  p_img3.build_bucketing_grid(nrows_,ncols_);

  m.trinocular_costs(vsols_, p_img3, corr_, 0, 1, 2);

  corr_->sort();
}

void mw_correspond_point_tool::
run_stereo_matcher_and_epipolar_candidates()
{
  mw_point_matcher m(cam_);
  corr_->corresp_.clear();
  m.epipolar_constraint(vsols_, corr_, 0, 1, *fm_);

  mw_subpixel_point_set p_img3(vsols_[2]);
  p_img3.build_bucketing_grid(nrows_,ncols_);

  m.trinocular_costs(vsols_, p_img3, corr_, 0, 1, 2);

  corr_->sort();
}
*/

//: output: selected_edgel_corresp_
// \return true if selected a valid edgel
bool mw_correspond_point_tool::
handle_corresp_query_whatever_view
    ( const vgui_event & e, 
    const bvis1_view_tableau_sptr& /*view*/ )
{
//  vcl_cout << "mouse over - Frame index: " << view->frame() << vcl_endl;

  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

  // I) Find edgel closest to ix,iy

//  unsigned row_cell = (unsigned)iy;
//  unsigned col_cell = (unsigned)ix;

  vgui_soview2D* selected_edgel_soview_base =  
    (vgui_soview2D*)edgel_tableau_current_->get_highlighted_soview();

  bgui_vsol_soview2D_line_seg *selected_edgel_soview_line
    = dynamic_cast<bgui_vsol_soview2D_line_seg *>(selected_edgel_soview_base); 

  if (!selected_edgel_soview_line || !selected_edgel_soview_line->sptr()) {
    // vcl_cout << "mouse over - Nothing\n";
    return false;
  }

  selected_edgel_corresp_ = selected_edgel_soview_line->sptr();
  // vcl_cout << "mouse over - Middle of selected edgel: " << 
    // *(selected_edgel_corresp_->middle()) << vcl_endl;
  return true;
}

//: Returns true if mouse over valid edge in 1st view
bool mw_correspond_point_tool::
handle_corresp_query_at_view_0
    ( const vgui_event & /*e*/, 
    const bvis1_view_tableau_sptr& /*view*/ )
{
  // identify the index of the selected edgel in our data
  unsigned idx;
  bool stat  = get_index_of_point( selected_edgel_corresp_, vsols_orig_cache_[0], &idx);
  if (stat) {
     vcl_cout << "View 1: mouse over point number: (" << idx+1 << ") out of " 
               << vsols_[0].size() << vcl_endl;
     p0_query_idx_ = idx;
  } else {
     vcl_cout << "View 1: mouse over mysterious pont\n";
     return false;
  }

  // show epipolar angle
  if (synthetic_ || synthetic_olympus_) {
    dbdif_3rd_order_point_2d &p1 = crv2d_gt_[0][p0_query_idx_];

    dbdif_rig rig(cam_[0].Pr_,cam_[1].Pr_);
    double epipolar_angle = dbdif_rig::angle_with_epipolar_line(p1.t,p1.gama,rig.f12);
    epipolar_angle *= 180.0/vnl_math::pi;

    vcl_cout << "epi angle: " << epipolar_angle << "deg" << vcl_endl;
  }

  // mark selected edgel
  if (p0_query_soview_)
    tab_[0]->remove(p0_query_soview_);

  tab_[0]->set_current_grouping( p0_query_layer_.c_str() );
  p0_query_soview_ = tab_[0]->add_vsol_line_2d(vsols_orig_cache_[0][idx]);
  p0_query_soview_->set_style(p0_query_style_);
      
  // add the correspondents in 2nd view
  
  for (vcl_list<bgui_vsol_soview2D_line_seg *>::iterator 
      itr = correspondents_soview_.begin() ; 
      itr != correspondents_soview_.end(); ++itr) {
    tab_[1]->remove(*itr);
  }
  correspondents_soview_.clear();
  correspondents_idx_.clear();

  vcl_list<mw_attributed_object>::const_iterator itr;

  itr = corr_->corresp_[idx].begin(); unsigned ii=0;
  for (; itr != corr_->corresp_[idx].end(); ++itr, ++ii) {
    if (ii < best_match_style_.size()) { //: display top 5 matches
      tab_[1]->set_current_grouping( best_match_layer_.c_str() );
    } else {
      tab_[1]->set_current_grouping( corresp_edges_layer_.c_str() );
    }


    correspondents_soview_.push_back(
        tab_[1]->add_vsol_line_2d(vsols_orig_cache_[1][itr->obj_]));
    correspondents_idx_[itr->obj_] = --correspondents_soview_.end();

    if (ii < best_match_style_.size()) { //: display top 5 matches
      correspondents_soview_.back()->set_style(best_match_style_[ii]);
    } else {
      correspondents_soview_.back()->set_style(corresp_edges_style_);
    }
  }
   
  for (unsigned i=0 ; i < nviews_; ++i)
    tab_[i]->post_redraw();

  return true;
}

void mw_correspond_point_tool::
toggle_lock_correspondence_query()
{
  vcl_string state;
  if (lock_corresp_query_) {
    lock_corresp_query_= false;
    state = "off";
  } else {
    lock_corresp_query_= true;
    state = "on";
  }
  vcl_cout << "Inspect correspondence " << state << vcl_endl;;
  vgui::out << "Inspect correspondence " << state << vcl_endl;;
}

void mw_correspond_point_tool::
run_stereo_matcher()
{
  // TODO
}

//: given linearized index idx into synthetic curves, return index of curve and point within curve
bool mw_correspond_point_tool::
find_crv3d_idx(unsigned idx, unsigned &i_crv, unsigned &i_pt) const
{

  unsigned  idx_curr=0;
  for (i_crv=0; i_crv < crv3d_gt_.size(); ++i_crv) { // ncurves
    for (i_pt=0; i_pt < crv3d_gt_[i_crv].size(); ++i_pt, ++idx_curr) {
      if (idx_curr == idx) {
        return true;
      }
    }
  }

  return false;
}

dbgl_eulerspiral * mw_correspond_point_tool::
get_new_eulerspiral(const dbdif_3rd_order_point_2d &p) const
{
  vgl_point_2d<double> start_pt(p.gama[0], p.gama[1]);
  double start_angle = mw_util::angle0To2Pi (vcl_atan2 (p.t[1],p.t[0]));
  return new dbgl_eulerspiral(start_pt, start_angle, p.k, p.kdot, 3);
}

void mw_correspond_point_tool::
srm_draw_eulerspiral() 
{
  if (es_.size() && es_.size() >= 3 && es_[2]) {
      if (es_gt_) {
        delete es_gt_;
        tab_[2]->remove(es_so_gt_);
        es_gt_=0;
      }
      delete es_[2];
      es_.pop_back();
      tab_[2]->remove(es_so_[2]);

      // regenerate cam
      vpgl_calibration_matrix<double> K(srm_K_);
      vpgl_perspective_camera<double> *P = 
            dbdif_turntable::camera_olympus(srm_angle_, K);
      srm_cam_.set_p(*P);

      bool valid;
      dbdif_3rd_order_point_2d p_rec_reproj = srm_cam_.project_to_image(srm_Prec_,&valid);

      if (valid) {
        vcl_cout << "Angle: " << srm_angle_ << "deg\n";
        es_.push_back(get_new_eulerspiral (p_rec_reproj)); 

        unsigned ie=2;
        vcl_vector<vsol_point_2d_sptr> vp;

        for (double s=-12.0; s<12.0; s+=0.1)
          vp.push_back(
              new vsol_point_2d(es_[ie]->point_at_length(s).x(), es_[ie]->point_at_length(s).y()));

        vsol_polyline_2d_sptr vpoly = new vsol_polyline_2d(vp);

        tab_[ie]->set_current_grouping(es_layer_.c_str());
        es_so_[ie] = tab_[ie]->add_vsol_polyline_2d(vpoly);
        es_so_[ie]->set_style(es_style_);
        tab_[ie]->post_redraw();
        printf("SRM: proj (pixels) -- k1: %8g\t(r=%8g),\tkdot1:%8g\n", p_rec_reproj.k, 1./p_rec_reproj.k, p_rec_reproj.kdot);
        printf("SRM: reconstr      --  K: %8g\t(R=%8g),\tKdot :%8g,\tTau:%8g\tEnergy(Gama3dot):%8g\n", srm_Prec_.K, 1./srm_Prec_.K, srm_Prec_.Kdot, srm_Prec_.Tau, srm_Prec_.Gamma_3dot_abs());
        double depth_srm = dot_product(srm_Prec_.Gama - srm_cam_.c, srm_cam_.F);
        vcl_cout << "SRM: reconstr      -- depth: " << depth_srm << vcl_endl;

        unsigned  i_crv, i_pt;
        bool found = find_crv3d_idx(p0_query_idx_, i_crv, i_pt);
        if (found) {
          assert(i_crv < crv3d_gt_.size() && i_pt < crv3d_gt_[i_crv].size());
          dbdif_3rd_order_point_3d &P = crv3d_gt_[i_crv][i_pt];
          printf("SRM: P gnd-truth   --  K: %8g\t(R=%8g),\tKdot:%8g,\tTau:%8g\tEnergy(Gama3dot):%8g\n", P.K, 1./P.K, P.Kdot, P.Tau, P.Gamma_3dot_abs());
          double depth_gnd = dot_product(P.Gama - cam_[0].c, cam_[0].F);
          vcl_cout << "SRM: P gnd-truth   --    Point(world coords): " << P.Gama << "\tnorm: "  << P.Gama.two_norm()<< "\tdepth: " << depth_gnd << vcl_endl;
        } else {
          vcl_cout << "WARNING: Not found ground-truth for P0!\n";
        }
        vcl_cout << "\n\n";
      }

      unsigned ie = 2;

      for (unsigned i=0; i < srm_allcrvs_so_.size(); ++i) {
        tab_[ie]->remove(srm_allcrvs_so_[i]);
      }
      srm_allcrvs_so_.clear();
      
      if (srm_display_full_) {
        // project whole dataset using srm cam

        tab_[ie]->set_current_grouping(srm_allcrvs_layer_.c_str());
        srm_allcrvs_so_.resize(crv3d_gt_.size());
        for (unsigned i_crv=0; i_crv < crv3d_gt_.size(); ++i_crv) {
          vcl_vector<vsol_point_2d_sptr> vp;
          vp.reserve(crv3d_gt_[i_crv].size());
          for (unsigned i_pt=0; i_pt < crv3d_gt_[i_crv].size(); i_pt+=4) {
            bool valid;
            dbdif_3rd_order_point_2d srm_p = srm_cam_.project_to_image(crv3d_gt_[i_crv][i_pt], &valid);
            if (valid) {
              vp.push_back(new vsol_point_2d(srm_p.gama[0],srm_p.gama[1]));
            }
          }
          vsol_polyline_2d_sptr vpoly = new vsol_polyline_2d(vp);

          srm_allcrvs_so_[i_crv] = tab_[ie]->add_vsol_polyline_2d(vpoly);
          srm_allcrvs_so_[i_crv]->set_style(srm_allcrvs_style_);
        }
        tab_[ie]->post_redraw();
      }

  }
}

//: Output energies of epipolar candidates in the order they are found in the
// correspondence datastructure.
void
mw_correspond_point_tool::
write_energies()
{
  vcl_vector<double> costs, speeds;

  unsigned idx = p0_query_idx_;
  unsigned match_candidate_index;

  vcl_list<mw_attributed_object>::const_iterator itr;
  itr = corr_->corresp_[idx].begin(); unsigned ii=0;
  for (; itr != corr_->corresp_[idx].end(); ++itr, ++ii) {
    costs.push_back(itr->cost_);
    if (itr->obj_ == idx && vnl_math_isfinite(itr->cost_))
      match_candidate_index = ii;

    // Reconstruct
    dbdif_3rd_order_point_2d &p1 = crv2d_gt_[0][p0_query_idx_];
    dbdif_3rd_order_point_2d &p2 = crv2d_gt_[1][itr->obj_];

    dbdif_3rd_order_point_2d p1_w, p2_w;
    dbdif_3rd_order_point_3d P_rec;

    dbdif_rig rig(cam_[0].Pr_,cam_[1].Pr_);

    rig.cam[0].img_to_world(&p1,&p1_w);
    rig.cam[1].img_to_world(&p2,&p2_w);

    rig.reconstruct_3rd_order(p1_w, p2_w, &P_rec);

    speeds.push_back(1.0/rig.cam[0].speed(P_rec));
  }

  (void) mywrite("dat/energies-epipolar-inspector.dat", costs);
  (void) mywrite("dat/speeds-epipolar-inspector.dat", speeds);

  vcl_vector<double> v_index;
  v_index.push_back(match_candidate_index);
  (void) mywrite("dat/p0_idx-epipolar-inspector.dat", v_index);
}
