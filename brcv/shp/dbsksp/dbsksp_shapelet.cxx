// This is file shp/dbsksp/dbsksp_shapelet.cxx

//:
// \file




#include "dbsksp_shapelet.h"
#include <dbsksp/dbsksp_xshock_node_descriptor.h>


#include <vnl/vnl_math.h>
#include <dbgl/algo/dbgl_closest_point.h>
#include <dbnl/dbnl_sinc.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vgl/vgl_distance.h>




// ============================================================================
//                 dbsksp_shapelet 
// ============================================================================

const double dbsksp_shapelet_epsilon = 1e-12;

// ----------------------------------------------------------------------------
//: set the shapelet's parameters from starting xnode and (len, m, phi1)
bool dbsksp_shapelet::
set_from(const vgl_point_2d<double >& pt0, double r0, 
    const vgl_vector_2d<double >& tangent0, double phi0, double m0, double len, 
    double phi1)
{
  // compute the angle between the chord and tangent
  double sin_alpha0 = m0 * vcl_sin(phi0);
  if (vnl_math::abs(sin_alpha0) > 1 || vnl_math::abs(m0*vcl_sin(phi1)) > 1)
    return false;

  vgl_vector_2d<double> v0 = rotated(tangent0, -vcl_asin(sin_alpha0));
  this->set(pt0.x(), pt0.y(), vcl_atan2(v0.y(), v0.x()), r0, phi0, phi1, m0, len);
  return true;
}






// ----------------------------------------------------------------------------
//: tangent at the starting point
vgl_vector_2d<double> dbsksp_shapelet::
tangent_start() const
{
  return rotated(this->chord_dir(), this->alpha_start());
}




// ----------------------------------------------------------------------------
//: tangent at the end point
vgl_vector_2d<double > dbsksp_shapelet::
tangent_end() const
{
  return rotated(this->chord_dir(), this->alpha_end());
}






// ----------------------------------------------------------------------------
//: normalized radius increment
// ( = radius increment / chord_length)
double dbsksp_shapelet::
normalized_radius_increment() const
{
  double a0 = this->alpha_start() + this->phi_start();
  double a2 = this->alpha_end() + this->phi_end();
  return - vcl_cos((a0+a2)/2) / vcl_cos((a0-a2)/2);
}

// ----------------------------------------------------------------------------
//: difference in radius between the end node and start node
double dbsksp_shapelet::
radius_increment() const
{ 
  double dr_over_len = this->normalized_radius_increment();  
  return dr_over_len * this->chord_length();
}


//: Return the start one boundary arc: 0: left boundary, 1: right boundary
vgl_point_2d<double > dbsksp_shapelet::
bnd_start(int side) const
{
  assert(side==0 || side==1);
  double side_factor = (side==0) ? 1 : -1;

  double alpha_start = this->alpha_start();
  double phi_start = this->phi_start();
  
  // compute boundary points from shock point + tangent + r + phi
  vgl_point_2d<double > bnd_start = this->start() + 
    this->radius_start() * rotated(this->chord_dir(), alpha_start + side_factor*phi_start);

  return bnd_start;
}

//: Return the end one boundary arc: 0: left boundary, 1: right boundary
vgl_point_2d<double > dbsksp_shapelet::
bnd_end(int side) const
{
  assert(side==0 || side==1);
  double side_factor = (side==0) ? 1 : -1;

  double alpha_end = this->alpha_end();
  double phi_end = this->phi_end();

  vgl_point_2d<double > bnd_end = this->end() + 
    this->radius_end() * rotated(this->chord_dir(), alpha_end + side_factor*phi_end);

  return bnd_end;
}




