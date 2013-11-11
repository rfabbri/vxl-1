// This is dbknee/pro/dbknee_plot_cylinder_cs_process.h
#ifndef dbknee_plot_cylinder_cs_process_h_
#define dbknee_plot_cylinder_cs_process_h_

//:
// \file
// \brief A process to regional thickness given a thickness map

// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Mar 9, 2007
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h> 

//: Process that builds a shock graph from a vsol polyline 
class dbknee_plot_cylinder_cs_process : public bpro1_process 
{

public:
  //: Constructor
  dbknee_plot_cylinder_cs_process();
  
  //: Destructor
  virtual ~dbknee_plot_cylinder_cs_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  //: Returns the name of this process
  vcl_string name() {  return "Plot cylinder CS"; }

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();


  

};

#endif
