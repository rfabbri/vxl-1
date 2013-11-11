// This is file shp/dbsksp/dbsksp_xshock_node.cxx

//:
// \file

#include "dbsksp_xshock_node_descriptor.h"
#include <vcl_algorithm.h>
#include <vcl_utility.h>
#include <vnl/vnl_math.h>
#include <dbnl/dbnl_angle.h>
#include <vgl/vgl_distance.h>

#include <dbsksp/dbsksp_xshock_edge.h>
#include <dbsksp/dbsksp_xshock_edge_sptr.h>


// ----------------------------------------------------------------------------
//: Constructor 1
dbsksp_xshock_node_descriptor::
dbsksp_xshock_node_descriptor(double x, double y, double psi, double phi, 
                              double radius) : 
pt_(x, y), psi_(psi), phi_(phi), radius_(radius)
{
  // make sure psi is between [0, 2pi]
  this->psi_ = dbnl_angle_0to2pi(psi);
  return;
}


// ----------------------------------------------------------------------------
//: Constructor 2
dbsksp_xshock_node_descriptor::
dbsksp_xshock_node_descriptor(const vgl_point_2d<double >& pt,
                              const vgl_point_2d<double >& bnd_pt_left, 
                              const vgl_point_2d<double >& bnd_pt_right)
{
  // This is a over-constraint problem since 6 numbers are specified while
  // we have only 5 parameters for a xshock_node_descriptor
  // Need to compropose some variable ...

  // Decision: we trust the position of the boundary points more than the shock points

  // i. phi
  double phi_x_2 = signed_angle(bnd_pt_right - pt, bnd_pt_left - pt);

  // make sure phi_x_2 is in correct range, i.e., [0, 2*pi]
  if (phi_x_2 < 0) phi_x_2 += 2*vnl_math::pi;
  this->phi_ = phi_x_2 / 2;

  // ii. shock tangent angle - orthogonal to the line connecting two boundary points
  vgl_vector_2d<double > shock_tangent = rotated(bnd_pt_right - pt, this->phi_);
  this->psi_ = vcl_atan2(shock_tangent.y(), shock_tangent.x());

  // iii. shock point
  this->pt_ = centre(bnd_pt_left, bnd_pt_right) - shock_tangent * vcl_cos(this->phi_);

  // iv. radius
  this->radius_ = vgl_distance(bnd_pt_right, this->pt_);
}



// ----------------------------------------------------------------------------
//: Constructor 3
dbsksp_xshock_node_descriptor::
dbsksp_xshock_node_descriptor(const vgl_point_2d<double >& pt, 
                              const vgl_vector_2d<double >& tangent, double phi, double radius)
{
  this->pt_ = pt;
  this->psi_ = vcl_atan2(tangent.y(), tangent.x());
  this->phi_ = phi;
  this->radius_ = radius;
}



//-----------------------------------------------------------------------------
//: set parameters
void dbsksp_xshock_node_descriptor::
set(const vgl_point_2d<double >& bnd_pt_left, 
    const vgl_point_2d<double >& bnd_pt_right, double phi)
{
  // tangent is is orthogonal to chord connecting two boundary points
  vgl_vector_2d<double > v = rotated((bnd_pt_right - bnd_pt_left)/2, vnl_math::pi_over_2);
  vgl_vector_2d<double > tangent = normalized(v);

  // radius is related to distance between two boundary points and phi
  double radius = v.length() / vcl_sin(phi);

  // shock point is computed from chordal point (mid point of chord) and tangent
  vgl_point_2d<double > chordal_pt = centre(bnd_pt_left, bnd_pt_right);
  vgl_point_2d<double > shock_pt = chordal_pt - vcl_cos(phi) * radius * tangent;

  // put everything together
  this->set(shock_pt.x(), shock_pt.y(), vcl_atan2(tangent.y(), tangent.x()), phi, radius); 
  return;
}


// ----------------------------------------------------------------------------
//: Shock tangent
vgl_vector_2d<double > dbsksp_xshock_node_descriptor::
shock_tangent() const
{
  return vgl_vector_2d<double >(vcl_cos(this->psi_), vcl_sin(this->psi_));
}


// ----------------------------------------------------------------------------
//: Set shock tangent with a vector
void dbsksp_xshock_node_descriptor::
set_shock_tangent(vgl_vector_2d<double > t)
{
  this->psi_ = vcl_atan2(t.y(), t.x());
}


// ----------------------------------------------------------------------------
//: Set shock tangent with an angle value
void dbsksp_xshock_node_descriptor::
set_shock_tangent(double psi)
{
  this->psi_ = dbnl_angle_0to2pi(psi);
}



// ----------------------------------------------------------------------------
//: left boundary point
vgl_point_2d<double > dbsksp_xshock_node_descriptor::
bnd_pt_left() const
{
  double a = this->psi_ + this->phi_;
  return this->pt_ + this->radius_*vgl_vector_2d<double >(vcl_cos(a), vcl_sin(a));
  
}


// ----------------------------------------------------------------------------
//: right boundary point
vgl_point_2d<double > dbsksp_xshock_node_descriptor::
bnd_pt_right() const
{
  double a = this->psi_ - this->phi_;
  return this->pt_ + this->radius_*vgl_vector_2d<double >(vcl_cos(a), vcl_sin(a));
}

//: Mid-pt of line segment connecting two boundary points
vgl_point_2d<double > dbsksp_xshock_node_descriptor::
bnd_mid_pt() const
{
  double d = this->radius() * vcl_cos(this->phi());
  return this->pt() + d * this->shock_tangent();
}


// ----------------------------------------------------------------------------
//: left boundary tangent
vgl_vector_2d<double > dbsksp_xshock_node_descriptor::
bnd_tangent_left() const
{
  double angle = this->psi_ + this->phi_ - vnl_math::pi_over_2;
  return vgl_vector_2d<double >(vcl_cos(angle), vcl_sin(angle));
}


// ----------------------------------------------------------------------------
//: right boundary tangent
vgl_vector_2d<double > dbsksp_xshock_node_descriptor::
bnd_tangent_right() const
{
  double angle = this->psi_ - this->phi_ + vnl_math::pi_over_2;
  return vgl_vector_2d<double >(vcl_cos(angle), vcl_sin(angle));
}



// ----------------------------------------------------------------------------
//: Return opposite of this xnode
dbsksp_xshock_node_descriptor dbsksp_xshock_node_descriptor::
opposite_xnode() const
{
  return dbsksp_xshock_node_descriptor(
    this->pt().x(), this->pt().y(), this->psi_ + vnl_math::pi, 
    (vnl_math::pi - this->phi_), this->radius_);
}

// ----------------------------------------------------------------------------
//: Print description of the xnode to an output stream
void dbsksp_xshock_node_descriptor::
print(vcl_ostream& os) const
{
  os << "dbsksp_xshock_node_descriptor"
    << " x[ " << this->pt().x() << " ]"
    << " y[ " << this->pt().y() << " ]"
    << " psi[ " << this->psi_ << " ]"
    << " phi[ " << this->phi_ << " ]"
    << " radius " << this->radius_ << " ]\n";
}





