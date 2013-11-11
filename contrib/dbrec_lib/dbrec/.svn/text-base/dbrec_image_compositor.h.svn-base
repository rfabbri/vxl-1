//:
// \file
// \brief class to encapsulate composition strategies for 2D images
//
//  Concrete compositor algorithms sub-class from the base 
//
// \author Ozge C. Ozcanli (Brown)
// \date   April 23, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
//


#if !defined(_dbrec_image_compositor_h_)
#define _dbrec_image_compositor_h_

#include "dbrec_part.h"
#include "dbrec_compositor.h"
#include "dbrec_compositor_sptr.h"
#include "dbrec_image_pairwise_models_sptr.h"
#include "dbrec_part_context.h"
#include "dbrec_type_id_factory.h"


#include <vbl/vbl_ref_count.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_point_2d.h>
#include <vnl/vnl_math.h>
#include <bsta/bsta_gaussian_sphere.h>
#include <bsta/bsta_joint_histogram.h>
#include <bxml/bsvg/bsvg_document.h>
#include <vil/vil_image_view.h>

//: Concrete class for the "OR" composition, e.g. at the root of a class
class dbrec_image_compositor : public dbrec_compositor {
public:
  dbrec_image_compositor() {}

  //: sample locations of each child in the composition wrt given location
  virtual void sample_locations(const vgl_point_2d<float>& loc, float scale, const vcl_vector<dbrec_part_sptr>& children, vcl_vector<vgl_point_2d<float> >& locs) const = 0;
  
  //: for the central_compositor, the direction vector is the direction vector of the central part, the first child
  virtual void direction_vector(const vcl_vector<dbrec_part_sptr>& children, vnl_vector_fixed<float,2>& v) const = 0;

  //: draw additional attributes on the composing parts, OR compositor does nothing
  virtual void draw_attributes(bsvg_document& doc, const vcl_vector<dbrec_part_sptr>& children, const vcl_vector<vgl_point_2d<float> >& locs) const = 0;

  //: draw additional attributes on the composing parts, OR compositor does nothing
  virtual void visualize(vcl_string& doc_name, const vcl_vector<dbrec_part_sptr>& children) const = 0;

  //: method to detect instances of a composition
  virtual dbrec_part_context_sptr detect_instances(unsigned type_id, dbrec_context_factory_sptr cf, const vcl_vector<dbrec_part_sptr>& children, const vcl_vector<float>& comp_priors, float radius) const = 0;

  //: method to train a compositional part's geometric models
  virtual bool train_instances(dbrec_context_factory_sptr cf, const vcl_vector<dbrec_part_sptr>& children, float radius) = 0;

  //: return self as a bxml_data_sptr
  virtual bxml_data_sptr xml_element() const;
  //: read self from the bxml_data_sptr
  static dbrec_compositor_sptr xml_parse_element(bxml_data_sptr data);
};

//: Concrete class for the "OR" composition, e.g. at the root of a class
class dbrec_or_compositor : public dbrec_image_compositor {
public:
  dbrec_or_compositor() {}

  //: sample locations of each child in the composition wrt given location
  virtual void sample_locations(const vgl_point_2d<float>& loc, float scale, const vcl_vector<dbrec_part_sptr>& children, vcl_vector<vgl_point_2d<float> >& locs) const;
  
  //: for the central_compositor, the direction vector is the direction vector of the central part, the first child
  virtual void direction_vector(const vcl_vector<dbrec_part_sptr>& children, vnl_vector_fixed<float,2>& v) const;

  //: draw additional attributes on the composing parts, OR compositor does nothing
  virtual void draw_attributes(bsvg_document& doc, const vcl_vector<dbrec_part_sptr>& children, const vcl_vector<vgl_point_2d<float> >& locs) const {}

  //: draw additional attributes on the composing parts, OR compositor does nothing
  virtual void visualize(vcl_string& doc_name, const vcl_vector<dbrec_part_sptr>& children) const {};

  //: method to detect instances of a composition
  virtual dbrec_part_context_sptr detect_instances(unsigned type_id, dbrec_context_factory_sptr cf, const vcl_vector<dbrec_part_sptr>& children, const vcl_vector<float>& comp_priors, float radius) const;

  //: method to train a compositional part's geometric models
  //  an OR node shall never be trained, cause it has no models
  virtual bool train_instances(dbrec_context_factory_sptr cf, const vcl_vector<dbrec_part_sptr>& children, float radius) { return true; }

