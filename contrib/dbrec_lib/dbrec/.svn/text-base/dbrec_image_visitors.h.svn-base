//:
// \file
// \brief Concrete Visitor Classes to parse a 2D image using a given meta-structure as a hierarchy or 
//        implement other algorithms that work with 2D images and image related primitives
//
//
// \author Ozge C. Ozcanli (Brown)
// \date   May 01, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
//


#if !defined(_dbrec_visitors_image_h_)
#define _dbrec_visitors_image_h_

#include "dbrec_visitor.h"
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>
#include <vcl_set.h>

//: write the hierarchy in XML format to be able to save a current hierarchy and load back later
//  this visitor visits the nodes recursively and first writes the tree structure keeping track of parts, then makes a single pass on the parts to dump them individually
class dbrec_write_xml_visitor : public dbrec_visitor {
public:
  dbrec_write_xml_visitor();

  virtual void visit_composition(dbrec_composition* c);
  virtual void visit_gaussian_primitive(dbrec_gaussian* g);

  //: create the document and first write the structure and then the nodes
  void write_doc(vcl_string& name);

protected:
  
  bxml_data_sptr structural_data_;  // only keeps track of whom is whose children
  bxml_data_sptr part_data_;        // keeps one node per part

  vcl_set<unsigned> part_set_;
};

//: parse the hierarchy given in XML format
class dbrec_parse_hierarchy_xml {
public:
  dbrec_parse_hierarchy_xml() {}
  bool parse(const vcl_string& name);

  static dbrec_part_sptr parse_composition(bxml_data_sptr d);
  static dbrec_part_sptr parse_gaussian_primitive(bxml_data_sptr d);
  static bool parse_composition_structure(bxml_data_sptr d, vcl_map<unsigned, vcl_pair<dbrec_part_sptr, bool> >& part_map);

  dbrec_hierarchy_sptr get_hierarchy();

protected:
  
  bxml_data_sptr structural_data_;  // only keeps track of whom is whose children
  bxml_data_sptr node_data_;        // keeps one node per part

  vcl_map<unsigned, vcl_pair<dbrec_part_sptr, bool> > part_map_;
};


//: sample locations of each child in the composition wrt given location
class dbrec_sample_2d_location_visitor : public dbrec_compositor_visitor {
public:
  dbrec_sample_2d_location_visitor(const vgl_point_2d<float>& composition_loc, float visualization_scale, float space = 10.0f) : composition_loc_(composition_loc), space_(space), scale_(visualization_scale) {}

  virtual void visit_or_compositor(dbrec_or_compositor* c, const vcl_vector<dbrec_part_sptr>& children);
  virtual void visit_central_compositor(dbrec_central_compositor* g, const vcl_vector<dbrec_part_sptr>& children);

protected:
  vgl_point_2d<float> composition_loc_;
  float space_;
  float scale_;
  vcl_vector<vgl_point_2d<float> > children_locs_;
  
};

//: Simple concrete class to draw as 2D dbsvg documents
//  sample from possible compositions and draw the composing parts
class dbrec_draw_bsvg_visitor : public dbrec_visitor {
public:
  dbrec_draw_bsvg_visitor(float w, float h, float cx, float cy, float vrad, const vcl_string& c) : width_(w), height_(h), 
    current_loc_(cx, cy), vis_rad_(vrad), color_(c), doc_(w,h), scale_(1.0f) {}

  virtual void visit_composition(dbrec_composition* c);
  virtual void visit_gaussian_primitive(dbrec_gaussian* g);

  void write(const vcl_string& name);

protected:
  
  float width_;
  float height_;
  vgl_point_2d<float> current_loc_;
  float vis_rad_;
  vcl_string color_;
  bsvg_document doc_;
  float scale_;
};

void dbrec_draw_class(dbrec_hierarchy_sptr h, const vcl_string& out, unsigned class_id, float visualization_radius, const vcl_string& color);
void dbrec_draw_composition(dbrec_hierarchy_sptr h, const vcl_string& out, unsigned composition_type_id, float visualization_radius, const vcl_string& color);

class dbrec_get_direction_vector_visitor : public dbrec_visitor {
public:
  dbrec_get_direction_vector_visitor() : v_(0.0f, 1.0f) {}
  
  virtual void visit_composition(dbrec_composition* c);
  virtual void visit_gaussian_primitive(dbrec_gaussian* g);

  vnl_vector_fixed<float,2> v_;
};

