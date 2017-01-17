//---------------------------------------------------------------------
// This is brcv/rec/dbskr/dbskr_shock_patch.h
//:
// \file
// \brief Shock patch class
//
// \author
//  O.C. Ozcanli - March 13, 2007
//
// \verbatim
//
//  Modifications
//
// \endverbatim
//
//-------------------------------------------------------------------------

#ifndef _dbskr_shock_patch_h
#define _dbskr_shock_patch_h


#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>

#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vgl/vgl_polygon.h>

#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbskr/dbskr_tree_sptr.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_resource_sptr.h>

#include "dbskr_shock_patch_sptr.h"
#include "dbskr_v_graph_sptr.h"

// Construct a virtual undirected graph for the subgraph extracted, this will be used to trace the boundary 
// all the degree two nodes will be merged and degree three nodes will constitute the nodes of this virtual graph
#include <dbgrl/dbgrl_edge.h>
#include <dbgrl/dbgrl_vertex.h>
#include <dbsk2d/dbsk2d_shock_node_sptr.h>
#include <dbsk2d/dbsk2d_shock_edge_sptr.h>

#include <dbinfo/dbinfo_observation_sptr.h>

#include <vgl/algo/vgl_h_matrix_2d.h>

//: helper classes in costruction of a patch
class dbskr_v_edge : public dbgrl_edge<dbskr_v_node> 
{
public:
  vcl_vector<dbsk2d_shock_edge_sptr> edges_;
  int start_node_id_;
  int end_node_id_;
  float length_;

  dbskr_v_edge() : dbgrl_edge<dbskr_v_node>(), start_node_id_(-1), end_node_id_(-1), length_(-1) {}
  dbskr_v_edge(dbskr_v_node_sptr vs, dbskr_v_node_sptr vt) : dbgrl_edge<dbskr_v_node>(vs, vt),  start_node_id_(-1), end_node_id_(-1), length_(-1) {}
  dbskr_v_edge(dbskr_v_node_sptr vs, dbskr_v_node_sptr vt, vcl_vector<dbsk2d_shock_edge_sptr>& edges, int sid, int eid) : dbgrl_edge<dbskr_v_node>(vs, vt), edges_(edges), start_node_id_(sid), end_node_id_(eid), length_(-1) {}
  dbskr_v_edge(const dbskr_v_edge& other);

  float length();

  virtual ~dbskr_v_edge() { edges_.clear(); }
};

class dbskr_v_node : public dbgrl_vertex<dbskr_v_edge> 
{
public:
  dbsk2d_shock_node_sptr original_shock_node_;
  int id_;

  dbskr_v_node(): dbgrl_vertex<dbskr_v_edge>(), id_(0), original_shock_node_(0) {}
  dbskr_v_node(int id): dbgrl_vertex<dbskr_v_edge>(), id_(id), original_shock_node_(0) {}
  dbskr_v_node(dbsk2d_shock_node_sptr n);
  dbskr_v_node(const dbskr_v_node& other);
  virtual ~dbskr_v_node() {}
};

//: a test graph class subclasssed from dbgrl_graph
class dbskr_v_graph : public dbgrl_graph<dbskr_v_node, dbskr_v_edge>
{
public:
  dbskr_v_graph() : dbgrl_graph<dbskr_v_node, dbskr_v_edge>() {}
  ~dbskr_v_graph(){}

  float node_overlap(dbskr_v_graph& other);
  bool same(dbskr_v_graph& other);
  //: does not check the edge list on the vertices but only the vertex ids and the edge source and target ids
  bool same_vertex_and_edges(dbskr_v_graph& other);
  vsol_box_2d_sptr bounding_box();

};


//: Shock Curve class
class dbskr_shock_patch : public vbl_ref_count 
{ 
public:
  
  //: default constructor
  dbskr_shock_patch() : color_contrast_(-1), grey_contrast_(-1), real_len_(0), contour_ratio_(-1), poly_len_(-1), traced_poly_len_(-1), 
    depth_(-1), id_(-1), 
    shock_pruning_threshold_(1.0f), elastic_splice_cost_(true), construct_circular_ends_(false), scurve_sample_ds_(1.0f),
    contour_tracer_sigma_(1.0f) , line_fitting_rms_(0.05f) {}

