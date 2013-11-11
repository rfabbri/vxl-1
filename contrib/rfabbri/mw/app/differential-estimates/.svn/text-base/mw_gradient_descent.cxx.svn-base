#include "mw_gradient_descent.h"

#include <dbgl/algo/dbgl_eulerspiral.h>
#include <vnl/vnl_math.h>
#include <dbsol/algo/dbsol_geno.h>
#include <mw/mw_util.h>

static void compute_tangent_gradient(
    const dbsol_geno_curve_2d &c,
    double delta_angle,
    vcl_vector<double> &grad);

static void 
compute_positional_gradient_arc(
    const dbsol_geno_curve_2d &c,
    double delta_normal,
    vcl_vector<double> &grad);

void 
compute_positional_gradient_arc_dt(
    const dbsol_geno_curve_2d &c,
    double delta_normal,
    vcl_vector<double> &grad);

//: Compute gradient of a smoothness energy on the eulerspiral-geno
// interpolation, relative to variation on tangent angles. This is used for
// gradient-descent refinement of the interpolation given by interpolate3_approx
//
// \param[in] delta_angle step to compute discrete partial derivatives
// \param[in] c geno curve
// \param[out] grad  gradient vector
void 
compute_tangent_gradient(
    const dbsol_geno_curve_2d &c,
    double delta_angle,
    vcl_vector<double> &grad) 
{
  assert(delta_angle > 0);

  vcl_vector<double> dk; //:< differences of curvature at each sample
  vcl_vector<double> dksq; //: squared dk's

  dk.resize(c.size()+1,0);
  grad.resize(dk.size(),0);
  dksq.resize(dk.size(),0);

  double E=0, tolerance = 1e-13;
  for (unsigned i=1; i<c.size(); ++i) {
    dk[i] = c[i].curvature_at(0) - c[i-1].curvature_at(1);
    dksq[i] = dk[i]*(dk[i]);
    E += dksq[i];
    if (!(fabs(c[i].tangent_angle_at(0)- c[i-1].tangent_angle_at(1)) < tolerance)) {
      vcl_cout.precision(20);

      vcl_cout << "Warning; tangent error: \n" << 
        "Interval " << i << vcl_endl << 
        "c[i].tangent_angle_at(0) " << c[i].tangent_angle_at(0) << vcl_endl << 
        "c[i-1].tangent_angle_at(1) " << c[i-1].tangent_angle_at(1) << vcl_endl
        << "difference: " << fabs(c[i].tangent_angle_at(0)- c[i-1].tangent_angle_at(1)) 
        << vcl_endl;
    }
  }

  for (unsigned i=2; i<(dk.size() -2); ++i) {
    double E_i_minus1, E_i_plus1;
    double dk_i_minus_prev , dk_i_minus_curr , dk_i_minus_next,
           dk_i_plus_prev , dk_i_plus_curr , dk_i_plus_next, angle;

    dbgl_eulerspiral *spiral_i_min_1, *spiral_i;

    // ---------- extract i-th partial derivative ---------

    // 1 - sum delta_angle to angle 

    angle = c[i].tangent_angle_at(0);

    angle = angle + delta_angle; 
    if (angle >= 2*vnl_math::pi)
      angle -= 2*vnl_math::pi;

    // 2 - reconstruct intervals 

    spiral_i = new dbgl_eulerspiral(
        c.point_at_sample(i)->get_p(),
        angle,
        c.point_at_sample(i+1)->get_p(),
        c[i].tangent_angle_at(1));

    spiral_i_min_1 = new dbgl_eulerspiral(
        c.point_at_sample(i-1)->get_p(),
        c[i-1].tangent_angle_at(0),
        c.point_at_sample(i)->get_p(),
        angle);

    // 3 - compute dk_i_plus

    dk_i_plus_prev  = c[i-2].curvature_at(1) - spiral_i_min_1->curvature_at(0);
    dk_i_plus_curr  = spiral_i_min_1->curvature_at(1) - spiral_i->curvature_at(0);
    dk_i_plus_next  = spiral_i->curvature_at(1) - c[i+1].curvature_at(0);

    dk_i_plus_prev *= dk_i_plus_prev;
    dk_i_plus_curr *= dk_i_plus_curr;
    dk_i_plus_next *= dk_i_plus_next;

    delete spiral_i;
    delete spiral_i_min_1;

    // Repeat for dk_i_minus1 ------------------
    
    // 1 - subtract delta_angle to angle 

    angle = c[i].tangent_angle_at(0);

    angle = angle - delta_angle; 
    if (angle < 0)
      angle += 2*vnl_math::pi;

    // 2 - reconstruct intervals 

    spiral_i = new dbgl_eulerspiral(
        c.point_at_sample(i)->get_p(),
        angle,
        c.point_at_sample(i+1)->get_p(),
        c[i].tangent_angle_at(1));

    spiral_i_min_1 = new dbgl_eulerspiral(
        c.point_at_sample(i-1)->get_p(),
        c[i-1].tangent_angle_at(0),
        c.point_at_sample(i)->get_p(),
        angle);

    // 3 - compute dk_i_plus

    dk_i_minus_prev = c[i-2].curvature_at(1) - spiral_i_min_1->curvature_at(0);
    dk_i_minus_curr = spiral_i_min_1->curvature_at(1) - spiral_i->curvature_at(0);
    dk_i_minus_next = spiral_i->curvature_at(1) - c[i+1].curvature_at(0);

    dk_i_minus_prev *= dk_i_minus_prev;
    dk_i_minus_curr *= dk_i_minus_curr;
    dk_i_minus_next *= dk_i_minus_next;

    delete spiral_i;
    delete spiral_i_min_1;


    // 5 - compute grad[i]

    E_i_plus1 = E_i_minus1 = E - dksq[i-1] - dksq[i] - dksq[i+1];

    E_i_minus1 += dk_i_minus_prev + dk_i_minus_curr + dk_i_minus_next;
    E_i_plus1  += dk_i_plus_prev + dk_i_plus_curr + dk_i_plus_next;

    grad[i] = (E_i_plus1 - E_i_minus1)/(2*delta_angle);
  }

  /* do start and endpoints: for starting point, change tangent and analyse difference in
   * curvature with second interval; similar for the endpoint */
  
  // first sample (index 0)
  {
    double E_i_minus1, E_i_plus1;
    double dk_i_minus_next, dk_i_plus_next, angle;

    dbgl_eulerspiral* spiral_i;

    // ---------- extract i-th partial derivative ---------

    // 1 - sum delta_angle to angle 

    angle = c[0].tangent_angle_at(0);

    angle = angle + delta_angle; 
    if (angle >= 2*vnl_math::pi)
      angle -= 2*vnl_math::pi;

    // 2 - reconstruct intervals 

    spiral_i = new dbgl_eulerspiral(
        c.point_at_sample(0)->get_p(),
        angle,
        c.point_at_sample(1)->get_p(),
        c[0].tangent_angle_at(1));

    // 3 - compute dk_i_plus

    dk_i_plus_next   = spiral_i->curvature_at(1) - c[1].curvature_at(0);
    dk_i_plus_next *= dk_i_plus_next;

    delete spiral_i;

    // Repeat for dk_i_minus1 ------------------
    
    // 1 - subtract delta_angle to angle 

    angle = c[0].tangent_angle_at(0);

    angle = angle - delta_angle; 
    if (angle < 0)
      angle += 2*vnl_math::pi;

    // 2 - reconstruct intervals 

    spiral_i = new dbgl_eulerspiral(
        c.point_at_sample(0)->get_p(),
        angle,
        c.point_at_sample(1)->get_p(),
        c[0].tangent_angle_at(1));

    dk_i_minus_next   = spiral_i->curvature_at(1) - c[1].curvature_at(0);

    // 3 - compute dk_i_plus

    dk_i_minus_next *= dk_i_minus_next;

    delete spiral_i;


    // 5 - compute grad[i]

    E_i_plus1 = E_i_minus1 = E  - dksq[1];

    E_i_minus1 += dk_i_minus_next;
    E_i_plus1  += dk_i_plus_next;

    grad[0] = (E_i_plus1 - E_i_minus1)/(2*delta_angle);
  }

  { // second sample (index 1)
    unsigned i=1;
    double E_i_minus1, E_i_plus1;
    double  dk_i_minus_curr , dk_i_minus_next,
            dk_i_plus_curr , dk_i_plus_next, angle;

    dbgl_eulerspiral* spiral_i_min_1, *spiral_i;

    // ---------- extract i-th partial derivative ---------

    // 1 - sum delta_angle to angle 

    angle = c[i].tangent_angle_at(0);

    angle = angle + delta_angle; 
    if (angle >= 2*vnl_math::pi)
      angle -= 2*vnl_math::pi;

    // 2 - reconstruct intervals 

    spiral_i = new dbgl_eulerspiral(
        c.point_at_sample(i)->get_p(),
        angle,
        c.point_at_sample(i+1)->get_p(),
        c[i].tangent_angle_at(1));

    spiral_i_min_1 = new dbgl_eulerspiral(
        c.point_at_sample(i-1)->get_p(),
        c[i-1].tangent_angle_at(0),
        c.point_at_sample(i)->get_p(),
        angle);

    // 3 - compute dk_i_plus

    dk_i_plus_curr = spiral_i_min_1->curvature_at(1) - spiral_i->curvature_at(0);
    dk_i_plus_next  = spiral_i->curvature_at(1) - c[i+1].curvature_at(0);

    dk_i_plus_curr *= dk_i_plus_curr;
    dk_i_plus_next *= dk_i_plus_next;

    delete spiral_i;
    delete spiral_i_min_1;

    // Repeat for dk_i_minus1 ------------------
    
    // 1 - subtract delta_angle to angle 

    angle = c[i].tangent_angle_at(0);

    angle = angle - delta_angle; 
    if (angle < 0)
      angle += 2*vnl_math::pi;

    // 2 - reconstruct intervals 

    spiral_i = new dbgl_eulerspiral(
        c.point_at_sample(i)->get_p(),
        angle,
        c.point_at_sample(i+1)->get_p(),
        c[i].tangent_angle_at(1));

    spiral_i_min_1 = new dbgl_eulerspiral(
        c.point_at_sample(i-1)->get_p(),
        c[i-1].tangent_angle_at(0),
        c.point_at_sample(i)->get_p(),
        angle);

    // 3 - compute dk_i_plus

    dk_i_minus_curr = spiral_i_min_1->curvature_at(1) - spiral_i->curvature_at(0);
    dk_i_minus_next = spiral_i->curvature_at(1) - c[i+1].curvature_at(0);

    dk_i_minus_curr *= dk_i_minus_curr;
    dk_i_minus_next *= dk_i_minus_next;

    delete spiral_i;
    delete spiral_i_min_1;


    // 5 - compute grad[i]

    E_i_plus1 = E_i_minus1 = E - dksq[i] - dksq[i+1];

    E_i_minus1 += dk_i_minus_curr + dk_i_minus_next;
    E_i_plus1  += dk_i_plus_curr + dk_i_plus_next;

    grad[i] = (E_i_plus1 - E_i_minus1)/(2*delta_angle);
  }

  { // last sample
    unsigned i = dk.size()-1;
    double E_i_minus1, E_i_plus1;
    double dk_i_minus_prev,dk_i_plus_prev, angle;

    dbgl_eulerspiral* spiral_i_min_1;

    // ---------- extract i-th partial derivative ---------

    // 1 - sum delta_angle to angle 

    angle = c[i-1].tangent_angle_at(1);

    angle = angle + delta_angle; 
    if (angle >= 2*vnl_math::pi)
      angle -= 2*vnl_math::pi;

    // 2 - reconstruct intervals 

    spiral_i_min_1 = new dbgl_eulerspiral(
        c.point_at_sample(i-1)->get_p(),
        c[i-1].tangent_angle_at(0),
        c.point_at_sample(i)->get_p(),
        angle);

    // 3 - compute dk_i_plus

    dk_i_plus_prev = c[i-2].curvature_at(1) - spiral_i_min_1->curvature_at(0);
    dk_i_plus_prev *= dk_i_plus_prev;

    delete spiral_i_min_1;

    // Repeat for dk_i_minus1 ------------------
    
    // 1 - subtract delta_angle to angle 

    angle = c[i-1].tangent_angle_at(1);

    angle = angle - delta_angle; 
    if (angle < 0)
      angle += 2*vnl_math::pi;

    // 2 - reconstruct intervals 

    spiral_i_min_1 = new dbgl_eulerspiral(
        c.point_at_sample(i-1)->get_p(),
        c[i-1].tangent_angle_at(0),
        c.point_at_sample(i)->get_p(),
        angle);

    // 3 - compute dk_i_plus

    dk_i_minus_prev = c[i-2].curvature_at(1) - spiral_i_min_1->curvature_at(0);
    dk_i_minus_prev *= dk_i_minus_prev;

    delete spiral_i_min_1;


    // 5 - compute grad[i]

    E_i_plus1 = E_i_minus1 = E - dksq[i-1];

    E_i_minus1 += dk_i_minus_prev; 
    E_i_plus1  += dk_i_plus_prev;

    grad[i] = (E_i_plus1 - E_i_minus1)/(2*delta_angle);
  }

  { // last sample minus 1
    unsigned i = dk.size()-2;
    double E_i_minus1, E_i_plus1;
    double dk_i_minus_prev , dk_i_minus_curr ,
           dk_i_plus_prev , dk_i_plus_curr , angle;

    dbgl_eulerspiral *spiral_i_min_1, *spiral_i;

    // ---------- extract i-th partial derivative ---------

    // 1 - sum delta_angle to angle 

    angle = c[i].tangent_angle_at(0);

    angle = angle + delta_angle; 
    if (angle >= 2*vnl_math::pi)
      angle -= 2*vnl_math::pi;

    // 2 - reconstruct intervals 

    spiral_i = new dbgl_eulerspiral(
        c.point_at_sample(i)->get_p(),
        angle,
        c.point_at_sample(i+1)->get_p(),
        c[i].tangent_angle_at(1));

    spiral_i_min_1 = new dbgl_eulerspiral(
        c.point_at_sample(i-1)->get_p(),
        c[i-1].tangent_angle_at(0),
        c.point_at_sample(i)->get_p(),
        angle);

    // 3 - compute dk_i_plus

    dk_i_plus_prev = c[i-2].curvature_at(1) - spiral_i_min_1->curvature_at(0);
    dk_i_plus_curr = spiral_i_min_1->curvature_at(1) - spiral_i->curvature_at(0);

    dk_i_plus_prev *= dk_i_plus_prev;
    dk_i_plus_curr *= dk_i_plus_curr;

    delete spiral_i;
    delete spiral_i_min_1;

    // Repeat for dk_i_minus1 ------------------
    
    // 1 - subtract delta_angle to angle 

    angle = c[i].tangent_angle_at(0);

    angle = angle - delta_angle; 
    if (angle < 0)
      angle += 2*vnl_math::pi;

    // 2 - reconstruct intervals 

    spiral_i = new dbgl_eulerspiral(
        c.point_at_sample(i)->get_p(),
        angle,
        c.point_at_sample(i+1)->get_p(),
        c[i].tangent_angle_at(1));

    spiral_i_min_1 = new dbgl_eulerspiral(
        c.point_at_sample(i-1)->get_p(),
        c[i-1].tangent_angle_at(0),
        c.point_at_sample(i)->get_p(),
        angle);

    // 3 - compute dk_i_plus

    dk_i_minus_prev = c[i-2].curvature_at(1) - spiral_i_min_1->curvature_at(0);
    dk_i_minus_curr = spiral_i_min_1->curvature_at(1) - spiral_i->curvature_at(0);

    dk_i_minus_prev *= dk_i_minus_prev;
    dk_i_minus_curr *= dk_i_minus_curr;

    delete spiral_i;
    delete spiral_i_min_1;


    // 5 - compute grad[i]

    E_i_plus1 = E_i_minus1 = E - dksq[i-1] - dksq[i];

    E_i_minus1 += dk_i_minus_prev + dk_i_minus_curr;
    E_i_plus1  += dk_i_plus_prev + dk_i_plus_curr;

    grad[i] = (E_i_plus1 - E_i_minus1)/(2*delta_angle);
  }
}

