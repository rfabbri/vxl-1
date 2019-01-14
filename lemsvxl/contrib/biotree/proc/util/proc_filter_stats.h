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
#include <vector>
#include <string>

class proc_filter_stats {
public:
  proc_filter_stats(){};
  ~proc_filter_stats(){};

  bool max(std::vector<xmvg_filter_response<double> > & responses,
                        xmvg_filter_response<double> &max);

  bool mean(std::vector<xmvg_filter_response<double> > & responses,
                        xmvg_filter_response<double> &mean_vals);

  double covar(std::vector<xmvg_filter_response<double> > & responses,
                         vnl_matrix<double> &covar_matrix);

  double noise_threshold(std::vector<xmvg_filter_response<double> > & responses,
                                   vnl_matrix<double> const& covar,
                                    double max_covar,
                                    xmvg_filter_response<double> const& mean,
                                    std::string fname);
  
};

#endif
