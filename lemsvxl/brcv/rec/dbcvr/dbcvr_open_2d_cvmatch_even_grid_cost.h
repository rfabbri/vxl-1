#ifndef _dbcvr_open_2d_cvmatch_even_grid_cost_h
#define _dbcvr_open_2d_cvmatch_even_grid_cost_h
//---------------------------------------------------------------------
// This is brcv/rec/dbcvr/dbcvr_open_2d_cvmatch_even_grid_cost.h
//:
// \file
// \brief This class computes the cost 
//        for 2D open curve matching on an even grid.
//
// \author H. Can Aras (can@lems.brown.edu)
// \date 2007-01-31
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//----------------------------------------------------------------------

#include <dbdp/dbdp_cost.h>
#include <dbsol/dbsol_interp_curve_2d.h>

class dbcvr_open_2d_cvmatch_even_grid_cost : public dbdp_cost
{
  // functions
public:
  //: c1, c2 : curves
  //  num_samples_c1, num_samples_c2: the number of samples taken
  //  on c1 and c2, respectively)
  //  R1 : bending cost weight coefficient
  dbcvr_open_2d_cvmatch_even_grid_cost(dbsol_interp_curve_2d *c1, dbsol_interp_curve_2d *c2, 
                                          int num_samples_c1, int num_samples_c2, 
                                          double R1);
  virtual ~dbcvr_open_2d_cvmatch_even_grid_cost() {};
  // compute properties of both curves
  void compute_properties();
  // compute cost for matching interval [c1(ip),c1(i)]  to [c2(jp),c2(j)]
  virtual double compute_interval_cost(int i, int ip, int j, int jp);

protected:
  void continuous_angles(vcl_vector<double> &angles);

  // variables
public:

protected:
  // pointers to curves
  dbsol_interp_curve_2d *c1_;
  dbsol_interp_curve_2d *c2_;
  // number of sampling points on each curve
  // also determines the DP grid size
  int num_samples_c1_, num_samples_c2_;
  // cost weight parameters for bending
  double R1_;
  // step size used for even resampling of the curves
  double ds1_, ds2_;

  // arclength at each sampling point
  vcl_vector<double>  curve1_lengths_;
  vcl_vector<double>  curve2_lengths_;
    // tangent vector angle at each sampling point
  vcl_vector<double>  curve1_thetas_;
  vcl_vector<double>  curve2_thetas_;
};

#endif

