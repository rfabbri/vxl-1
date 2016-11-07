#include "poly_cam_observer.h"

#include <vgl/vgl_homg_plane_3d.h> 
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_line_3d_2_points.h> 
#include <vgl/vgl_closest_point.h> 
#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_homg_operators_3d.h>
#include <vgl/algo/vgl_fit_plane_3d.h>

#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_matrix_inverse.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>

#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_point_3d_sptr.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/algo/vpgl_backproject.h>


void poly_cam_observer::set_ground_plane(double x1, double y1, double x2, double y2)
{
  vgl_homg_point_3d<double> world_point1, world_point2;
  vgl_homg_plane_3d<double> xy_plane(0, 0, 1, 0);

  intersect_ray_and_plane(vgl_homg_point_2d<double> (x1, y1), xy_plane, world_point1);
  intersect_ray_and_plane(vgl_homg_point_2d<double> (x2, y2), xy_plane, world_point2);

  // define the third point in z direction which is the normal to the z=0 plane
  vgl_homg_point_3d<double> world_point3(world_point1.x(), world_point1.y(), 
    world_point1.z()+3.0, world_point1.w());

  proj_plane_ = vgl_homg_plane_3d<double> (world_point1, world_point2, world_point3);
}
void poly_cam_observer::proj_point(vgl_point_3d<double> world_pt, vgl_point_2d<double> &image_pt) {
  double u,v;
  camera_->project(world_pt.x(), world_pt.y(), world_pt.z(), u,v);
  image_pt.set(u,v);
  return;
}

void poly_cam_observer::proj_poly(vsol_polygon_3d_sptr poly3d, 
                               vsol_polygon_2d_sptr& poly2d)
{
  vcl_vector<vsol_point_2d_sptr> vertices;
if(!poly3d)
  return;
  for (unsigned i=0; i<poly3d->size(); i++) {

    //vgl_homg_point_2d<double> hp = camera_->project(vgl_homg_point_3d<double> (poly3d->vertex(i)->x(), poly3d->vertex(i)->y(), poly3d->vertex(i)->z()));
    //vsol_point_2d_sptr p = new vsol_point_2d(hp.x()/hp.w(), hp.y()/hp.w());
    double u = 0,v = 0;
    //vcl_cout << "point " << *(poly3d->vertex(i)) << vcl_endl;
    camera_->project(poly3d->vertex(i)->x(), poly3d->vertex(i)->y(), poly3d->vertex(i)->z(),u,v); 
    vsol_point_2d_sptr p = new vsol_point_2d(u,v);
    vertices.push_back(p);
  }

  poly2d = new vsol_polygon_2d (vertices);
}

void poly_cam_observer::proj_poly(vcl_vector<dbmsh3d_vertex*> verts, vcl_vector<vgl_point_2d<double> > &projections)
{
  for (unsigned v=0; v<verts.size(); v++) {
    dbmsh3d_vertex* vert = (dbmsh3d_vertex*)verts[v];
    double img_x = 0, img_y = 0;
    camera_->project(vert->pt().x(),vert->pt().y(),vert->pt().z(),img_x,img_y);
    vgl_point_2d<double> projection(img_x,img_y);
    projections.push_back(projection);
  }
}

