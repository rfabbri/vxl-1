#include "mw_synth_interact_tool.h"
#include "mw_curve_tracing_tool_common_2.h"

#include <vnl/vnl_random.h>
#include <vil/algo/vil_colour_space.h>

#include <sdetd/pro/sdetd_sel_storage.h>
#include <vpgld/io/vpgld_io_cameras.h>
#include <bmcsd/algo/bmcsd_algo_util.h>

static double focal_length_;
static vnl_random g_myrand;


mw_synth_interact_tool::
mw_synth_interact_tool()
{
}

void mw_synth_interact_tool::
init(bmcsd_curve_stereo *impl)
{
  cc_style_ = vgui_style::new_style(0.2, 0.5, 1, 1.0f, 3.0f); 

  ep_style_ = vgui_style::new_style();
  ep_style_->rgba[0] = 1;
  ep_style_->rgba[1] = 1;
  ep_style_->rgba[2] = 1;
  ep_style_->rgba[3] = 0.07;
  ep_style_->line_width = 1;
  ep_style_->point_size = 1;
  gesture0_ = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);

}


mw_synth_interact_tool::
~mw_synth_interact_tool()
{
}

bool 
mw_synth_interact_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  curve_tableau_current_.vertical_cast(tableau);
  if( curve_tableau_current_ == 0 )  {
    std::cerr << "Warning: working in a tableau which is not expected\n";
    return false;
  }

  return true;
}

std::string
mw_synth_interact_tool::name() const
{
  return "Multiview Curve Tracing v.2 Maro Project";
}

void mw_synth_interact_tool::
activate()
{
  assert(s_);
  std::cout << "mw_synth_interact_tool ON\n";

  std::vector< bvis1_view_tableau_sptr > views;
  views = MANAGER->get_views();

  edgs_.reserve(nviews());

  std::cout << "Working in frames ";
  for (unsigned i=0; i<nviews(); ++i) {
    frame_v_[i] = views[i]->frame();
    std::cout << frame_v(i) << "  ";
  }
  std::cout << std::endl;

  init_tableaux(views);
}

void   
mw_synth_interact_tool::
deactivate ()
{
  std::cout << "mw_synth_interact_tool OFF\n";
}

