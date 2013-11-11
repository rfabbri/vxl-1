// This is file shp/dbsksp/dbsksp_shock_snake_cost_function.cxx

//:
// \file

#include "dbsksp_shock_snake_cost_function_old.h"

#include <vnl/vnl_math.h>
#include <vnl/vnl_numeric_traits.h>


#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_shock_fragment.h>




// =====================================================================
//  dbsksp_shock_snake_cost_function
// =====================================================================


//:  The main function.  Given the parameter vector x, compute the value of f(x).
double dbsksp_shock_snake_cost_function::
f(vnl_vector<double> const& x)
{
  this->shock_model()->update_shock_graph(x);
  
  // return a huge number if the graph is illegal
  if (!this->shock_model()->shock_graph()->is_legal())
  {
    return vnl_numeric_traits<double >::maxval;
  }
  return this->external_energy_function()->f(this->shock_model()->shock_graph());
}



// =====================================================================
//  dbsksp_shapelet_cost_function
// =====================================================================




// --------------------- THE MAIN FUNCTION ---------------------
  
//:  The main function.  Given the parameter vector x, compute the value of f(x).
double dbsksp_shapelet_cost_function::
f(vnl_vector<double> const& x)
{
  assert (x.size() == this->get_number_of_unknowns());
  
  vnl_vector<double >::const_iterator xiter = x.begin();
  for (unsigned i=0; i<this->param_template_.size(); ++i)
  {
    if (this->param_filter_[i]==0) continue;
    param_template_[i] = *xiter;
    ++xiter;
  }
  
  dbsksp_shapelet_sptr shapelet = new dbsksp_shapelet(param_template_);

  if (!shapelet->is_legal()) return vnl_numeric_traits<double >::maxval;
  return this->external_energy_function()->f(shapelet);
}


vnl_vector<double > dbsksp_shapelet_cost_function::
free_params() const
{
  vnl_vector<double > p(this->get_number_of_unknowns(), 0);

  vnl_vector<double >::iterator xiter = p.begin();
  for (unsigned i=0; i<this->param_template_.size(); ++i)
  {
    if (this->param_filter_[i]==0) continue;
    *xiter = param_template_[i];
    ++xiter;
  }
  
  return p;

}



// =====================================================================
//  dbsksp_half_shapelet_cost_function
// =====================================================================


//: Constructor
dbsksp_half_shapelet_cost_function::
dbsksp_half_shapelet_cost_function(const dbsksp_shock_edge_sptr& e,
                                   const dbsksp_shock_node_sptr& node, 
                                   dbsksp_external_energy_function* fnt,
                                   double internal_external_energy_ratio):
external_energy_function_(fnt)
{
  assert(node == e->source() || node == e->target());

  this->start_ = node->pt();
  this->shock_tangent_start_ = rotated(e->chord_dir(node), node->descriptor(e)->alpha);
  this->phi_start_ = node->descriptor(e)->phi;
  this->radius_start_ = node->radius();
  this->chord_length_ = e->chord_length();
  this->lambda_ = internal_external_energy_ratio;
}




//: Constructor
dbsksp_half_shapelet_cost_function::
dbsksp_half_shapelet_cost_function(vgl_point_2d<double > start,
double theta0,
double phi_start,
double radius_start,
double chord_length,
  dbsksp_external_energy_function* fnt,
  double internal_external_energy_ratio):
external_energy_function_(fnt),
lambda_(internal_external_energy_ratio),
start_(start),
shock_tangent_start_(vgl_vector_2d<double >(vcl_cos(theta0), vcl_sin(theta0))),
phi_start_(phi_start),
radius_start_(radius_start),
chord_length_(chord_length)
{}


//: Constructor
// structure of fixed params
// x0
// y0
// theta0
// phi0
// r0
// L
dbsksp_half_shapelet_cost_function::
dbsksp_half_shapelet_cost_function(const vnl_vector<double >& fixed_params,
  dbsksp_external_energy_function* fnt,
  double internal_external_energy_ratio):
external_energy_function_(fnt),
lambda_(internal_external_energy_ratio),
start_(vgl_point_2d<double >(fixed_params[0], fixed_params[1])),
shock_tangent_start_(vgl_vector_2d<double >(vcl_cos(fixed_params[2]), vcl_sin(fixed_params[2]))),
phi_start_(fixed_params[3]),
radius_start_(fixed_params[4]),
chord_length_(fixed_params[5])
{
}



// ----------------------------------------------------------------------------
//: Get a shapelet given a vector of free variables (4x1)
dbsksp_shapelet_sptr dbsksp_half_shapelet_cost_function::
get_shapelet(const vnl_vector<double >& x)
{
  assert(x.size()==2);
  // structure of x
  // phi1
  // m
  double x0 = this->start_.x();
  double y0 = this->start_.y();
  
  double r0 = this->radius_start_;
  double phi0 = this->phi_start_;
  double phi1 = x[0];
  double m0 = x[1];
  double len = this->chord_length_;
  double alpha0 = vcl_asin(m0*vcl_sin(phi0));
  vgl_vector_2d<double > chord_dir = rotated(this->shock_tangent_start_, -alpha0);
  double theta0 = vcl_atan2(chord_dir.y(), chord_dir.x());
  return new dbsksp_shapelet(x0, y0, theta0, r0, phi0, phi1, m0, len);
}





