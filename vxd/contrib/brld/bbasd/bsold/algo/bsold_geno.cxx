//:
// \file
// \author Ricardo Fabbri

#include "bsold_geno.h"
#include <bgld/algo/bgld_eulerspiral.h>
#include <vnl/vnl_math.h>
#include <cmath>

//: 2nd order GENO (Geometric Non-Oscillatory) Interpolation
void bsold_geno::
interpolate(bsold_geno_curve_2d *c, std::vector<vsol_point_2d_sptr> const &pts, bool closed) 
{
   if (pts.size() <= 2){
      std::cerr << "geno interpolation currently needs at least 2 points";
      return;
   }

   if (!closed) {
      std::vector<bgld_param_curve *> ints(pts.size()-1);
      unsigned i;
      bool fwd;

      c->stencil_.resize(ints.size());

      // do first interval separately
      ints[0] = new bgld_arc(pts[0]->get_p(),pts[1]->get_p(), pts[2]->get_p());
      c->set_upper(0,true);

      // do middle intervals
      for (i=1; i<ints.size()-1; ++i) {
         ints[i] = interpolate_interval ( pts[i-1]->get_p(), pts[i]->get_p(), 
               pts[i+1]->get_p(), pts[i+2]->get_p(), &fwd);
         c->set_upper(i,fwd);
      }
      // do last interval separately
      ints[i] = new bgld_arc(pts[i]->get_p(), pts[i+1]->get_p(), pts[i-1]->get_p());
      c->set_upper(i,false);

      c->make(ints,closed);
   } else {

      std::vector<bgld_param_curve *> ints(pts.size());
      unsigned i;
      bool fwd;

      c->stencil_.resize(ints.size());

      // do first interval separately
      ints[0] = interpolate_interval ( pts.back()->get_p(), pts[0]->get_p(), 
            pts[1]->get_p(), pts[2]->get_p(), &fwd);
      c->set_upper(0,fwd);

      // do middle intervals
      for (i=1; i<ints.size()-2; ++i) {
         ints[i] = interpolate_interval ( pts[i-1]->get_p(), pts[i]->get_p(), 
               pts[i+1]->get_p(), pts[i+2]->get_p(), &fwd);
         c->set_upper(i,fwd);
      }

      // do last interval
      ints[i] = interpolate_interval ( pts[i-1]->get_p(), pts[i]->get_p(), 
      pts[i+1]->get_p(), pts[0]->get_p(), &fwd);
      c->set_upper(i,fwd);

      // close cycle
      ++i;
      ints[i] = interpolate_interval ( pts[i-1]->get_p(), pts[i]->get_p(), 
      pts[0]->get_p(), pts[1]->get_p(), &fwd);
      c->set_upper(i,fwd);
 
      c->make(ints,closed);
   }
}