bool
mw_synth_interact_tool::
handle( const vgui_event & e, 
        const bvis1_view_tableau_sptr& view )
{
  if (!tab_[0])
    return false;

  if (gesture0_(e))
    return handle_mouse_click(e,view);
  // else nothing selected

  if (e.type == vgui_KEY_PRESS && p0_) {
    switch (e.key) {
      case 's': {
          swap_p0_and_pn();

        return true;
        break;
      }
      case '.': { // advance pn outwards
        vgui::out << "Moving outwards\n";
        std::cout  << "Moving outwards\n";

        vsol_point_2d_sptr pt = s_->advance_endpoint();
        if (pt)
          update_pn(pt);

        return true;
        break;
      }
      case ',': { // advance pn inwards
        vgui::out << "Moving inwards\n";
        std::cout  << "Moving inwards\n";

        vsol_point_2d_sptr pt = s_->recede_endpoint();
        if (pt)
          update_pn(pt);

        return true;
        break;
      }

      case 'u': {
        vgui::out << "Equalizing matching subcurves\n";
        std::cout  << "Equalizing matching subcurves\n";

        equalize_matching_subcurve();
        return true;
        break;
      }

      case 'g': { 
        // toggle if should display all intersections of pencil ep0..ep1 and edges in image 2
        std::cout << "Toggling display intersections\n";
        vgui::out << "Toggling display intersections\n";

        display_all_intersections_ = !display_all_intersections_;
        //: if false, remove soviews; if true, add them (add_spatial_object)
        update_display_for_intersection_stuff();

        return true;
        break;
      }
      case 'i': {
        // toggle if should display+compute all intersections of pencil ep0..ep1 and edges in image 2
        std::cout << "Toggling compute intersections\n";
        vgui::out << "Toggling compute intersections\n";

        compute_isets_ = !compute_isets_;
        display_all_intersections_ = compute_isets_;
        //: if false, remove soviews; if true, add them (add_spatial_object)
        if (compute_isets_)
          draw_candidate_curves();
        update_display_for_intersection_stuff();

        return true;
        break;
      }
      case 'e': {
        // toggle if should display+compute all ep0..ep1 in image 2
        std::cout << "Toggling display epipolars in image 2\n";
        vgui::out << "Toggling display epipolars in image 2\n";

        display_all_right_epips_ = !display_all_right_epips_;
        //: if false, remove soviews; if true, add them (add_spatial_object)
        update_display_for_epipolar_curve_pencil();

        return true;
        break;
      }
      case 'w': {
        // toggle if should display+compute all ep0..ep1 in image 1
        std::cout << "Toggling display epipolars in image 1\n";
        vgui::out << "Toggling display epipolars in image 1\n";

        display_all_left_epips_ = !display_all_left_epips_;
        //: if false, remove soviews; if true, add them (add_spatial_object)
        update_display_for_epipolar_curve_pencil();

        return true;
        break;
      }
      case '4': {
        // toggle if should display+compute all ep0..ep1 in image 3
        std::cout << "Toggling display epipolars in image[v], v >= 2\n";
        vgui::out << "Toggling display epipolars in image[v], v >= 2\n";

        display_all_nth_view_epips_= !display_all_nth_view_epips_;
        //: if false, remove soviews; if true, add them (add_spatial_object)
        update_display_for_epipolar_curve_pencil();

        return true;
        break;
      }
      case 'y': {

        std::cout << "Reconstructing selected matches using all n views \n";
        vgui::out << "Reconstructing selected matches using all n views \n";
        s_->reconstruct_multiview();

        return true;
        break;
      }
      case '1': { 
        std::cout << "Breaking the curves into episegs\n";
        vgui::out << "Breaking the curves into episegs\n";
        break_curves_into_episegs();
        break;
      }
      case '2': { 
        std::string state;
        if (click_selects_whole_curve_) {
          click_selects_whole_curve_ = false;
          state = "off";
        } else {
          click_selects_whole_curve_ = true;
          state = "on";
        }
        std::cout << "Whole curve selection mode " << state << std::endl;;
        vgui::out << "Whole curve selection mode " << state << std::endl;;
        break;
      }
      case '>': { 

        increment_focalength(++focal_length_*1./11.);
        break;
      }
      case '<': { 
        increment_focalength(--focal_length_*1./11.);
      }

      default:
        std::cout << "Unassigned key: " << e.key << " pressed.\n";
        break;
    }
  }

  //  We are not interested in other events,
  //  so pass event to base class:
  return false;
}


void mw_synth_interact_tool::
init_tableaux(std::vector< bvis1_view_tableau_sptr > &views)
{
  // Get two active tableaus (original edge vsols), one from each view
  for (unsigned i=0 ; i < nviews(); ++i) {
    vgui_tableau_sptr tab_ptr = views[i]->selector()->active_tableau();

    if (tab_ptr) {
      bgui_vsol2D_tableau_sptr curve_tab;
      curve_tab.vertical_cast(tab_ptr);
      if (!curve_tab) {
        std::cerr << "error: no active vsol in left tableau\n";
        return;
      }
    } else {
      std::cerr << "error: Could not find active child tableaus in selector; line" << __LINE__ << std::endl;
      return ;
    }
  }


  // -------- Add tableaus to draw on
 
  std::string type("vsol2D");
  std::string name("mw_curve_tracer");

  for (unsigned i=0 ; i < nviews(); ++i) {
    bpro1_storage_sptr 
      n_data  = MANAGER->repository()->new_data_at(type,name,frame_v_[i]);

    if (n_data) {
       MANAGER->add_to_display(n_data);
    } else {
       std::cerr << "error: unable to register new data\n";
       return ;
    }
  }

  MANAGER->display_current_frame();

  for (unsigned i=0 ; i < nviews(); ++i) {
    vgui_tableau_sptr tab_ptr1 = views[i]->selector()->get_tableau(name);
    if (tab_ptr1) {
      tab_[i].vertical_cast(tab_ptr1);
    } else {
      std::cerr << "error: Could not find child tableaus in selector\n";
      return ;
    }

    std::string active;
    active = views[i]->selector()->active_name();

    views[i]->selector()->set_active(name);
    views[i]->selector()->active_to_top();
    views[i]->selector()->set_active(active);
  }
}

