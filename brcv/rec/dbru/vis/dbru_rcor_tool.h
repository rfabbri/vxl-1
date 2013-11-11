// This is brcv/rec/dbru/vis/dbru_rcor_tool.h
#ifndef dbru_rcor_tool_h_
#define dbru_rcor_tool_h_
//:
// \file
// \brief Region Correspondence Query tool
//      
// \author Ozge C Ozcanli Ozbay, (ozge@lems.brown.edu)
// \date 6/24/05
//
// \verbatim
//   Modifications
//
//   Amir Tamrakar 12/04/05    Modified this tool to work with precomputed
//                             region correspondence information instead of
//                             recomputing it in the tool.
//      
// \endverbatim

#include <bvis1/bvis1_tool.h>
#include <vgui/vgui_event_condition.h>
#include <vgl/vgl_point_2d.h>

#include <vbl/vbl_array_2d.h>

#include <dbru/vis/dbru_rcor_tableau_sptr.h>
#include <dbru/vis/dbru_rcor_tableau.h>
#include <dbru/pro/dbru_rcor_storage_sptr.h>
#include <dbru/pro/dbru_rcor_storage.h>

#include <dbsol/dbsol_interp_curve_2d_sptr.h>
#include <dbcvr/dbcvr_cv_cor_sptr.h>

//: A tool to query the region correspondence bwtween two observations
class dbru_rcor_tool : public bvis1_tool
{
public:

  dbru_rcor_tool();

  //: This is called when the tool is activated
  virtual void activate();

  //: Set the tableau to work with
  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );

  //: Set the storage class for the active tableau
  virtual bool set_storage ( const bpro1_storage_sptr& storage);

  //: Return the name of this tool
  virtual vcl_string name() const { return "Region Correspondence Tool"; }
  
  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );
  
protected:
  dbru_rcor_tableau_sptr tableau_;
  dbru_rcor_storage_sptr storage_;

  bvis1_view_tableau_sptr view0_;
  bvis1_view_tableau_sptr view1_;

  bool activation_ok_;
  
  vgui_event_condition gesture_click;
  vgui_event_condition gesture_print_histogram;  // only usable after line intersections algorithm
  
  //: currently displayed corresponding points
  vgl_point_2d<int> current_pt0_;
  vgl_point_2d<int> current_pt1_;

  // currently displayed nearest points on the contour
  // and its correspondence on curve2 coming from sillhouette correspondence
  vgl_point_2d<float> current_ct0_;
  vgl_point_2d<float> current_ct1_;

  //: Once clicked, the current point under the mouse is held as a fixed point
  vcl_vector<vgl_point_2d<int> > fixed_set0_;
  vcl_vector<vgl_point_2d<int> > fixed_set1_;
  vbl_array_2d<float> colors_;
  
  // fix the rcor at initialization from active storage
  dbru_rcor_sptr rcor_;
  dbsol_interp_curve_2d_sptr curve11_;
  dbsol_interp_curve_2d_sptr curve22_;
  dbcvr_cv_cor_sptr sil_cor_;

};

#endif // dbru_rcor_tool_h_
