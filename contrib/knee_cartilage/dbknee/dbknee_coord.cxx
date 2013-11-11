// This is file dbknee/dbknee_coord.cxx

//:
// \file

#include "dbknee_coord.h"


#include <vcl_iostream.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_vector_2d.h>

#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbmsh3d/algo/dbmsh3d_ray_intersect.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbmsh3d/algo/dbmsh3d_bucketing.h>

// ============================================================================
// dbknee_coord_fit_cylinder_function
// ============================================================================
class dbknee_coord_fit_cylinder_function : public vnl_least_squares_function
{
public:
  //: constructor
  dbknee_coord_fit_cylinder_function(dbmsh3d_mesh* point_set):
      vnl_least_squares_function(6, point_set->num_vertices(), no_gradient),
      point_set_(point_set)
  {};

  //: Destructor 
  virtual ~dbknee_coord_fit_cylinder_function() {}

  //: Get and set the point set
  dbmsh3d_mesh* point_set() const {return this->point_set_; }
  void set_point_set(dbmsh3d_mesh* pt_set) {this->point_set_ = pt_set; }

  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  // Structure of the parameter vector
  // x = [px, py, pz, phi, theta]
  // Check http://en.wikipedia.org/wiki/Spherical_coordinates
  virtual void f(vnl_vector<double> const& x, vnl_vector<double>& fx);

  //: compute initial parameters based on the point set
  vnl_vector<double > compute_init_params();

protected:
  //: Pointer to the data point set
  dbmsh3d_mesh* point_set_;
};



// ----------------------------------------------------------------------------
//: The main function.
//  Given the parameter vector x, compute the vector of residuals fx.
// x = [px, py, pz, phi, theta]
void dbknee_coord_fit_cylinder_function::
f(vnl_vector<double> const& x, vnl_vector<double>& fx)
{
  vgl_point_3d<double > center(x[0], x[1], x[2]);
  // orientation in spherical coordinate
  double phi = x[3];
  double theta = x[4];
  vgl_vector_3d<double > orientation(vcl_sin(phi)*vcl_cos(theta), 
    vcl_sin(phi)*vcl_sin(theta), vcl_cos(phi));

  // radius of cylinder
  double radius = x[5];
  
  vnl_vector<double >::iterator fx_it = fx.begin();
  this->point_set()->reset_vertex_traversal();
  for (dbmsh3d_vertex* vbase = 0; this->point_set()->next_vertex(vbase);)
  {
    dbmsh3d_vertex* v = (dbmsh3d_vertex*) vbase;
    double d = cross_product(orientation, v->pt()-center).length();
    *fx_it = d-radius;
    ++fx_it;
  }
}


// ----------------------------------------------------------------------------
//: compute initial parameters based on the point set
//vgl_point_3d<double > center(x[0], x[1], x[2]);
//  // orientation in spherical coordinate
//  double phi = x[3];
//  double theta = x[4];
//  vgl_vector_3d<double > orientation(vcl_sin(phi)*vcl_cos(theta), 
//    vcl_sin(phi)*vcl_sin(theta), vcl_cos(phi));
//
//  // radius of cylinder
//  double radius = x[5];
vnl_vector<double > dbknee_coord_fit_cylinder_function::
compute_init_params()
{
  vnl_vector<double > x_init(6, 0);
  vgl_point_3d<double > centroid = 
    dbmsh3d_mesh_algos::compute_centroid(this->point_set());
  x_init[0] = centroid.x();
  x_init[1] = centroid.y();
  x_init[2] = centroid.z();

  // a hack for laser scan's knee
  
  //// pointing along x-direction
  //x_init[3] = vnl_math::pi_over_2;
  //x_init[4] = 0;


  //// pointing along y-direction
  //x_init[3] = vnl_math::pi_over_2;
  //x_init[4] = vnl_math::pi_over_2;

  // pointing along z-direction
  x_init[3] = 0;
  x_init[4] = 0;

  x_init[5] = 20;

  return x_init;
}


// ============================================================================
// dbknee_cylinder_based_coord
// ============================================================================


// ----------------------------------------------------------------------------
//: default coordinate system
void dbknee_cylinder_based_coord::
init_default()
{
  this->set_origin(vgl_point_3d<double >(0, 0, 0));
  this->set_x_axis(vgl_vector_3d<double >(1, 0, 0));
  this->set_y_axis(vgl_vector_3d<double >(0, 1, 0));
  this->set_z_axis(vgl_vector_3d<double >(0, 0, 1));

  vgl_cylinder<double > cylinder(this->origin(), 100, 50);
  this->set_cylinder(cylinder);

}



