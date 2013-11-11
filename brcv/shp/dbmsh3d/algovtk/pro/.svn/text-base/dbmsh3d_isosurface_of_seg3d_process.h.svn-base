//This is dbmsh3d/algovtk/pro/dbmsh3d_isosurface_of_seg3d_process.h
#ifndef dbmsh3d_isosurface_of_seg3d_process_h_
#define dbmsh3d_isosurface_of_seg3d_process_h_

//:
// \file
// \brief A process for loading a .bnd file into the current frame
// \author Nhon Trinh
// \date Oct 5, 2006
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vcl_vector.h>

//: This process loads a .bnd file into a vidpro1_vsol_storage class
class dbmsh3d_isosurface_of_seg3d_process : public bpro1_process
{
public:
  dbmsh3d_isosurface_of_seg3d_process();
  virtual ~dbmsh3d_isosurface_of_seg3d_process();

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

#endif // dbmsh3d_isosurface_of_seg3d_process_h_
