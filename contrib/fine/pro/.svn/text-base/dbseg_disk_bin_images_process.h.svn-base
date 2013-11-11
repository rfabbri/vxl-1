//This is contrib/fine/pro/dbseg_disk_bin_images_process.h

#ifndef dbseg_disk_bin_images_process_h_
#define dbseg_disk_bin_images_process_h_

//:
// \file
// \brief A process for saving all the binary images of a sequence of spine segmentations (disks, bones, protrusions, spine)
// \author Eli Fine
// \date Feb 19, 2009
// \verbatim
//  Modifications:
// \endverbatim

#include <sstream>

#include <bpro1/bpro1_process.h>
#include <structure/dbseg_seg_storage_sptr.h>
#include <structure/dbseg_seg_storage.h>
#include <structure/dbseg_seg_object.h>

#include <vil/vil_save.h>

//: This process analyzes a sequence of segmentation objects
class dbseg_disk_bin_images_process : public bpro1_process
{
public:
  dbseg_disk_bin_images_process();
  virtual ~dbseg_disk_bin_images_process();

  //: Clone the process
  virtual bpro1_process* clone() const;
  
  //: The name of the video process
  virtual vcl_string name();

  //: Returns the number of frames needed for input
  // (prior to and including the current frame)
  virtual int input_frames();
  
  //: Returns the number of frames used for output
  // (prior to and including the current frame)
  virtual int output_frames();

  //: Returns a vector of strings describing the required storage class input types
  virtual vcl_vector< vcl_string > get_input_type();
  
  //: Returns a vector of strings describing the storage class output types produced
  virtual vcl_vector< vcl_string > get_output_type();

  //: Execute the process on using the storage classes already set
  virtual bool execute();
  
  //: Perform any clean up or final computation after computing on all frames
  virtual bool finish();

};

#endif // dbseg_disk_bin_images_process_h_