//: Refine 3rd order intrpolation by a gradient descent on tangents, minimizing
// curvature difference between neighboring intervals at sample points.
// 
// Performs one step gradient descent.
//
// \param[in] psi  how far to move along gradient at each step.
// \param[in] n_iter number of iterations
// \param[in] delta_angle angular variation step to compute partial derivatives
//
//
void 
gradient_descent(
    dbsol_geno_curve_2d *cnew,
    const dbsol_geno_curve_2d &c,
    double delta_angle,
    double psi
    )
{

  assert(c.closed() == false);
  assert(c.order() == 3);
  
  vcl_vector<double> grad;
  vcl_vector<double> tangent;

  // compute gradient vector
  compute_tangent_gradient(c, delta_angle, grad);

  vcl_vector<vsol_point_2d_sptr> pts;

  pts.resize(grad.size());

  assert (grad.size() == c.size()+1);
  tangent.resize(grad.size());
  for (unsigned i=0; i<c.size(); ++i) {
    tangent[i] = c[i].tangent_angle_at(0) - psi*grad[i];
    
    if (!(fabs(psi*grad[i]) < vnl_math::pi/6)) {
      vcl_cout << "Warning: Interval" << i << vcl_endl <<
        "psi*grad[i] = " << psi*grad[i]/vnl_math::pi << "pi" <<
        vcl_endl;
    }

    if (tangent[i] < 0)
      tangent[i] += 2*vnl_math::pi;
    else if (tangent[i] >= 2*vnl_math::pi)
      tangent[i] -= 2*vnl_math::pi;

    pts[i] = c.point_at_sample(i);
  }
  tangent[c.size()] = c[c.size()-1].tangent_angle_at(1) - psi*grad[c.size()];
  pts[c.size()] = c.point_at_sample(c.size());

  // re-interpolate using new tangents

  dbsol_geno::interpolate3_from_tangents(cnew, pts, tangent, false);
}

