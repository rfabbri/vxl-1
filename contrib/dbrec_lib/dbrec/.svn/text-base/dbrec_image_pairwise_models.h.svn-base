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


#if !defined(_dbrec_image_pairwise_models_h_)
#define _dbrec_image_pairwise_models_h_

#include "dbrec_part_sptr.h"
#include "dbrec_part_context.h"
#include "dbrec_image_pairwise_models_sptr.h"

#include <vbl/vbl_ref_count.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_point_2d.h>
#include <vnl/vnl_math.h>
#include <bsta/bsta_gaussian_sphere.h>
#include <bsta/bsta_joint_histogram.h>
#include <bxml/bsvg/bsvg_document.h>

class dbrec_model_factory : public vbl_ref_count {
public:
  virtual ~dbrec_model_factory() {}
  virtual dbrec_pairwise_model_sptr generate_model() const = 0;
};
typedef vbl_smart_ptr<dbrec_model_factory> dbrec_model_factory_sptr;

class dbrec_pairwise_indep_gaussian_model_factory : public dbrec_model_factory {
public:
  dbrec_pairwise_indep_gaussian_model_factory(float min_stad_dev_dist, float min_stad_dev_angle) : min_stad_dev_dist_(min_stad_dev_dist), min_stad_dev_angle_(min_stad_dev_angle) {}
  dbrec_pairwise_model_sptr generate_model() const;
protected:
  float min_stad_dev_dist_;
  float min_stad_dev_angle_;
};
class dbrec_pairwise_discrete_model_factory : public dbrec_model_factory {
public:
  dbrec_pairwise_discrete_model_factory(unsigned int nbins_a, float min_dist, float max_dist, unsigned int nbins_dist) : nbins_a_(nbins_a), min_dist_(min_dist), max_dist_(max_dist), nbins_dist_(nbins_dist) {}
  dbrec_pairwise_model_sptr generate_model() const;
protected:
  unsigned int nbins_a_;
  float min_dist_;
  float max_dist_;
  unsigned int nbins_dist_;
};


class dbrec_pairwise_model : public vbl_ref_count {
public:
  
  //: sample location of the second wrt first's given location
  virtual void sample_location(const vgl_point_2d<float>& loc, float scale, dbrec_part_sptr first_part, vgl_point_2d<float>& second_loc) const {}

  //: create a probe box using the model given the first part's location
  virtual vgl_box_2d<float> get_probe_box(const vgl_point_2d<float>& central_part_loc, const vnl_vector_fixed<float, 2>& central_part_direction_vector) const = 0;

  virtual double prob_density(const vnl_vector_fixed<float, 2>& central_part_direction_vector, 
    const vgl_point_2d<float>& central_part_loc, const vgl_point_2d<float>& second_part_loc) const = 0;

  //: the magnitudes in this method are the weights of the parts, usually their class posterior
  virtual void update_models(const vnl_vector_fixed<float, 2>& central_part_direction_vector,  
    const vnl_vector_fixed<float,2>& central_part_loc, const vnl_vector_fixed<float,2>& second_part_loc, float central_part_mag = 1.0f, float second_part_mag = 1.0f) {}

  //: return self as a bxml_data_sptr
  virtual bxml_data_sptr xml_element() const = 0;
  //: read self from the bxml_data_sptr
  static dbrec_pairwise_model_sptr xml_parse_element(bxml_data_sptr data) { return 0; }

  //: visualize the model 
  virtual void visualize(vcl_string& doc_name) const = 0;

  virtual dbrec_model_factory_sptr get_a_factory() const = 0;

protected:

};

//: Concrete classes for pairwise modeling

//: independent gaussian models for distance and angle
class dbrec_pairwise_indep_gaussian_model : public dbrec_pairwise_model {
public:
  dbrec_pairwise_indep_gaussian_model(const bsta_gaussian_sphere<double, 1>& dist_model, const bsta_gaussian_sphere<double, 1>& angle_model)
    : min_stad_dev_dist_(2.0f), min_stad_dev_angle_(10.0f), dist_model_(dist_model), angle_model_(angle_model) {}

