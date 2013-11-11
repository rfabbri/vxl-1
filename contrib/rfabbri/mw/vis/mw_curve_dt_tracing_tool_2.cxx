#include "mw_curve_dt_tracing_tool_2.h"
#include "mw_curve_tracing_tool_common_2.h"

#include <vil/vil_image_resource.h> 
#include <vsol/vsol_line_2d.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>
#include <dbdet/pro/dbdet_sel_storage.h>

#include <mw/mw_discrete_corresp.h>
#include <mw/algo/mw_algo_util.h>
#include <mw/algo/mw_dt_curve_stereo.h>
#include <mw/algo/mw_odt_curve_stereo.h>


mw_curve_dt_tracing_tool_2::
mw_curve_dt_tracing_tool_2()
  :
    mw_curve_tracing_tool_2(new mw_odt_curve_stereo),
    tau_distance_(10.0),
    tau_dtheta_(10.0), //< degrees
    tau_min_inliers_per_view_(20.0),
    tau_min_total_inliers_(5.0),
    tau_min_epiangle_(30.0) //< degrees
{
  s_dt_ = static_cast<mw_odt_curve_stereo *> (s_);
  init();
}

void mw_curve_dt_tracing_tool_2::
init()
{
  best_match_soview_ = NULL;
}

vcl_string
mw_curve_dt_tracing_tool_2::name() const
{
  return "Dist. Tr. Curve Tracing v.2";
}

void mw_curve_dt_tracing_tool_2::
activate()
{
  vcl_cout << "mw_curve_dt_tracing_tool_2 ON\n";
  mw_curve_tracing_tool_2::activate();
  get_images();
  get_edgemaps();
  get_tangents();
}

void mw_curve_dt_tracing_tool_2::
deactivate ()
{
  mw_curve_tracing_tool_2::deactivate();
  vcl_cout << "mw_curve_dt_tracing_tool_2 OFF\n";
}

void mw_curve_dt_tracing_tool_2::
get_images()
{
  vcl_vector<vil_image_view<vxl_uint_32> > dts(nviews());
  vcl_vector<vil_image_view<unsigned> > labels(nviews());

  for (unsigned i=0; i<nviews(); ++i) {
    bpro1_storage_sptr
      p = MANAGER->repository()->get_data_by_name_at("EDT Image", frame_v_[i]);

    {
    vidpro1_image_storage_sptr dt_storage;
    dt_storage.vertical_cast(p);

    if(!dt_storage) { 
      vcl_cerr << "Error: tool requires distance map image storage " <<
       "(from Distance Transform process) in all frames" << vcl_endl;
      return;
    }

    vil_image_view_base_sptr bview = dt_storage->get_image()->get_view();
    assert(bview);

    dts[i] = vil_image_view<vxl_uint_32>(bview);
    assert(dts[i]);

    vcl_cout << "DT: " << dts[i] << vcl_endl;
    }

    // Get label image
    p = MANAGER->repository()->get_data_by_name_at("Label Image", frame_v_[i]);

    {
    vidpro1_image_storage_sptr label_storage;
    label_storage.vertical_cast(p);

    if(!label_storage) {
      vcl_cerr << "Error: tool requires Label image storage "
        << "(from Distance Transform process) in all frames" << vcl_endl;
      return;
    }

    vil_image_view_base_sptr bview = label_storage->get_image()->get_view();
    assert(bview);

    labels[i] = vil_image_view<unsigned>(bview);
    assert(labels[i]);

    vcl_cout << "Label: " << labels[i] << vcl_endl;
    }
  }

  s_dt_->set_all_dt_label(dts, labels);
}