//: Compute gradient of a smoothness energy on the eulerspiral-geno
// interpolation, relative to variation on point position along normal. This is used for
// gradient-descent smoothing of a curve given by interpolate3_approx.
//
// \param[in] delta_normal step to compute discrete partial derivatives
// \param[in] c geno curve
// \param[out] grad  gradient vector
void 
compute_positional_gradient(
    const dbsol_geno_curve_2d &c,
    double delta_normal,
    vcl_vector<double> &grad) 
{
  assert(delta_normal > 0);

  vcl_vector<double> dk; //:< differences of curvature at each sample
  vcl_vector<double> dksq; //: squared dk's

  dk.resize(c.size()+1,0);
  grad.resize(dk.size(),0);
  dksq.resize(dk.size(),0);

  double E=0, tolerance = 1e-13;
  for (unsigned i=1; i<c.size(); ++i) {
    dk[i] = c[i].curvature_at(0) - c[i-1].curvature_at(1);
    dksq[i] = dk[i]*(dk[i]);
    E += dksq[i];
    if (!(fabs(c[i].tangent_angle_at(0)- c[i-1].tangent_angle_at(1)) < tolerance)) {
      vcl_cout.precision(20);

      vcl_cout << "Warning; tangent error: \n" << 
        "Interval " << i << vcl_endl << 
        "c[i].tangent_angle_at(0) " << c[i].tangent_angle_at(0) << vcl_endl << 
        "c[i-1].tangent_angle_at(1) " << c[i-1].tangent_angle_at(1) << vcl_endl
        << "difference: " << fabs(c[i].tangent_angle_at(0)- c[i-1].tangent_angle_at(1)) 
        << vcl_endl;
    }
  }

  for (unsigned i=2; i<(dk.size() -2); ++i) {
    double E_i_plus1;
    double dk_i_plus_prev , dk_i_plus_curr , dk_i_plus_next,
           normal_x, normal_y;
    vsol_point_2d_sptr point;

    dbgl_eulerspiral *spiral_i_min_1, *spiral_i;

    // ---------- extract i-th partial derivative ---------

    // 1 - sum delta_normal to point

    get_normal(c, i, &normal_x, &normal_y);

    point = c.point_at_sample(i);
    point->set_x(point->x() + delta_normal*normal_x);
    point->set_y(point->y() + delta_normal*normal_y);

    // 2 - reconstruct intervals 

    spiral_i = new dbgl_eulerspiral(
        point->get_p(),
        c[i].tangent_angle_at(0),
        c.point_at_sample(i+1)->get_p(),
        c[i].tangent_angle_at(1));

    spiral_i_min_1 = new dbgl_eulerspiral(
        c.point_at_sample(i-1)->get_p(),
        c[i-1].tangent_angle_at(0),
        point->get_p(),
        c[i].tangent_angle_at(0));
    

    // 3 - compute dk_i_plus

    dk_i_plus_prev  = c[i-2].curvature_at(1) - spiral_i_min_1->curvature_at(0);
    dk_i_plus_curr  = spiral_i_min_1->curvature_at(1) - spiral_i->curvature_at(0);
    dk_i_plus_next  = spiral_i->curvature_at(1) - c[i+1].curvature_at(0);

    dk_i_plus_prev *= dk_i_plus_prev;
    dk_i_plus_curr *= dk_i_plus_curr;
    dk_i_plus_next *= dk_i_plus_next;

    delete spiral_i;
    delete spiral_i_min_1;


    // 4 - compute grad[i]

    E_i_plus1 = E - dksq[i-1] - dksq[i] - dksq[i+1];
    E_i_plus1 += dk_i_plus_prev + dk_i_plus_curr + dk_i_plus_next;

    grad[i] = (E_i_plus1 - E)/delta_normal;
  }

  /* do start and endpoints: for starting point, change tangent and analyse difference in
   * curvature with second interval; similar for the endpoint */
  
  // first sample (index 0)
  {
    double E_i_plus1;
    double dk_i_plus_next, 
           normal_x, normal_y;
    vsol_point_2d_sptr point;

    dbgl_eulerspiral* spiral_i;

    // ---------- extract i-th partial derivative ---------

    // 1 - sum delta_normal to point

    get_normal(c, 0, &normal_x, &normal_y);

    point = c.point_at_sample(0);
    point->set_x(point->x() + delta_normal*normal_x);
    point->set_y(point->y() + delta_normal*normal_y);

    // 2 - reconstruct intervals 

    spiral_i = new dbgl_eulerspiral(
        point->get_p(),
        c[0].tangent_angle_at(0),
        c.point_at_sample(1)->get_p(),
        c[0].tangent_angle_at(1));

    // 3 - compute dk_i_plus

    dk_i_plus_next  = spiral_i->curvature_at(1) - c[1].curvature_at(0);
    dk_i_plus_next *= dk_i_plus_next;

    delete spiral_i;

    // 5 - compute grad[i]

    E_i_plus1  = E  - dksq[1];
    E_i_plus1 += dk_i_plus_next;

    grad[0] = (E_i_plus1-E)/(delta_normal);
  }

  { // second sample (index 1)
    unsigned i=1;
    double E_i_plus1;
    double dk_i_plus_curr , dk_i_plus_next, 
           normal_x, normal_y;
    vsol_point_2d_sptr point;

    dbgl_eulerspiral *spiral_i_min_1, *spiral_i;

    // ---------- extract i-th partial derivative ---------

    // 1 - sum delta_normal to point

    get_normal(c, i, &normal_x, &normal_y);

    point = c.point_at_sample(i);
    point->set_x(point->x() + delta_normal*normal_x);
    point->set_y(point->y() + delta_normal*normal_y);

    // 2 - reconstruct intervals 

    spiral_i = new dbgl_eulerspiral(
        point->get_p(),
        c[i].tangent_angle_at(0),
        c.point_at_sample(i+1)->get_p(),
        c[i].tangent_angle_at(1));

    spiral_i_min_1 = new dbgl_eulerspiral(
        c.point_at_sample(i-1)->get_p(),
        c[i-1].tangent_angle_at(0),
        point->get_p(),
        c[i].tangent_angle_at(0));
    

    // 3 - compute dk_i_plus

    dk_i_plus_curr  = spiral_i_min_1->curvature_at(1) - spiral_i->curvature_at(0);
    dk_i_plus_next  = spiral_i->curvature_at(1) - c[i+1].curvature_at(0);

    dk_i_plus_curr *= dk_i_plus_curr;
    dk_i_plus_next *= dk_i_plus_next;

    delete spiral_i;
    delete spiral_i_min_1;


    // 4 - compute grad[i]

    E_i_plus1 = E - dksq[i] - dksq[i+1];
    E_i_plus1 += dk_i_plus_curr + dk_i_plus_next;

    grad[i] = (E_i_plus1-E)/delta_normal;
  }

  { // last sample
    unsigned i = dk.size()-1;
    double E_i_plus1;
    double dk_i_plus_prev , 
           normal_x, normal_y;
    vsol_point_2d_sptr point;

    dbgl_eulerspiral *spiral_i_min_1;

    // ---------- extract i-th partial derivative ---------

    // 1 - sum delta_normal to point

    get_normal(c, i, &normal_x, &normal_y);

    point = c.point_at_sample(i);
    point->set_x(point->x() + delta_normal*normal_x);
    point->set_y(point->y() + delta_normal*normal_y);

    // 2 - reconstruct intervals 

    spiral_i_min_1 = new dbgl_eulerspiral(
        c.point_at_sample(i-1)->get_p(),
        c[i-1].tangent_angle_at(0),
        point->get_p(),
        c[i-1].tangent_angle_at(1));

    // 3 - compute dk_i_plus

    dk_i_plus_prev  = c[i-2].curvature_at(1) - spiral_i_min_1->curvature_at(0);

    dk_i_plus_prev *= dk_i_plus_prev;

    delete spiral_i_min_1;


    // 4 - compute grad[i]

    E_i_plus1 = E - dksq[i-1];
    E_i_plus1 += dk_i_plus_prev; 

    grad[i] = (E_i_plus1-E)/delta_normal;
  }

  { // last sample minus 1
    unsigned i = dk.size()-2;
    double E_i_plus1;
    double dk_i_plus_prev , dk_i_plus_curr, 
           normal_x, normal_y;
    vgl_vector_2d<double> tgt;
    vsol_point_2d_sptr point;

    dbgl_eulerspiral *spiral_i_min_1, *spiral_i;

    // ---------- extract i-th partial derivative ---------

    // 1 - sum delta_normal to point

    get_normal(c, i, &normal_x, &normal_y);

    point = c.point_at_sample(i);
    point->set_x(point->x() + delta_normal*normal_x);
    point->set_y(point->y() + delta_normal*normal_y);

    // 2 - reconstruct intervals 

    spiral_i = new dbgl_eulerspiral(
        point->get_p(),
        c[i].tangent_angle_at(0),
        c.point_at_sample(i+1)->get_p(),
        c[i].tangent_angle_at(1));

    spiral_i_min_1 = new dbgl_eulerspiral(
        c.point_at_sample(i-1)->get_p(),
        c[i-1].tangent_angle_at(0),
        point->get_p(),
        c[i].tangent_angle_at(0));
    

    // 3 - compute dk_i_plus

    dk_i_plus_prev  = c[i-2].curvature_at(1) - spiral_i_min_1->curvature_at(0);
    dk_i_plus_curr  = spiral_i_min_1->curvature_at(1) - spiral_i->curvature_at(0);

    dk_i_plus_prev *= dk_i_plus_prev;
    dk_i_plus_curr *= dk_i_plus_curr;

    delete spiral_i;
    delete spiral_i_min_1;


    // 4 - compute grad[i]

    E_i_plus1 = E - dksq[i-1] - dksq[i];
    E_i_plus1 += dk_i_plus_prev + dk_i_plus_curr;

    grad[i] = (E_i_plus1-E)/delta_normal;
  }
}