  //: return self as a bxml_data_sptr
  virtual bxml_data_sptr xml_element() const;
  //: read self from the bxml_data_sptr
  static dbrec_compositor_sptr xml_parse_element(bxml_data_sptr data);
};

//: Concrete class for composition of a bunch of parts using one of them as the central part, uses pairwise compositors

//: each child has a pairwise model with the "central" part which is assumed to be the "first" child of a composition
class dbrec_central_compositor : public dbrec_image_compositor {
public:
  dbrec_central_compositor(const vcl_vector<dbrec_pairwise_model_sptr>& class_models) : models_(class_models) {}

  //: sample locations of each child in the composition wrt given location
  virtual void sample_locations(const vgl_point_2d<float>& loc, float scale, const vcl_vector<dbrec_part_sptr>& children, vcl_vector<vgl_point_2d<float> >& locs) const;
  
  //: for the central_compositor, the direction vector is the direction vector of the central part, the first child
  virtual void direction_vector(const vcl_vector<dbrec_part_sptr>& children, vnl_vector_fixed<float,2>& v) const;

  //: draw additional attributes on the composing parts, central compositor marks the central part by displaying its direction vector, and draws lines connecting center to other parts
  virtual void draw_attributes(bsvg_document& doc, const vcl_vector<dbrec_part_sptr>& children, const vcl_vector<vgl_point_2d<float> >& locs) const;

  //: draw additional attributes on the composing parts, OR compositor does nothing
  virtual void visualize(vcl_string& doc_name, const vcl_vector<dbrec_part_sptr>& children) const;
  
  //: method to detect instances of a composition
  virtual dbrec_part_context_sptr detect_instances(unsigned type_id, dbrec_context_factory_sptr cf, const vcl_vector<dbrec_part_sptr>& children, const vcl_vector<float>& comp_priors, float radius) const;

  //: method to train a compositional part's geometric models
  virtual bool train_instances(dbrec_context_factory_sptr cf, const vcl_vector<dbrec_part_sptr>& children, float radius);

  vcl_vector<dbrec_pairwise_model_sptr>& models() { return models_; }

  //: return self as a bxml_data_sptr
  virtual bxml_data_sptr xml_element() const;
  //: read self from the bxml_data_sptr
  static dbrec_compositor_sptr xml_parse_element(bxml_data_sptr data);

protected:
  //: each composition has a class model, and possibly a prior for the class model, for each pair, there may be additional models, e.g. class-foreground, class-background etc.
  vcl_vector<dbrec_pairwise_model_sptr> models_;
};

//: a compositor that always works with two sub-parts and handles their composition in a rotation invariant way
class dbrec_pairwise_compositor : public dbrec_image_compositor {
public:
  dbrec_pairwise_compositor(dbrec_pairwise_rot_invariant_model_sptr model) : model_(model) {}

  //: sample locations of each child in the composition wrt given location
  virtual void sample_locations(const vgl_point_2d<float>& loc, float scale, const vcl_vector<dbrec_part_sptr>& children, vcl_vector<vgl_point_2d<float> >& locs) const;
  
  virtual void direction_vector(const vcl_vector<dbrec_part_sptr>& children, vnl_vector_fixed<float,2>& v) const  { throw 0; }

  //: draw additional attributes on the composing parts
  virtual void draw_attributes(bsvg_document& doc, const vcl_vector<dbrec_part_sptr>& children, const vcl_vector<vgl_point_2d<float> >& locs) const  { throw 0; }

  virtual void visualize(vcl_string& doc_name, const vcl_vector<dbrec_part_sptr>& children) const  { throw 0; }
  
  //: method to detect instances of a composition
  virtual dbrec_part_context_sptr detect_instances(unsigned type_id, dbrec_context_factory_sptr cf, const vcl_vector<dbrec_part_sptr>& children, const vcl_vector<float>& comp_priors, float radius) const { throw 0; }

  //: method to detect instances of a composition in a rotationally invariant way for the 2 case problem: class and non-class
  dbrec_part_context_sptr detect_instances_rot_inv(unsigned type_id, dbrec_context_factory_sptr cf, const vcl_vector<dbrec_part_sptr>& children, float comp_class_prior, float radius);

  //: method to detect instances of a composition in a rotationally invariant way for the 4 case problem: class-foreground, non-class foreground, class background, non-class background
  dbrec_part_context_sptr detect_instances_rot_inv(unsigned type_id, dbrec_context_factory_sptr cf, const vcl_vector<dbrec_part_sptr>& children, float comp_class_prior, vil_image_view<float>& fg_prob, float radius);

