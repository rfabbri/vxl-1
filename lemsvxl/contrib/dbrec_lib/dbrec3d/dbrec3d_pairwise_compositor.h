// This is dbrec3d_pairwise_compositor.h
#ifndef dbrec3d_pairwise_compositor_h
#define dbrec3d_pairwise_compositor_h

//:
// \file
// \brief A class that implements the dbrec3d_part concept for composite parts (i.e non-leafs). 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  24-Jun-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "dbrec3d_part.h"
#include "dbrec3d_octree_context.h"
#include "dbrec3d_part_instance.h"
#include "dbrec3d_composite_part.h"
#include "dbrec3d_parts_manager.h"
#include "dbrec3d_context_manager.h"


#include <boxm/boxm_scene.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vul/vul_timer.H>

#include <bxml/bxml_document.h>

template < class T_model>
class dbrec3d_pairwise_compositor
{
public:
  
  typedef T_model model_type;
  
public:
  //: Default constructor. Initialize constant member variables
  dbrec3d_pairwise_compositor(): canonical_rotation_axis_(vnl_float_3(1,0,0)), canonical_auxiliary_axis_(vnl_float_3(0,1,0)){}
  
  //: Constructor from model. 
  dbrec3d_pairwise_compositor(T_model model): canonical_rotation_axis_(vnl_float_3(1,0,0)), canonical_auxiliary_axis_(vnl_float_3(0,1,0)), model_(model){}
  
  //: method to detect instances of a composition, within a region; the search box should be in coordinates relative to the center part?
  template < class T_instance>
  int detect_instances(int composition_id, dbrec3d_part_sptr part1, dbrec3d_part_sptr part2,
                                        boxm_scene<boct_tree<short, dbrec3d_pair_composite_instance > > *scene_out, vgl_box_3d<double> search_box);

  dbrec3d_pair_composite_instance compose(unsigned type_id, dbrec3d_part_instance pi,  dbrec3d_part_instance pi2);
  
  vgl_rotation_3d<float> compute_rotation( vnl_float_3 axis1, vnl_float_3 aux_axis1, vnl_float_3 axis2, vnl_float_3 aux_axis2,vnl_float_3 &new_aux_axis);
  
  T_model model() { return model_; }
  void set_model(T_model model) { model_ = model; }
  //: XML write
  bxml_data_sptr xml_element() const;

  //: XML read
  dbrec3d_pairwise_compositor<T_model>* parse_xml_element(bxml_data_sptr d) const;

  dbrec3d_pairwise_compositor<T_model>& operator=(dbrec3d_pairwise_compositor<T_model> const& r) 
  { this->model_ = r.model_; return *this;}
  
protected:
 
  //: The canonical position for rotation axis
  const vnl_float_3 canonical_rotation_axis_;//(1,0,0);
  
  
   // :Define a coordinate system for part instance.
  const vnl_float_3 canonical_auxiliary_axis_;//(0,1,0);
  
  //: Model for this particular pairwise-composition
  T_model model_;

  
};


