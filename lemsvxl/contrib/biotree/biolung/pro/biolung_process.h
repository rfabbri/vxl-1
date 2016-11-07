// This is contrib/biotree/biolung/pro/biolung_process.h

#ifndef biolung_process_h_
#define biolung_process_h_

//:
// \file
// \brief Generates a realistic 3D lung model and generates volumetric data for it 
// \author Rahul Gautam (corresponding author) & Amir Tamrakar
// \date 07/14/04
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>

class biolung_process : public bpro1_process
{
 public:
  
  biolung_process();
 ~biolung_process();

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

};

#endif // biolung_process_h_
