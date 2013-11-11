#ifndef proc_filter_stats_h_
#define proc_filter_stats_h_

//: 
// \file   proc_filter_stats.h
// \brief  computes some statistical values on the filter responses
//         
// \author    Gamze D. Tunali
// \date      2005-10-01
// 

#include <xmvg/xmvg_filter_response.h>
#include <vcl_vector.h>
#include <vcl_string.h>

class proc_filter_stats {
public:
  proc_filter_stats(){};
  ~proc_filter_stats(){};

  bool max(vcl_vector<xmvg_filter_response<double> > & responses,
                        xmvg_filter_response<double> &max);

  bool mean(vcl_vector<xmvg_filter_response<double> > & responses,
                        xmvg_filter_response<double> &mean_vals);

  double covar(vcl_vector<xmvg_filter_response<double> > & responses,
                         vnl_matrix<double> &covar_matrix);

  double noise_threshold(vcl_vector<xmvg_filter_response<double> > & responses,
                                   vnl_matrix<double> const& covar,
                                    double max_covar,
                                    xmvg_filter_response<double> const& mean,
                                    vcl_string fname);
  
};

#endif
