// This is brl/blem/brcv/mvg/dbirl/pro/dbirl_h_computation_process.h
#ifndef dbirl_h_computation_process_h_
#define dbirl_h_computation_process_h_

//:
// \file
// \brief A bpro1 process that computes homographies from tracks.
// \author Thomas Pollard
// \date Oct 8, 2005
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>


class dbirl_h_computation_process : public bpro1_process
{
public:

  dbirl_h_computation_process();
  ~dbirl_h_computation_process();

  //: Clone the process
  virtual bpro1_process* clone() const; 
  
  //: The name of the video process
  virtual vcl_string name() {return "dbirl_h_computation_process";}

  int input_frames();
  int output_frames();
  
  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();
  
private:

};

#endif // dbirl_h_computation_process_h_
