// This is seg/dbsks/dbsks_wcm.h
#ifndef dbsks_wcm_h_
#define dbsks_wcm_h_

//:
// \file
// \brief Whole contour matching
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Feb 6, 2009
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbsks/dbsks_ccm.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vgl/vgl_polygon.h>

// ============================================================================
// dbsks_wcm
// ============================================================================


//: A class to compute the matching between a shock graph and a contour map, 
// treating each contour as one single element.
class dbsks_wcm : public dbsks_ccm
{
public:
  // Constructor / destructor -------------------------------------------------
  
  //: constructor
  dbsks_wcm(): weight_unmatched_(0), weight_switching_contours_(0){};

  //: destructor
  virtual ~dbsks_wcm(){};

  // Access member variables ---------------------------------------------------
  
  //: Return weight of cost component due to unmatched edges
  float weight_unmatched() const {return this->weight_unmatched_; }

  //: Set weight of unmatched edges
  void set_weight_unmatched(float new_weight)
  { this->weight_unmatched_  = new_weight; }

  //: Return weight of cost component due to switching between linked contours
  float weight_switching_contours() const {return this->weight_switching_contours_; }

  //: Set weight of switching-contour cost
  void set_weight_switching_contours(float new_weight)
  { this->weight_switching_contours_ = new_weight; }


  //: Set the edge label image from a set of polylines
  void set_edge_labels(const vcl_vector<vsol_polyline_2d_sptr >& contours); 

  // Utilities -----------------------------------------------------------------

  //: Image cost of a contour (an ordered set of oriented points) given the shape's 
  // complete boundary
  float f_whole_contour(const vcl_vector<int >& x, const vcl_vector<int >& y, 
    const vcl_vector<int >& orient_channel, double contour_length,
    const vgl_polygon<double >& boundary);

  //: Cost of unmatched edges
  float cost_unmatched_edges(const vcl_vector<int >& x, const vcl_vector<int >& y, 
    const vcl_vector<int >& orient_channel, double contour_length,
    const vgl_polygon<double >& shape_boundary);

  //: Cost due to switching between linked contours (to avoid spurious edges)
  float cost_switching_linked_contours(const vcl_vector<int >& x, const vcl_vector<int >& y, 
    const vcl_vector<int >& orient_channel);


  
protected:
  //: weight of cost component due to unmatched portion of contours
  float weight_unmatched_;

  //: weight of cost component due to jumping between linked contours
  float weight_switching_contours_;



  //: An image containing contour labels of all edges in the image. A value of
  // 0 corresponds to no contour (background).
  vil_image_view<unsigned > edge_label_;
  vcl_map<unsigned, vsol_polyline_2d_sptr > map_label2contour_;

  //////////////////////////////////
  // old version - no longer used
  //: Image cost of a contour (an ordered set of oriented points) given the shape's 
  // complete boundary
  float f_whole_contour_v1(const vcl_vector<int >& x, const vcl_vector<int >& y, 
    const vcl_vector<int >& orient_channel, double contour_length,
    const vgl_polygon<double >& boundary);

  


};


#endif // seg/dbsks/dbsks_wcm.h


