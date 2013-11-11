#ifndef psm_opt_alpha_cost_function2_h_
#define psm_opt_alpha_cost_function2_h_

#include <vcl_vector.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/vnl_cost_function.h>

#include <psm/psm_apm_traits.h>
#include <psm/psm_scene.h>
//#include <psm/psm_cell_enumerator.h>

#include "psm_opt_sample.h"

template <psm_apm_type APM>
class psm_opt_alpha_cost_function2 : public vnl_least_squares_function
{
public:
  psm_opt_alpha_cost_function2(psm_sample<APM> &cell, vcl_vector<psm_opt_sample<typename psm_apm_traits<APM>::obs_datatype> > const& data, double min_sigma);

  virtual void f(vnl_vector<double> const& x, vnl_vector<double>& fx);

  virtual void gradf(vnl_vector<double> const&x, vnl_matrix<double> &J);

  void cell_to_x(psm_sample<APM> const& cell, vnl_vector<double> &x);

  void x_to_cell(vnl_vector<double> const& x, psm_sample<APM> &cell_val);

protected:

  //: the cell whose parameters are being optimized
  psm_sample<APM> &cell_;

  //: the edges used to drive the optimization
  vcl_vector<psm_opt_sample<typename psm_apm_traits<APM>::obs_datatype> > const& data_;

  //: the damping value to control cell value changes between iterations
  const double damp_;

  //: deflation term to make sure (completely) occluded cells converge to zero probability
  const double alpha_deflate_;

  //: constant in residual function (maxP_ - P) / maxP_
  const double maxP_;

};


#endif

