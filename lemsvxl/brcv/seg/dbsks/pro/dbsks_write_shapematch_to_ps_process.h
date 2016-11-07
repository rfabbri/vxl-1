// This is dbsks/pro/dbsks_write_shapematch_to_ps_process.h
#ifndef dbsks_write_shapematch_to_ps_process_h_
#define dbsks_write_shapematch_to_ps_process_h_

//:
// \file
// \brief 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Mar 9, 2008
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>
//#include <vil/vil_image_resource_sptr.h>
#include <dbsksp/dbsksp_shock_graph_sptr.h>



//: Process to write results of a shapematch to a postscript file
class dbsks_write_shapematch_to_ps_process : public bpro1_process 
{

public:
  //: Constructor
  dbsks_write_shapematch_to_ps_process();
  
  //: Destructor
  virtual ~dbsks_write_shapematch_to_ps_process();

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

  static bool save_ps_file(const vcl_string& out_ps_file,
    const vcl_string& shapematch_file, 
    const vcl_string& image_file,
    const dbsksp_shock_graph_sptr& graph);

};

#endif
