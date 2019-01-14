// This is edgemodeling/pro/dbrl_edge_point_tangent_fg_detect_process.h
#ifndef dbrl_edge_point_tangent_fg_detect_process_h_
#define dbrl_edge_point_tangent_fg_detect_process_h_

//:
// \file
// \brief Process that detects foreground using a mixture model
// \author Vishal Jain (vj@lems.brown.edu)
// \date 08/30/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vector>
#include <string>

//: Derived video process class for detecting foreground
class dbrl_edge_point_tangent_fg_detect_process : public bpro1_process {

public:

  dbrl_edge_point_tangent_fg_detect_process();
  virtual ~dbrl_edge_point_tangent_fg_detect_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  std::string name();

  int input_frames();
  int output_frames();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  bool execute();
  bool finish();

};

#endif
