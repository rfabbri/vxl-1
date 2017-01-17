//:
// \file
// \brief classes to represent and generate gaussian extrema primitives
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


#if !defined(_dbrec_gaussian_primitive_h_)
#define _dbrec_gaussian_primitive_h_

#include <dbrec/dbrec_part_sptr.h>
#include <dbrec/dbrec_part.h>
#include <dbrec/dbrec_primitive_factory.h>
#include <vbl/vbl_array_2d.h>
#include <dbrec/dbrec_image_models_sptr.h>
#include <vil/vil_image_view.h>

class dbrec_visitor;
class dbrec_gaussian : public dbrec_part {

public:
  dbrec_gaussian(unsigned type, float lambda0, float lambda1, float theta, bool bright);

  virtual ~dbrec_gaussian() { models_.clear(); masks_.clear(); }

  dbrec_gaussian(const dbrec_gaussian& other) : dbrec_part(other) { 
    lambda0_ = other.lambda0_; 
    lambda1_ = other.lambda1_;
    theta_ = other.theta_;
    bright_ = other.bright_;
    mask_ = other.mask_;
    rj_ = other.rj_; ri_ = other.ri_;
    models_ = other.models_;
    masks_ = other.masks_;
  }

  virtual bool equal(const dbrec_gaussian& other) const;
  virtual vcl_ostream& print(vcl_ostream& out) const;
  void initialize_mask();

  virtual vcl_string string_identifier() const { 
    vcl_stringstream ss; ss << "gaussian_" << lambda0_ << "_" << lambda1_ << "_" << theta_;
    if (bright_) ss << "_bright"; 
    else ss << "_dark"; 
    return ss.str(); }

  virtual vcl_string string_identifier(float angle) const;

  virtual void visualize(bsvg_document& doc, float x, float y, float vis_rad, const vcl_string& color) const;

  //: each part needs to have a direction vector
  void get_direction_vector(vnl_vector_fixed<float,2>& v) const;

  //: a method to compute a direction vector for rotationally invariant parts given a particular absolute angle for its orientation
  void get_direction_vector(float abs_theta, vnl_vector_fixed<float, 2>& v) const;

  //: construct the bg mu and sigma images using mean and std_dev of the bg model. 
  //  we model the bg operator response as a gaussian at every pixel, so mu image contains the mean of the gaussian at every pixel, and sigma contains its std dev
  bool construct_bg_response_model_gauss(const vil_image_view<float>& mean_img, const vil_image_view<float>& std_dev_img, vil_image_view<float> &mu_img, vil_image_view<float> &sigma_img);

  //: find P(alpha in foreground): the probability that this operator alpha in foreground
  //  P(alpha in foreground) = argmax_x_kl P(x_kl in foreground) where x_kl is in mask of operator alpha
  float fg_prob_operator(const vil_image_view<float>& fg_prob_img, unsigned i, unsigned j);

  //: dilate the given fg_map and write to the new output
  void fg_map_dilate(const vil_image_view<float>& fg_prob_img, vil_image_view<float>& fg_prob_out);

  //: find P(alpha in foreground): the probability that this operator alpha in foreground
  //  P(alpha in foreground) = argmax_x_kl P(x_kl in foreground) where x_kl is in mask of operator alpha
  //  the mask is re-calculated based on the passed angle (angle is in degrees)
  float fg_prob_operator_rotational(const vil_image_view<float>& fg_prob_img, unsigned i, unsigned j, float angle);

  bool mask_operator(const vil_image_view<bool>& mask_img, unsigned i, unsigned j);
  //: dilate the given mask and write to the new output
  void mask_dilate(const vil_image_view<bool>& mask_img, vil_image_view<bool>& mask_out);


  //: allow for various visitors (encapsulated algos working with the meta-structure) visit the gaussian primitive
  virtual void accept(dbrec_visitor* v); 
  friend class dbrec_draw_bsvg_visitor;
  friend class dbrec_mark_receptive_field_visitor;
  friend class dbrec_parse_image_visitor;
  friend class dbrec_parse_image_with_fg_map_visitor;
  friend class dbrec_write_xml_visitor;
  friend class dbrec_sample_and_draw_part_visitor;
  friend class dbrec_has_direction_vector_visitor;
  friend class dbrec_get_symmetry_angle_visitor;
  friend class dbrec_parse_image_rot_inv_visitor;
  friend class dbrec_image_construct_bg_model_rot_inv_visitor;
  friend class dbrec_parse_image_rot_inv_with_fg_map_visitor;
  friend class dbrec_rot_inv_gaussian_weibull_model_learner_visitor;

  void set_models(const vcl_vector<dbrec_gaussian_appearance_model_sptr>& models) { models_ = models; }

  //: the default for extraction method is normal which is when fast_ = false, set fast_ to true using this method if you want fast extraction 
  void set_extraction_method_to_fast() { fast_ = true; }
  //: the default for extraction method is normal so this method doesn't need to be called
  void set_extraction_method_to_normal() { fast_ = false; }
  bool fast() { return fast_; }

  //: default is 0.01 so 1% of the tails of the gaussian is cut off during visualization
  void set_cutoff_percentage(float val) { cutoff_percentage_ = val; }

  //: mass of the gaussian primitives is the area of their mask
  float mass() const;

  //: retrieve a cached mask, if the mask for this angle is not in the cache then compute and cache it
  void get_mask(vbl_array_2d<bool>& mask, float theta);

protected:
  float lambda0_;
  float lambda1_;
  float theta_;
  bool bright_;
  bool fast_;

  //: parameter to define how big a receptive field will be marked, default is 0.01 so 1% of the tails of the gaussian is cut off
  float cutoff_percentage_;

  vbl_array_2d<bool> mask_;
  int rj_, ri_;

  //: there might be one or more models to interpret the strength of the operator, 
  //  these models should be learned during training and set before parsing
  vcl_vector<dbrec_gaussian_appearance_model_sptr> models_;  // e.g. if only two models: class non-class appearance models 
                                                           //      if four models: class-foreground, non-class-foreground, class-background, non-class background appearance models
                                                           //   CAUTION: the order of the models is important, the prior models vector of the image parser should also abide this ordering 

  vcl_map<float, vbl_array_2d<bool> > masks_; // cache the masks of angles encountered so far for future use
};


//: Concrete factory classes
class dbrec_gaussian_factory : public dbrec_primitive_factory {
public:
  //: add an instance with the given parameters to the factory
  //  primitives with the same masks are considered to be equivalent, makes sure primitives are not repeated, e.g. {lambda0, lambda1, 0, bright} == {lambda1, lambda0, 90, bright}
  void add(float lambda0, float lambda1, float theta, bool bright, bool fast);
  void add(dbrec_gaussian* g);

  //: create instances with the given number of directions, lambda range and with lambda increments
  void populate(int ndirs, float lambda_range, float lambda_inc, bool bright, bool fast);
  void populate(int ndirs, float lambda_min, float lambda_max, float lambda_inc, bool bright, bool fast);

  void populate_rot_inv(float lambda_min, float lambda_max, float lambda_inc, bool bright, bool fast);

};


#endif  //_dbrec_gaussian_primitive_h_
