//: 
// \file   proc_filter_stats.cxx
// \brief  computes some statistical values on the filter responses like
//         mean, max, covariance etc..
//         
// \author    Gamze D. Tunali
// \date      2005-10-01
// 

#include "proc_filter_stats.h"

#include <vnl/vnl_matrix.h>
#include <vcl_iostream.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vcl_iostream.h>

// finds the maximum of each filter response
bool
proc_filter_stats::max(vcl_vector<xmvg_filter_response<double> > & responses,
                        xmvg_filter_response<double> &max_vect)
{
  double min_double = -1.*vnl_numeric_traits<double>::maxval;
  if (responses.size() > 0) 
    max_vect = xmvg_filter_response<double> (max_vect.size(), min_double);
  else 
    return false;
  
  for(unsigned int i=0; i<responses.size(); i++) {
    xmvg_filter_response<double> resp = responses[i];
    for (unsigned int j=0; j<resp.size(); j++) {
      double max_i = max_vect[j];
      double resp_i = resp[j];
      if (max_i < resp_i)
        max_vect[j] = resp[j];
    }
  }
  return true;
}

// finds the mean of each filter response
bool
proc_filter_stats::mean(vcl_vector<xmvg_filter_response<double> > & responses,
                        xmvg_filter_response<double>  &mean_vals)
{
  if (responses.size() > 0) 
    mean_vals = xmvg_filter_response<double> (mean_vals.size(), 0);
  else 
    return false;
  int num=0;
  /*int d = responses.size();*/
  //unused variable

  for(unsigned int i=0; i<responses.size(); i++) {
    xmvg_filter_response<double> ptr = responses[i];

    // this is to eliminate the all 0 valued responses, which is the result
    // using two processing box while computing the filter responses.. 
    // The outer part of the active box is all 0s.
    if (!ptr.all_zero()) {
      num++;
      for (unsigned int j=0; j<ptr.size(); j++) {
        double val = ptr[j];
        mean_vals[j] += val;
      }
    }
  }

  mean_vals /= num; 
  return true;
}

// computes the covariance matrix
double
proc_filter_stats::covar(vcl_vector<xmvg_filter_response<double> > & responses,
                         vnl_matrix<double> &covar_matrix){
  // check if the there are any responses
  if (responses.size() == 0) 
    return 0;
  
  xmvg_filter_response<double> resp = responses[0];
  xmvg_filter_response<double> mean_vals(resp.size(), 0);
  
  // first compute the vmean values o filters
  mean(responses, mean_vals);

  // compute the variance
  vnl_matrix<double> f(resp.size(), 1, 0.0);
  vnl_matrix<double> f_transp(1, resp.size(), 0.0);
  vnl_matrix<double> covar(resp.size(), resp.size(), 0.0);

  //double var = 0;//unused variable
  for(unsigned int i=0; i<responses.size(); i++) {
    xmvg_filter_response<double> resp = responses[i];
    if (resp.sum() != 0){
    for (unsigned int j=0; j<resp.size(); j++) {
      double diff = resp[j] - mean_vals[j];
      f[j][0] = diff;
    }
    f_transp = f.transpose();
    covar += f*f_transp;
    }
  }

  // divide the variance by the number of samples
  covar_matrix = covar / responses.size();
  vcl_cout << covar_matrix << vcl_endl;
  return 1;
}

// computes some values especially for noise on the filter responses, which is
// for each filter fi --> sqrt(sum((f-f_mean)^tCovar^-1(f-f_mean)))
// the results are written to a file unless fname is "" (null string)
double
proc_filter_stats::noise_threshold(vcl_vector<xmvg_filter_response<double> > & responses,
                                   vnl_matrix<double> const& covar_matrix,
                                   double max_covar,
                                   xmvg_filter_response<double> const& mean,
                                   vcl_string fname) 
{
  vnl_matrix<double> f(6, 1);
  vnl_matrix<double> f_transp(1, 6);
  vnl_matrix<double> e(1, 1);       
  double E=0, max_E=0;
  vnl_matrix<double> covar_inv = vnl_matrix_inverse<double>(covar_matrix);

  FILE *fp=NULL;
  if (!fname.empty())
     fp = fopen(fname.data(), "w");

  for (unsigned int r = 0; r<responses.size(); r++) {
    xmvg_filter_response<double> resp = responses[r];
    for(unsigned int i=0; i<resp.size(); i++){
      f[i][0] = resp[i] - mean[i];
    }

    f_transp = f.transpose();
    e = f_transp*covar_inv*f;
    E = (1.0/max_covar) * e[0][0];   
    E = sqrt(E);
    if (!fname.empty()) 
      fprintf(fp, "%lf\n", E);
    if (max_E < E) {
      max_E = E;
    }
  }
  fclose(fp);
  return max_E;
}
