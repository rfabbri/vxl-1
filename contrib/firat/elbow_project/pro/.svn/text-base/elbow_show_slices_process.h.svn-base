// This is dbsksp/pro/elbow_show_slices_process.h
#ifndef elbow_show_slices_process_h_
#define elbow_show_slices_process_h_

//:
// \file
// \brief Process to show 2d slices of the loaded 3D dataset
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
// \date Oct 11, 2011
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

class elbow_show_slices_process : public bpro1_process
{

public:
  //: Constructor
  elbow_show_slices_process();
  
  //: Destructor
  virtual ~elbow_show_slices_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  //: Returns the name of this process
  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();
protected:
  int num_frames_;

};

#endif
