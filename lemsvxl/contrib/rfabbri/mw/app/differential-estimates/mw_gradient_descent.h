// This is mw_gradient_descent.h
#ifndef mw_gradient_descent_h
#define mw_gradient_descent_h
//:
//\file
//\brief Gradient descent optimization of point properties for geometry estimation
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@gmail.com)
//\date 09/23/2005 04:43:29 PM EDT
//


#include <bsold/algo/bsold_geno.h>


void gradient_descent(
    bsold_geno_curve_2d *cnew,
    const bsold_geno_curve_2d &c,
    double delta_angle,
    double psi);

void 
gradient_descent_positional(
    std::vector<vsol_point_2d_sptr> &pts,
    const bsold_geno_curve_2d &c,
    double delta_norm,
    double psi,
    std::vector<double> &grad
    );

void
refine_tangents(bsold_geno_curve_2d **input, bsold_geno_curve_2d **refined,double delta_angle,double psi,unsigned n_iter);


#endif // mw_gradient_descent_h
