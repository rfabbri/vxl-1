// This is file shp/dbsksp/algo/dbsksp_interp_xfrag_cost_function.cxx

//:
// \file

#include "dbsksp_interp_xfrag_cost_function.h"

#include <dbgl/algo/dbgl_closest_point.h>

#include <dbnl/dbnl_math.h>
#include <dbnl/dbnl_angle.h>

#include <vgl/vgl_distance.h>
#include <vnl/vnl_math.h>



//------------------------------------------------------------------------------
//: Coarse interpolation between two xsample
bool dbsksp_xfrag_coarse_interp_along_shock_curve(const dbsksp_xshock_node_descriptor& start,
                                             const dbsksp_xshock_node_descriptor& end,
                                             double t,
                                             dbsksp_xshock_node_descriptor& xsample)
{
  // construct shock curve
  dbgl_biarc shock_curve(start.pt(), start.shock_tangent(), 
    end.pt(), end.shock_tangent());

  if (!shock_curve.is_consistent())
    return false;

  // Interpolate to get shock point
  vgl_point_2d<double > shock_pt = shock_curve.point_at(t * shock_curve.len());

  // project the shock point to the two boundary biarcs
  dbsksp_xshock_fragment xfrag(start, end);
  dbgl_biarc left_bnd = xfrag.bnd_left_as_biarc();
  dbgl_biarc right_bnd = xfrag.bnd_right_as_biarc();

  // if something is wrong, then just take the first xdesc
  if (left_bnd.is_consistent() && right_bnd.is_consistent())
  {
    double left_s = 0;
    dbgl_closest_point::point_to_biarc(shock_pt, left_bnd, left_s);
    vgl_point_2d<double > left_pt = left_bnd.point_at(left_s);

    double right_s = 0;
    dbgl_closest_point::point_to_biarc(shock_pt, right_bnd, right_s);
    vgl_point_2d<double > right_pt = right_bnd.point_at(right_s);

    // compute phi
    //vgl_vector_2d<double > shock_tangent = rotated(right_pt-left_pt, vnl_math::pi_over_2);
    double twophi = signed_angle(right_pt-shock_pt, left_pt-shock_pt);
    double phi = dbnl_angle_0to2pi(twophi) / 2;

    xsample.set(left_pt, right_pt, phi);
  }
  else
  {
    return false;
  }
  return true;
}






//==============================================================================
// dbsksp_compute_shapelet_chord_cost_function
//==============================================================================


//------------------------------------------------------------------------------
//: Main function of the cost function
double dbsksp_compute_shapelet_chord_cost_function::
f(const vnl_vector<double >& x)
{
  double theta = x[0];
  double m_start = vcl_sin(this->start_psi_ - theta) / sin_start_phi_;
  double m_end = vcl_sin(this->end_psi_ - theta) / sin_end_phi_;

  // ideally, the two should be opposite of each other
  return (m_start + m_end) * (m_start + m_end);
}





//==============================================================================
// dbsksp_xfrag_sample_cost_function
//==============================================================================



//------------------------------------------------------------------------------
//: Constructor
dbsksp_xfrag_sample_kdiff_cost_function::
dbsksp_xfrag_sample_kdiff_cost_function(const dbsksp_xshock_fragment& xfrag, 
                                        double t) :   
vnl_least_squares_function(4, 6, vnl_least_squares_function::no_gradient), 
//vnl_least_squares_function(4, 4, vnl_least_squares_function::no_gradient), 
xfrag_(xfrag), t_(t)
{
  //1) Find boundary points corresponding to t
  for (int i =0; i < 2; ++i)
  {
    this->bnd_biarc[i] = xfrag.bnd_as_biarc(dbsksp_xshock_fragment::bnd_side(i));
    this->bnd_pt[i] = bnd_biarc[i].point_at(t * bnd_biarc[i].len());
  }

  this->half_width = vgl_distance(bnd_pt[0], bnd_pt[1])/2;

  this->start_ = xfrag.start();
  this->end_ = xfrag.end();

  
}


