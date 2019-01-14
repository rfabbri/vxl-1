// This is dbsks/pro/dbsks_local_match_process.h
#ifndef dbsks_local_match_process_h_
#define dbsks_local_match_process_h_

//:
// \file
// \brief 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Feb 11, 2008
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vector>
#include <string>


//: 
class dbsks_local_match_process : public bpro1_process 
{

public:
  //: Constructor
  dbsks_local_match_process();
  
  //: Destructor
  virtual ~dbsks_local_match_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  //: Returns the name of this process
  std::string name();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();
};

#endif
