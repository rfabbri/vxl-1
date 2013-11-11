// This is brcv/seg/dbdet/vis/dbdet_temporal_map_tableau.h
#ifndef dbdet_temporal_map_tableau_h_
#define dbdet_temporal_map_tableau_h_

//:
// \file

#include <vgui/vgui_gl.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_parent_child_link.h>
#include <vgui/vgui_event_condition.h>

#include "dbdet_temporal_map_tableau_sptr.h"
#include <Spatemp/algo/dbdet_temporal_map_sptr.h>
#include <Spatemp/algo/dbdet_temporal_model.h>
#include <Spatemp/algo/dbdet_temporal_bundle.h>
#include <Spatemp/pro/dbdet_temporal_map_storage_sptr.h>


#include <dbdet/sel/dbdet_edgel.h>
#include <dbdet/sel/dbdet_curvelet_map.h>

#include <dbdet/edge/dbdet_edgemap_sptr.h>

#include <vnl/vnl_vector_fixed.h>
class dbdet_temporal_map_V_bundle;
//: A tableau to display dbdet_sel_storage objects
class dbdet_temporal_map_tableau : public vgui_tableau 
{
public:
  friend class dbdet_temporal_map_V_bundle;
  //: Constructor
  dbdet_temporal_map_tableau(dbdet_temporal_map_storage_sptr tmap);
  virtual ~dbdet_temporal_map_tableau();

  //: handle the render and select events
  virtual bool handle( const vgui_event & );
    //supporting functions
  //dbdet_edgel* find_closest_edgel(float x, float y);
  //dbdet_link* find_closest_link(float x, float y);

  //: set the data structures 
  void set_data() 
  { }

  //: Generate the popup menu for this tableau
  void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  dbdet_edgel* find_closest_edgel(float ix, float iy);
  void draw_edges();
  void draw_neighbor_edges();

  void print_edgel_stats(dbdet_edgel * e);

  void print_cvlet_info(dbdet_curvelet* cvlet);

  void draw_current_curvlets(dbdet_edgel * e);
  void draw_curvelet(dbdet_curvelet* cvlet, float r, float g, float b);
  void draw_current_temporal_bundles(dbdet_edgel * e, int thresh);
  void draw_edgel(dbdet_edgel* e, float r, float g, float b);
  void draw_temporal_model(dbdet_temporal_model * m, dbdet_edgel * refc, double r, double g, double b );
  void draw_max_temporal_bundles(dbdet_edgel * e, int thresh);
  void show_potential_seeds();
  void draw_if_seed(dbdet_edgel * e);
  void draw_delaunay_valid();
  void draw_delaunay();
  void draw_layer();
  void construct_contour_fragment(dbdet_curvelet * refc, dbdet_temporal_bundle  * b);

  void draw_contour_fragments();
  //: to draw temporal bundles which have correspondence on one side of the time meaning -2, -1 and none from 1 and 2
  void draw_one_sided_bundles();

  void draw_current_longest_hypothesis(dbdet_edgel * e);
  void draw_current_across_and_along_hypothesis(dbdet_edgel * e);
  void list_hypothesis_by_nweight(dbdet_edgel * e);
void draw_neighbor_relations(dbdet_edgel *e);
int point_inside(float ix, float iy);
void print_velocity_per_triangle(float ix, float iy);
void draw_edges_agreed_with_model(dbdet_3D_velocity_model &c);
void draw_regions();
void get_region(float ix, float iy);
void display_salient_regions();
void draw_betas();
void draw_backward_betas();
void draw_all_neighbor_edges();
dbdet_edgel* compute_closes_edge(double x, double y, dbdet_edgemap_sptr emap);

private:
    dbdet_temporal_map_sptr tmap_;
    dbdet_edgemap_sptr EM_;           ///< The edgemap (EM)
    dbdet_curvelet_map CM_;   
    int frame_number_;
    int thresh_groupings_;
    bool draw_only_current_curvelets_;
    bool draw_only_current_temoral_bundles_;
    bool draw_only_max_temoral_bundles_;
    bool show_potential_seeds_;
    bool draw_if_seed_;
    bool draw_dynmaic_fragment_;
    bool draw_contour_fragment_;
    bool display_delaunay_valid_;
    bool show_one_sided_bundles_;
    bool draw_layers_;
    bool display_delaunay_;

    bool current_longest_hypothesis_;

    bool current_across_and_along_hypothesis_;

    bool list_hypothesis_by_nweight_;

    bool display_salient_regions_;
    bool neighbor_relations_;
    bool draw_regions_;
    bool show_model_per_triangle_;
    float kthresh_;
    float delaunay_thresh_;
    dbdet_edgel*     cur_edgel;
    float ix, iy;

    float Vx;
    float Vy;
    float V_z;

    bool compute_error_;

    bool draw_betas_;
    bool draw_backward_betas_;
    bool all_neighbor_frames_;


    bool display_V_bundle;
  dbdet_temporal_map_V_bundle * bundle;

  vgui_event_condition gesture0_, gesture1_;

};


//: Create a smart-pointer to a dbdet_temporal_map_tableau.
struct dbdet_temporal_map_tableau_new : public dbdet_temporal_map_tableau_sptr
{
  typedef dbdet_temporal_map_tableau_sptr base;

  //: Constructor - creates a pointer to a dbdet_temporal_map_tableau
  dbdet_temporal_map_tableau_new(dbdet_temporal_map_storage_sptr sel) : base(new dbdet_temporal_map_tableau(sel)) { }
};

#endif //dbdet_temporal_map_tableau_h_
