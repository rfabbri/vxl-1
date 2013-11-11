// This is contrib/fine/pro/dbseg_jseg_process.h

#ifndef dbseg_jseg_process_h_
#define dbseg_jseg_process_h_

//:
// \file
// \brief A process for running the JSEG segmentation algorithm on an image
// \author Eli Fine
// \date 8/25/08
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>
#include <vcl_list.h>
#include <vul/vul_timer.h>

#include <vcl_iostream.h>
#include <bpro1/bpro1_parameters.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>

#include <algos/jseg/dbseg_jseg_segment.h>
#include <algos/jseg/dbseg_jseg_ioutil.h>
#include <algos/jseg/dbseg_jseg_imgutil.h>
#include <algos/jseg/dbseg_jseg_mathutil.h>
#include <algos/jseg/dbseg_jseg_quan.h>
#include <algos/jseg/dbseg_jseg_memutil.h>

#include <vector>
using namespace std;


class dbseg_jseg_process : public bpro1_process
{
 public:

  dbseg_jseg_process();
 ~dbseg_jseg_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  void clear_output();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();


  static vil_image_view<vxl_byte> dbseg_jseg_process::static_execute(vil_image_view<vxl_byte> imageView, float TQUAN, float threshcolor, int scale);

};

#endif // dbseg_jseg_process


