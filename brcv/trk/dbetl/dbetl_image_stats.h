// This is /algo/dbetl/dbetl_image_stats.h
#ifndef dbetl_image_stats_h_
#define dbetl_image_stats_h_

//:
// \file
// \brief  A class to maintain image statistics along an epipolar line
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date   11/23/2004


#include <vil/vil_rgb.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>


//: A simple class to maintain image statistics along an epipole line
class dbetl_image_stats
{
public:
  friend dbetl_image_stats operator+ (const dbetl_image_stats& lhs, 
                                     const dbetl_image_stats& rhs);
  //: Default Constructor
  dbetl_image_stats()
    : num_samples_(0), moment1_(vil_rgb<double>(0.0,0.0,0.0)), moment2_(0.0) {}

  //: Constructor
  dbetl_image_stats(int num, const vil_rgb<double>& moment1, double moment2)
    : num_samples_(num), moment1_(moment1), moment2_(moment2) {}

  //: Add a sample point to the statistics
  void add_sample( const vil_rgb<double>& s ) 
  {
    moment1_ += s;
    double grey = s.grey();
    moment2_ += grey*grey;
    ++num_samples_;
  }

  //: Return the mean color
  vil_rgb<double> color() const
  {
    if(num_samples_ <= 0)
      return moment1_;
    return moment1_/num_samples_;
  }

  //: Return the intensity mean
  double int_mean() const 
  { 
    if(num_samples_ < 1) return 0.0;
    return moment1_.grey()/num_samples_; 
  }

  //: Return the intensity variance
  double int_var() const 
  { 
    // Set a minimum variance so that small set do not have zero variance
    const double min_var = 1e-8;
    if(num_samples_ < 2) return min_var;
    double mean = this->int_mean();
    // the bias-corrected sample variance 
    double s = (moment2_ - mean*mean*num_samples_)/(num_samples_-1);
    return s + min_var/num_samples_;
  }

  //: Returns the negative log of the probability of \p val
  double intensity_cost(double val) const
  {
    double error = val - this->int_mean();
    double var = 0.001;//this->int_var();
    return error*error/var + vcl_log(2*vnl_math::pi*var)/2.0;
  }

  //: Addition operator for updating statistics
  dbetl_image_stats& operator += (const dbetl_image_stats& rhs)
  {
    this->num_samples_ += rhs.num_samples_;
    this->moment1_     += rhs.moment1_;
    this->moment2_     += rhs.moment2_;
    return *this;
  }

private:
  //: The number of samples
  int num_samples_;
  //: The first moment of the samples (in RGB)
  vil_rgb<double> moment1_;
  //: The second moment of the samples (intensity only)
  double moment2_;
};


//: Addition operator for combining statistics
inline dbetl_image_stats 
operator+ (const dbetl_image_stats& lhs, const dbetl_image_stats& rhs)
{
  return dbetl_image_stats( lhs.num_samples_ + rhs.num_samples_,
                           lhs.moment1_     + rhs.moment1_,
                           lhs.moment2_     + rhs.moment2_);
}


#endif // dbetl_image_stats_h_
