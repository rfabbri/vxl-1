#include <splr/splr_per_orbit_index_roster.h>

splr_per_orbit_index_roster::splr_per_orbit_index_roster(xscan_scan scan, worldpt_box bounding_box, double spacing)
  : scan_(scan), bounding_box_(bounding_box), spacing_(spacing) {}

void splr_per_orbit_index_roster::populate(orbit_index t){
  /* 
     Get the camera from the scan.
     Enumerate the lattice points in the bounding_box
     For each point, use the camera to find its projection
     in the image plane, and check that the projection lies
     in the image rectangle.  If so, add the point to the hash table entry.
  */

  vcl_vector<worldpt> points;
  //get the camera from the scan
  xmvg_perspective_camera<double> * camera = scan_(t);
  //get bounds on the image coordinates
  int uMax = camera->get_img_dim().x();
  int vMax = camera->get_img_dim().y();
    //triple-nested loop: enumerate all grid points in the box
  for (double x=box.min_x(); x <= box.max_x(); x += spacing_){
    for (double y=box.min_y(); y <= box.max_y(); y += spacing_){
      for (double z=box.min_z(); z <= box.max_z(); z += spacing_){
       worldpt pt(x,y,z);
       //use camera to find projection into image plane
       vgl_homg_point_2d<double> homg_pt(pt);
       vgl_homg_point_2d<double> image_plane_pt = camera->project(homg_pt);
       u = image_plane_pt.x();
       v = image_plane_pt.y();
       if (u >= 0 && u <= uMax && v >= 0 && v <= vMax){
         points.push_back(pt);
       }
      }
    }
  }
  map_.insert(t, points);
}

splr_iterator_pair splr_per_orbit_index_roster::num_points(orbit_index t) const {
  return map_.lookup(t).size();
}

splr_iterator_pair splr_per_orbit_index_roster::point(orbit_index t, unsigned int i) const {
  return map_.lookup(t)[i];
}
