// This is dbsks/pro/dbsks_detect_shape_process.h
#ifndef dbsks_detect_shape_process_h_
#define dbsks_detect_shape_process_h_

//:
// \file
// \brief 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Oct 12, 2007
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vector>
#include <string>


//: 
class dbsks_detect_shape_process : public bpro1_process 
{

public:
  //: Constructor
  dbsks_detect_shape_process();
  
  //: Destructor
  virtual ~dbsks_detect_shape_process();

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
