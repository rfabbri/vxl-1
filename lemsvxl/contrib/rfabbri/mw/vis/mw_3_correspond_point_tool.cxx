#include "mw_3_correspond_point_tool.h"

#include <iomanip>
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

#include <vpgld/pro/vpgld_camera_storage.h>

#include <vgui/vgui.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <bgui/bgui_vsol_soview2D.h>

#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>

#include <bmcsd/bmcsd_util.h>
#include <bdifd/bdifd_rig.h>
#include <mw/mw_subpixel_point_set.h>
#include <becld/becld_epiline_interceptor.h>
//#include <mw/algo/mw_point_matcher.h>

#include <bdifd/bdifd_rig.h>
#include <bdifd/algo/bdifd_data.h>
#include <bdifd/bdifd_rig.h>
#include <mw/algo/mw_match_position_band.h>
#include <mw/algo/mw_match_tangent_band.h>

#include <bmcsd/pro/bmcsd_discrete_corresp_storage_3.h>
#include <bgld/algo/bgld_eulerspiral.h>


// Just for running symmetric epipolar triplet builder:
#include <bmcsd/bmcsd_discrete_corresp_n.h>
#include <becld/becld_epiband_builder.h>
#include <mw/algo/mw_point_matcher.h>

mw_3_correspond_point_tool::
mw_3_correspond_point_tool()
  :
    mw_correspond_point_tool_basic()
{
  gesture_query_corresp_= vgui_event_type(vgui_MOUSE_MOTION);
  corresp_edges_style_  = vgui_style::new_style(0.5, 1, 0.5, 
                                    5.0f/*ptsize*/, 3.0f/*linesize*/); // Green

  wrongly_matched_edgels_style_ = vgui_style::new_style(1, 0, 1, 
                                    5.0f/*ptsize*/, 3.0f/*linesize*/); // Magenta

  best_match_style_.push_back(vgui_style::new_style(1, 1, 0, 1.0f, 3.0f));   // Yellow
  best_match_style_.push_back(vgui_style::new_style(1, 0.5, 0, 1.0f, 3.0f)); // Orange
  best_match_style_.push_back(vgui_style::new_style(1, 0, 1, 1.0f, 3.0f));   // Magenta
  best_match_style_.push_back(vgui_style::new_style(0, 1, 1, 1.0f, 3.0f));   // Green blue

  //: basic stuff is in layer 50
  //: higher numbers mean higher layers
  best_match_layer_ = "layer91";
  corresp_edges_layer_ = "layer60";


  // Extras part of tool -----------------------------
  es_style_ = vgui_style::new_style(1, 1, 1, 
                                    5.0f/*ptsize*/, 2.0f/*linesize*/); // White
  es_style_gt_ = vgui_style::new_style(0.7, 0.7, 0.7, 
                                    5.0f/*ptsize*/, 2.0f/*linesize*/); // Gray
  srm_allcrvs_style_ = vgui_style::new_style(7, 0, 0, 
                                    5.0f/*ptsize*/, 1.5f/*linesize*/); // White

  es_layer_ = "layer70";
  es_layer_gt_ = "layer69";
  srm_allcrvs_layer_ = "layer65";
  es_gt_ = 0;
}

std::string mw_3_correspond_point_tool::
name() const
{
  return "Multiview _3_ point correspond";
}

void mw_3_correspond_point_tool::
activate()
{
  mw_correspond_point_tool_basic::activate();

  p_query_style_.resize(nviews_);
  p_query_soview_.resize(nviews_);
  p_corresp_soview_.resize(nviews_);
  p_query_id_.resize(nviews_, (unsigned)-1);
  correspondents_idx_.resize(nviews_);
  correspondents_soview_.resize(nviews_);
  for (unsigned v=0; v < nviews_; ++v)
    p_query_style_[v] = vgui_style::new_style(0, 0, 1, 
                                      5.0f/*ptsize*/, 8.0f/*linesize*/); // Blue
  p_query_layer_.resize(nviews_);
  for (unsigned v=0; v < nviews_; ++v)
    p_query_layer_[v] = "layer85";

  lock_corresp_query_ = false;
  for (unsigned v=0; v < nviews_; ++v) {
    p_query_soview_[v] = 0;
    p_corresp_soview_[v].resize(vsols_[v].size(),0);
  }
  query_is_corresp_ = false;

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

  synthetic_ = false;
  synthetic_olympus_ = false;
  srm_display_full_ = false;
  srm_display_es_   = true;
  has_sp_ = false;
  //----------

  get_corresp(); 
  get_images();

  // Generate synthetic ground-truth for inspection, if requested
  if (synthetic_ || synthetic_olympus_) {
    // generate spc curve & store it

    std::cout << "Generating curves..."; std::cout.flush();

    if (synthetic_) {
      bdifd_data::space_curves_ctspheres( crv3d_gt_ );
      // hardcode nrows, ncols:
      nrows_ = 314;
      ncols_ = 600;
    } else  {
      bdifd_data::space_curves_olympus_turntable( crv3d_gt_ );
      nrows_ = 400;
      ncols_ = 500;
      angle_cam_.resize(nviews_);
      angle_cam_[0] = 30;
      angle_cam_[1] = 60;
      angle_cam_[2] = 90;
      unsigned  crop_origin_x_ = 450;
      unsigned  crop_origin_y_ = 1750;
      bdifd_turntable::internal_calib_olympus(srm_K_, ncols_, crop_origin_x_, crop_origin_y_);
      vpgl_calibration_matrix<double> K(srm_K_);
      cam_gt_.resize(nviews_);
      for (unsigned i=0; i < nviews_; ++i) {
        vpgl_perspective_camera<double> *P = 
              bdifd_turntable::camera_olympus(angle_cam_[i], K);
        cam_gt_[i].set_p(*P);
      }
    }


    bdifd_data::project_into_cams_without_epitangency(crv3d_gt_, cam_gt_, crv2d_gt_, vnl_math::pi/6.0);
    std::cout << "done.\n";

    gt_3_.set_size(crv2d_gt_[0].size(), crv2d_gt_[1].size(), crv2d_gt_[2].size());
    assert (crv2d_gt_[0].size() == crv2d_gt_[1].size() && crv2d_gt_[1].size() == crv2d_gt_[2].size());

    for (unsigned i=0; i < crv2d_gt_[0].size(); ++i) {
      gt_3_.l_.put(i,i,i,bmcsd_match_attribute());
    }

    std::cout << "Hashing ground truth...";
    std::cout.flush();
    gt_3_.hash();
    std::cout << "done\n";
  }

  // coloring of points in image 0 having any correspondents
  color_pts0_with_correspondents();
}

void mw_3_correspond_point_tool::
get_corresp()
{ // get correspondence storage at view 1


  //Prompt the user to select input/output variable
  vgui_dialog io_dialog("Select Inputs" );


  std::vector< std::string > input_type_list;
  input_type_list.push_back("mw_3_pt_corresp");

  io_dialog.message("Select Input(s) From Available ones:");
  std::vector<int> input_choices(input_type_list.size());
  std::vector< std::vector <std::string> > available_storage_classes(input_type_list.size());
  std::vector< std::string > input_names(input_type_list.size());
       
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
    std::cout << "Canceled\n";
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

  std::cout << "Selected input: " << input_names[0] << " ";
  
  bpro1_storage_sptr 
    p = MANAGER->repository()->get_data_by_name_at(input_names[0],frame_v_[0]);


  p_sto_3_.vertical_cast(p);
  if(!p_sto_3_) {
    std::cerr << "Error: tool requires a valid correspondence storage" << std::endl;
    return;
  }
  
  corr_3_ = p_sto_3_->corresp();
  if(!corr_3_) {
    std::cerr << "Empty storage - allocating data" << std::endl;
    corr_3_ = new bmcsd_discrete_corresp_3(vsols_[0].size(),vsols_[1].size(),vsols_[2].size());
    p_sto_3_->set_corresp(corr_3_); // storage deletes it
  } else {
    if (corr_3_->n0() != vsols_[0].size() || corr_3_->n1() != vsols_[1].size() ||corr_3_->n2() != vsols_[2].size()) {
      std::cerr << "Error: input correspondence is not valid for current edgels\n" 
        << std::endl;
      std::cerr << "corresp size n0: " << corr_3_->n0() << std::endl;
      std::cerr << "vsols size 0: " << vsols_[0].size() << std::endl;
      return;
    }
  }

  std::cout << "Hashing corresp 3...";
  std::cout.flush();
  corr_3_->hash();
  std::cout << "done\n";

  std::cout << "Corresp NAME: " << p_sto_3_->name() << std::endl;
  // std::cout << "Corresp: " << " : \n" << *corr_ << std::endl;
}

