// This is brcv/shp/dbsk2d/algo/dbsk2d_shock_transforms.h
#ifndef dbsk2d_shock_transforms_h_
#define dbsk2d_shock_transforms_h_
//:
// \file
// \brief Algorithms to do shock transforms on coarse shock graph and correct the shock topology
// \author Ozge Can Ozcanli
// \date 10/10/06
// 
// \verbatim
//  Modifications
//   O.C.Ozcanli   10/10/2006    added gap transform method
//                               gap transform is possible when the source of the edge being removed
//                               is of type SOURCE (meaning that in_degree of source node is zero)
//                               and furthermore, the edge is a DEGENERATE or a SEMIDEGENERATE edge.
//                               We remove the given intrinsic edge AND the SOURCE node AND the other outgoing intrinsic edge
// 
//                               CAUTION: currently we are not altering the boundary elements in the shock graph
//                                        when the gap transform is performed.
//                               (normally when the gap is closed, the completing boundary should be added.
//                                the situation in the DEGENERATE case is clear, the completion is an Euler spiral,
//                                however in the SEMIDEGENERATE case when the gap is closed, a T-junction is formed
//                                and it is not clear how to alter the boundary elements in this case.)
//
//                                This class assumes the existence of a coarse graph as well as an intrinsic shock graph.
//                                Most of the checks and operations are based on intrinsic shock, only computation of appearance
//                                cost depends on coarse graph's grouped edges
//
//  O. C. Ozcanli 02/16/2007     added appearance and contour continuity (via euler spiral) costs
//
//  O. C. Ozcanli 07/15/2007     added a non-redundant version of the gap closure decision function.
//
// \endverbatim 

#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/algo/dbsk2d_prune_ishock.h>
#include <dbgl/algo/dbgl_eulerspiral.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <dbsol/dbsol_interp_curve_2d_sptr.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>

#include <vcl_set.h>

#define REGULAR_GAP             0
#define SEMIDEGENERATE1_GAP     1
#define SEMIDEGENERATE2_GAP     2
#define DEGENERATE_GAP          3
#define DEGENERATEDEGTHREE_GAP  4

// \relates dbsk2d_shock_graph
class dbsk2d_shock_transforms : public dbsk2d_prune_ishock
{
public:

  //: Constructor
  dbsk2d_shock_transforms(dbsk2d_ishock_graph_sptr intrinsic_shock_graph, dbsk2d_shock_graph_sptr coarse_shock_graph) : 
      dbsk2d_prune_ishock(intrinsic_shock_graph, coarse_shock_graph), image_set_(false), bpoint1_(0), bpoint2_(0), es_(0), 
        valid_gap_node_radius_(-1.0f), coarse_shock_graph_source_(0),
        color_gamma_(14.0f), curve_gamma_(40.0f), curve_power_(2.0f), curve_offset_(0.1f), curve_length_gamma_(4.0f) {}

  //: Destructor
  virtual ~dbsk2d_shock_transforms() {}
  
  //: gap transform 
  //  remove the edge AND its source and other outgoing edges 
  //  and correct the shock graph topology
  //  return true if transform successful (input edge was truly a SOURCE nodes outgoing edge)
  bool gap_transform(dbsk2d_ishock_edge* iedge);

  bool gap_transform(dbsk2d_shock_edge_sptr sedge);
  bool gap_transform(dbsk2d_shock_node_sptr snode);

  void recompile_coarse();

  int type_iedge(dbsk2d_ishock_edge *iedge);

  bool valid_gap(dbsk2d_shock_edge_sptr sedge);
  bool valid_gap(dbsk2d_shock_node_sptr snode);

  bool valid_gap_iedge(dbsk2d_ishock_edge *iedge);
  

  // the function that will operate on intrinsic shock graph and hide the degree two node
  // and its immediate intrinsic edges
  // compile_coarse_shock_graph() method should be run once after this to recreate the "entire" coarse shock graph
  // this is the best way to make sure that the ordering of edges are correct after gap transforms
  bool gap_transform_only_hide(dbsk2d_ishock_edge* iedge);