// ------------------------------------------------------------------
//: Return boundary arcs: 0: left boundary, 1: right boundary
dbgl_circ_arc dbsksp_shapelet::
bnd_arc(int side) const
{
  assert(side==0 || side==1);
  double side_factor = (side==0) ? 1 : -1;

  double alpha_start = this->alpha_start();
  double phi_start = this->phi_start();
  
  double alpha_end = this->alpha_end();
  double phi_end = this->phi_end();


  
  //// Compute boundary arc
  vgl_point_2d<double > bnd_start = this->start() + 
    this->radius_start() * rotated(this->chord_dir(), alpha_start + side_factor*phi_start);

  vgl_point_2d<double > bnd_end = this->end() + 
    this->radius_end() * rotated(this->chord_dir(), alpha_end + side_factor*phi_end);

  vgl_vector_2d<double > bnd_start_tangent = 
    rotated(this->chord_dir(), alpha_start+side_factor*(phi_start-vnl_math::pi_over_2));

  dbgl_circ_arc bnd_arc;
  bnd_arc.set_from(bnd_start, bnd_start_tangent, bnd_end);
  return bnd_arc;
}

// ------------------------------------------------------------------
dbgl_conic_arc dbsksp_shapelet::
shock_geom() const
{
  // Compute the shock geometry ( a conic)
  vgl_vector_2d<double > shock_start_tangent = 
    rotated(this->chord_dir(), this->alpha_start());
  vgl_vector_2d<double > shock_end_tangent = 
    -rotated(-this->chord_dir(), this->alpha_end());

  dbgl_circ_arc left_bnd = this->bnd_arc(0);
  dbgl_circ_arc right_bnd = this->bnd_arc(1);

  // curvature
  double kbar_right = right_bnd.k() / ( 1 - this->radius_start()*right_bnd.k());
  double kbar_left = -left_bnd.k() / (1 - this->radius_start()*(-left_bnd.k()));

  double shock_start_k = (-kbar_left + kbar_right) * vcl_sin(this->phi_start()) /2;

  dbgl_conic_arc conic;
  conic.set_from(this->start(), this->chord_dir(), this->chord_length(), 
    shock_start_tangent, shock_end_tangent, shock_start_k);
  return conic;
}




//: Return area bounded by the shapelet
double dbsksp_shapelet::
area() const
{
  return this->area_left() + this->area_right();
}

//: Return the area of the ``left'' region, bounded by the left boundary,
// shock chord, and the contact shocks
double dbsksp_shapelet::
area_left() const
{
  dbgl_circ_arc arc = this->bnd_arc_left();
  // area of the quadrilateral
  // let the central angle spanning the left arc be a = k * s
  // let R be the radius of the left arc
  // area =  1/2 * (R + r1) * (R + r2) - 1/2 R^2 * sin(a)
  //      = 1/2 * ( (r1+r2)R + r1 * r2) sin(a)
  //      = 1/2 * ( (r1+r2)* s * sin(a) / a + r1 * r2 * sin(a) )
  double r1 = this->radius_start();
  double r2 = this->radius_end();
  double s = arc.length();
  double a = arc.k() * s; 
  double area_quad = ( (r1+r2)*s* dbnl_sinc(a) + r1*r2*vcl_sin(a) ) / 2;

  double area_arc_segment = arc.area();
  return area_quad - vnl_math::sgn(arc.k()) * area_arc_segment;
}

//: Return the area of the ``right'' region, bounded by the left boundary,
// shock chord, and the contact shocks
double dbsksp_shapelet::
area_right() const
{
  dbgl_circ_arc arc = this->bnd_arc_right();
  // area of the quadrilateral
  // let the central angle spanning the left arc be a = k * s
  // let R be the radius of the left arc
  // this formular is for k > 0 but the final formula is also true for k < 0
  // area =  1/2 R^2 * sin(a) - 1/2 * (R - r1) * (R - r2) 
  //      = 1/2 * (r1+r2)R  - (1/2) * r1 * r2 * sin(a)
  //      = 1/2 * (r1+r2)* s * sin(a) / a - (1/2) * r1 * r2 * sin(a)
  double r1 = this->radius_start();
  double r2 = this->radius_end();
  double s = arc.length();
  double a = arc.k() * s; 
  double area_quad = ( (r1+r2)*s* dbnl_sinc(a) - r1*r2*vcl_sin(a) ) / 2;
  double area_arc_segment = arc.area();
  
  return area_quad + vnl_math::sgn(arc.k()) * area_arc_segment;
}







