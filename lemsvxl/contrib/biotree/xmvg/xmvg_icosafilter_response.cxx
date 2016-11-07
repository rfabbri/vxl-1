#include <xmvg/xmvg_icosafilter_response.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vcl_list.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>

// max eigen value of the noise covariance matrix is multiplied by a big number to get 
// smaller results at the end as strength
double xmvg_icosafilter_response::XMVG_ICOSAFILTER_MAX_EIG_VAL = 2.02834*100000000.0;

double xmvg_icosafilter_response::XMVG_ICOSAFILTER_NOISE_THRESH = 214.0; //65.0;

xmvg_icosafilter_response::xmvg_icosafilter_response(unsigned const & size, double const &val)
: xmvg_filter_response<double>(size, val)
{
  normalize();
}


xmvg_icosafilter_response::xmvg_icosafilter_response(const vnl_vector<double> & res)
: xmvg_filter_response<double>(res)
{
  normalize();
}

xmvg_icosafilter_response::xmvg_icosafilter_response(xmvg_icosafilter_response const& resp)
:xmvg_filter_response<double>(resp)
{
}

xmvg_icosafilter_response::xmvg_icosafilter_response(xmvg_filter_response<double> const& resp)
:xmvg_filter_response<double>(resp)
{
}

xmvg_icosafilter_response::~xmvg_icosafilter_response(void)
{
}

void xmvg_icosafilter_response::normalize(void) {
  // get the sum of the vector values
  double sum = 0.0;
  double max = 0.0;
  for (unsigned i=0; i < this->size(); i++) {
    double val = vcl_fabs(res_[i]);
    sum += val;
    if (val > max)
      max = val;
  }
  
  if (sum != 0.0) {
    for (unsigned i=0; i < res_.size(); i++) {
      //double x = res_[i];
      //x = vcl_fabs(res_[i]);
      //x = x/max;
      //x *= 2.25;
      set(i, (res_[i]/sum));
    }
  }
}