// ----------------------------------------------------------------------------
//: read from a cylinder-based coordinate system file
// Example cs file:
// origin_x 91.6919
// origin_y -74.2875
// origin_z -69.6523
// x_axis_x -0.831151
// x_axis_y -0.55549
// x_axis_z -0.0248692
// y_axis_x -0.55577
// y_axis_y 0.831317
// y_axis_z 0.00567378
// z_axis_x 0.0175225
// z_axis_y 0.0185373
// z_axis_z -0.999675
// cylinder_radius 17.0202
// cylinder_length 66.2678
// fitting_error 2.21707
bool dbknee_cylinder_based_coord::
load_from_cs_file(const vcl_string& cs_file)
{
  vcl_map<vcl_string, double > param_list;
  dbknee_read_coord_param_file(cs_file, param_list);

  
  
  // if any param is specified then overwrite it
  vcl_map< vcl_string, double >::const_iterator itr;

  // 1. origin
  double origin_x = 0;
  double origin_y = 0;
  double origin_z = 0;
  
  if( (itr = param_list.find("origin_x")) != param_list.end() )
  {
    origin_x = itr->second;
  };

  if( (itr = param_list.find("origin_y")) != param_list.end() )
  {
    origin_y = itr->second;
  };

  if( (itr = param_list.find("origin_z")) != param_list.end() )
  {
    origin_z = itr->second;
  };
  this->set_origin(vgl_point_3d<double >(origin_x, origin_y, origin_z));
  

  // 2. x-axis
  double x_axis_x = 0;
  double x_axis_y = 0;
  double x_axis_z = 0;
  if( (itr = param_list.find("x_axis_x")) != param_list.end() )
  {
    x_axis_x = itr->second;
  };

  if( (itr = param_list.find("x_axis_y")) != param_list.end() )
  {
    x_axis_y = itr->second;
  };

  if( (itr = param_list.find("x_axis_z")) != param_list.end() )
  {
    x_axis_z = itr->second;
  };
  this->set_x_axis(vgl_vector_3d<double >(x_axis_x, x_axis_y, x_axis_z));
  


  // 3. y-axis
  double y_axis_x = 0;
  double y_axis_y = 0;
  double y_axis_z = 0;
  if( (itr = param_list.find("y_axis_x")) != param_list.end() )
  {
    y_axis_x = itr->second;
  };

  if( (itr = param_list.find("y_axis_y")) != param_list.end() )
  {
    y_axis_y = itr->second;
  };

  if( (itr = param_list.find("y_axis_z")) != param_list.end() )
  {
    y_axis_z = itr->second;
  };
  this->set_y_axis(vgl_vector_3d<double >(y_axis_x, y_axis_y, y_axis_z));
  
  // 4. z-axis
  double z_axis_x = 0;
  double z_axis_y = 0;
  double z_axis_z = 0;

  if( (itr = param_list.find("z_axis_x")) != param_list.end() )
  {
    z_axis_x = itr->second;
  };

  if( (itr = param_list.find("z_axis_y")) != param_list.end() )
  {
    z_axis_y = itr->second;
  };

  if( (itr = param_list.find("z_axis_z")) != param_list.end() )
  {
    z_axis_z = itr->second;
  };
  this->set_z_axis(vgl_vector_3d<double >(z_axis_x, z_axis_y, z_axis_z));




  // 5. Cylinder radius and length
  double cylinder_radius = 0;
  double cylinder_length = 0;

  if( (itr = param_list.find("cylinder_radius")) != param_list.end() )
  {
    cylinder_radius = itr->second;
  };

  if( (itr = param_list.find("cylinder_length")) != param_list.end() )
  {
    cylinder_length = itr->second;
  };
  vgl_cylinder<double > cylinder(this->origin(), cylinder_radius, 
    cylinder_length, this->z_axis());
  this->set_cylinder(cylinder);




  // 6. z-coordinates of the band centroids
  if( (itr = param_list.find("z_top_band_centroid")) != param_list.end() )
  {
    this->set_z_top_band_centroid(itr->second);
  };

  if( (itr = param_list.find("z_bot_band_centroid")) != param_list.end() )
  {
    this->set_z_bot_band_centroid(itr->second);
  };

  return true;
}







// ----------------------------------------------------------------------------
//: Build the coordinate system from input data
void dbknee_cylinder_based_coord::
build()
{
  // 1. Crop the original full mesh
  vcl_cout << "Crop the original point set.\n";
  vgl_box_3d<double > box = this->cropping_box();

  dbmsh3d_mesh_algos::crop_mesh(*this->point_set(), box, this->cropped_mesh_);

  this->set_point_set(&this->cropped_mesh_);


  
  
  ////
  //crop_points(this->point_set(), 
  //  float(box.min_x()), float(box.min_y()), float(box.min_z()),
  //  float(box.max_x()), float(box.max_y()), float(box.max_z()));

  // 2. Fit a cylinder to the remaining points
  vcl_cout << "Fit a cylinder to the point set \n";
  this->build_cylinder();

  // 3. Build the coordinate system
  vcl_cout << "Build a coordinate system \n";
  this->build_coord_system();
  
  return;
}






