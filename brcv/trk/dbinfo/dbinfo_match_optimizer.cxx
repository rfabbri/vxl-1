#include <vnl/vnl_numeric_traits.h>
#include <vnl/algo/vnl_amoeba.h>

#include <dbinfo/dbinfo_match_optimizer.h>
#include <dbinfo/dbinfo_region_geometry.h>
#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_observation_generator.h>
#include <dbinfo/dbinfo_observation_matcher.h>

#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vcl_cstdlib.h> // for rand()
#include <dbinfo/dbinfo_feature_data.h>
#include <vsol/vsol_point_2d.h>

dbinfo_cost_func::dbinfo_cost_func(unsigned number_of_parameters):
  vnl_cost_function(number_of_parameters),
  no_params_(number_of_parameters), obs0_(0),initial_obs_(0),current_obs_(0),
  radius_(1.0), sufficient_points_thresh_(0.9f), debug_level_(0), forced_scale_(1.0f)
{
}

// the cost function for optimization. In this application,
// the vector x corresponds to the parameters of the allowable transformation
// between the two observations, e.g. tx and ty. The cost 
// is based on the total mutual information between
// the observations.  Later, individual feature mutual information values might
// be considered.
double 
dbinfo_cost_func::f(vnl_vector<double> const& x)
{
  assert(obs0_);
  // Ozge: commented this assertion since f is applying the transformation on
  // initial_obs_
  //  assert(current_obs_);
  double bigval =  vnl_numeric_traits<double>::maxval;
  vgl_h_matrix_2d<float> H;
  H.set_identity();
  
  switch (no_params_)
    {
    case 2:
      {
        assert(x.size()==2);
        if(forced_scale_!=1.0f)
          H.set_scale(forced_scale_);
        H.set_translation(static_cast<float>(x[0]), static_cast<float>(x[1]));
        break;
      }
    case 3:
      {
        assert(x.size()==3);
        //normalize by radius to keep the elements of X at the same scale
        H.set_rotation(static_cast<float>(x[2]/(radius_)));
        if(forced_scale_!=1.0f)
          H.set_scale(forced_scale_);
        H.set_translation(static_cast<float>(x[0]), static_cast<float>(x[1]));
        break;
      }
    case 4:
      {
        assert(x.size()==4);
        H.set_rotation(static_cast<float>(x[2]/(radius_)));
        if(forced_scale_==1.0f)
          H.set_scale(static_cast<float>(x[3]/radius_+1.0f));
        else
          H.set_scale(forced_scale_);
        H.set_translation(static_cast<float>(x[0]), static_cast<float>(x[1]));
        break;
      }
    default:  
      vcl_cout << "Fatal error in dbinfo_cost_func::f() - "
               << " unknown transformation spec\n";
      return bigval;
    }
  
  
  current_obs_ = 
    dbinfo_observation_generator::generate(initial_obs_, H,
                                           sufficient_points_thresh_);
  if(!current_obs_||!current_obs_->scan(frame_, resc_))
    return bigval;

  double c = max_info_-dbinfo_observation_matcher::minfo(obs0_, current_obs_,
                                                         use_intensity_,
                                                         use_gradient_,
                                                         false);
  if(debug_level_>1)
    switch(no_params_)
      {
      case 2:
        vcl_cout << "c(" << x[0] << ' ' << x[1] << ")= " << c << '\n';
        break;
      case 3:
        vcl_cout << "c(" << x[0] << ' ' << x[1] << ' ' 
                 << x[2] << ")= " << c << '\n';
        break;
      case 4:
        vcl_cout << "c(" << x[0] << ' ' << x[1] << ' ' 
                 << x[2] << ' ' << x[3] << ")= " << c << '\n';
        break;
   }
  return c;
}


dbinfo_match_optimizer::dbinfo_match_optimizer(const float max_info, 
                                               const float dx,
                                               const float dtheta,
                                               const float dscale) :
  debug_level_(0), max_info_(max_info), dx_(dx), dtheta_(dtheta),
  dscale_(dscale), no_params_(0), current_cost_(-1), spec_(TRANS_SPEC_UNKNOWN),
  frame_(0), resc_(0), obs0_(0), best_seed_(0), optimized_obs_(0),
  radius_(1.0), fraction_(0.2), sufficient_points_thresh_(0.9f), forced_scale_(1.0f)
{
}

