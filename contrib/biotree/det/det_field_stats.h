// This is /contrib/biotree/det/det_field_stats.h

#ifndef det_field_stats_h_
#define det_field_stats_h_

//:
// \file    det_field_stats.h
// \brief   Field Statistics
// \author  H. Can Aras 
// \date    2005-05-02
//

#include <vcl_vector.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matlab_filewrite.h>

#include <xmvg/xmvg_filter_response.h>
#include <xscan/xscan_dummy_scan.h>
#include <xmvg/xmvg_no_noise_filter_descriptor.h>
#include <xmvg/xmvg_no_noise_filter_3d.h>
#include <xmvg/xmvg_composite_filter_3d.h>
#include <proc/bioproc_dummy_splat_resource.h>
#include <proc/bioproc_filtering_proc.h>

class det_field_stats
{
public:
  //: default constructor
  det_field_stats();
  //: constructor from a vector of responses
  det_field_stats(vcl_vector< xmvg_filter_response<double> > responses);
  //: take a vector of points, compute responses at these, and populate responses member
  void compute_responses_at(xscan_dummy_scan scan, vgl_box_3d<double> box);
  //: write responses as .mat file
  void write_responses(char *filename);

public:
  vcl_vector< xmvg_filter_response<double> > responses_;
};

#endif