// ----------------------------------------------------------------------------
//: Compute (local) z-coodinates of band centroids
void dbknee_cylinder_based_coord::
compute_z_of_band_centroids()
{

  double band_width_factor = 0.2;
  double angles[] = {-30, -70, -110, -150};

  // 1. Separate out two groups of points, each forming a band
  // [0] - top regions
  // [1] - bottom regions
  enum {TOP = 0, BOTTOM = 1};
  vcl_vector<vgl_point_3d<double > > point_groups[2];

  this->point_set()->reset_vertex_traversal();
  for (dbmsh3d_vertex* vb = 0; this->point_set()->next_vertex(vb); )
  {
    vgl_point_3d<double > pt = vb->pt();
    if (dot_product(pt-this->origin(), this->z_axis()) >= 0)
    {
      point_groups[TOP].push_back(pt);
    }
    else
    {
      point_groups[BOTTOM].push_back(pt);
    }
  }


  // 2. Compute z-coordinate of centroids for each group
  for (int group = 0; group < 2; ++group)
  {
    // for each group, compute the centroid's z-coordinate
    int num_pts = point_groups[group].size();
    double total_z = 0;
    for (int i=0; i< num_pts; ++i)
    {
      vgl_point_3d<double > pt = point_groups[group][i];
      total_z += dot_product(pt-this->origin(), this->z_axis());
    }
    double centroid_z = total_z / num_pts;

    // save the centroid
    if (group == TOP)
    {
      this->set_z_top_band_centroid(centroid_z);
    }
    else if (group = BOTTOM)
    {
      this->set_z_bot_band_centroid(centroid_z);
    }
  }

  return;

}














// ----------------------------------------------------------------------------
//: write info of the coordinate system to an output stream
void dbknee_cylinder_based_coord::
print(vcl_ostream & os)
{
  os << "origin_x" << " " << this->origin().x() << "\n"
    << "origin_y" << " " << this->origin().y() << "\n"
    << "origin_z" << " " << this->origin().z() << "\n"
    << "x_axis_x" << " " << this->x_axis().x() << "\n"
    << "x_axis_y" << " " << this->x_axis().y() << "\n"
    << "x_axis_z" << " " << this->x_axis().z() << "\n"
    << "y_axis_x" << " " << this->y_axis().x() << "\n"
    << "y_axis_y" << " " << this->y_axis().y() << "\n"
    << "y_axis_z" << " " << this->y_axis().z() << "\n"
    << "z_axis_x" << " " << this->z_axis().x() << "\n"
    << "z_axis_y" << " " << this->z_axis().y() << "\n"
    << "z_axis_z" << " " << this->z_axis().z() << "\n"
    << "cylinder_radius" << " " << this->cylinder().radius() << "\n"
    << "cylinder_length" << " " << this->cylinder().length() << "\n"
    << "z_top_band_centroid" << " " << this->z_top_band_centroid() << "\n"
    << "z_bot_band_centroid" << " " << this->z_bot_band_centroid() << "\n"
    << "fitting_error" << " " << this->fitting_error() << "\n"
    ;
}




// ----------------------------------------------------------------------------
//: convert from local coordinate to world coordinate
vgl_point_3d<double > dbknee_cylinder_based_coord::
local_to_wcs(const vgl_point_3d<double >& pt) const
{
  return this->origin() + 
    pt.x() * this->x_axis() + 
    pt.y() * this->y_axis() + 
    pt.z() * this->z_axis();
}


//: convert local cylinderical coordinate to world coordinate
vgl_point_3d<double > dbknee_cylinder_based_coord::
local_cyl_to_wcs(double radius, double theta, double z)
{
  vgl_point_3d<double > p(radius*vcl_cos(theta), radius*vcl_sin(theta), z);
  return this->local_to_wcs(p);
}


// ----------------------------------------------------------------------------
//: convert form world coordinate to local coordinate
vgl_point_3d<double > dbknee_cylinder_based_coord::
wc_to_local(const vgl_point_3d<double >& pt) const
{
  return vgl_point_3d<double >(
    dot_product(pt-this->origin(), this->x_axis()),
    dot_product(pt-this->origin(), this->y_axis()),
    dot_product(pt-this->origin(), this->z_axis())
    );
}







