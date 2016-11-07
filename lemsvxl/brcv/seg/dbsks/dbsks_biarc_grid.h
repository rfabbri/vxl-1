// This is seg/dbsks/dbsks_biarc_grid.h
#ifndef dbsks_biarc_grid_h_
#define dbsks_biarc_grid_h_

//:
// \file
// \brief A grid of biarcs
//       
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Oct 28, 2008
//
// \verbatim
//  Modifications
// \endverbatim



#include <vnl/vnl_vector.h>
#include <dbgl/algo/dbgl_biarc.h>




// =============================================================================
// dbsks_biarc_grid_params
// =============================================================================
//: one type of parameter set to construct a xnode grid
struct dbsks_biarc_grid_params
{
  double min_x;
  double step_x;
  int num_x;
  
  double min_y;
  double step_y;
  int num_y;

  double min_dx;
  double step_dx;
  int num_dx;

  double min_dy;
  double step_dy;
  int num_dy;

  double min_alpha0;
  double step_alpha0;
  int num_alpha0;

  double min_alpha2;
  double step_alpha2;
  int num_alpha2;
};
  

  










// ============================================================================
// dbsks_biarc_grid
// ============================================================================

// Shape fragment grid parameters
class dbsks_biarc_grid
{
public:
  //: Constructor 
  dbsks_biarc_grid(){};
  ~dbsks_biarc_grid(){};


  //: Compute the grid from a parameter set
  bool compute(const dbsks_biarc_grid_params& p);


  // x
  double step_x_;
  vnl_vector<double > x_;

  // y
  double step_y_;
  vnl_vector<double > y_;

  // dx - difference between x-coordinates of start and end points
  double step_dx_;
  vnl_vector<double > dx_;

  // dy - difference between y-coordinates of start and end  points
  double step_dy_;
  vnl_vector<double > dy_;

  // alpha0 - angle between the chord (connecting two end points) and the curve
  // tangent at the starting point
  double step_alpha0_;
  vnl_vector<double > alpha0_;

  // alpha2 - angle between the chord (connecting two end points) and the curve 
  // tangent at the end points
  double step_alpha2_;
  vnl_vector<double > alpha2_;

  //: Print grid information
  vcl_ostream& print(vcl_ostream& os) const;
};



#endif // seg/dbsks/dbsks_biarc_grid.h


