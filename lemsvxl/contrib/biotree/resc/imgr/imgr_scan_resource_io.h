#ifndef imgr_bounded_scan_h_
#define imgr_bounded_scan_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Resource io for a multiview scan with file-based images
// \author J. L. Mundy
// \date Mar. 06, 2005
// \verbatim
// the file path should be of the form <path>filename####.tif
// where the #### symbols will be replaced by incrementing unsigned values
// padded by zeros. This format will handle the typical case where image
// files are numbered sequentially. The scan class provides the path to 
// the images.
// \endverbatim
// 
// This code was adapted from vil3d_file_format methods.
#include <imgr/imgr_scan_resource.h>
#include <imgr/imgr_dummy_scan.h>
#include <xscan/xscan_scan.h>

class imgr_scan_resource_io 
{
 protected:
  //:only static methods so default constructor shouldn't be used
  imgr_scan_resource_io();
 public:
  ~imgr_scan_resource_io();

  //: reading the image resource
  static imgr_scan_resource_sptr read_resource(xscan_scan const& scan);
};

#endif // imgr_bounded_scan_h_