// ----------------------------------------------------------------------------
// SUPPORT FUNCTIONS
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//: build the cylinder
bool dbknee_cylinder_based_coord::
build_cylinder()
{
  // The fitting function
  dbknee_coord_fit_cylinder_function fit_cylinder(this->point_set());

  //dbknee_coord_fit_cylinder_function fit_cylinder(&this->cropped_mesh_);

  vnl_vector<double > x_init = fit_cylinder.compute_init_params();

  // solve for root using nonlinear minimization, vnl_levenberg_marquardt algorithm
  vnl_levenberg_marquardt lm(fit_cylinder);
  vnl_vector<double > x = x_init;
 
  if (! lm.minimize(x)){
    vcl_cerr << "vnl_levenberg_marquardt minimization failed.\n";
    return false;
  }
  
  lm.diagnose_outcome();

  this->set_fitting_error(lm.get_end_error());

  // x = [px, py, pz, phi, theta]
  double phi = x[3];
  double theta = x[4];
  vgl_vector_3d<double > orientation(vcl_sin(phi)*vcl_cos(theta), 
    vcl_sin(phi)*vcl_sin(theta), vcl_cos(phi));
  
  vgl_cylinder<double > cylinder;
  cylinder.set_orientation(orientation);
  cylinder.set_center(vgl_point_3d<double >(x[0], x[1], x[2]));
  cylinder.set_radius(x[5]);
  
  // >> Determine exact center and length of the cylinder

  // Approximate the center by using centroid of the point set
  vgl_point_3d<double > centroid = 
    dbmsh3d_mesh_algos::compute_centroid(this->point_set());
  vgl_line_3d_2_points<double > centerline(
    cylinder.center(), cylinder.center() + 100*cylinder.orientation());
  cylinder.set_center(vgl_closest_point(centerline, centroid));

  // Compute range [t_min, t_max] of cylinder using the center point as reference
  this->point_set()->reset_vertex_traversal();
  double t_min = 1e100; 
  double t_max = -1e100; 
  for (dbmsh3d_vertex* vbase=0; this->point_set()->next_vertex(vbase); )
  {
    dbmsh3d_vertex* v = (dbmsh3d_vertex*) vbase;
    double t = dot_product(cylinder.orientation(), v->pt()-cylinder.center());
    t_min = vnl_math_min(t, t_min);
    t_max = vnl_math_max(t, t_max);
  }

  // set the exact center and length
  cylinder.set_center(cylinder.center() + (t_min+t_max)/2*cylinder.orientation());
  cylinder.set_length(t_max - t_min);
  this->set_cylinder(cylinder);

  return true;
}



// ----------------------------------------------------------------------------

//: build cartesian coordinate system from the cylinder and notch point
bool dbknee_cylinder_based_coord::
build_coord_system()
{
  // for now, just compute the direction of the three axis
  // but later, need to compute the coordinate system properly


  vgl_cylinder<double > cylinder = this->cylinder();
  // origin
  this->set_origin(cylinder.center());
  
  // z-axis: point along the cylinder.
  // Make sure the notch point has z>0
  if (dot_product(cylinder.orientation(), this->notch_point()-cylinder.center()) < 0)
  {
    this->set_z_axis(-cylinder.orientation());
  }
  else
  {
    this->set_z_axis(cylinder.orientation());
  }

  // x-axis: // orthogonal from notch point to x-axis
  vgl_line_3d_2_points<double > centerline(cylinder.center(), 
    cylinder.center() + 100 * cylinder.orientation());
  vgl_point_3d<double > notch_proj = 
    vgl_closest_point(centerline, this->notch_point());

  this->set_x_axis(this->notch_point()-notch_proj);

  // y-axis : assuming this is a right-handed coordinate system
  this->set_y_axis(cross_product(this->z_axis(), this->x_axis()));



  //// Graph of defined CS
  //// All CS must be added to this graph as they are created.
  //vcsl_graph_sptr graph_cs = new vcsl_graph;

  //// World Coordinate System
  //vcsl_spatial_sptr wcs = new vcsl_cartesian_3d;

  //// Add WCS to the global map of coordinate systems.
  //wcs->set_graph(graph_cs);
  
  return true;
}



// ----------------------------------------------------------------------------
//: convert in-place on a mesh from world coordinate to local coordinate
void dbknee_cylinder_based_coord::
wc_to_local(dbmsh3d_mesh* mesh) const
{
  mesh->reset_vertex_traversal();
  for (dbmsh3d_vertex* vb = 0; mesh->next_vertex(vb); )
  {
    dbmsh3d_vertex* v = (dbmsh3d_vertex*) vb;
    vgl_point_3d<double > pt = v->pt();
    v->set_pt(this->wc_to_local(pt));
  }

}


// ----------------------------------------------------------------------------
//: convert from world coordinate to local cylindrical coordinate
void dbknee_cylinder_based_coord::
wc_to_local_cyl(const vgl_point_3d<double >& pt, 
                double& radius, double& theta, double& z) const
{
  vgl_point_3d<double > local = this->wc_to_local(pt);
  z = local.z();
  
  vgl_vector_2d<double > xy_proj(local.x(), local.y());
  radius = xy_proj.length();
  theta = vcl_atan2(xy_proj.y(), xy_proj.x());
  return;
}




