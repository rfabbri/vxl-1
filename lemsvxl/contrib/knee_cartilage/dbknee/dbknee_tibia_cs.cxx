// This is file dbknee/dbknee_tibia_cs.cxx

//:
// \file

#include "dbknee_tibia_cs.h"


//#include <vcl_iostream.h>
//#include <vnl/vnl_least_squares_function.h>
//#include <vnl/algo/vnl_levenberg_marquardt.h>
//#include <vnl/vnl_math.h>
//#include <vgl/vgl_line_3d_2_points.h>
//#include <vgl/vgl_closest_point.h>
//#include <vgl/vgl_vector_2d.h>
//
//#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
//#include <dbmsh3d/algo/dbmsh3d_ray_intersect.h>
//#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
//#include <dbmsh3d/algo/dbmsh3d_bucketing.h>
#include <dbgl/dbgl_area.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>

//
//
// ============================================================================
// dbknee_tibia_cs
// ============================================================================



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
bool dbknee_tibia_cs::
load_from_cs_file(const vcl_string& cs_file)
{
//  vcl_map<vcl_string, double > param_list;
//  dbknee_read_coord_param_file(cs_file, param_list);
//
//  
//  
//  // if any param is specified then overwrite it
//  vcl_map< vcl_string, double >::const_iterator itr;
//
//  // 1. origin
//  double origin_x = 0;
//  double origin_y = 0;
//  double origin_z = 0;
//  
//  if( (itr = param_list.find("origin_x")) != param_list.end() )
//  {
//    origin_x = itr->second;
//  };
//
//  if( (itr = param_list.find("origin_y")) != param_list.end() )
//  {
//    origin_y = itr->second;
//  };
//
//  if( (itr = param_list.find("origin_z")) != param_list.end() )
//  {
//    origin_z = itr->second;
//  };
//  this->set_origin(vgl_point_3d<double >(origin_x, origin_y, origin_z));
//  
//
//  // 2. x-axis
//  double x_axis_x = 0;
//  double x_axis_y = 0;
//  double x_axis_z = 0;
//  if( (itr = param_list.find("x_axis_x")) != param_list.end() )
//  {
//    x_axis_x = itr->second;
//  };
//
//  if( (itr = param_list.find("x_axis_y")) != param_list.end() )
//  {
//    x_axis_y = itr->second;
//  };
//
//  if( (itr = param_list.find("x_axis_z")) != param_list.end() )
//  {
//    x_axis_z = itr->second;
//  };
//  this->set_x_axis(vgl_vector_3d<double >(x_axis_x, x_axis_y, x_axis_z));
//  
//
//
//  // 3. y-axis
//  double y_axis_x = 0;
//  double y_axis_y = 0;
//  double y_axis_z = 0;
//  if( (itr = param_list.find("y_axis_x")) != param_list.end() )
//  {
//    y_axis_x = itr->second;
//  };
//
//  if( (itr = param_list.find("y_axis_y")) != param_list.end() )
//  {
//    y_axis_y = itr->second;
//  };
//
//  if( (itr = param_list.find("y_axis_z")) != param_list.end() )
//  {
//    y_axis_z = itr->second;
//  };
//  this->set_y_axis(vgl_vector_3d<double >(y_axis_x, y_axis_y, y_axis_z));
//  
//  // 4. z-axis
//  double z_axis_x = 0;
//  double z_axis_y = 0;
//  double z_axis_z = 0;
//
//  if( (itr = param_list.find("z_axis_x")) != param_list.end() )
//  {
//    z_axis_x = itr->second;
//  };
//
//  if( (itr = param_list.find("z_axis_y")) != param_list.end() )
//  {
//    z_axis_y = itr->second;
//  };
//
//  if( (itr = param_list.find("z_axis_z")) != param_list.end() )
//  {
//    z_axis_z = itr->second;
//  };
//  this->set_z_axis(vgl_vector_3d<double >(z_axis_x, z_axis_y, z_axis_z));
//
//
//
//
//  // 5. Cylinder radius and length
//  double cylinder_radius = 0;
//  double cylinder_length = 0;
//
//  if( (itr = param_list.find("cylinder_radius")) != param_list.end() )
//  {
//    cylinder_radius = itr->second;
//  };
//
//  if( (itr = param_list.find("cylinder_length")) != param_list.end() )
//  {
//    cylinder_length = itr->second;
//  };
//  vgl_cylinder<double > cylinder(this->origin(), cylinder_radius, 
//    cylinder_length, this->z_axis());
//  this->set_cylinder(cylinder);
//
//
//
//
//  // 6. z-coordinates of the band centroids
//  if( (itr = param_list.find("z_top_band_centroid")) != param_list.end() )
//  {
//    this->set_z_top_band_centroid(itr->second);
//  };
//
//  if( (itr = param_list.find("z_bot_band_centroid")) != param_list.end() )
//  {
//    this->set_z_bot_band_centroid(itr->second);
//  };

  return true;
}