//: returns the distance between a ray and a 3D polygon
bool poly_cam_observer::find_intersection_points(vgl_homg_point_2d<double> const img_point1,
                                             vgl_homg_point_2d<double> const img_point2,
                                             vsol_polygon_3d_sptr poly3d,
                                             vgl_point_3d<double>& point1,
                                             vgl_point_3d<double>& l1, vgl_point_3d<double>& l2, // end points of the first polygon segment
                                             vgl_point_3d<double>& point2,
                                             vgl_point_3d<double>& l3, vgl_point_3d<double>& l4) // end points of the second polygon segment
{
  
  vgl_homg_plane_3d<double> poly_plane = poly3d->plane();
  //vgl_homg_plane_3d<double> poly_plane = find_plane(poly3d);

 /* vsol_point_3d_sptr v = poly3d->vertex(0);
  vgl_vector_3d<double> normal = poly3d->normal_at_point(poly3d->vertex(0));
  vgl_homg_point_3d<double> hv(v->x(), v->y(), v->z());
  vgl_homg_plane_3d<double> poly_plane(normal, hv);*/

  vgl_homg_point_3d<double> p1,p2;
  intersect_ray_and_plane(img_point1, poly_plane, p1);
  intersect_ray_and_plane(img_point2, poly_plane, p2);

  if (is_ideal(p1))
    vcl_cout << "p1 is ideal" << vcl_endl;
  if (is_ideal(p2))
    vcl_cout << "p2 is ideal" << vcl_endl;

  vgl_point_3d<double> non_homg_p1(p1);
  vgl_point_3d<double> non_homg_p2(p2);

  double *x_list, *y_list, *z_list;
  get_vertices_xyz(poly3d, &x_list, &y_list, &z_list);
  
  double dist1 = vgl_distance_to_closed_polygon (x_list, y_list, z_list, poly3d->size(), 
    non_homg_p1.x(), non_homg_p1.y(), non_homg_p1.z());

  double dist2 = vgl_distance_to_closed_polygon (x_list, y_list, z_list, poly3d->size(), 
    non_homg_p2.x(), non_homg_p2.y(), non_homg_p2.z());
 
  //vcl_cout << "dist1=" << dist1 << "   dist2=" << dist2 << vcl_endl;

  // if it is close enough, find the intersection
  if ((dist1 < 2.0) && (dist2 < 2.0)) {
    double point1_x, point1_y, point1_z, 
      point2_x, point2_y, point2_z;

    int edge_index1 = vgl_closest_point_to_closed_polygon(point1_x, point1_y, point1_z, 
      x_list, y_list, z_list, poly3d->size(),
      non_homg_p1.x(), non_homg_p1.y(), non_homg_p1.z());
    
    int edge_index2 = vgl_closest_point_to_closed_polygon(point2_x, point2_y, point2_z, 
      x_list, y_list, z_list, poly3d->size(),
      non_homg_p2.x(), non_homg_p2.y(), non_homg_p2.z());

    if (edge_index1 == edge_index2) {
      vcl_cerr << "poly_cam_observer::find_intersection_points() -- Both points are on the same edge!!!" << vcl_endl;
      return false;
    }

    l1 = vgl_point_3d<double> (x_list[edge_index1], y_list[edge_index1], z_list[edge_index1]);
    int next_index = edge_index1+1;
    if (next_index == poly3d->size()) 
      next_index = 0;
    l2 = vgl_point_3d<double> (x_list[next_index], y_list[next_index], z_list[next_index]); 
    l3 = vgl_point_3d<double> (x_list[edge_index2], y_list[edge_index2], z_list[edge_index2]);
    next_index = edge_index2+1;
    if (edge_index2+1 == poly3d->size()) 
      next_index = 0;
    l4 = vgl_point_3d<double> (x_list[next_index], y_list[next_index], z_list[next_index]); 
    point1 = vgl_point_3d<double>(point1_x, point1_y, point1_z);
    point2 = vgl_point_3d<double>(point2_x, point2_y, point2_z);
    return true;
  }
  return false;
}

//: determines if a given line intersects any faces of the mesh, if intersects 
// then returns the points in point1 and point2
bool poly_cam_observer::intersect(obj_observable* obj, unsigned face_id, 
                              float x1, float y1, float x2, float y2)
{
  vgl_homg_point_2d<double> image_point1(x1, y1);
  vgl_homg_point_2d<double> image_point2(x2, y2);

  if (obj) {
    vsol_polygon_3d_sptr face = obj->extract_face(face_id);
    if (face != 0) {
      vgl_point_3d<double> l1, l2, l3, l4;
      vgl_point_3d<double> point1, point2;
      if (this->find_intersection_points(image_point1, image_point2, face, point1, l1, l2, point2, l3, l4)) {
        obj->divide_face(face_id, l1, l2, point1, l3, l4, point2);
        return true;
      }
    }
  }
  return false;
}