void mw_synth_interact_tool::
get_curves(std::vector< bvis1_view_tableau_sptr > &views)
{
  std::vector<std::vector< vsol_polyline_2d_sptr > > curves (nviews()); 

  for (unsigned v=0 ; v < nviews(); ++v)
    if (!get_vsols(views[v], v, &curves[v]))
      if (!get_sels(views[v], &curves[v]))
        std::cerr << "Error: Neither vsols or sels are active in the current view." << std::endl;

  s_->set_curves(curves);
}

bool  mw_synth_interact_tool::
get_sels(
    const bvis1_view_tableau_sptr &view, 
    std::vector< vsol_polyline_2d_sptr > *pcurves)
{
  std::vector< vsol_polyline_2d_sptr > &curves = *pcurves;

  bpro1_storage_sptr 
      p = MANAGER->storage_from_tableau(view->selector()->active_tableau());

  sdetd_sel_storage_sptr sel_storage = NULL;
  sel_storage.vertical_cast(p);
  if(!sel_storage)
    return false;
  sels_.push_back(sel_storage);

  // Now extract the curve fragments and put in the curve std::vector.

  if (sels_.back()->CFG().frags.empty())
    return false;

  sdet_edgel_chain_list &frags = sels_.back()->CFG().frags;
  curves.reserve(frags.size());

  for (sdet_edgel_chain_list_iter f_it = frags.begin(); 
      f_it != frags.end(); ++f_it) {

    std::vector<vsol_point_2d_sptr> pts;
    bmcsd_algo_util::sdet_to_vsol((*f_it)->edgels, &pts);
    curves.push_back(new vsol_polyline_2d(pts));
  }

  return true;
}

void mw_synth_interact_tool::
clear_previous_selections() {
  // Clear previous selections
  if (reproj_soview_.front()) {
    for (unsigned k=0; k+2 < nviews(); ++k) {

      tab_[k+2]->remove(reproj_soview_[k]);
      tab_[k+2]->remove(ep0_soview_2n_[k]);
      tab_[k+2]->remove(epn_soview_2n_[k]);
      tab_[k+2]->post_redraw();
      reproj_soview_[k] = 0;
      
      std::list<vgui_soview2D_infinite_line *>::const_iterator itr;
      for (itr = ep_soviews_2n_[k].begin(); 
          itr != ep_soviews_2n_[k].end(); ++itr)
        tab_[k+2]->remove(*itr);
      ep_soviews_2n_[k].clear();
    }

      
    for (unsigned k=0; k < nviews(); ++k) {
      for (unsigned i=0; i < p_reproj_soviews_[k].size(); ++i)
        tab_[k]->remove(p_reproj_soviews_[k][i]);
      p_reproj_soviews_[k].clear();
    }
  }
}

