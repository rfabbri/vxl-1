// This is dbdet_curve_fragment_cues.h
#ifndef dbdet_curve_fragment_cues_h
#define dbdet_curve_fragment_cues_h
//:
//\file
//\brief Compute several geometric and appearance cues of a curve fragment
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 05/25/2016 21:57:40 BRT
//


// just to make it easier and more descriptive to index the feature vector
// rarely used since we just process the feature vec. generically
enum yuliang_features {
  Y_BG_GRAD, Y_SAT_GRAD, Y_HUE_GRAD, Y_ABS_K, Y_EDGE_SPARSITY, Y_WIGG, Y_LEN, Y_MEAN_CONF
};

#define Y_NUM_FEATURES 8

typedef vnl_vector_fixed<double, NUM_FEATURES> y_feature_vector;

static const vxl_uint_32 dbdet_curve_fragment_cues_unvisited vcl_numeric_limits<vxl_uint_32>::max()
//: Compute curve fragment cues for many curves.
// Holds state information such as distance transform and auxiliary buffers,
// so that computation os fast for many curve fragments in the same image.
class dbdet_curve_fragment_cues {
public:

  // Make sure the input parameters stay valid
  // while this class is in use.
  void dbdet_curve_fragment_cues(
    const vil_image_view<rgbP >&hsv,
    const dbdet_edgemap &em
    )
    :
    visited_img_(hsv.ni(), hsv.nj(), 1),
    visited_id_(0),
    hsv_(hsv),
    //    dt_(dt),
    em_(em),
    dt_(NULL)
  {
    visited_img_.fill(dbdet_curve_fragment_cues_unvisited);
    // outside indices return false (visited)
    visited_ = vil_border_create_accessor(visited_img_,
        vil_border_create_constant(visited_img_, dbdet_curve_fragment_cues_unvisited));
  }

  // Try to speedup lateral edge sparsity computation for many fragments,
  // using with distance transform.
  //
  // Pass in the curve fragment list which will be queried.
  //  void use_dt(const dbdet_edgel_chain_list &frags)
  //  {
    // remove edgels of curves from curve maps
    // perform DT of the remaining map
    // const vil_image_view<vxl_uint_32> &dt
  //    dt_ = &dt;
  //  }

  void compute_cues(
      const dbdet_edgel_chain &c, 
      y_feature_vector *features_ptr // indexed by the enum
      );
  void cuvature_cues(
        const dbdet_edgel_chain &c, 
        y_feature_vector *features_ptr // indexed by the enum
        );
  void hsv_gradient_cues(
        const dbdet_edgel_chain &c, 
        y_feature_vector *features_ptr // indexed by the enum
      );
  double lateral_edge_sparsity_cues(const dbdet_edgel_chain &c
        y_feature_vector *features_ptr // indexed by the enum
      );

  static double euclidean_length(const dbdet_edgel_chain &c) {
    double len=0;
    for (unsigned i=0; i+1 < c.edgels.size(); ++i)
      len += vgl_distance(c.edgels[i]->pt, c.edgels[i+1]->pt);
    return len;
  }
private:
//  bool use_dt() const { return dt_ != NULL; }
  bool visited(int i, int j) const { visited_(i,j) == visited_id_; }
  bool not_visited(int i, int j) const { visited_(i,j) != visited_id_; }
  void mark_visited(int i, int j) { visited_(i,j) = visited_id_; }
  const vil_image_view<rgbP >&hsv_;
  //  vil_image_view<vxl_uint_32> *dt_;
  // visited(i,j) = c marks pixels(i,j) as visited by curve c's nhood tube
  // visited(i,j) = UIHNT_MAX marks pixels(i,j) as not visited
  vil_image_view<vxl_uint_32> visited_img_;
  vil_border_accessor<vil_image_view<unsigned> > visited_;
  // each compute_cues() run increments the id to mark traversals in scrap buffer visited_
  // that way we reuse the buffer without clearing it
  vxl_uint_32 visited_id_;
  const dbdet_edgemap &em_;
  static unsigned const local_dist_ = 1; // distance used for local sampling
  static unsigned const nbr_width_ = 3;  // distance used for lateral edge sparsity
};


#endif // dbdet_curve_fragment_cues_h