//: determines if a given line intersects any faces of the mesh, if intersects 
// then returns the points in point1 and point2
/// ASK DAN, how about the case of rational camera
bool poly_cam_observer::intersect(float x1, float y1, float x2, float y2)
{
  vcl_map<obj_observable *, vcl_vector<vgui_soview2D_polygon* > >::iterator itr = objects.begin();
  vcl_vector<obj_observable*> intersecting_obs;
  vgl_homg_point_2d<double> image_point1(x1, y1);
  vgl_homg_point_2d<double> image_point2(x2, y2);
 
  // method of intersecting is different depending on what type of camera we are using
  if (vpgl_proj_camera<double> *pro_cam = dynamic_cast<vpgl_proj_camera<double>*>(camera_)) {

    // first, check the intersection with the bounding box
    for (; itr != objects.end(); itr++) {
      vgl_box_3d<double> bb;
      obj_observable* obs =(*itr).first;
      dbmsh3d_mesh* mesh = new dbmsh3d_mesh(*(obs->get_object()));
      detect_bounding_box(mesh, bb);
      vgl_point_3d<double> point1, point2;
      if (intersect_ray_and_box(bb, image_point1, point1) && intersect_ray_and_box(bb,image_point2, point2)) {
        intersecting_obs.push_back(obs);
      }
    }
  }
 
  // choose the polygon amongst the selected objects
  // select the one closest to the camera
  vgl_homg_point_3d<double> cam_center;
  if (vpgl_proj_camera<double> *pro_cam = dynamic_cast<vpgl_proj_camera<double>*>(camera_)) {
    cam_center = pro_cam->camera_center();
   }
   else if (vpgl_rational_camera<double> *rat_cam = dynamic_cast<vpgl_rational_camera<double>*>(camera_)) { 
     // THIS IS JUST A PLACE HOLDER TILL WE FIGURE OUT SOMETHING BETTER...
     cam_center.set(rat_cam->offset(rat_cam->X_INDX),
                    rat_cam->offset(rat_cam->Y_INDX),
                    rat_cam->offset(rat_cam->Z_INDX));
   }
   else {
     //unsupported camera type
     vcl_cerr << "poly_cam_observer::intersect(): unsupported camera type!\n";
     return false;
   }
    // choose the polygon amongst the selected objects
    // select the one closest to the camera
    int closest_face_id = 0;
    vgl_point_3d<double> seg_l1, seg_l2, seg_l3, seg_l4, inters_point1, inters_point2;
    double dist = 1e06;
    bool found=false;
    for (unsigned i=0; i<intersecting_obs.size(); i++) {
      obj_observable* obs = intersecting_obs[i];
      vcl_map<int, vsol_polygon_3d_sptr> faces = obs->extract_faces();
      vcl_map<int, vsol_polygon_3d_sptr>::iterator iter = faces.begin();
   
      while (iter != faces.end()) {
        vgl_point_3d<double> l1, l2, l3, l4;
        vgl_point_3d<double> point1, point2;
        vsol_polygon_3d_sptr face = iter->second;
        if (this->find_intersection_points(image_point1, image_point2, face, point1, l1, l2, point2, l3, l4)) {
          vcl_cout << "found intersecting line:" << vcl_endl;
          found = true;
          double dist1 = vgl_distance(cam_center, vgl_homg_point_3d<double> (point1));
          double dist2 = vgl_distance(cam_center, vgl_homg_point_3d<double> (point2));
          double avg_dist = (dist1 + dist2)/2;
          if (avg_dist < dist) {
            dist = avg_dist;
            closest_face_id = iter->first;
            seg_l1 = l1;
            seg_l2 = l2;
            seg_l3 = l3;
            seg_l4 = l4;
            inters_point1 = point1;
            inters_point2 = point2;
          }
        }
        iter++;
      }
      if (found) {
        obs->divide_face(closest_face_id, seg_l1, seg_l2, inters_point1, 
          seg_l3, seg_l4, inters_point2);
        return true;
      }
  }
  return false;

}