bool mw_synth_interact_tool::
handle_mouse_click( 
    const vgui_event & e, 
    const bvis1_view_tableau_sptr& view)
{
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
  ix = MW_ROUND(ix); iy = MW_ROUND(iy);

  clear_previous_selections();

  // Curve selection

  vgui_soview2D* selected_curve_soview =  
    (vgui_soview2D*)curve_tableau_current_->get_highlighted_soview();

  std::cout << "Frame index: " << view->frame() << std::endl;

  bgui_vsol_soview2D_polyline *selected_curve_soview_poly
    = dynamic_cast<bgui_vsol_soview2D_polyline *>(selected_curve_soview); 

  if (!selected_curve_soview_poly) {
    std::cout << "Selected non-Polyline object" << std::endl;
    return false;
  }

  vsol_polyline_2d_sptr scrv = selected_curve_soview_poly->sptr();
  std::cout << "Size of selected curve: " << scrv->size() << std::endl;


  if (view->frame() == frame_v(0)) {
    s_->set_selected_crv(0, scrv);

    if (selected_crv_id_view0() != curve_tableau_current_->get_highlighted()) {
      selected_new_curve_ = true;
      set_selected_crv_id_view0(curve_tableau_current_->get_highlighted());
    } else
      selected_new_curve_ = false;

    if (click_selects_whole_curve_) {
        std::cout << "Selecting whole curve.\n";

        s_->initialize_subcurve(0);
        initialize_curve_selection();
        s_->update_endpoint(s_->selected_crv(0)->size()-1);
        update_pn(s_->selected_crv(0)->p1());
    
    }  else { // ! click_selects_whole_curve_
      if (selected_new_curve_) {
        std::cout << "New curve selected.\n";

        // mark clicked point
        vsol_point_2d_sptr pt = new vsol_point_2d(ix,iy);
        unsigned mindist;
        unsigned near_id = bmcsd_util::find_nearest_pt(pt, s_->selected_crv(0), mindist);

        s_->initialize_subcurve(near_id);
        initialize_curve_selection();

      } else { // State C: p0 and pn selected; one of them is active

        vsol_point_2d_sptr pt = new vsol_point_2d(ix,iy);
        unsigned mindist;
        unsigned near_id = bmcsd_util::find_nearest_pt(pt, s_->selected_crv(0), mindist);
        s_->update_endpoint(near_id);
        pt = s_->selected_crv(0)->vertex(near_id);
        update_pn(pt);
      }
    } // endif ! click_selects_whole_curve_
  } else { 
    if (view->frame() == frame_v(1)) {
      std::cout << "Processing a curve in the second view" << std::endl;

      unsigned jnz;
      bool stat = s_->get_index_of_candidate_curve(scrv, &jnz);

      if (stat) {
        std::cout << "You selected curve[" << jnz << "] among " 
                 << s_->num_candidates() << " epipolar candidates." << std::endl;

        show_reprojections(jnz);
      } else
        std::cout << "Selected soview not found in list of candidate vsols.\n";

      s_->set_selected_crv(1, scrv);

      const unsigned v = 1;
      if (selected_crv_soviews_n_[v-1])
        tab_[v]->remove(selected_crv_soviews_n_[v-1]);

      selected_crv_soviews_n_[v-1]
        = tab_[v]->add_vsol_polyline_2d(s_->selected_crv(v), best_match_style_[3]); 
      // just any color, whatever
      
      tab_[v]->post_redraw();

    } else { // (view >= 2)

      const int v = view_from_frame_index(view->frame());

      s_->set_selected_crv(v, scrv);

      std::cout << "Processing a curve in view[v], v = " << v << std::endl;

      if (selected_crv_soviews_n_[v-1])
        tab_[v]->remove(selected_crv_soviews_n_[v-1]);

      selected_crv_soviews_n_[v-1]
        = tab_[v]->add_vsol_polyline_2d(s_->selected_crv(v), best_match_style_[3]); // just any color, whatever
      tab_[v]->post_redraw();
    }
  }
  return true;
}

//: Given selected curves in the first two views, reprojects the underlying 3D
// curve in all other views and displays it.
//
//\param[in] crv2_id : index into crv_candidates_ptrs_ of selected candidate curve in 2nd
// view
void mw_synth_interact_tool::
show_reprojections(unsigned crv2_id)
{
  //: reproj[view_id][pt_id]
  std::vector< std::vector<vsol_point_2d_sptr> > reproj; 

  s_->reproject_in_all_views(crv2_id, &reproj);

  // ---------- GUI -----------

  for (unsigned i=0; i < nviews(); ++i)
    tab_[i]->set_current_grouping( "Drawing" );

  for (unsigned v=2; v < nviews(); ++v) {
    // Add a soview with the reprojection. 
    vsol_polyline_2d_sptr reproj_poly = new vsol_polyline_2d(reproj[v]);
    vgui_style_sptr cc_style_pumped = vgui_style::new_style(0.2, 0.5, 1, 1.0f, 5.0f); 
    reproj_soview_[v-2] = tab_[v]->add_vsol_polyline_2d(reproj_poly, cc_style_pumped);

    // soview for epip. lines for reprojection: endpts
    tab_[v]->set_foreground(color_pn_.r,color_pn_.g,color_pn_.b);
    epn_soview_2n_[v-2] = tab_[v]->add_infinite_line(
        s_->ep_v1_end(v-2).a(),s_->ep_v1_end(v-2).b(),s_->ep_v1_end(v-2).c());

    tab_[v]->set_foreground(color_p0_.r,color_p0_.g,color_p0_.b);
    ep0_soview_2n_[v-2] = tab_[v]->add_infinite_line(
        s_->ep_v1_ini(v-2).a(),s_->ep_v1_ini(v-2).b(),s_->ep_v1_ini(v-2).c());
  }

  // epip. lines for reprojection: all other points
  if (display_all_nth_view_epips_) {
    ep_soviews_2n_.resize(nviews()-2);
    for (unsigned v=0; v+2 < nviews(); ++v) {
      // II: Soviews
      std::list<vgui_soview2D_infinite_line *>::const_iterator itr;
      for (itr = ep_soviews_2n_[v].begin(); itr != ep_soviews_2n_[v].end(); ++itr)
        tab_[v+2]->remove(*itr);
      ep_soviews_2n_[v].clear();

      for (unsigned i=0; i < s_->ep_v1(v).size(); ++i) {
        ep_soviews_2n_[v].push_back(
          tab_[v+2]->add_infinite_line(s_->ep_v1(v)[i].a(),s_->ep_v1(v)[i].b(),s_->ep_v1(v)[i].c() ));
        ep_soviews_2n_[v].back()->set_style(ep_style_);
      }
    }
  }
    
  for (unsigned i=0; i < nviews(); ++i)
    tab_[i]->post_redraw();
}