//------------------------------------------------------------------------------
//: Main function
void dbsksp_xfrag_sample_kdiff_cost_function::
f(const vnl_vector<double >& x, vnl_vector<double >& fx)
{
  dbsksp_xshock_node_descriptor xdesc = this->x_to_xsample(x);

  // compute curvature difference as a result
  dbsksp_xshock_fragment start_xfrag(this->start_, xdesc);
  dbsksp_xshock_fragment end_xfrag(xdesc, this->end_);

  // curvature difference - 6 terms
  for (int i =0; i < 2; ++i)
  {
    dbsksp_xshock_fragment::bnd_side side = dbsksp_xshock_fragment::bnd_side(i);
    dbgl_biarc start_biarc = start_xfrag.bnd_as_biarc(side);
    dbgl_biarc end_biarc = end_xfrag.bnd_as_biarc(side);

    if (start_biarc.flag() == -1 || end_biarc.flag() == -1)
    {
      fx[3*i] = fx[3*i+1] = fx[3*i+2] = vnl_numeric_traits<double >::maxval;
      continue;
    }

    double start_k[2]; // curvature of two arcs of start_biarc
    start_k[0] = start_biarc.k1();
    start_k[1] = (start_biarc.flag()==0) ? start_biarc.k1() : start_biarc.k2();
    
    double start_len[2]; // length of the two arcs of start_biarc
    // in degenerate cases, divide the length evenly betwen two arcs
    start_len[0] = (start_biarc.flag()==0) ? start_biarc.len()/2 : start_biarc.len1();
    start_len[1] = (start_biarc.flag()==0) ? start_biarc.len()/2 : start_biarc.len2();


    double end_k[2]; // curvature of two arcs of start_biarc
    end_k[0] = end_biarc.k1();
    end_k[1] = (end_biarc.flag()==0) ? end_biarc.k1() : end_biarc.k2();

    double end_len[2]; // length of the two arcs of end_biarc
    // in degenerate cases, divide the length evenly betwen two arcs
    end_len[0] = (end_biarc.flag()==0) ? end_biarc.len()/2 : end_biarc.len1();
    end_len[1] = (end_biarc.flag()==0) ? end_biarc.len()/2 : end_biarc.len2();


    // cost components: curvature difference^2 * len
    fx[3*i    ] = (start_k[1] - start_k[0]) * (start_len[0] + start_len[1]);
    fx[3*i + 1] = (end_k[0]   - start_k[1]) * (end_len[0] + start_len[1]  );
    fx[3*i + 2] = (end_k[1]   - end_k[0]  ) * (end_len[1] + end_len[0]    );
  }
}


//------------------------------------------------------------------------------
//: Convert from 'x' to xnode descriptor
dbsksp_xshock_node_descriptor dbsksp_xfrag_sample_kdiff_cost_function::
x_to_xsample(const vnl_vector<double >& x) const
{
  vgl_point_2d<double > chordal_pt = midpoint(bnd_pt[0], bnd_pt[1], x[0]);

  // tangent vector
  double psi = x[1];
  vgl_vector_2d<double > orient(vcl_cos(psi), vcl_sin(psi));

  // shock point
  double b = x[2];
  vgl_point_2d<double > shock_pt = chordal_pt + b * orient;

  // half chordal width
  double h = x[3] * this->half_width;

  // radius
  double radius = vnl_math_hypot(h, b);

  // phi
  double phi = vcl_acos(-b/radius);

  return dbsksp_xshock_node_descriptor(shock_pt.x(), shock_pt.y(), psi, phi, radius);
}


//------------------------------------------------------------------------------
//: Initial value for 'x'
vnl_vector<double > dbsksp_xfrag_sample_kdiff_cost_function::
initial_x() const
{
  vnl_vector<double > x(4, 0);
  // x[0]: parameter of chordal point along the line segment connecting two initial boundary point
  x[0] = 0.5; // chordal is mid-point of bnd_pt[0] (left) and bnd_pt[1] (right)

  // x[1]: tangent angle at the shock point
  vgl_vector_2d<double > orient = rotated(bnd_pt[1]-bnd_pt[0], vnl_math::pi_over_2);
  x[1] = vcl_atan2(orient.y(), orient.x());

  // x[2]: signed distance (from chordal point, along the tangent vector) of the shock point
  x[2] = 0;

  // x[3]: ratio between the actual chordal length and half distance between two initial boundary point
  x[3] = 1;
  
  return x;
}









//==============================================================================
// dbsksp_xfrag_sample_along_shock_biarc_cost_function
//==============================================================================

  //: Constructor
