#ifndef psm_opt_alpha_cost_function2_txx_
#define psm_opt_alpha_cost_function2_txx_

#include <vcl_cmath.h>
#include <vcl_vector.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/vnl_cost_function.h>

#include <psm/psm_apm_traits.h>
#include <psm/psm_sample.h>

#include "psm_opt_sample.h"
#include "psm_opt_alpha_cost_function2.h"


template<psm_apm_type APM>
psm_opt_alpha_cost_function2<APM>::psm_opt_alpha_cost_function2(psm_sample<APM> &cell, vcl_vector<psm_opt_sample<typename psm_apm_traits<APM>::obs_datatype> > const& data,  double min_sigma)
: vnl_least_squares_function(1, data.size(), use_gradient), cell_(cell), data_(data), 
damp_(0.5), alpha_deflate_(1e-5), maxP_(vcl_pow(vnl_math::one_over_sqrt2pi / min_sigma, (double)psm_apm_traits<APM>::obs_dim)) {}

template<psm_apm_type APM>
void psm_opt_alpha_cost_function2<APM>::f(vnl_vector<double> const& x, vnl_vector<double>& fx)
{
  double alpha = x[0];
  if (alpha < 0.0) {
    alpha = 0.0;
  }

  typename vcl_vector<psm_opt_sample<typename psm_apm_traits<APM>::obs_datatype> >::const_iterator data_it = data_.begin();
  vnl_vector<double>::iterator fx_it = fx.begin();

  for (; data_it != data_.end(); ++data_it, ++fx_it) {
    *fx_it = 0.0;
    double PI =  data_it->PI_;
    double pass_prob = vcl_exp(-alpha * data_it->seg_len_);
    double pass_prob_diff = pass_prob - vcl_exp(-cell_.alpha * data_it->seg_len_);

    double Pt = data_it->pre_ + PI*data_it->vis_ + pass_prob*data_it->vis_*(data_it->post_ - PI);

    //*fx_it = A_ / (A_ + Pt);
    *fx_it = (maxP_ - Pt);// / maxP_;

    if (*fx_it < 0.0){
      vcl_cerr << vcl_endl << "ERROR: residual value " << *fx_it << "is less than 0" << vcl_endl;
      *fx_it = 0;
    }

    *fx_it += alpha_deflate_*alpha*data_it->seg_len_ + damp_*pass_prob_diff*pass_prob_diff;

    if (x[0] < 0.0) {
      // penalize alpha values < 0
      *fx_it += x[0]*x[0];
    }
    //vcl_cout << "fx = " << *fx_it;
    //*fx_it *= (data_it->seg_len_*0.1);
    //vcl_cout << "  fx scaled = " << *fx_it << vcl_endl;
  }
  //vcl_cout << "f(" << x[0] << ") = " << fx << vcl_endl;
}



template<psm_apm_type APM>
void psm_opt_alpha_cost_function2<APM>::gradf(vnl_vector<double> const&x, vnl_matrix<double> &J)
{
  double alpha = x[0];
  if (alpha < 0.0) {
    alpha = 0.0;
  }
  typename vcl_vector<psm_opt_sample<typename psm_apm_traits<APM>::obs_datatype> >::const_iterator data_it = data_.begin();

  for (unsigned int i=0; i < get_number_of_residuals(); ++i, ++data_it) {
    // force alpha to be non-negative
    if (x[0] < 0) {
      J[i][0] = 2.0*x[0];
    }
    else {
      double PI = data_it->PI_;
      double pass_prob = vcl_exp(-alpha * data_it->seg_len_);
      double pass_prob_diff = pass_prob - vcl_exp(-cell_.alpha * data_it->seg_len_);
      double Pt = data_it->pre_ + PI*data_it->vis_ + pass_prob*data_it->vis_*(data_it->post_ - PI);

      // compute d_f / d_Pt
      //double df_dPt = -A_ / ((A_ + Pt)*(A_ + Pt));
      double df_dPt = -1.0;// / maxP_;
      // compute d_Pt / d_alpha
      double dPt_dalpha = -data_it->seg_len_ * pass_prob  * data_it->vis_ * (data_it->post_ - PI);

      // total partial is product of two
      double partial = df_dPt * dPt_dalpha;

      // add terms for damping and alpha penalty
      partial += alpha_deflate_*data_it->seg_len_ - 2.0*damp_*pass_prob_diff*data_it->seg_len_*pass_prob;
      J[i][0] = partial;
    }
  }
  //vcl_cout << "J(" << x[0] << ")= " << J << vcl_endl;
  return;
}


template <psm_apm_type APM>
void psm_opt_alpha_cost_function2<APM>::cell_to_x(psm_sample<APM> const& cell, vnl_vector<double> &x)
{
  x[0] = cell.alpha;
}

template <psm_apm_type APM>
void psm_opt_alpha_cost_function2<APM>::x_to_cell(vnl_vector<double> const& x, psm_sample<APM> &cell)
{
  cell.alpha = (float)x[0];

  if (cell.alpha < 0.0f) {
    cell.alpha = 0.0f;
  }
  if (cell.alpha > 1e3) {
    cell.alpha = 1e3;
  }
  return;
}

#define PSM_OPT_ALPHA_COST_FUNCTION2_INSTANTIATE(T) \
  template class psm_opt_alpha_cost_function2<T>


#endif