//: Break curves into epipolar tangent segments.
void mw_synth_interact_tool::
break_curves_into_episegs()
{
  std::vector<std::vector< vsol_polyline_2d_sptr > > broken_vsols;
  {
    std::vector<bbld_subsequence_set> sseq;
    s_->break_curves_into_episegs_pairwise(&broken_vsols, &sseq);
  }
  assert(broken_vsols.size() == nviews());

  for (unsigned v=0; v < nviews(); ++v) {
    // Create vsol storage; add to repository; make it active
    std::vector<vsol_spatial_object_2d_sptr> base_vsols2;
    base_vsols2.resize(broken_vsols[v].size());
    for (unsigned i=0; i< broken_vsols[v].size(); ++i)
      base_vsols2[i] = broken_vsols[v][i].ptr();

    // output {broken_vsols}i with the smallest non-existent i
    
    std::string broken_vsols_name("invalid_name");
    {
      std::vector< bvis1_view_tableau_sptr > views;
      views = MANAGER->get_views();
      vgui_selector_tableau &selector = *(views[v]->selector());
      for (unsigned i=0; i < 50; ++i) {
        std::ostringstream s;
        if (i != 0) {
          s << i;
        }

        std::string putative_name = std::string("broken_vsols") + s.str();
        vgui_tableau_sptr t = selector.get_tableau(putative_name);
        if (!t) {
          if (broken_vsols_name == "invalid_name")
            broken_vsols_name = putative_name;
        } else {
          broken_vsols_name = "invalid_name";
          assert(i != 49);
        }
      }
    }

    // create the output storage class
    {
      vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
      output_vsol->add_objects(base_vsols2, broken_vsols_name);
      output_vsol->set_name(broken_vsols_name);
      MANAGER->repository()->store_data_at(output_vsol, frame_v_[v]);
      MANAGER->add_to_display(output_vsol);
      MANAGER->display_current_frame();
    }

    // make it active
    std::vector< bvis1_view_tableau_sptr > views;
    views = MANAGER->get_views();
    assert (views.size() == nviews());
    views[v]->selector()->set_active(broken_vsols_name);
  }
}

unsigned mw_synth_interact_tool::
view_from_frame_index(unsigned fi) const
{
  std::vector<int>::const_iterator result = 
    std::find(frame_v_.begin(), frame_v_.end(), fi);
  
  return result - frame_v_.begin();
}

void mw_synth_interact_tool::
equalize_matching_subcurve()
{
  // this block could be made into a memer of bmcsd_curve_stereo if it turns out to be useful.
  unsigned ini_id_sub, end_id_sub;
  { 
  if (!s_->selected_crv(0) || !s_->selected_crv(1))
    std::cerr << "A curve must be selected in each view." << std::endl;

  unsigned ic;
  if (!s_->get_index_of_candidate_curve(s_->selected_crv(1), &ic))
    std::cerr << "Error: selected curve not found among candidates.\n";

  unsigned ini_id, 
           end_id; 

  s_->get_increasing_endpoints(&ini_id, &end_id);

  s_->get_matching_subcurve(
      ic,
      ini_id,
      end_id,
      &ini_id_sub,
      &end_id_sub);

  s_->set_subcurve(ini_id_sub, end_id_sub);
  }

  initialize_curve_selection();
  update_pn(s_->subcurve()->vertex(s_->subcurve()->size()-1));
}