// ----------------------------------------------------------------------------
//: Return the area of the front arc of the shapelet
double dbsksp_shapelet::
area_front_arc() const
{
  double sector_angle = 2 * (vnl_math::pi - this->phi_start());
  double radius = this->radius_start();
  double sector_area = radius * radius * sector_angle / 2;

  // the triangle area is negative when the sector is bigger than pi, so we
  // this formula still aplies for sectors > pi
  double triangle_area = radius * radius * vcl_sin(sector_angle) / 2;
  return sector_area - triangle_area;
}


// ----------------------------------------------------------------------------
//: Return the area of the rear arc of the shapelet
double dbsksp_shapelet::
area_rear_arc() const
{
  double sector_angle = 2 * this->phi_end();
  double radius = this->radius_end();
  double sector_area = radius * radius * sector_angle /2;

  // the triangle area is negative when the sector is bigger than pi, so we
  // this formula still aplies for sectors > pi
  double triangle_area = radius * radius * vcl_sin(sector_angle) / 2;
  return (sector_area - triangle_area);

}




//: Return angle phi at a shock point
double dbsksp_shapelet::
phi_at(double t) const
{
  // shock curve (conic) and shock point
  dbgl_conic_arc shock_curve = this->shock_geom();
  vgl_point_2d<double > shock_pt = shock_curve.point_at(t);

  // boundary arc
  dbgl_circ_arc left_bnd = this->bnd_arc_left();

  // contact shock
  double left_ratio = -1;
  dbgl_closest_point::point_to_circular_arc(shock_pt, 
    left_bnd.point1(), left_bnd.point2(), left_bnd.k(), left_ratio);
  vgl_point_2d<double > left_bnd_pt = left_bnd.point_at(left_ratio);

  // tangent of at shock point
  vgl_vector_2d<double > shock_dir = shock_curve.tangent_at(t);

  vgl_vector_2d<double > contact_shock = left_bnd_pt - shock_pt;
  double phi = angle(shock_dir, contact_shock);

  return phi;
}



//: Return radius at a shock point
double dbsksp_shapelet::
radius_at(double t) const
{
  // shock curve (conic) and shock point
  dbgl_conic_arc shock_curve = this->shock_geom();
  vgl_point_2d<double > shock_pt = shock_curve.point_at(t);

  // boundary arc
  dbgl_circ_arc left_bnd = this->bnd_arc_left();

  // contact shock
  double left_ratio = -1;
  dbgl_closest_point::point_to_circular_arc(shock_pt, 
    left_bnd.point1(), left_bnd.point2(), left_bnd.k(), left_ratio);
  vgl_point_2d<double > left_bnd_pt = left_bnd.point_at(left_ratio);

  double radius = vgl_distance(shock_pt, left_bnd_pt);
  return radius;
}








////------------------------------------------------------------------------------
////: Compute an extrinsic shock sample
//dbsksp_xshock_node_descriptor dbsksp_shapelet::
//sample_at(double t)
//{
//  return dbsksp_xshock_node_descriptor();  
//}





//------------------------------------------------------------------------------
//: Return the quadrilateral connecting the boundary end points
vsol_polygon_2d_sptr dbsksp_shapelet::
bounding_quad() const
{
  vcl_vector<vsol_point_2d_sptr > pts;
  pts.push_back(new vsol_point_2d(this->bnd_start(0)));
  pts.push_back(new vsol_point_2d(this->bnd_start(1)));
  pts.push_back(new vsol_point_2d(this->bnd_end(1)));
  pts.push_back(new vsol_point_2d(this->bnd_end(0)));
  
  vsol_polygon_2d_sptr quad = new vsol_polygon_2d(pts);
  return quad;
}