void mw_curve_dt_tracing_tool_2::
get_edgemaps()
{
  vcl_vector<dbdet_edgemap_sptr> em;
  
  em.reserve(nviews());

  for (unsigned i=0; i < nviews(); ++i) {

    vcl_vector<vcl_string> edgemap_storages 
      = MANAGER->repository()->get_all_storage_class_names("edge_map", 
          frame_v_[i]);

    if (!edgemap_storages.size()) {
      vcl_cerr << "Tool did not find an edgemap storage in view[" << i << 
        "],\n which is ok if you don't need orientation-based matching.\n";
      return;
    }

    vcl_cout << "Reading edgemap storage " << edgemap_storages.back()
      << " from view[" << i << "].\n";

    bpro1_storage_sptr p 
      = MANAGER->repository()->get_data_by_name_at(edgemap_storages.back(), 
          frame_v_[i]);

    dbdet_edgemap_storage_sptr edgemap_storage = NULL;
    edgemap_storage.vertical_cast(p);
    if(!edgemap_storage->get_edgemap()) {
      vcl_cerr << "The edgemap storage in view[" << i << "] is NULL (Ok as long\n"
        "as this is not used." << vcl_endl;
      return;
    }
    em.push_back(edgemap_storage->get_edgemap());
  }
  s_dt_->set_all_edgemaps(em);
}

void mw_curve_dt_tracing_tool_2::
get_tangents()
{
  vcl_vector<vcl_vector<vcl_vector<double> > > tangents(nviews());

  if (sels_.size() < 2) {
    vcl_cerr << 
      "Warning: no SEL found in views[0,1], which is ok if you don't use\n" <<
      "this information.\n";

    vcl_cout << "Computing tangents by interpolating vsol.\n";

    // Set the tangents from interpolating vsol

    for (unsigned v=0; v < nviews(); ++v) {
      tangents[v].resize(s_->num_curves(v));
      for (unsigned c=0; c < s_->num_curves(v); ++c) {
        dbdet_edgel_chain ec;
        mw_algo_util::extract_edgel_chain(*(s_->curves(v,c)), &ec);

        tangents[v][c].resize(ec.edgels.size());
        for (unsigned i=0; i < ec.edgels.size(); ++i) {
          tangents[v][c][i] = ec.edgels[i]->tangent;
          delete ec.edgels[i];
        }
      }
    }
  } else {
    // TODO we need to deal with v >= 2 and interpolate from vsol if we don't
    // want to have SEL in all views. This is because, when we call
    // break_curves_into_episegs, all views are broken, and if we only have
    // tangents for the first two, this will cause undefined behavior.
    for (unsigned v=0; v < 2; ++v) {
      if (!sels_[v]) {
        vcl_cerr << 
          "Warning: no SEL found in view[v], which is ok if you don't use\n" <<
          "this information.\n";
        return;
      }
      // Read-off tangents from SEL
      dbdet_edgel_chain_list &frags = sels_[v]->CFG().frags;
      tangents[v].reserve(frags.size());
      assert(frags.size() == s_->num_curves(v));

      unsigned c=0;
      for (dbdet_edgel_chain_list_iter f_it = frags.begin(); 
          f_it != frags.end(); 
          ++f_it, ++c) {
        const dbdet_edgel_list &edgels = (*f_it)->edgels;
        tangents[v][c].resize(edgels.size());
        for (unsigned i=0; i < edgels.size(); ++i)
          tangents[v][c][i] = edgels[i]->tangent;
      }
    }
  }
  
  // now call set_tangents
  s_dt_->set_tangents(tangents);
}


