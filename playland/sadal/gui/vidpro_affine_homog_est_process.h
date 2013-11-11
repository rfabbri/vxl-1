// This is playland/sadal/gui/vidpro_affine_homog_est_process.h
#ifndef vidpro_affine_homog_est_process_h_
#define vidpro_affine_homog_est_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Find affine homographies between given sets of vsol
//
// \author
//   Your Name (sadal@lems.brown.edu)
//
// \verbatim
//  Modifications:
//   Name         Date        Changes made
// \endverbatim
//--------------------------------------------------------------------------------


#include <vidpro/vidpro_process.h>
// List other include files here, but only those needed


//: Description of this process
class dvidpro_affine_homog_est_process : public vidpro_process 
{
public:

  //: Constructor
  dvidpro_affine_homog_est_process();
  //: Destructor
  virtual ~dvidpro_affine_homog_est_process();

  virtual vidpro_process* clone() const;

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


#endif // vidpro_affine_homog_est_process_h_
