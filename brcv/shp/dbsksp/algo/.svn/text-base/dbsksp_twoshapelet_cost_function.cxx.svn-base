// This is file shp/dbsksp/dbsksp_shock_snake_cost_function.cxx

//:
// \file

#include "dbsksp_twoshapelet_cost_function.h"

#include <vnl/vnl_math.h>
#include <vnl/vnl_numeric_traits.h>
#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_shock_fragment.h>





// ============================================================================
// dbsksp_twoshapelet_type2_cost_fnt
// ============================================================================

//: Constructor
dbsksp_twoshapelet_type2_cost_fnt::
dbsksp_twoshapelet_type2_cost_fnt(const dbsksp_twoshapelet_sptr& ss,
                                  dbsksp_potential_integrator* fnt,
                                  double lambda):
vnl_cost_function(5),
lambda_(lambda),
potential_integrator_(fnt)
{
  this->extract_fixed_params(ss,
    this->x0_, this->y0_, this->psi0_, this->r0_, this->phi0_, this->len_);
}


//: Get the fixed parameters of the twoshapelet
// x_fixed = [x0, y0, psi0, r0, phi0, len]
void dbsksp_twoshapelet_type2_cost_fnt::
extract_fixed_params(const dbsksp_twoshapelet_sptr& ss,
   double& x0, double& y0, double& psi0, 
   double& r0, double& phi0, double& len) const
{
  dbsksp_shapelet_sptr s = ss->shapelet_start();
  x0 = s->start().x();
  y0 = s->start().y();
  vgl_vector_2d<double > x_axis(1, 0);
  psi0 = signed_angle(x_axis, s->shock_geom().tangent_at(0));
  r0 = s->radius_start();
  phi0 = s->phi_start();
  len = s->chord_length() + ss->shapelet_end()->chord_length();
  return;
}


//: Get the free parameters of the twoshapelet
// x_free = [m0, phi1, m1, phi2, len_diff]
vnl_vector<double > dbsksp_twoshapelet_type2_cost_fnt::
get_free_params(const dbsksp_twoshapelet_sptr& s)
{
  vnl_vector<double > params(5, 0);
  params[0] = s->m0();
  params[1] = s->phi1();
  params[2] = s->m1();
  params[3] = s->phi2();
  params[4] = (s->len1() - s->len0()) / (s->len1() + s->len0());
  return params; 
}




//: Get fixed parameters
void dbsksp_twoshapelet_type2_cost_fnt::
get_fixed_params(double& x0, double& y0, double& psi0, 
                 double& r0, double& phi0, double& len)
{
  x0 = this->x0_;
  y0 = this->y0_;
  psi0 = this->psi0_;
  r0 = this->r0_;
  phi0 = this->phi0_;
  len = this->len_;
  return;
}






//: Compute two shapelet from a vector of free parameters
dbsksp_twoshapelet_sptr dbsksp_twoshapelet_type2_cost_fnt::
get_twoshapelet(vnl_vector<double > free_params)
{
  // turn the vector into something meaningful
  double m0 = free_params[0];
  double phi1 = free_params[1];
  double m1 = free_params[2];
  double phi2 = free_params[3];
  // x[4] = (len1-len0) / len
  // -1 < x[4] < 1
  double len_ratio = free_params[4];
  double len_diff = this->len_ * len_ratio;
  double len1 = (this->len_ + len_diff) / 2;
  double len0 = (this->len_ - len_diff) / 2;

  double alpha0 = vcl_asin(m0 * vcl_sin(this->phi0_));
  double theta0  = this->psi0_ - alpha0;;

  return new dbsksp_twoshapelet(
    this->x0_, this->y0_, theta0, this->r0_, this->phi0_, 
    m0, len0, phi1, m1, len1, phi2);
}





// THE MAIN FUNCTIONS -------------------------------------------------------