bool mw_curve_dt_tracing_tool_2::
match_using_dt(stereo_matcher_type matcher)
{
  //: index into s().crv_candidates_ptrs of the top match.
  unsigned i_best;
  bool has_a_match=true;

  s_dt_->set_tau_distance_squared(tau_distance_*tau_distance_);
  s_dt_->set_dtheta_threshold(tau_dtheta_*vnl_math::pi/180.0);
  s_dt_->set_min_inliers_per_view(static_cast<unsigned>(tau_min_inliers_per_view_));
  s_dt_->set_min_total_inliers(static_cast<unsigned>(tau_min_total_inliers_));

  switch (matcher) {
    case MW_ORIENTED_DT: {
      vcl_vector<unsigned long> votes;
      if (!s_dt_->ready_for_oriented_matching() || 
          !s_dt_->match_using_orientation_dt(&i_best, &votes))
        return false;
      has_a_match = !votes.empty();
    }
    break;
    case MW_ORIENTED_DT_EXTRAS: {
      vcl_vector<unsigned long> votes;
      if (!s_dt_->ready_for_oriented_matching() || 
          !s_dt_->match_using_orientation_dt_extras(&i_best, &votes))
        return false;
      has_a_match = !votes.empty();
    }
    break; 
    default: {
      if (!s_dt_->match_using_dt(&i_best))
        return false;
    }
    break;
  }

  // Draw the result.
  if (best_match_soview_) {
    tab_[1]->remove(best_match_soview_);
    best_match_soview_ = NULL;
  }

  if (has_a_match) {
    best_match_soview_ = tab_[1]->add_vsol_polyline_2d(
          s_dt_->crv_candidates_ptrs(i_best), best_match_style_[0]);
  }
  tab_[1]->post_redraw();
  
  return true;
}

bool
mw_curve_dt_tracing_tool_2::
handle( const vgui_event & e, 
        const bvis1_view_tableau_sptr& view )
{
  if (e.type == vgui_KEY_PRESS) {
    switch (e.key) {
      case 'p': {
        vcl_cout << "Matching views using _oriented_ reprojection distance to edgels\n";
        vgui::out << "Matching views using _oriented_ reprojection distance to edgels\n";
        vcl_vector<dbdif_1st_order_curve_3d> crv3d;
        mw_discrete_corresp corresp;
        if (!dbmcs_match_and_reconstruct_all_curves(*s_dt_, &crv3d, &corresp)) {
          vcl_cerr << "Error: while matching all views.\n";
          return false;
        }

        bool retval = mywritev("dat/reconstr_curves_workdir/crv3d", ".dat", crv3d);
        if(!retval) {
          vcl_cerr << "Error while trying to write file.\n";
          abort();
        }

        // TODO: display all matches.
        // TODO: write to a correspondence structure.
        return true;
      }
     case '1': {
      s_dt_->set_min_epiangle(tau_min_epiangle_*vnl_math::pi/180.0);
      } break;
      default:
        break;
    }

    if (p0_) {
      switch (e.key) {
        case 'm': {
          vcl_cout << "Matching selected curve using reprojection distance to edgels\n";
          vgui::out << "Matching selectec curve using reprojection distance to edgels\n";
          if(!match_using_dt())
            vcl_cerr << "Error: Something went wrong during matching.\n";
          return true;
        }
        case 'o': {
          vcl_cout << "Matching selected curve using _oriented_ reprojection distance to edgels\n";
          vgui::out << "Matching selected curve using _oriented_ reprojection distance to edgels\n";
          if (!match_using_dt(MW_ORIENTED_DT))
            vcl_cerr << "Error: Something went wrong during matching.\n";

          return true;
        }
        case '6': {
          vcl_cout << "Matching selected curve using oriented reprojection and _extras_.\n";
          vgui::out << "Matching selected curve using _oriented_ reprojection and _extras_.\n";
          if (!match_using_dt(MW_ORIENTED_DT_EXTRAS))
            vcl_cerr << "Error: Something went wrong during matching.\n";
          return true;
        }
        case '5': {
          vcl_cout << "Displaying tgts\n";
          vgui::out << "Displaying tgts\n";
          display_selected_tangents();
          return true;
        }
        default:
          break;
      }
    }
  }

  return mw_curve_tracing_tool_2::handle(e, view);
}