//: Refine 3rd order intrpolation by a gradient descent on tangents, minimizing
// curvature difference between neighboring intervals at sample points.
// 
// Performs one step gradient descent.
//
// \param[in] psi  how far to move along gradient at each step.
// \param[in] n_iter number of iterations
// \param[in] delta_norm variation along normal to estimate partial derivatives
//
//
void 
gradient_descent_positional(
    vcl_vector<vsol_point_2d_sptr> &pts,
    const dbsol_geno_curve_2d &c,
    double delta_norm,
    double psi,
    vcl_vector<double> &grad
    )
{

  assert(c.closed() == false);
  assert(c.order() == 3 || c.order() == 2);

  // compute gradient vector
  if (c.order() == 3)
    compute_positional_gradient(c, delta_norm, grad);
  else
    compute_positional_gradient_arc_dt(c, delta_norm, grad);
//    compute_positional_gradient_arc(c, delta_norm, grad);

  pts.clear();
  pts.resize(grad.size(),0);
  double normal_x, normal_y;

   // NORMALIZATION
  double max = vcl_fabs(grad[0]);
  for (unsigned i=1; i<=c.size(); ++i) {
    if (vcl_fabs(grad[i]) > max)
      max = vcl_fabs(grad[i]);
  }

  // normalize gradient to [-1,1] range
//  min = vcl_fabs(min);
//  for (unsigned i=0; i<=c.size(); ++i) {
//    grad[i] =  2*(grad[i] - min)/(max - min) + 1;
//  }

  // normalize gradient
  if (vcl_fabs(max) > 1e-10 ) {
    for (unsigned i=0; i<=c.size(); ++i) {
      grad[i] /=  max;
    }
  } else {
    vcl_cerr << "Warning: grad == 0" << vcl_endl;
  }

  assert (grad.size() == c.size()+1);

  // Evolve points
  for (unsigned i=0; i<c.size(); ++i) {

    /* Compute normal vector */
//    get_normal(c, i, &normal_x, &normal_y);
    get_normal_arc(c, i, &normal_x, &normal_y);

    vsol_point_2d_sptr point = c.point_at_sample(i);
//    if (i ==4) {
      point->set_x(point->x() - psi*grad[i]*normal_x);
      point->set_y(point->y() - psi*grad[i]*normal_y);
//    }
      
    pts[i] = point;
  }

  // --- Do last point ---
//  get_normal(c, c.size(), &normal_x, &normal_y);
  get_normal_arc(c, c.size(), &normal_x, &normal_y);
  vsol_point_2d_sptr point = c.point_at_sample(c.size());

  point->set_x(point->x() - psi*grad[c.size()]*normal_x);
  point->set_y(point->y() - psi*grad[c.size()]*normal_y);

  pts[c.size()] = point;
}

//: refines tangents by iterating on gradient descent steps
// It will delete the input; and the refined version will be pointed by "refined"
// 
void
refine_tangents(dbsol_geno_curve_2d **input, dbsol_geno_curve_2d **refined,double delta_angle,double psi,unsigned n_iter)
{
  dbsol_geno_curve_2d *pgc_refined, *pgc;

  pgc_refined = *input;
  pgc = 0;

  for (unsigned i=0; i<n_iter; ++i) {
    if (pgc) 
      delete pgc;

    pgc = pgc_refined;
    pgc_refined = new dbsol_geno_curve_2d();

    gradient_descent(pgc_refined,*pgc,delta_angle,psi);
  }

  *input = pgc;
  *refined = pgc_refined;
}