//:  Compute value of f(x) given a (free) parameter vector x
double dbsksp_twoshapelet_type2_cost_fnt::
f(vnl_vector<double> const& x)
{
  dbsksp_twoshapelet_sptr ss = this->get_twoshapelet(x);
    

  vnl_matrix<double > bnd_energy(2, 2, 0);
  vnl_matrix<double > bnd_length(2, 2, 0);
  vnl_vector<double > area(2, 0);

  for (int frag_index=0; frag_index<2; ++frag_index)
  {
    dbsksp_shapelet_sptr shapelet = ss->shapelet(frag_index);
    if (!shapelet->is_legal()) return vnl_numeric_traits<double >::maxval;
    for (int bnd_side = 0; bnd_side < 2; ++bnd_side)
    {
      bnd_energy(frag_index, bnd_side) = this->potential_integrator_->f(shapelet, bnd_side);
      bnd_length(frag_index, bnd_side) = shapelet->bnd_arc(bnd_side).length();
    }
    area(frag_index) = shapelet->area();
  }

  double cost_left = bnd_energy(0,0) + bnd_energy(1,0);
  double cost_right = bnd_energy(0,0) + bnd_energy(1,0);

  double total_energy = bnd_energy(0,0) + bnd_energy(0,1) + 
    bnd_energy(1,0) + bnd_energy(1,1);
  double energy_per_unit_length = total_energy / (ss->len0()+ss->len1());


  double total_area = area(0) + area(1);

  double length_left = bnd_length(0,0) + bnd_length(1,0);
  double length_right = bnd_length(0,1) + bnd_length(1,1);
  double total_length = bnd_length(0,0) + bnd_length(0,1) + 
    bnd_length(1,0) + bnd_length(1,1);

  double shape_prior = vnl_math_sqr(ss->phi2() - ss->phi0()) * 
    vnl_math_sqr(ss->shapelet_end()->radius_end());

  //double shape_prior = vnl_math_abs(ss->shapelet_end()->radius_end() - ss->r0());


  //double fitness_left =  (cost_left - lambda*length_left) / length_left;
  //double fitness_right = (cost_right - lambda*length_right) / length_right;
  //double mean_fitness = (fitness_left+fitness_right)/2;


  double lambda = this->lambda_;
  return energy_per_unit_length + this->lambda_* shape_prior;
}






















































// ============================================================================
// dbsksp_twoshapelet_cost_function
// ============================================================================

// ----------------------------------------------------------------------------
// CONSTRUCTORS / DESTRUCTORS
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
dbsksp_twoshapelet_cost_function::
dbsksp_twoshapelet_cost_function(const dbsksp_twoshapelet_sptr& s,
                                 dbsksp_external_energy_function* fnt,
                                 double internal_external_energy_ratio):
  vnl_cost_function(4), 
  lambda_(internal_external_energy_ratio), 
  external_energy_function_(fnt)
{
  vnl_vector<double> fixed_params = this->extract_fixed_params(s);
  this->set_fixed_params(fixed_params);
}
  

// ----------------------------------------------------------------------------
// DATA ACCESS
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//: Get a twoshapelet given a vector of free variables (4x1)
// x_free = [m0, phi1, m1, phi2]
dbsksp_twoshapelet_sptr dbsksp_twoshapelet_cost_function::
get_twoshapelet(const vnl_vector<double >& x)
{
  assert (x.size() == 4);
  double x0 = this->x0_;
  double y0 = this->y0_;
  
  double r0 = this->r0_;
  double phi0 = this->phi0_;
  double m0 = x[0];
  double len0 = this->len0_;
  double phi1 = x[1];
  double m1 = x[2];
  double len1 = this->len1_;
  double phi2 = x[3];

  double alpha0 = vcl_asin(m0 * vcl_sin(phi0));
  double theta0  = this->psi0_ - alpha0;;

  return new dbsksp_twoshapelet( x0,  y0,  theta0,  r0, 
     phi0,  m0,  len0, 
     phi1,  m1,  len1, 
     phi2 );
}


// ----------------------------------------------------------------------------
//: Get the free parameters of the twoshapelet
// x_free = [m0, phi1, m1, phi2]
vnl_vector<double > dbsksp_twoshapelet_cost_function::
get_free_params(const dbsksp_twoshapelet_sptr& s)
{
  vnl_vector<double > params(4, 0);
  params[0] = s->m0();
  params[1] = s->phi1();
  params[2] = s->m1();
  params[3] = s->phi2();

  return params;
}


