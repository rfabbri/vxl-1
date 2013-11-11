// This is brcv/shp/dbskfg/pro/dbskfg_form_containment_graph_process.h
#ifndef dbskfg_form_containment_graph_process_h_
#define dbskfg_form_containment_graph_process_h_

//:
// \file
// \brief This process forms a containment graph data structure from
//        a vsol2d storage
//
// \author Maruthi Narayanan
// \date 07/07/10
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

class dbskfg_form_containment_graph_process : public bpro1_process 
{

public:
  //: Constructor
  dbskfg_form_containment_graph_process();
  
  //: Destructor
  virtual ~dbskfg_form_containment_graph_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();
  
};

#endif  //dbskfg_form_containment_graph_process_h_