//
//: Compute gradient of a smoothness energy on the arc-GENO
// interpolation, relative to variation on point position along normal. This is used for
// gradient-descent smoothing of a curve given by dbsol_geno::interpolate
//
// \param[in] delta_normal step to compute discrete partial derivatives
// \param[in] c geno curve
// \param[out] grad  gradient vector
void 
compute_positional_gradient_arc(
    const dbsol_geno_curve_2d &c,
    double delta_normal,
    vcl_vector<double> &grad) 
{
  bool fwd; // is arc fwd or not

  assert(delta_normal > 0);
  assert(c.order() == 2);
  assert(c.size() >= 6);

  vcl_vector<double> dk; //:< differences of curvature at each sample
  vcl_vector<double> dksq; //: squared dk's
  vcl_vector<double> dt; //:< differences of tangent at each sample
  vcl_vector<double> dtsq; //: squared dt's

  const unsigned nsamples = c.size()+1;

  dk.resize(nsamples,0);
  dksq.resize(nsamples,0);
  dt.resize(nsamples,0);
  dtsq.resize(nsamples,0);
  grad.resize(nsamples,0);

  double E=0;
  for (unsigned i=1; i<c.size(); ++i) {
    dk[i] = c[i].curvature_at(0) - c[i-1].curvature_at(1);
    dksq[i] = dk[i]*(dk[i]);

    dt[i] = mw_util::angle_difference(c[i].tangent_angle_at(0), c[i-1].tangent_angle_at(1));
    dtsq[i] = dt[i]*(dt[i]);

    E += dksq[i] + dtsq[i];
  }

  for (unsigned i=2; i < nsamples -2; ++i) {
    double E_i_plus1;
    double normal_x, normal_y;
    vsol_point_2d_sptr point;

    dbgl_arc *arc[4];

    // ---------- extract i-th partial derivative ---------

    // 1 - sum delta_normal to point

    get_normal_arc(c, i, &normal_x, &normal_y);

    point = c.point_at_sample(i);
    point->set_x(point->x() + delta_normal*normal_x);
    point->set_y(point->y() + delta_normal*normal_y);

    // 2 - reconstruct 4 intervals 

    arc[1] = dbsol_geno::interpolate_interval(
        c.point_at_sample(i-2)->get_p(),
        c.point_at_sample(i-1)->get_p(),
        point->get_p(),
        c.point_at_sample(i+1)->get_p(),&fwd);

    arc[2] = dbsol_geno::interpolate_interval(
        c.point_at_sample(i-1)->get_p(),
        point->get_p(),
        c.point_at_sample(i+1)->get_p(),
        c.point_at_sample(i+2)->get_p(),&fwd);

    // 1st interval
    if (i == 2) {
      arc[0] = new dbgl_arc(
        c.point_at_sample(i-2)->get_p(),
        c.point_at_sample(i-1)->get_p(),
        point->get_p());
      dk[0] = dt[0] = 0;
    } else {
      arc[0] = dbsol_geno::interpolate_interval(
        c.point_at_sample(i-3)->get_p(),
        c.point_at_sample(i-2)->get_p(),
        c.point_at_sample(i-1)->get_p(),
        point->get_p(), &fwd);
      dk[i-2] = c[i-3].curvature_at(1) - arc[0]->curvature_at(0);
      dt[i-2] = mw_util::angle_difference(
                c[i-3].tangent_angle_at(1), arc[0]->tangent_angle_at(0));
    }

    // last interval
    if (i == dk.size()-3) {
      arc[3] = new dbgl_arc(
        point->get_p(),
        c.point_at_sample(i+1)->get_p(),
        c.point_at_sample(i+2)->get_p());
      dk[i+1] = dt[i+1] = 0;
    } else {
      arc[3] = dbsol_geno::interpolate_interval(
        point->get_p(),
        c.point_at_sample(i+1)->get_p(),
        c.point_at_sample(i+2)->get_p(),
        c.point_at_sample(i+3)->get_p(),
        &fwd);
      dk[i+2] = arc[3]->curvature_at(1) - c[i+2].curvature_at(0);
      dt[i+2] = mw_util::angle_difference(
                arc[3]->tangent_angle_at(1), c[i+2].tangent_angle_at(0));
    }

    // 3 - compute dk_i_plus


    // note we are reusing dk vector here 
    dk[i-1] = arc[0]->curvature_at(1) - arc[1]->curvature_at(0);
    dk[i] = arc[1]->curvature_at(1) - arc[2]->curvature_at(0);
    dk[i+1] = arc[2]->curvature_at(1) - arc[3]->curvature_at(0);

    dt[i-1] = mw_util::angle_difference(
        arc[0]->tangent_angle_at(1), arc[1]->tangent_angle_at(0));
    dt[i] = mw_util::angle_difference(
        arc[1]->tangent_angle_at(1), arc[2]->tangent_angle_at(0));
    dt[i+1] = mw_util::angle_difference(
        arc[2]->tangent_angle_at(1), arc[3]->tangent_angle_at(0));


    E_i_plus1 = E;
    unsigned jj;
    for (jj=i-2; jj<i+2; ++jj) {
      delete arc[jj - i + 2];
      E_i_plus1 += dk[jj]*dk[jj] -dksq[jj] +dt[jj]*dt[jj] -dtsq[jj];
    }
    E_i_plus1 += dk[jj]*dk[jj] -dksq[jj] +dt[jj]*dt[jj] -dtsq[jj];

    grad[i] = (E_i_plus1 - E)/delta_normal;
  }


  /* do start and endpoints: for starting point, change tangent and analyse difference in
   * curvature with second interval; similar for the endpoint */
  
  // first sample (index 0)
  {
    double E_i_plus1;
    double normal_x, normal_y;
    vsol_point_2d_sptr point;

    dbgl_arc *arc;

    // ---------- extract i-th partial derivative ---------

    // 1 - sum delta_normal to point

    get_normal_arc(c, 0, &normal_x, &normal_y);

    point = c.point_at_sample(0);
    point->set_x(point->x() + delta_normal*normal_x);
    point->set_y(point->y() + delta_normal*normal_y);

    // 2 - reconstruct 1 interval

    arc = new dbgl_arc(
      point->get_p(),
      c.point_at_sample(1)->get_p(),
      c.point_at_sample(2)->get_p());

    dk[1] = c[1].curvature_at(0) - arc->curvature_at(1);
    dt[1] = mw_util::angle_difference(
        c[1].tangent_angle_at(0), arc->tangent_angle_at(1));

    E_i_plus1 = E;
    delete arc;

    E_i_plus1 += dk[1]*dk[1] -dksq[1] +dt[1]*dt[1] -dtsq[1];

    grad[0] = (E_i_plus1 - E)/delta_normal;
  }

  { // second sample (index 1)
    unsigned i = 1;
    double E_i_plus1;
    double normal_x, normal_y;
    vsol_point_2d_sptr point;

    dbgl_arc *arc[4];

    // ---------- extract i-th partial derivative ---------

    // 1 - sum delta_normal to point

    get_normal_arc(c, i, &normal_x, &normal_y);

    point = c.point_at_sample(i);
    point->set_x(point->x() + delta_normal*normal_x);
    point->set_y(point->y() + delta_normal*normal_y);

    // 2 - reconstruct 3 intervals 

    arc[1] = new dbgl_arc(
        c.point_at_sample(i-1)->get_p(),
        point->get_p(),
        c.point_at_sample(i+1)->get_p()
      );

    arc[2] = dbsol_geno::interpolate_interval(
        c.point_at_sample(i-1)->get_p(),
        point->get_p(),
        c.point_at_sample(i+1)->get_p(),
        c.point_at_sample(i+2)->get_p(),&fwd);

    // last interval
    if (i == dk.size()-3) {
      arc[3] = new dbgl_arc(
        point->get_p(),
        c.point_at_sample(i+1)->get_p(),
        c.point_at_sample(i+2)->get_p());
      dk[i+1] = dt[i+1] = 0;
    } else {
      arc[3] = dbsol_geno::interpolate_interval(
        point->get_p(),
        c.point_at_sample(i+1)->get_p(),
        c.point_at_sample(i+2)->get_p(),
        c.point_at_sample(i+3)->get_p(),
        &fwd);
      dk[i+2] = arc[3]->curvature_at(1) - c[i+2].curvature_at(0);
      dt[i+2] = mw_util::angle_difference(
                arc[3]->tangent_angle_at(1), c[i+2].tangent_angle_at(0));
    }

    // 3 - compute dk_i_plus


    // note we are reusing dk vector here 
    dk[i-1] = 0;
    dk[i]   = arc[1]->curvature_at(1) - arc[2]->curvature_at(0);
    dk[i+1] = arc[2]->curvature_at(1) - arc[3]->curvature_at(0);

    dt[i-1] = 0;
    dt[i] = mw_util::angle_difference(
        arc[1]->tangent_angle_at(1), arc[2]->tangent_angle_at(0));
    dt[i+1] = mw_util::angle_difference(
        arc[2]->tangent_angle_at(1), arc[3]->tangent_angle_at(0));

    E_i_plus1 = E;

    unsigned jj;
    for (jj=i-1; jj<i+2; ++jj) {
      delete arc[jj - i + 2];
      E_i_plus1 += dk[jj]*dk[jj] -dksq[jj] +dt[jj]*dt[jj] -dtsq[jj];
    }
    E_i_plus1 += dk[jj]*dk[jj] -dksq[jj] +dt[jj]*dt[jj] -dtsq[jj];

    grad[i] = (E_i_plus1 - E)/delta_normal;
  }

  { // last sample
    unsigned i = dk.size()-1;
    double E_i_plus1;
    double normal_x, normal_y;
    vsol_point_2d_sptr point;

    dbgl_arc *arc;

    // ---------- extract i-th partial derivative ---------

    // 1 - sum delta_normal to point

    get_normal_arc(c, i, &normal_x, &normal_y);

    point = c.point_at_sample(i);
    point->set_x(point->x() + delta_normal*normal_x);
    point->set_y(point->y() + delta_normal*normal_y);

    // 2 - reconstruct interval

    arc = new dbgl_arc(
      c.point_at_sample(i-1)->get_p(),
      point->get_p(),
      c.point_at_sample(i-2)->get_p()
      );

    // 3 - compute dk_i_plus


    // note we are reusing dk vector here 
    dk[i-1] = arc->curvature_at(0) - c[i-2].curvature_at(1);

    dt[i-1] = mw_util::angle_difference(
        arc->tangent_angle_at(0), c[i-2].tangent_angle_at(1));

    E_i_plus1 = E;

    delete arc;

    E_i_plus1 += dk[i-1]*dk[i-1] -dksq[i-1] +dt[i-1]*dt[i-1] -dtsq[i-1];

    grad[i] = (E_i_plus1 - E)/delta_normal;
  }

  { // last sample minus 1
    unsigned i = dk.size()-2;

    double E_i_plus1;
    double normal_x, normal_y;
    vsol_point_2d_sptr point;

    dbgl_arc *arc[4];

    // ---------- extract i-th partial derivative ---------

    // 1 - sum delta_normal to point

    get_normal_arc(c, i, &normal_x, &normal_y);

    point = c.point_at_sample(i);
    point->set_x(point->x() + delta_normal*normal_x);
    point->set_y(point->y() + delta_normal*normal_y);

    // 2 - reconstruct 4 intervals 

    arc[1] = dbsol_geno::interpolate_interval(
        c.point_at_sample(i-2)->get_p(),
        c.point_at_sample(i-1)->get_p(),
        point->get_p(),
        c.point_at_sample(i+1)->get_p(),&fwd);

    arc[2] = new dbgl_arc(
        point->get_p(),
        c.point_at_sample(i+1)->get_p(),
        c.point_at_sample(i-1)->get_p());

    // 1st interval
    if (i == 2) {
      arc[0] = new dbgl_arc(
        c.point_at_sample(i-2)->get_p(),
        c.point_at_sample(i-1)->get_p(),
        point->get_p());
      dk[0] = dt[0] = 0;
    } else {
      arc[0] = dbsol_geno::interpolate_interval(
        c.point_at_sample(i-3)->get_p(),
        c.point_at_sample(i-2)->get_p(),
        c.point_at_sample(i-1)->get_p(),
        point->get_p(), &fwd);
      dk[i-2] = c[i-3].curvature_at(1) - arc[0]->curvature_at(0);
      dt[i-2] = mw_util::angle_difference(
                c[i-3].tangent_angle_at(1), arc[0]->tangent_angle_at(0));
    }

    // 3 - compute dk_i_plus


    // note we are reusing dk vector here 
    dk[i-1] = arc[0]->curvature_at(1) - arc[1]->curvature_at(0);
    dk[i] = arc[1]->curvature_at(1) - arc[2]->curvature_at(0);

    dt[i-1] = mw_util::angle_difference(
        arc[0]->tangent_angle_at(1), arc[1]->tangent_angle_at(0));
    dt[i] = mw_util::angle_difference(
        arc[1]->tangent_angle_at(1), arc[2]->tangent_angle_at(0));

    E_i_plus1 = E;

    unsigned jj;
    for (jj=i-2; jj<i+1; ++jj) {
      delete arc[jj - i + 2];
      E_i_plus1 += dk[jj]*dk[jj] -dksq[jj] +dt[jj]*dt[jj] -dtsq[jj];
    }

    grad[i] = (E_i_plus1 - E)/delta_normal;
  }
}

