// This is file shp/dbsksp/dbsksp_external_energy_function.cxx

//:
// \file

#include "dbsksp_external_energy_function.h"

#include <vgl/vgl_distance.h>

#include <vil/algo/vil_gauss_filter.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/vil_bilin_interp.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_numeric_traits.h>
#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_shock_fragment.h>



// =====================================================================
//  dbsksp_external_energy_function
// =====================================================================



// ----------------------------------------------------------------------------
//: Constructor
dbsksp_external_energy_function::
dbsksp_external_energy_function()
{
};



// =====================================================================
//  dbsksp_potential_energy_function
// =====================================================================


// ----------- CONSTRUCTORS / DESTRUCTORS -------------------------------
//: Constructor
  
dbsksp_potential_energy_function::
dbsksp_potential_energy_function():
  dbsksp_external_energy_function(),
  sampling_length_(1)
{
}

//: Destructor
dbsksp_potential_energy_function::
~dbsksp_potential_energy_function()
{
}



// ----------- DATA ACCESS -------------------------------  



void dbsksp_potential_energy_function::
set_potential_field(const vil_image_view<float >& potential_field)
{
  this->potential_field_ = potential_field; 

  // compute gradient of the potential field

  // smooth image
  vil_image_view<float > gauss_image;
  vil_gauss_filter_5tap<float, float >(potential_field,
    gauss_image, vil_gauss_filter_5tap_params(0.5));
  
  // compute gradient using sobel kernel
  vil_sobel_3x3<float, float >(gauss_image, this->grad_x_, this->grad_y_);
}


// ----------------------------------------------------------------------------
double dbsksp_potential_energy_function::
potential_at(double x, double y) const
{
  ////
  //int i = int(x+0.5);
  //int j = int(y+0.5);
  //if (i<0 || i>= (int)this->potential_field_.ni() ||
  //  j<0 || j>= (int)this->potential_field_.nj())
  //  return vnl_numeric_traits<double >::maxval;

  //return this->potential_field_((unsigned int)i, (unsigned int)j); 

  return vil_bilin_interp_safe_extend(this->potential_field_, x, y);
}


double dbsksp_potential_energy_function::
grad_x(double x, double y) const
{

  ////
  //int i = int(x+0.5);
  //int j = int(y+0.5);
  //if (i<0 || i>= (int)this->potential_field_.ni() ||
  //  j<0 || j>= (int)this->potential_field_.nj())
  //  return vnl_numeric_traits<double >::maxval;

  //return this->grad_x_((unsigned int)i, (unsigned int)j); 

  return vil_bilin_interp_safe_extend(this->grad_x_, x, y);
}


double dbsksp_potential_energy_function::
grad_y(double x, double y) const
{
  ////
  //int i = int(x+0.5);
  //int j = int(y+0.5);
  //if (i<0 || i>= (int)this->potential_field_.ni() ||
  //  j<0 || j>= (int)this->potential_field_.nj())
  //  return vnl_numeric_traits<double >::maxval;

  //return this->grad_y_((unsigned int)i, (unsigned int)j); 

  return vil_bilin_interp_safe_extend(this->grad_y_, x, y);

}



// ---------------- UTILITY -------------------------------

 //:  The main function. Compute external energy given a shock graph
double dbsksp_potential_energy_function::
f(const dbsksp_shock_graph_sptr& shock_graph)
{
  // interate thru the boundary to sum up the potential
  // all boundary elements correspond to an edge, either regular or degenerate
  double energy = 0;
  double bnd_length = 0;
  for (dbsksp_shock_graph::edge_iterator eit = shock_graph->edges_begin();
    eit != shock_graph->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = (*eit);
    
    // iterate thru boundary elements on both left and right sides
    dbsksp_shock_fragment_sptr fragment = e->fragment();
    for (int i=0; i<2; ++i)
    {
      dbsksp_bnd_arc_sptr bnd = fragment->bnd(i);
      dbgl_circ_arc arc = bnd->arc();
      for (double s = 0; s<arc.len(); s += this->sampling_length())
      {
        vgl_point_2d<double > pt = arc.point_at_length(s);
        energy += vnl_math_sqr(this->potential_at(pt.x(), pt.y()));
      }
      bnd_length += arc.len();
    }
  }
  return energy/bnd_length;
}



