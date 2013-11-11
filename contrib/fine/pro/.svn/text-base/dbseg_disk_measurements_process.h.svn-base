//This is contrib/fine/pro/dbseg_disk_measurements_process.h

#ifndef dbseg_disk_measurements_process_h_
#define dbseg_disk_measurements_process_h_

//:
// \file
// \brief A process for finding measurements relating to disk volume of a sequence of spine segmentations
// \author Eli Fine
// \date Jan 20, 2009
// \verbatim
//  Modifications:
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <structure/dbseg_seg_storage_sptr.h>
#include <structure/dbseg_seg_storage.h>
#include <structure/dbseg_seg_object.h>

//: This process analyzes a sequence of segmentation objects
class dbseg_disk_measurements_process : public bpro1_process
{
public:
  dbseg_disk_measurements_process();
  virtual ~dbseg_disk_measurements_process();

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

#endif // dbseg_disk_measurements_process_h_


