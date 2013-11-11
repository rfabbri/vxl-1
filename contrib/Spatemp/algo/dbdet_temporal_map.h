// This is dbdet_temporal_map.h
#ifndef dbdet_temporal_map_h
#define dbdet_temporal_map_h
//:
//\file
//\brief Temporal Map data structure
//\author Vishal Jain
//\date 01/09/08
//
//\endverbatim

#include <vcl_vector.h>
#include <vcl_list.h>
#include <vcl_map.h>
#include <dbdet/sel/dbdet_edgel.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/sel/dbdet_curvelet_map.h>
#include <Spatemp/algo/dbdet_temporal_options_sptr.h>
#include <Spatemp/algo/dbdet_spatial_temporal_options_sptr.h>

#include <Spatemp/algo/dbdet_contour_fragment.h>
#include <Spatemp/algo/dbdet_temporal_map_params.h>
#include <Spatemp/algo/dbdet_region.h>

#include "dbdet_temporal_bundle.h"
#include <dbdet/sel/dbdet_edgel_link_graph.h>
#include <Spatemp/algo/dbdet_spatial_temporal_derivatives.h>

#include "dbdet_delaunay_map.h"

//: This class stores the map of curvelets formed by the SEL edge linker.
//  This is an intermediate structure before actual edge linking occurs.
class dbdet_temporal_map: public vbl_ref_count 
{
public:
    //: The curvelet map, indexed by edgel IDs
    // vcl_vector<vcl_map<dbdet_curvelet*, dbdet_temporal_options* >  >map_;

    vcl_vector<dbdet_spatial_temporal_options_sptr> map_;
    vcl_map<dbdet_edgel*,dbdet_spatial_temporal_derivatives*> derivatives_map_;

    vcl_map<int,dbdet_curvelet_map*> neighbor_cmap_;
    vcl_map<int,dbdet_edgemap_sptr>  neighbor_emap_;

    vcl_vector< vcl_vector<int> > valid_neighbor_map_;
    vcl_map<int,vcl_list<dbdet_3D_velocity_model> > motion_triangle_;

    vcl_vector<int> label_map_;

    dbdet_delaunay_map * dt_;

    dbdet_edgemap_sptr  emap_;
    //: store the link graph for current frame.

    dbdet_edgel_link_graph elg_;

    //: store the chain for each hypothesis.
    vcl_vector<dbdet_contour_fragment> cf_list;

    vcl_vector<dbdet_region> regions_;

    vcl_vector<dbdet_edgel_chain*> one_chains;

    vcl_map<int,int> edge_to_one_chain;

    vcl_map<int, vnl_vector<float> > v_modes_;

    //: beta from current frame edge to the next frame
    vcl_map<dbdet_edgel*, vcl_vector<float> > beta_edge_map;
    //: beta from previous frame edge to the current frame edge and is indexed by current frame edge
    vcl_map<dbdet_edgel*, vcl_vector<float> > backward_beta_edge_map;

    vcl_map<dbdet_edgel*, vcl_vector<vgl_vector_2d<double>> > backward_vector_edge_map;
      dbdet_temporal_map(dbdet_edgemap_sptr  emap);
      dbdet_temporal_map(dbdet_edgemap_sptr  emap, dbdet_temporal_map_params & params);
      dbdet_temporal_map(dbdet_curvelet_map  & cmap);
      dbdet_temporal_map(dbdet_curvelet_map  & cmap, dbdet_temporal_map_params & params);

  //: destructor
  ~dbdet_temporal_map()
  {
    clear(); //delete everything upon exit
  }


  //: set the linking parameters
  void set_parameters(dbdet_temporal_map_params &mparams)
  { 
  }
    bool extract_regions();

  bool form_bundles();
  bool compute_temporal_derivatives();
  bool compute_velocity_3D();
  bool bundles_intersect(dbdet_temporal_bundle & b1,dbdet_temporal_bundle & b2);
  bool optimize();

  //bool compute_RANSAC_V();
  bool compute_V(dbdet_temporal_normal_velocity_model * m1,dbdet_temporal_normal_velocity_model * m2 , float & theta, float & phi);
  bool compute_V(dbdet_temporal_normal_velocity_model * m1, dbdet_temporal_normal_velocity_model * m2,
  dbdet_temporal_normal_velocity_model * m3,float & theta, float & phi);
  bool compute_motion_for_triangles();
  //: compute distribution per visual fragment
  bool compute_distribution_per_region();
  bool compute_distribution_per_region_new();

  //bool pruning_neighbor_links();
  //; check if a curvelet passes thru a link in link graph
  bool pass_through_link(vcl_list<dbdet_curvelet*> clist,dbdet_curvelet * c );

  //: function to compute the weights of the hypthoses using neighborhood graph.
  //void relaxation();

  double compute_probabilistic_error_1(dbdet_region r, double Vx, double Vy, double Vz);

  void update_delaunay_edge_weights();

  vcl_vector<int> find_path(dbdet_curvelet * s);
  bool form_contour_groupings();
  bool form_spatial_bundles();
  void extract_one_chains_from_the_link_graph();

  //: function to check if any hypothesis in the two edges intersect.
  bool edges_intersect(int id1,int id2);

  //: function to dind  the temporal bundles which have one sided correspondence.
  bool find_onesided_temporal_bundles();

  double get_weight_across_neighbors(dbdet_temporal_bundle * b);

  double get_weight_along_neighbors(dbdet_temporal_bundle * b, float scale);
  double compute_geometric_error(dbdet_region r, double Vx, double Vy, double Vz, 
                                 vcl_vector<double> & xs, vcl_vector<double> & ys);
 double compute_geometric_error(dbdet_region & r, dbdet_spherical_histogram_sptr Vs);
double computer_error_per_region(dbdet_region & re, float vx, float vy, float vz,vcl_vector<float> &xs,vcl_vector<float> &ys);

  double compute_error_edge(double x, double y, dbdet_edgemap_sptr emap,double theta);
  bool compute_hough_transform();
  bool compute_motion_per_region(double Vx, double Vy, double Vz);
  bool confirm_3D_model(dbdet_3D_velocity_model & c, dbdet_temporal_bundle b,float & x1, float & y1);
  int left_or_right(dbdet_edgel * b, dbdet_edgel * a , dbdet_edgel * test);
vcl_vector<vcl_pair<int,int> >  find_modes(vnl_matrix<float> & m, int bandwidth);

  bool compute_model_in_a_region(dbdet_region r);

  bool compute_intial_motion();
  //: for computing beta ( normal velcotiy ) from an edge to a curvelet
  bool compute_forward_beta();
  //: for computing beta ( normal velcotiy ) from a curvelet  to an egde
  bool compute_backward_beta();
  bool compute_spatial_derivatives();
  bool compute_spatial_temporal_bundles();

double compute_geometric_error_for_family(dbdet_region r, vnl_vector<float> v1,vnl_vector<float> v2,vcl_vector<double> & xs, vcl_vector<double> & ys,
                                          float phi_min,float phi_max );
double compute_probabilistic_error(dbdet_region r, double Vx, double Vy, double Vz);
bool compute_initial_probability();

  //: resize the graph
  void resize(int size)
  { 
    if (size!=map_.size())
      clear();

    map_.resize(size);
  }

  //: clear the graph
  void clear()
  {
    //delete all the curvelets in the map

    map_.clear();
  }


  friend class dbdet_edge_map;

  dbdet_temporal_map_params params_;
};

#endif // dbdet_temporal_map_h
