// This is dbsksp/pro/dbsksp_shock_snake_process.h
#ifndef dbsksp_shock_snake_process_h_
#define dbsksp_shock_snake_process_h_

//:
// \file
// \brief A shock snake that snaps a shock shape to edges of an image 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Oct 8, 2006
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>


#include <dbsksp/dbsksp_shock_edge_sptr.h>
#include <dbsksp/dbsksp_shock_node_sptr.h>
#include <dbsksp/dbsksp_shapelet_sptr.h>

//: A shock snake that snaps a shock shape to edges of an image
class dbsksp_shock_snake_process : public bpro1_process 
{

public:
  //: Constructor
  dbsksp_shock_snake_process();
  
  //: Destructor
  virtual ~dbsksp_shock_snake_process();

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
  dbsksp_twoshapelet_sptr form_twoshapelet(const dbsksp_shock_edge_sptr& e0,
    const dbsksp_shock_edge_sptr& e1,
    dbsksp_shock_node_sptr& node_start);
};

#endif