//: 3nd order GENO, approximate algorithm:
// 1) Compute tangents at each sample point as mean of left and right intervals,
// using circular GENO; 
//  - if left and right tangents are too different, the estimate is not reliable; 
//  so we output a vector of tangent differences so you can choose whether to
//  discard that tangent or not; or whether to place shocks
//
// 2) For each tangent pair, get the Euler spiral
//
// TODO: should call interpolate3_from_tangents, to avoid repetition of code
void bsold_geno::
interpolate3_approx(bsold_geno_curve_2d *c, std::vector<vsol_point_2d_sptr> const &pts, bool closed) 
{
  if (pts.size() <= 2){
     std::cerr << "interpolation needs least 2 points\n";
     return;
  } if (closed) {
     std::cerr << "closed curves not yet supported\n";
     return;
  }

  unsigned i;

  double delta_angle_threshold=vnl_math::pi_over_2;

  bsold_geno_curve_2d arc_geno;
  interpolate(&arc_geno,pts,closed);

  std::vector<double> tangents(pts.size());
  std::vector<bool> tangent_reliable(pts.size(),true);

  // --- TANGENTS ---
  tangents[0] = arc_geno[0].tangent_angle_at(0);
  for (i=1; i<pts.size()-1; ++i) {
    double angle0, angle1, mid; float delta;

    angle0 = arc_geno[i-1].tangent_angle_at(1);
    angle1 = arc_geno[i].tangent_angle_at(0);

    mid = (angle0 + angle1)/2;
    
    if (std::fabs(angle1 - angle0) > vnl_math::pi) {
      mid = mid + vnl_math::pi;
      delta = vnl_math::twopi - std::fabs(angle1 - angle0);
    } else
      delta = std::fabs(angle1 - angle0);

    if (delta > delta_angle_threshold)
      tangent_reliable[i] = false;
      
    tangents[i] = std::fmod(mid,vnl_math::twopi);
  }

  tangents[i] = arc_geno[i-1].tangent_angle_at(1);

  /* 
  std::ofstream f_points, f_tangents;

  f_points.open("/tmp/points.dat");
  f_tangents.open("/tmp/tangents.dat");
  for (i=0; i<pts.size(); ++i) {
     f_points << pts[i]->x() << " " << pts[i]->y() << std::endl;
     f_tangents << tangents[i] << std::endl;
  }
  f_points.close();
  f_tangents.close();
  */

  // --- INTERPOLATION ---

  std::vector<bgld_param_curve *> ints(pts.size()-1);

  /*
  for (i=0; i<ints.size(); ++i) {
    double angle0, angle1;
    if (tangent_reliable[i])
      angle0 = tangents[i];
    else
      angle0 = arc_geno[i].tangent_angle_at(0);

    if (tangent_reliable[i+1])
      angle1 = tangents[i+1];
    else 
      angle1 = arc_geno[i].tangent_angle_at(1);

    ints[i] = (bgld_param_curve *)(new bgld_eulerspiral(pts[i]->get_p(),angle0,pts[i+1]->get_p(),angle1));
  }
  */

  for (i=0; i<ints.size(); ++i) {
    double angle0, angle1;
    angle0 = tangents[i];
    angle1 = tangents[i+1];

    ints[i] = new bgld_eulerspiral(pts[i]->get_p(),angle0,pts[i+1]->get_p(),angle1);
    /* Debugging code:
    if (i ==743 || i == 742 || i == 744) {
      std::cout << "I: " << i << std::endl;
      std::cout.precision(25);
      std::cout << "angle0: " << angle0 << "   angle1: " << angle1 << std::endl;
      std::cout << "ang_ints0: " << ints[i]->tangent_angle_at(0) << 
               "    ang_ints1: " << ints[i]->tangent_angle_at(1) << std::endl;

      std::cout << "points:" << *(pts[i]) << " ; " << *(pts[i+1]) << std::endl;
      std::cout << "Ints size: " << ints.size() << std::endl;
      std::cout << "Kdot: " << (reinterpret_cast<bgld_eulerspiral*> (ints[i]))->gamma() << std::endl;
      std::cout << "K_start: " << ints[i]->curvature_at(0) << "   K_end:" << ints[i]->curvature_at(1);
      std::cout << "\n==========================\n\n\n" << std::endl;
    }
    */
  }

  c->make(ints,closed);

  c->order_ = 3;
  
  // dont care about stencil position in this implementation
  c->stencil_.resize(ints.size(),0);
}

//: 3nd order GENO, approximate algorithm, variant code:
//
// 1) Get a vector with tangent field 
//
// 2) For each tangent pair, get the Euler spiral
//
void bsold_geno::
interpolate3_from_tangents(
    bsold_geno_curve_2d *c, 
    std::vector<vsol_point_2d_sptr> const &pts, 
    std::vector<double> const &tangent_angles,
    bool closed) 
{
  if (pts.size() <= 2){
     std::cerr << "interpolation needs least 2 points\n";
     return;
  } if (closed) {
     std::cerr << "closed curves not yet supported\n";
     return;
  }

  unsigned i;

  // --- INTERPOLATION ---

  std::vector<bgld_param_curve *> ints(pts.size()-1);

  for (i=0; i<ints.size(); ++i) {
    double angle0, angle1;
    angle0 = tangent_angles[i];
    angle1 = tangent_angles[i+1];

    ints[i] = new bgld_eulerspiral(pts[i]->get_p(),angle0,pts[i+1]->get_p(),angle1);
  }

  c->make(ints,closed);

  c->order_ = 3;
  
  // dont care about stencil position in this implementation
  c->stencil_.resize(ints.size(),0);
}