//------------------------------------------------------------------------------
//: Compute extrinsic shock samples given a list of parameter t, t \in [0, 1]
// Return false if computation fails, e.g. t < 0 or t > 1
bool dbsksp_shapelet::
compute_xshock_samples(const vcl_vector<double >& ts,
                       vcl_vector<dbsksp_xshock_node_descriptor >& list_xdesc)
{
  list_xdesc.clear();
  list_xdesc.reserve(ts.size());

  // Take case of degenerate case: the two end shock points coincide
  if (this->chord_length() < dbsksp_shapelet_epsilon)
  {
    // The two boundary have the same center, which is the shock point!
    // Since any pair of points on the two boundary forms a symmetric point-pair
    // we sample uniformly on the two boundary arcs instead of the degenerate shock curve
    
    // boundary arc
    dbgl_circ_arc left_bnd = this->bnd_arc_left();
    dbgl_circ_arc right_bnd = this->bnd_arc_right();
    vgl_point_2d<double > pt = this->start();

    // sample uniformly on the two arc
    for (unsigned i =0; i < ts.size(); ++i)
    {
      double t = ts[i];
      vgl_point_2d<double > left = left_bnd.point_at(t);
      vgl_point_2d<double > right = right_bnd.point_at(t);
      list_xdesc.push_back(dbsksp_xshock_node_descriptor(pt, left, right));
    }
  }
  // normal case
  else
  {
    // shock curve (conic) and shock point
    dbgl_conic_arc shock_curve = this->shock_geom();

    // boundary arc
    dbgl_circ_arc left_bnd = this->bnd_arc_left();
    dbgl_circ_arc right_bnd = this->bnd_arc_right();

    for (unsigned i =0; i < ts.size(); ++i)
    {
      double t = ts[i];

      // sanity check
      if (t < 0 || t > 1)
      {
        list_xdesc.clear();
        return false;
      }

      vgl_point_2d<double > shock_pt = shock_curve.point_at(t);

      double left_ratio = -1;
      dbgl_closest_point::point_to_circular_arc(shock_pt, 
        left_bnd.point1(), left_bnd.point2(), left_bnd.k(), left_ratio);
      vgl_point_2d<double > left_bnd_pt = left_bnd.point_at(left_ratio);

      vgl_vector_2d<double > n = left_bnd_pt - shock_pt;
      vgl_vector_2d<double > shock_tangent = shock_curve.tangent_at(t);
      double radius = n.length();
      double phi = signed_angle(shock_tangent, n);

      dbsksp_xshock_node_descriptor sample(shock_pt.x(), shock_pt.y(),
        vcl_atan2(shock_tangent.y(), shock_tangent.x()), phi, radius);

      list_xdesc.push_back(sample);
    }
  }
  
  return true;
}


////: Compute point samples of the shapelet, given sampling rate "ds"
//vcl_vector<vgl_point_2d<double > > dbsksp_shapelet::
//compute_samples(double ds) const
//{
//  vcl_vector<vgl_point_2d<double > > pts;
//  for (int i=0; i<2; ++i)
//  {
//    dbgl_circ_arc arc = this->bnd_arc(i);
//    // sample at the middle of the interval
//    for (double s = ds/2; s<arc.len(); s += ds)
//    {
//      pts.push_back(arc.point_at_length(s));
//    }
//  }
//  return pts;
//}




// UTILITY FUNCTIONS----------------------------------------------------------


// ----------------------------------------------------------------------------
dbsksp_shapelet_sptr dbsksp_shapelet::
reversed_dir() const
{
  double x0 = this->end().x();
  double y0 = this->end().y();
  double theta0 = vcl_atan2(-vcl_sin(this->theta0()), -vcl_cos(this->theta0()) );
  double r0 = this->radius_end();
  double phi0 = vnl_math::pi - this->phi_end();
  double phi1 = vnl_math::pi - this->phi_start();
  double len = this->chord_length();
  double m0 = -this->m0();
  return new dbsksp_shapelet(x0, y0, theta0, r0, phi0, phi1, m0, len);
}




// ----------------------------------------------------------------------------
//: Create a scaled-up version of ``this'' shapelet. When
// scale > 1, the resulting shapelet is bigger
// scale < 1, the resulting shapelet is smaller
dbsksp_shapelet_sptr dbsksp_shapelet::
scale_up(double scale) const
{
  return new dbsksp_shapelet(this->x0(), this->y0(), this->theta0(), this->r0() * scale,
    this->phi0(), this->phi1(), this->m0(), this->len() * scale);
}









