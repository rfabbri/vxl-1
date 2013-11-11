/**************************************************************/
/*  Name: MingChing (mcchang)
//  File: dbgui_curvematch_tableau.h
//  Asgn: bvis
//  Date: Thu Aug 28 14:28:55 EDT 2003
***************************************************************/

#ifndef dbcvr_curvematch_tableau_h_
#define dbcvr_curvematch_tableau_h_

#include <vgui/vgui_tableau.h>
#include <dbcvr/dbcvr_cvmatch.h>
#include <dbcvr/dbcvr_cvmatch_sptr.h>
#include <dbcvr/dbcvr_cv_cor_sptr.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_utils.h>
#include <vcl_iostream.h>
#include <bvis1/bvis1_tool_sptr.h>
#include <bvis1/bvis1_tool.h>
#include <vgl/vgl_distance.h>
#include <vbl/vbl_array_2d.h>

#include <dbcvr/vis/dbcvr_curvematch_tableau_sptr.h>

class dbcvr_curvematch_tableau : public vgui_tableau {
private:
  GLenum gl_mode;
protected:
  dbcvr_cvmatch_sptr curvematch_;
  dbcvr_cv_cor_sptr cv_cor_;
  vcl_vector<vsol_point_2d_sptr> curve1_pts_, curve2_pts_;
  bvis1_tool_sptr active_tool_sptr;

  vbl_array_2d<int> ColorArray_;

  int interval_;  // number of samples to pass before drawing correspondence
  bool draw_matching_lines_;  
  int size_;  // size of matched points
  int dummy_size_;
  int offsetx2_, offsety2_;

public:
  
  dbcvr_curvematch_tableau();
  virtual ~dbcvr_curvematch_tableau() {}
  
  virtual bool handle( const vgui_event & );
  
  void set_curvematch (dbcvr_cvmatch_sptr new_curvematch) {
    curvematch_ = new_curvematch;
    size_ = (curvematch_->finalMap())->size();
    vcl_vector<double>* costs = curvematch_->finalMapCost();
    vcl_cout << "printing costs along the alignment curve: \n";
    for (int i = costs->size()-1; i >= 0; i--) {
      vcl_cout << (*costs)[i] << " ";
    }
    vcl_cout << "\n";
  }

  void set_curve_cor (dbcvr_cv_cor_sptr new_curve_cor);
  
  void draw_select();
  void draw_render();
  
  virtual void set_active_tool (bvis1_tool_sptr tool) {
    active_tool_sptr = tool;
  }

  //: Generate the popup menu for this tableau
  void get_popup(const vgui_popup_params& params, vgui_menu &menu);

protected:
  void DrawPointMatching(vcl_vector<vgl_point_2d<double> >& pts1, vcl_vector<vgl_point_2d<double> >& pts2, 
                        double OffsetX1, double OffsetY1, double OffsetX2, double OffsetY2, bool show_matching_lines);
  void DrawCurveMatching (bsol_intrinsic_curve_2d_sptr curve1, 
                bsol_intrinsic_curve_2d_sptr curve2, FinalMapType* fmap, 
                double OffsetX1, double OffsetY1, double OffsetX2, double OffsetY2,
                bool bShowMatchingLines);

  void DrawCurves(vcl_vector<vsol_point_2d_sptr>& curve1_pts, 
                  vcl_vector<vsol_point_2d_sptr>& curve2_pts,
                  double OffsetX1, double OffsetY1, double OffsetX2, double OffsetY2, bool open_cv_match);

};

//: Create a smart-pointer to a dbcvr_curvematch_tableau.
struct dbcvr_curvematch_tableau_new : public dbcvr_curvematch_tableau_sptr
{
  typedef dbcvr_curvematch_tableau_sptr base;

  //: Constructor - creates a pointer to a dbcvr_curvematch_tableau.
  dbcvr_curvematch_tableau_new() : base(new dbcvr_curvematch_tableau()) { }
};

#endif