// ----------------------------------------------------------------------------
//: crop a mesh using cylindrical coordinates
bool dbknee_cylinder_based_coord::
crop_mesh_cylindrical(dbmsh3d_mesh& source_mesh, 
                      dbmsh3d_mesh& new_mesh,
                      double z_min, double z_max,
                      double theta_min, double theta_max,
                      double r_min, double r_max)
{
  // for convenicence : angle check is a bit tricky, easier to use vector
  vgl_vector_2d<double > vmin(vcl_cos(theta_min), vcl_sin(theta_min));
  vgl_vector_2d<double > vmax(vcl_cos(theta_max), vcl_sin(theta_max));
  
  // angle range
  double angle_max = signed_angle(vmin, vmax);
  if (angle_max < 0) angle_max += 2*vnl_math::pi;
 
  vcl_vector<dbmsh3d_face* > face_list;
  source_mesh.reset_face_traversal();
  for ( dbmsh3d_face* face = 0; source_mesh.next_face(face); )
  {
    bool inside = true;
    for (unsigned i=0; i < face->vertices().size() && inside; ++i)
    {
      const dbmsh3d_vertex* vertex = 
        static_cast<const dbmsh3d_vertex* >(face->vertices(i));
      vgl_point_3d<double > pt = vertex->pt();
      double radius, theta, z;
      this->wc_to_local_cyl(pt, radius, theta, z);
      // range check
      inside = inside && (radius >= r_min) && (radius <= r_max);
      inside = inside && (z >= z_min) && (z <= z_max);

      // angle [0, 2pi] wrt to theta_min
      vgl_vector_2d<double > v0(vcl_cos(theta), vcl_sin(theta));
      double angle = signed_angle(vmin, v0);
      if (angle < 0) angle += 2*vnl_math::pi;
      inside = inside && (angle <= angle_max);
    }

    if (inside)
      face_list.push_back(face);
  }
  return dbmsh3d_mesh_algos::submesh(source_mesh, face_list, new_mesh);
}




// ============================================================================
// dbknee_cylinder_based_coord_params
// ============================================================================

// ----------------------------------------------------------------------------
dbknee_cylinder_based_coord_params::
dbknee_cylinder_based_coord_params(const vgl_box_3d<double >& bounding_box,
                                   const vcl_map<vcl_string, double >& param_list)
{
  // default: take 1/2 of x-axis as the cropping box
  this->cropping_box = bounding_box;
  double width = bounding_box.width();
  double height = bounding_box.height();
  double depth = bounding_box.depth();


  // if any param is specified then overwrite it
  vcl_map< vcl_string, double >::const_iterator itr;

  // fraction of the object's width will be kept in cropping
  double crop_width_start = 0.0;
  if( (itr = param_list.find("crop_width_start")) != param_list.end() )
  {
    crop_width_start = itr->second;
  };

  double crop_width_end = 1.0;
  if( (itr = param_list.find("crop_width_end")) != param_list.end() )
  {
    crop_width_end = itr->second;
  };

  //
  this->cropping_box.set_min_x(cropping_box.min_x() + width*crop_width_start);
  this->cropping_box.set_max_x(cropping_box.min_x() + width*crop_width_end);


  // fraction of the object's height will be kept in cropping
  double crop_height_start = 0.0;
  if( (itr = param_list.find("crop_height_start")) != param_list.end() )
  {
    crop_height_start = itr->second;
  };

  double crop_height_end = 1.0;
  if( (itr = param_list.find("crop_height_end")) != param_list.end() )
  {
    crop_height_end = itr->second;
  };
  //
  this->cropping_box.set_min_y(cropping_box.min_y() + height*crop_height_start);
  this->cropping_box.set_max_y(cropping_box.min_y() + height*crop_height_end);

  // fraction of the object's depth will be kept in cropping
  double crop_depth_start = 0.0;
  if( (itr = param_list.find("crop_depth_start")) != param_list.end() )
  {
    crop_depth_start = itr->second;
  };

  double crop_depth_end = 1.0;
  if( (itr = param_list.find("crop_depth_end")) != param_list.end() )
  {
    crop_depth_end = itr->second;
  };
  //
  this->cropping_box.set_min_z(cropping_box.min_z() + depth*crop_depth_start);
  this->cropping_box.set_max_z(cropping_box.min_z() + depth*crop_depth_end);
  
  // x_min and x_max
  if( (itr = param_list.find("crop_x_min")) != param_list.end() )
  {
    this->cropping_box.set_min_x(itr->second);
  };
  if( (itr = param_list.find("crop_x_max")) != param_list.end() )
  {
    this->cropping_box.set_max_x(itr->second);
  };

  // y_min and y_max
  if( (itr = param_list.find("crop_y_min")) != param_list.end() )
  {
    this->cropping_box.set_min_y(itr->second);
  };
  if( (itr = param_list.find("crop_y_max")) != param_list.end() )
  {
    this->cropping_box.set_max_y(itr->second);
  };
  
  // z_min and z_max
  if( (itr = param_list.find("crop_z_min")) != param_list.end() )
  {
    this->cropping_box.set_min_z(itr->second);
  };
  if( (itr = param_list.find("crop_z_max")) != param_list.end() )
  {
    this->cropping_box.set_max_z(itr->second);
  };

  // voxel size
  this->dx = 1;
  this->dy = 1;
  this->dz = 1;
  if( (itr = param_list.find("dx")) != param_list.end() )
  {
    this->dx = itr->second;
  };
  if( (itr = param_list.find("dy")) != param_list.end() )
  {
    this->dy = itr->second;
  };
  if( (itr = param_list.find("dz")) != param_list.end() )
  {
    this->dz = itr->second;
  };

  // the notch point
  this->notch_point = this->cropping_box.centroid();
  double notch_x = this->notch_point.x();
  double notch_y = this->notch_point.y();
  double notch_z = this->notch_point.z();
  if( (itr = param_list.find("notch_i")) != param_list.end() )
  {
    notch_x = (itr->second) * dx;
  };
  if( (itr = param_list.find("notch_j")) != param_list.end() )
  {
    notch_y = (itr->second) * dy;
  };
  if( (itr = param_list.find("notch_p")) != param_list.end() )
  {
    notch_z = itr->second * dz;
  };


  // if the notch "readl world" coordinates are specified explicity,
  // the overwrite the one from the image

  if( (itr = param_list.find("notch_x")) != param_list.end() )
  {
    notch_x = (itr->second);
  };

  if( (itr = param_list.find("notch_y")) != param_list.end() )
  {
    notch_y = (itr->second);
  };

  if( (itr = param_list.find("notch_z")) != param_list.end() )
  {
    notch_z = (itr->second);
  };


  this->notch_point.set(notch_x, notch_y, notch_z);
}





