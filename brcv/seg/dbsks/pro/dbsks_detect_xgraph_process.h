// This is dbsks/pro/dbsks_detect_xgraph_process.h
#ifndef dbsks_detect_xgraph_process_h_
#define dbsks_detect_xgraph_process_h_

//:
// \file
// \brief 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Nov 2, 2008
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>


#include <vil/vil_image_resource_sptr.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>

//: 
class dbsks_detect_xgraph_process : public bpro1_process 
{

public:
  //: Constructor
  dbsks_detect_xgraph_process();
  
  //: Destructor
  virtual ~dbsks_detect_xgraph_process();

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
};


// -----------------------------------------------------------------------------
//: Detect an object, represented as a shock graph, in an image
bool dbsks_detect_xgraph_in_image(vil_image_resource_sptr& image_resource, 
                                  vcl_vector<dbsksp_xshock_graph_sptr >& list_xgraphs);


#endif