void dbinfo_match_optimizer::set_obs0(dbinfo_observation_sptr const & obs0)
{
  assert(obs0);
  obs0_ = obs0;
  dbinfo_region_geometry_sptr rg = obs0_->geometry();
  radius_ = (rg->diameter())/2.0;
}

bool dbinfo_match_optimizer::optimize()
{
  //the necessary ingredients
  assert(resc_);
  assert(obs0_);
  assert(best_seed_);
  bool valid_transform = false;
  //determine the class of transformation
  if(dtheta_==0 && dscale_ == 0)
    {
      spec_ = TRANSLATION;
      no_params_ = 2;
      x_.set_size(no_params_);
      x_[0]=dx_;      x_[1]=dx_;
      valid_transform = true;
    }
  else if(dscale_ == 0)
    {
      spec_ = TRANSLATION_ROTATION;
      no_params_ = 3;
      x_.set_size(no_params_);
      x_[0]=dx_;      x_[1]=dx_;
      x_[2]=dtheta_*radius_;
      valid_transform = true;
    }
  else
    {
      spec_ = TRANSLATION_ROTATION_SCALE;
      no_params_ = 4;
      x_.set_size(no_params_);
      x_[0]=dx_;      x_[1]=dx_;
      x_[2]=dtheta_*radius_;  x_[3]=dscale_*radius_;
      valid_transform = true;
    }
  if(!valid_transform)
    return false;
  //construct the least squares function
  dbinfo_cost_func c(no_params_);
  c.set_max_info(max_info_);
  c.set_obs0(obs0_);
  c.set_initial(best_seed_);
  c.set_frame(frame_);
  c.set_resource(resc_);
  c.set_radius(radius_);
  c.set_thresh(sufficient_points_thresh_);
  c.set_debug_level(debug_level_);
  c.set_forced_scale(forced_scale_);
  c.set_use_intensity(use_intensity_);
  c.set_use_gradient(use_gradient_);
  //construct the Levenberg Marquardt minimizer
  vnl_amoeba minimizer(c);
  minimizer.set_relative_diameter(radius_*fraction_);
  minimizer.set_x_tolerance(0.05);
  minimizer.set_max_iterations(150);
  if(debug_level_>2)
    minimizer.verbose = true;
  if(debug_level_>0)
    vcl_cout << "Start cost " << c.f(x_) << '\n';
  minimizer.minimize(x_);
  
  // set the optimized observation
  optimized_obs_ = c.current_obs();
  if(!optimized_obs_)
    return false;
  //set the current cost
  current_cost_ = c.f(x_);
  if(debug_level_>0)
    {
      vcl_vector<double> p = this->current_params();
      vcl_cout << "Optimized cost(";
      for(vcl_vector<double>::iterator pit = p.begin();
          pit != p.end(); ++pit)
        vcl_cout << *pit << ' ';
      vcl_cout << ")= " << current_cost_ << '\n';
    }
  return true;
}

vcl_vector<double> dbinfo_match_optimizer::current_params()
{
  if(!x_.size()||!no_params_)
    return vcl_vector<double>(0);
  vcl_vector<double> p(no_params_);
  for(unsigned i=0; i<no_params_; ++i)
    p[i]=x_[i];

  if(no_params_==2)
    return p;
  if(no_params_==3)
    {
      p[2]/=radius_;
      return p;
    }
  if(no_params_ ==4)
    {
      p[2]/=radius_;
      p[3]/= radius_;
      return p;
    }
p.clear();
 return p;
}

vgl_h_matrix_2d<double> dbinfo_match_optimizer::current_transform()
{
  vgl_h_matrix_2d<double> H;
  H.set_identity();
  vcl_vector<double> p = this->current_params();
  if(p.size()==0)
    return H;
  if(no_params_==2)
    {
      H.set_translation(p[0], p[1]);
      return H;
    }
  if(no_params_==3)
    {
      H.set_rotation(p[2]);
      H.set_translation(p[0], p[1]);
      return H;
    }
  if(no_params_==4)
    {
      H.set_rotation(p[2]);
      H.set_scale(1.0+p[3]);
      H.set_translation(p[0], p[1]);
      return H;
    }
  return H;
}