//: Return the front terminal shapelet (A_infty shock point)
dbsksp_shapelet_sptr dbsksp_shapelet::
terminal_shapelet_front() const
{
  double x0 = this->start().x();
  double y0 = this->start().y();
  vgl_vector_2d<double > t0 = -this->tangent_start();
  double theta0 = vcl_atan2(t0.y(), t0.x());

  double r0 = this->radius_start();
  double phi0 = vnl_math::pi - this->phi_start();
  double phi1 = 0;
  double m0 = 0;
  double len = 0;
  return new dbsksp_shapelet(x0, y0, theta0, r0, phi0, phi1, m0, len);
}

//: Return the front terminal shapelet (A_infty shock point)
dbsksp_shapelet_sptr dbsksp_shapelet::
terminal_shapelet_rear() const
{
  double x0 = this->end().x();
  double y0 = this->end().y();
  vgl_vector_2d<double > t0 = this->tangent_end();
  double theta0 = vcl_atan2(t0.y(), t0.x());

  double r0 = this->radius_end();
  double phi0 = this->phi_end();
  double phi1 = 0;
  double m0 = 0;
  double len = 0;
  return new dbsksp_shapelet (x0, y0, theta0, r0, phi0, phi1, m0, len);
}





// ------------------------------------------------------------------
//: Check legality of this shapelet
bool dbsksp_shapelet::
is_legal() const
{
  bool legal = true;

  // radius >= 0
  legal = legal && (this->radius_start() >=0 ) && (this->radius_end() >= 0);

  // 0 <= phi <= pi
  legal = legal && (this->phi_start() >= 0) && (this->phi_start() <= vnl_math::pi);
  legal = legal && (this->phi_end() >= 0) && (this->phi_end() <= vnl_math::pi);

  // length >= 0
  legal = legal && (this->chord_length() >= 0);

  // |m*sin(phi) <= 1|
  legal = legal && (vnl_math::abs( vcl_sin(this->phi_start()) * this->m_start() ) <= 1); 
  legal = legal && (vnl_math::abs( vcl_sin(this->phi_end()) * this->m_end() ) <= 1);

  // max_radius condition
  //  when R is too big, self-intersecting happens
  // R1 * sin(pi - theta0 - theta2) < d * sin(theta2)
  
  // left side check
  double theta_start_left = this->phi_start() + this->alpha_start();
  double theta_end_left = vnl_math::pi - (this->phi_end() + this->alpha_end());
  legal = legal && 
    (this->radius_start()*vcl_sin(theta_start_left+theta_end_left) <= 
    this->chord_length()*vcl_sin(theta_end_left));

  // right side check
  double theta_start_right = this->phi_start() - this->alpha_start();
  double theta_end_right = vnl_math::pi - this->phi_end() + this->alpha_end();
  legal = legal &&
    (this->radius_start()*vcl_sin(theta_start_right+theta_end_right) <=
    this->chord_length()* vcl_sin(theta_end_right));

  // min radius condition
  // left and right boundary do not intersect

  // There has to be a better way to do this
  // For now, use the boundary explicitly
  dbgl_circ_arc left_arc = this->bnd_arc(0);
  dbgl_circ_arc right_arc = this->bnd_arc(1);

  vcl_vector<double > left_ratios;
  vcl_vector<double > right_ratios;

  legal = legal &&
    ( dbgl_closest_point::circular_arc_to_circular_arc(
    left_arc.point1(), left_arc.point2(), left_arc.k(), 
    right_arc.point1(), right_arc.point2(), right_arc.k(), left_ratios, right_ratios)
    > 0);
  return legal;
}


// -----------------------------------------------------------------
//: illegal cost. >0 is good. < 0 is illegal
vnl_vector<double > dbsksp_shapelet::
legality_measure() const
{
  vnl_vector<double > a(5, 0);

  // radius >= 0
  a(0) = this->radius_start();
  a(1) = this->radius_end(); 

  // length >= 0
  a(2) = this->chord_length();

  // max_radius condition
  //  when R is too big, self-intersecting happens
  // R1 * sin(pi - theta0 - theta2) < d * sin(theta2)
  
  // left side check
  double theta_start_left = this->phi_start() + this->alpha_start();
  double theta_end_left = vnl_math::pi - (this->phi_end() + this->alpha_end());
  a(3) = - this->radius_start()*vcl_sin(theta_start_left+theta_end_left) +
    this->chord_length()*vcl_sin(theta_end_left);

  // right side check 
  double theta_start_right = this->phi_start() - this->alpha_start();
  double theta_end_right = vnl_math::pi - this->phi_end() + this->alpha_end();
  a(4) = -this->radius_start()*vcl_sin(theta_start_right+theta_end_right) +
    this->chord_length()* vcl_sin(theta_end_right);

  return a;
}








