#include "mw_sel_inliers_to_curve.h"

#include <vcl_limits.h>

#include <vgui/vgui.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_style.h>
#include <bgui/bgui_vsol_soview2D.h>

#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>

#include <bpro1/bpro1_storage_sptr.h>
#include <vidpro1/vidpro1_repository.h>
#define MANAGER bvis1_manager::instance()

#include <dbdet/algo/dbdet_sel.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <dbcsi/dbcsi_curve_distance.h>
#include <mw/mw_util.h>



mw_sel_inliers_to_curve::
mw_sel_inliers_to_curve() :
  current_curve_id_(0),
  tau_distance_(9.0), //< squared.
  tau_dtheta_(10.0) //< degrees
{
}

vcl_string mw_sel_inliers_to_curve::
name() const
{
  return "SEL Curve Inliers";
}

void   
mw_sel_inliers_to_curve::
activate ()
{
  dbdet_sel_explorer_tool::activate();

  // tool only works in 1 view
  assert (nviews_ == 1);

  // Get curves
  { 
    vidpro1_vsol2D_storage_sptr sto=NULL;
    sto.vertical_cast(MANAGER->repository()->get_data_at("vsol2D",frame_v_[0]));
    if (sto == NULL) {
      vcl_cerr << "Tool error: Could not find a vsol.\n";
      return;
    }

    vcl_vector< vsol_spatial_object_2d_sptr > base = sto->all_data ();

    if (!base.size()) {
      vcl_cerr << "Tool error: Could not find a vsol.\n";
      return;
    }

    vcl_cout << "Number of vsols in storage named " << sto->name() <<  " in frame " << frame_v_[0]
      << ": " << base.size() << vcl_endl;

    curves_.resize(base.size());

    for (unsigned i=0; i < base.size(); ++i) {
      // Only take 1 polyline from the view.
      vsol_polyline_2d_sptr cptr = dynamic_cast<vsol_polyline_2d *> (base[i].ptr());

      if (!cptr) {
        vcl_cerr << "Non-polyline found; but only POLYLINES supported!" << vcl_endl;
        return;
      }
      mw_util::get_vsol_point_vector(*cptr, &(curves_[i]));

      vcl_cout << "Polyline size: " << curves_[i].size() << vcl_endl;
    }
  }

  // for now, automatically select 1st curve to inspect (default in the constructor).
  // \todo update this curve by clicking/selecting a curve


  // Get DT and label images.

  {
    bpro1_storage_sptr 
      p = MANAGER->repository()->get_data_by_name_at("EDT Image", frame_v_[0]);

    {
    vidpro1_image_storage_sptr dt_storage;
    dt_storage.vertical_cast(p);

    if(!dt_storage) {
      vcl_cerr << "Error: tool requires distance map image storage (from Distance Transform process) " << vcl_endl;
      return;
    }

    vil_image_view_base_sptr bview = dt_storage->get_image()->get_view();
    assert(bview);

    dt_ = vil_image_view<vxl_uint_32>(bview);
    assert(dt_);

    vcl_cout << "DT: " << dt_ << vcl_endl;
    }

    // Get label image
    p = MANAGER->repository()->get_data_by_name_at("Label Image", frame_v_[0]);

    {
    vidpro1_image_storage_sptr label_storage;
    label_storage.vertical_cast(p);

    if(!label_storage) {
      vcl_cerr << "Error: tool requires Label image storage (from Distance Transform process)" << vcl_endl;
      return;
    }

    vil_image_view_base_sptr bview = label_storage->get_image()->get_view();
    assert(bview);

    label_ = vil_image_view<unsigned>(bview);
    assert(label_);

    vcl_cout << "Label: " << label_ << vcl_endl;
    }
  }
}

void mw_sel_inliers_to_curve::
deactivate ()
{
  vcl_cout << "mw_sel_inliers_to_curve OFF\n";
  dbdet_sel_explorer_tool::deactivate();
}

bool mw_sel_inliers_to_curve::
handle( const vgui_event & e, 
        const bvis1_view_tableau_sptr& view )
{
  bool stat = dbdet_sel_explorer_tool::handle(e,view);

  if( e.type == vgui_OVERLAY_DRAW ) {
    // If curve is selected, draw the curvelets within a distance threshold of it.

    curvelet_list inlier_curvelets;

    unsigned num_inliers = dbcsi_curve_distance::inlier_curvelets_dt(
        curves_[current_curve_id_], tau_distance_, dt_, label_, 
        sel_storage_->CM(), &inlier_curvelets);

    vcl_cout << "Number of inlier curvelets: " << num_inliers << vcl_endl;

    draw_curvelets(inlier_curvelets);
  }

  return stat;
}

void mw_sel_inliers_to_curve::
draw_curvelets(const curvelet_list &curvelets)
{
  for ( 
    curvelet_list_const_iter cv_it = curvelets.begin();
    cv_it != curvelets.end(); 
    cv_it++) {

    const dbdet_curvelet *cvlet = *cv_it;

    if (cvlet->order() > min_order_to_display_)
      draw_grouping(cvlet);
  }
}


void mw_sel_inliers_to_curve::
get_popup( const vgui_popup_params& /*params*/, vgui_menu &menu )
{
  menu.add( "tau_distance", 
            bvis1_tool_set_param, (void*)(&tau_distance_) );

  menu.add( "tau_dtheta (deg)", 
            bvis1_tool_set_param, (void*)(&tau_dtheta_) );
}
