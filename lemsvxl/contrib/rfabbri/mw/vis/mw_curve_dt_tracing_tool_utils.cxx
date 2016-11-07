#include "mw_curve_dt_tracing_tool_common.h"
#include "mw_curve_dt_tracing_tool_utils.h"

#include <dbsol/algo/dbsol_geno.h>
#include <dbgl/algo/dbgl_curve_smoothing.h>
#include <dbdet/sel/dbdet_edgel.h>

vcl_string
mw_curve_dt_tracing_tool::name() const
{
  return "Dist. Tr. Curve Tracing";
}

bool 
mw_curve_dt_tracing_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  curve_tableau_current_.vertical_cast(tableau);
  if( curve_tableau_current_ == 0 )  {
    vcl_cerr << "Warning: working in a tableau which is not expected\n";
    return false;
  }
  return true;
}


void mw_curve_dt_tracing_tool::
add_all_episegs_to_tool(const vcl_vector<vcl_vector< vsol_polyline_2d_sptr > > &broken_vsols) const
{
  for (unsigned v=0; v < broken_vsols.size(); ++v)
    add_episegs_to_tool(broken[v], frame_v_[v]);
}

void mw_curve_dt_tracing_tool::
break_curves_into_episegs()
{
  vcl_vector<vcl_vector< vsol_polyline_2d_sptr > > broken_vsols;

  break_curves_into_episegs_pairwise(&broken_vsols);
  add_all_episegs_to_tool(broken_vsols);

  vcl_cout << " Done breaking !! " << vcl_endl;
}



void
add_episegs_to_tool(const vcl_vector< vsol_polyline_2d_sptr >  &vsols, unsigned frame)
{
  //----------------------------------------------------------------------
  // C) Create vsol storage; add to repository; make it active

  vcl_vector<vsol_spatial_object_2d_sptr> base_vsols2;
  base_vsols2.resize(vsols.size());
  for (unsigned i=0; i< vsols.size(); ++i)
    base_vsols2[i] = vsols[i].ptr();

  {
  // create the output storage class
  vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
  output_vsol->add_objects(base_vsols2, vcl_string("broken_vsols"));
  output_vsol->set_name("broken_vsols");
  MANAGER->repository()->store_data_at(output_vsol, frame);
  MANAGER->add_to_display(output_vsol);
  MANAGER->display_current_frame();
  }

  // make it active
  vcl_vector< bvis1_view_tableau_sptr > views;
  views = MANAGER->get_views();
  views[view]->selector()->set_active("broken_vsols");
}