  //: method to train a compositional part's geometric models
  virtual bool train_instances(dbrec_context_factory_sptr cf, const vcl_vector<dbrec_part_sptr>& children, float radius)  { throw 0; }

  static void collect_rot_inv_stats(dbrec_context_factory_sptr cf, int posterior_id, dbrec_part_sptr p1, dbrec_part_sptr p2, float radius, const vcl_string& output_file_prefix);

  //: return self as a bxml_data_sptr
  virtual bxml_data_sptr xml_element() const;
  //: read self from the bxml_data_sptr
  static dbrec_compositor_sptr xml_parse_element(bxml_data_sptr data);

  //: data members
  dbrec_pairwise_rot_invariant_model_sptr model_;
};

//: a compositor that always works with two sub-parts and handles their composition in a rotation invariant way, uses a model with a discrete histogram to model d and rho distribution of the part
class dbrec_pairwise_discrete_compositor : public dbrec_image_compositor {
public:
  dbrec_pairwise_discrete_compositor(dbrec_pairwise_rot_invariant_discrete_model_sptr model) : model_(model) {}

  //: sample locations of each child in the composition wrt given location
  virtual void sample_locations(const vgl_point_2d<float>& loc, float scale, const vcl_vector<dbrec_part_sptr>& children, vcl_vector<vgl_point_2d<float> >& locs) const { throw 0; }
  virtual void direction_vector(const vcl_vector<dbrec_part_sptr>& children, vnl_vector_fixed<float,2>& v) const  { throw 0; }
  //: draw additional attributes on the composing parts
  virtual void draw_attributes(bsvg_document& doc, const vcl_vector<dbrec_part_sptr>& children, const vcl_vector<vgl_point_2d<float> >& locs) const  { throw 0; }
  virtual void visualize(vcl_string& doc_name, const vcl_vector<dbrec_part_sptr>& children) const;
  //: method to detect instances of a composition
  virtual dbrec_part_context_sptr detect_instances(unsigned type_id, dbrec_context_factory_sptr cf, const vcl_vector<dbrec_part_sptr>& children, const vcl_vector<float>& comp_priors, float radius) const { throw 0; }

  //: method to detect instances of a composition in a rotationally invariant way for the 2 case problem: class and non-class
  dbrec_part_context_sptr detect_instances_rot_inv(unsigned type_id, dbrec_context_factory_sptr cf, const vcl_vector<dbrec_part_sptr>& children, float comp_class_prior, float radius);

  //: method to detect instances of a composition in a rotationally invariant way for the 4 case problem: class-foreground, non-class foreground, class background, non-class background
  dbrec_part_context_sptr detect_instances_rot_inv(unsigned type_id, dbrec_context_factory_sptr cf, const vcl_vector<dbrec_part_sptr>& children, float comp_class_prior, vil_image_view<float>& fg_prob, float radius);

  //: method to train a compositional part's geometric models
  virtual bool train_instances(dbrec_context_factory_sptr cf, const vcl_vector<dbrec_part_sptr>& children, float radius);

  //: return self as a bxml_data_sptr
  virtual bxml_data_sptr xml_element() const;
  //: read self from the bxml_data_sptr
  static dbrec_compositor_sptr xml_parse_element(bxml_data_sptr data);

  //: data members
  dbrec_pairwise_rot_invariant_discrete_model_sptr model_;
};