// ----------------------------------------------------------------------------
//: Get vector of fixed params
// x_fixed = [x0, y0, psi0, r0, phi0, len0, len1]
vnl_vector<double > dbsksp_twoshapelet_cost_function::
fixed_params() const
{
  vnl_vector<double > params(7, 0);
  //: fixed parameters
  params[0] = this->x0_   ;
  params[1] = this->y0_   ;
  params[2] = this->psi0_ ;
  params[3] = this->r0_   ;
  params[4] = this->phi0_ ;
  params[5] = this->len0_ ;
  params[6] = this->len1_ ;
  return params;
}


// ----------------------------------------------------------------------------
//: Set the fixed parameters
// x_fixed = [x0, y0, psi0, r0, phi0, len0, len1]
void dbsksp_twoshapelet_cost_function::
set_fixed_params(const vnl_vector<double >& params)
{
  assert(params.size()==7);
  this->x0_ = params[0];
  this->y0_ = params[1];
  this->psi0_ = params[2];
  this->r0_ = params[3];
  this->phi0_= params[4];
  
  this->len0_ = params[5];
  this->len1_ = params[6];
}



// --------------------------------------------------------------------------
// THE MAIN FUNCTIONS
// --------------------------------------------------------------------------

////:  The main function.  Given the parameter vector x, compute the value of f(x).
//double dbsksp_twoshapelet_cost_function::
//f(vnl_vector<double> const& x)
//{
//  dbsksp_twoshapelet s = *this->get_twoshapelet(x);
//  double total_cost = 0;
//  double bnd_length = 0;
//  double expected_length = 0;
//  for (int i=0; i<2; ++i)
//  {
//    dbsksp_shapelet_sptr shapelet = s.shapelet(i);
//    if (!shapelet->is_legal()) return vnl_numeric_traits<double >::maxval;
//    total_cost += this->external_energy_function()->f(shapelet);
//    bnd_length += shapelet->bnd_arc(0).length() + shapelet->bnd_arc(1).length();
//    double chord = shapelet->chord_length();
//    double phi_mean = (shapelet->phi_start()+(vnl_math::pi -shapelet->phi_end()))/2;
//    expected_length += 2*chord*vcl_sin(phi_mean);
//  }
//
//  double normalized_length = vnl_math_max(bnd_length,expected_length);
//  return total_cost / normalized_length;
//}




// ----------------------------------------------------------------------------
//: Compute value of f(x) given a (free) parameter vector x
// x_free = [m0, phi1, m1, phi2]
double dbsksp_twoshapelet_cost_function::
f(vnl_vector<double> const& x)
{
  dbsksp_twoshapelet s = *this->get_twoshapelet(x);
  vnl_matrix<double > bnd_energy(2, 2, 0);
  vnl_matrix<double > bnd_length(2, 2, 0);
  vnl_matrix<double > expected_length(2, 2, 0);

  for (int frag_index=0; frag_index<2; ++frag_index)
  {
    dbsksp_shapelet_sptr shapelet = s.shapelet(frag_index);
    if (!shapelet->is_legal()) return vnl_numeric_traits<double >::maxval;
    for (int bnd_side = 0; bnd_side < 2; ++bnd_side)
    {
      bnd_energy(frag_index, bnd_side) = this->external_energy_function()->f(shapelet, bnd_side);
      bnd_length(frag_index, bnd_side) = shapelet->bnd_arc(bnd_side).length();
      double phi_mean = (shapelet->phi_start()+ shapelet->phi_end())/2;
      expected_length(frag_index, bnd_side) = shapelet->chord_length()*vcl_sin(phi_mean);
    }
  }

  // compute cost of each side separately
  double total_cost = 0;
  for (int side = 0; side < 2; ++side)
  {
    double energy = bnd_energy(0,side) + bnd_energy(1,side);
    double length = vnl_math_max( bnd_length(0,side) + bnd_length(1,side),
      expected_length(0, side) + expected_length(1, side) );
    total_cost += energy/ length;
  }
  return total_cost / 2;
}


