// This is dbsksp/pro/dbsksp_morph_shock_graph_different_topology_process.h
#ifndef dbsksp_morph_shock_graph_different_topology_process_h_
#define dbsksp_morph_shock_graph_different_topology_process_h_

//:
// \file
// \brief A process to morph shock graphs 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date April 6, 2007
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

//: Process that builds a shock graph from a vsol polyline 
class dbsksp_morph_shock_graph_different_topology_process : public bpro1_process 
{

public:
  //: Constructor
  dbsksp_morph_shock_graph_different_topology_process();
  
  //: Destructor
  virtual ~dbsksp_morph_shock_graph_different_topology_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  //: Returns the name of this process
  vcl_string name();
  
  //: Clear output
  virtual void clear_output(int resize);

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

  int num_frames() const {return num_frames_; }
  void set_num_frames(int num_frames){ this->num_frames_ = num_frames; }

protected:
  int num_frames_;

};

#endif
