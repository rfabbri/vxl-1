// This is seg/dbsks/dbsks_subpix_ccm.h
#ifndef dbsks_subpix_ccm_h_
#define dbsks_subpix_ccm_h_

//:
// \file
// \brief Contour Chamfer Matching based on sub-pixel edge map
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date June 20, 2009
//
// \verbatim
//  Modifications
// \endverbatim



#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <vgl/vgl_box_2d.h>
#include <vnl/vnl_vector.h>


// ============================================================================
class dbsks_subpix_ccm
{
public:

  // Customized data types ---------------------------------------------------

  //: Method of computing corresponding edge points
  enum edge_correspondence_method
  {
    USE_CLOSEST_ORIENTED_EDGE = 2
  };


  // Constructor / destructor -------------------------------------------------
  
  //: constructor
  dbsks_subpix_ccm(): local_window_width_ (3.0f), 
    sampling_step_for_contour_orient_cost_(3.0f)
  {};

  //: destructor
  virtual ~dbsks_subpix_ccm(){};
  
  // Access member-variables ---------------------------------------------------

  //: Edge map associated with this cost
  dbdet_edgemap_sptr edgemap() const { return this->edgemap_; }

  //: Set edge map and clean up existing pre-computed costs
  void set_edgemap(const dbdet_edgemap_sptr& edgemap);

  //: Set neccessary parameters to convert distance difference to cost
  void set_distance_params(float distance_threshold, float distance_tol_near_zero)
  {
    this->distance_threshold_ = distance_threshold;
    this->distance_tol_near_zero_ = distance_tol_near_zero;
  }

  //: Set neccessary parameters to convert orientation difference to cost
  void set_orient_params(float orient_threshold, float orient_tol_near_zero)
  {
    this->orient_threshold_ = orient_threshold;
    this->orient_tol_near_zero_ = orient_tol_near_zero;
  }

  //: Set related parameters to compute ocm cost
  // the weights will be normalized such that their sum is equal to 1
  void set_weights(float weight_chamfer, float weight_edge_orient, float weight_contour_orient)
  {
    float sum = weight_chamfer +  weight_edge_orient + weight_contour_orient;
    this->weight_chamfer_ = weight_chamfer / sum;
    this->weight_edge_orient_ = weight_edge_orient / sum;
    this->weight_contour_orient_ = weight_contour_orient / sum;
  }

  //: Weight of chamfer cost
  float weight_chamfer() const {return this->weight_chamfer_; }

  //: Weight of edge orientation cost
  float weight_edge_orient() const {return this->weight_edge_orient_;}

  //: Weight of contour orientation cost
  float weight_contour_orient() const {return this->weight_contour_orient_; }

  //: Set number of angle bins from 0 to pi
  void set_nbins_0topi(int nbins_0topi)
  {
    this->nbins_0topi_ = nbins_0topi;
  }

  //: Angle per orientation channel
  double radians_per_bin() const;

  //: Number of angle bins from 0 to 2pi
  int nbins_0topi() const
  { return this->nbins_0topi_; }

  //: Compute bin number of an angle (in radian)
  int orient_bin(double angle);

  //: Return width of search window around each oriented poiint
  float local_window_width() const {return this->local_window_width_; }

  //: Set search window width
  void set_local_window_width(float new_width)
  { this->local_window_width_ = new_width; }

  
  // Utility functions --------------------------------------------------------


  ////: Clear the memory
  //void clear();


  //: Compute cost components for the whole image
  void compute(edge_correspondence_method = USE_CLOSEST_ORIENTED_EDGE,
    bool precompute_contour_orient_cost = true);

  //: Compute all cost components (internal data) for a given window
  void compute(const vgl_box_2d<int >& roi, edge_correspondence_method = USE_CLOSEST_ORIENTED_EDGE,
    bool precompute_contour_orient_cost = true);

  //: average cost of an ordered set of oriented points, each represented by an
  // index triplet (i_x, i_y, i_orient), where 0 <= i_orient < 2*nbins_0topi
  float f(const vcl_vector<int >& xs, const vcl_vector<int >& ys, 
    const vcl_vector<int >& orient_bins) const;

  //: Estimate lower bound value of ccm cost for the specified ROI
  float compute_f_lower_bound() const;

  //: Estimate upper bound valu of ccm cost for the specified ROI
  float compute_f_upper_bound() const;

  //: Retrieve match edgels given position and orientation of contour piont
  dbdet_edgel* matched_edgel(int ix, int iy, int ibin) const;


protected:
  //: Convert distance between query point and edgel to Chamfer cost [0, 1]
  float chamfer_cost(double distance) const;

  //: Convert angle between query point and edgel to "edge orientation cost" [0, 1]
  float edge_orient_cost(double signed_angle_diff) const;

  //: Contour orientation cost
  float contour_orient_cost(int cur_x, int cur_y, int cur_orient_bin,
    int next_x, int next_y, int next_orient_bin) const;

  //: Compute CCM cost as cost to edge closest in both distance and orientation
  // assuming memory for internal variables have been properly allocated
  bool compute_ccm_cost_using_closest_oriented_edge();

  //: Matching edgels to image points using Oriented Chamfer Matching cost
  // Output is saved to a member variable (matched_edgels_)
  bool compute_matched_edgels_using_ocm_cost();

  //: Compute the 3 components (chamfer, edge orient, contour orient) from matched_edgels
  // Results are saved in 3 member variables
  bool compute_cost_components_from_matched_edgels();

  //: Compute orthgonal distance to matched edgels
  bool compute_ortho_dist_from_matched_edgels();
  

  // Member variables ----------------------------------------------------------
  
protected:
   //>> Flags >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  
  bool precompute_contour_orient_cost_;

  //>> Model parameters >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

  //: Normalization constant for Chamfer distance. All edges farther than this 
  // will be ignored
  float distance_threshold_; 

  //: distance tolerance due to discretization
  float distance_tol_near_zero_;

  //: Normalization constant for orientation. 
  float orient_threshold_;

  // orientation tolerance due to discretization
  float orient_tol_near_zero_;

  //: weight of Chamfer cost
  float weight_chamfer_; 

  //: weight of edge orientation cost
  float weight_edge_orient_;

  //: weight of contour orientation cost
  float weight_contour_orient_; 

  //: Angle discretization - Number of angle bins from 0 to pi
  int nbins_0topi_;

  //: Width of window around each oriented point to search for supporting edges
  float local_window_width_;

  //: Sampling step to estimate contour orientation difference
  float sampling_step_for_contour_orient_cost_;

  //>> Input Data >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  
  //: Edge map
  dbdet_edgemap_sptr edgemap_;

  //>> Internal data >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

  
  //======== Cost components ===================================================

  //: Window on which cost has been computed
  vgl_box_2d<int > roi_;

  //: Status of pre-computed costs: true if pre-computed costs are up-to-date
  vbl_array_2d<bool > cost_update_to_date_;

  //: Matched edgel for each oriented point (x, y, orient) on the image
  vbl_array_2d<vcl_vector<dbdet_edgel*> > matched_edgel_; 

  //: Distance (Chamfer) cost
  vbl_array_2d<vnl_vector<float > > chamfer_cost_;

  //: edge orientation cost
  vbl_array_2d<vnl_vector<float > > edge_orient_cost_;

  //: contour orientation cost
  vbl_array_2d<vnl_vector<float > > contour_orient_cost_;

  //: distance to the matched edgel - along the direction orthogonal to tangent
  vbl_array_2d<vnl_vector<float > > ortho_dist_to_matched_edgel_;
};


#endif // seg/dbsks/dbsks_subpix_ccm.h