//: return the 3d intersection point between the ray generated by a single image point and a face of the object
bool poly_cam_observer::intersect(obj_observable* obj, float img_x, float img_y, unsigned face_id, vgl_point_3d<double> &pt3d)
{
  vgl_homg_point_2d<double> img_point(img_x,img_y);
  if (obj) {
    vsol_polygon_3d_sptr face = obj->extract_face(face_id);
    if (face) {
      return (this->find_intersection_point(img_point,face,pt3d));
    }
  }
  return false;
}

//: determines the intersection point between a single ray and a 3D polygon
bool poly_cam_observer::find_intersection_point(vgl_homg_point_2d<double> img_point, 
                                                vsol_polygon_3d_sptr poly3d, 
                                                vgl_point_3d<double> &point3d)
{
  vgl_homg_plane_3d<double> poly_plane = find_plane(poly3d);
  vgl_homg_point_3d<double> p3d_homg;
  
  intersect_ray_and_plane(img_point,poly_plane,p3d_homg);

  if (is_ideal(p3d_homg)) {
    vcl_cout << "intersection point is ideal!" <<vcl_endl;
  }
  vgl_point_3d<double> p3d(p3d_homg);

  double *x_list, *y_list, *z_list;
  this->get_vertices_xyz(poly3d, &x_list, &y_list, &z_list);
  
  double dist = vgl_distance_to_closed_polygon(x_list,y_list,z_list,poly3d->size(),p3d.x(),p3d.y(),p3d.z());

  //vcl_cout << "dist = "<<dist<<vcl_endl;
  if (dist)
    return false;
  
  return true;
}

void poly_cam_observer::connect_inner_face(obj_observable* obj, 
                        unsigned face_id, 
                        vsol_polygon_2d_sptr poly2d)
{
  if (obj) {
    vsol_polygon_3d_sptr poly = obj->extract_face(face_id);
    if (poly != 0) {
      vgl_homg_plane_3d<double> plane = poly->plane();
      vsol_polygon_3d_sptr poly3d;

      // back project the inner polygon to the plane of the outer polygon
      backproj_poly(poly2d, poly3d, plane);

      // attach the inner polygon to the outer one
      obj->attach_inner_face(face_id, poly3d);
    }
  }
}

void poly_cam_observer::backproj_point(vsol_point_2d_sptr p2d, vsol_point_3d_sptr& p3d)
{
 
  vgl_homg_point_3d<double> world_point;

  vgl_homg_point_2d<double> image_point(p2d->x(), p2d->y());
  //vcl_cout << " Before projection---> x=" << p->x() << "  y=" << p->y() << vcl_endl;

  intersect_ray_and_plane(image_point,proj_plane_,world_point);
  double x = world_point.x()/world_point.w();
  double y = world_point.y()/world_point.w();
  double z = world_point.z()/world_point.w();  
  p3d = new vsol_point_3d (x, y, z);
}

void poly_cam_observer::backproj_poly(vsol_polygon_2d_sptr poly2d, 
                              vsol_polygon_3d_sptr& poly3d,
                              double dist)
{
  // create a new plane which is the translation of the proj_plane
  // by a dist,
  // Remember the definition of a plane, which is 
  // a*x + b*y + c*z + d = 0
  // where (a,b,c) is your normal, and d is negative distance to origin. 

  // recalculating d based on the old d and the dist
  double d = proj_plane_.d();
  double a = proj_plane_.a();
  double b = proj_plane_.b();
  double c = proj_plane_.c();

  double trans_d = d - dist;
  vgl_homg_plane_3d<double> trans_plane(a, b, c, 1*trans_d);
  backproj_poly(poly2d, poly3d, trans_plane);
}

