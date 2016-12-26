//:
// \file
// \brief Probability models to aid parsing 2D images using a given meta-structure as a hierarchy
//
//
// \author Ozge C. Ozcanli (Brown)
// \date   May 05, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
//


#if !defined(_dbrec_image_models_h_)
#define _dbrec_image_models_h_

#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>
#include <bsta/bsta_weibull.h>
#include <vil/vil_image_view.h>
#include <dbrec/dbrec_image_models_sptr.h>

//: possibly location variant appearance model, if the underlying model is location invariant, the location argument is simply ignored
class dbrec_gaussian_appearance_model : public vbl_ref_count {
public:
  virtual ~dbrec_gaussian_appearance_model() {}
  
  //: The probability density at this sample and at this location
  virtual float prob_density(float s, unsigned i, unsigned j) const = 0;

  //: the probability integrated over a box at this location
  virtual float probability(float min_s, float max_s, unsigned i, unsigned j) const { return 0.0f; }
};

//: the location "invariant" weibull appearance model
class dbrec_gaussian_weibull_appearance_model : public dbrec_gaussian_appearance_model {
public:
  dbrec_gaussian_weibull_appearance_model(float lambda, float k) : model_(lambda, k) {}
  virtual ~dbrec_gaussian_weibull_appearance_model() {}
  
  //: The probability density at this sample and at this location
  virtual float prob_density(float s, unsigned i, unsigned j) const { return model_.prob_density(s); }

  //: the probability integrated over a box at this location
  virtual float probability(float min_s, float max_s, unsigned i, unsigned j) const { return model_.probability(min_s, max_s); }

protected:
  bsta_weibull<float> model_;
};

//: the location "variant" background appearance model
class dbrec_gaussian_bg_appearance_model : public dbrec_gaussian_appearance_model {
public:
  dbrec_gaussian_bg_appearance_model(const vil_image_view<float>& mu_img, const vil_image_view<float>& sigma_img) : mu_img_(mu_img), sigma_img_(sigma_img) {}
  virtual ~dbrec_gaussian_bg_appearance_model() {}
  
  //: The probability density at this sample and at this location
  virtual float prob_density(float s, unsigned i, unsigned j) const;

protected:
  vil_image_view<float> mu_img_;
  vil_image_view<float> sigma_img_;
};

//: possibly location variant prior model, this prior models will be set and used by image parsers depending on the input image/parameters
class dbrec_prior_model : public vbl_ref_count {
public:
  virtual ~dbrec_prior_model() {}
  
  //: The prior at this location
  virtual float prior(unsigned i, unsigned j) const = 0;
};

//: location invariant constant prior model
class dbrec_constant_prior_model : public dbrec_prior_model {
public:
  dbrec_constant_prior_model(float constant) : constant_(constant) { assert(constant_ <= 1.0f); }
  virtual ~dbrec_constant_prior_model() {}
  
  //: The prior at this location
  virtual float prior(unsigned i, unsigned j) const { return constant_; }

protected:
  float constant_;
};

//: location variant prior model, e.g. when using a background map to interpret the strengths
class dbrec_loc_prior_model : public dbrec_prior_model {
public:
  dbrec_loc_prior_model(const vil_image_view<float>& p) : prior_img_(p) {} 
  virtual ~dbrec_loc_prior_model() {}
  
  //: The prior at this location
  virtual float prior(unsigned i, unsigned j) const;

protected:
  vil_image_view<float> prior_img_;
};

class dbrec_indep_prior_model : public dbrec_prior_model {
public:
  dbrec_indep_prior_model(const vcl_vector<dbrec_prior_model_sptr>& models) : models_(models) {}
  dbrec_indep_prior_model(dbrec_prior_model_sptr model1, dbrec_prior_model_sptr model2) { models_.push_back(model1); models_.push_back(model2); }

  //: The prior at this location
  virtual float prior(unsigned i, unsigned j) const;

protected:
  vcl_vector<dbrec_prior_model_sptr> models_;
};


#endif  //_dbrec_image_models_h_