  //: contructor with data
  dbskr_shock_patch(int id, int depth) : color_contrast_(-1), grey_contrast_(-1), real_len_(0), contour_ratio_(-1), poly_len_(-1), traced_poly_len_(-1),
    depth_(depth), id_(id), 
    shock_pruning_threshold_(1.0f), elastic_splice_cost_(true), construct_circular_ends_(false), scurve_sample_ds_(1.0f),
    contour_tracer_sigma_(1.0f) , line_fitting_rms_(0.05f) {}

  //: Interpolate/copy constructor
  //  Constructs and interpolated copy of an scurve
  dbskr_shock_patch(dbskr_shock_patch const& old);

  //: Destructor
  ~dbskr_shock_patch() { clear(); };
  
  //: Assignment operator
  //dbskr_shock_patch& operator=(const dbskr_shock_patch &rhs);

  void set_outer_boundary(vsol_polygon_2d_sptr poly); 
  vsol_polygon_2d_sptr get_outer_boundary() { return poly_; }
  bool inside_box(double x, double y);
  
  bool inside(double x, double y);

  vsol_box_2d_sptr bounding_box() { return bounding_box_; }
  vsol_box_2d_sptr union_box(dbskr_shock_patch& other);
  vsol_box_2d_sptr bounding_box_real();

  //: this function is used to determine if any of the real boundaries is on this box
  //  the input box is usually the bounding box of the scene (all curve fragments in the scene) 
  //  then this patch is usually eliminated 
  bool boundary_on_the_box(vsol_box_2d_sptr bbox);
  
  //: clean the real boundary set from the bbox contours
  void clean_real_boundaries(vsol_box_2d_sptr bbox);

  void add_real_boundary(vsol_polyline_2d_sptr p) { real_boundaries_.push_back(p); }
  vcl_vector<vsol_polyline_2d_sptr>& get_real_boundaries() { return real_boundaries_; }

  void clear() { 
    real_boundaries_.clear(); 
    poly_ = 0;
    traced_poly_ = 0;
    bounding_box_ = 0;
    bounding_box_real_ = 0;
    obs_ = 0;
    kill_tree();
    kill_v_graph();
    kill_shock_graph();
  }
  
  //: create the observation
  //bool create_observation(vil_image_resource_sptr img, bool intensity_channel, bool gradient_channel, bool color_channel);

  //: bool create a binary image and contour trace it to get a non-overlapping outer boundary for shock extraction
  bool trace_outer_boundary();

  //: bool extract shocks from the simple closed traced boundary
  bool extract_simple_shock();

  //: extract the shock graph from the outer poly if wanted
  bool extract_shock_from_outer_boundary();

  //: bool extract shocks from the simple closed traced boundary
  bool prepare_tree();

  void set_traced_boundary(vsol_polygon_2d_sptr p) { traced_poly_ = p; }
  void set_shock_graph(dbsk2d_shock_graph_sptr sg) { traced_shock_ = sg; }
  
  //: if not computed yet, compute it!
  vsol_polygon_2d_sptr get_traced_boundary();

  //: this method returns the existing traced_shock_, check if it is zero and if so first call extract_simple_shock!
  dbsk2d_shock_graph_sptr shock_graph() { return traced_shock_; }

  //dbsk2d_shock_graph_sptr trimmed_shock_graph() { return trimmed_shock_; }

  void set_tree_parameters(bool elastic_splice_cost, bool construct_circular_ends, bool combined_edit,
                           float scurve_sample_ds, float scurve_interp_ds) {
      elastic_splice_cost_ = elastic_splice_cost, construct_circular_ends_ = construct_circular_ends,
      combined_edit_ = combined_edit, scurve_sample_ds_ = scurve_sample_ds, scurve_interp_ds_ = scurve_interp_ds; }

  //: if not computed yet, this method computes it by calling prepare_tree()!
  //  so set the tree computation parameters before calling this method if it will be computed
  dbskr_tree_sptr tree();
  
  void output_info();

  float contour_ratio(void);
  float real_boundary_length(void);
  float outer_boundary_length(void);
  float traced_boundary_length(void);

