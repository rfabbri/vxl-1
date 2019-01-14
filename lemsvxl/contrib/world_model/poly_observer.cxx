#include "poly_observer.h"
#include <vector>
#include <vgl/vgl_homg_plane_3d.h> 
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_line_3d_2_points.h> 
#include <vgl/vgl_closest_point.h> 
#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/algo/vgl_homg_operators_3d.h>
#include <vgl/algo/vgl_fit_plane_3d.h>
#include <vgl/algo/vgl_intersection.h>

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

void poly_observer::update (vgui_message const& msg) 
{
  vgui_message m = const_cast <vgui_message const& > (msg);
  const observable* o = static_cast<const observable*> (m.from);
  observable* o2= const_cast<observable*> (o);
  std::string type = o2->type_name();
  if (strcmp(type.data(),"obj_observable") == 0)
  {
    const obj_observable* o = static_cast<const obj_observable*> (m.from);
    obj_observable* observable = const_cast<obj_observable*> (o);
    handle_update(msg, observable);
  }
}

void poly_observer::handle_update(vgui_message const& msg, 
                                  obj_observable* observable) 
{
  const std::string* str = static_cast<const std::string*> (msg.data);

  std::vector<vgui_soview2D_polygon* > poly_list;
  if (str->compare("delete") == 0) {
    std::vector<vgui_soview2D_polygon* > p = objects[observable];
    std::vector<std::vector<vgui_soview2D_circle*> > ov = object_verts[observable];
    objects.erase(observable);
#if 0 //JLM
    if (p.size() == 0)
      std::cerr << "Unknown observable" << std::endl;
#endif
    for (unsigned i=0; i<p.size(); i++) 
      {
        this->remove(p[i]);    
        for(unsigned j = 0; j<ov[i].size(); j++)
          this->remove(ov[i][j]);
      }
  } else {

  std::map<int, vsol_polygon_3d_sptr> faces = observable->extract_faces();
  std::map<int, vsol_polygon_3d_sptr>::iterator iter = faces.begin();
  std::vector<std::vector<vgui_soview2D_circle*> >poly_verts;  

  while (iter != faces.end()) {
    // project the new object with the given camera
    vsol_polygon_3d_sptr obj = iter->second;
    vsol_polygon_2d_sptr poly_2d;
    proj_poly(obj, poly_2d);
    float *x, *y;
    this->get_vertices_xy(poly_2d, &x, &y);
    unsigned nverts = poly_2d->size();
    std::vector<vgui_soview2D_circle*> verts;
    this->set_foreground(0,1,0);
    for(unsigned i = 0; i<nverts; ++i)
      {
        vgui_soview2D_circle* sopt = this->add_circle(x[i],y[i],1.0f);
        verts.push_back(sopt);
      }
    poly_verts.push_back(verts);
    this->set_foreground(1,1,0);
    vgui_soview2D_polygon* polygon = this->add_polygon(nverts, x, y);
    poly_list.push_back(polygon);
    observable->set_id(iter->first, polygon->get_id());

    // draw the normal
    /*obj->compute_bounding_box();
    vgl_box_3d<double> b(obj->get_min_x(), obj->get_min_y(), obj->get_min_z(),
      obj->get_max_x(), obj->get_max_y(), obj->get_max_z());
    vsol_point_3d_sptr p = new vsol_point_3d(b.centroid());
    vgl_vector_3d<double> v = obj->normal_at_point(p);
    v *= 10;
    vsol_point_3d_sptr p1 = new vsol_point_3d(p->get_p());
    p1->set_x(p->x() + v.x());
    p1->set_y(p->y() + v.y());
    p1->set_z(p->z() + v.z());
    vsol_point_3d_sptr p2(p);
  
    std::vector<vsol_point_3d_sptr> ver(3);
    ver[0] = p;
    ver[1] = p1;
    ver[2] = p2;
    vsol_polygon_3d_sptr normal = new vsol_polygon_3d(ver);
    vsol_polygon_2d_sptr n;
    proj_poly(normal, n);
    this->add_line(n->vertex(0)->x(), n->vertex(0)->y(), n->vertex(1)->x(), n->vertex(1)->y());
    this->add_circle(n->vertex(1)->x(), n->vertex(1)->y(), 2);*/
    //////////////////////////

    // get the inner faces connected to this face
    int face_id = iter->first;
    //std::cout << "extracting face=" << face_id << std::endl;
    std::map<int, vsol_polygon_3d_sptr> inner_faces = observable->extract_inner_faces(face_id);
    std::map<int, vsol_polygon_3d_sptr>::iterator inner_iter= inner_faces.begin();
    while (inner_iter != inner_faces.end()) {
      vsol_polygon_3d_sptr poly = inner_iter->second;
      vsol_polygon_2d_sptr poly_2d;
      proj_poly(poly, poly_2d);
      float *x, *y;
      this->get_vertices_xy(poly_2d, &x, &y);
      vgui_soview2D_polygon* polygon = this->add_polygon(poly_2d->size(), x, y);
      poly_list.push_back(polygon);
      inner_iter++;
    }
    iter++;
  }

    if (str->compare("new") == 0) {
      //this->set_foreground(0.0, 0.5, 0.5);
      objects[observable] = poly_list;
      object_verts[observable] = poly_verts;
    } else if (str->compare("update") == 0) {
      std::vector<vgui_soview2D_polygon* > p = objects[observable];
      std::vector<std::vector<vgui_soview2D_circle* > > ov = 
        object_verts[observable];
      for (unsigned i=0; i<p.size(); i++) {
        this->remove(p[i]);
        for(unsigned j = 0; j<ov[i].size(); j++)
          this->remove(ov[i][j]);
      }
      objects[observable] = poly_list;
      object_verts[observable] = poly_verts;
    } else if (str->compare("move") == 0) {
      std::vector<vgui_soview2D_polygon* > p = objects[observable];
      std::vector<std::vector<vgui_soview2D_circle* > > ov = 
        object_verts[observable];
      for (unsigned i=0; i<p.size(); i++) {
        this->remove(p[i]);
        for(unsigned j = 0; j<ov[i].size(); j++)
          this->remove(ov[i][j]);
      }
      objects[observable] = poly_list;
      object_verts[observable] = poly_verts;
    }
  }
  
  this->post_overlay_redraw();
}