//// ASK DAN!!!!
void poly_cam_observer::backproj_poly(vsol_polygon_2d_sptr poly2d, 
                              vsol_polygon_3d_sptr& poly3d,
                              vgl_homg_plane_3d<double> proj_plane)
{

  vcl_vector<vsol_point_3d_sptr> projected_list;
  vgl_homg_point_3d<double> world_point;

  for(unsigned i=0; i<poly2d->size(); i++) {
    vsol_point_2d_sptr p = poly2d->vertex(i);
    vgl_homg_point_2d<double> image_point(p->x(), p->y());
    //vcl_cout << " Before projection---> x=" << p->x() << "  y=" << p->y() << vcl_endl;

    intersect_ray_and_plane(image_point,proj_plane,world_point);
    double x = world_point.x()/world_point.w();
    double y = world_point.y()/world_point.w();
    double z = world_point.z()/world_point.w();
      //vcl_cout << i << "            After  x=" << x << " y=" << y << 
     //   " z=" << z << vcl_endl;

    projected_list.push_back(new vsol_point_3d (x, y, z));
  }

 /*   double x=0, y=0, z=0;
    if (vpgl_proj_camera<double>* pro_cam = dynamic_cast<vpgl_proj_camera<double>*>(camera_)) {

      vgl_homg_line_3d_2_points<double> ray = pro_cam->backproject(image_point);
      vgl_homg_operators_3d<double> oper;
      vgl_homg_point_3d<double> proj_p = oper.intersect_line_and_plane(ray,proj_plane);
      x = proj_p.x()/proj_p.w();
      y = proj_p.y()/proj_p.w();
      z = proj_p.z()/proj_p.w();
      vcl_cout << i << "            After  x=" << x << " y=" << y << 
        " z=" << z << vcl_endl;

      projected_list.push_back(new vsol_point_3d (x, y, z));
    }
    // rational camera
    else if (vpgl_rational_camera<double> *rat_cam = dynamic_cast<vpgl_rational_camera<double>*>(camera_)) {
    vgl_point_2d<double>  p2d(img_point.x()/img_point.w(),img_point.y()/img_point.w());
    // initial guess for backprojection - just use center point of image 
    vgl_point_3d<double> guess(rat_cam->offset(rat_cam->X_INDX),
                               rat_cam->offset(rat_cam->Y_INDX),
                               rat_cam->offset(rat_cam->Z_INDX));
    vgl_plane_3d<double> plane_nonhomg(plane);
    vgl_point_3d<double> p;

      if (!vpgl_backproject::bproj_plane(*rat_cam, p2d, plane_nonhomg, guess, p)) {
        vcl_cout << "vpgl_backproject::broj_plane() failed." << vcl_endl;
        return false;
      }
      world_point.set(p.x(),p.y(),p.z());
  }
  // other camera type
  else {
    vcl_cout << "error: unsupported camera type!" << vcl_endl;
  }
  */
  poly3d = new vsol_polygon_3d(projected_list);
}