  dbrec_pairwise_indep_gaussian_model(const bsta_gaussian_sphere<double, 1>& dist_model, const bsta_gaussian_sphere<double, 1>& angle_model, 
    float min_stad_dev_dist, float min_stad_dev_angle) 
    : min_stad_dev_dist_(min_stad_dev_dist), min_stad_dev_angle_(min_stad_dev_angle), dist_model_(dist_model), angle_model_(angle_model) {}

  dbrec_pairwise_indep_gaussian_model(float min_stad_dev_dist, float min_stad_dev_angle) 
    : min_stad_dev_dist_(min_stad_dev_dist), min_stad_dev_angle_(min_stad_dev_angle) {}

  //: sample location of the second wrt first's given location
  virtual void sample_location(const vgl_point_2d<float>& loc, float scale, dbrec_part_sptr first_part, vgl_point_2d<float>& second_loc) const;

  //: create a probe box using the model given the first part's location
  virtual vgl_box_2d<float> get_probe_box(const vgl_point_2d<float>& central_part_loc, const vnl_vector_fixed<float, 2>& central_part_direction_vector) const;

  virtual double prob_density(const vnl_vector_fixed<float, 2>& central_part_direction_vector, 
    const vgl_point_2d<float>& central_part_loc, const vgl_point_2d<float>& second_part_loc) const;

  void update_models(const vnl_vector_fixed<float, 2>& central_part_direction_vector,  
    const vnl_vector_fixed<float,2>& central_part_loc, const vnl_vector_fixed<float,2>& second_part_loc, float central_part_mag = 1.0f, float second_part_mag = 1.0f); 

  //: return self as a bxml_data_sptr
  virtual bxml_data_sptr xml_element() const;
  //: read self from the bxml_data_sptr
  static dbrec_pairwise_model_sptr xml_parse_element(bxml_data_sptr data);

  //: visualize the model 
  virtual void visualize(vcl_string& doc_name) const {};

  virtual dbrec_model_factory_sptr get_a_factory() const { return new dbrec_pairwise_indep_gaussian_model_factory(min_stad_dev_dist_, min_stad_dev_angle_); }

protected:
  float min_stad_dev_dist_;
  float min_stad_dev_angle_;

  bsta_gaussian_sphere<double, 1> dist_model_;
  bsta_gaussian_sphere<double, 1> angle_model_;
};

//: independent uniform models for distance and angle
class dbrec_pairwise_indep_uniform_model : public dbrec_pairwise_model {
public:
  dbrec_pairwise_indep_uniform_model(float dmin, float dmax, float angle_min = 0.0f, float angle_max = 2.0f*vnl_math::pi)
    : min_d_(dmin), max_d_(dmax), min_angle_(angle_min), max_angle_(angle_max) {}

  //: sample location of the second wrt first's given location
  virtual void sample_location(const vgl_point_2d<float>& loc, float scale, dbrec_part_sptr first_part, vgl_point_2d<float>& second_loc) const;

  //: create a probe box using the model given the first part's location
  virtual vgl_box_2d<float> get_probe_box(const vgl_point_2d<float>& central_part_loc, const vnl_vector_fixed<float, 2>& central_part_direction_vector) const;

  virtual double prob_density(const vnl_vector_fixed<float, 2>& central_part_direction_vector, 
    const vgl_point_2d<float>& central_part_loc, const vgl_point_2d<float>& second_part_loc) const;

  //: the magnitudes in this method are the weights of the parts, usually their class posterior
  virtual void update_models(const vnl_vector_fixed<float, 2>& central_part_direction_vector,  
    const vnl_vector_fixed<float,2>& central_part_loc, const vnl_vector_fixed<float,2>& second_part_loc, float central_part_mag = 1.0f, float second_part_mag = 1.0f) { throw 0; }