  //: region width is the number of pixels along the normal direction that the function considers to collect color stats
  void find_color_contrast(vil_image_view<float>& L, vil_image_view<float>& A, vil_image_view<float>& B, float region_width = 5.0);
  void find_grey_contrast(vil_image_view<vxl_byte>& I, float region_width = 5.0);

  float color_contrast() { return color_contrast_; }
  float app_contrast() { return grey_contrast_; }

  void set_depth(int depth) { depth_ = depth; }
  int depth() { return depth_; }

  void set_id(int id) { id_ = id; }
  int id() { return id_; }
  int start_node_id(int factor = 100);
  
  void set_v_graph(dbskr_v_graph_sptr vg) { vg_ = vg; }
  dbskr_v_graph_sptr get_v_graph() { return vg_; }

  //: return the ratio of number of overlaping nodes in the v_graphs of the given patches
  //  CAUTION: does not consider if the number of children of the nodes are the same, only checks their ids and if the same, counts them as an overlap
  float v_graph_node_overlap(dbskr_shock_patch_sptr sp);

  //: return true if the two v_graphs are exactly the same
  //  checks whether the number of children are exactly the same as well 
  bool v_graph_same(dbskr_shock_patch_sptr sp);

  //:  there are cases such that subgraphs are different but after shock trimming the real boundaries
  //   and hence the trimmed shock is almost exactly the same
  //   or e.g. real boundaries traced from outer shocks of an object, and traced from inner shocks are the same
  //   check if areas and bounding box centers are almost exactly the same
  bool same_real_boundaries(dbskr_shock_patch_sptr sp);

  //: trees are taking up too much space with all the scurves etc, they should be killed after usage and recomputed everytime
  void kill_tree(); 
  void kill_shock_graph();
  void kill_v_graph();

  //: mutual info methods
  void set_observation(dbinfo_observation_sptr obs) { obs_ = obs; }
  dbinfo_observation_sptr observation(void) { return obs_; }
  void extract_observation(vil_image_resource_sptr img, bool app, bool grad, bool color);

  //: compute the mapped image of this patch onto the target image using the homography which is computed usign some correspondence
  //  e.g. using edit distance correspondence of this patch with some other patch extracted from the target image.
  vil_image_resource_sptr mapped_image(vil_image_resource_sptr img_source, vil_image_resource_sptr img_target, const vgl_h_matrix_2d<double>& H, bool recompute_obs);

  //-----------------------
  //:  BINARY I/O METHODS |
  //-----------------------

  //: Serial I/O format version
  virtual unsigned version() const {return 4;} // version 4: not writing real boundaries, if you wanna write & read them go back to version 3
                                               // version 3: added construct_circular_ends_ flag
                                               // version 2: writing real boundaries as well

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const {return "dbskr_shock_patch";}

  //: determine if this is the given class
  virtual bool is_class(vcl_string const& cls) const
   { return cls==is_a();}
  
  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const ;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

public:

  //: shock computation variables
  float shock_pruning_threshold_;

  //: tree computation variables
  bool elastic_splice_cost_;
  bool construct_circular_ends_;
  bool combined_edit_;
  float scurve_sample_ds_;
  float scurve_interp_ds_;

  //: variables for tracing outer polygon
  float contour_tracer_sigma_ , line_fitting_rms_;

protected:
  //----------------------------------------
  // Shock Patch Data
  //----------------------------------------

  int depth_, id_;

  vsol_polygon_2d_sptr poly_, traced_poly_;
  vgl_polygon<double> p_;
  vsol_box_2d_sptr bounding_box_, bounding_box_real_;
  vcl_vector<vsol_polyline_2d_sptr> real_boundaries_;

  //: this graph is the coarse subgraph of shock graph which is used to extract this patch
  //  it is onyl kept for temporary processing
  dbskr_v_graph_sptr vg_;
  dbsk2d_shock_graph_sptr traced_shock_;//, trimmed_shock_;
  dbskr_tree_sptr tree_;

  //: create an observation for poylgon filling and mutual info calculations
  dbinfo_observation_sptr obs_;
  
  //: force these values to be computed before being used by putting them into protected area
  float color_contrast_, grey_contrast_;
  float contour_ratio_, real_len_, poly_len_, traced_poly_len_;
};

#endif  // _dbskr_shock_patch_h