bool poly_cam_observer::intersect_ray_and_box(vgl_box_3d<double> box, 
                                           vgl_homg_point_2d<double> img_point,
                                           vgl_point_3d <double> &point)
{
  // test the intersection of the line with 6 planes of the box
  vcl_vector<vgl_homg_plane_3d<double> > planes;

  // make the box a thad bigger so that we accept closeby points
  box.expand_about_centroid (2);
  vgl_homg_point_3d<double> min_point( box.min_point());
  vgl_homg_point_3d<double> max_point( box.max_point());

  // create planes from plane parameteres a, b, c, d
  vgl_plane_3d<double> plane1(1, 0, 0, min_point.x());
  vgl_plane_3d<double> plane2(1, 0, 0, max_point.x());
  vgl_plane_3d<double> plane3(0, 1, 0, min_point.y());
  vgl_plane_3d<double> plane4(0, 1, 0, max_point.y());
  vgl_plane_3d<double> plane5(0, 0, 1, min_point.z());
  vgl_plane_3d<double> plane6(0, 0, 1, max_point.z());

  planes.push_back(plane1);
  planes.push_back(plane2);
  planes.push_back(plane3);
  planes.push_back(plane4);
  planes.push_back(plane5);
  planes.push_back(plane6);

  vcl_vector< vgl_point_3d <double> > intersection_points;

  for (unsigned i=0; i<planes.size(); i++) {
    vgl_homg_point_3d<double> p;
    intersect_ray_and_plane(img_point,planes[i],p);
    vgl_point_3d<double> ip(p);
    if (box.contains(ip))
      intersection_points.push_back(ip);  
  }
  
  if (intersection_points.size() > 0) 
    return true;
  else {
    vcl_cout << "Rays do not intersect the box" << vcl_endl;
    return false;
  }
}

bool poly_cam_observer::intersect_ray_and_plane(vgl_homg_point_2d<double> img_point,
                                            vgl_homg_plane_3d<double> plane,
                                            vgl_homg_point_3d<double> &world_point)
{

  // projective camera
  if (vpgl_proj_camera<double> *pro_cam = dynamic_cast<vpgl_proj_camera<double>*>(camera_)) {
    vgl_homg_line_3d_2_points<double> ray = pro_cam->backproject(img_point);
    vgl_homg_operators_3d<double> oper;
    vgl_homg_point_3d<double> p = oper.intersect_line_and_plane(ray,plane);
    world_point = p;
  }

  // rational camera
  else if (vpgl_rational_camera<double> *rat_cam = dynamic_cast<vpgl_rational_camera<double>*>(camera_)) {
    vgl_point_2d<double>  p2d(img_point.x()/img_point.w(),img_point.y()/img_point.w());
    // initial guess for backprojection - just use center point of image 
    vgl_point_3d<double> guess(rat_cam->offset(rat_cam->X_INDX),
                               rat_cam->offset(rat_cam->Y_INDX),
                               rat_cam->offset(rat_cam->Z_INDX));
    vgl_plane_3d<double> plane_nonhomg(plane);
    vgl_point_3d<double> p;

      if (!vpgl_backproject::bproj_plane(*rat_cam, p2d, plane_nonhomg, guess, p)) {
        vcl_cout << "vpgl_backproject::broj_plane() failed." << vcl_endl;
        return false;
      }
      world_point.set(p.x(),p.y(),p.z());
  }

  // other camera type
  else {
    vcl_cout << "error: unsupported camera type!" << vcl_endl;
  }
  
  return true;
}

vgl_vector_3d<double> poly_cam_observer::camera_direction(vgl_point_3d<double> origin)
{
  vgl_vector_3d<double> direction(0.0, 0.0 ,0.0);

  if (vpgl_proj_camera<double> *pro_cam = dynamic_cast<vpgl_proj_camera<double>*>(camera_)) {
    //cam_center.set(pro_cam->camera_center().x(),pro_cam->camera_center().y(),pro_cam->camera_center().z());
    vgl_point_3d<double> cam_center(pro_cam->camera_center());
    direction.set(cam_center.x() - origin.x(), cam_center.y() - origin.y(), cam_center.z() - origin.z());
    direction = direction / direction.length();

    return direction;
  }
  else if (vpgl_rational_camera<double> *rat_cam = dynamic_cast<vpgl_rational_camera<double>*>(camera_)) {
    vcl_cerr << "Error: you probably want to use camera_direction_rational()!\n";
    
   }
   else {
     //unsupported camera type
     vcl_cerr << "Error: poly_observer::intersect(): unsupported camera type!\n";
   }
   return direction;
}

