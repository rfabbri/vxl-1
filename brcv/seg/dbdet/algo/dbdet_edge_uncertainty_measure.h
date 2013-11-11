// This is brcv/seg/dbdet/algo/dbdet_edge_uncertainty_measure.h
#ifndef dbdet_edge_uncertainty_measure_h
#define dbdet_edge_uncertainty_measure_h
//:
//\file
//\brief 
//
//
//\author Amir Tamrakar 
//\date 07/07/08
//
//\verbatim
//  Modifications
//\endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vbl/vbl_array_2d.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>

class dbdet_edge_uncertainty_measure
{
protected:
  double thresh_; ///< threshold
  
  //references to the data passed to this algo
  const vil_image_view<double>& Ix_; 
  const vil_image_view<double>& Iy_;

  vil_image_view<double> H_;

public:
  //: Constructor 
  dbdet_edge_uncertainty_measure(const vil_image_view<double>& grad_x, const vil_image_view<double>& grad_y, double sigma);

  //: Destructor
  ~dbdet_edge_uncertainty_measure(){}

  //Accessors
  unsigned width() { return H_.ni(); }
  unsigned height() { return H_.nj(); }

  //: return the uncertainty measure
  vil_image_view<double>& H_measure(){ return H_; } 

  //: get the uncertainty measures at the edgel locations
  void get_edgel_uncertainties(vcl_vector<vgl_point_2d<double> > & edge_locs, vcl_vector<double>& edge_uncertainties);

};

#endif // dbdet_edge_uncertainty_measure_h
