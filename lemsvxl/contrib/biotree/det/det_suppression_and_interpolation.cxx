#include "det_suppression_and_interpolation.h"

#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vbl/vbl_array_2d.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_qr.h>
#include <bio_defs.h>
#include <biob/biob_explicit_neighborhood_structure.h>
#include <biob/biob_explicit_worldpt_roster.h>

/* Procedures needed:
   - given values on a plane, fit a quadratic surface and return the point where
     the maximum is achieved
   class needed:
   - given a 3d point p0 and a normal n, construct a structure that maps 
     3d points to local coordinates in the plane through p0 normal to n
*/

class plane_coordinate_system {
private:
  vgl_vector_3d<double> v_;
  vgl_vector_3d<double> w_;
  vgl_point_3d<double> p0_;
public:
  plane_coordinate_system(vgl_vector_3d<double> normal, worldpt p0)
    : p0_(p0) {
    //select vectors defining a coordinate system within the plane
    double a=normal.x(); double b = normal.y(); double c = normal.z();
    vgl_vector_3d<double> v(c, 0., -a);
    vgl_vector_3d<double> w(-a*b, a*a + c*c, -b*c);
    /* Let u denote normal.  Then u . v = a*c - c . a = 0
       u x v = (u_y v_z - u_z v_y, u_z v_x - u_x v_z, u_x v_y - u_y v_x)
             = (b  (-a) - c   0  , c   c   - a  (-a),  a   0  -  b   c )
             = (-a*b, a*a + c*c, -b*c)
             = w
    */
    v_ = normalize(v);
    w_ = normalize(w);
  }
  //given a worldpt, find the representation in the plane's coordinate system
  vgl_point_2d<double> project_to_2d(worldpt p){
    vgl_point_2d<double> to_return(dot_product(p - p0_, w_),  dot_product(p - p0_, v_));
    return to_return;
  }
  //:given a position in the plane coordinate system, return the corresponding worldpt (still in the plane)
  vgl_point_3d<double> to_3d(vgl_point_2d<double> p2d){
    vgl_point_3d<double> to_return = p0_ + p2d.x() * w_ + p2d.y() * v_;
    return to_return;
  }
};




vgl_point_2d<double> fit_2d(const vcl_vector<vgl_point_2d<double> > &locations_in_plane,
                             const vcl_vector<double> & strengths){
  int num_eqns = locations_in_plane.size();
  int num_vars = 6;
  
  double mean = 0;
  for (int i = 0; i < num_eqns; ++i){
    mean += strengths[i];
  }
  mean /= num_eqns;
  
  vnl_matrix<double> A(num_eqns, num_vars);
  vnl_vector<double> v(num_eqns);
  
  for (int i = 0; i < num_eqns; ++i){
    v[i] = strengths[i] - mean;
    double x = locations_in_plane[i].x();
    double y = locations_in_plane[i].y();
    A[i][0] = x*x;
    A[i][1] = y*y;
    A[i][2] = x*y;
    A[i][3] = x;
    A[i][4] = y;
    A[i][5] = 1;
  }
  
  // solve A x = v to find coefficient
  vnl_qr<double> qr(A);
  vnl_vector<double> coef = qr.solve(v);
  
  // find the true location by 
  double a = coef[0];
  double b = coef[1];
  double c = coef[2];
  double d = coef[3];
  double e = coef[4];
  double f = coef[5];
  
  vgl_point_2d<double> pos((c*e-2*b*d)/(4*a*b - c*c),
                            (c*d-2*a*e)/(4*a*b - c*c));
  return pos;
}

biob_worldpt_field<vgl_vector_3d<double> > 
det_suppression_and_interpolation::apply(biob_worldpt_field<vgl_vector_3d<double> > & field, double width, double height,
                                         double min_radius,
                                         double strength_threshold){
  /* For each point p0 with nonzero vector,
        1) construct the plane perpendicular to the vector direction that includes p0
        2) find points p whose distance to the plane is less than height
                         and whose projection onto the plane has distance
                         less than width from p0
          2a) if any such point has a vector of magnitude greater than that of p0,
              then there is no corresponding point in the output.
          2b) otherwise, imagine each such point p has moved to the plane,
              ignore the orientations of the vectors,
              and fit a quadratic surface to the magnitudes as a function of the positions in the plane
              then output a unit vector at the position where the quadratic surface achieves its maximum
  */
  biob_explicit_neighborhood_structure neighborhood_structure;
  neighborhood_structure.populate(field.roster(), vcl_sqrt(height*height + width*width));
  biob_explicit_worldpt_roster * new_locations = new biob_explicit_worldpt_roster();
  //shouldn't have to have a separate variable...
  biob_worldpt_roster_sptr new_roster_sptr = new_locations;
  biob_worldpt_field<vgl_vector_3d<double> > new_field(new_roster_sptr);
  for (int i = 0; i < field.roster()->num_points(); ++i){
    worldpt p0 = field.roster()->point(biob_worldpt_index(i));
    vgl_vector_3d<double> c0 = field.values()[i];
    double strength_at_p0 = c0.length();
    if (strength_at_p0 > strength_threshold){
      bool is_local_maximum = true;//look for nearby points that have higher strengths
      //find the plane through p0 perpendicular to c0
      vgl_plane_3d<double> the_plane(c0, p0);
      plane_coordinate_system plane_coords(c0, p0);
      vcl_vector<vgl_point_2d<double> > locations_in_plane;
      vcl_vector<double> strengths;
      //find the points that are close enough to the plane
      biob_worldpt_neighborhood_structure::neighbors_t neighbors = neighborhood_structure.neighbors(biob_worldpt_index(i));
      for (biob_worldpt_neighborhood_structure::neighbors_t::const_iterator it = neighbors.begin();
           it != neighbors.end(); ++it){
        //get the neighbor point p
        worldpt p = field.roster()->point(*it);
        //find the projection of p onto the plane
        worldpt p_projected = vgl_closest_point(the_plane, p);
        //check if the point p is near enough
        double radius = vgl_distance(p_projected, p0);
        if (vgl_distance(p, p_projected) < height
            && radius < width && radius >= min_radius){
          //include point
          double strength_at_p = field.values()[it->index()].length();
          //see if p0 is a local maximum
          if (strength_at_p > strength_at_p0){
            //nearby point has higher strength, so forget about this point
            is_local_maximum = false;
            break;
          }
          locations_in_plane.push_back(plane_coords.project_to_2d(p));
          strengths.push_back(strength_at_p);
        }
      }
      if (is_local_maximum){
        //Now we have all the data, so fit a surface and get the max point
        vgl_point_2d<double> max_location = fit_2d(locations_in_plane, strengths);
        //for debugging...
        bool flag = false;
        if (flag){
          vcl_cout << "locations in plane: \n" ;
          for (int j = 0; j < locations_in_plane.size(); ++j){
            vcl_cout << locations_in_plane[i] << "\n";
          }
        }
        worldpt max_location_3d = plane_coords.to_3d(max_location);
        //add the point to the output roster
        new_locations->add_point(max_location_3d);
        //could use the interpolated strength but that seems dangerous.
        //instead, using the original strength at the original point p0
        vgl_vector_3d<double> new_value = c0;
        new_field.values().push_back(new_value);
      }
    }
  }
  return new_field;
}                           