  // valid_gap_node function should be run, before these two functions that creates the Euler spiral and the region samples!!
  //: return the appearance cost of gap transform if the regions are created
  double gap_transform_appearance_cost();
  //: return the euler spiral cost of gap transform if the euler spiral is created
  double gap_transform_contour_cost();

  //: if both costs are available use both of them to decide
  //  this old method is used in BMVC07 experiments, but it is redundant.
  //  just two thresholds is enough, the function below is the new version
  bool close_the_gap_old(double thres_contour_low, double thres_contour_high, 
                        double thres_app_low, double thres_app_high, 
                        double alpha_contour, double alpha_app, double& cont_cost, double& app_cost);

  //: new version of the gap closure decision
  bool close_the_gap(double thres_contour, 
                     double thres_app, 
                     double alpha_contour, double alpha_app, double& cont_cost, double& app_cost);


  //: perform all possible gap transforms within threshold in a rank ordered fashion
  void perform_all_gap_transforms(double thres_contour, double thres_app, 
                                  double alpha_contour, double alpha_app, bool keep_eulerspirals = false);

  dbgl_eulerspiral* get_eulerspiral() { return es_; }

  vcl_vector<vsol_point_2d_sptr>& get_region_plus_points() { return region_plus_points_; }
  vcl_vector<vsol_point_2d_sptr>& get_region_minus_points() { return region_minus_points_; }

  // merge edges till you hit a degree three node!! 
  // If we're at a degerate degree two node, immediate neighbors are degree threes
  // in semidegenerate case, we should continue all the way to the first degree three, merging all degree twos on the way if there are any
  double create_shock_interpolators(dbsk2d_shock_edge_sptr sedge, dbsol_interp_curve_2d_sptr& c, dbsk2d_shock_edge_sptr& new_edge, dbsk2d_shock_node_sptr& new_node);
  
  //: degree 2 node has immediate neighbor points, we use the radius and the nearest boundary at those points to extend the region beyond degree 2's immediate shock edges
  double create_till_boundary_interpolators(dbsk2d_shock_edge_sptr sedge, dbsk2d_shock_node_sptr srcn, dbsol_interp_curve_2d_sptr& c);
  
  void set_image(vil_image_resource_sptr image);

  void set_color_gamma(double val) { color_gamma_ = val; }
  void set_curve_gamma(double val) { curve_gamma_ = val; }
  void set_curve_power(double val) { curve_power_ = val; }
  void set_curve_offset(double val) { curve_offset_ = val; }
  void set_curve_length_gamma(double val) { curve_length_gamma_ = val; }

  void clear_all() {
    region_plus_points_.clear();
    region_minus_points_.clear();
    if (es_) {
      delete es_;
      es_ = 0;
    } 
  }

  void get_eulerspirals(vcl_vector< vsol_spatial_object_2d_sptr >& contours);
  void clear_eulerspirals() { ess_.clear(); }

protected:
  dbgl_eulerspiral* es_;
  double valid_gap_node_radius_;
  vcl_vector<vsol_point_2d_sptr> region_plus_points_;
  vcl_vector<vsol_point_2d_sptr> region_minus_points_;

  bool image_set_;
  vil_image_resource_sptr image_; ///< This is the image associated with the current geometry
  vil_image_view<float> L_, A_, B_;
  vil_image_view<vxl_byte> I_;  // if the input image is a grey image

  bool grey_image_;
  double color_gamma_, curve_gamma_;
  double curve_power_, curve_offset_, curve_length_gamma_;

  vcl_vector<dbgl_eulerspiral*> ess_;

  //: keep a list of degree two nodes which have become obsolete after performing a gap transform
  //  idea is to do "one closure per end node"
  vcl_set<dbsk2d_ishock_node*> obsolete_degree_twos_;

  //: keep these 
  dbsk2d_ishock_bpoint *bpoint1_, *bpoint2_;
  //: keep a list of end points which have become obsolete after performing a gap transform
  vcl_set<dbsk2d_ishock_bpoint*> obsolete_end_points_;
  typedef vcl_set<dbsk2d_ishock_bpoint*>::iterator obsolete_iter;
  
  dbsk2d_shock_node_sptr coarse_shock_graph_source_;
  int gap_type_;

};

#endif //dbsk2d_ishock_prune_h_