dbsksp_xfrag_sample_along_shock_biarc_cost_function::
dbsksp_xfrag_sample_along_shock_biarc_cost_function(
  const dbsksp_xshock_fragment& xfrag, double t) : 
vnl_least_squares_function(3, 6, vnl_least_squares_function::no_gradient), 
xfrag_(xfrag), t_(t)
{
  //1) Find boundary points corresponding to t
  this->start_ = xfrag.start();
  this->end_ = xfrag.end();

  dbsksp_xfrag_coarse_interp_along_shock_curve(this->start_, this->end_, t, this->init_xsample_);
  return;
}


//------------------------------------------------------------------------------
  //: Main function
void dbsksp_xfrag_sample_along_shock_biarc_cost_function::
f(const vnl_vector<double >& x, vnl_vector<double >& fx)
{
  dbsksp_xshock_node_descriptor xdesc = this->x_to_xsample(x);

  // compute curvature difference as a result
  dbsksp_xshock_fragment start_xfrag(this->start_, xdesc);
  dbsksp_xshock_fragment end_xfrag(xdesc, this->end_);

  // curvature difference - 6 terms
  for (int i =0; i < 2; ++i)
  {
    dbsksp_xshock_fragment::bnd_side side = dbsksp_xshock_fragment::bnd_side(i);
    dbgl_biarc start_biarc = start_xfrag.bnd_as_biarc(side);
    dbgl_biarc end_biarc = end_xfrag.bnd_as_biarc(side);

    if (start_biarc.flag() == -1 || end_biarc.flag() == -1)
    {
      fx[3*i] = fx[3*i+1] = fx[3*i+2] = vnl_numeric_traits<double >::maxval;
      continue;
    }

    double start_k[2]; // curvature of two arcs of start_biarc
    start_k[0] = start_biarc.k1();
    start_k[1] = (start_biarc.flag()==0) ? start_biarc.k1() : start_biarc.k2();
    
    double start_len[2]; // length of the two arcs of start_biarc
    // in degenerate cases, divide the length evenly betwen two arcs
    start_len[0] = (start_biarc.flag()==0) ? start_biarc.len()/2 : start_biarc.len1();
    start_len[1] = (start_biarc.flag()==0) ? start_biarc.len()/2 : start_biarc.len2();


    double end_k[2]; // curvature of two arcs of start_biarc
    end_k[0] = end_biarc.k1();
    end_k[1] = (end_biarc.flag()==0) ? end_biarc.k1() : end_biarc.k2();

    double end_len[2]; // length of the two arcs of end_biarc
    // in degenerate cases, divide the length evenly betwen two arcs
    end_len[0] = (end_biarc.flag()==0) ? end_biarc.len()/2 : end_biarc.len1();
    end_len[1] = (end_biarc.flag()==0) ? end_biarc.len()/2 : end_biarc.len2();


    // cost components: curvature difference^2 * len
    fx[3*i    ] = (start_k[1] - start_k[0]) * (start_len[0] + start_len[1]);
    fx[3*i + 1] = (end_k[0]   - start_k[1]) * (end_len[0] + start_len[1]  );
    fx[3*i + 2] = (end_k[1]   - end_k[0]  ) * (end_len[1] + end_len[0]    );
  }

  return;
}



//------------------------------------------------------------------------------
  //: Convert from 'x' to xnode descriptor
dbsksp_xshock_node_descriptor dbsksp_xfrag_sample_along_shock_biarc_cost_function::
x_to_xsample(const vnl_vector<double >& x) const
{
  // x[0]: radius
  double radius = x[0]; //> radius

  // x[1]: tangent angle
  double psi = x[1];
  double phi = x[2]; 
  return dbsksp_xshock_node_descriptor(this->init_xsample_.x(), 
    this->init_xsample_.y(), psi, phi, radius);
}



//------------------------------------------------------------------------------
//: Initial value for 'x'
vnl_vector<double > dbsksp_xfrag_sample_along_shock_biarc_cost_function::
initial_x() const
{
  vnl_vector<double > x(3, 0);
  x[0] = this->init_xsample_.radius();
  x[1] = this->init_xsample_.psi();
  x[2] = this->init_xsample_.phi();
  return x;
}