  //: return self as a bxml_data_sptr
  virtual bxml_data_sptr xml_element() const;
  //: read self from the bxml_data_sptr
  static dbrec_pairwise_model_sptr xml_parse_element(bxml_data_sptr data);

  //: visualize the model 
  virtual void visualize(vcl_string& doc_name) const {};

  //: factory not implemented for this type
  virtual dbrec_model_factory_sptr get_a_factory() const { throw 0; }

protected:
  float min_d_;
  float max_d_;
  float min_angle_;
  float max_angle_;
};

//: assumes 8 discrete directions for the angle
class dbrec_pairwise_indep_uniform_discrete_model : public dbrec_pairwise_model {
public:
  dbrec_pairwise_indep_uniform_discrete_model(float radius) : radius_(radius) {}

  //: create a probe box using the model given the first part's location
  virtual vgl_box_2d<float> get_probe_box(const vgl_point_2d<float>& central_part_loc, const vnl_vector_fixed<float, 2>& central_part_direction_vector) const;
  virtual vgl_box_2d<float> get_probe_box(const vgl_point_2d<float>& central_part_loc) const;

  virtual double prob_density(const vnl_vector_fixed<float, 2>& central_part_direction_vector, 
    const vgl_point_2d<float>& central_part_loc, const vgl_point_2d<float>& second_part_loc) const { return 1.0f/radius_ * 1.0f/8.0f; }

  //: the magnitudes in this method are the weights of the parts, usually their class posterior
  virtual void update_models(const vnl_vector_fixed<float, 2>& central_part_direction_vector, 
    const vnl_vector_fixed<float,2>& central_part_loc, const vnl_vector_fixed<float,2>& second_part_loc, float central_part_mag = 1.0f, float second_part_mag = 1.0f) { throw 0; }

  //: return self as a bxml_data_sptr
  virtual bxml_data_sptr xml_element() const;
  //: read self from the bxml_data_sptr
  static dbrec_pairwise_model_sptr xml_parse_element(bxml_data_sptr data);

  //: visualize the model 
  virtual void visualize(vcl_string& doc_name) const {};

  //: factory not implemented for this type
  virtual dbrec_model_factory_sptr get_a_factory() const { throw 0; }

protected:
  float radius_;
};

//: a 2D gaussian model for x and y
class dbrec_pairwise_2d_gaussian_model : public dbrec_pairwise_model {
public:
  //: sample location of the second wrt first's given location
  virtual void sample_location(const vgl_point_2d<float>& loc, float scale, dbrec_part_sptr first_part, vgl_point_2d<float>& second_loc) const;

  //: create a probe box using the model given the first part's location
  virtual vgl_box_2d<float> get_probe_box(const vgl_point_2d<float>& central_part_loc, const vnl_vector_fixed<float, 2>& central_part_direction_vector) const;
protected:
};

//: a model which contains a joint histogram (a discrete model) for angle and distance
class dbrec_pairwise_discrete_model : public dbrec_pairwise_model {
public:
  //: convention for the histogram's two dimenstions: first dimension (a) is angle, and second (b) is distance, 
  //  the angle range is within [-pi, pi], just set the number of bins (usually 8), distance range is given by [min_dist, max_dist]
  dbrec_pairwise_discrete_model(unsigned int nbins_a, float min_dist, float max_dist, unsigned int nbins_dist) : hist_((float)(-vnl_math::pi), (float)(vnl_math::pi), nbins_a, min_dist, max_dist, nbins_dist) {}

  //: constructor with the histogram, shall only be used by the xml parser
  dbrec_pairwise_discrete_model(const bsta_joint_histogram<float>& hist) : hist_(hist) {}

  //: sample location of the second wrt first's given location
  virtual void sample_location(const vgl_point_2d<float>& loc, float scale, dbrec_part_sptr first_part, vgl_point_2d<float>& second_loc) const;

