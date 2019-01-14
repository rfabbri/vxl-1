// This is contrib/fine/pro/dbseg_mean_shift_process.h

#ifndef dbseg_mean_shift_process_h_
#define dbseg_mean_shift_process_h_

//:
// \file
// \brief A process for running the means shift segmentation algorithm on an image
// \author Eli Fine
// \date 8/14/08
//
// \verbatim
//  Modifications
// \endverbatim


#include <vector>
#include <string>
#include <bpro1/bpro1_process.h>
#include <list>
#include <vul/vul_timer.h>

#include <iostream>
#include <bpro1/bpro1_parameters.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>

#include <algos/mean_shift/dbseg_mean_shift_msImageProcessor.h>

class dbseg_mean_shift_process : public bpro1_process
{
 public:

  dbseg_mean_shift_process();
 ~dbseg_mean_shift_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  std::string name();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  void clear_output();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

};

#endif // dbseg_mean_shift_process


