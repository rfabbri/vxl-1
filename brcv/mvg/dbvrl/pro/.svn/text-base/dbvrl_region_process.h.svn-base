// This is brcv/mvg/dbvrl/pro/dbvrl_region_process.h
#ifndef dbvrl_region_process_h_
#define dbvrl_region_process_h_

//:
// \file
// \brief Process that computes planar transformations between images
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 4/26/04
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

//: Derived video process class for computing transformations
class dbvrl_region_process : public bpro1_process {

public:

  dbvrl_region_process();
  virtual ~dbvrl_region_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

};

#endif //dbvrl_region_process_h_
