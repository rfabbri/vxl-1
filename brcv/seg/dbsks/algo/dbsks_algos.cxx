// This is dbsks/algo/dbsks_algos.cxx

//:
// \file

#include "dbsks_algos.h"


#include <dbsks/dbsks_det_desc_xgraph.h>
#include <dbsks/xio/dbsks_xio_xshock_det.h>

#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
#include <dbsksp/algo/dbsksp_screenshot.h>
#include <bpro1/bpro1_parameters.h>

#include <vsol/vsol_box_2d.h>
#include <vil/vil_save.h>
#include <vnl/vnl_math.h>
#include <vul/vul_file.h>
#include <vul/vul_sprintf.h>





//-------------------------------------------------------------------------------
// Compute detection windows to cover the full image
void dbsks_algos::
compute_detection_windows(int max_window_width, int max_window_height,
                                int image_width, int image_height,
                                vcl_vector<vgl_box_2d<int > >& windows)
{
  windows.clear();

  // hard-coded maximum window size
  int max_width = max_window_width; 
  int max_height = max_window_height;

  int ni = image_width;
  int nj = image_height;

  int cur_xmin = 0;
  do
  {
    int cur_ymin = 0;
    do
    {
      // determine window dimension
      int xmax = vnl_math::min(cur_xmin + max_width-1, ni-1); // padding on right
      int ymax = vnl_math::min(cur_ymin + max_height-1, nj-1); // padding at bottom
      int xmin = cur_xmin; //vnl_math::max(xmax-max_width+1, 0);
      int ymin = cur_ymin; //vnl_math::max(ymax-max_height+1, 0);
      windows.push_back(vgl_box_2d<int >(xmin, xmax, ymin, ymax));

      // update
      cur_ymin += max_height/2;
    }
    while (cur_ymin < (nj-max_height/2)); // padding on top
    cur_xmin += max_width/2;
  }
  while (cur_xmin < (ni-max_width/2)); // padding in front
}