//: a templated method to generate initial pool of compositional parts from the parts in a given depth of an initial hierarchy
//  create one pool and initiate a root for each class OR'in this pool for each
//  WARNING: looses the information that parts at this depth were OR'ed in a particular way in the original hierarchy
//  this is alright because we consider a pool of parts for each class anyways during selection, and if the prims had higher mutual infos, their good combinations should also have higher mutual info
//  radius usually depends on depth, e.g. set as depth*prim_radius, but it could also be customized depending on depth
template <class F>
dbrec_hierarchy_sptr populate_depth_central_compositor(dbrec_hierarchy_sptr orig_hierarchy, int depth, float radius, const F& model_factory)
{
  //: use an id factory and register the parts to create unique ids for the new parts
  dbrec_type_id_factory* ins = dbrec_type_id_factory::instance();
  orig_hierarchy->register_parts(ins);
  
  vcl_vector<dbrec_part_sptr> parts;
  orig_hierarchy->get_parts(depth, parts);
  vcl_vector<dbrec_part_sptr> part_pool;
  //: create all pairs nxn (the central part makes a difference)
  for (unsigned i = 0; i < parts.size(); i++) {
    for (unsigned j = 0; j < parts.size(); j++) {
      vcl_vector<dbrec_part_sptr> pp; pp.push_back(parts[i]); pp.push_back(parts[j]);
      dbrec_pairwise_model_sptr m = model_factory.generate_model(); // one model per pair
      vcl_vector<dbrec_pairwise_model_sptr> models; models.push_back(m);
      dbrec_compositor_sptr cc = new dbrec_central_compositor(models);
      dbrec_part_sptr composition = new dbrec_composition(ins->new_type(), pp, cc, radius);
      part_pool.push_back(composition);
    }
  }
  dbrec_hierarchy_sptr new_h = new dbrec_hierarchy();
  for (unsigned c = 0; c < orig_hierarchy->class_cnt(); c++) {
    dbrec_compositor_sptr oc = new dbrec_or_compositor();
    dbrec_part_sptr class_composition = new dbrec_composition(ins->new_type(), part_pool, oc, radius); // the OR node is higher in the hierarchy but has the same radius
    new_h->add_root(class_composition);
  }
  return new_h;
}

//: a templated method to generate initial pool of compositional parts from the parts right below each class node (root) in the hierarchy
template <class F>
dbrec_hierarchy_sptr populate_class_based_central_compositor(dbrec_hierarchy_sptr orig_hierarchy, float radius, const F& model_factory)
{
  //: use an id factory and register the parts to create unique ids for the new parts
  dbrec_type_id_factory* ins = dbrec_type_id_factory::instance();
  orig_hierarchy->register_parts(ins);

  dbrec_hierarchy_sptr new_h = new dbrec_hierarchy();
  for (unsigned c = 0; c < orig_hierarchy->class_cnt(); c++) {
    dbrec_part_sptr c_node = orig_hierarchy->root(c);
    dbrec_composition* c_node_comp = dynamic_cast<dbrec_composition*>(c_node.ptr());
    vcl_vector<dbrec_part_sptr> parts = c_node_comp->children();  // this is an OR composition, use all of its parts to generate new compositional nodes
    vcl_vector<dbrec_part_sptr> part_pool;  
    for (unsigned i = 0; i < parts.size(); i++) {
      for (unsigned j = i; j < parts.size(); j++) {
        vcl_vector<dbrec_part_sptr> pp; pp.push_back(parts[i]); pp.push_back(parts[j]);
        dbrec_pairwise_model_sptr m = model_factory.generate_model(); // one model per pair
        vcl_vector<dbrec_pairwise_model_sptr> models; models.push_back(m);
        dbrec_compositor_sptr cc = new dbrec_central_compositor(models);
        dbrec_part_sptr composition = new dbrec_composition(ins->new_type(), pp, cc, radius);
        part_pool.push_back(composition);
      }
    }
    dbrec_compositor_sptr oc = new dbrec_or_compositor();
    dbrec_part_sptr class_composition = new dbrec_composition(ins->new_type(), part_pool, oc, radius); // the OR node is higher in the hierarchy but has the same radius
    new_h->add_root(class_composition);
  }

  return new_h;
}

// generate an initial pool of compositional parts from the parts right below each class node (root) in the hieararchy
//dbrec_hierarchy_sptr populate_class_based_pairwise_discrete_compositor(dbrec_hierarchy_sptr orig_hierarchy, int radius, int gamma_interval, int d_interval, int rho_interval);
dbrec_hierarchy_sptr populate_class_based_pairwise_discrete_compositor(dbrec_hierarchy_sptr orig_hierarchy, int radius, float angle_increment, int d_interval, int rho_interval);

//: a method to generate a new pool of compositional parts from a given pool, the given pool needs to have been trained
// for each part in the existing pool add n new parts
dbrec_hierarchy_sptr populate_class_based_central_compositor_by_sampling(dbrec_hierarchy_sptr orig_hierarchy, unsigned n);
//: a method to generate a new pool of compositional parts from a given pool, the given pool needs to have been trained
// for each part in the existing pool add n new parts with indep gaussian as model
dbrec_hierarchy_sptr populate_class_based_central_compositor_by_indep_gaussian_model(dbrec_hierarchy_sptr orig_hierarchy, unsigned n);


#endif  //_dbrec_compositor_h_