// ----------------------------------------------------------------------------
//: Read a cropping parameter from a parameter file
void dbknee_read_coord_param_file(const vcl_string& param_file,
                                  vcl_map<vcl_string, double >& param_list
                                  )
{

  // open file for reading
  vcl_ifstream infp(param_file.c_str(), vcl_ios_in);
  if (!infp) 
  {
    vcl_cerr << " Error opening file  " << param_file << vcl_endl;
    return;
  }

  vcl_cout << "Parameter list: \n";
  while (!infp.eof())
  {
    vcl_string param_name;
    double param_value;
    infp >> param_name >> param_value;
    vcl_cout << param_name << " = " ;
    vcl_cout << param_value << vcl_endl;
    param_list.insert(vcl_make_pair(param_name, param_value));
  }
  infp.close();
  return; 
}






// ----------------------------------------------------------------------------
//: Read a coordinate system file and overwrite parameters in a coordinate system
void dbknee_read_cs_file(const vcl_string& cs_file, dbknee_cylinder_based_coord& cs)
{
  vcl_map<vcl_string, double > param_list;
  dbknee_read_coord_param_file(cs_file, param_list);

  
  // if any param is specified then overwrite it
  vcl_map< vcl_string, double >::const_iterator itr;

  double origin_x = 0;
  double origin_y = 0;
  double origin_z = 0;
  
  if( (itr = param_list.find("origin_x")) != param_list.end() )
  {
    origin_x = itr->second;
  };

  if( (itr = param_list.find("origin_y")) != param_list.end() )
  {
    origin_y = itr->second;
  };

  if( (itr = param_list.find("origin_z")) != param_list.end() )
  {
    origin_z = itr->second;
  };
  cs.set_origin(vgl_point_3d<double >(origin_x, origin_y, origin_z));
  

  double x_axis_x = 0;
  double x_axis_y = 0;
  double x_axis_z = 0;
  if( (itr = param_list.find("x_axis_x")) != param_list.end() )
  {
    x_axis_x = itr->second;
  };

  if( (itr = param_list.find("x_axis_y")) != param_list.end() )
  {
    x_axis_y = itr->second;
  };

  if( (itr = param_list.find("x_axis_z")) != param_list.end() )
  {
    x_axis_z = itr->second;
  };
  cs.set_x_axis(vgl_vector_3d<double >(x_axis_x, x_axis_y, x_axis_z));
  


  double y_axis_x = 0;
  double y_axis_y = 0;
  double y_axis_z = 0;
  if( (itr = param_list.find("y_axis_x")) != param_list.end() )
  {
    y_axis_x = itr->second;
  };

  if( (itr = param_list.find("y_axis_y")) != param_list.end() )
  {
    y_axis_y = itr->second;
  };

  if( (itr = param_list.find("y_axis_z")) != param_list.end() )
  {
    y_axis_z = itr->second;
  };
  cs.set_y_axis(vgl_vector_3d<double >(y_axis_x, y_axis_y, y_axis_z));
  
  double z_axis_x = 0;
  double z_axis_y = 0;
  double z_axis_z = 0;

  if( (itr = param_list.find("z_axis_x")) != param_list.end() )
  {
    z_axis_x = itr->second;
  };

  if( (itr = param_list.find("z_axis_y")) != param_list.end() )
  {
    z_axis_y = itr->second;
  };

  if( (itr = param_list.find("z_axis_z")) != param_list.end() )
  {
    z_axis_z = itr->second;
  };
  cs.set_z_axis(vgl_vector_3d<double >(z_axis_x, z_axis_y, z_axis_z));




  double cylinder_radius = 0;
  double cylinder_length = 0;

  if( (itr = param_list.find("cylinder_radius")) != param_list.end() )
  {
    cylinder_radius = itr->second;
  };

  if( (itr = param_list.find("cylinder_length")) != param_list.end() )
  {
    cylinder_length = itr->second;
  };

  vgl_cylinder<double > cylinder(cs.origin(), cylinder_radius, cylinder_length);
  cs.set_cylinder(cylinder);


  
}




