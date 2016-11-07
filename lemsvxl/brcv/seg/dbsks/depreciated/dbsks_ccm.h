// This is seg/dbsks/dbsks_ccm.h
#ifndef dbsks_ccm_h_
#define dbsks_ccm_h_

//:
// \file
// \brief Oriented Chamfer Matching for contours
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Nov 11, 2007
//
// \verbatim
//  Modifications
// \endverbatim


#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vil/vil_image_view.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_array_3d.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vcl_vector.h>


// ============================================================================
class dbsks_ccm
{
public:

  // Customized data types ---------------------------------------------------

  //: Method of computing corresponding edge points
  enum edge_correspondence_method
  {
    USE_CLOSEST_EDGE = 1,
    USE_CLOSEST_ORIENTED_EDGE = 2
  };


  // Constructor / destructor -------------------------------------------------
  
  //: constructor
  dbsks_ccm(){};

  //: destructor
  virtual ~dbsks_ccm(){};
  
  // User-interface functions -------------------------------------------------

  //: Clear the memory
  void clear();

  //: Set related parameters to compute ocm cost
  // "nchannel" is the number of bins to cover [0, pi]
  void set_ocm_params(float distance_threshold, float tol_near_zero, float lambda, 
    float gamma,
    int nchannel_0topi = 18)
  {
    this->distance_threshold_ = distance_threshold;
    this->tol_near_zero_ = tol_near_zero;
    this->lambda_ = lambda;
    this->gamma_ = gamma;
    this->nchannel_0topi_ = nchannel_0topi;
  }

  //: Compute edge strength at each pixel from an edgemap
  bool set_edge_strength(const vil_image_view<float >& edgemap, 
    float edge_val_threshold,
    float max_edge_val);

  //: Set edge orientation from a matrix of edge angles
  bool set_edge_orient(const vil_image_view<float >& edge_angle);

  //: Angle per orientation channel
  double radians_per_channel() const;

  //: Number of angle bins from 0 to 2pi
  int nbins_0to2pi() const
  { return this->nchannel_0topi_ * 2; }

  //: Is the point inside the image's range?
  inline bool in_image_range(int ix, int iy) const
  {
    return this->edge_strength_.in_range(ix, iy);
  }

  //: Weight of edge orientation cost
  float weight_edge_orient() const { return this->lambda_; }

  //: Weight of contour orientation cost
  float weight_contour_orient() const {return this->gamma_; }

  //: Weight of chamfer cost
  float weight_chamfer() const {return 1.0f - this->lambda_ - this->gamma_; }


  //: Compute cost components for the whole image
  void compute(edge_correspondence_method = USE_CLOSEST_ORIENTED_EDGE,
    bool precompute_contour_orient_cost = true);

  //: Compute cost components for a given window
  void compute(const vgl_box_2d<int >& roi, edge_correspondence_method = USE_CLOSEST_ORIENTED_EDGE,
    bool precompute_contour_orient_cost = true);

  // Utility functions --------------------------------------------------------

  //: cost of an oriented point, represented by its index
  float f(int i, int j, int orient_channel);

  //: cost of an oriented point, represented by its index
  float f(const vcl_vector<int >& x, const vcl_vector<int >& y, const vcl_vector<int >& orient_channel);

protected:

  //: Set edge orientation by computing gradient of DT of binary (by thresholding) edgemap
  bool set_closest_edge_orient(const vil_image_view<float >& edgemap, float threshold);


  //: Chamfer cost [0, 1] given distance of edge to query point
  float chamfer_cost(double distance) const;

  //: Edge orientation cost [0, 1] given difference in orientation between the edge point
  // and the query point
  float edge_orient_cost(double angle_difference) const;

  //: Contour orientation cost
  float contour_orient_cost(int cur_x, int cur_y, int cur_orient_bin,
    int next_x, int next_y, int next_orient_bin);

  //: Compute OCM cost as cost to closest edge
  bool compute_ocm_cost_using_closest_edge();

  //: Compute CCM cost as cost to edge closest in both distance and orientation
  bool compute_ocm_cost_using_closest_oriented_edge();

  // Member variables ---------------------------------------------------------

  // Flags
  bool precompute_contour_orient_cost_;

  // Input data --------------------------------------------------------------
  float distance_threshold_; // all edges farther than this will be ignored
  float tol_near_zero_; // due to discretization
  float lambda_; // weight orientation cost
  float gamma_; // weight of derivative of distance to closest edge
  int nchannel_0topi_;

  //: Edge strength
  vil_image_view<float > edge_strength_; //>>> normalized to [0, 1]

  //: Edge orientation
  vil_image_view<float > edge_orient_x_; //>>> x-component of normalized orientation vector
  vil_image_view<float > edge_orient_y_; //>>> y-component of normalized orientation vector
  vil_image_view<float > edge_angle_;    //>>> between [0, 2pi]

  // Info regarding computed corresponding edges of each image point -----------

  //: Position (x,y) of edges corresponding to each image point
  vbl_array_2d<vnl_vector<int > > matched_edge_ix_;
  vbl_array_2d<vnl_vector<int > > matched_edge_iy_;

  //: Orientation of closest edge
  vil_image_view<float > closest_edge_orient_x_; //>>> x-component of normalized orientation vector
  vil_image_view<float > closest_edge_orient_y_; //>>> y-component of normalized orientation vector

  // Cost components -----------------------------------------------------------

  //: Cost computation window
  vgl_box_2d<int > roi_;
  
  //: Distance (Chamfer) cost
  vbl_array_2d<vnl_vector<float > > chamfer_cost_;

  //: edge orientation cost
  vbl_array_2d<vnl_vector<float > > edge_orient_cost_;

  //: contour orientation cost
  vbl_array_2d<vnl_vector<float > > contour_orient_cost_;
};


#endif // seg/dbsks/dbsks_ccm.h


