#ifndef psm_appearance_model_traits_h_
#define psm_appearance_model_traits_h_


#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_gauss_if3.h>
#include <vnl/vnl_vector_fixed.h>
#include <vil/vil_rgb.h>

#include <vcl_ostream.h>

class psm_mog_grey_processor;
class psm_simple_grey_processor;
class psm_mog_rgb_processor;
class psm_simple_rgb_processor;

enum psm_apm_type
{
  PSM_APM_MOG_GREY = 0,
  PSM_APM_MOG_RGB,
  PSM_APM_SIMPLE_GREY,
  PSM_APM_SIMPLE_RGB,
  PSM_APM_NA,
  PSM_APM_UNKNOWN
};

//: Pixel properties for templates.
template <psm_apm_type>
class psm_apm_traits;


//: traits for a mixture of gaussian appearance model of gray-scale images
template<>
class psm_apm_traits<PSM_APM_MOG_GREY>
{
public:
  static const unsigned int n_gaussian_modes_ = 3;

  typedef bsta_num_obs<bsta_gauss_f1> gauss_type_f1;
  typedef bsta_num_obs<bsta_mixture_fixed<gauss_type_f1, n_gaussian_modes_> > mix_gauss_f1_type;

public:
  static const unsigned int obs_dim = 1;
  typedef  mix_gauss_f1_type apm_datatype;
  typedef float obs_datatype;
  typedef float obs_mathtype;
  typedef psm_mog_grey_processor apm_processor;

};


//: traits for a mixture of gaussian appearance model of rgb images
template<>
class psm_apm_traits<PSM_APM_MOG_RGB>
{
public:
  static const unsigned int n_gaussian_modes_ = 3;

  typedef bsta_num_obs<bsta_gauss_if3> gauss_type_if3;
  typedef bsta_num_obs<bsta_mixture_fixed<gauss_type_if3, n_gaussian_modes_> > mix_gauss_if3_type;

public:
  static const unsigned int obs_dim = 3;
  typedef  mix_gauss_if3_type apm_datatype;
  typedef vil_rgb<float> obs_datatype;
  typedef float obs_mathtype;
  typedef psm_mog_rgb_processor apm_processor;

};


//: traits for a simple one-valued appearance model of grey-scale images
class psm_simple_grey
{
public:
  psm_simple_grey() : color_(0.5f), one_over_sigma_(1.0f) /*, gauss_weight_(0.0f)*/ {}
  psm_simple_grey(float color, float std_dev, float gauss_weight=1.0f) : color_(color), one_over_sigma_(1/std_dev)/* ,gauss_weight_(gauss_weight)*/ { check_vals(); }
  psm_simple_grey(vnl_vector_fixed<float,3> const& params) : color_(params[0]), one_over_sigma_(1.0f/params[1])/*,gauss_weight_(params[2])*/ { check_vals(); }

  inline float color() const {return color_;}
  inline float sigma() const {return 1.0f/one_over_sigma_;}
  inline float gauss_weight() const {return 1.0f;}//{return gauss_weight_;}
  inline float one_over_sigma() const {return one_over_sigma_;}

protected:
  inline void check_vals()
  { 
    if (!(color_ > 0.0f)) 
      color_ = 0.0f;
    if (!(color_ < 1.0f))
      color_ = 1.0f;
    if (!(one_over_sigma_ < 1e4))
      one_over_sigma_ = 1e4f;
    if (!(one_over_sigma_ > 1e-4))
      one_over_sigma_ = 1e-4f;
    //if (!(gauss_weight_ > 0.0f))
    //  gauss_weight_ = 0.0f;
    //if (!(gauss_weight_ < 1.0f))
    //  gauss_weight_ = 1.0f;
  }

  float color_;
  float one_over_sigma_;
  //float gauss_weight_;


};

template<>
class psm_apm_traits<PSM_APM_SIMPLE_GREY>
{

public:
  static const unsigned int obs_dim = 1;
  static const unsigned int n_params = 3;
  typedef psm_simple_grey apm_datatype;
  typedef float obs_datatype;
  typedef float obs_mathtype;
  typedef psm_simple_grey_processor apm_processor;

};


//: traits for a simple one-valued appearance model of rgb images
class psm_simple_rgb
{
public:
  psm_simple_rgb() : color_(0.5f, 0.0f, 0.0f), one_over_sigma_(1.0f) {}
  psm_simple_rgb(vnl_vector_fixed<float,3> color, vnl_vector_fixed<float,3> std_dev) : color_(color), one_over_sigma_(1.0f/std_dev[0], 1.0f/std_dev[1], 1.0f/std_dev[2]) { check_vals(); } 
  psm_simple_rgb(vnl_vector_fixed<float,3> color, float std_dev) : color_(color), one_over_sigma_(1.0f/std_dev, 1.0f/std_dev, 1.0f/std_dev) {check_vals();}
  psm_simple_rgb(vnl_vector_fixed<float,4> const& params) : color_(params.extract(3,0)), one_over_sigma_(1.0f/params[3], 1.0f/params[4], 1.0f/params[5]) {check_vals();}



  inline vnl_vector_fixed<float,3> color() const {return color_;}
  //inline vnl_vector_fixed<float,3> color_vec() const {return color_;}
  inline vnl_vector_fixed<float,3> sigma() const {return vnl_vector_fixed<float,3>(1.0f/one_over_sigma_[0], 1.0f/one_over_sigma_[1], 1.0f/one_over_sigma_[2]);}
  inline vnl_vector_fixed<float,3> one_over_sigma() const {return one_over_sigma_;}
  inline float gauss_weight() const {return 1.0f;}

protected:
  inline void check_vals()
  { 
    if (!(color_[0] > 0.0f)) 
      color_[0] = 0.0f;
    if (!(color_[0] < 1.0f))
      color_[0] = 1.0f;  
    
    for (unsigned int i=1; i<3; ++i) {
      if (!(color_[i] > -1.0f)) 
        color_[i] = -1.0f;
      if (!(color_[i] < 1.0f))
        color_[i] = 1.0f;
    }

    for (unsigned int i=0; i<3; ++i) {
      if (!(one_over_sigma_[i] < 1e3))
        one_over_sigma_[i] = 1e3f;
      if (!(one_over_sigma_[i] > 0.1))
        one_over_sigma_[i] = 0.1f;
    }
  }

  vnl_vector_fixed<float,3> color_;
  vnl_vector_fixed<float,3> one_over_sigma_;
};

template<>
class psm_apm_traits<PSM_APM_SIMPLE_RGB>
{

public:
  static const unsigned int obs_dim = 3;
  static const unsigned int n_params = 4;
  typedef psm_simple_rgb apm_datatype;
  typedef vil_rgb<float> obs_datatype;
  typedef float obs_mathtype;
  typedef psm_simple_rgb_processor apm_processor;

};


#endif