bool poly_cam_observer::shift_rational_camera(double dx, double dy)
{
  if (vpgl_rational_camera<double> *rat_cam = dynamic_cast<vpgl_rational_camera<double>*>(camera_)) {
    double u_off,v_off;

    rat_cam->image_offset(u_off,v_off);
    rat_cam->set_image_offset(u_off + dx, v_off + dy);

  }
  else {
    vcl_cerr << "Error: camera is not rational\n";
    return false;
  }
  vcl_map<obj_observable *, vcl_vector<vgui_soview2D_polygon* > >::iterator objit;
  for (objit = objects.begin(); objit != objects.end(); objit++) {
    vgui_message msg;
    msg.from = objit->first;
    msg.data = new vcl_string("update");
    update(msg);
  }

  return true;
}


vgl_vector_3d<double> poly_cam_observer::camera_direction_rational(bgeo_lvcs& lvcs)
{
  vgl_vector_3d<double> direction(0,0,0);

  if (vpgl_proj_camera<double> *pro_cam = dynamic_cast<vpgl_proj_camera<double>*>(camera_)) {
    vcl_cerr << "Error: Use camera_direction() instead of camera_direction_rational() with a projective camera.\n";
    
    return direction;
  }
  else if (vpgl_rational_camera<double> *rat_cam = dynamic_cast<vpgl_rational_camera<double>*>(camera_)) {
     /* Assume that the camera is a satellite camera, and center is at infinity. 
      * Calculate the direction based on intersection points of two planes near the ground */

    double pix_offset_x = rat_cam->offset(rat_cam->U_INDX);
    double pix_offset_y = rat_cam->offset(rat_cam->V_INDX);
    double z_offset = rat_cam->offset(rat_cam->Z_INDX);


    // use center point of image
    vgl_point_2d<double>  img_pt(pix_offset_x,pix_offset_y);

    double zval1 = z_offset, zval2 = z_offset + 200;

    // initial guess for backprojection - just use center point of image 
    vgl_point_3d<double> guess(rat_cam->offset(rat_cam->X_INDX),
                               rat_cam->offset(rat_cam->Y_INDX),
                               rat_cam->offset(rat_cam->Z_INDX));
    vgl_vector_3d<double> plane_normal(0.0,0.0,1.0);
    vgl_point_3d<double> plane_point1(rat_cam->offset(rat_cam->X_INDX),
                                      rat_cam->offset(rat_cam->Y_INDX),zval1);
    vgl_point_3d<double> plane_point2(rat_cam->offset(rat_cam->X_INDX),
                                      rat_cam->offset(rat_cam->Y_INDX),zval2);

    vgl_plane_3d<double> plane1(plane_normal,plane_point1);
    vgl_plane_3d<double> plane2(plane_normal,plane_point2);

    vgl_point_3d<double> p1,p2;
    if (!vpgl_backproject::bproj_plane(*rat_cam, img_pt, plane1, plane_point1, p1)) {
      vcl_cerr << "Error: vpgl_backproject::broj_plane() failed." << vcl_endl;
    }
    if (!vpgl_backproject::bproj_plane(*rat_cam, img_pt, plane2, plane_point2, p2)) {
      vcl_cerr << "Error: vpgl_backproject::broj_plane() failed." << vcl_endl;
    }
    // convert p1 and p2 to lvcs
    double x,y,z;
    lvcs.global_to_local(p1.x(),p1.y(),p1.z(),bgeo_lvcs::wgs84,x,y,z,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
    p1.set(x,y,z);
    lvcs.global_to_local(p2.x(),p2.y(),p2.z(),bgeo_lvcs::wgs84,x,y,z,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
    p2.set(x,y,z);

    // find vector defined by p1 and p2
    vgl_vector_3d<double> direction(p2.x() - p1.x(), p2.y() - p1.y(), p2.z() - p1.z());
    direction = direction / direction.length();

    return direction;
   }
   else {
     //unsupported camera type
     vcl_cerr << "Error: poly_observer::intersect(): unsupported camera type!\n";
   }
   return direction;
}
