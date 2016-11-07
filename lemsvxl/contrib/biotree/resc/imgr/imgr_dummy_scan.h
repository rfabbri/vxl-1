#ifndef imgr_dummy_scan_h_
#define imgr_dummy_scan_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A fake scan class to use until the real one is ready
// \author J.L. Mundy
// \verbatim
// 03/20/05 Kongbin Kang: Change it to use xscan_scan
// \endverbatim

#include <vcl_string.h>
#include <xmvg/xmvg_perspective_camera.h>
#include <xscan/xscan_scan.h>
#include <bio_defs.h>

class imgr_dummy_scan : public xscan_scan 
{
 public:
  imgr_dummy_scan();
  ~imgr_dummy_scan();

 private:
};



#endif // imgr_dummy_scan_h_