  //: create a probe box using the model given the first part's location
  virtual vgl_box_2d<float> get_probe_box(const vgl_point_2d<float>& central_part_loc, const vnl_vector_fixed<float, 2>& central_part_direction_vector) const;

  virtual double prob_density(const vnl_vector_fixed<float, 2>& central_part_direction_vector, 
    const vgl_point_2d<float>& central_part_loc, const vgl_point_2d<float>& second_part_loc) const;

  //: the magnitudes in this method are the weights which will be multiplied and used to upcount the joint histogram
  //  CAUTION: this method does not check whether the values for angle and distance are within the histogram range
  void update_models(const vnl_vector_fixed<float, 2>& central_part_direction_vector,  
    const vnl_vector_fixed<float,2>& central_part_loc, const vnl_vector_fixed<float,2>& second_part_loc, float central_part_mag = 1.0f, float second_part_mag = 1.0f);

  void print_hist_to_vrml(vcl_ostream& os) { hist_.print_to_vrml(os); }

  //: return self as a bxml_data_sptr
  virtual bxml_data_sptr xml_element() const;
  //: read self from the bxml_data_sptr
  static dbrec_pairwise_model_sptr xml_parse_element(bxml_data_sptr data);

  //: visualize the model 
  virtual void visualize(vcl_string& doc_name) const;

  virtual dbrec_model_factory_sptr get_a_factory() const { return new dbrec_pairwise_discrete_model_factory(hist_.nbins_a(), hist_.min_b(), hist_.max_b(), hist_.nbins_b()); }

  //: create new instances of self by sampling from the histogram
  bool sample_new_instances(unsigned n, vcl_vector<dbrec_pairwise_model_sptr>& new_ins);

  //: create new instances of indep gaussian models by sampling from the histogram
  bool sample_new_indep_gaussian_instances(unsigned n, vcl_vector<dbrec_pairwise_model_sptr>& new_ins);

protected:
  //: a joint histogram for angle and distance to be used, convention: first dimension (a) is angle, and second (b) is distance
  bsta_joint_histogram<float> hist_;  

};

class dbrec_pairwise_rot_invariant_model : public dbrec_pairwise_model {
public:
  dbrec_pairwise_rot_invariant_model(int d_min, int d_max, int rho_min, int rho_max, int rho_range, int gamma_min, int gamma_max, int gamma_range, int nbins_d, int nbins_rho)
    : d_min_(d_min), d_max_(d_max), rho_min_(rho_min), rho_max_(rho_max), rho_range_(rho_range), 
      gamma_min_(gamma_min), gamma_max_(gamma_max), gamma_range_(gamma_range)
  { assert(gamma_range_ > gamma_min_ && gamma_range_ > gamma_max_); 
    assert(rho_range_ > rho_min_ && rho_range_ > rho_max_); 
    if (rho_range_ > 0) 
      non_class_prob_ = 1.0/nbins_d * 1.0/nbins_rho;
    else 
      non_class_prob_ = 1.0/nbins_d;
  }
  
  dbrec_pairwise_rot_invariant_model(int d_min, int d_max, int rho_min, int rho_max, int rho_range, int gamma_min, int gamma_max, int gamma_range, double non_class_prob)
    : d_min_(d_min), d_max_(d_max), rho_min_(rho_min), rho_max_(rho_max), rho_range_(rho_range), gamma_min_(gamma_min), gamma_max_(gamma_max), gamma_range_(gamma_range), non_class_prob_(non_class_prob) {}
  
  int sample_d(vnl_random& rng) const;
  int sample_rho(vnl_random& rng) const;
  int sample_gamma(vnl_random& rng) const;  

  double prob_density(dbrec_part_ins_sptr p1_ins, dbrec_part_ins_sptr p2_ins) const;
  double prob_density_gamma(int gamma) const;
  double prob_density_rho(int rho) const;
  double prob_density_d(int d) const;