// --------------------------------------------------------------------------
// INTERNAL SUPPORT FUNCTIONS
// --------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//: Get the fixed parameters of the twoshapelet cost
// x_fixed = [x0, y0, psi0, r0, phi0, len0, len1]
vnl_vector<double > dbsksp_twoshapelet_cost_function::
extract_fixed_params(const dbsksp_twoshapelet_sptr& s)
{
  dbsksp_shapelet_sptr start = s->shapelet_start();
  dbsksp_shapelet_sptr end = s->shapelet_end();

  vnl_vector<double > params(7, 0);
  params[0] = start->x0();
  params[1] = start->y0();
  params[2] = 
    signed_angle(vgl_vector_2d<double >(1, 0), start->shock_geom().tangent_at(0));
  params[3] = start->r0();
  params[4] = start->phi0();
  params[5] = start->len();
  params[6] = end->len();
  return params;
}




// ============================================================================
// dbsksp_terminal_twoshapelet_cost_fnt
// ============================================================================

// CONSTRUCTORS / DESTRUCTORS -------------------------------------------------

// ----------------------------------------------------------------------------
//: Constructor
dbsksp_terminal_twoshapelet_cost_fnt::
dbsksp_terminal_twoshapelet_cost_fnt(const dbsksp_shapelet_sptr& s,
                                     vil_image_view<float > potential_image,
                                     double internal_external_energy_ratio):
  vnl_cost_function(3), 
  potential_image_(potential_image),
  lambda_(internal_external_energy_ratio)
{
  this->extract_fixed_params(s, this->x0_, this->y0_, this->psi0_, 
    this->r0_, this->phi0_, this->init_len0_);
}


// DATA ACCESS ----------------------------------------------------------------

// ----------------------------------------------------------------------------
//: Get the fixed parameters of the twoshapelet
// x_fixed = [x0, y0, psi0, r0, phi0, init_len0]
void dbsksp_terminal_twoshapelet_cost_fnt::
    extract_fixed_params(const dbsksp_shapelet_sptr& s,
    double& x0, double& y0, double& psi0, 
    double& r0, double& phi0, double& init_len0 ) const
{
  x0 = s->x0();
  y0 = s->y0();
  dbgl_conic_arc shock_curve = s->shock_geom();
  psi0 = signed_angle(vgl_vector_2d<double >(1, 0), shock_curve.tangent_at(0));
  r0 = s->r0();
  phi0 = s->phi0();
  return;
} 


// ----------------------------------------------------------------------------
//: Get fixed parameters
void dbsksp_terminal_twoshapelet_cost_fnt::
get_fixed_params(double& x0, double& y0, double& psi0, 
                 double& r0, double& phi0, double& init_len0)
{
  x0 = this->x0_;
  y0 = this->y0_;
  psi0 = this->psi0_;
  r0 = this->r0_;
  phi0 = this->phi0_;
  init_len0 = this->init_len0_;
}


// ----------------------------------------------------------------------------
//: Get the free parameters of the terminal_twoshapelet
// x_free = [len0_multiplier, m0, phi1]
vnl_vector<double > dbsksp_terminal_twoshapelet_cost_fnt::
get_free_params(const dbsksp_shapelet_sptr& s)
{
  vnl_vector<double > params(3, 0);
  params[0] = s->len() / this->init_len0_;
  params[1] = s->m0();
  params[2] = s->phi_end();
  return params;
}

