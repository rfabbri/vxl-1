// This is brcv/seg/dbdet/vis/dbdet_sel_tableau.h
#ifndef dbdet_sel_tableau_h_
#define dbdet_sel_tableau_h_

//:
// \file
// \brief A tableau to display dbdet_sel_storage objects.
// \author Amir Tamrakar
// \date 03/28/06
//
// \verbatim
//  Modifications
//    Amir Tamrakar  10/17/06       Added the functionality of browsing the sel storage class
//                                  for edge groups etc.
//
// \endverbatim

#include <vgui/vgui_gl.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_parent_child_link.h>
#include <vgui/vgui_event_condition.h>

#include "dbdet_sel_tableau_sptr.h"
#include <dbdet/algo/dbdet_sel_sptr.h>

#include <dbdet/pro/dbdet_sel_storage_sptr.h>

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix.h>

//: A tableau to display dbdet_sel_storage objects
class dbdet_sel_tableau : public vgui_tableau 
{
public:

  //: Constructor
  dbdet_sel_tableau(dbdet_sel_storage_sptr sel);
  virtual ~dbdet_sel_tableau();

  //: handle the render and select events
  virtual bool handle( const vgui_event & );
    //supporting functions
    dbdet_edgel* find_closest_edgel(float x, float y);
    dbdet_link* find_closest_link(float x, float y);

  //: set the data structures 
  void set_data(dbdet_edgemap_sptr EM, 
                dbdet_curvelet_map& CM, 
                dbdet_edgel_link_graph& ELG, 
                dbdet_curve_fragment_graph& CFG, 
                vbl_array_2d<bool>& EULM,
                vcl_list<dbdet_edgel_chain_list>& c_groups) 
  { EM_=EM; CM_=CM; ELG_=ELG; CFG_=CFG; EULM_=EULM; c_groups_=c_groups; }

  //: return the instance of the edge linker stored here
  dbdet_sel_sptr edge_linker() { return edge_linker_; }

  //: Generate the popup menu for this tableau
  void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  // rendering funtions

  //: draw selected curvelet
  void draw_selected_cvlet(dbdet_curvelet* cvlet);

  //: render the curvelet groupings for the edgels
  void draw_edgel_groupings();

  //; draw little circles around the edges showing their uncertainty zone
  void draw_uncertainty_zone(dbdet_curvelet* cvlet, float r, float g, float b);

  //: draw a curvelet as a polyline
  void draw_curvelet(dbdet_curvelet* cvlet, float r, float g, float b);

  //: draw only the special links of the curvelet (i.e., the ones it is immediately connected to)
  void draw_curvelet_special(dbdet_curvelet* cvlet, float r, float g, float b);

  //: draw only the extra special links of the curvelet (i.e., if the edgel it is connected to is also connected to it)
  void draw_curvelet_extra_special(dbdet_curvelet* cvlet, float r, float g, float b);

  //: draw the best fit line of a curvelet
  void draw_line_fit(dbdet_curvelet* cvlet, float r, float g, float b, float line_width=1.0);
  
  //: draw the best fit CC of a curvelet
  void draw_CC_fit(dbdet_curvelet* cvlet, float r, float g, float b, float line_width=1.0);

  //: draw the best fit CC2 of a curvelet
  void draw_CC2_fit(dbdet_curvelet* cvlet, float r, float g, float b, float line_width=1.0, bool forward=true);

  //: draw a CC
  void draw_CC(double sx, double sy, double theta, double k, double Lm, double Lp,
               float r, float g, float b, float line_width, bool forward);

  //: draw the best fit CC3d of a curvelet
  void draw_CC3d_fit(dbdet_curvelet* cvlet, float r, float g, float b, float line_width, bool forward);

  //: draw the best fit ES of a curvelet
  void draw_ES_fit(dbdet_curvelet* cvlet, float r, float g, float b, float line_width=1.0);

  //: draw the edgel link graph
  void draw_edgel_link_graph();

  //: draw the edgel chains
  void draw_edgel_chains();

  //: draw the group of edgel chains, By Yuliang Nov 23 2010
  void draw_contour_groups();

  //: draw the prune contours in evaluation process
  void draw_prune_contours();

  //: output the grouping information for this edgel
  void print_edgel_stats(dbdet_edgel* e);

  //: output the curvelet info at this link
  void print_link_info(dbdet_link* link);

  //: ouput the curvelet info
  void print_cvlet_info(dbdet_curvelet* cvlet, dbdet_edgel* e, unsigned pos);

  //: set the curve fragment color
  void set_curve_color(float r, float g, float b)
  {
	curve_color_[0]=r; curve_color_[1]=g; curve_color_[2]=b;
  }

private:

