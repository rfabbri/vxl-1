// This is brl/dvidpro1/process/dbetl_tracker_process.h
#ifndef dbetl_tracker_process_h_
#define dbetl_tracker_process_h_

//:
// \file
// \brief A process that tracks epipolar points on curves
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 11/22/04
//
// \verbatim
//  Modifications
// \endverbatim


#include <vector>
#include <string>
#include <bpro1/bpro1_process.h>

#include <dbetl/dbetl_tracker.h>

//: A process that tracks epipolar points on curves
class dbetl_tracker_process : public bpro1_process
{
public:

  dbetl_tracker_process();
  ~dbetl_tracker_process();

  dbetl_tracker_process(const dbetl_tracker_process& other);

  //: Clone the process
  virtual bpro1_process* clone() const;
  
  std::string name();

  int input_frames();
  int output_frames();
  
  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  bool execute();
  bool finish();

private:
  dbetl_tracker* tracker_;

};

#endif // dbetl_tracker_process_h_