//: Given a new sample at i, reinterpolate necessary intervals and return new
// energy.
// For use by compute_positional_gradient_arc_dt
// i == 2 to N-3
//
double energy_dt_middlepts(
    const dbsol_geno_curve_2d &c,
    unsigned i,
    const vsol_point_2d_sptr & point, // new point at sample i
    vcl_vector<double> &dt, // not read; only written and used as scrap
    const vcl_vector<double> &dtsq, 
    vcl_vector<double> &dk, // not read; only written and used as scrap
    const vcl_vector<double> &,//dksq,
    double E_ini 
    )
{
    dbgl_arc *arc[4];
    bool fwd; // is arc fwd or not
    double nsamples = c.size()+1;

    // 2 - reconstruct 4 intervals 

    arc[1] = dbsol_geno::interpolate_interval(
        c.point_at_sample(i-2)->get_p(),
        c.point_at_sample(i-1)->get_p(),
        point->get_p(),
        c.point_at_sample(i+1)->get_p(),&fwd);

    arc[2] = dbsol_geno::interpolate_interval(
        c.point_at_sample(i-1)->get_p(),
        point->get_p(),
        c.point_at_sample(i+1)->get_p(),
        c.point_at_sample(i+2)->get_p(),&fwd);

    // 1st interval
    if (i == 2) {
      arc[0] = new dbgl_arc(
        c.point_at_sample(i-2)->get_p(),
        c.point_at_sample(i-1)->get_p(),
        point->get_p());
      dk[0] = dt[0] = 0;
    } else {
      arc[0] = dbsol_geno::interpolate_interval(
        c.point_at_sample(i-3)->get_p(),
        c.point_at_sample(i-2)->get_p(),
        c.point_at_sample(i-1)->get_p(),
        point->get_p(), &fwd);
      dk[i-2] = c[i-3].curvature_at(1) - arc[0]->curvature_at(0);
      dt[i-2] = mw_util::angle_difference(
                c[i-3].tangent_angle_at(1), arc[0]->tangent_angle_at(0));
    }

    // last interval
    if (i == nsamples-3) {
      arc[3] = new dbgl_arc(
        point->get_p(),
        c.point_at_sample(i+1)->get_p(),
        c.point_at_sample(i+2)->get_p());
      dk[i+1] = dt[i+1] = 0;
    } else {
      arc[3] = dbsol_geno::interpolate_interval(
        point->get_p(),
        c.point_at_sample(i+1)->get_p(),
        c.point_at_sample(i+2)->get_p(),
        c.point_at_sample(i+3)->get_p(),
        &fwd);
      dk[i+2] = arc[3]->curvature_at(1) - c[i+2].curvature_at(0);
      dt[i+2] = mw_util::angle_difference(
                arc[3]->tangent_angle_at(1), c[i+2].tangent_angle_at(0));
    }

    // 3 - compute dk_i_plus


    // note we are reusing dk vector here 
    dk[i-1] = arc[0]->curvature_at(1) - arc[1]->curvature_at(0);
    dk[i] = arc[1]->curvature_at(1) - arc[2]->curvature_at(0);
    dk[i+1] = arc[2]->curvature_at(1) - arc[3]->curvature_at(0);

    dt[i-1] = mw_util::angle_difference(
        arc[0]->tangent_angle_at(1), arc[1]->tangent_angle_at(0));
    dt[i] = mw_util::angle_difference(
        arc[1]->tangent_angle_at(1), arc[2]->tangent_angle_at(0));
    dt[i+1] = mw_util::angle_difference(
        arc[2]->tangent_angle_at(1), arc[3]->tangent_angle_at(0));

    double E_new = E_ini;
    unsigned jj;
    for (jj=i-2; jj<i+2; ++jj) {
      delete arc[jj - i + 2];
      E_new += +dt[jj]*dt[jj] -dtsq[jj];
    }
    E_new += +dt[jj]*dt[jj] -dtsq[jj];

    return E_new;
}