class dbrec_mark_receptive_fields_visitor : public dbrec_visitor {
public: 
  dbrec_mark_receptive_fields_visitor(dbrec_part_context_sptr c, const vil_image_view<float>& center_map) : c_(c), center_map_(center_map), out_rec_field_map_(center_map.ni(), center_map.nj()) { out_rec_field_map_.fill(0.0f); }

  virtual void visit_composition(dbrec_composition* c);
  virtual void visit_gaussian_primitive(dbrec_gaussian* g);

  vil_image_view<float>& get_rec_field_map() { return out_rec_field_map_; }
  //: create an image with 3 planes, where the red channel is the original image, and receptive field is in blue channel so the centers are cyan color
  void get_colored_img(const vil_image_view<vxl_byte>& img, vil_image_view<vxl_byte>& out);

  //: a static method to create the colored image given the center map and the receptive field map
  static void get_colored_img(const vil_image_view<vxl_byte>& img, const vil_image_view<float>& center_map, 
    const vil_image_view<float>& rec_fields_map, vil_image_view<vxl_byte>& out);

protected:
  dbrec_part_context_sptr c_;  // the context of the highest part that the traversal will start should be passed
  vil_image_view<float> center_map_;
  vil_image_view<float> out_rec_field_map_;
};


class dbrec_mark_receptive_field_visitor : public dbrec_visitor {
public:
  dbrec_mark_receptive_field_visitor(dbrec_part_ins_sptr pi, float val, vil_image_view<float>& map) : pi_(pi), val_(val), map_(map) {}
  
  virtual void visit_composition(dbrec_composition* c);
  virtual void visit_gaussian_primitive(dbrec_gaussian* g);
protected:
  dbrec_part_ins_sptr pi_;
  float val_;
  vil_image_view<float> map_;
};

class dbrec_has_direction_vector_visitor : public dbrec_visitor {
public: 
  dbrec_has_direction_vector_visitor() : has_it_(false) {}
  virtual void visit_composition(dbrec_composition* c) { has_it_ = true; }
  virtual void visit_gaussian_primitive(dbrec_gaussian* g); 
  bool has_it_;
};

//: the symmetry of the composition is determined by the primitives that make it up
//  the primitives should have a well-defined angle.   angle = 360 means no rotational symmetry, angle = 0 means full rot symm, all angles in between are possible
class dbrec_get_symmetry_angle_visitor : public dbrec_visitor {
public:
  dbrec_get_symmetry_angle_visitor() : angle_(0) {}
  virtual void visit_composition(dbrec_composition* c);
  virtual void visit_gaussian_primitive(dbrec_gaussian* g);
  int angle_;  // in degrees
};

//: a specialized visitor to sample from rotationally invariant pairwise compositions and draw them at a given location with a given orientation
class dbrec_sample_and_draw_part_visitor : public dbrec_visitor {
public:
  dbrec_sample_and_draw_part_visitor(vil_image_view<vxl_byte>& image, int i, int j, int rot_angle, float strength, vnl_random& rng) : image_(image), i_(i), j_(j), rot_angle_(rot_angle), strength_(strength), rng_(rng) {}
  virtual void visit_composition(dbrec_composition* c);
  virtual void visit_gaussian_primitive(dbrec_gaussian* g);
protected:
  int i_;
  int j_;
  int rot_angle_; // in degrees
  float strength_;  // a value in range [0,1], scales the appearance based on this value
  vil_image_view<vxl_byte> image_;
  vnl_random rng_;
};

//: a specialized visitor to measure foreground probability of rotationally invariant pairwise compositions with a given orientation
class dbrec_measure_fg_prob_visitor : public dbrec_visitor {
public:
  dbrec_measure_fg_prob_visitor(vil_image_view<float>& fg_prob, dbrec_part_ins_sptr pi) : fg_prob_(fg_prob), pi_(pi) {}
  virtual void visit_composition(dbrec_composition* c);
  virtual void visit_gaussian_primitive(dbrec_gaussian* g);
  float prob_;
protected:
  dbrec_part_ins_sptr pi_;
  vil_image_view<float> fg_prob_;
};

const vcl_string file_fg_suffix = "_fg_params_dbrec.txt";
//const vcl_string file_fg_suffix = "_fg_params.txt";

//: helper to set the appearance models of gaussian primitives
class dbrec_parse_image_visitor_helper : public dbrec_visitor {
public:
  dbrec_parse_image_visitor_helper(const vcl_string& fg_appearance_model_path) : fg_app_model_path_(fg_appearance_model_path) {}
  virtual void visit_composition(dbrec_composition* c);
  virtual void visit_gaussian_primitive(dbrec_gaussian* g);
  
protected:
  vcl_string fg_app_model_path_;

};

