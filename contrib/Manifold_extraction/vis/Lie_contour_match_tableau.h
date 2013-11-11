/**************************************************************/
/*  Name: pradeep yarlagadda
//  File: Lie_contour_match_tableau.h
//  Date: Tues 12 June 2007
***************************************************************/

#ifndef Lie_contour_match_tableau_h_
#define Lie_contour_match_tableau_h_

#include <vgui/vgui_tableau.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_utils.h>
#include <vcl_iostream.h>
#include <bvis1/bvis1_tool_sptr.h>
#include <bvis1/bvis1_tool.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_2d.h>
#include <vbl/vbl_array_2d.h>
#include <manifold_extraction\Lie_contour_match.h>
#include <manifold_extraction\Lie_contour_match_sptr.h>
#include <manifold_extraction\Lie_cv_cor_sptr.h>
#include <manifold_extraction\Lie_cv_cor.h>
#include <dbsol/dbsol_interp_curve_2d.h>
#include <dbsol/dbsol_interp_curve_2d_sptr.h>
#include "Lie_contour_match_tableau_sptr.h"

class Lie_contour_match_tableau : public vgui_tableau {
private:
  GLenum gl_mode;
protected:
  Lie_contour_match_sptr curvematch_;
  Lie_cv_cor_sptr cv_cor_;
  int min_idx_;
  bvis1_tool_sptr active_tool_sptr;

  vbl_array_2d<int> ColorArray_;

  int interval_;  // number of samples to pass before drawing correspondence
  bool draw_matching_lines_;  
  int size_;  // size of matched points
  int dummy_size_;
  int offsetx2_, offsety2_;

public:
  
  Lie_contour_match_tableau();
  virtual ~Lie_contour_match_tableau() {}
  
  virtual bool handle( const vgui_event & );
  
  void set_curvematch (Lie_contour_match_sptr new_curvematch) {
    curvematch_ = new_curvematch;
   
    vcl_vector<double> cost_vec = curvematch_->finalCost();

   // curvematch_->curve1()->print(vcl_cout);

    double min_cost = 1e20;
        int min_idx = -1; // infinity

        for (unsigned int i = 0;i<cost_vec.size();i++)
            {
            if (min_cost > cost_vec[i])
                {
                min_idx = i;
                min_cost = cost_vec[i];
                }
            }
   
    size_ = (curvematch_->finalMap(min_idx)).size();
    min_idx_ = min_idx;
  }

  void set_curve_cor (Lie_cv_cor_sptr new_curve_cor) {
       cv_cor_ = new_curve_cor;
    size_ = (cv_cor_->get_contour_pts1()).size();
      }
  
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
  void DrawCurveMatching (dbsol_interp_curve_2d_sptr curve1, 
                dbsol_interp_curve_2d_sptr curve2, FinalMapType* fmap, 
                double OffsetX1, double OffsetY1, double OffsetX2, double OffsetY2,
                bool bShowMatchingLines);

};

//: Create a smart-pointer to a Lie_contour_match_tableau.
struct Lie_contour_match_tableau_new : public Lie_contour_match_tableau_sptr
{
  typedef Lie_contour_match_tableau_sptr base;

  //: Constructor - creates a pointer to a Lie_contour_match_tableau.
  Lie_contour_match_tableau_new() : base(new Lie_contour_match_tableau()) { }
};

#endif