//: Given a sample at 0, reinterpolate necessary intervals and return new
// energy.
// For use by compute_positional_gradient_arc_dt
//
double energy_dt_sample0(
    const dbsol_geno_curve_2d &c,
    const vsol_point_2d_sptr & point, // new point at sample 0
    const vcl_vector<double> &dtsq, 
    const vcl_vector<double> &,//dksq,
    double E_ini 
    )
{
    dbgl_arc *arc;
    double dk,dt;

    // 2 - reconstruct 1 interval

    arc = new dbgl_arc(
      point->get_p(),
      c.point_at_sample(1)->get_p(),
      c.point_at_sample(2)->get_p());

    dk = c[1].curvature_at(0) - arc->curvature_at(1);
    dt = mw_util::angle_difference(
        c[1].tangent_angle_at(0), arc->tangent_angle_at(1));

    delete arc;

    return E_ini -dtsq[1] +dt*dt;
}

//: Given a sample at 1, reinterpolate necessary intervals and return new
// energy.
// For use by compute_positional_gradient_arc_dt
//
double energy_dt_sample1(
    const dbsol_geno_curve_2d &c,
    const vsol_point_2d_sptr & point, // new point at sample 0
    vcl_vector<double> &dt, // not read; only written and used as scrap
    const vcl_vector<double> &dtsq, 
    vcl_vector<double> &dk, // not read; only written and used as scrap
    const vcl_vector<double> &,//dksq,
    double E_ini 
    )
{
    dbgl_arc *arc[4];
    bool fwd; // is arc fwd or not
    double nsamples = c.size()+1;
    unsigned i = 1;

    // reconstruct 3 intervals 

    arc[1] = new dbgl_arc(
        c.point_at_sample(i-1)->get_p(),
        point->get_p(),
        c.point_at_sample(i+1)->get_p()
      );

    arc[2] = dbsol_geno::interpolate_interval(
        c.point_at_sample(i-1)->get_p(),
        point->get_p(),
        c.point_at_sample(i+1)->get_p(),
        c.point_at_sample(i+2)->get_p(),&fwd);

    // last interval
    if (i == nsamples-3) {
      arc[3] = new dbgl_arc(
        point->get_p(),
        c.point_at_sample(i+1)->get_p(),
        c.point_at_sample(i+2)->get_p());
      dk[i+1] = dt[i+1] = 0;
    } else {
      arc[3] = dbsol_geno::interpolate_interval(
        point->get_p(),
        c.point_at_sample(i+1)->get_p(),
        c.point_at_sample(i+2)->get_p(),
        c.point_at_sample(i+3)->get_p(),
        &fwd);
      dk[i+2] = arc[3]->curvature_at(1) - c[i+2].curvature_at(0);
      dt[i+2] = mw_util::angle_difference(
                arc[3]->tangent_angle_at(1), c[i+2].tangent_angle_at(0));
    }

    // 3 - compute dk_i_plus


    // note we are reusing dk vector here 
    dk[i-1] = 0;
    dk[i]   = arc[1]->curvature_at(1) - arc[2]->curvature_at(0);
    dk[i+1] = arc[2]->curvature_at(1) - arc[3]->curvature_at(0);

    dt[i-1] = 0;
    dt[i] = mw_util::angle_difference(
        arc[1]->tangent_angle_at(1), arc[2]->tangent_angle_at(0));
    dt[i+1] = mw_util::angle_difference(
        arc[2]->tangent_angle_at(1), arc[3]->tangent_angle_at(0));

    double E_new= E_ini;

    unsigned jj;
    for (jj=i-1; jj<i+2; ++jj) {
      delete arc[jj - i + 2];
      E_new += +dt[jj]*dt[jj] -dtsq[jj];
    }
    E_new += +dt[jj]*dt[jj] -dtsq[jj];

    return E_new;
}

//: Given new point at last sample, reinterpolate necessary intervals and return
// new energy.
// For use by compute_positional_gradient_arc_dt
//
double energy_dt_sample_last(
    const dbsol_geno_curve_2d &c,
    const vsol_point_2d_sptr & point, // new point at sample 0
    const vcl_vector<double> &dtsq, 
    const vcl_vector<double> &,//dksq,
    double E_ini 
    )
{
    dbgl_arc *arc;
    unsigned i = c.size(); // nsamples-1
    double dk,dt;

    // 2 - reconstruct interval

    arc = new dbgl_arc(
      c.point_at_sample(i-1)->get_p(),
      point->get_p(),
      c.point_at_sample(i-2)->get_p()
      );

    // 3 - compute dk_i_plus


    // note we are reusing dk vector here 
    dk = arc->curvature_at(0) - c[i-2].curvature_at(1);

    dt = mw_util::angle_difference(
        arc->tangent_angle_at(0), c[i-2].tangent_angle_at(1));

    delete arc;

    return E_ini -dtsq[i-1] + dt*dt ;
}

//: Given a sample at 1, reinterpolate necessary intervals and return new
// energy.
// For use by compute_positional_gradient_arc_dt
//
double energy_dt_sample_before_last(
    const dbsol_geno_curve_2d &c,
    const vsol_point_2d_sptr & point, // new point at sample 0
    vcl_vector<double> &dt, // not read; only written and used as scrap
    const vcl_vector<double> &dtsq, 
    vcl_vector<double> &dk, // not read; only written and used as scrap
    const vcl_vector<double> &,//dksq,
    double E_ini 
    )
{
    dbgl_arc *arc[4];
    bool fwd; // is arc fwd or not
    unsigned nsamples = c.size()+1;
    unsigned i = nsamples-2;

    // 2 - reconstruct 4 intervals 

    arc[1] = dbsol_geno::interpolate_interval(
        c.point_at_sample(i-2)->get_p(),
        c.point_at_sample(i-1)->get_p(),
        point->get_p(),
        c.point_at_sample(i+1)->get_p(),&fwd);

    arc[2] = new dbgl_arc(
        point->get_p(),
        c.point_at_sample(i+1)->get_p(),
        c.point_at_sample(i-1)->get_p());

    // 1st interval
    if (i == 2) {
      arc[0] = new dbgl_arc(
        c.point_at_sample(i-2)->get_p(),
        c.point_at_sample(i-1)->get_p(),
        point->get_p());
      dk[0] = dt[0] = 0;
    } else {
      arc[0] = dbsol_geno::interpolate_interval(
        c.point_at_sample(i-3)->get_p(),
        c.point_at_sample(i-2)->get_p(),
        c.point_at_sample(i-1)->get_p(),
        point->get_p(), &fwd);
      dk[i-2] = c[i-3].curvature_at(1) - arc[0]->curvature_at(0);
      dt[i-2] = mw_util::angle_difference(
                c[i-3].tangent_angle_at(1), arc[0]->tangent_angle_at(0));
    }

    // 3 - compute dk_i_plus


    // note we are reusing dk vector here 
    dk[i-1] = arc[0]->curvature_at(1) - arc[1]->curvature_at(0);
    dk[i] = arc[1]->curvature_at(1) - arc[2]->curvature_at(0);

    dt[i-1] = mw_util::angle_difference(
        arc[0]->tangent_angle_at(1), arc[1]->tangent_angle_at(0));
    dt[i] = mw_util::angle_difference(
        arc[1]->tangent_angle_at(1), arc[2]->tangent_angle_at(0));

    double E_new = E_ini;
    unsigned jj;
    for (jj=i-2; jj<i+1; ++jj) {
      delete arc[jj - i + 2];
      E_new += +dt[jj]*dt[jj] -dtsq[jj];
    }

    return E_new;
}