// --------------------- MISCELLANEOUS ---------------------

// ------------------------------------------------------------------
//: write info of the dbskbranch to an output stream
void dbsksp_shapelet::
print(vcl_ostream & os)
{
  os << "<< dbsksp_shapelet \n"
    << "x0 [ " << this->x0() << " ]\n"
    << "y0 [ " << this->y0() << " ]\n"
    << "theta0[ " << this->theta0() << " ]\n"
    << "r0[ " << this->r0() << " ]\n"
    << "phi0[ " << this->phi0() << " ]\n"
    << "phi1[ " << this->phi_end() << " ]\n"
    << "m0[ " << this->m0() << " ]\n"
    << "len[ " << this->len() << " ]\n";
  return;
}

















// ============================================================================
//                  dbsksp_twoshapelet
// ============================================================================

//: Construct a twoshapelet by breaking an existing shapelets into two pieces
dbsksp_twoshapelet::
dbsksp_twoshapelet(const dbsksp_shapelet_sptr& s, double t)
{
  
  double x0 = s->x0();
  double y0 = s->y0();
  double theta0;
  double r0 = s->radius_start();
  double phi0 = s->phi0();
  double m0;
  double len0; 
  double phi1 = s->phi_at(t);
  double m1;
  double len1;
  double phi2 = s->phi_end(); 

  // len0
  vgl_point_2d<double > pt0 = s->start();
  dbgl_conic_arc conic = s->shock_geom();
  vgl_point_2d<double > pt1 = conic.point_at(t);
  len0 = vgl_distance(pt0, pt1);

  // thet0
  vgl_vector_2d<double > chord_dir0 = (len0 < dbsksp_shapelet_epsilon) ? 
    conic.tangent_at(0) : normalized(pt1-pt0);
  theta0 = vcl_atan2(chord_dir0.y(), chord_dir0.x());

  // m0
  vgl_vector_2d<double > shock_dir0 = conic.tangent_at(0);
  double sin_alpha0 = cross_product(chord_dir0, shock_dir0);
  m0 = sin_alpha0 / vcl_sin(phi0);

  // len1
  vgl_point_2d<double > pt2 = s->end();
  len1 = vgl_distance(pt1, pt2);

  // m1
  vgl_vector_2d<double > chord_dir1 = (len1 < dbsksp_shapelet_epsilon) ? 
    conic.tangent_at(1) : normalized(pt2-pt1);
  vgl_vector_2d<double > shock_dir1 = conic.tangent_at(1);
  double sin_alpha2 = cross_product(chord_dir1, shock_dir1);
  m1 = -sin_alpha2 / vcl_sin(phi2);

  // set params of ``this'' twoshapelet
  this->set(x0, y0, theta0, r0, phi0, m0, len0, phi1, m1, len1, phi2);

  return;
}











// ----------------------------------------------------------------------------
//                    Geometry
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
dbsksp_shapelet_sptr dbsksp_twoshapelet::
shapelet_start() const
{
  return new dbsksp_shapelet(this->x0(), this->y0(), this->theta0(), this->r0(), this->phi0(),
    this->phi1(), this->m0(), this->len0()); 
}


// ----------------------------------------------------------------------------
dbsksp_shapelet_sptr dbsksp_twoshapelet::
shapelet_end() const
{
  dbsksp_shapelet_sptr shapelet_start = this->shapelet_start();
  double x1 = shapelet_start->end().x();
  double y1 = shapelet_start->end().y();

  double alpha12 = vcl_asin(this->m1()*vcl_sin(this->phi1()));
  vgl_vector_2d<double > chord_dir_1 = 
    rotated(shapelet_start->shock_geom().tangent_at(1), -alpha12);
  double theta1 = vcl_atan2(chord_dir_1.y(), chord_dir_1.x());
  
  double r1 = shapelet_start->radius_end();

  return new dbsksp_shapelet(x1, y1, theta1, r1, 
    this->phi1(), this->phi2(), 
    this->m1(), this->len1());
}


