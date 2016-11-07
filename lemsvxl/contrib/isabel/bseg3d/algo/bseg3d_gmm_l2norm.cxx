#include "bseg3d_gmm_l2norm.h"
//:
// \file
#include <vnl/vnl_math.h>
#include <vcl_cmath.h>
#include <vcl_vector.h>


double bseg3d_gmm_l2norm::distance (mix_gauss_type const& g, mix_gauss_type const& f)
{
  //define some constants so that they are only calculated once
  //srqt(4*pi)
  double k1 = (double(1.0)/double(4.0)) * vnl_math::two_over_sqrtpi;

  // This distance is composed of 5 terms
  double t1 = 0;
  double t2 = 0;
  double t3 = 0;
  double t4 = 0;
  double t5 = 0;

  //terms 1,2,3 
  for(unsigned i = 0; i < g.num_components(); i++)
  {
    t1 = t1 + vcl_pow(g.weight(i),2 )* k1 * (1/vcl_sqrt(g.distribution(i).var()));

    for(unsigned j = i+1; j < g.num_components(); j++)
    {
      double sum_var = g.distribution(i).var() +  g.distribution(j).var();
      t2 = t2 + g.weight(i)*g.weight(j)
        * vnl_math::one_over_sqrt2pi 
        * (double(1)/vcl_sqrt(sum_var)) 
        * vcl_exp(-(double(1.0)/double(2.0)) * vcl_pow((g.distribution(i).mean()- g.distribution(j).mean()),2)
        * (double(1)/sum_var));
    }

    for(unsigned j = 0; j < f.num_components(); j++)
    {
      double sum_var = g.distribution(i).var() + f.distribution(j).var();
      t3 = t3 + g.weight(i)*f.weight(j)
        * vnl_math::one_over_sqrt2pi
        * (double(1)/vcl_sqrt(sum_var))
        * vcl_exp(-(double(1)/double(2))* vcl_pow((g.distribution(i).mean() - f.distribution(j).mean()), 2)
        * (double(1)/sum_var));
    }

  }

  //terms 4,5
  for(unsigned i = 0; i < f.num_components(); i++)
  {
    t4 = t4 + vcl_pow(f.weight(i),2)*k1*(1/vcl_sqrt(f.distribution(i).var()));

    for(unsigned j = i+1; j < f.num_components(); j++)
    {
      double sum_var = f.distribution(i).var() + f.distribution(j).var();

      t5 = t5 + f.weight(i)*f.weight(j)
        * vnl_math::one_over_sqrt2pi
        * (double(1)/vcl_sqrt(sum_var))
        *  vcl_exp(-(double(1)/double(2))*vcl_pow(f.distribution(i).mean() - f.distribution(j).mean(),2) 
        * (double(1)/sum_var));
    }
  }

  return vcl_sqrt(t1 +2*t2 - 2*t3 + t4 + 2*t5); 

}

//: Calculate l2-norm between two gaussian pdfs
float bseg3d_gmm_l2norm::l2_gauss2mix(gauss_type const&g, mix_gauss_undef const&f, bool normalize)
{
  //define some constants so that they are only calculated once
  //srqt(4*pi)
  double k1 = (double(1.0)/double(4.0)) * vnl_math::two_over_sqrtpi;

  // This distance is composed of 5 terms
  double t1 = 0;
  double t2 = 0;
  double t3 = 0;
  double t4 = 0;
  double t5 = 0;

  //terms 1,2,3 


  t1 = k1 * (1/vcl_sqrt(g.var()));

  for(unsigned j = 0; j < f.num_components(); j++)
  {
    double sum_var = g.var() + f.distribution(j).var();
    t3 = t3 + f.weight(j)
      * vnl_math::one_over_sqrt2pi
      * (double(1)/vcl_sqrt(sum_var))
      * vcl_exp(-(double(1)/double(2))* vcl_pow((g.mean() - f.distribution(j).mean()), 2)
      * (double(1)/sum_var));
  }



  //terms 4,5
  for(unsigned i = 0; i < f.num_components(); i++)
  {
    t4 = t4 + vcl_pow(f.weight(i),2)*k1*(1/vcl_sqrt(f.distribution(i).var()));

    for(unsigned j = i+1; j < f.num_components(); j++)
    {
      double sum_var = f.distribution(i).var() + f.distribution(j).var();

      t5 = t5 + f.weight(i)*f.weight(j)
        * vnl_math::one_over_sqrt2pi
        * (double(1)/vcl_sqrt(sum_var))
        *  vcl_exp(-(double(1)/double(2))*vcl_pow(f.distribution(i).mean() - f.distribution(j).mean(),2) 
        * (double(1)/sum_var));
    }
  }
  if(normalize)
    return float(vcl_sqrt(t1 - 2*t3 + t4 + 2*t5))/float(vcl_sqrt(t1 + t4 + 2*t5)); 
  else
    return float(vcl_sqrt(t1 - 2*t3 + t4 + 2*t5));


}

//: Calculate l2-norm between a mixture and gaussian pdfs
float bseg3d_gmm_l2norm::l2_gauss(gauss_type const&g1, gauss_type const& g2, bool  normalize)
{
  //define some constants so that they are only calculated once
  //srqt(4*pi)
  float k = (1.0f/4.0f) * float(vnl_math::two_over_sqrtpi);
  float t1 = k*(1.0f/vcl_sqrt(g1.var()));
  float t2 = k*(1.0f/vcl_sqrt(g2.var()));
  float t3 = float(vnl_math::one_over_sqrt2pi)
    * (1.0f/vcl_sqrt(g1.var() + g2.var()))
    *  vcl_exp(-(1.0f/2.0f)*vcl_pow(g1.mean() - g2.mean(),2) 
    * (1.0f/(g1.var() + g2.var())));

  if (normalize)
    return vcl_sqrt(t1 + t2 - 2*t3)/vcl_sqrt(t1 + t2);

  else 
    return vcl_sqrt(t1 + t2 - 2*t3);

}


//: Calculates KL divergence for two gaussian distributions KL(g1|g2)
float bseg3d_gmm_l2norm::kl_distance(gauss_type const&g1, gauss_type const& g2)
{
  double t1 = (2.0)*vcl_log(vcl_sqrt(double(g2.var())/double(g1.var())));
  double t2 = double(g1.var())/double(g2.var());
  double t3 = vcl_pow((double(g2.mean()) - double(g1.mean())),2);
  if(t1+t2+t3<1.0)
    vcl_cout<<"smaller than 0 \n";
  return 0.5*(t1+t2+t3 -1.0);
}

//:Calculates symmetric version of KL divergence KL_sym(g1,g2) = 1/2(KL(g1|g2)+ KL(g2|g1))
float bseg3d_gmm_l2norm::kl_symmetric_distance(gauss_type const&g1, gauss_type const& g2)
{
  return float(0.5)*(kl_distance(g1,g2)+kl_distance(g2,g1));
}