  bool rho_defined() { return rho_range_ > 0; }
  bool gamma_defined() { return gamma_range_ > 0; }

  static int measure_gamma(dbrec_part_ins_sptr p1_ins, dbrec_part_ins_sptr p2_ins);
  static int measure_rho(dbrec_part_ins_sptr p1_ins, dbrec_part_ins_sptr p2_ins);
  static int measure_d(dbrec_part_ins_sptr p1_ins, dbrec_part_ins_sptr p2_ins);

  //: this is a constant value and depends on the number of bins for d and rho
  double prob_density_non_class() const { return non_class_prob_; }

  virtual void sample_location(const vgl_point_2d<float>& loc, float scale, dbrec_part_sptr first_part, vgl_point_2d<float>& second_loc) const { throw 0; }

  virtual vgl_box_2d<float> get_probe_box(const vgl_point_2d<float>& central_part_loc, const vnl_vector_fixed<float, 2>& central_part_direction_vector) const { throw 0; }

  virtual double prob_density(const vnl_vector_fixed<float, 2>& central_part_direction_vector, 
    const vgl_point_2d<float>& central_part_loc, const vgl_point_2d<float>& second_part_loc) const { throw 0; }

  virtual void update_models(const vnl_vector_fixed<float, 2>& central_part_direction_vector,  
    const vnl_vector_fixed<float,2>& central_part_loc, const vnl_vector_fixed<float,2>& second_part_loc, float central_part_mag = 1.0f, float second_part_mag = 1.0f) { throw 0; }

  virtual bxml_data_sptr xml_element() const;
  static dbrec_pairwise_model_sptr xml_parse_element(bxml_data_sptr data);

  virtual void visualize(vcl_string& doc_name) const { throw 0; }

  virtual dbrec_model_factory_sptr get_a_factory() const { throw 0; }

protected:
  int d_min_, d_max_;          // [d_min, d_max)
  int rho_min_, rho_max_;      // [rho_min, rho_max)
  int rho_range_;   // this range is the lowest common multiplier of the rotational symmetry angles of the two composing parts
                    // rho is in [0,rho_range)
  int gamma_min_, gamma_max_;  // [gamma_min_, gamma_max)
  int gamma_range_; // this range is the greatest common divisor of the rotational symmetry angles of the two composing parts                
                    // gamma is in [0,gamma_range)

  double non_class_prob_;
};

//: a model which contains a joint histogram (a discrete model) for rho and distance
//  convention for the histogram's two dimensions: first dimension (a) is angle, and second (b) is distance, 
//  and also stores a min and max value to denote gamma range
class dbrec_pairwise_rot_invariant_discrete_model : public dbrec_pairwise_model {
public:
  //: constructor when both gamma and rho are defined for the two parts
  dbrec_pairwise_rot_invariant_discrete_model(int gamma_min, int gamma_max, int gamma_range, int min_rho, int max_rho, unsigned int nbins_rho, int min_dist, int max_dist, unsigned int nbins_dist) : 
    gamma_min_(gamma_min), gamma_max_(gamma_max), gamma_range_(gamma_range), gamma_defined_(true), rho_defined_(true),
    class_hist_((float)min_rho, (float)max_rho, nbins_rho, (float)min_dist, (float)max_dist, nbins_dist), non_class_hist_((float)min_rho, (float)max_rho, nbins_rho, (float)min_dist, (float)max_dist, nbins_dist) {}

  //: constructor when gamma is undefined but rho is defined for the two parts
  dbrec_pairwise_rot_invariant_discrete_model(int min_rho, int max_rho, unsigned int nbins_rho, int min_dist, int max_dist, unsigned int nbins_dist) : 
    gamma_min_(-1), gamma_max_(-1), gamma_range_(0), gamma_defined_(false), rho_defined_(true),
    class_hist_((float)min_rho, (float)max_rho, nbins_rho, (float)min_dist, (float)max_dist, nbins_dist), non_class_hist_((float)min_rho, (float)max_rho, nbins_rho, (float)min_dist, (float)max_dist, nbins_dist) {}

