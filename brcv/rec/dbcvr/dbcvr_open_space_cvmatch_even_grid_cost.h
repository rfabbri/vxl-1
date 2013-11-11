#ifndef _dbcvr_open_space_cvmatch_even_grid_cost_h
#define _dbcvr_open_space_cvmatch_even_grid_cost_h
//---------------------------------------------------------------------
// This is brcv/rec/dbcvr/dbcvr_open_space_cvmatch_even_grid_cost.h
//:
// \file
// \brief This class computes the cost 
//        for 3D open curve matching on an even grid. Both explicit and 
//        implicit formulations are implemented.
//
// \author H. Can Aras (can@lems.brown.edu)
// \date 2007-01-16
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//----------------------------------------------------------------------

#include <dbdp/dbdp_cost.h>
#include <dbsol/dbsol_interp_curve_3d.h>

class dbcvr_open_space_cvmatch_even_grid_cost : public dbdp_cost
{
  // functions
public:
  //: c1, c2 : curves
  //  num_samples_c1, num_samples_c2: the number of samples taken
  //  on c1 and c2, respectively)
  //  R1, R2 : cost weight coefficients (R1 for bending cost, R2 for twisting cost)
  //  If cost_formula_comput_type is "explicit" / "implicit", 
  //  the explicit/implicit cost function is used.
  //  If angle_der_comput_type is "geometric-formulas" / "eno-scheme", 
  //  the angle derivatives are computed by using geometric formulas / ENO
  //  interpolation scheme.
  dbcvr_open_space_cvmatch_even_grid_cost(dbsol_interp_curve_3d *c1, dbsol_interp_curve_3d *c2, 
                                          int num_samples_c1, int num_samples_c2, 
                                          double R1, double R2,
                                          char *cost_formula_comput_type,
                                          char *angle_der_comput_type = 0);
  virtual ~dbcvr_open_space_cvmatch_even_grid_cost() {};
  // compute properties of both curves
  void compute_properties();
  // compute cost for matching interval [c1(ip),c1(i)]  to [c2(jp),c2(j)]
  virtual double compute_interval_cost(int i, int ip, int j, int jp);

protected:
  int pick_points_for_eno(vcl_vector<double> &points, vcl_vector<double> &picked_points,    
                          vcl_vector<double> &arclengths, vcl_vector<double> &picked_arclengths,
                          int index);
  void continuous_angles(vcl_vector<double> &angles);
  // variables
public:

protected:
  // pointers to curves
  dbsol_interp_curve_3d *c1_;
  dbsol_interp_curve_3d *c2_;
  // number of sampling points on each curve
  // also determines the DP grid size
  int num_samples_c1_, num_samples_c2_;
  // cost weight parameters for bending and twisting
  double R1_, R2_;
  // cost formula computation type, case-sensitive
  char *cfct_;
  // angle derivative computation type (only when cfct_ = "implicit"), case-sensitive
  char *adct_;
  // step size used for even resampling of the curves
  double ds1_, ds2_;

  // arclength at each sampling point
  vcl_vector<double>  curve1_lengths_;
  vcl_vector<double>  curve2_lengths_;
  // curvature at each sampling point
  vcl_vector<double>  curve1_curvatures_;
  vcl_vector<double>  curve2_curvatures_;
  // torsion at each sampling point
  vcl_vector<double>  curve1_torsions_;
  vcl_vector<double>  curve2_torsions_;
  // phi (tilt angle of the tangent vector) at each sampling point
  vcl_vector<double>  curve1_phis_;
  vcl_vector<double>  curve2_phis_;
  // theta (azimuth angle of the tangent vector) at each sampling point
  vcl_vector<double>  curve1_thetas_;
  vcl_vector<double>  curve2_thetas_;
  // dphi at each sample point (derivative w.r.t. to arclength)
  vcl_vector<double>  curve1_dphis_;
  vcl_vector<double>  curve2_dphis_;
  // dtheta at each sample point (derivative w.r.t. to arclength)
  vcl_vector<double>  curve1_dthetas_;
  vcl_vector<double>  curve2_dthetas_;
};

#endif

