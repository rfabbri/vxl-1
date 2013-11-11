#include "mw_correspond_point_tool_band.h"

#include <vcl_stack.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <bgui/bgui_vsol_soview2D.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
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
#include <mw/mw_epi_interceptor.h>
#include <dbecl/dbecl_epiband.h>
#include <dbecl/dbecl_epiband_iterator.h>
#include <dbecl/dbecl_epiband_builder.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_box_2d.h>


static void 
get_vertices(
    dbecl_epiband &eb,
    vcl_vector<vsol_point_2d_sptr> &v1);



//---------------------------------------------------------------------------------------------------------------
mw_correspond_point_tool_band::
mw_correspond_point_tool_band()
  :
    err_pos_(2.0)
{


  covering_pt_style_ = vgui_style::new_style(0.7, 0.7, 1, 
      3/*ptsize*/, 1/*linesize*/);
  covering_pt_style_->rgba[3] = 0.4;

}

//---------------------------------------------------------------------------------------------------------------
vcl_string mw_correspond_point_tool_band::
name() const
{
  return "Multiview point correspond BAND";
}

//---------------------------------------------------------------------------------------------------------------
void   
mw_correspond_point_tool_band::
activate ()
{
  mw_correspond_point_tool_basic::activate();

  display_epiband_covering_= false;


  epband_style_.resize(nviews_);

  //: ep_style_[i] == style of epipolars defined by i-th selected point
  epband_style_[0] = vgui_style::new_style(0, 1, 0, 1/*ptsize*/, 2/*linesize*/); // Green
  epband_style_[0]->rgba[3] = 0.5;

  epband_style_[1] = vgui_style::new_style(1, 1, 0, 1/*ptsize*/, 2/*linesize*/); // Yellow
  epband_style_[1]->rgba[3] = 0.5;

  if (nviews_ > 2) {
    epband_style_[2] = vgui_style::new_style(0.6, 0.6, 1, 1/*ptsize*/, 2/*linesize*/); // Blue
    epband_style_[2]->rgba[3] = 0.5;
  }

  //: All others are random colors
  for (unsigned i=3; i < nviews_; ++i) {
    epband_style_[i] = vgui_style::new_style(1, 0.7, 0.7, 1/*ptsize*/, 2/*linesize*/); // Light red
    epband_style_[i]->rgba[3] = 0.5;
  }

  epband_.resize(nviews_);
  epband_soview_.resize(nviews_);
  for (unsigned i=0; i < nviews_; ++i) {
    epband_[i].resize(nviews_,0);
    epband_soview_[i].resize(nviews_,0);
  }

  covering_soviews_.resize(nviews_);

  bbox_.resize(nviews_);
  w_.resize(nviews_);
  for (unsigned iv=0; iv < nviews_; ++iv) {
    vsol_polyline_2d poly(vsols_[iv]);
    bbox_[iv] = poly.get_bounding_box();
    w_[iv] = new dbecl_grid_cover_window(vgl_box_2d<double>(bbox_[iv]->get_min_x(),bbox_[iv]->get_max_x(), bbox_[iv]->get_min_y(),bbox_[iv]->get_max_y()),0);
  }


  err_pos_   = 2.0;
  if (!get_params(&err_pos_)) {
    vcl_cerr << "Error: in getting params" << vcl_endl;
    return;
  }

  // cache fmatrices


  fm_.resize(nviews_);
  for (unsigned i=0; i < nviews_; ++i) {
    fm_[i].resize(nviews_);
    for (unsigned k=0; k < nviews_; ++k) {
      if (k == i)
        continue;
      fm_[i][k] = vpgl_fundamental_matrix <double> (cam_[i].Pr_,cam_[k].Pr_);
    }
  }
}


//---------------------------------------------------------------------------------------------------------------
void   
mw_correspond_point_tool_band::
deactivate ()
{
  vcl_cout << "mw_correspond_point_tool_band OFF\n";
  mw_correspond_point_tool_basic::deactivate ();

  fm_.clear();

  for (unsigned i=0; i < nviews_; ++i)
    delete w_[i];

  w_.clear();
}


//---------------------------------------------------------------------------------------------------------------
bool mw_correspond_point_tool_band::
handle( const vgui_event & e, 
    const bvis1_view_tableau_sptr& view )
{

  if (e.type == vgui_KEY_PRESS) {
    vcl_cout << "-------------\n";
    vcl_cout << "Frame index: " << view->frame() << vcl_endl;
    return handle_key(e.key);
  }


  if (gesture0_(e)) {
    if ( !handle_mouse_event_whatever_view(e,view) )
      return false;
  }

  //  We are not interested in other events,
  //  so pass event to base class:
  return false;
}

