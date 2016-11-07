// This is shp/dbsksp/dbsksp_xshock_node_descriptor.h
#ifndef dbsksp_xshock_node_descriptor_h_
#define dbsksp_xshock_node_descriptor_h_

//:
// \file
// \brief An extrinsic shock node descriptor
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date 2/10/2005
// \verbatim
//   Modifications:
//     Aug 2, 2008: added dbsksp_xshock_node_descriptor class 
// \endverbatim


#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vgl/vgl_point_2d.h>
#include <vcl_cmath.h>

// ============================================================================
// dbsksp_xshock_node_descriptor
// ============================================================================

class dbsksp_xshock_node_descriptor
{
public:
  //: Constructor 1
  dbsksp_xshock_node_descriptor(){};
  dbsksp_xshock_node_descriptor(double x, double y, double psi, double phi, 
    double radius);

  

  //: Constructor 2
  dbsksp_xshock_node_descriptor(const vgl_point_2d<double >& pt,
    const vgl_point_2d<double >& bnd_pt_left, 
    const vgl_point_2d<double >& bnd_pt_right);

  //: Constructor 3
  dbsksp_xshock_node_descriptor(const vgl_point_2d<double >& pt, 
    const vgl_vector_2d<double >& tangent, double phi, double radius);

  //: Destructor
  ~dbsksp_xshock_node_descriptor(){};

public:
  vgl_point_2d<double > pt_;
  double psi_;
  double phi_;
  double radius_;

  // Geometry

  //: set all parameters
  void set(double x, double y, double psi, double phi, double radius)
  {
    this->pt_.set(x,y);
    this->psi_ = psi;
    this->phi_ = phi;
    this->radius_ = radius;
    return;
  }

  //: Get all parameters
  void get(double& x, double& y, double& psi, double& phi, double& radius) const
  {
    x = this->pt_.x();
    y = this->pt_.y();
    psi = this->psi_;
    phi = this->phi_;
    radius = this->radius_;
  }

  //: set parameters
  void set(const vgl_point_2d<double >& bnd_pt_left, 
    const vgl_point_2d<double >& bnd_pt_right, double phi);

  //: Return phi
  double phi() const {return this->phi_; }

  //: Set phi
  void set_phi(double phi) 
  {
    this->phi_ = phi; 
  }

  //: Return radius
  double radius() const {return this->radius_; }

  //: Set radius
  void set_radius(double radius)
  {
    this->radius_ = radius;
  }

  //: Chordal width (distance between two boundary points)
  double chordal_radius() const
  {
    return this->radius() * vcl_sin(this->phi());
  }

  

  //: Shock tangent
  vgl_vector_2d<double > shock_tangent() const;

  //: Shock tangent angle

  double shock_tangent_angle() const
  { return this->psi_; }

  double psi() const
  { return this->psi_; }

  //: Set shock tangent with a vector
  void set_shock_tangent(vgl_vector_2d<double > t);

  //: Set shock tangent with an angle value
  void set_shock_tangent(double psi);

  //: Shock point
  vgl_point_2d<double > pt() const 
  { return this->pt_; }

  //: Set shock point
  void set_pt(const vgl_point_2d<double >& pt)
  {
    this->pt_ = pt;
  }

  //: x-coordinate of shock point
  double x() const
  { return this->pt_.x(); }

  //: y-coordinate of shock point
  double y() const
  { return this->pt_.y(); }
  
  //: left boundary point
  vgl_point_2d<double > bnd_pt_left() const;

  //: right boundary point
  vgl_point_2d<double > bnd_pt_right() const;

  //: Mid-pt of line segment connecting two boundary points
  vgl_point_2d<double > bnd_mid_pt() const;

  //: left boundary tangent
  vgl_vector_2d<double > bnd_tangent_left() const;

  //: right boundary tangent
  vgl_vector_2d<double > bnd_tangent_right() const;

  //: Return opposite of this xnode
  dbsksp_xshock_node_descriptor opposite_xnode() const;

  //: Print description of the xnode to an output stream
  void print(vcl_ostream& os) const;
};

#endif // shp/dbsksp/dbsksp_xshock_node_descriptor.h
