// This is brcv/seg/dbdet/pro/dbmcs_nearest_edgels_process.h
#ifndef dbmcs_nearest_edgels_process_h_
#define dbmcs_nearest_edgels_process_h_

//:
// \file
// \brief Process to generate nearest edgels to polyline
// \author Ricardo Fabbri
// \date Tue May  9 10:00:51 EDT 2006
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

class dbmcs_nearest_edgels_process : public bpro1_process 
{
public:

  dbmcs_nearest_edgels_process();
  virtual ~dbmcs_nearest_edgels_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

};

#endif