//
//: Compute gradient of a smoothness energy on the arc-GENO
// interpolation, relative to variation on point position along normal. This is used for
// gradient-descent smoothing of a curve given by dbsol_geno::interpolate
//
// \param[in] delta_normal step to compute discrete partial derivatives
// \param[in] c geno curve
// \param[out] grad  gradient vector
void 
compute_positional_gradient_arc_dt(
    const dbsol_geno_curve_2d &c,
    double delta_normal,
    vcl_vector<double> &grad) 
{

  double energy_dt_middlepts(
      const dbsol_geno_curve_2d &c,
      unsigned i,
      const vsol_point_2d_sptr & newpoint,
      vcl_vector<double> &dt, // not read; only written and used as scrap
      const vcl_vector<double> &dtsq, 
      vcl_vector<double> &dk, // not read; only written and used as scrap
      const vcl_vector<double> &,//dksq,
      double E_ini 
      );
  double energy_dt_sample0(
      const dbsol_geno_curve_2d &c,
      const vsol_point_2d_sptr & point, // new point at sample 0
      const vcl_vector<double> &dtsq, 
      const vcl_vector<double> &,//dksq,
      double E_ini 
      );
  double energy_dt_sample1(
      const dbsol_geno_curve_2d &c,
      const vsol_point_2d_sptr & point, // new point at sample 0
      vcl_vector<double> &dt, // not read; only written and used as scrap
      const vcl_vector<double> &dtsq, 
      vcl_vector<double> &dk, // not read; only written and used as scrap
      const vcl_vector<double> &,//dksq,
      double E_ini 
      );
  double energy_dt_sample_last(
      const dbsol_geno_curve_2d &c,
      const vsol_point_2d_sptr & point, // new point at sample 0
      const vcl_vector<double> &dtsq, 
      const vcl_vector<double> &,//dksq,
      double E_ini 
      );
  double energy_dt_sample_before_last(
      const dbsol_geno_curve_2d &c,
      const vsol_point_2d_sptr & point, // new point at sample 0
      vcl_vector<double> &dt, // not read; only written and used as scrap
      const vcl_vector<double> &dtsq, 
      vcl_vector<double> &dk, // not read; only written and used as scrap
      const vcl_vector<double> &,//dksq,
      double E_ini 
      );

  assert(delta_normal > 0);
  assert(c.order() == 2);
  assert(c.size() >= 6);

  vcl_vector<double> dk; //:< differences of curvature at each sample
  vcl_vector<double> dksq; //: squared dk's
  vcl_vector<double> dt; //:< differences of tangent at each sample
  vcl_vector<double> dtsq; //: squared dt's

  const unsigned nsamples = c.size()+1;
  dk.resize(nsamples,0);
  dksq.resize(nsamples,0);
  dt.resize(nsamples,0);
  dtsq.resize(nsamples,0);
  grad.resize(nsamples,0);

  double E=0;
  for (unsigned i=1; i<c.size(); ++i) {
    dk[i] = c[i].curvature_at(0) - c[i-1].curvature_at(1);
    dksq[i] = dk[i]*(dk[i]);

    dt[i] = mw_util::angle_difference(c[i].tangent_angle_at(0), c[i-1].tangent_angle_at(1));
    dtsq[i] = dt[i]*(dt[i]);

    E += dtsq[i];
  }

  for (unsigned i=2; i<(nsamples -2); ++i) {
    double E_i_plus1, E_i_minus1;
    double normal_x, normal_y;
    vsol_point_2d_sptr point;

    // ---------- extract i-th partial derivative ---------

    // 1 - sum delta_normal to point
    get_normal_arc(c, i, &normal_x, &normal_y);

    point = c.point_at_sample(i);
    point->set_x(point->x() + delta_normal*normal_x);
    point->set_y(point->y() + delta_normal*normal_y);

    E_i_plus1 = energy_dt_middlepts(c, i, point, dt, dtsq, dk, dksq, E);
    
    // 2 - subtract delta_normal to point
    point = c.point_at_sample(i);
    point->set_x(point->x() - delta_normal*normal_x);
    point->set_y(point->y() - delta_normal*normal_y);

    E_i_minus1 = energy_dt_middlepts(c, i, point, dt, dtsq, dk, dksq, E);

    grad[i] = (E_i_plus1 - E_i_minus1)/(2*delta_normal);
  }


  /* do start and endpoints: for starting point, change tangent and analyse difference in
   * curvature with second interval; similar for the endpoint */
  
  // first sample (index 0)
  {
    double E_i_plus1, E_i_minus1;
    double normal_x, normal_y;
    vsol_point_2d_sptr point;

    // ---------- extract i-th partial derivative ---------

    // 1 - sum delta_normal to point

    get_normal_arc(c, 0, &normal_x, &normal_y);

    point = c.point_at_sample(0);
    point->set_x(point->x() + delta_normal*normal_x);
    point->set_y(point->y() + delta_normal*normal_y);

    E_i_plus1 = energy_dt_sample0(c, point, dtsq, dksq, E);

    point = c.point_at_sample(0);
    point->set_x(point->x() - delta_normal*normal_x);
    point->set_y(point->y() - delta_normal*normal_y);

    E_i_minus1 = energy_dt_sample0(c, point, dtsq, dksq, E);

    grad[0] = (E_i_plus1 - E_i_minus1)/(2*delta_normal);
  }

  { // second sample (index 1)
    double E_i_plus1, E_i_minus1;
    double normal_x, normal_y;
    const unsigned i=1;
    vsol_point_2d_sptr point;

    // ---------- extract i-th partial derivative ---------

    // 1 - sum delta_normal to point

    get_normal_arc(c, i, &normal_x, &normal_y);

    point = c.point_at_sample(i);
    point->set_x(point->x() + delta_normal*normal_x);
    point->set_y(point->y() + delta_normal*normal_y);

    E_i_plus1 = energy_dt_sample1(c, point, dt, dtsq, dk, dksq, E);

    point = c.point_at_sample(i);
    point->set_x(point->x() - delta_normal*normal_x);
    point->set_y(point->y() - delta_normal*normal_y);

    E_i_minus1 = energy_dt_sample1(c, point, dt, dtsq, dk, dksq, E);

    grad[i] = (E_i_plus1 - E_i_minus1)/(2*delta_normal);
  }

  { // last sample
    unsigned i = nsamples-1;
    double E_i_plus1, E_i_minus1;
    double normal_x, normal_y;
    vsol_point_2d_sptr point;

    // ---------- extract i-th partial derivative ---------

    // 1 - sum delta_normal to point

    get_normal_arc(c, i, &normal_x, &normal_y);

    point = c.point_at_sample(i);
    point->set_x(point->x() + delta_normal*normal_x);
    point->set_y(point->y() + delta_normal*normal_y);

    E_i_plus1 = energy_dt_sample_last(c, point, dtsq, dksq, E);

    point = c.point_at_sample(i);
    point->set_x(point->x() - delta_normal*normal_x);
    point->set_y(point->y() - delta_normal*normal_y);

    E_i_minus1 = energy_dt_sample_last(c, point, dtsq, dksq, E);

    grad[i] = (E_i_plus1 - E_i_minus1)/(2*delta_normal);
  }

  { // last sample minus 1
    double E_i_plus1, E_i_minus1;
    double normal_x, normal_y;
    unsigned i = nsamples-2;
    vsol_point_2d_sptr point;

    // ---------- extract i-th partial derivative ---------

    // 1 - sum delta_normal to point

    get_normal_arc(c, i, &normal_x, &normal_y);

    point = c.point_at_sample(i);
    point->set_x(point->x() + delta_normal*normal_x);
    point->set_y(point->y() + delta_normal*normal_y);

    E_i_plus1 = energy_dt_sample_before_last(c, point, dt, dtsq, dk, dksq, E);

    point = c.point_at_sample(i);
    point->set_x(point->x() - delta_normal*normal_x);
    point->set_y(point->y() - delta_normal*normal_y);

    E_i_minus1 = energy_dt_sample_before_last(c, point, dt, dtsq, dk, dksq, E);

    grad[i] = (E_i_plus1 - E_i_minus1)/(2*delta_normal);
  }
}