// ----------------------------------------------------------------------------
//: Write the coordinates 
void dbknee_compute_write_local_coords_to_file(const dbknee_cylinder_based_coord& cs,
                                               dbmsh3d_mesh* pt_set,
                                               const vcl_string& outfile)
{
  // compute the local coordinates of the points
  vnl_matrix<double > pc(pt_set->num_vertices(), 3);
  pc.fill(0);
  int row_counter = 0;
  pt_set->reset_vertex_traversal();
  for (dbmsh3d_vertex* vb = 0; pt_set->next_vertex(vb); ++row_counter)
  {
    // world-coordinate
    vgl_point_3d<double > wcs_pt = (vb)->pt();

    // local coordinates
    vgl_point_3d<double > local_pt;
    local_pt.set( dot_product(wcs_pt-cs.origin(), cs.x_axis()), 
      dot_product(wcs_pt-cs.origin(), cs.y_axis()),
      dot_product(wcs_pt-cs.origin(), cs.z_axis()) );


    pc(row_counter, 0) = local_pt.x();
    pc(row_counter, 1) = local_pt.y();
    pc(row_counter, 2) = local_pt.z();
  }

  // print out the matrix
  // open file for writing
  vcl_ofstream outfp(outfile.c_str(), vcl_ios_out);
  pc.print(outfp);
  outfp.close();  

}






// ----------------------------------------------------------------------------
//: Extract the regions of interest on the knee cartilage
void dbknee_compute_cartilage_regions(const dbknee_cylinder_based_coord& cs,
          vcl_vector<vcl_vector<vgl_point_3d<double > > >& top_regions,
          vcl_vector<vcl_vector<vgl_point_3d<double > > >& bot_regions)
{
  double band_width_factor = 0.2;
  double angles[] = {-30, -70, -110, -150};

  // 1. Separate out two groups of points, each forming a band
  // [0] - top regions
  // [1] - bottom regions
  enum {TOP = 0, BOTTOM = 1};
  vcl_vector<vgl_point_3d<double > > point_groups[2];

  cs.point_set()->reset_vertex_traversal();
  for (dbmsh3d_vertex* vb = 0; cs.point_set()->next_vertex(vb); )
  {
    vgl_point_3d<double > pt = ((dbmsh3d_vertex*)(vb))->pt();
    if (dot_product(pt-cs.origin(), cs.z_axis()) >= 0)
    {
      point_groups[TOP].push_back(pt);
    }
    else
    {
      point_groups[BOTTOM].push_back(pt);
    }
  }

  // 2. Compute band-points for each group
  vcl_vector<vcl_vector<vgl_point_3d<double > > >* point_regions[2];
  point_regions[0] = &top_regions;
  point_regions[1] = &bot_regions;
  for (int group = 0; group < 2; ++group)
  {
    // for each group, compute the centroid's z-coordinate
    int num_pts = point_groups[group].size();
    double total_z = 0;
    for (int i=0; i< num_pts; ++i)
    {
      vgl_point_3d<double > pt = point_groups[group][i];
      total_z += dot_product(pt-cs.origin(), cs.z_axis());
    }
    double centroid_z = total_z / num_pts;

    // Then put the points in appropriate regions
    double band_width = band_width_factor * cs.cylinder().length();

    // reserve space for the 3 regions for each band
    point_regions[group]->clear();
    for (int m=0; m<3; ++m)
    {
      (*point_regions[group]).push_back(vcl_vector<vgl_point_3d<double > >());
    }

    for (int i=0; i< num_pts; ++i)
    {
      vgl_point_3d<double > pt = point_groups[group][i];
      vgl_point_3d<double > local_pt = cs.wc_to_local(pt);
      
      
      // if a point is outside the band, ignore it
      if (local_pt.z() < (centroid_z-band_width/2) || local_pt.z() > (centroid_z+band_width/2))
        continue;

      // if outside angle range, ignore
      double angle = vcl_atan2(local_pt.y(), local_pt.x()) * 180 / vnl_math::pi;

      if (angle > angles[0])
        continue;
      else if (angle > angles[1])
        (*point_regions[group])[0].push_back(pt);
      else if (angle > angles[2])
        (*point_regions[group])[1].push_back(pt);
      else if (angle > angles[3])
        (*point_regions[group])[2].push_back(pt);
      else
        continue;
      
    }
  }

  return;
}


