#pragma once
#include <dbsta/bsta_attributes.h>
#include <dbsta/bsta_gauss_if3.h>
#include <dbsta/bsta_gauss_f1.h>
#include <dbsta/bsta_gauss_ff3.h>
#include <dbsta/bsta_mixture.h>

typedef bsta_gauss_if3 bsta_gauss_i;
typedef bsta_num_obs<bsta_gauss_i> gauss_indep;


typedef bsta_gauss_f1 bsta_gauss1_t;
typedef bsta_num_obs<bsta_gauss1_t> gauss_type1;


typedef bsta_gauss_ff3 bsta_gauss3_f;
typedef bsta_num_obs<bsta_gauss3_f> gauss_full;



//tratis class for merging components in a mixtures of gaussians. The merge equations can be foind in
//EM algorithms for Gaussian mixtures with split-and-merge operation, Z. Zhang et al. ,Pattern Recognition 36 (2003)
template<class dist_,class T>
class bsta_mixture_traits
{
public:

	static dist_ merge(const dist_& d1,T w1,const dist_& d2,T w2);
	inline static bool areTheSame(const dist_& d1,const dist_& d2);

};
template <>
class bsta_mixture_traits<gauss_type1,float>
{

public:
  static gauss_type1 merge(const gauss_type1& dist1,float w1,const gauss_type1& dist2,float w2 )
  {
	  	  gauss_type1 tempDist;
		  gauss_type1::vector_ tempMean;
		  gauss_type1::covar_type tempCovar;
		  tempMean=(dist1.mean()*w1+dist2.mean()*w2)/(w1+w2);
		  tempCovar=(dist1.covar()+dist1.mean()*dist1.mean())*w1+(dist2.covar()+dist2.mean()*dist2.mean())*w2;
		  tempCovar=tempCovar/(w1+w2);
		  tempCovar=tempCovar-tempMean*tempMean;
	      return tempDist;
  }
   inline static bool areTheSame(const gauss_type1& d1,const gauss_type1& d2)
  {
	  return ((d1.mean()==d2.mean())&&(d1.covar()==d2.covar()));
  }
   
};

template <>
class bsta_mixture_traits<gauss_indep,float>
{

public:
  static gauss_indep merge(const gauss_indep& dist1,float w1,const gauss_indep& dist2,float w2 )
  {
		  gauss_indep tempDist;
		  gauss_indep::vector_ tempMean;
		  gauss_indep::covar_type tempCovar;
		  tempMean=(dist1.mean()*w1+dist2.mean()*w2)/(w1+w2);
		  tempCovar=(dist1.covar()+element_product(dist1.mean(),dist1.mean()))*w1+
			      (dist2.covar()+element_product(dist2.mean(),dist2.mean()))*w2;
		  tempCovar=tempCovar/(w1+w2);
		  tempCovar=tempCovar-element_product(tempMean,tempMean);
		  tempDist.set_mean(tempMean);
		  tempDist.set_covar(tempCovar);
	      return tempDist;
  }
  inline static bool areTheSame(const gauss_indep& d1,const gauss_indep& d2)
  {
	  return ((d1.mean()-d2.mean()).two_norm()<1/255);
  }
};

template <>
class bsta_mixture_traits<bsta_gauss_i,float>
{

public:
  static bsta_gauss_i merge(const bsta_gauss_i& dist1,float w1,const bsta_gauss_i& dist2,float w2 )
  {
		  bsta_gauss_i tempDist;
		  bsta_gauss_i::vector_ tempMean;
		  bsta_gauss_i::covar_type tempCovar;
		  tempMean=(dist1.mean()*w1+dist2.mean()*w2)/(w1+w2);
		  tempCovar=(dist1.covar()+element_product(dist1.mean(),dist1.mean()))*w1+
			      (dist2.covar()+element_product(dist2.mean(),dist2.mean()))*w2;
		  tempCovar=tempCovar/(w1+w2);
		  tempCovar=tempCovar-element_product(tempMean,tempMean);
		  tempDist.set_mean(tempMean);
		  tempDist.set_covar(tempCovar);
	      return tempDist;
  }
  inline static bool areTheSame(const bsta_gauss_i& d1,const bsta_gauss_i& d2)
  {
	  return ((d1.mean()-d2.mean()).two_norm()<1/255);
  }
};

template <>
class bsta_mixture_traits<gauss_full,float>
{

public:
  static gauss_full merge(const gauss_full& dist1,float w1,const gauss_full& dist2,float w2 )
  {
		  gauss_full tempDist;
		  gauss_full::vector_type tempMean;
		  gauss_full::covar_type tempCovar;
		  tempMean=(dist1.mean()*w1+dist2.mean()*w2)/(w1+w2);
		  tempCovar=(dist1.covar()+outer_product(dist1.mean(),dist1.mean()))*w1+
			      (dist2.covar()+outer_product(dist2.mean(),dist2.mean()))*w2;
		  tempCovar=tempCovar/(w1+w2);
		  tempCovar=tempCovar-outer_product(tempMean,tempMean);
	      return tempDist;
  }
  inline static bool areTheSame(const gauss_full& d1,const gauss_full& d2)
  {
	  return ((d1.mean()-d2.mean()).two_norm()<0.001);
  
  }
};

