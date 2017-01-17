//:
// \file
// \brief classes to collect stats for parts
//
// \author Ozge C. Ozcanli (Brown)
// \date   April 30, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
//


#if !defined(_dbrec_image_learner_visitors_h_)
#define _dbrec_image_learner_visitors_h_

#include "dbrec_part_context_sptr.h"
#include "dbrec_part_sptr.h"
#include "dbrec_image_learner_visitors_sptr.h"
#include "dbrec_visitor.h"
#include <vbl/vbl_ref_count.h>
#include <vil/vil_image_view.h>
#include <bsta/bsta_histogram.h>

class dbrec_gaussian_weibull_model_learner : public vbl_ref_count {
public:
  dbrec_gaussian_weibull_model_learner() : x_sum_(0.0f), xsq_sum_(0.0f), p_sum_(0.0f), h_(-7.0f, 1.0f, 32) {}

  void update(float op_response, float prob) 
  { 
    x_sum_ += prob*op_response; 
    xsq_sum_ += prob*op_response*op_response; 
    p_sum_ += prob; 
    h_.upcount(vcl_log10(op_response), prob); 
  }

  //: fits a weibull model to the current parameters
  dbrec_gaussian_appearance_model_sptr fit_weibull(double& k, double& lambda); 

  //: creates a histogram from the model and writes as an svg doc
  static void visualize_model(dbrec_gaussian_appearance_model_sptr m, const vcl_string& name, float width = 600.0f, float height = 600.0f, float margin = 40.0f, int font_size = 30);
  //: writes the histogram as an svg doc
  void visualize_hist(const vcl_string& name, float width = 600.0f, float height = 600.0f, float margin = 40.0f, int font_size = 30);

protected:
  double x_sum_;
  double xsq_sum_;
  double p_sum_;

  bsta_histogram<float> h_;
};

//: extracts stats from a context factory for the gaussian primitive parts of the hierarchy
class dbrec_gaussian_weibull_model_learner_visitor : public dbrec_visitor {

protected:
  typedef vcl_map<unsigned, vcl_pair<dbrec_gaussian_weibull_model_learner_sptr, dbrec_gaussian_weibull_model_learner_sptr> > map_type;
public:
  
  dbrec_gaussian_weibull_model_learner_visitor(dbrec_hierarchy_sptr h) : h_(h), data_set_(false) {}

  virtual void visit_composition(dbrec_composition* c);
  virtual void visit_gaussian_primitive(dbrec_gaussian* g);

  void set_data(dbrec_context_factory_sptr cf, const vil_image_view<float>& prob_img, const vil_image_view<bool>& mask_img) 
  { cf_ = cf;
    prob_img_ = prob_img; 
    mask_img_ = mask_img; 
    data_set_ = true; }

  dbrec_hierarchy_sptr h() { return h_; }
  void print_current_histograms(const vcl_string& prefix);
  void print_current_models(const vcl_string& prefix);

protected:
  
  dbrec_hierarchy_sptr h_;
  bool data_set_;

  map_type learners_;
  dbrec_context_factory_sptr cf_;
  vil_image_view<float> prob_img_;
  vil_image_view<bool> mask_img_;
  
};

class dbrec_rot_inv_gaussian_weibull_model_learner_visitor : public dbrec_gaussian_weibull_model_learner_visitor {
public:
  dbrec_rot_inv_gaussian_weibull_model_learner_visitor(dbrec_hierarchy_sptr h) : dbrec_gaussian_weibull_model_learner_visitor(h) {}
  virtual void visit_gaussian_primitive(dbrec_gaussian* g);
  //: same as dbrec_gaussian_weibull_model_learner_visitor::print_current_models(), but it prints the model parameters for each orientation
  void print_current_models_at_each_orientation(const vcl_string& prefix, float angle_inc);
};

//: finds mu and sigma images for each primitive part and writes them to model folder, does nothing for the compositions, just passes the composition to children
class dbrec_image_construct_bg_model_visitor : public dbrec_visitor {
public:
  dbrec_image_construct_bg_model_visitor(const vil_image_view<float>& mean_img, 
    const vil_image_view<float>& std_dev_img, const vcl_string& model_folder) : mean_img_(mean_img), std_dev_img_(std_dev_img), model_folder_(model_folder) {}

  virtual void visit_composition(dbrec_composition* c);
  virtual void visit_gaussian_primitive(dbrec_gaussian* g);
protected:
  vil_image_view<float> mean_img_;
  vil_image_view<float> std_dev_img_;
  vcl_string model_folder_;
};

class dbrec_image_construct_bg_model_rot_inv_visitor : public dbrec_image_construct_bg_model_visitor {
public:
  dbrec_image_construct_bg_model_rot_inv_visitor(const vil_image_view<float>& mean_img, const vil_image_view<float>& std_dev_img, 
    float angle_inc, const vcl_string& model_folder) : dbrec_image_construct_bg_model_visitor(mean_img, std_dev_img, model_folder), angle_inc_(angle_inc) {}
  virtual void visit_gaussian_primitive(dbrec_gaussian* g);
protected:
  float angle_inc_;
};


#endif  //_dbrec_image_learner_visitors_h_
