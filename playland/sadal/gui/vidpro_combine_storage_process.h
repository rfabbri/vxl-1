// This is playland/sadal/vidpro_combine_storage_process.h
#ifndef vidpro_combine_storage_process_h_
#define vidpro_combine_storage_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Combines storage classes in one frame into one 
//
// \author
//   Sancar Adali(sadali@lems.brown.edu)
//
// \verbatim
//  Modifications:
//   Name         Date        Changes made
// \endverbatim
//--------------------------------------------------------------------------------


#include <vidpro/vidpro_process.h>
// List other include files here, but only those needed


//: Description of this process
class vidpro_combine_storage_process : public vidpro_process 
{
public:

  //: Constructor
  vidpro_combine_storage_process();
  //: Destructor
  virtual ~vidpro_combine_storage_process();

  virtual vidpro_process * clone() const;

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


#endif // vidpro_combine_storage_process_h_
