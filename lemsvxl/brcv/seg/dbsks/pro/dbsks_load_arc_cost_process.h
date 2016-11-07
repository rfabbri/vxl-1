// This is dbsksp/pro/dbsks_load_arc_cost_process.h
#ifndef dbsks_load_arc_cost_process_h_
#define dbsks_load_arc_cost_process_h_

//:
// \file
// \brief 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Nov 24, 2007
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

//: 
class dbsks_load_arc_cost_process : public bpro1_process 
{

public:
  //: Constructor
  dbsks_load_arc_cost_process();
  
  //: Destructor
  virtual ~dbsks_load_arc_cost_process();

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

};

#endif
