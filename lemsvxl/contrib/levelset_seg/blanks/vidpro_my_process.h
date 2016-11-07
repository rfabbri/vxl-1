// This is contrib/levelset_seg/vidpro1_my_process.h
#ifndef vidpro1_my_process_h_
#define vidpro1_my_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Short description of the process
//
// \author
//   Your Name
//
// \verbatim
//  Modifications:
//   Name         Date        Changes made
// \endverbatim
//--------------------------------------------------------------------------------


#include <bpro1/bpro1_process.h>
// List other include files here, but only those needed


//: Description of this process
class vidpro1_my_process : public bpro1_process 
{
public:

  //: Constructor
  vidpro1_my_process();
  //: Destructor
  virtual ~vidpro1_my_process();

  //: The name of this process
  vcl_string name();

  //: Returns the number of input frames to this process
  int input_frames();
  //: Return the number of output frames for this process
  int output_frames();

  //: Returns a vector of strings describing the input types to this process
  vcl_vector< vcl_string > get_input_type();
  //: Returns a vector of strings describing the output types of this process
  vcl_vector< vcl_string > get_output_type();

  //: Execute the process
  bool execute();
  bool finish();

protected:
  // Define anything else you need here

};


#endif // vidpro1_my_process_h_