// ----------------------------------------------------------------------------
//: Build the coordinate system from input data
void dbknee_tibia_cs::
build()
{
  // Treat each face as a weighted point. Compute their centroid
  vcl_vector<vgl_point_3d<double > > face_centers;
  face_centers.reserve(this->mesh()->facemap().size());

  vcl_vector<double > face_weights;
  face_weights.reserve(this->mesh()->facemap().size());

  this->mesh()->reset_face_traversal();
  for (dbmsh3d_face* face=0; this->mesh()->next_face(face);)
  {
    // compute center and area of the face
    vcl_vector<dbmsh3d_vertex* > face_vertices;
    
    face->get_bnd_Vs(face_vertices);
    assert(face_vertices.size() >= 3);

    double face_area = 0;
    // iterate thru the triangles of the face
    dbmsh3d_vertex* v1 = face_vertices[0];
    for (unsigned i=1; i < face_vertices.size()-1; ++i)
    {
      // compute area of the triangle
      dbmsh3d_vertex* v2 = face_vertices[i];
      dbmsh3d_vertex* v3 = face_vertices[i+1];
      double triangle_area = dbgl_area_triangle(v1->pt(), v2->pt(), v3->pt());
      face_area += triangle_area;
    }

    // face center
    double x[] = {0, 0, 0};
    for (unsigned i=0; i < face_vertices.size(); ++i)
    {
      vgl_point_3d<double > pt = face_vertices[i]->pt();
      x[0] += pt.x();
      x[1] += pt.y();
      x[2] += pt.z();
    }
    vgl_point_3d<double > face_center(x[0] / face_vertices.size(), 
      x[1] / face_vertices.size(), x[2] / face_vertices.size() );

    face_centers.push_back(face_center);
    face_weights.push_back(face_area);
  }

  // Compute centroid of the mesh
  double x[] = {0,0,0};
  double sum_weight = 0;
  for (unsigned i=0; i<face_centers.size(); ++i)
  {
    vgl_point_3d<double > pt = face_centers[i];
    double w = face_weights[i];
    x[0] += w*pt.x();
    x[1] += w*pt.y();
    x[2] += w*pt.z();
    sum_weight += w;
  }
  vgl_point_3d<double > mesh_centroid(x[0]/sum_weight, x[1]/sum_weight, x[2]/sum_weight);
  this->set_origin(mesh_centroid);

  // Now translate every face to the local coordinate system to compute moment axis
  for (unsigned i=0; i<face_centers.size(); ++i)
  {
    vgl_point_3d<double > pt = face_centers[i];
    face_centers[i] = vgl_point_3d<double >(0,0,0) + (pt - this->origin()) ; 
  }


  // Now we are ready to compute the inertia moment tensor
  vnl_matrix<double > I(3,3,0);
  for (unsigned i=0; i<face_centers.size(); ++i)
  {
    vgl_point_3d<double > pt = face_centers[i];
    double xi = pt.x();
    double yi = pt.y();
    double zi = pt.z();
    double mi = face_weights[i];
    double xx = mi * (yi*yi + zi*zi);
    double yy = mi * (xi*xi + zi*zi);
    double zz = mi * (xi*xi + yi*yi);
    double xy = -mi * (xi*yi);
    double xz = -mi * (xi*zi);
    double yz = -mi * (yi*zi);

    I(0,0) += xx;
    I(0,1) += xy;
    I(0,2) += xz;
    I(1,0) += xy;
    I(1,1) += yy;
    I(1,2) += yz;
    I(2,0) += xz;
    I(2,1) += yz;
    I(2,2) += zz;
  }

  vnl_matrix<double > V;
  vnl_vector<double > D;
  vnl_symmetric_eigensystem_compute(I, V, D);
  vnl_vector<double > z_col = V.get_column(2);
  vnl_vector<double > y_col = V.get_column(1);
  vnl_vector<double > x_col = V.get_column(0);

  this->set_x_axis(vgl_vector_3d<double >(x_col[0], x_col[1], x_col[2]));
  this->set_y_axis(vgl_vector_3d<double >(y_col[0], y_col[1], y_col[2]));
  this->set_z_axis(vgl_vector_3d<double >(z_col[0], z_col[1], z_col[2]));
  
   
  return;
}