// ----------------------------------------------------------------------------
//:
vnl_vector<double > dbsksp_half_shapelet_cost_function::
get_fixed_params(const dbsksp_shock_edge_sptr& e,
                 const dbsksp_shock_node_sptr& node)
{
  vnl_vector<double > fixed_params(6, 0);
  fixed_params[0] = node->pt().x();
  fixed_params[1] = node->pt().y();
  vgl_vector_2d<double > chord_dir = rotated(e->chord_dir(node), node->descriptor(e)->alpha);
  fixed_params[2] = vcl_atan2(chord_dir.y(), chord_dir.x());
  fixed_params[3] = node->descriptor(e)->phi;
  fixed_params[4] = node->radius();
  fixed_params[5] = e->chord_length();
  return fixed_params;
}






vnl_vector<double > dbsksp_half_shapelet_cost_function::
get_free_params(const dbsksp_shock_edge_sptr& e,
                const dbsksp_shock_node_sptr& node)
{
  // structure of free params
  // phi1
  // m
  vnl_vector<double > free_params(2, 0);
  free_params[0] = e->opposite(node)->descriptor(e)->phi;
  free_params[1] = e->param_m(node);
  return free_params;
}



// --------------------- THE MAIN FUNCTION ---------------------

//:  The main function.  Given the parameter vector x, compute the value of f(x).
double dbsksp_half_shapelet_cost_function::
f(vnl_vector<double> const& x)
{
  dbsksp_shapelet_sptr shapelet = this->get_shapelet(x);
  if (!shapelet->is_legal()) return vnl_numeric_traits<double >::maxval;
  double ext_energy = this->external_energy_function()->f(shapelet);

  return ext_energy;
}






// =====================================================================
//  dbsksp_3_param_shapelet_cost_function
// =====================================================================


//: Constructor
dbsksp_3_param_shapelet_cost_function::
dbsksp_3_param_shapelet_cost_function(const dbsksp_shock_edge_sptr& e,
                                      const dbsksp_shock_node_sptr& node, 
                                      dbsksp_external_energy_function* fnt,
                                      double internal_external_energy_ratio): 
external_energy_function_(fnt)
{
  assert(node == e->source() || node == e->target());

  this->start_ = node->pt();
  this->shock_tangent_start_ = rotated(e->chord_dir(node), node->descriptor(e)->alpha);
  this->phi_start_ = node->descriptor(e)->phi;
  this->radius_start_ = node->radius();
  this->lambda_ = internal_external_energy_ratio;
  return;
}

// --------------------- DATA ACCESS --------------------- 

//: Get a shapelet given a vector of free variables (4x1)
dbsksp_shapelet dbsksp_3_param_shapelet_cost_function::
get_shapelet(const vnl_vector<double >& x)
{

  assert(x.size()==3);
  // structure of x
  // phi1
  // m
  // L
  double x0 = this->start_.x();
  double y0 = this->start_.y();
  
  double r0 = this->radius_start_;
  double phi0 = this->phi_start_;
  double phi1 = x[0];
  double m0 = x[1];
  double len = x[2];
  double alpha0 = vcl_asin(m0*vcl_sin(phi0));
  vgl_vector_2d<double > chord_dir = rotated(this->shock_tangent_start_, -alpha0);
  double theta0 = vcl_atan2(chord_dir.y(), chord_dir.x());
  return dbsksp_shapelet(x0, y0, theta0, r0, phi0, phi1, m0, len);
}


//
vnl_vector<double > dbsksp_3_param_shapelet_cost_function::
get_free_params(const dbsksp_shock_edge_sptr& e, const dbsksp_shock_node_sptr& node)
{
  // structure of free params
  // phi1
  // m
  // L
  vnl_vector<double > free_params(3, 0);
  free_params[0] = e->opposite(node)->descriptor(e)->phi;
  free_params[1] = e->param_m(node);
  free_params[2] = e->chord_length();
  return free_params;
}


// --------------------- THE MAIN FUNCTION ---------------------

//:  The main function.  Given the parameter vector x, compute the value of f(x).
double dbsksp_3_param_shapelet_cost_function::
f(vnl_vector<double> const& x)
{
  dbsksp_shapelet shapelet = this->get_shapelet(x);
  if (!shapelet.is_legal()) return vnl_numeric_traits<double >::maxval;
  double ext_energy = this->external_energy_function()->f(&shapelet);

  double total_length = 0;
  
  // internal energy is the length of the contour
  for (int i=0; i<2; ++i)
  {
    total_length += shapelet.bnd_arc(i).length();
  }


  //return ext_energy / total_length;



  // internal energy is the length of the contour
  double int_energy = 0;
  for (int i=0; i<2; ++i)
  {
    int_energy += shapelet.bnd_arc(i).length();
  }

  return ext_energy - this->lambda_*int_energy;
}