// ----------------------------------------------------------------------------
//: Separate the cartilage mesh into two separate top and bottom surfaces.
// given the axis of a cylinder
// A face (f) is consider in the inner surface if a ray starting from and 
// orthogonal to the cylinder axis pointing toward (f) hits another face after 
// hitting (f)
void dbknee_separate_inner_outer_surfaces(dbmsh3d_mesh& mesh,
                                          const vgl_point_3d<double >& axis_origin,
                                          const vgl_vector_3d<double >& axis_dir,
                                          dbmsh3d_mesh& inner_mesh,
                                          dbmsh3d_mesh& outer_mesh)
{
  vcl_map<int, dbmsh3d_face* > face_map = mesh.facemap();
  vcl_vector<dbmsh3d_face* > inner_face_list;
  vcl_vector<dbmsh3d_face* > outer_face_list;

  // projection of face center onto the axis
  vgl_vector_3d<double > t = normalized(axis_dir);


  vcl_cout << "\nSeparate mesh into inner and outer meshes\n.";
  
  
  vcl_cout << "1. Put the faces into buckets...";
  int ncols = 10;
  int nrows = 10;
  int nslabs = 10;

  vgl_box_3d<double> bbox;
  detect_bounding_box(&mesh, bbox);
  
  // enlarge the bounding box to avoid boundary problems
  vgl_vector_3d<double > diagonal(bbox.width(), bbox.height(), bbox.depth());
  vgl_box_3d<double > enlarged_bbox(bbox.min_point() - 0.05 * diagonal, 
    bbox.max_point() + 0.05 * diagonal);
  
  dbmsh3d_bucketing_face face_bucketing(ncols, nrows, nslabs, enlarged_bbox);

  face_bucketing.add(mesh);
  
  vcl_cout << "done.\n";



  vcl_cout << "2. Classifying the faces ...";

  
  // bruce-forge iterate thru all the faces
  int num_faces = mesh.facemap().size();
  int face_count = 0;
  mesh.reset_face_traversal();
  double current_percentage = 0;
  while (!face_map.empty())
  {
    double percent_processed_faces = (num_faces - face_map.size()) * 100.0/ num_faces;
    if (percent_processed_faces > (current_percentage + 5))
    {
      current_percentage += 5;
      vcl_cout << current_percentage << "%  ";
    }

    dbmsh3d_face* face = face_map.begin()->second;
    face_map.erase(face_map.begin());

    // compute the ray 
    // - starting from the cylinder axis, 
    // - orthogonal to the cylinder axis, and
    // - passing thru the face center
    
    // face center
    vgl_point_3d<double > face_center = face->compute_center_pt();

    double d = dot_product(t, face_center - axis_origin);
    vgl_point_3d<double > face_center_proj = axis_origin + d * t;
    vgl_vector_3d<double > n = normalized( face_center - face_center_proj );

    double dist_face = dot_product(n, face_center - face_center_proj);

    // Now we have determined the ray, which is represented by face_center_proj
    // and orthogonal vector n. We need to find out where else this ray intersects the mesh
    // For each face of the mesh, we check whether the ray intersects the face
    
    /////////////////////////////////////////////////////
    // mesh.reset_face_traversal();
    // for (dbmsh3d_face* face2 = 0; mesh.next_face(face2);)
    /////////////////////////////////////////////////////

    // a different method to traverse the neighboring faces of ``face"
    // collect the containing bucket and neighboring bucket
    vcl_vector<dbmsh3d_face_bucket_sptr > bucket_list;
    dbmsh3d_face_bucket_sptr bucket = 
      face_bucketing.get_bucket(face->compute_center_pt());
    if (bucket)
    {
      for (int col = bucket->col()-1; col < bucket->col()+2; ++col)
        for (int row = bucket->row()-1; row < bucket->row()+2; ++row)
          for (int slab = bucket->slab()-1; slab < bucket->slab()+2; ++slab)
          {
            dbmsh3d_face_bucket_sptr b = face_bucketing.get_bucket(col, row, slab);
            if (b)
              bucket_list.push_back(b);
          }
    }

    bool found_face = false;
    for (unsigned int i=0; i<bucket_list.size() && !found_face; ++i)
    {
      dbmsh3d_face_bucket_sptr b = bucket_list[i];
      for (vcl_map<int, dbmsh3d_face* >::const_iterator itr = 
        b->face_list().begin(); itr != b->face_list().end(); ++itr)
      {
        dbmsh3d_face* face2 = itr->second;

        if (face2 == face)
          continue;

        vgl_point_3d<double > intersect_pt;
        if (!dbmsh3d_intersect_ray_face(face_center_proj, n, face2, intersect_pt))
        {
          continue;
        }
        else
        {
          // found intersection point, between face and face2, one is inner and one is outer
          double dist_face2 = dot_product(n, intersect_pt - face_center_proj);

          // ``face" is part of the inner surface
          if (dist_face2 > dist_face)
          {
            inner_face_list.push_back(face);

            // also put face2 in the outer face list
            outer_face_list.push_back(face2);
            face_map.erase(face2->id());
          
          }
          // ``face" is part of the outer surface
          else
          {
            outer_face_list.push_back(face);
            
            // also put face2 in the outer face list
            inner_face_list.push_back(face2);
            face_map.erase(face2->id());
          }
          found_face = true;
          break;
        }
      }
    }


    if (!found_face)
    {
      vcl_cout << "Face with no opposite face, face id = " << face->id() << vcl_endl;
      vcl_cout << "Face center = " << face->compute_center_pt() << vcl_endl;
    }
  }

  
  // All faces have been classified, now output them to the mesh
  vcl_cout << "\nCreating inner and outer meshes ...";
  dbmsh3d_mesh_algos::submesh(mesh, inner_face_list, inner_mesh);
  dbmsh3d_mesh_algos::submesh(mesh, outer_face_list, outer_mesh);
  vcl_cout << "done.\n";

  return;
}


