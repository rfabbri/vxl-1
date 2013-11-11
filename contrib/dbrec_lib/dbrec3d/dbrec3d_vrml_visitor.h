// This is dbrec3d_vrml_visitor.h
#ifndef dbrec3d_vrml_visitor_h
#define dbrec3d_vrml_visitor_h

//:
// \file
// \brief  A class that implements the dbrec3d_part concept for composite parts (i.e non-leafs). 
// \author Gamze Tunali  
// \date   01-Sep-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "dbrec3d_primitive_kernel_part.h"
#include "dbrec3d_part.h"
#include "dbrec3d_octree_context.h"

#include <bvrml/bvrml_write.h>
#include <vgl/vgl_sphere_3d.h>

class dbrec3d_vrml_visitor
{

public:
  dbrec3d_vrml_visitor(vcl_ofstream& os) : os_(os), radius_(1.0) { bvrml_write::write_vrml_header(os); }
  dbrec3d_vrml_visitor(vcl_ofstream& os, double radius) : os_(os), radius_(radius) { bvrml_write::write_vrml_header(os); }

#if 0
  void visit(dbrec3d_part_sptr part){
    dbrec3d_part_instance *pp = static_cast<dbrec3d_part_instance *>(part.as_pointer());
    visit(pp);
  }
#endif
  void write_box(vgl_box_3d<double> const& box) {
    //vgl_box_3d<int> box_int(box.min_x(),box.min_y(),box.min_z(),box.max_x(),box.max_y(),box.max_z());
    bvrml_write::write_vrml_wireframe_box(os_,box,1,0,0,0.9); }
  
  template <class T_instance>
  void write_context(dbrec3d_octree_context<T_instance>* context)
  {

    write_box(context->bounding_box());
    context->first();
    unsigned count = 0;
    while (!context->end()) {
      T_instance part = context->next();
      if(part.type_id() >=0){
        visit(part);
        count ++;
      }
    }
    vcl_cout << " number of vrml parts: " << count << vcl_endl;
  }

  
  template <class T_instance>
  void write_context(dbrec3d_octree_context<T_instance>* context, vgl_box_3d<double> roi)
  {
    
    write_box(roi);
    float N = context->part()->max_posterior();
    vcl_vector<T_instance> parts;
    context->query(roi, parts);
    typename vcl_vector<T_instance>::iterator it = parts.begin();
    unsigned count = 0;
    for(; it!=parts.end(); it++)
    {
        visit(*it, N);
        count++;
      
    }
    
    vcl_cout << " number of vrml parts: " << count << vcl_endl;
  }
  
  void visit(const dbrec3d_part_instance &part, float N=1.0f){
    vgl_point_3d<float> l(part.location().x(),part.location().y(),part.location().z());
    vgl_sphere_3d<float> sphere(l, radius_);
    float transparency  = 0.5 - part.posterior();
    if (transparency > 1.0) transparency = 1.0;
    bvrml_write::write_vrml_sphere(os_,sphere, 1, 1, 1, transparency);
  }

// void visit(dbrec3d_pair_composite_instance* part){
//   vgl_point_3d<float> l(part->location().x(),part->location().y(),part->location().z());
//   vgl_sphere_3d<float> sphere(l, radius_);
//   bvrml_write::write_vrml_sphere(os_,sphere);
//  }
  
  //Visit a composition instance, optionally indicate normalization constant
  void visit(const dbrec3d_pair_composite_instance &part, float N=1.0f){
    vgl_point_3d<double> l(part.location());
    //float transparency  =  1.0 - part.posterior()/N;
    float transparency = part.posterior();
    //if (transparency > 1.0) transparency = 1.0;
    vgl_line_segment_3d<double> line(part.location_child1(), part.location_child2());
    bvrml_write::write_vrml_line_segment(os_,line,  0, 0, 1, transparency);
  }
  
private:
  vcl_ofstream& os_;
  double radius_; //the radius associated with this visitor
};

#endif