// ----------------------------------------------------------------------------
dbsksp_shapelet_sptr dbsksp_twoshapelet::
shapelet(int i) const
{
  if (i==0)
    return this->shapelet_start();
  else
    return this->shapelet_end();
}


// ----------------------------------------------------------------------------
//                      UTILITY FUNCTIONS 
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//: Check legality of this shapelet
bool dbsksp_twoshapelet::
is_legal() const
{
  return (this->shapelet_start()->is_legal() && this->shapelet_end()->is_legal());
}


// ----------------------------------------------------------------------------
//: Return a twoshapelet of reversed direction
dbsksp_twoshapelet_sptr dbsksp_twoshapelet::
reversed_dir() const
{
  dbsksp_shapelet_sptr start = this->shapelet_end()->reversed_dir();
  dbsksp_shapelet_sptr end = this->shapelet_start()->reversed_dir();
  return new dbsksp_twoshapelet(start->x0(), start->y0(), start->theta0(), start->r0(),
    start->phi0(), start->m0(), start->len(), 
    end->phi0(), end->m0(), end->len(), 
    end->phi_end());
}

// ----------------------------------------------------------------------------
//                    MISCELLANEOUS
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
//: write info of the dbskbranch to an output stream
void dbsksp_twoshapelet::
print(vcl_ostream & os)
{
  os << "<< dbsksp_twoshapelet \n"
    << "x0 [ " << this->x0() << " ]\n"
    << "y0 [ " << this->y0() << " ]\n"
    << "theta0[ " << this->theta0() << " ]\n"
    << "r0[ " << this->r0() << " ]\n"
    << "phi0[ " << this->phi0() << " ]\n"
    << "m0[ " << this->m0() << " ]\n"
    << "len0[ " << this->len0() << " ]\n"
    << "phi1[ " << this->phi1() << " ]\n"
    << "m1[ " << this->m1() << " ]\n"
    << "len1[ " << this->len1() << " ]\n"
    << "phi2[ " << this->phi2() << " ]\n";

  return;
}

  






// ============================================================================
// dbsksp_terminal_shapelet
// ============================================================================

void dbsksp_terminal_shapelet::
set(double x0, double y0, double theta0, double r0, double phi0)
{
  this->params_[0] = x0;
  this->params_[1] = y0;
  this->params_[2] = theta0;
  this->params_[3] = r0;
  this->params_[4] = phi0;
}

void dbsksp_terminal_shapelet::
set(const vnl_vector<double >& params)
{
  assert(params.size() == this->params_.size());
  this->params_.copy_in(params.data_block());
}

    
//: Return boundary arcs: 0: left boundary, 1: right boundary
dbgl_circ_arc dbsksp_terminal_shapelet::
bnd_arc(int i) const
{
  dbgl_circ_arc arc;
  //arc.set_from(
  return arc;
}

//: Return area bounded by the shapelet
double dbsksp_terminal_shapelet::
area() const
{
  return 0;
}

  
//: Check legality of this shapelet
bool dbsksp_terminal_shapelet::
is_legal() const
{
  return 0;
}

//: illegal cost. >0 is good. < 0 is illegal
vnl_vector<double > dbsksp_terminal_shapelet::
legality_measure() const
{
  return vnl_vector<double >();
}


//: write info of the dbskbranch to an output stream
void dbsksp_terminal_shapelet::
print(vcl_ostream & os)
{
  return;
}

  







//------------------------------------------------------------------------------
//: Create a shapelet that is a terminal fragment
// All parameters are for the start node (the non-trivial node)
dbsksp_shapelet_sptr dbsksp_new_terminal_shapelet(double x0, double y0, double theta0, 
                                                  double r0, double phi0)
{
  double phi1 = 0;
  double m0 = 0;
  double len = 0;
  return new dbsksp_shapelet(x0, y0, theta0, r0, phi0, phi1, m0, len);
}


