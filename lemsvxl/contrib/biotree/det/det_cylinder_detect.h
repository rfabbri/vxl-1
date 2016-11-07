#ifndef det_cylinder_detect_h
#define det_cylinder_detect_h
//: 
// \file     det_cylinder_detect.h
// \brief    defines some detection methods on filter responses 
//           to determine if the filter response is a noise, a cylinder, a sphere
//           or a plane
// \         or noise
// \author   Gamze Tunali
// \date     2005-09-20
// \verbatim
//  Modifications
//     K. Kang - Jan 16 2006 - add a detection interface from response and 
//    filter description and deprecate the old interface.
//     Gamze Tunali - July 17, 2006 - added a new method - apply() - for 
//    running detection module on the whole cylinder map and returning a
//     Gamze Tunali - Aug 20, 2006 added radius data and not using the noise
//    threshold anymore  
//    a resulting cylinder map
//  \endverbatim
// 

#include <vil3d/vil3d_image_view.h>

#include <xmvg/xmvg_filter_response.h>
#include <xmvg/xmvg_filter_descriptor.h>
#include <xmvg/xmvg_composite_filter_descriptor.h>
#include <det/det_cylinder_map.h>
#include <vcl_string.h>
#include <vil3d/vil3d_image_view.h>

typedef enum  {SPHERE, CYLINDER, NOISE, PLANE} det_icosafilter_resp_types;

class det_cylinder_detect
{
public:
  //: max eigenvalue of the covariance matrix of the filter responses
  // on a given noise area
  static double DET_MAX_EIG_VAL;

  //: noise threshold to be used at determining the voxel type either
  // noise or cylinder
  static double DET_NOISE_THRESH;

  // epsilon value used to compute filter response values at sub positions
  static double EPS;

  det_cylinder_detect();

  ~det_cylinder_detect(void);
 
   //: find out the cylinder by the filter with maximial response
  cme_t det_type(xmvg_filter_response<double> & resp, 
    xmvg_composite_filter_descriptor const& fd,
    double radius=0);

  det_cylinder_map apply(unsigned nx, unsigned ny, unsigned nz,
    vcl_vector<xmvg_filter_response<double> > const& responses,
    xmvg_composite_filter_descriptor const & fds,
    vil3d_image_view<unsigned char>* radius_view=0);

  det_cylinder_map apply(unsigned nx, unsigned ny, unsigned nz,
    vil3d_image_view<float> &directions,
    vil3d_image_view<float> &I_s);

private:
  vcl_vector<vcl_vector<unsigned > *> includes(vcl_vector<vcl_vector<unsigned > *> v, 
    unsigned index);

  void normalize(xmvg_filter_response<double> &resp);

  vcl_vector<vgl_vector_3d<double> > compute_v(xmvg_filter_response<double> &resp,
            vcl_vector<vgl_vector_3d<double> > filter_dir);

  vgl_vector_3d<double> detect_dir(xmvg_filter_response<double>& resp,
                            vcl_vector<vgl_vector_3d<double> > v,
                            vcl_vector<vgl_vector_3d<double> > filter_dir);
};

#endif