//double dbsksp_potential_energy_function::
//f(const dbsksp_shapelet& shapelet)
//{
//
//  // interate thru the boundary to sum up the potential
//  double energy = 0;
//  double bnd_length = 0;
//  for (int i=0; i<2; ++i)
//  {
//    dbgl_circ_arc arc = shapelet.bnd_arc(i);
//    for (double s = 0; s<arc.len(); s += this->sampling_length())
//    {
//      vgl_point_2d<double > pt = arc.point_at_length(s);
//      
//      //energy += vnl_math_sqr(this->potential_at(pt.x(), pt.y()));
//      //double potential = 1/ vnl_math_sqr((1+vnl_math_sqr(this->potential_at(pt.x(), pt.y()))));
//      double potential = 1/ vnl_math_sqr((1+vnl_math_abs(this->potential_at(pt.x(), pt.y()))));
//      vgl_vector_2d<double > t = arc.tangent_at_length(s);
//      vgl_vector_2d<double > level_dir(-this->grad_y(pt.x(), pt.y()), this->grad_x(pt.x(), pt.y()));
//      double alignment_factor = vnl_math_abs(cos_angle(t, level_dir));
//
//      energy -= potential * alignment_factor;
//    }
//    bnd_length += arc.len();
//  }
//  return energy;
//
//  //return energy/bnd_length;
//}


double dbsksp_potential_energy_function::
f(const dbsksp_shapelet_sptr& shapelet)
{

  // interate thru the boundary to sum up the potential
  double energy[] = {0, 0};
  double bnd_length = 0;
  double chord_length = 0;
  for (int i=0; i<2; ++i)
  {
    dbgl_circ_arc arc = shapelet->bnd_arc(i);
    for (double s = this->sampling_length()/2 ; s<arc.len(); 
      s += this->sampling_length())
    {
      vgl_point_2d<double > pt = arc.point_at_length(s);
      
      //energy += vnl_math_sqr(this->potential_at(pt.x(), pt.y()));
      //double potential = 1/ vnl_math_sqr((1+vnl_math_sqr(this->potential_at(pt.x(), pt.y()))));
      double potential = 1/ vnl_math_sqr((1+vnl_math_abs(this->potential_at(pt.x(), pt.y()))));
      vgl_vector_2d<double > t = arc.tangent_at_length(s);
      vgl_vector_2d<double > level_dir(-this->grad_y(pt.x(), pt.y()), 
        this->grad_x(pt.x(), pt.y()));
      double alignment_factor = vnl_math_abs(cos_angle(t, level_dir));
      energy[i] -= potential * alignment_factor;
      bnd_length += arc.length();
    }
  }

  return energy[0]+energy[1];
}



// =====================================================================
//  dbsksp_no_alignment_potential_energy_function
// =====================================================================

double dbsksp_no_alignment_potential_energy_function::
f(const dbsksp_shapelet_sptr &shapelet)
{
  // interate thru the boundary to sum up the potential
  double energy[] = {0, 0};
  double bnd_length = 0;
  double chord_length = 0;
  for (int i=0; i<2; ++i)
  {
    dbgl_circ_arc arc = shapelet->bnd_arc(i);
    for (double s = this->sampling_length()/2 ; s<arc.len(); 
      s += this->sampling_length())
    {
      vgl_point_2d<double > pt = arc.point_at_length(s);
      
      //energy += vnl_math_sqr(this->potential_at(pt.x(), pt.y()));
      double potential = 1/ vnl_math_sqr((1+vnl_math_sqr(this->potential_at(pt.x(), pt.y()))));
      //double potential = 1/ vnl_math_sqr((1+vnl_math_abs(this->potential_at(pt.x(), pt.y()))));
      //vgl_vector_2d<double > t = arc.tangent_at_length(s);
      //vgl_vector_2d<double > level_dir(-this->grad_y(pt.x(), pt.y()), 
        //this->grad_x(pt.x(), pt.y()));
      //double alignment_factor = vnl_math_abs(cos_angle(t, level_dir));
      double alignment_factor = 1;
      energy[i] -= potential * alignment_factor;
      bnd_length += arc.length();
    }
  }

  //// a hack for now, constraint on min and max radius
  //double d_start = vgl_distance(shapelet->bnd_arc(0).point1(), shapelet->bnd_arc(1).point1());
  //double d_end = vgl_distance(shapelet->bnd_arc(0).point2(), shapelet->bnd_arc(1).point2());

  //if ((d_end < this->d_min_) || (d_end > this->d_max_))
  //{
  //  return 1e10;
  //}

  return energy[0]+energy[1];
  

}