class dbrec_parse_image_visitor : public dbrec_visitor {
public:
  //: assumes the input image is float and scaled to [0,1] range
  //: during training we just want to get strength maps, since no appearance model is available yet, thus the path is passed as "" during training
  dbrec_parse_image_visitor(dbrec_hierarchy_sptr h, vil_image_resource_sptr img, float class_prior, const vcl_vector<float>& comp_priors, const vcl_string& fg_appearance_model_path = "");

  //: if a context factory is available for the image for some parts of the hierarchy, use it to avoid re-extraction
  dbrec_parse_image_visitor(dbrec_hierarchy_sptr h, dbrec_context_factory_sptr cf, vil_image_resource_sptr img, float class_prior, const vcl_vector<float>& comp_priors, const vcl_string& fg_appearance_model_path = "");

  virtual void visit_composition(dbrec_composition* c);
  virtual void visit_gaussian_primitive(dbrec_gaussian* g);

  dbrec_context_factory_sptr context_factory() { return cf_; }

  static float marginal_value(const vcl_vector<dbrec_gaussian_appearance_model_sptr>& app_models, const vcl_vector<dbrec_prior_model_sptr>& priors, float res, unsigned i, unsigned j);

protected:
  void initialize(const vcl_string& model_path, float class_prior);
  
  dbrec_hierarchy_sptr h_;
  vil_image_resource_sptr img_;
  vcl_string fg_model_path_;
  
  vcl_vector<float> composition_priors_; // a prior for each case of compositions need to be passed. if size 0, then class prior saved in the composition will be used
                                         // if size 4, then 4 cases: class-fg, non-class-fg, class-bg, non-class-bg
                                         // if size 2, then 2 cases: class, non-class -> the compositor decides how to use the models according to the size of this vector
  
  dbrec_context_factory_sptr cf_;
  vcl_vector<dbrec_prior_model_sptr> priors_; // a prior for each appearance model in the primitives need to be passed, otherwise it won't parse
  
};

const vcl_string file_bg_mu_suffix = "_bg_mu_img_dbrec.tiff";
const vcl_string file_bg_sigma_suffix = "_bg_sigma_img_dbrec.tiff";
//const vcl_string file_bg_mu_suffix = "_bg_mu_img.tiff";
//const vcl_string file_bg_sigma_suffix = "_bg_sigma_img.tiff";

//: Concrete class to parse a 2D image using image based primitives like gaussian extrema
//  All the contexts that are encountered so far are saved in the context factory for reuse
//  assumes that all the compositions have a fixed prior probability for being class-foreground, class-background, non-class-foreground or non-class-background
//  assumes a fixed prior probability at each pixel for being on a class instance for all the primitives, the parameter class_prior is this prior probability, it is used in the posterior calculations of primitive parts
class dbrec_parse_image_with_fg_map_visitor : public dbrec_parse_image_visitor {
public:
  dbrec_parse_image_with_fg_map_visitor(dbrec_hierarchy_sptr h, vil_image_resource_sptr img, const vil_image_view<float>& fg_prob, 
    float class_prior, const vcl_vector<float>& comp_priors, const vcl_string& fg_model_path, const vcl_string& bg_model_path);

  //: if a context factory is available for the image for some parts of the hierarchy, use it to avoid re-extraction
  dbrec_parse_image_with_fg_map_visitor(dbrec_hierarchy_sptr h, dbrec_context_factory_sptr cf, vil_image_resource_sptr img, const vil_image_view<float>& fg_prob,
    float class_prior, const vcl_vector<float>& comp_priors, const vcl_string& fg_model_path, const vcl_string& bg_model_path);

  virtual void visit_composition(dbrec_composition* c);
  virtual void visit_gaussian_primitive(dbrec_gaussian* g);

protected:
  //void initialize_priors(const vil_image_view<float>& fg_prob, float class_prior);

  vil_image_view<float> fg_prob_;
  float class_prior_;
  vcl_string bg_model_path_;
  
};

class dbrec_train_compositional_parts_visitor : public dbrec_visitor {
public:
  //: depth is the depth of the compositional parts in the given hierarchy to be trained
  dbrec_train_compositional_parts_visitor(dbrec_hierarchy_sptr h, int depth, dbrec_context_factory_sptr cf) : h_(h), depth_(depth), cf_(cf) {}
  