  //: constructor when both gamma and rho are undefined for the two parts
  dbrec_pairwise_rot_invariant_discrete_model(int min_dist, int max_dist, unsigned int nbins_dist) : 
    gamma_min_(-1), gamma_max_(-1), gamma_range_(0), gamma_defined_(false), rho_defined_(false), 
    class_hist_(0.0f, 2.0f, 1, (float)min_dist, (float)max_dist, nbins_dist), non_class_hist_(0.0f, 2.0f, 1, (float)min_dist, (float)max_dist, nbins_dist) {}

  //: constructor for xml reader
  dbrec_pairwise_rot_invariant_discrete_model(int gamma_min, int gamma_max, int gamma_range, bool gamma_defined, bool rho_defined, 
    const bsta_joint_histogram<float>& class_hist, const bsta_joint_histogram<float>& non_class_hist) :  
  gamma_min_(gamma_min), gamma_max_(gamma_max), gamma_range_(gamma_range), gamma_defined_(gamma_defined), rho_defined_(rho_defined), class_hist_(class_hist), non_class_hist_(non_class_hist) {}
    

  //int sample_d(vnl_random& rng) const;
  //int sample_rho(vnl_random& rng) const;
  //int sample_gamma(vnl_random& rng) const;  

  void prob_densities(dbrec_part_ins_sptr p1_ins, dbrec_part_ins_sptr p2_ins, double& class_density, double& non_class_density) const;
  
  void update_d(int d, double class_magnitude, double non_class_magnitude); // case when rho & gamma are undefined
  void update_rho_d(int rho, int d, double class_magnitude, double non_class_magnitude);  // case when gamma is undefined
  void update_gamma_rho_d(int gamma, int rho, int d, double class_magnitude, double non_class_magnitude);
  
  virtual void sample_location(const vgl_point_2d<float>& loc, float scale, dbrec_part_sptr first_part, vgl_point_2d<float>& second_loc) const { throw 0; }
  virtual vgl_box_2d<float> get_probe_box(const vgl_point_2d<float>& central_part_loc, const vnl_vector_fixed<float, 2>& central_part_direction_vector) const { throw 0; }
  virtual double prob_density(const vnl_vector_fixed<float, 2>& central_part_direction_vector, 
    const vgl_point_2d<float>& central_part_loc, const vgl_point_2d<float>& second_part_loc) const { throw 0; }
  virtual void update_models(const vnl_vector_fixed<float, 2>& central_part_direction_vector,  
    const vnl_vector_fixed<float,2>& central_part_loc, const vnl_vector_fixed<float,2>& second_part_loc, float central_part_mag = 1.0f, float second_part_mag = 1.0f) { throw 0; }

  virtual bxml_data_sptr xml_element() const;
  static dbrec_pairwise_model_sptr xml_parse_element(bxml_data_sptr data);

  virtual void visualize(vcl_string& doc_name) const;
  virtual dbrec_model_factory_sptr get_a_factory() const { throw 0; }

  bool rho_defined() const { return rho_defined_; }
  bool gamma_defined() const { return gamma_defined_; }

  float class_entropy();
  float non_class_entropy();
  float class_volume();
  float non_class_volume();
protected:
  int gamma_min_, gamma_max_;  // [gamma_min_, gamma_max)
  int gamma_range_; // this range is the greatest common divisor of the rotational symmetry angles of the two composing parts                
                    // gamma is in [0,gamma_range)
  bool gamma_defined_, rho_defined_;

  //: a joint histogram for rho and distance to be used, convention: first dimension (a) is angle, and second (b) is distance
  bsta_joint_histogram<float> class_hist_;  
  bsta_joint_histogram<float> non_class_hist_;
};



#endif  //_dbrec_image_pairwise_models_h_
