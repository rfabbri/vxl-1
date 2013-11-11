// This is mw_gradient_descent.h
#ifndef mw_gradient_descent_h
#define mw_gradient_descent_h
//:
//\file
//\brief Gradient descent optimization of point properties for geometry estimation
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 09/23/2005 04:43:29 PM EDT
//


#include <dbsol/algo/dbsol_geno.h>


void gradient_descent(
    dbsol_geno_curve_2d *cnew,
    const dbsol_geno_curve_2d &c,
    double delta_angle,
    double psi);

void 
gradient_descent_positional(
    vcl_vector<vsol_point_2d_sptr> &pts,
    const dbsol_geno_curve_2d &c,
    double delta_norm,
    double psi,
    vcl_vector<double> &grad
    );

void
refine_tangents(dbsol_geno_curve_2d **input, dbsol_geno_curve_2d **refined,double delta_angle,double psi,unsigned n_iter);


#endif // mw_gradient_descent_h