// ============================================================================
// dbsksp_potential_integrator
// ============================================================================

  

// --------------------------------------------------------------------------
//                  MAIN FUNCTIONS
// --------------------------------------------------------------------------

//: Integrate energy along a shock graph
double dbsksp_potential_integrator::
f(const dbsksp_shock_graph_sptr& shock_graph)
{
  // interate thru the boundary to sum up the potential
  // all boundary elements correspond to an edge, either regular or degenerate
  double sum = 0;
  for (dbsksp_shock_graph::edge_iterator eit = shock_graph->edges_begin();
    eit != shock_graph->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = (*eit);
    
    // iterate thru boundary elements on both left and right sides
    dbsksp_shock_fragment_sptr fragment = e->fragment();
    for (int i=0; i<2; ++i)
    {
      dbsksp_bnd_arc_sptr bnd = fragment->bnd(i);
      dbgl_circ_arc arc = bnd->arc();
      for (double s = 0; s<arc.len(); s += this->sampling_length())
      {
        vgl_point_2d<double > pt = arc.point_at_length(s);
        sum += vnl_math_sqr(this->potential_at(pt.x(), pt.y()));
      }
    }
  }
  return sum;

}


// ----------------------------------------------------------------------------
//: Integrate energy along two boundary of a shapelet
double dbsksp_potential_integrator::
f(const dbsksp_shapelet_sptr& s)
{
  // interate thru the boundary to sum up the potential
  double sum = 0;
  for (int i=0; i<2; ++i)
  {
    dbgl_circ_arc arc = s->bnd_arc(i);
    for (double s = this->sampling_length()/2 ; s<arc.len(); 
      s += this->sampling_length())
    {
      vgl_point_2d<double > pt = arc.point_at_length(s);
      sum += this->potential_at(pt.x(), pt.y());
    }
  }
  return sum;
}


// ----------------------------------------------------------------------------
//: Integrate energy along each boundary separately
// which_side = 0 for left boundary, = 1 for right boundary
double dbsksp_potential_integrator::
f(const dbsksp_shapelet_sptr& s, int which_side)
{
  // interate thru the boundary to sum up the potential
  double sum = 0; 
  dbgl_circ_arc arc = s->bnd_arc(which_side);
  for (double s = this->sampling_length()/2 ; s < arc.len(); 
    s += this->sampling_length())
  {
    vgl_point_2d<double > pt = arc.point_at_length(s);
    sum += this->potential_at(pt.x(), pt.y());
  }
  return sum;
}


//: Sum of energy at a set of points
float dbsksp_potential_integrator::
f(const vcl_vector<vgl_point_2d<double > >& pts, float default_value)
{
  // this->default_value_ = default_value;
  float sum = 0; 

  for (unsigned i=0; i<pts.size(); ++i) 
  {
    int im_i = int(pts[i].x());
    int im_j = int(pts[i].y());
    sum += ((im_i<0 || im_i >= this->ni_ || im_j <0 || im_j >= this->nj_) ? default_value : 
      this->potential_field_(im_i, im_j));
  }


  ////
  //for (unsigned i=0; i<pts.size(); ++i) 
  //{
  //  sum += this->potential_at(pts[i].x(), pts[i].y());
  //}
  return sum;
}


// --------------------------------------------------------------------------
//                  SUPPORT FUNCTIONS
// --------------------------------------------------------------------------

//: Return potential at a particular position
double dbsksp_potential_integrator::
potential_at(double x, double y) const
{
  // choose the nearest point
  int i = int(x);
  int j = int(y);
  // if out of range return a huge value
  if (i<0 || i>=this->ni_ || j <0 || j >= this->nj_)
    return this->default_value_;

  return this->potential_field_(i, j);
  //return vil_bilin_interp_safe_extend(this->potential_field_, x, y);
}