//: method to detect instances of a compositions. Returns the id of composition context
template <class T_model> template <class T_instance>
int dbrec3d_pairwise_compositor<T_model>::detect_instances(int composition_id, dbrec3d_part_sptr part1, dbrec3d_part_sptr part2,
                                                           boxm_scene<boct_tree<short, dbrec3d_pair_composite_instance > > *scene_out, 
                                                           vgl_box_3d<double> search_box)
{
#ifdef DEBUG
  vcl_cout << " Composition Scene has:\n"
  << "Finest level: " <<scene_out->finest_level()
  << "Max level: " << scene_out->max_level()
  << "Init Level: " << scene_out->init_level() << vcl_endl;
#endif
  vul_timer t;
  t.mark();
 
  if (!part1 || !part2) 
    return -1;
  
  dbrec3d_octree_context<T_instance> *part1_c = 
  dynamic_cast<dbrec3d_octree_context<T_instance>* >(CONTEXT_MANAGER->get_context(part1->type_id()).as_pointer());
  dbrec3d_octree_context<T_instance> *part2_c = 
  dynamic_cast<dbrec3d_octree_context<T_instance>* >(CONTEXT_MANAGER->get_context(part2->type_id()).as_pointer());
  
  if (!part1_c || !part2_c)
    return -1;
  
    
  double cell_length = part1->min_cell_length();
  
  //register and output context
  int out_c_id = CONTEXT_MANAGER->register_empty_context(scene_out, composition_id);
  dbrec3d_octree_context<dbrec3d_pair_composite_instance> *out_c = dynamic_cast<dbrec3d_octree_context<dbrec3d_pair_composite_instance>* > ((CONTEXT_MANAGER->get_context(out_c_id)).as_pointer());
  if(!out_c)
    return -1;
  
  //: now go through each instance of the first part
  dbrec3d_part_instance pi = part1_c->first();
 
  vgl_point_3d<double> search_box_min =  search_box.min_point();
  vgl_point_3d<double> search_box_max =  search_box.max_point();
  
  while (!part1_c->end()) {   
    
    if(pi.type_id() < 0){
      pi = part1_c->next();
      continue;
    }
    
    vgl_point_3d<double> box_origin = pi.location();
    
    //change the coordinates of the box to be in global coordinates and shift the box around the current part
    vgl_point_3d<double> min_point(box_origin.x() + search_box_min.x()*cell_length, box_origin.y() + search_box_min.y()*cell_length, box_origin.z() + search_box_min.z()*cell_length);
    vgl_point_3d<double> max_point(box_origin.x() + search_box_max.x()*cell_length, box_origin.y() + search_box_max.y()*cell_length, box_origin.z() + search_box_max.z()*cell_length);

    // query the parts within search box
    vgl_box_3d<double> global_box(min_point,max_point);
    vcl_vector<T_instance > parts_c2;
    part2_c->query(global_box, parts_c2);
    //vcl_cout << "\t found " << out.size() << " part2 instances around it!\n";
    
    float best_posterior = 0.0f;
    int best_part_index = -1;
    vcl_vector<dbrec3d_pair_composite_instance > compositions;
    
    // form new instances
    unsigned temp_idx=0;
    for (unsigned i = 0; i< parts_c2.size(); i++) {
      
      dbrec3d_part_instance pi2 = parts_c2[i]; //here ozge chooses the best part
      if(pi2.type_id()<0)
        continue;
      dbrec3d_pair_composite_instance ci = compose(composition_id, pi, pi2);
      
      compositions.push_back(ci);
      if (ci.posterior() > best_posterior ) {
        best_posterior = ci.posterior();
        best_part_index = (int)temp_idx;
      }
      temp_idx ++ ;
      
    }
    if(best_part_index >=0)
      out_c->add_part_instance(compositions[best_part_index]);
    
    pi = part1_c->next();
  }
  vcl_cout << "Time to detect pairwise compositions: " << t.all() << vcl_endl;
  out_c->save_to_disk();
  return out_c_id;
}

