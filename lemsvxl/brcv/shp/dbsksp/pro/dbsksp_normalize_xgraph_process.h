// This is dbsksp/pro/dbsksp_normalize_xgraph_process.h
#ifndef dbsksp_normalize_xgraph_process_h_
#define dbsksp_normalize_xgraph_process_h_

//:
// \file
// \brief A process to normalize an xgraph around the center of its bounding box
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Mar 27, 2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vector>
#include <string>

//: Process that builds a shock graph from a vsol polyline 
class dbsksp_normalize_xgraph_process : public bpro1_process 
{
public:
  //: Constructor
  dbsksp_normalize_xgraph_process();
  
  //: Destructor
  virtual ~dbsksp_normalize_xgraph_process();

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

protected:
};

#endif