// ----------------------------------------------------------------------------
//: Get a shapelet (next to terminal_edge) given a free variable vector (3x1)
// x_free = [len0_multiplier, m0, phi1]
dbsksp_shapelet_sptr dbsksp_terminal_twoshapelet_cost_fnt::
get_shapelet(const vnl_vector<double >& x)
{
  assert (x.size() == 3);
  double x0 = this->x0_;
  double y0 = this->y0_;
  double r0 = this->r0_;
  double phi0 = this->phi0_;
  double len0_multiplier = x[0];
  double len0 = this->init_len0_ * len0_multiplier;
  double m0 = x[1];
  double phi1 = x[2];

  double alpha0 = vcl_asin(m0 * vcl_sin(phi0));
  double theta0  = this->psi0_ - alpha0;

  // make sure the angle is within [-pi ... pi]
  // first, make sure theta0 is within [-2pi, 2pi] by using fmod
  // then add 2pi to make it positive,
  // Note that fmod(x, y) returns a number with the same sign as x
  // and with absolute value less than |y|
  theta0 = vcl_fmod(theta0, vnl_math::pi*2) + vnl_math::pi*2;

  // now convert it to [-pi, pi] range by the "add and subtract" trick
  theta0 = vcl_fmod(theta0+vnl_math::pi, vnl_math::pi*2) - vnl_math::pi;
  return new dbsksp_shapelet(x0, y0, theta0, r0, phi0, phi1, m0, len0);
}



////: Get a twoshapelet, including the A_infty fragment, 
//// given a free variable vector (3x1)
//// x_free = [len0, m0, phi1]
//dbsksp_twoshapelet_sptr dbsksp_terminal_twoshapelet_cost_fnt::
//get_twoshapelet(const vnl_vector<double >& x)
//{
//  assert (x.size() == 3);
//  double x0 = this->x0_;
//  double y0 = this->y0_;
//  double r0 = this->r0_;
//  double phi0 = this->phi0_;
//  double len0 = x[0];
//  double m0 = x[1];
//  double phi1 = vnl_math::pi-x[2];
//  double m1 = 0;
//  double len1 = 0;
//  double phi2 = vnl_math::pi;
//
//  double alpha0 = vcl_asin(m0 * vcl_sin(phi0));
//  double theta0  = this->psi0_ - alpha0;;
//
//  return new dbsksp_twoshapelet( x0,  y0,  theta0,  r0, 
//     phi0,  m0,  len0, 
//     phi1,  m1,  len1, 
//     phi2 );
//  return new dbsksp_twoshapelet();
//  
//
//}









// --------------------------------------------------------------------------
// THE MAIN FUNCTIONS
// --------------------------------------------------------------------------
  
// ----------------------------------------------------------------------------
//:  The main function.  Given the parameter vector x, compute the value of f(x).
// x = [len0, m0, phi1]
double dbsksp_terminal_twoshapelet_cost_fnt::
f(vnl_vector<double> const& x)
{
  dbsksp_shapelet_sptr s = this->get_shapelet(x);
    

  // set up the potential integrator to compute cost from a potential image
  dbsksp_potential_integrator potential_integrator;
  potential_integrator.set_potential_field(this->potential_image_);
  potential_integrator.set_sampling_length(1.0);

  vnl_matrix<double > bnd_energy(2, 2, 0);
  vnl_matrix<double > bnd_length(2, 2, 0);
  vnl_vector<double > area(2, 0);

  // cost of the first shapelet
  if (!s->is_legal()) return vnl_numeric_traits<double >::maxval;
  
  for (int bnd_side = 0; bnd_side < 2; ++bnd_side)
  {
    bnd_energy(0, bnd_side) = potential_integrator.f(s, bnd_side);
    bnd_length(0, bnd_side) = s->bnd_arc(bnd_side).length();
  }
  area(0) = s->area();


  // START HERE !!!!!!!!!!!!!!!!!!!!!!!!!!
  //HACK
  // cost of the terminal shapelet
  for (int bnd_side = 0; bnd_side < 2; ++bnd_side)
  {
    bnd_energy(1, bnd_side) = 0;
    bnd_length(1, bnd_side) = 0;
  }
  area(1) = 0;


  // compute overall cost
  double total_cost = bnd_energy(0,0) + bnd_energy(0,1) + 
    bnd_energy(1,0) + bnd_energy(1,1);
  double total_length = bnd_length(0,0) + bnd_length(0,1) + 
    bnd_length(1,0) + bnd_length(1,1);

  double length_left = bnd_length(0,0) + bnd_length(1,0);
  double length_right = bnd_length(0,1) + bnd_length(1,1);
  
  return total_cost - 
    this->lambda_*(total_length - vnl_math_abs(length_left - length_right));
}












