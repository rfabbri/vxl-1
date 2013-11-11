// This is brcv/seg/dbsta/dbsta_basic_functors.h
#ifndef dbsta_basic_functors_h_
#define dbsta_basic_functors_h_

//:
// \file
// \brief Basic functors for simple operations on Gaussian mixtures  
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 07/13/05
//
// 
//
// \verbatim
//  Modifications
// \endverbatim

#include "dbsta_functor.h"
#include "dbsta_mixture.h"
#include "dbsta_gaussian.h"


//: A functor to return the probability of a sample
template <class T>
class dbsta_probability_functor : public dbsta_data_functor<T>
{
 public:
  
  //: The size of the vector returned by the functor
  virtual unsigned int return_size() const { return 1; }
  
  //: The main function
  virtual vnl_vector<T> operator() ( const dbsta_distribution<T>& d, 
                                     const vnl_vector<T>& sample  ) const
  {
    return vnl_vector<T>(1,d.probability(sample));
  }

};


//: A functor to return the mean of the Gaussian
// \note the distribution must be Gaussian
template <class T>
class dbsta_mean_functor : public dbsta_functor<T>
{
 public:
 
  //: Constructor - optionally specify the return size
  dbsta_mean_functor<T>(unsigned int return_size=0) : return_size_(return_size) {}
  
  //: The size of the vector returned by the functor
  virtual unsigned int return_size() const { return return_size_; }
  
  //: The main function
  virtual vnl_vector<T> operator() ( const dbsta_distribution<T>& d ) const
  {
    assert(dynamic_cast<const dbsta_gaussian<T>* >(&d));
    return static_cast<const dbsta_gaussian<T>& >(d).mean();
  }
  
 private:
  unsigned int return_size_;
};


//: A functor to return the variance of the Gaussian
// \note the distribution must be Gaussian
template <class T>
class dbsta_var_functor : public dbsta_functor<T>
{
 public:
  
  //: The main function
  virtual vnl_vector<T> operator() ( const dbsta_distribution<T>& d ) const
  {
    assert(dynamic_cast<const dbsta_gaussian<T>* >(&d));
    const dbsta_gaussian<T>& g = static_cast<const dbsta_gaussian<T>& >(d);
    vnl_matrix<T> covar = g.covar();
    vnl_vector<T> var(g.dim());
    for(unsigned int i=0; i<g.dim(); ++i)
      var[i] = covar(i,i);
    return var;
  }
};


//: A functor to return the determinant of the covariance of the Gaussian
// \note the distribution must be Gaussian
template <class T>
class dbsta_det_covar_functor : public dbsta_functor<T>
{
 public:
 
  //: The size of the vector returned by the functor
  virtual unsigned int return_size() const { return 1; }
  
  //: The main function
  virtual vnl_vector<T> operator() ( const dbsta_distribution<T>& d ) const
  {
    assert(dynamic_cast<const dbsta_gaussian<T>* >(&d));
    return vnl_vector<T>(1,static_cast<const dbsta_gaussian<T>& >(d).det_covar());
  }
};


//: A functor to return the weight of the component with given index
// \note the distribution must be a mixture
template <class T>
class dbsta_weight_functor : public dbsta_functor<T>
{
 public:
  //: Constructor
  dbsta_weight_functor(unsigned int index = 0) : idx(index) {}
  
  //: The size of the vector returned by the functor
  virtual unsigned int return_size() const { return 1; }
  
  //: The main function
  virtual vnl_vector<T> operator() ( const dbsta_distribution<T>& d ) const
  {
    assert(dynamic_cast<const dbsta_mixture<T>* >(&d));
    const dbsta_mixture<T>& mix = static_cast<const dbsta_mixture<T>& >(d);
    if(idx < mix.num_components())
      return vnl_vector<T>(1,mix.weight(idx));
    
    return vnl_vector<T>(0);
  }
  
  unsigned int idx;
};


