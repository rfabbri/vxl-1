#include "mw_curve_appearance_tracing_tool_common.h"
#include "mw_curve_appearance_tracing_tool_utils.h"

#include <dbecl/dbecl_episeg_sptr.h>
#include <dbecl/dbecl_episeg.h>
#include <dbecl/dbecl_epipole.h>
#include <dbecl/dbecl_episeg_from_curve_converter.h>

vcl_string
mw_curve_appearance_tracing_tool::name() const
{
  return "HOG Curve Tracing";
}

bool 
mw_curve_appearance_tracing_tool::set_tableau( const vgui_tableau_sptr& tableau )
{

  curve_tableau_current_.vertical_cast(tableau);
  if( curve_tableau_current_ == 0 )  {
    vcl_cerr << "Warning: working in a tableau which is not expected\n";
    return false;
  }

  return true;
}

//: Break curves into epipolar tangent segments.
void mw_curve_appearance_tracing_tool::
break_curves_into_episegs()
{
  //
  // FIXME Should have trinocular way of breaking these curves.
  //
  vgl_homg_point_2d<double> e, e_prime;
  fm_[0][1].get_epipoles(e,e_prime);
  break_curves_into_episegs(vsols_[0], e, frame_v_[0], 0 /*view*/);
  break_curves_into_episegs(vsols_[1], e_prime, frame_v_[1], 1 /*view*/);

  for (unsigned v=2; v < nviews_; v++) {
    fm_[0][v].get_epipoles(e,e_prime);
    break_curves_into_episegs(vsols_[v], e_prime, frame_v_[v], v /*view*/);
  }

  vcl_cout <<" Done breaking !! " << vcl_endl;
}


void mw_curve_appearance_tracing_tool::
break_curves_into_episegs(
  const vcl_vector< vsol_polyline_2d_sptr >  &vsols,
  const vgl_homg_point_2d<double> &e, int frame, int view) const
{
  static const double threshold_number_of_points=3;
  // ----------------------------------------------------------------------
  // Break curve

  dbecl_epipole_sptr epipole = new dbecl_epipole(e.x()/e.w(), e.y()/e.w());
  dbecl_episeg_from_curve_converter factory(epipole);

  // A) For each vsol, do:
  
  vcl_vector<dbecl_episeg_sptr> all_episegs;
  for (unsigned i=0; i < vsols.size(); ++i) {
    
    // A1 - convert to digital curve
    vsol_digital_curve_2d_sptr dc = new vsol_digital_curve_2d;
    for (unsigned k=0; k < vsols[i]->size(); ++k)
      dc->add_vertex(vsols[i]->vertex(k));

    // A2 - apply episeg
    vcl_vector<dbecl_episeg_sptr> eps = factory.convert_curve(dc);

    for(vcl_vector<dbecl_episeg_sptr>::iterator itr = eps.begin(); 
        itr != eps.end();  ++itr)
      all_episegs.push_back(*itr);
  }

  // B) Extract vsol from episegs

  vcl_vector<vsol_polyline_2d_sptr> vsols2;
  vsols2.reserve(all_episegs.size());

  for(unsigned k=0; k < all_episegs.size(); ++k) {

    if ( all_episegs[k]->max_index() - all_episegs[k]->min_index()  < threshold_number_of_points)
      continue;

    vsols2.push_back(new vsol_polyline_2d);
    // B1 - traverse episegs
    for (unsigned i=(unsigned ) vcl_ceil(all_episegs[k]->min_index()); i <= (unsigned ) vcl_floor(all_episegs[k]->max_index()); ++i) {
      vsols2.back()->add_vertex( all_episegs[k]->curve()->point(i) );
    }
  }
  

  //----------------------------------------------------------------------
  // C) Create vsol storage; add to repository; make it active

  vcl_vector<vsol_spatial_object_2d_sptr> base_vsols2;
  base_vsols2.resize(vsols2.size());
  for (unsigned i=0; i< vsols2.size(); ++i)
    base_vsols2[i] = vsols2[i].ptr();

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