void poly_observer::get_vertices_xy(vsol_polygon_2d_sptr poly2d, 
                                          float **x, float **y)
{
  int n = poly2d->size();
  *x = (float*) malloc(sizeof(float) * n);
  *y = (float*) malloc(sizeof(float) * n);
  for (int i=0; i<n; i++) {
    (*x)[i] = poly2d->vertex(i)->x();
    (*y)[i] = poly2d->vertex(i)->y();
   //std::cout << "X=" << poly2d->vertex(i)->x() << " Y=" << poly2d->vertex(i)->y() << std::endl;
  }
}

void poly_observer::get_vertices_xyz(vsol_polygon_3d_sptr poly3d, 
                                          double **x, double **y, double **z)
{
  int n = poly3d->size();
  *x = (double*) malloc(sizeof(double) * n);
  *y = (double*) malloc(sizeof(double) * n);
  *z = (double*) malloc(sizeof(double) * n);
  for (int i=0; i<n; i++) {
    (*x)[i] = poly3d->vertex(i)->x();
    (*y)[i] = poly3d->vertex(i)->y();
    (*z)[i] = poly3d->vertex(i)->z();
    //std::cout << i << " " << *(poly3d->vertex(i)) << std::endl;
  }
}

vsol_polygon_2d_sptr 
poly_observer::get_vsol_polygon_2d(vgui_soview2D_polygon* polygon)
{
  float* x = polygon->x;
  float* y = polygon->y;
  unsigned n = polygon->n;
  std::vector<vsol_point_2d_sptr> vertices;
  for (unsigned i=0; i<n; i++) {
    vsol_point_2d_sptr v = new vsol_point_2d(x[i], y[i]);
    vertices.push_back(v);
  }

  vsol_polygon_2d_sptr poly = new vsol_polygon_2d(vertices);
  return poly;
}

vgl_homg_plane_3d<double> poly_observer::find_plane(vsol_polygon_3d_sptr poly3d) 
{ 
  vgl_fit_plane_3d<double> fit_plane;
  for (unsigned i=0; i<poly3d->size(); i++) {
    fit_plane.add_point(poly3d->vertex(i)->x(), poly3d->vertex(i)->y(), poly3d->vertex(i)->z());
  }

  fit_plane.fit(0.1);
  vgl_homg_plane_3d<double> plane = fit_plane.get_plane();

  return plane;
}

unsigned poly_observer::get_selected_3d_vertex_index(unsigned poly_id)
{
  obj_observable * found_obj = 0;
  unsigned found_poly_index = 0;
  for(std::map<obj_observable *, std::vector<vgui_soview2D_polygon* > >::iterator oit = objects.begin(); oit != objects.end(); ++oit)
    {
      unsigned pindex = 0;
      std::vector<vgui_soview2D_polygon* > polys = oit->second;
      for(std::vector<vgui_soview2D_polygon* >::iterator pit = polys.begin();
          pit != polys.end(); ++pit, ++pindex)
        if(*pit && (*pit)->get_id() == poly_id)
          {
            found_obj = oit->first;
            found_poly_index = pindex;
          }
    }
  if(!found_obj)
    return 0;
  std::vector<vgui_soview2D_circle* > verts = 
    object_verts[found_obj][found_poly_index];
  unsigned found_vert_index = 0;
  for(std::vector<vgui_soview2D_circle* >::iterator vit = verts.begin();
      vit != verts.end(); ++vit, found_vert_index++)
    if(this->is_selected((*vit)->get_id()))
      return found_vert_index;
  return 0;
}