//: A functor to apply another functor to one distribution in the mixture
// \note the distribution must be a mixture
template <class T>
class dbsta_mixture_functor : public dbsta_functor<T>
{
 public:
  //: Constructor
  dbsta_mixture_functor(const vbl_smart_ptr<dbsta_functor<T> >& f, 
                        unsigned int index = 0 ) 
  : functor(f), idx(index) {}
  
  //: The size of the vector returned by the functor
  virtual unsigned int return_size() const { return functor->return_size(); }
  
  //: The main function
  //: \returns A zero dimension vector if no component of that index exists
  virtual vnl_vector<T> operator() ( const dbsta_distribution<T>& d ) const
  {
    assert(dynamic_cast<const dbsta_mixture<T>* >(&d));
    const dbsta_mixture<T>& mix = static_cast<const dbsta_mixture<T>& >(d);
    if(idx < mix.num_components() && mix.weight(idx) > T(0))
      return (*functor)(mix.distribution(idx));
      
    return vnl_vector<T>(0);
  }
  
  //: The functor to apply
  vbl_smart_ptr<dbsta_functor<T> > functor;
  //: The index to apply to
  unsigned int idx;
};


//: A functor to apply another functor to each distribution and produce a weighted sum
// \note the distribution must be a mixture
template <class T>
class dbsta_weighted_sum_functor : public dbsta_functor<T>
{
  public:
  //: Constructor
  dbsta_weighted_sum_functor(const vbl_smart_ptr<dbsta_functor<T> >& f):
    functor(f) {}

  //: The size of the vector returned by the functor
  virtual unsigned int return_size() const { return functor->return_size(); }

  //: The main function
  //: \returns A zero dimension vector if no component of that index exists
  virtual vnl_vector<T> operator() ( const dbsta_distribution<T>& d ) const
  {
    assert(dynamic_cast<const dbsta_mixture<T>* >(&d));
    const dbsta_mixture<T>& mix = static_cast<const dbsta_mixture<T>& >(d);
    const unsigned int nc = mix.num_components();
    if(nc > 0){
      vnl_vector<T> result = mix.weight(0) * (*functor)(mix.distribution(0));
      for(unsigned int idx=1; idx<nc; ++idx)
        result += mix.weight(idx) * (*functor)(mix.distribution(idx));
      return result;
    }
    return vnl_vector<T>(0);
  }

  //: The functor to apply
  vbl_smart_ptr<dbsta_functor<T> > functor;
};


//: A functor to count the number of components in the mixture
// \note the distribution must be a mixture
template <class T>
class dbsta_mixture_size_functor : public dbsta_functor<T>
{
 public:
  //: Constructor
  dbsta_mixture_size_functor() {}
  
  //: The size of the vector returned by the functor
  virtual unsigned int return_size() const { return 1; }
  
  //: The main function
  virtual vnl_vector<T> operator() ( const dbsta_distribution<T>& d ) const
  {
    assert(dynamic_cast<const dbsta_mixture<T>* >(&d));
    const dbsta_mixture<T>& mix = static_cast<const dbsta_mixture<T>& >(d);
    return vnl_vector<T>(1,mix.num_components()); 
  }
};

//: A functor to count the number of components in the mixture encoded in RGB
// \note the distribution must be a mixture
// \note currently only encodes values up to 7
template <class T>
class dbsta_mixture_size_color_functor : public dbsta_functor<T>
{
 public:
  //: Constructor
  dbsta_mixture_size_color_functor() {}
  
  //: The size of the vector returned by the functor
  virtual unsigned int return_size() const { return 3; }
  
  //: The main function
  virtual vnl_vector<T> operator() ( const dbsta_distribution<T>& d ) const
  {
    assert(dynamic_cast<const dbsta_mixture<T>* >(&d));
    const dbsta_mixture<T>& mix = static_cast<const dbsta_mixture<T>& >(d);
    unsigned int num = mix.num_components();
    vnl_vector<T> result(3);
    result[0] = num & 1;
    result[1] = num & 2;
    result[2] = num & 4;
    return result; 
  }
};


#endif // dbsta_basic_functors_h_