void mw_curve_dt_tracing_tool_2::
display_selected_tangents()
{
  vcl_vector<vcl_vector<vsol_line_2d_sptr> > tgts; //:< tgts[iview][isample]
  bool retval  = compute_selected_tangents(&tgts);
  if (!retval) {
    vcl_cerr << "Warning: something wrong when asked to display tangents\n";
    return;
  }

  assert(tgts.size() == nviews());

  for (unsigned v=0; v < tgts_soview_.size(); ++v) {
    for (unsigned i=0; i < tgts_soview_[v].size(); ++i) {
      tab_[v]->remove(tgts_soview_[v][i]);
    }
  }

  tgts_soview_.resize(nviews());
  for (unsigned v=0; v < nviews(); ++v) {
    tab_[v]->set_current_grouping("SelectedTangents");
    tab_[v]->set_foreground(1,1,1);
    tgts_soview_[v].resize(tgts[v].size());
    for (unsigned i=0; i < tgts[v].size(); ++i) {
      tgts_soview_[v][i] = tab_[v]->add_vsol_line_2d(tgts[v][i]);
    }
    tab_[v]->post_redraw();
    tab_[v]->set_current_grouping( "default" );
  }
}

bool mw_curve_dt_tracing_tool_2::
compute_selected_tangents(vcl_vector<vcl_vector<vsol_line_2d_sptr> > *tgts_ptr) const
{
  if (!s_dt_->has_curve_tangents()) {
    vcl_cerr << "ERROR: we have no curve tangents\n";
    return false;
  }

  // tgts[iview][isample]
  vcl_vector<vcl_vector<vsol_line_2d_sptr> > &tgts = *tgts_ptr;

  tgts.resize(nviews());

  //: Do the first two views
  for (unsigned v=0; v < 2; ++v) {
    if (s_->selected_crv(v)) {
      tgts[v].resize(s_->selected_crv(v)->size());
      for (unsigned i=0; i < s_->selected_crv(v)->size(); ++i) {
        double dir = s_dt_->curve_tangents(v, s_->selected_crv_id(v), i);
        tgts[v][i] = new vsol_line_2d(vgl_vector_2d<double>(vcl_cos(dir)/2.0, 
          vcl_sin(dir)/2.0), s_->selected_crv(v)->vertex(i)->get_p());
      }
    }
  }

  //: Do the reprojections

  if (!s_dt_->has_reprojection_crv()) {
    vcl_cerr << "Warning: we have no reprojections\n";
  } else {
    //: Find id of the selected curve in view 2 amongst the candidate curves
    unsigned id_candidate;

    bool retval = 
    s_->get_index_of_candidate_curve(s_->selected_crv(1), &id_candidate);

    if (!retval) {
      vcl_cerr << "Warning: selected curve is not within candidates.\n";
      return false;
    }
   
    //: Add the tangents for the reprojections
    for (unsigned v=2; v < nviews(); ++v) {
      unsigned n = s_dt_->reprojection_crv_size(id_candidate, v);
      tgts[v].resize(n);
      for (unsigned i=0; i < n; ++i) {
        dbdet_edgel edg = s_dt_->reprojection_crv(id_candidate, v, i);
        tgts[v][i] = new vsol_line_2d(vgl_vector_2d<double>(vcl_cos(edg.tangent)/2.0, 
          vcl_sin(edg.tangent)/2.0), new vsol_point_2d(edg.pt));
      }
    }
  }

  return true;
}

void mw_curve_dt_tracing_tool_2::
get_popup( const vgui_popup_params& /*params*/, vgui_menu &menu )
{
  menu.add( "tau_distance", 
            bvis1_tool_set_param, (void*)(&tau_distance_) );

  menu.add( "tau_dtheta (deg)", 
            bvis1_tool_set_param, (void*)(&tau_dtheta_) );

  menu.add( "tau_min_inliers_per_view", 
            bvis1_tool_set_param, (void*)(&tau_min_inliers_per_view_) );

  menu.add( "tau_min_total_inliers", 
            bvis1_tool_set_param, (void*)(&tau_min_total_inliers_) );

  menu.add( "tau_min_epiangle (deg)", 
            bvis1_tool_set_param, (void*)(&tau_min_epiangle_) );
}