  virtual void visit_composition(dbrec_composition* c);
  virtual void visit_gaussian_primitive(dbrec_gaussian* g);

protected:
  dbrec_hierarchy_sptr h_;
  int depth_;
  dbrec_context_factory_sptr cf_;
};

class dbrec_train_rot_inv_compositional_parts_visitor : public dbrec_visitor {
public:
  //: depth is the depth of the compositional parts in the given hierarchy to be trained
  dbrec_train_rot_inv_compositional_parts_visitor(dbrec_hierarchy_sptr h, int depth, dbrec_context_factory_sptr cf) : h_(h), depth_(depth), cf_(cf) {}
  
  virtual void visit_composition(dbrec_composition* c);
  virtual void visit_gaussian_primitive(dbrec_gaussian* g);

protected:
  dbrec_hierarchy_sptr h_;
  int depth_;
  dbrec_context_factory_sptr cf_;
};

//: the absolute angle of the gaussian extrema operator is meaningless when parsing rotationally invariant way
//  the absolute angle of each parsed instance of a primitive will be stored in the dbrec_rot_inv_part_ins
class dbrec_parse_image_rot_inv_visitor : public dbrec_parse_image_visitor {
public:
  //: assumes the input image is float and scaled to [0,1] range, a good value for theta_inc (increment) is 45 degrees
  dbrec_parse_image_rot_inv_visitor(dbrec_hierarchy_sptr h, vil_image_resource_sptr img, const vil_image_view<bool>& valid_region_mask, float class_prior, float theta_inc, const vcl_string& fg_appearance_model_path = "") 
    : dbrec_parse_image_visitor(h, img, class_prior, vcl_vector<float>(), fg_appearance_model_path), theta_inc_(theta_inc), valid_region_mask_(valid_region_mask) {}

  //: if a context factory is available for the image for some parts of the hierarchy, use it to avoid re-extraction, a good value for theta_inc (increment) is 45 degrees
  dbrec_parse_image_rot_inv_visitor(dbrec_hierarchy_sptr h, dbrec_context_factory_sptr cf, vil_image_resource_sptr img, const vil_image_view<bool>& valid_region_mask, float class_prior, float theta_inc, const vcl_string& fg_appearance_model_path = "")
    : dbrec_parse_image_visitor(h, cf, img, class_prior, vcl_vector<float>(), fg_appearance_model_path), theta_inc_(theta_inc), valid_region_mask_(valid_region_mask) {}

  virtual void visit_gaussian_primitive(dbrec_gaussian* g);
  virtual void visit_composition(dbrec_composition* c);

protected:  
  float theta_inc_; // the increments in the absolute orientations in the range [0, 180) for the primitives, the default is 45, e.g. theta_ = 0, 45, 90 and 135
  vil_image_view<bool> valid_region_mask_;
};

class dbrec_parse_image_rot_inv_with_fg_map_visitor : public dbrec_visitor {
public:
  dbrec_parse_image_rot_inv_with_fg_map_visitor(dbrec_hierarchy_sptr h, vil_image_resource_sptr img, const vil_image_view<bool>& valid_region_mask, const vil_image_view<float>& fg_prob, 
    float class_prior, float theta_inc, const vcl_string& fg_model_path, const vcl_string& bg_model_path);
  
  dbrec_parse_image_rot_inv_with_fg_map_visitor(dbrec_hierarchy_sptr h, dbrec_context_factory_sptr cf, vil_image_resource_sptr img, 
    const vil_image_view<bool>& valid_region_mask, const vil_image_view<float>& fg_prob,
    float class_prior, float theta_inc, const vcl_string& fg_model_path, const vcl_string& bg_model_path);

  virtual void visit_composition(dbrec_composition* c);
  virtual void visit_gaussian_primitive(dbrec_gaussian* g);
  dbrec_context_factory_sptr context_factory() { return cf_; }

protected:
  void initialize(const vcl_string& fg_model_path); 
  dbrec_gaussian_appearance_model_sptr get_map(vcl_map<float, dbrec_gaussian_appearance_model_sptr>& bg_model_map, float angle, dbrec_gaussian* g);

  dbrec_hierarchy_sptr h_;
  dbrec_context_factory_sptr cf_;
  vil_image_resource_sptr img_;
  vcl_string fg_model_path_;
  vcl_string bg_model_path_;

  vil_image_view<float> fg_prob_;  
  vil_image_view<bool> valid_region_mask_;

  float class_prior_;
  float theta_inc_;
};


#endif  //_dbrec_visitors_image_h_