//---------------------------------------------------------------------------------------------------------------
bool mw_correspond_point_tool_band::
handle_key(vgui_key key)
{
  bool base_stat = mw_correspond_point_tool_basic::handle_key(key);

  switch (key) {


    case 'p': { 
                if (!any_selected_point_)
                  return false;

                // toggle if should display+compute all ep0..ep1 in image 3
                vcl_cout << "Toggling display epipolar band covering pixels\n";
                vgui::out << "Toggling display epipolar band covering pixels\n";

                display_epiband_covering_= !display_epiband_covering_;

                update_display_for_epipolar_bands(true);

                return true;
                break;
              }


    default:
              if (!base_stat)
                vcl_cout << "(corresp_tool_band) Unassigned key: " << key << " pressed.\n";
              return base_stat;
  }


  return true;
}


//---------------------------------------------------------------------------------------------------------------
bool mw_correspond_point_tool_band::
handle_mouse_event_whatever_view( 
    const vgui_event & e, 
    const bvis1_view_tableau_sptr& view )
{
  mw_correspond_point_tool_basic::handle_mouse_event_whatever_view(e,view);

  vsol_point_2d_sptr pt = selected_edgel_->middle();

  unsigned v = iv_frame_[view->frame()];

  vcl_vector<bool> is_specified(nviews_,false);
  vcl_vector<vsol_point_2d_sptr> selected_points(nviews_);

  for (unsigned iv=0; iv < nviews_; ++iv) {
    if (p_[iv]) {
      is_specified[iv] = true;
      selected_points[iv] = vsols_[iv][p_idx_[iv]];
    }
  }

  dbecl_epiband_builder::build_epibands_iteratively(
      true, v, is_specified, selected_points, epband_, bbox_, fm_, err_pos_);

  update_display_for_epipolar_bands(true);
  
  return true;
}


//---------------------------------------------------------------------------------------------------------------
//: \remarks No computation involved
// Currently only updates after event in p_[0]
void mw_correspond_point_tool_band::
update_display_for_epipolar_bands( bool redraw)
{
  for (unsigned i=0 ; i < nviews_; ++i) {
    tab_[i]->set_current_grouping( "layer99" );
  }


  for (unsigned v=0; v < nviews_; ++v) {

    vcl_list<vgui_soview2D_point *>::iterator it;
    for (it = covering_soviews_[v].begin(); it!= covering_soviews_[v].end(); ++it)
      tab_[v]->remove(*it);
    covering_soviews_[v].clear();


    for (unsigned i=0; i < nviews_; ++i) {
      if (v == i)
        continue;

      if (epband_soview_[v][i]) {
        tab_[i]->remove(epband_soview_[v][i]);
        epband_soview_[v][i] = 0;
      }

      if (epband_[v][i]) {
        vcl_vector<vsol_point_2d_sptr> v1;
        get_vertices(*(epband_[v][i]), v1);

        if (v1.size()) {
          vsol_polygon_2d_sptr poly = new vsol_polygon_2d(v1);
          epband_soview_[v][i] = tab_[i]->add_vsol_polygon_2d(poly, epband_style_[v]);
        }

//        epband_[v][i]->polygon().print(vcl_cout); vcl_cout << vcl_endl;
      }
    } /* end for i*/


    if (display_epiband_covering_) {
      assert (epband_[v][v] != 0);
      for (dbecl_epiband_iterator it(*(epband_[v][v]),*(w_[v])); it.nxt(); ) {
        covering_soviews_[v].push_back(tab_[v]->add_point(it.x(),it.y()));
        covering_soviews_[v].back()->set_style(covering_pt_style_);
      }
    }

  } /* end for v */


  if (redraw)
    for (unsigned i=0 ; i < nviews_; ++i)
      tab_[i]->post_redraw();
}



//---------------------------------------------------------------------------------------------------------------

//: Display dialog to get params
bool  mw_correspond_point_tool_band::
get_params(double *err_pos)
{
  vgui_dialog* d = new vgui_dialog("Correspond Tool: Basic Parameters");
//  d->message("USAGE: Choose initial view and second view then click on the initial.\nNote: do not change frames while using this tool. If you do, please restart the tool.");

//  epipolar_dialog->checkbox("Use image", iparams->use_given_image);

  mw_correspond_point_tool_band::add_params(d,err_pos);

  return d->ask();
}


//---------------------------------------------------------------------------------------------------------------
void mw_correspond_point_tool_band::
add_params(vgui_dialog *d, double *err_pos)
{
//  d->checkbox("Use epipolar band?", *use_band);

  d->field("Feature position uncertanty (pixels)", *err_pos);
}

//---------------------------------------------------------------------------------------------------------------
void 
get_vertices(
    dbecl_epiband &eb,
    vcl_vector<vsol_point_2d_sptr> &v)
{
  if (eb.polygon().num_sheets()) { 
    v.resize(eb.polygon()[0].size());
    for (unsigned p=0; p < eb.polygon()[0].size(); ++p) {
      v[p] = new vsol_point_2d(eb.polygon()[0][p]);
    }
  }
}