void mw_3_correspond_point_tool::
get_images()
{
  // get image storage at all 3 views
  for (unsigned i=0; i < nviews_; ++i) {
    bpro1_storage_sptr 
      p = MANAGER->repository()->get_data_at("image",frame_v_[i]);

    vidpro1_image_storage_sptr frame_image;
    frame_image.vertical_cast(p);

    if (!frame_image) {
      if (!synthetic_ && !synthetic_olympus_) {
        std::cout << "ERROR: no images in view " << i << std::endl;
        return;
      }
    } else {
      images_.push_back(frame_image->get_image());
      ncols_ = images_[i]->ni();
      nrows_ = images_[i]->nj();

      std::cout << "Image view # " << i << "  ";
      std::cout << "Nrows: " << nrows_;
      std::cout << "  Ncols: " << ncols_ << std::endl;
    }
  }
}


void   
mw_3_correspond_point_tool::
deactivate ()
{
  std::cout << "mw_3_correspond_point_tool OFF\n";
  mw_correspond_point_tool_basic::deactivate ();

  if (has_sp_)
    for (unsigned i=0; i<nviews_; ++i)
      delete sp_[i];
}

bool mw_3_correspond_point_tool::
handle( const vgui_event & e, 
        const bvis1_view_tableau_sptr& view )
{
  if (e.type == vgui_KEY_PRESS) {
    std::cout << "Frame index: " << view->frame() << std::endl;
    return handle_key(e.key);
  }

  //: mouse motion
  if (e.type == gesture_query_corresp_) {
    if ( handle_corresp_query_whatever_view(e,view) ) {
      if ( view->frame() == frame_v_[0] ) {
        if (!lock_corresp_query_)
          handle_corresp_query_at_view_0(e,view);
      }
      if ( view->frame() == frame_v_[1] ) {
        if (lock_corresp_query_)
          handle_corresp_inspection_at_view_1(e,view);
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
    } else { // 3rd view
      std::cout << "3rd view\n";
      handle_mouse_event_at_view_2(e,view);
    }
  }

  //  We are not interested in other events,
  //  so pass event to base class:
  return false;
}

bool mw_3_correspond_point_tool::
handle_key(vgui_key key)
{
  bool base_stat = mw_correspond_point_tool_basic::handle_key(key);

  switch (key) {
    case 277: // Del
      if (query_is_corresp_) {
        std::cout << "Removing " 
          << p_query_id_[0] << " "
          << p_query_id_[1] << " "
          << p_query_id_[2]
          << std::endl;

        assert(corr_3_->l_.fullp(p_query_id_[0],p_query_id_[1],p_query_id_[2]));
        corr_3_->l_.erase(vbl_make_triple(p_query_id_[0],p_query_id_[1],p_query_id_[2]));
        query_is_corresp_ = false;

        // - delete this guy from correspondents soview list, if there.

        std::map<unsigned, std::list<bgui_vsol_soview2D_line_seg *>::iterator >::iterator 
          itr = correspondents_idx_[1].find(p_query_id_[1]);
        if (itr != correspondents_idx_[1].end() && *((*itr).second) != 0) {
          tab_[1]->remove(*(correspondents_idx_[1][p_query_id_[1]]));
          correspondents_soview_[1].erase(correspondents_idx_[1][p_query_id_[1]]);
          correspondents_idx_[1].erase(p_query_id_[1]);
        }

        itr = correspondents_idx_[2].find(p_query_id_[2]);
        if (itr != correspondents_idx_[2].end() && *((*itr).second) != 0) {
          tab_[2]->remove(*(correspondents_idx_[2][p_query_id_[2]]));
          correspondents_soview_[2].erase(correspondents_idx_[2][p_query_id_[2]]);
          correspondents_idx_[2].erase(p_query_id_[2]);
        }

        corr_3_->hash();
        if (corr_3_->triplets(0, p_query_id_[0]).empty() && 
            p_corresp_soview_[0][p_query_id_[0]] != 0) {
          tab_[0]->remove(p_corresp_soview_[0][p_query_id_[0]]);
          p_corresp_soview_[0][p_query_id_[0]] = 0;
        }

        tab_[2]->post_redraw();
        tab_[1]->post_redraw();
        tab_[0]->post_redraw();
      } else {
        std::cout << "Selected correspondence inexist\n";
      }

      return true;
      break;

    case 275: // Home (think "Ins", but macs don't have ins, so I used home)
      if (!query_is_corresp_) {
        bool tuplet_not_selected = false;
        for (unsigned v=0; v < nviews_; ++v) {
          if (p_query_id_[v] == (unsigned)-1) {
            std::cout << "Error: edgels must be selected in all views prior to inserting.\n";
            tuplet_not_selected = true;
            break;
          }
        }
        if (tuplet_not_selected)
          break;

        bool  stat = 
          corr_3_->l_.put(p_query_id_[0],p_query_id_[1],p_query_id_[2], bmcsd_match_attribute());

        if (stat) {
          std::cout << "Inserting " 
            << p_query_id_[0] << " "
            << p_query_id_[1] << " "
            << p_query_id_[2]
            << std::endl;

          query_is_corresp_ = true;

          tab_[1]->set_current_grouping( corresp_edges_layer_.c_str() );
          // - add this guy to correspondents soview list; breaking sort order
          correspondents_soview_[1].push_back(
              tab_[1]->add_vsol_line_2d(vsols_orig_cache_[1][p_query_id_[1]]));
          correspondents_soview_[1].back()->set_style(corresp_edges_style_);
          correspondents_idx_[1][p_query_id_[1]] = --correspondents_soview_[1].end();

          tab_[2]->set_current_grouping(corresp_edges_layer_.c_str() );
          correspondents_soview_[2].push_back(
              tab_[2]->add_vsol_line_2d(vsols_orig_cache_[2][p_query_id_[2]]));
          correspondents_soview_[2].back()->set_style(corresp_edges_style_);
          correspondents_idx_[2][p_query_id_[2]] = --correspondents_soview_[2].end();

          tab_[0]->set_current_grouping(corresp_edges_layer_.c_str() );
          if (p_corresp_soview_[0][p_query_id_[0]] == 0) {
            p_corresp_soview_[0][p_query_id_[0]] = tab_[0]->add_vsol_line_2d(
                vsols_orig_cache_[0][p_query_id_[0]]);
            p_corresp_soview_[0][p_query_id_[0]]->set_style(corresp_edges_style_);
          }

          tab_[0]->post_redraw();
          tab_[1]->post_redraw();
          tab_[2]->post_redraw();
          corr_3_->hash();
        } else {
          std::cout << "Error/Insert: tuplet is already in corresp datastructure\n";
          std::cout << "p_query_id_[0]:" << p_query_id_[0] 
            << "  p_query_id_[1]:" << p_query_id_[1] 
            << "  p_query_id_[2]:" << p_query_id_[2] 
            << std::endl;
        }
      } else {
        std::cout << "Selected correspondence already inserted\n";
      }
      break;

    case 'p': // print misc info / debug
      std::cout << *corr_3_ << std::endl;

      return true;
      break;
      /*
    case 's': // fill corresp. structure with stereo results
      std::cout << "Epipolar constraint";
      run_stereo_matcher();

      return true;
      break;
      */
    case 'l': // misc. stuff
      toggle_lock_correspondence_query();
      return true;
      break;

    case 'm': // compute trinocular epipolar match for current point + populate corr structure then show
      trinocular_epipolar_candidates_1pt();
      break;

    case 't': // compute trinocular matches for current point using tangential constraint
      run_stereo_matcher_1pt(mw_point_matcher::C_THRESHOLD_TANGENT);
      break;

    case 'b': // compute trinocular matches for current point using tangential banding constraint
      run_stereo_matcher_1pt(mw_point_matcher::C_TANGENT_BAND);
      break;

    case 'y':
      trinocular_epipolar_candidates_1pt_with_tangent_constraint();
      break;

    case 'r': // compute trinocular tangential match for current point
      reproject_from_triplet();
      break;

    case 's': // compute trinocular tangential match for all correspondences
      reproject_from_triplet_allcorr();
      break;

      /*
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

    case 5: { // Ctrl-E
      srm_display_full_ = !srm_display_full_;
      vgui::out << "Display srm reproj " << ( (srm_display_full_)? "ON" : "OFF") << std::endl;
    }
    break;
    */

    case '^': /* Shift-6 */
      // - Display eulerspiral off
      srm_display_es_ = !srm_display_es_;
      vgui::out << "Display Eulerspirals " << ( (srm_display_es_)? "ON" : "OFF") << std::endl;
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

      std::cout << "6 pressed\n";
      if (srm_display_es_) { // TODO: also make sure some point is selected 
        std::cout << "Drawing Eulerspirals\n";
        vgui::out << "Drawing Eulerspirals\n";
        srm_angle_= angle_cam_[2];

//        vpgl_calibration_matrix<double> K(srm_K_);
//        vpgl_perspective_camera<double> *P = 
//              bdifd_turntable::camera_olympus(srm_angle_, K);
        srm_cam_.set_p(cam_[2].Pr_);

        bdifd_3rd_order_point_2d p1,p2;
        if (crv2d_gt_.size()) {
          // get hold of p1, theta1, k1, kdot1
          p1 = crv2d_gt_[0][p_query_id_[0]];

          // get hold of p2, theta2, k2, kdot2
          p2 = crv2d_gt_[1][p_query_id_[1]];
        } else {
          // - get hold of p1, theta1
          // - set k1, kdot1 to zero

          p1.gama[0]  = vsols_[0][p_query_id_[0]]->x();
          p1.gama[1]  = vsols_[0][p_query_id_[1]]->y();
          p1.gama[2]  = 0;
          
          vgl_vector_2d<double> dir = vsols_orig_cache_[0][p_query_id_[0]]->direction();

          p1.t[0] = dir.x();
          p1.t[1] = dir.y();
          p1.t[2] = 0;
          p1.n[0] = -dir.y();
          p1.n[1] = dir.x();
          p1.n[2] = 0;
          p1.k = 0; 
          p1.kdot = 0;
          p1.valid = true;

          p2.gama[0]  = vsols_[1][p_query_id_[1]]->x();
          p2.gama[1]  = vsols_[1][p_query_id_[1]]->y();
          p2.gama[2]  = 0;
          
          dir = vsols_orig_cache_[1][p_query_id_[1]]->direction();

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

        bdifd_3rd_order_point_2d p1_w, p2_w;

        // get hold of Prec
        bdifd_rig rig(cam_[0].Pr_,cam_[1].Pr_);

        rig.cam[0].img_to_world(&p1,&p1_w);
        rig.cam[1].img_to_world(&p2,&p2_w);

        rig.reconstruct_3rd_order(p1_w, p2_w, &srm_Prec_);

//        std::cout << "HERE: Prec.K: " << srm_Prec_.K
//                 << " Prec.T[0]: " << srm_Prec_.T[0] 
//                 << " Prec.T[1]: " << srm_Prec_.T[1]
//                 << " Prec.T[2]: " << srm_Prec_.T[2]
//                 << " Prec.Gama[0]: " << srm_Prec_.Gama[0] 
//                 << " Prec.Gama[1]: " << srm_Prec_.Gama[1]
//                 << " Prec.Gama[2]: " << srm_Prec_.Gama[2] << std::endl;
        
        // theta1 = theta(view3) 

        bool valid;
        bdifd_3rd_order_point_2d p_rec_reproj = srm_cam_.project_to_image(srm_Prec_,&valid);

//        std::cout << "HERE: p_rec_reproj.K: " << p_rec_reproj.k
//                 << " p_rec_reproj.T[0]: " << p_rec_reproj.t[0] 
//                 << " p_rec_reproj.T[1]: " << p_rec_reproj.t[1]
//                 << " p_rec_reproj.T[2]: " << p_rec_reproj.t[2]
//                 << " p_rec_reproj.Gama[0]: " << p_rec_reproj.gama[0] 
//                 << " p_rec_reproj.Gama[1]: " << p_rec_reproj.gama[1]
//                 << " p_rec_reproj.Gama[2]: " << p_rec_reproj.gama[2] << std::endl;

        bdifd_3rd_order_point_3d gt_Prec;
        bdifd_3rd_order_point_2d p_rec_reproj_gt;
        if (synthetic_ || synthetic_olympus_) {
          // same for g-t cameras
          //
          // get hold of Prec
          bdifd_rig rig_gt(cam_gt_[0].Pr_,cam_gt_[1].Pr_);

          rig_gt.cam[0].img_to_world(&p1,&p1_w);
          rig_gt.cam[1].img_to_world(&p2,&p2_w);

          rig_gt.reconstruct_3rd_order(p1_w, p2_w, &gt_Prec);
          
          // theta1 = theta(view3) 

          bool valid_gt;
          p_rec_reproj_gt = cam_gt_[2].project_to_image(gt_Prec,&valid_gt);

          if (valid_gt)
             es_gt_ = get_new_eulerspiral (p_rec_reproj_gt);

          { // construct eulerspirals for true cams
            std::vector<vsol_point_2d_sptr> vp;
            vp.reserve((unsigned)std::ceil(24.0/0.1));
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
          std::vector<vsol_point_2d_sptr> vp;

          vp.reserve((unsigned)std::ceil(24.0/0.1));
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
    /*
    case '7':
      std::cout << "Epipolar constraint...";
      std::cout.flush();
      get_epipolar_candidates();
      std::cout << "done!\n";
    break;

    case '8':
      std::cout << "Writing energies to file...";
      vgui::out << "Writing energies to file...";
      write_energies();
      std::cout << "done!\n";
      vgui::out << "done!\n";
    break;

    case '9':
      std::cout << "Trimming any ones bellow cost threshold...";
      corr_->threshold_by_cost(500);
      std::cout << "done!\n";
    break;

    case '0': // misc. stuff
      std::cout << "Sorting corrresps....";
      corr_->sort();
      std::cout << "done!\n";
      break;
      */

    default:
      if (!base_stat)
        std::cout << "Unassigned key: " << key << " pressed.\n";
      return base_stat;
      break;
  }
  return true;
}


bool mw_3_correspond_point_tool::
handle_mouse_event_whatever_view( 
    const vgui_event & e, 
    const bvis1_view_tableau_sptr& view )
{
  return mw_correspond_point_tool_basic::handle_mouse_event_whatever_view(e,view);
}

bool mw_3_correspond_point_tool::
handle_mouse_event_at_view_0( 
    const vgui_event & e, 
    const bvis1_view_tableau_sptr& view )
{
  return mw_correspond_point_tool_basic::handle_mouse_event_whatever_view(e,view);
}

bool mw_3_correspond_point_tool::
handle_mouse_event_at_view_1( 
    const vgui_event & /*e*/, 
    const bvis1_view_tableau_sptr& /*view*/ )
{
  // identify the index of the selected edgel in our data
  unsigned idx;
  bool stat  = get_index_of_point( selected_edgel_in_corresp_, vsols_orig_cache_[1], &idx);

  if (stat) {
     std::cout << "View #2: mouse click on point number: (" << idx+1 << ") out of " 
               << vsols_[1].size() << std::endl;
     p_query_id_[1] = idx;
  } else {
     std::cout << "View #2: mouse click on mysterious pont\n";
     return false;
  }

  if (p_query_soview_[1])
    tab_[1]->remove(p_query_soview_[1]);

  tab_[1]->set_current_grouping(p_query_layer_[1].c_str());
  p_query_soview_[1] 
    = tab_[1]->add_vsol_line_2d(vsols_orig_cache_[1][idx]);

  p_query_soview_[1]->set_style(p_query_style_[1]);
  
  // find clicked point among candidates + print info

  if (corr_3_->l_.fullp(p_query_id_[0],p_query_id_[1],p_query_id_[2])) {

//    std::cout << "Clicked on candidate (" <<  ii+1 << ")"  << " out of " 
//      << corr_->corresp_[p_query_id_[0]].size() << std::endl;
//    std::cout << "    " << *itr << std::endl;
    query_is_corresp_ = true;

    /*
    if (synthetic_ || synthetic_olympus_) {
      bdifd_3rd_order_point_2d &p1 = crv2d_gt_[0][p_query_id_[0]];
      bdifd_3rd_order_point_2d &p2 = crv2d_gt_[1][idx];

      bdifd_3rd_order_point_2d p1_w, p2_w;
      bdifd_3rd_order_point_3d P_rec;

      bdifd_rig rig(cam_[0].Pr_,cam_[1].Pr_);

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
      std::cout << "reconstr       --  Point(world coords): " << P_rec.Gama << "\tnorm: "  << P_rec.Gama.two_norm() << "\tdepth: " << depth_rec << std::endl;
      bool is_the_match = (idx == p_query_id_[0]);
      if (is_the_match) {
        unsigned  i_crv, i_pt;
        bool found = find_crv3d_idx(idx, i_crv, i_pt);
        if (found) {
          assert(i_crv < crv3d_gt_.size() && i_pt < crv3d_gt_[i_crv].size());
          bdifd_3rd_order_point_3d &P = crv3d_gt_[i_crv][i_pt];
          printf("gnd-truth      --  K: %8g\t(R=%8g),\tKdot:%8g,\tTau:%8g\tSpeed:%8g\tGamma3dot:%8g\n", P.K, 1./P.K, P.Kdot, P.Tau, 1.0/rig.cam[0].speed(P), P.Gamma_3dot_abs());
          double depth_gnd = dot_product(P.Gama - cam_[0].c, cam_[0].F);
          std::cout << "gnd-truth      --  Point(world coords): " << P.Gama << "\tnorm: "  << P.Gama.two_norm()<< "\tdepth: " << depth_gnd << std::endl;

          // angle of reprojection with the edgel in 3rd view
          bool valid;
          bdifd_3rd_order_point_2d p3_reproj = cam_[2].project_to_image(P_rec,&valid);
          std::cout << "gnd-truth      --  reproj tangent: " << p3_reproj.t <<"\tview3 tangent: " << crv2d_gt_[2][p_query_id_[0]].t 
            << "\tangle(rad): " << std::acos(bmcsd_util::clump_to_acos(p3_reproj.t[0]*(crv2d_gt_[2][p_query_id_[0]].t[0]) + p3_reproj.t[1]*(crv2d_gt_[2][p_query_id_[0]].t[1]))) << std::endl;
          double dotprod = p3_reproj.t[0]*(crv2d_gt_[2][p_query_id_[0]].t[0]) + p3_reproj.t[1]*(crv2d_gt_[2][p_query_id_[0]].t[1]);
          std::cout << "gnd-truth      --  dotprod tangent clumped: " << bmcsd_util::clump_to_acos(dotprod) << "\t> one?" << ((dotprod > 1)?"yes":"no") << std::endl;
          
        } else {
          std::cout << "WARNING: Not found among ground-truth!\n";
        }
      }
    } // ! synthetic
    */
  } else {
    std::cout << "Clicked point NOT found among candidates\n";
    query_is_corresp_ = false;
  }


  // reconstruct+reproject + add soview

  return true;
}

bool mw_3_correspond_point_tool::
handle_mouse_event_at_view_2( 
    const vgui_event & /*e*/, 
    const bvis1_view_tableau_sptr& /*view*/ )
{
  // identify the index of the selected edgel in our data
  unsigned idx;
  bool stat  = get_index_of_point( selected_edgel_in_corresp_, vsols_orig_cache_[2], &idx);

  if (stat) {
     std::cout << "View #3: mouse click on point number: (" << idx+1 << ") out of " 
               << vsols_[2].size() << std::endl;
     p_query_id_[2] = idx;
  } else {
     std::cout << "View #3: mouse click on mysterious pont\n";
     return false;
  }

  if (p_query_soview_[2])
    tab_[2]->remove(p_query_soview_[2]);

  tab_[2]->set_current_grouping(p_query_layer_[2].c_str());
  p_query_soview_[2] 
    = tab_[2]->add_vsol_line_2d(vsols_orig_cache_[2][idx]);

  p_query_soview_[2]->set_style(p_query_style_[2]);
  
  // find clicked point among candidates + print info XXX

  if (corr_3_->l_.fullp(p_query_id_[0],p_query_id_[1],p_query_id_[2])) {

//    std::cout << "Clicked on candidate (" <<  ii+1 << ")"  << " out of " 
//      << corr_->corresp_[p_query_id_[0]].size() << std::endl;
//    std::cout << "    " << *itr << std::endl;
    query_is_corresp_ = true;

    /*
    std::cout << "Clicked on candidate (" <<  ii+1 << ")"  << " out of " 
      << corr_->corresp_[p_query_id_[0]].size() << std::endl;
    std::cout << "    " << *itr << std::endl;
    p1_query_itr_ = itr;
    query_is_corresp_ = true;

    if (synthetic_ || synthetic_olympus_) {
      bdifd_3rd_order_point_2d &p1 = crv2d_gt_[0][p_query_id_[0]];
      bdifd_3rd_order_point_2d &p2 = crv2d_gt_[1][idx];

      bdifd_3rd_order_point_2d p1_w, p2_w;
      bdifd_3rd_order_point_3d P_rec;

      bdifd_rig rig(cam_[0].Pr_,cam_[1].Pr_);

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
      std::cout << "reconstr       --  Point(world coords): " << P_rec.Gama << "\tnorm: "  << P_rec.Gama.two_norm() << "\tdepth: " << depth_rec << std::endl;
      bool is_the_match = (idx == p_query_id_[0]);
      if (is_the_match) {
        unsigned  i_crv, i_pt;
        bool found = find_crv3d_idx(idx, i_crv, i_pt);
        if (found) {
          assert(i_crv < crv3d_gt_.size() && i_pt < crv3d_gt_[i_crv].size());
          bdifd_3rd_order_point_3d &P = crv3d_gt_[i_crv][i_pt];
          printf("gnd-truth      --  K: %8g\t(R=%8g),\tKdot:%8g,\tTau:%8g\tSpeed:%8g\tGamma3dot:%8g\n", P.K, 1./P.K, P.Kdot, P.Tau, 1.0/rig.cam[0].speed(P), P.Gamma_3dot_abs());
          double depth_gnd = dot_product(P.Gama - cam_[0].c, cam_[0].F);
          std::cout << "gnd-truth      --  Point(world coords): " << P.Gama << "\tnorm: "  << P.Gama.two_norm()<< "\tdepth: " << depth_gnd << std::endl;

          // angle of reprojection with the edgel in 3rd view
          bool valid;
          bdifd_3rd_order_point_2d p3_reproj = cam_[2].project_to_image(P_rec,&valid);
          std::cout << "gnd-truth      --  reproj tangent: " << p3_reproj.t <<"\tview3 tangent: " << crv2d_gt_[2][p_query_id_[0]].t 
            << "\tangle(rad): " << std::acos(bmcsd_util::clump_to_acos(p3_reproj.t[0]*(crv2d_gt_[2][p_query_id_[0]].t[0]) + p3_reproj.t[1]*(crv2d_gt_[2][p_query_id_[0]].t[1]))) << std::endl;
          double dotprod = p3_reproj.t[0]*(crv2d_gt_[2][p_query_id_[0]].t[0]) + p3_reproj.t[1]*(crv2d_gt_[2][p_query_id_[0]].t[1]);
          std::cout << "gnd-truth      --  dotprod tangent clumped: " << bmcsd_util::clump_to_acos(dotprod) << "\t> one?" << ((dotprod > 1)?"yes":"no") << std::endl;
          
        } else {
          std::cout << "WARNING: Not found among ground-truth!\n";
        }
      }
    } // !synthetic
    */
  } else {
    std::cout << "Clicked point NOT found among candidates\n";
    query_is_corresp_ = false;
  }

  // reconstruct+reproject + add soview
  return true;
}



//: output: selected_edgel_in_corresp_
// \return true if selected a valid edgel
bool mw_3_correspond_point_tool::
handle_corresp_query_whatever_view
    ( const vgui_event & e, 
    const bvis1_view_tableau_sptr& /*view*/ )
{
//  std::cout << "mouse over - Frame index: " << view->frame() << std::endl;

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
    // std::cout << "mouse over - Nothing\n";
    return false;
  }

  selected_edgel_in_corresp_ = selected_edgel_soview_line->sptr();
  // std::cout << "mouse over - Middle of selected edgel: " << 
    // *(selected_edgel_in_corresp_->middle()) << std::endl;
  return true;
}

//: Returns true if mouse over valid edge in 1st view
bool mw_3_correspond_point_tool::
handle_corresp_query_at_view_0
    ( const vgui_event & /*e*/, 
    const bvis1_view_tableau_sptr& /*view*/ )
{
  // identify the index of the selected edgel in our data
  unsigned idx;
  bool stat  = get_index_of_point( selected_edgel_in_corresp_, vsols_orig_cache_[0], &idx);
  if (stat) {
     std::cout << "mw_3_corr: View[0]: mouse over point number: (" << idx+1 << ") out of " 
               << vsols_[0].size() << std::endl;
     p_query_id_[0] = idx;
     query_is_corresp_ = false; // TODO need to properly check if edgels are already in correps datastruct
  } else {
     std::cout << "mw_3_corr: View[0]: mouse over mysterious point\n";
     return false;
  }

  // show epipolar angle
  if (synthetic_ || synthetic_olympus_) {
    bdifd_3rd_order_point_2d &p1 = crv2d_gt_[0][p_query_id_[0]];

    bdifd_rig rig(cam_[0].Pr_,cam_[1].Pr_);
    double epipolar_angle = bdifd_rig::angle_with_epipolar_line(p1.t,p1.gama,rig.f12);
    epipolar_angle *= 180.0/vnl_math::pi;

    std::cout << "epi angle: " << epipolar_angle << "deg" << std::endl;
  }

  // mark selected edgel
  if (p_query_soview_[0])
    tab_[0]->remove(p_query_soview_[0]);

  tab_[0]->set_current_grouping( p_query_layer_[0].c_str() );
  p_query_soview_[0] = tab_[0]->add_vsol_line_2d(vsols_orig_cache_[0][idx]);
  p_query_soview_[0]->set_style(p_query_style_[0]);
  p_query_style_[0]->apply_all();
  p_query_soview_[0]->draw();
      
  // add the correspondents in 2nd and 3rd view
  
  for (std::list<bgui_vsol_soview2D_line_seg *>::iterator 
      itr = correspondents_soview_[1].begin() ; 
      itr != correspondents_soview_[1].end(); ++itr) {
    tab_[1]->remove(*itr);
  }
  correspondents_soview_[1].clear();
  correspondents_idx_[1].clear();

  for (std::list<bgui_vsol_soview2D_line_seg *>::iterator 
      itr = correspondents_soview_[2].begin() ; 
      itr != correspondents_soview_[2].end(); ++itr) {
    tab_[2]->remove(*itr);
  }
  correspondents_soview_[2].clear();
  correspondents_idx_[2].clear();


  std::list<unsigned> p1_l, p2_l;

  //TODO: sort
  mw_3_correspond_point_tool::get_candidates_from_p0(*corr_3_, idx, p1_l, p2_l);

  std::list<unsigned>::const_iterator itr;

  itr = p1_l.begin(); unsigned ii=0;
  for (; itr != p1_l.end(); ++itr, ++ii) {
    if (ii < best_match_style_.size()) { //: display top 5 matches
      tab_[1]->set_current_grouping( best_match_layer_.c_str() );
    } else {
      tab_[1]->set_current_grouping( corresp_edges_layer_.c_str() );
    }

    correspondents_soview_[1].push_back(
        tab_[1]->add_vsol_line_2d(vsols_orig_cache_[1][*itr]));
    correspondents_idx_[1][*itr] = --correspondents_soview_[1].end();

    if (ii < best_match_style_.size()) { //: display top 5 matches
      correspondents_soview_[1].back()->set_style(best_match_style_[ii]);
    } else {
      correspondents_soview_[1].back()->set_style(corresp_edges_style_);
    }
  }

  itr = p2_l.begin(); ii=0;
  for (; itr != p2_l.end(); ++itr, ++ii) {
    if (ii < best_match_style_.size()) { //: display top 5 matches
      tab_[2]->set_current_grouping( best_match_layer_.c_str() );
    } else {
      tab_[2]->set_current_grouping( corresp_edges_layer_.c_str() );
    }

    correspondents_soview_[2].push_back(
        tab_[2]->add_vsol_line_2d(vsols_orig_cache_[2][*itr]));
    correspondents_idx_[2][*itr] = --correspondents_soview_[2].end();

    if (ii < best_match_style_.size()) { //: display top 5 matches
      correspondents_soview_[2].back()->set_style(best_match_style_[ii]);
    } else {
      correspondents_soview_[2].back()->set_style(corresp_edges_style_);
    }
  }
   
  for (unsigned i=0 ; i < nviews_; ++i)
    tab_[i]->post_redraw();

  return true;
}

//: Returns true if mouse over valid edge in 1st view
bool mw_3_correspond_point_tool::
handle_corresp_inspection_at_view_1
    ( const vgui_event & /*e*/, 
    const bvis1_view_tableau_sptr& /*view*/ )
{
  // identify the index of the selected edgel in our data
  unsigned idx, p1_idx;
  bool stat  = get_index_of_point( selected_edgel_in_corresp_, vsols_orig_cache_[1], &idx);
  if (stat) {
     std::cout << "View #2: mouse over point number: (" << idx+1 << ") out of " 
               << vsols_[1].size() << std::endl;
     p1_idx = idx;
  } else {
     std::cout << "View #2: mouse over mysterious pont\n";
     return false;
  }

  // show epipolar angle
  if (synthetic_ || synthetic_olympus_) {
    bdifd_3rd_order_point_2d &p1 = crv2d_gt_[1][p1_idx];

    bdifd_rig rig(cam_[1].Pr_,cam_[0].Pr_);
    double epipolar_angle = bdifd_rig::angle_with_epipolar_line(p1.t,p1.gama,rig.f12);
    epipolar_angle *= 180.0/vnl_math::pi;

    std::cout << "epi angle: " << epipolar_angle << "deg" << std::endl;
  }

  // add the correspondents in 3rd view
  
  /*
  for (std::list<bgui_vsol_soview2D_line_seg *>::iterator 
      itr = correspondents_soview_3_.begin() ; 
      itr != correspondents_soview_3_.end(); ++itr) {
    tab_[2]->remove(*itr);
  }
  correspondents_soview_3_.clear();
  correspondents_idx_[1]3_.clear();


  std::set<triplet_uuu> s3;

  corr_3_->triplets(0,p_query_id_[0], 1,p1_idx,s3);

  std::set<triplet_uuu>::const_iterator itr;

  itr = s3.begin(); unsigned ii=0;
  for (; itr != s3.end(); ++itr, ++ii) {
    if (ii < best_match_style_.size()) { //: display top 5 matches
      tab_[2]->set_current_grouping( best_match_layer_.c_str() );
    } else {
      tab_[2]->set_current_grouping( corresp_edges_layer_.c_str() );
    }

    correspondents_soview_3_.push_back(
        tab_[2]->add_vsol_line_2d(vsols_orig_cache_[2][itr->third]));
    correspondents_idx_[1]3_[itr->third] = --correspondents_soview_3_.end();

    if (ii < best_match_style_.size()) { //: display top 5 matches
      correspondents_soview_3_.back()->set_style(best_match_style_[ii]);
    } else {
      correspondents_soview_3_.back()->set_style(corresp_edges_style_);
    }
  }
  */
   
  for (unsigned i=0 ; i < nviews_; ++i)
    tab_[i]->post_redraw();

  return true;
}

//: This fn. depends on the correspondence structure implementation.
// TODO: sort contents of lists according to cost
void mw_3_correspond_point_tool::
get_candidates_from_p0(const bmcsd_discrete_corresp_3 &c, unsigned idx, std::list<unsigned> &p1_l, std::list<unsigned> &p2_l)
{
  std::vector<bool> p1_in,p2_in;

  p1_in.resize(c.n1(),false); p2_in.resize(c.n2(),false);

  const std::set<triplet_uuu> t = c.triplets(0,idx);

  std::set<triplet_uuu>::const_iterator p;

  for (p=t.begin(); p!=t.end(); ++p) {
    const unsigned j= p->second;
    const unsigned k= p->third;
    if (c(idx,j,k).cost_ != std::numeric_limits<double>::infinity()) {
      if (!p1_in[j]) {
        p1_l.push_back(j);
        p1_in[j] = true;
      }
      if (!p2_in[k]) {
        p2_l.push_back(k);
        p2_in[k] = true;
      }
    }
  }
}


void mw_3_correspond_point_tool::
toggle_lock_correspondence_query()
{
  std::string state;
  if (lock_corresp_query_) {
    lock_corresp_query_= false;
    state = "off";
  } else {
    lock_corresp_query_= true;
    state = "on";
  }
  std::cout << "Inspect correspondence " << state << std::endl;;
  vgui::out << "Inspect correspondence " << state << std::endl;;

}

//: given linearized index idx into synthetic curves, return index of curve and point within curve
bool mw_3_correspond_point_tool::
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

bgld_eulerspiral * mw_3_correspond_point_tool::
get_new_eulerspiral(const bdifd_3rd_order_point_2d &p) const
{
  vgl_point_2d<double> start_pt(p.gama[0], p.gama[1]);
  double start_angle = bmcsd_util::angle0To2Pi (std::atan2 (p.t[1],p.t[0]));
  return new bgld_eulerspiral(start_pt, start_angle, p.k, p.kdot, 3);
}

void mw_3_correspond_point_tool::
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
            bdifd_turntable::camera_olympus(srm_angle_, K);
      srm_cam_.set_p(*P);

      bool valid;
      bdifd_3rd_order_point_2d p_rec_reproj = srm_cam_.project_to_image(srm_Prec_,&valid);

      if (valid) {
        std::cout << "Angle: " << srm_angle_ << "deg\n";
        es_.push_back(get_new_eulerspiral (p_rec_reproj)); 

        unsigned ie=2;
        std::vector<vsol_point_2d_sptr> vp;

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
        std::cout << "SRM: reconstr      -- depth: " << depth_srm << std::endl;

        unsigned  i_crv, i_pt;
        bool found = find_crv3d_idx(p_query_id_[0], i_crv, i_pt);
        if (found) {
          assert(i_crv < crv3d_gt_.size() && i_pt < crv3d_gt_[i_crv].size());
          bdifd_3rd_order_point_3d &P = crv3d_gt_[i_crv][i_pt];
          printf("SRM: P gnd-truth   --  K: %8g\t(R=%8g),\tKdot:%8g,\tTau:%8g\tEnergy(Gama3dot):%8g\n", P.K, 1./P.K, P.Kdot, P.Tau, P.Gamma_3dot_abs());
          double depth_gnd = dot_product(P.Gama - cam_[0].c, cam_[0].F);
          std::cout << "SRM: P gnd-truth   --    Point(world coords): " << P.Gama << "\tnorm: "  << P.Gama.two_norm()<< "\tdepth: " << depth_gnd << std::endl;
        } else {
          std::cout << "WARNING: Not found ground-truth for P0!\n";
        }
        std::cout << "\n\n";
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
          std::vector<vsol_point_2d_sptr> vp;
          vp.reserve(crv3d_gt_[i_crv].size());
          for (unsigned i_pt=0; i_pt < crv3d_gt_[i_crv].size(); i_pt+=4) {
            bool valid;
            bdifd_3rd_order_point_2d srm_p = srm_cam_.project_to_image(crv3d_gt_[i_crv][i_pt], &valid);
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

void mw_3_correspond_point_tool::
run_stereo_matcher_1pt(mw_point_matcher::trinocular_DG_constraint constr) 
  
{
  // Apply symmetric tangential constraint
  mw_point_matcher m(cam_);

  std::vector<std::vector<bdifd_3rd_order_point_2d> > crv2d;
  build_point_tangents_from_edgels(crv2d,vsols_orig_cache_);

  m.trinocular_DG_costs_3(
    crv2d,
    corr_3_,
    0,1,2,
    constr
   );
  std::cout << "Resulting corr\n"<< *corr_3_<<std::endl;
}

void mw_3_correspond_point_tool::
trinocular_epipolar_candidates_1pt() 
{
  // - make sure point in view 1 is selected
  // - get hold of point in view 1
  // - run stereo such as:

  std::cout << "Computing trinocular epipolar correspondent for current point...";
  if (p_[0] == 0) {
    std::cout << "No selected point in view 0!\n";
    return;
  }

  if (!has_sp_) {
    sp_.resize(nviews_);
    for (unsigned i=0; i < 3; ++i) {
      sp_[i] = new mw_subpixel_point_set (vsols_[i]);
      sp_[i]->build_bucketing_grid(nrows_,ncols_);
    }
    has_sp_ = true;
  }

  std::vector<std::vector <vpgl_fundamental_matrix<double> > > fms;

  fms.resize(nviews_);
  for (unsigned i=0; i < nviews_; ++i) {
    fms[i].resize(nviews_);
    for (unsigned k=0; k < nviews_; ++k) {
      if (k == i)
        continue;
      fms[i][k] = vpgl_fundamental_matrix <double> (cam_[i].Pr_,cam_[k].Pr_);
    }
  }


  std::vector<std::vector< vsol_point_2d_sptr > > vsols_1pt;
  vsols_1pt = vsols_;
  vsols_1pt[0].clear();
  vsols_1pt[0].push_back(vsols_[0][p_idx_[0]]);

  bmcsd_discrete_corresp_n corr_1pt; // all triplets containing point p_idx_[0]
  //: todo initialize 
  std::vector<unsigned> npts(nviews_);
  for (unsigned i=0; i < nviews_; ++i) {
    npts[i] = vsols_[i].size();
  }
  corr_1pt.set_size(npts);

  unsigned err_pos = 2;

  mw_match_position_band(vsols_1pt, &corr_1pt, fms, sp_, err_pos,false);// false = do not debug synthetic
  std::cout << corr_1pt;
  //: Just need to transfer all triplets to internal corr_3 structure, but changing the index of the
  //first point to the right index.
  
  delete corr_3_;
  corr_3_ = new bmcsd_discrete_corresp_3(vsols_[0].size(),vsols_[1].size(),vsols_[2].size());
  p_sto_3_->set_corresp(corr_3_);

  // convert to 3-corresp structure + right index of 1st point
  vbl_sparse_array_base<bmcsd_match_attribute,bmcsd_ntuplet>::const_iterator p;

  for (p = corr_1pt.l_.begin(); p != corr_1pt.l_.end(); ++p) {
    const bmcsd_ntuplet &tup = p->first;
    assert(tup[0] == 0);
    corr_3_->l_.put(p_idx_[0],tup[1],tup[2],p->second);
  }

  std::cout << "Hashing corresp 3...";
  std::cout.flush();
  corr_3_->hash();
  std::cout << "done\n";

  color_pts0_with_correspondents();
}

//: Integrated epipolar + tangent constraint - test tangential consistency as ntuplets are being
// constructed, rather than afterwards.
void mw_3_correspond_point_tool::
trinocular_epipolar_candidates_1pt_with_tangent_constraint() 
{
  // - make sure point in view 1 is selected
  // - get hold of point in view 1
  // - run stereo such as:

  std::cout << "Computing trinocular epipolar correspondent + tangent constraint for current point...";
  if (p_[0] == 0) {
    std::cout << "No selected point in view 0!\n";
    return;
  }

  if (!has_sp_) {
    sp_.resize(nviews_);
    for (unsigned i=0; i < 3; ++i) {
      sp_[i] = new mw_subpixel_point_set (vsols_[i]);
      sp_[i]->build_bucketing_grid(nrows_,ncols_);
    }
    has_sp_ = true;
  }

  std::vector<std::vector <vpgl_fundamental_matrix<double> > > fms;

  fms.resize(nviews_);
  for (unsigned i=0; i < nviews_; ++i) {
    fms[i].resize(nviews_);
    for (unsigned k=0; k < nviews_; ++k) {
      if (k == i)
        continue;
      fms[i][k] = vpgl_fundamental_matrix <double> (cam_[i].Pr_,cam_[k].Pr_);
    }
  }



  std::vector<std::vector< bdifd_3rd_order_point_2d > > dg_points_1pt;
  {
    std::vector<std::vector< vsol_line_2d_sptr > > vsols_line_1pt;
    vsols_line_1pt = vsols_orig_cache_;
    vsols_line_1pt[0].clear();
    vsols_line_1pt[0].push_back(vsols_orig_cache_[0][p_idx_[0]]);

    build_point_tangents_from_edgels(dg_points_1pt,vsols_line_1pt);
  }

  std::vector<std::vector< vsol_point_2d_sptr > > vsols_1pt;
  vsols_1pt = vsols_;
  vsols_1pt[0].clear();
  vsols_1pt[0].push_back(vsols_[0][p_idx_[0]]);

  bmcsd_discrete_corresp_n corr_1pt; // all triplets containing point p_idx_[0]
  //: todo initialize 
  std::vector<unsigned> npts(nviews_);
  for (unsigned i=0; i < nviews_; ++i) {
    npts[i] = vsols_[i].size();
  }
  corr_1pt.set_size(npts);

  double err_pos = 2; double err_t=vnl_math::pi/180.;

  mw_match_tangent_band(vsols_1pt, dg_points_1pt, &corr_1pt, fms, cam_, sp_, err_pos, err_t, false);// false = do not debug synthetic
  std::cout << corr_1pt;
  //: Just need to transfer all triplets to internal corr_3 structure, but changing the index of the
  //first point to the right index.
  
  delete corr_3_;
  corr_3_ = new bmcsd_discrete_corresp_3(vsols_[0].size(),vsols_[1].size(),vsols_[2].size());
  p_sto_3_->set_corresp(corr_3_);

  // convert to 3-corresp structure + right index of 1st point
  vbl_sparse_array_base<bmcsd_match_attribute,bmcsd_ntuplet>::const_iterator p;

  for (p = corr_1pt.l_.begin(); p != corr_1pt.l_.end(); ++p) {
    const bmcsd_ntuplet &tup = p->first;
    assert(tup[0] == 0);
    corr_3_->l_.put(p_idx_[0],tup[1],tup[2],p->second);
  }

  std::cout << "Hashing corresp 3...";
  std::cout.flush();
  corr_3_->hash();
  std::cout << "done\n";

  color_pts0_with_correspondents();
}

void mw_3_correspond_point_tool::
reproject_from_triplet() 
{
  const double t_thresh=vnl_math::pi/20.0;
  std::cout << "Computing reprojections for selected triplet.";
  
  // - make sure three base points are selected
  //    - the indices into vsols are stored in p_idx_[iv]
  if (p_[0] == 0) {
    std::cout << "No selected point in view 0!\n";
    return;
  }
  if (p_[1] == 0) {
    std::cout << "No selected point in view 1!\n";
    return;
  }
  if (p_[2] == 0) {
    std::cout << "No selected point in view 2!\n";
    return;
  }
  

  // - compute reprojections

  std::vector<bdifd_3rd_order_point_2d> dg_pt(3);

  //    - TODO add reprojection objects like in eulerspiral case.
  
  for (unsigned iv=0; iv < nviews_; ++iv) {
    dg_pt[iv].gama[0]  = vsols_[iv][p_idx_[iv]]->x();
    dg_pt[iv].gama[1]  = vsols_[iv][p_idx_[iv]]->y();
    dg_pt[iv].gama[2]  = 0;

    vgl_vector_2d<double> dir = normalized(vsols_orig_cache_[iv][p_idx_[iv]]->direction());

    dg_pt[iv].t[0] = dir.x();
    dg_pt[iv].t[1] = dir.y();
    dg_pt[iv].t[2] = 0;
    dg_pt[iv].n[0] = -dir.y();
    dg_pt[iv].n[1] = dir.x();
    dg_pt[iv].n[2] = 0;
    dg_pt[iv].k = 0; 
    dg_pt[iv].kdot = 0;
    dg_pt[iv].valid = true;
  }
  
  const bdifd_3rd_order_point_2d &pt_img1 = dg_pt[0];
  const bdifd_3rd_order_point_2d &pt_img2 = dg_pt[1];
  const bdifd_3rd_order_point_2d &pt_img3 = dg_pt[2];

  bdifd_rig rig12(cam_[0].Pr_, cam_[1].Pr_);
  bdifd_rig rig13(cam_[0].Pr_, cam_[2].Pr_);
  bdifd_rig rig23(cam_[1].Pr_, cam_[2].Pr_);

  // -- 3
  
  bdifd_3rd_order_point_3d Prec;
  bdifd_3rd_order_point_2d p_rep3; 
  bool valid;
  valid = bdifd_transfer::transfer_by_reconstruct_and_reproject ( pt_img1, pt_img2, p_rep3, Prec, cam_[2], rig12);

  // Output reconstruction from views 0,1
  std::cout << std::setprecision(20);
  std::cout << "Reconstruction from views 0,1:" << std::endl;
  std::cout << "\t Prec.Gama: " << Prec.Gama << std::endl;
  std::cout << "\t Prec.Tangent: " << Prec.T << std::endl;

  // -- 2

  bdifd_3rd_order_point_2d p_rep2; 
  valid = bdifd_transfer::transfer_by_reconstruct_and_reproject ( pt_img1, pt_img3, p_rep2, Prec, cam_[1], rig13);
  
  // -- 1

  bdifd_3rd_order_point_2d p_rep1; 
  valid = bdifd_transfer::transfer_by_reconstruct_and_reproject ( pt_img2, pt_img3, p_rep1, Prec, cam_[0], rig23);
  
  // compare p_rep and pt_img3
  double dt3 = std::acos(bmcsd_util::clump_to_acos(p_rep3.t[0]*pt_img3.t[0] + p_rep3.t[1]*pt_img3.t[1]));
  // compare p_rep and pt_img2
  double dt2 = std::acos(bmcsd_util::clump_to_acos(p_rep2.t[0]*pt_img2.t[0] + p_rep2.t[1]*pt_img2.t[1]));
  // compare p_rep and pt_img1
  double dt1 = std::acos(bmcsd_util::clump_to_acos(p_rep1.t[0]*pt_img1.t[0] + p_rep1.t[1]*pt_img1.t[1]));

  std::cout << "dt1, dt2, dt3 = " << dt1*(180./vnl_math::pi) << "\t" << dt2*(180./vnl_math::pi) << "\t" << dt3*(180./vnl_math::pi) << " (deg)" << std::endl;

  std::cout << "directions: " 
    << vsols_orig_cache_[0][p_idx_[0]]->direction() << "; " 
    << vsols_orig_cache_[1][p_idx_[1]]->direction() << "; " 
    << vsols_orig_cache_[2][p_idx_[2]]->direction() << std::endl;


  if ( dt1 > t_thresh ||
       dt2 > t_thresh ||
       dt3 > t_thresh 
      ) {
    std::cout << "Triplet FAIL\n";
//        if (dt1 < t_thresh && dt2 > t_thresh)
//          std::cout << "dt1 < thresh but symmetrics are not\n";
//    if (dt1 < t_thresh)
//      std::cout << "dt1 < thresh but symmetrics are not\n";

//    if (dt2 < t_thresh)
//      std::cout << "dt2 < thresh but symmetrics are not\n";

//    if (dt3 < t_thresh)
//      std::cout << "dt3 < thresh but symmetrics are not\n";

  }
}

void mw_3_correspond_point_tool::
reproject_from_triplet_allcorr() 
{
  std::cout << std::setprecision(20);

  // Output cameras
  for (unsigned v=0; v < nviews_; ++v) {
    std::cout << "Cam from View: " << v << std::endl;
    std::cout << "K:\n" << cam_[v].K_ << std::endl;
    std::cout << "Rot: " << cam_[v].Rot << std::endl;
    std::cout << "C: " << cam_[v].c << std::endl;
  }

  vbl_sparse_array_3d<bmcsd_match_attribute>::const_iterator p;
  for (p = corr_3_->l_.begin(); p != corr_3_->l_.end(); ++p) {
      std::cout   << '(' << (*p).first.first
          << ',' << (*p).first.second
          << ',' << (*p).first.third
          << "): " << (*p).second << '\n';

      std::vector<unsigned> p_id(3);
      p_id[0] = (*p).first.first;
      p_id[1] = (*p).first.second;
      p_id[2] = (*p).first.third;

      
      // Output image points and tangents

      std::vector<bdifd_3rd_order_point_2d> dg_pt(3);
      for (unsigned iv=0; iv < nviews_; ++iv) {
        dg_pt[iv].gama[0]  = vsols_[iv][p_id[iv]]->x();
        dg_pt[iv].gama[1]  = vsols_[iv][p_id[iv]]->y();
        dg_pt[iv].gama[2]  = 0;

        std::cout << "gama_img[" << iv << "]:" << dg_pt[iv].gama << std::endl;

        vgl_vector_2d<double> dir = normalized(vsols_orig_cache_[iv][p_id[iv]]->direction());

        dg_pt[iv].t[0] = dir.x();
        dg_pt[iv].t[1] = dir.y();
        dg_pt[iv].t[2] = 0;
        std::cout << "tgt_img[" << iv << "]:" << dg_pt[iv].t << std::endl;

        dg_pt[iv].n[0] = -dir.y();
        dg_pt[iv].n[1] = dir.x();
        dg_pt[iv].n[2] = 0;
        dg_pt[iv].k = 0; 
        dg_pt[iv].kdot = 0;
        dg_pt[iv].valid = true;
      }
      const bdifd_3rd_order_point_2d &pt_img1 = dg_pt[0];
      const bdifd_3rd_order_point_2d &pt_img2 = dg_pt[1];
      const bdifd_3rd_order_point_2d &pt_img3 = dg_pt[2];

      bdifd_rig rig12(cam_[0].Pr_, cam_[1].Pr_);
      bdifd_rig rig13(cam_[0].Pr_, cam_[2].Pr_);
      bdifd_rig rig23(cam_[1].Pr_, cam_[2].Pr_);

      // -- 3
      
      bdifd_3rd_order_point_3d Prec;
      bdifd_3rd_order_point_2d p_rep3; 
      bool valid;
      valid = bdifd_transfer::transfer_by_reconstruct_and_reproject ( pt_img1, pt_img2, p_rep3, Prec, cam_[2], rig12);

      // Output reconstruction from views 0,1
      std::cout << std::setprecision(20);
      std::cout << "Reconstruction from views 0,1:" << std::endl;
      std::cout << "\t Prec.Gama: " << Prec.Gama << std::endl;
      std::cout << "\t Prec.Tangent: " << Prec.T << std::endl;

      // -- 2

      bdifd_3rd_order_point_2d p_rep2; 
      valid = bdifd_transfer::transfer_by_reconstruct_and_reproject ( pt_img1, pt_img3, p_rep2, Prec, cam_[1], rig13);
      
      // -- 1

      bdifd_3rd_order_point_2d p_rep1; 
      valid = bdifd_transfer::transfer_by_reconstruct_and_reproject ( pt_img2, pt_img3, p_rep1, Prec, cam_[0], rig23);
      
      // compare p_rep and pt_img3
      double dt3 = std::acos(bmcsd_util::clump_to_acos(p_rep3.t[0]*pt_img3.t[0] + p_rep3.t[1]*pt_img3.t[1]));
      // compare p_rep and pt_img2
      double dt2 = std::acos(bmcsd_util::clump_to_acos(p_rep2.t[0]*pt_img2.t[0] + p_rep2.t[1]*pt_img2.t[1]));
      // compare p_rep and pt_img1
      double dt1 = std::acos(bmcsd_util::clump_to_acos(p_rep1.t[0]*pt_img1.t[0] + p_rep1.t[1]*pt_img1.t[1]));

      std::cout << "dt1, dt2, dt3 = " << dt1*(180./vnl_math::pi) << "\t" << dt2*(180./vnl_math::pi) << "\t" << dt3*(180./vnl_math::pi) << " (deg)" << std::endl;

      std::cout << "directions: " 
        << vsols_orig_cache_[0][p_id[0]]->direction() << "; " 
        << vsols_orig_cache_[1][p_id[1]]->direction() << "; " 
        << vsols_orig_cache_[2][p_id[2]]->direction() << std::endl;

      const double t_thresh=vnl_math::pi/20.0;

      if ( dt1 > t_thresh ||
           dt2 > t_thresh ||
           dt3 > t_thresh 
          ) {
        std::cout << "Triplet FAIL orientation threshold of " 
          << t_thresh*180.0/vnl_math::pi << "deg" << std::endl;
    //        if (dt1 < t_thresh && dt2 > t_thresh)
    //          std::cout << "dt1 < thresh but symmetrics are not\n";
    //    if (dt1 < t_thresh)
    //      std::cout << "dt1 < thresh but symmetrics are not\n";

    //    if (dt2 < t_thresh)
    //      std::cout << "dt2 < thresh but symmetrics are not\n";

    //    if (dt3 < t_thresh)
    //      std::cout << "dt3 < thresh but symmetrics are not\n";

      }
      std::cout << "---------" << std::endl;
  }


  // Same as above, but now output rows nx3.
  // -1 separates triplets.

  for (p = corr_3_->l_.begin(); p != corr_3_->l_.end(); ++p) {
      std::cout << "-1 -1 -1\n";    // triplet delimiter
      std::cout << (*p).first.first << ' ' << (*p).first.second
          << ' ' << (*p).first.third << std::endl;

      std::vector<unsigned> p_id(3);
      p_id[0] = (*p).first.first;
      p_id[1] = (*p).first.second;
      p_id[2] = (*p).first.third;

      
      // Output image points and tangents

      std::vector<bdifd_3rd_order_point_2d> dg_pt(3);
      for (unsigned iv=0; iv < nviews_; ++iv) {
        dg_pt[iv].gama[0]  = vsols_[iv][p_id[iv]]->x();
        dg_pt[iv].gama[1]  = vsols_[iv][p_id[iv]]->y();
        dg_pt[iv].gama[2]  = 0;

        std::cout << dg_pt[iv].gama << std::endl;

        vgl_vector_2d<double> dir = normalized(vsols_orig_cache_[iv][p_id[iv]]->direction());

        dg_pt[iv].t[0] = dir.x();
        dg_pt[iv].t[1] = dir.y();
        dg_pt[iv].t[2] = 0;
        std::cout << dg_pt[iv].t << std::endl;

        dg_pt[iv].n[0] = -dir.y();
        dg_pt[iv].n[1] = dir.x();
        dg_pt[iv].n[2] = 0;
        dg_pt[iv].k = 0; 
        dg_pt[iv].kdot = 0;
        dg_pt[iv].valid = true;
      }
      const bdifd_3rd_order_point_2d &pt_img1 = dg_pt[0];
      const bdifd_3rd_order_point_2d &pt_img2 = dg_pt[1];
      const bdifd_3rd_order_point_2d &pt_img3 = dg_pt[2];

      bdifd_rig rig12(cam_[0].Pr_, cam_[1].Pr_);
      bdifd_rig rig13(cam_[0].Pr_, cam_[2].Pr_);
      bdifd_rig rig23(cam_[1].Pr_, cam_[2].Pr_);

      // -- 3
      
      bdifd_3rd_order_point_3d Prec;
      bdifd_3rd_order_point_2d p_rep3; 
      bool valid;
      valid = bdifd_transfer::transfer_by_reconstruct_and_reproject ( pt_img1, pt_img2, p_rep3, Prec, cam_[2], rig12);

      // Output reconstruction from views 0,1
      std::cout << Prec.Gama << std::endl;
      std::cout << Prec.T << std::endl;

      // -- 2

      bdifd_3rd_order_point_2d p_rep2; 
      valid = bdifd_transfer::transfer_by_reconstruct_and_reproject ( pt_img1, pt_img3, p_rep2, Prec, cam_[1], rig13);
      
      // -- 1

      bdifd_3rd_order_point_2d p_rep1; 
      valid = bdifd_transfer::transfer_by_reconstruct_and_reproject ( pt_img2, pt_img3, p_rep1, Prec, cam_[0], rig23);
      
      // compare p_rep and pt_img3
      double dt3 = std::acos(bmcsd_util::clump_to_acos(p_rep3.t[0]*pt_img3.t[0] + p_rep3.t[1]*pt_img3.t[1]));
      // compare p_rep and pt_img2
      double dt2 = std::acos(bmcsd_util::clump_to_acos(p_rep2.t[0]*pt_img2.t[0] + p_rep2.t[1]*pt_img2.t[1]));
      // compare p_rep and pt_img1
      double dt1 = std::acos(bmcsd_util::clump_to_acos(p_rep1.t[0]*pt_img1.t[0] + p_rep1.t[1]*pt_img1.t[1]));

      std::cout << dt1*(180./vnl_math::pi) << " " << dt2*(180./vnl_math::pi) << " " << dt3*(180./vnl_math::pi) << std::endl;
  }
}

void mw_3_correspond_point_tool::
color_pts0_with_correspondents() 
{
  tab_[0]->set_current_grouping(corresp_edges_layer_.c_str());

  assert (corr_3_->is_hashed());
  for (unsigned i=0; i < corr_3_->n0(); ++i) {
    if (p_corresp_soview_[0][i]) {
      tab_[0]->remove(p_corresp_soview_[0][i]);
      p_corresp_soview_[0][i] = 0;
    }
      
    if ( !(corr_3_->hash_[0][i].empty()) ) {
      p_corresp_soview_[0][i] = 
        tab_[0]->add_vsol_line_2d(vsols_orig_cache_[0][i]);

       p_corresp_soview_[0][i]->set_style(corresp_edges_style_);

      /* TODO
      if (!gt_.is_empty()) {
        if (corr_->is_gt_among_top5(i, &gt_)) {
          p_corresp_soview_[0][i]->set_style(corresp_edges_style_);
        } else {
          p_corresp_soview_[0][i]->set_style(wrongly_matched_edgels_style_);
        }
      } 
      */
    }
  }
  tab_[0]->post_redraw();
}

void mw_3_correspond_point_tool::
build_point_tangents_from_edgels(
    std::vector<std::vector<bdifd_3rd_order_point_2d> > &dg,
    const std::vector<std::vector<vsol_line_2d_sptr> > &v
    )
{
  unsigned nviews = v.size();
  dg.resize(nviews);
  for (unsigned iv=0; iv < nviews; ++iv) {
    dg[iv].resize(v[iv].size());
    for (unsigned ip=0; ip < v[iv].size(); ++ip) {
      dg[iv][ip].gama[0]  = v[iv][ip]->middle()->x();
      dg[iv][ip].gama[1]  = v[iv][ip]->middle()->y();
      dg[iv][ip].gama[2]  = 0;
      
      vgl_vector_2d<double> dir = normalized(v[iv][ip]->direction());

      dg[iv][ip].t[0] = dir.x();
      dg[iv][ip].t[1] = dir.y();
      dg[iv][ip].t[2] = 0;
      dg[iv][ip].n[0] = -dir.y();
      dg[iv][ip].n[1] = dir.x();
      dg[iv][ip].n[2] = 0;
      dg[iv][ip].k = 0; 
      dg[iv][ip].kdot = 0;
      dg[iv][ip].valid = true;
    }
  }
}

