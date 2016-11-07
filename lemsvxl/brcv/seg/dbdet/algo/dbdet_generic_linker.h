// This is brcv/seg/dbdet/algo/dbdet_generic_linker.h
#ifndef dbdet_generic_linker_h
#define dbdet_generic_linker_h
//:
//\file
//\brief Generic edge linker based on tracing on the image grid
//\author Amir Tamrakar
//\date 03/29/07
//
//\verbatim
//  Modifications
//   Amir Tamrakar Nov 07  Moved all data structures out of this algorithm class.
//
//\endverbatim

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_array_2d.h>
#include <vcl_vector.h>
#include <vcl_list.h>
#include <vcl_set.h>
#include <vcl_map.h>
#include <vcl_utility.h>

#include <dbdet/sel/dbdet_sel_utils.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/sel/dbdet_edgel.h>
#include <dbdet/sel/dbdet_curve_fragment_graph.h>

class dbdet_generic_linker : public vbl_ref_count
{
public:

  //: constructor
  dbdet_generic_linker(dbdet_edgemap_sptr edgemap, 
                       dbdet_curve_fragment_graph& curve_frag_graph,
                       vbl_array_2d<bool> & unlinked, 
                       double threshold=0.0); //parameters

  //: destructor
  virtual ~dbdet_generic_linker();

  // return parameters
  unsigned nrows() { return nrows_; }
  unsigned ncols() { return ncols_; }

  //: return a reference to the edgel buckets
  vbl_array_2d<vcl_vector<dbdet_edgel*> > & cells() { return edgemap_->edge_cells; }

  //: return the curve fragment graph
  dbdet_curve_fragment_graph & get_curve_fragment_graph() { return curve_frag_graph_; }

  //------------------------------------------------------------------------
  // Functions for tracing through the edge map
  //------------------------------------------------------------------------

  //: extract image contours by tracing
  void extract_image_contours_by_tracing();

  void trace_contours_in_both_directions();
  void trace_contours_from_end_points();
  void trace_contours_from_junctions();
  void form_junctions();

  int count_unlinked_edgels();
  bool unlinked(int x, int y) { return unlinked_(y, x); }

  //: recursively trace a contour from the given starting point
  vgl_point_2d<int> Grow_Seed_Edge (int x, int y, dbdet_edgel_chain* cur_chain, 
                                    double dx1, double dy1, 
                                    bool forward_trace);

    bool Is_Inside_Image (int row, int col);
    bool Is_8_Neighbor (int x1, int y1, int x2, int y2);
    bool Is_4_Neighbor (int x1, int y1, int x2, int y2);
    int Num_of_8_Neighbors (int row, int col);
    int Find_8_Neighbors (int row, int col, int *neighbor_row, int *neighbor_col);
    int Num_of_unlinked_8_Neighbors (int row, int col);
    int Find_unlinked_8_Neighbors (int row, int col, int *neighbor_row, int *neighbor_col);

    //set various flags for edge linking
    void require_appearance_consistency(bool val) { req_app_consistency_ = val; } //orientation consistency
    void set_appearance_threshold(double app_thresh) { app_thresh_ = app_thresh; }
    void require_orientation_consistency(bool val){ req_orient_consistency_ = val; } //orientation consistency
    void require_smooth_continuation(bool val){ req_smooth_continuation_ = val; } //smooth continuation
    void require_low_curvatures(bool val){ req_low_curvatures_ = val; } //low curvatures

  // for debug
  void report_stats();

protected:

  dbdet_edgemap_sptr edgemap_;                   ///< the edgemap to link
  vbl_array_2d<bool> &occupied_; ///< The matrix of flags to keep track of valid edgels (for tracing)
  vbl_array_2d<bool> &unlinked_; ///< The matrix of flags to keep track of the unlinked edgels
  dbdet_curve_fragment_graph& curve_frag_graph_; ///< The curve fragment graph (CFG)

  unsigned nrows_, ncols_;     // size of the edgemap
  
  //parameters of the linking algorithm
  double threshold_; //only link the edgels that are above the threshold
 
  bool req_app_consistency_; //use appearance consistency check for linking
  double app_thresh_; ///< appearance threshold

  bool req_orient_consistency_; ///< use orientation consistency check for linking
  bool req_smooth_continuation_;
  bool req_low_curvatures_;

};

#endif // dbdet_generic_linker_h
