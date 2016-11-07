#include "poly_utils.h"
#include <vgl/algo/vgl_fit_plane_3d.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_point_3d.h>
#include <vgl/vgl_closest_point.h>

vsol_polygon_3d_sptr poly_utils::move_points_to_plane(vsol_polygon_3d_sptr polygon)
{
  vgl_fit_plane_3d<double> fitter;
  for (unsigned i=0; i<polygon->size(); i++) {
    fitter.add_point(polygon->vertex(i)->x(), 
      polygon->vertex(i)->y(), polygon->vertex(i)->z());
  }

  vgl_homg_plane_3d<double> plane;

  if (fitter.fit(0.1)) {
  //fitter.fit();

  plane = fitter.get_plane();
  } else {
    vcl_cout << "NO FITTING" << vcl_endl;
    return 0;
  }


  // find the closest point on the plane and replace it for each point
  vcl_vector<vsol_point_3d_sptr> points;
  for (unsigned i=0; i<polygon->size(); i++) {
    vgl_homg_point_3d<double> hp(polygon->vertex(i)->x(), 
      polygon->vertex(i)->y(), polygon->vertex(i)->z());
    vgl_homg_point_3d<double> p = vgl_closest_point(plane, hp);
    points.push_back(new vsol_point_3d(p.x()/p.w(), p.y()/p.w(), p.z()/p.w()));
    
  }
  vsol_polygon_3d_sptr new_polygon = new vsol_polygon_3d(points);
  return new_polygon;
}

vsol_polygon_3d_sptr poly_utils::move_points_to_plane(vcl_vector<vsol_point_3d_sptr> points)
{
  vgl_fit_plane_3d<double> fitter;
  vcl_cout << "fitting----------------" << vcl_endl;
  for (unsigned i=0; i<points.size(); i++) {
    fitter.add_point(points[i]->x(), 
      points[i]->y(), points[i]->z());
    vcl_cout << *(points[i]) << vcl_endl;
  }

  vgl_homg_plane_3d<double> plane;

  if (fitter.fit(1.0)) {
  // fitter.fit();
  plane = fitter.get_plane();
  } else {
    vcl_cout << "NO FITTING" << vcl_endl;
    return 0;
  }


  // find the closest point on the plane and replace it for each point
  vcl_vector<vsol_point_3d_sptr> new_points;
  for (unsigned i=0; i<points.size(); i++) {
    vgl_homg_point_3d<double> hp(points[i]->x(), points[i]->y(), points[i]->z());
    vgl_homg_point_3d<double> p = vgl_closest_point(plane, hp);
    new_points.push_back(new vsol_point_3d(p.x()/p.w(), p.y()/p.w(), p.z()/p.w()));
    
  }
  vsol_polygon_3d_sptr polygon = new vsol_polygon_3d(new_points);
  return polygon;
}
