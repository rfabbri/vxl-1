#ifndef xmvg_icosafilter_response_h
#define xmvg_icosafilter_response_h
//: 
// \file     xmvg_icosafilter_response.h
// \brief    define an icosahedron filter response and some operations on
// \         to determine if the filter response is a cylinder, a sphere 
// \         or noise
// \author   Gamze Tunali
// \date     2005-09-20
// 
#include <xmvg/xmvg_filter_response.h>
#include <det/det_cylinder_map.h>
#include <vcl_string.h>

class xmvg_icosafilter_response: public xmvg_filter_response<double>
{
public:
  //: max eigenvalue of the covariance matrix of the filter responses
  // on a given noise area
  static double XMVG_ICOSAFILTER_MAX_EIG_VAL;

  //: noise threshold to be used at determining the voxel type either
  // noise or cylinder
  static double XMVG_ICOSAFILTER_NOISE_THRESH;

  const static int XMVG_ICOSAFILTER_SIZE = 6;

  xmvg_icosafilter_response(unsigned const & size = 0, double const &val = 0);

  xmvg_icosafilter_response(const vnl_vector<double> & res);

  xmvg_icosafilter_response(xmvg_icosafilter_response const& resp);

  xmvg_icosafilter_response(xmvg_filter_response<double> const& resp);

  ~xmvg_icosafilter_response(void);

  void normalize(void);

  void set(int i, double val) { res_[i] = val; }

  double min_value() { return res_.min_value(); }

  double max_value() { return res_.max_value(); }
  
};

#endif
