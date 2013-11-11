// This is dbsksp/pro/dbsksp_edit_distance_process.h
#ifndef dbsksp_edit_distance_process_h_
#define dbsksp_edit_distance_process_h_

//:
// \file
// \brief Compute edit distance between two xgraphs
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Nov 18, 2009
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

//: Process that builds a shock graph from a vsol polyline 
class dbsksp_edit_distance_process : public bpro1_process 
{
public:
  //: Constructor
  dbsksp_edit_distance_process();
  
  //: Destructor
  virtual ~dbsksp_edit_distance_process();

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
};

#endif