// ----------------------------------------------------------------------------
//: write info of the coordinate system to an output stream
void dbknee_tibia_cs::
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
    ;
}




// ----------------------------------------------------------------------------
//: convert from local coordinate to world coordinate
vgl_point_3d<double > dbknee_tibia_cs::
local_to_wcs(const vgl_point_3d<double >& pt) const
{
  return this->origin() + 
    pt.x() * this->x_axis() + 
    pt.y() * this->y_axis() + 
    pt.z() * this->z_axis();
}


// ----------------------------------------------------------------------------
//: convert form world coordinate to local coordinate
vgl_point_3d<double > dbknee_tibia_cs::
wc_to_local(const vgl_point_3d<double >& pt) const
{
  return vgl_point_3d<double >(
    dot_product(pt-this->origin(), this->x_axis()),
    dot_product(pt-this->origin(), this->y_axis()),
    dot_product(pt-this->origin(), this->z_axis())
    );
}




// ----------------------------------------------------------------------------
//: convert in-place on a mesh from world coordinate to local coordinate
void dbknee_tibia_cs::
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

//
//// ============================================================================
//// dbknee_tibia_cs_params
//// ============================================================================
//
//// ----------------------------------------------------------------------------
//dbknee_tibia_cs_params::
//dbknee_tibia_cs_params(const vgl_box_3d<double >& bounding_box,
//                                   const vcl_map<vcl_string, double >& param_list)
//{
//  // default: take 1/2 of x-axis as the cropping box
//  this->cropping_box = bounding_box;
//  double width = bounding_box.width();
//  double height = bounding_box.height();
//  double depth = bounding_box.depth();
//
//
//  // if any param is specified then overwrite it
//  vcl_map< vcl_string, double >::const_iterator itr;
//
//  // fraction of the object's width will be kept in cropping
//  double crop_width_start = 0.0;
//  if( (itr = param_list.find("crop_width_start")) != param_list.end() )
//  {
//    crop_width_start = itr->second;
//  };
//
//  double crop_width_end = 1.0;
//  if( (itr = param_list.find("crop_width_end")) != param_list.end() )
//  {
//    crop_width_end = itr->second;
//  };
//
//  //
//  this->cropping_box.set_min_x(cropping_box.min_x() + width*crop_width_start);
//  this->cropping_box.set_max_x(cropping_box.min_x() + width*crop_width_end);
//
//
//  // fraction of the object's height will be kept in cropping
//  double crop_height_start = 0.0;
//  if( (itr = param_list.find("crop_height_start")) != param_list.end() )
//  {
//    crop_height_start = itr->second;
//  };
//
//  double crop_height_end = 1.0;
//  if( (itr = param_list.find("crop_height_end")) != param_list.end() )
//  {
//    crop_height_end = itr->second;
//  };
//  //
//  this->cropping_box.set_min_y(cropping_box.min_y() + height*crop_height_start);
//  this->cropping_box.set_max_y(cropping_box.min_y() + height*crop_height_end);
//
//  // fraction of the object's depth will be kept in cropping
//  double crop_depth_start = 0.0;
//  if( (itr = param_list.find("crop_depth_start")) != param_list.end() )
//  {
//    crop_depth_start = itr->second;
//  };
//
//  double crop_depth_end = 1.0;
//  if( (itr = param_list.find("crop_depth_end")) != param_list.end() )
//  {
//    crop_depth_end = itr->second;
//  };
//  //
//  this->cropping_box.set_min_z(cropping_box.min_z() + depth*crop_depth_start);
//  this->cropping_box.set_max_z(cropping_box.min_z() + depth*crop_depth_end);
//  
//  // x_min and x_max
//  if( (itr = param_list.find("crop_x_min")) != param_list.end() )
//  {
//    this->cropping_box.set_min_x(itr->second);
//  };
//  if( (itr = param_list.find("crop_x_max")) != param_list.end() )
//  {
//    this->cropping_box.set_max_x(itr->second);
//  };
//
//  // y_min and y_max
//  if( (itr = param_list.find("crop_y_min")) != param_list.end() )
//  {
//    this->cropping_box.set_min_y(itr->second);
//  };
//  if( (itr = param_list.find("crop_y_max")) != param_list.end() )
//  {
//    this->cropping_box.set_max_y(itr->second);
//  };
//  
//  // z_min and z_max
//  if( (itr = param_list.find("crop_z_min")) != param_list.end() )
//  {
//    this->cropping_box.set_min_z(itr->second);
//  };
//  if( (itr = param_list.find("crop_z_max")) != param_list.end() )
//  {
//    this->cropping_box.set_max_z(itr->second);
//  };
//
//  // voxel size
//  this->dx = 1;
//  this->dy = 1;
//  this->dz = 1;
//  if( (itr = param_list.find("dx")) != param_list.end() )
//  {
//    this->dx = itr->second;
//  };
//  if( (itr = param_list.find("dy")) != param_list.end() )
//  {
//    this->dy = itr->second;
//  };
//  if( (itr = param_list.find("dz")) != param_list.end() )
//  {
//    this->dz = itr->second;
//  };
//
//  // the notch point
//  this->notch_point = this->cropping_box.centroid();
//  double notch_x = this->notch_point.x();
//  double notch_y = this->notch_point.y();
//  double notch_z = this->notch_point.z();
//  if( (itr = param_list.find("notch_i")) != param_list.end() )
//  {
//    notch_x = (itr->second) * dx;
//  };
//  if( (itr = param_list.find("notch_j")) != param_list.end() )
//  {
//    notch_y = (itr->second) * dy;
//  };
//  if( (itr = param_list.find("notch_p")) != param_list.end() )
//  {
//    notch_z = itr->second * dz;
//  };
//
//
//  // if the notch "readl world" coordinates are specified explicity,
//  // the overwrite the one from the image
//
//  if( (itr = param_list.find("notch_x")) != param_list.end() )
//  {
//    notch_x = (itr->second);
//  };
//
//  if( (itr = param_list.find("notch_y")) != param_list.end() )
//  {
//    notch_y = (itr->second);
//  };
//
//  if( (itr = param_list.find("notch_z")) != param_list.end() )
//  {
//    notch_z = (itr->second);
//  };
//
//
//  this->notch_point.set(notch_x, notch_y, notch_z);
//}
//
//