  //data structures to be displayed
  dbdet_edgemap_sptr EM_;           ///< The edgemap (EM)
  dbdet_curvelet_map& CM_;          ///< The curvelet map (CM) 
  dbdet_edgel_link_graph& ELG_;     ///< The edge link graph (ELG)
  dbdet_curve_fragment_graph& CFG_; ///< The curve fragment graph (CFG)
  vbl_array_2d<bool>& EULM_;        ///< The edge Unlinked map (EULM)
  vcl_list<dbdet_edgel_chain_list>& c_groups_ ; ///< the contour groups from CFG, By Yuliang
  dbdet_edgel_chain_list prune_frags_; ///< curve fragments pruned in evaluation process
  //keep an instance of the sel linker for sel_commands
  dbdet_sel_sptr edge_linker_;

  //-----------------------------------------------------------------------------------------------
  //curvelet display flags
  bool display_points_;         ///< display the edgel centers
  bool display_groupings_;      ///< display the groupings with a polyline
  bool display_largest_;        ///< display only the largest order groupings for each edgel
  bool display_curve_;          ///< display the curve fits for each grouping
  bool display_curve_bundle_;   ///< display the whole bundle when displaying the fits

  bool display_groupings_special_; ///< only the closest links of each grouping
  bool display_extra_special_;     ///< only the closest reciprocal links of each grouping
  
  unsigned smallest_to_display_;   ///< smallest length of curvelet to display
  unsigned largest_to_display_;

  double min_quality_to_display_;  ///< minimum quality of curvelets to display
  bool only_display_used_curvelets_; ///< to visualize the importance of the curvelets for linking

  double app_threshold_;
  bool display_app_consistent_only_;

  bool display_multicolored_curvelets_;

  vnl_vector_fixed<float,3> cvlet_color_;
  float cvlet_line_width_;

  //-----------------------------------------------------------------------------------------------
  //link graph display flags
  bool display_link_graph_;     ///< display the link graph formed from the local grouping
  unsigned min_votes_to_display_;  ///< while displaying the link graph, display only those links that have votes larger than this
  bool draw_links_in_color_; ///< This mode shows the links in different colors as a funciton of their overlap strength
  bool color_by_votes_; ///< color the links either by votes or by degree of overlap

  vnl_vector_fixed<float,3> link_color_;
  float link_line_width_;

  //-----------------------------------------------------------------------------------------------
  //contour fragment display flags
  bool display_contours_;       ///< display the regular contours in the link graph
  unsigned smallest_curve_fragments_to_display_; ///< threshold on the length of contours

  bool display_multicolored_curves_; ///< display each image contour in a different color
  bool display_thick_curves_;        ///< display the linked curves with the same thickness as the dpos parameter
  
  bool display_CFG_end_points_;
  
  bool display_contour_groups_;// to show groups of fragments ,  By Yuliang Nov 23 2010

  bool display_prune_contours_;

  vnl_vector_fixed<float,3> curve_color_;
  float curve_line_width_;

  //-----------------------------------------------------------------------------------------------
  //these are for querying the sel object 
  vgui_event_condition gesture0_, gesture1_;
  bool draw_anchored_only_;

  //-----------------------------------------------------------------------------------------------
  dbdet_edgel* cur_edgel; ///< currently selected edgel
  dbdet_link* cur_link;  ///< currently selected link

  //float col_pal[10][3]; ///< random colormap

  /*const float col_pal[10][3] = {  {1.0000 ,      0.0  ,0.4000},
                            {1.0000 ,      0  ,0.8000}
                            {0.8000 ,      0  ,1.0000}
                            {0.4000 ,      0  ,1.0000}
                            {     0 ,      0  ,1.0000}
                            {     0 , 0.4000  ,1.0000}
                            {     0 , 0.8000  ,1.0000}
                            {     0 , 1.0000  ,0.8000}
                            {     0 , 1.0000  ,0.4000}
                            {     0 , 1.0000  ,     0}};*/


  //For scaling the local tableau
  int local_zoom_factor;

  vnl_matrix<double> color_mat_;
};


//: Create a smart-pointer to a dbdet_sel_tableau.
struct dbdet_sel_tableau_new : public dbdet_sel_tableau_sptr
{
  typedef dbdet_sel_tableau_sptr base;

  //: Constructor - creates a pointer to a dbdet_sel_tableau
  dbdet_sel_tableau_new(dbdet_sel_storage_sptr sel) : base(new dbdet_sel_tableau(sel)) { }
};

#endif //dbdet_sel_tableau_h_