template <class T_model>
dbrec3d_pair_composite_instance dbrec3d_pairwise_compositor<T_model>::compose(unsigned type_id, dbrec3d_part_instance pi,  dbrec3d_part_instance pi2)
{
 
  // create new composition instance 
  float posterior = 1.0f;
  
  dbrec3d_pair_composite_instance ci(type_id, posterior, pi.location(), pi2.location());
  
  // compute posterior probability
  posterior =model_.prob_density(ci);
  ci.set_posterior(posterior);
  
  return ci;
  
}


             
//: Compute rotation axis of the composition part. Axis is given by the axis of second part rotated, such that axis of firts part is in the cannonical position
 template<class T_model>
 vgl_rotation_3d<float>  dbrec3d_pairwise_compositor<T_model>::compute_rotation( vnl_float_3 axis1, vnl_float_3 aux_axis1, 
                                                                                 vnl_float_3 axis2, vnl_float_3 aux_axis2, 
                                                                                 vnl_float_3 &new_aux_axis)
{
  
  // rotation axis should be unit vectors - does magnitude() compute the val everytime? if so, store
  if (axis1.magnitude() > 1e-7)
    axis1 /= axis1.magnitude();
  if (axis2.magnitude() > 1e-7)
    axis2 /= axis2.magnitude();
  if (aux_axis1.magnitude() > 1e-7)
    aux_axis1 /= aux_axis1.magnitude() ;
  if (aux_axis2.magnitude() > 1e-7)
    aux_axis2 /= aux_axis2.magnitude();
  
  
  //construct a rotation to align pi.rotation_axis to the canonical position
  vgl_rotation_3d<float> r_align(axis1, canonical_rotation_axis_ );
  
  //take care of the auxiliary axis
  vgl_rotation_3d<float> align_aux_axes(r_align*aux_axis1, canonical_auxiliary_axis_);

  r_align = align_aux_axes*r_align;
  
  vcl_cout << r_align << vcl_endl;
  
#if 0 //this is noy checked in vgl_rotation_3d and it may be safe to remove
  //sanity check - the rotation of the frist instance should now be identical to the cannonical rotation angle
  if((vnl_cross_3d((r_align.as_matrix() * axis1), canonical_rotation_axis_).two_norm() <1e-2) && 
     (vnl_cross_3d((r_align.as_matrix() * aux_axis1), canonical_auxiliary_axis_).two_norm() <1e-2))
  {
     vcl_cout << "Error computing rotation" << vcl_endl;
    return vgl_rotation_3d<float>(); 
  }
#endif
  
  //find rotation angle between difference between auxiliar axis
    
  //rotate pi2 axis
 vnl_float_3 new_axis2 = r_align.as_matrix() * axis2; 
  
  //construct a rotation to align p2.rotation_axis to the canonical position
  vgl_rotation_3d<float> r_align2(new_axis2, canonical_rotation_axis_ );
  
  //take care of the auxiliary axis
  vgl_rotation_3d<float> align_aux_axes2(aux_axis2, canonical_auxiliary_axis_);
  
  r_align = align_aux_axes2*r_align2;
  
  new_aux_axis = vgl_rotation_3d<float>(canonical_rotation_axis_, r_align.axis()) * canonical_auxiliary_axis_;
  
  new_aux_axis = vgl_rotation_3d<float>(vnl_quaternion<float>(r_align.axis(), r_align.angle())) * new_aux_axis;
  
  return r_align;

}

//: XML write
template<class T_model>
bxml_data_sptr dbrec3d_pairwise_compositor<T_model>::xml_element() const
{
  bxml_element* data = new bxml_element("dbrec3d_pairwise_compositor");
  data->append_text("\n");
  bxml_data_sptr model_data= model_.xml_element();
  data->append_data(model_data);
  data->append_text("\n");
  return data;
}
 
//: XML read
template<class T_model>
dbrec3d_pairwise_compositor<T_model>*
dbrec3d_pairwise_compositor<T_model>::parse_xml_element(bxml_data_sptr d) const
{
  bxml_element query("dbrec3d_pairwise_compositor");
  bxml_data_sptr root = bxml_find_by_name(d, query);
  if (!root || root->type() != bxml_data::ELEMENT) {
    return NULL;
  }
  bxml_element* gp_root = dynamic_cast<bxml_element*>(root.ptr());
  
  //get the variables
  //int type_id = -1; float class_prior= 0.0f; 
  //gp_root->get_attribute("type_id", type_id );
  //gp_root->get_attribute("class_prior", class_prior);
  T_model model;
  model.parse_xml_element(gp_root);
  
  //create the part
  dbrec3d_pairwise_compositor<T_model>* comp = new dbrec3d_pairwise_compositor<T_model>(model);
  return comp;
}           
         
         
      
         
#endif
