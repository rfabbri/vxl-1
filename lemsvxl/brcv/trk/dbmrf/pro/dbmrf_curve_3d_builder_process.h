// This is brcv/trk/dbmrf/pro/dbmrf_curve_3d_builder_process.h
#ifndef dbmrf_curve_3d_builder_process_h_
#define dbmrf_curve_3d_builder_process_h_

//:
// \file
// \brief A process that builds a set of bmrf_curvel_3d from a bmrf_network
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 3/24/04
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>

#include <bpro1/bpro1_parameters.h>
#include <bmrf/bmrf_network_sptr.h>
#include "dbmrf_curvel_3d_storage_sptr.h"


class dbmrf_curve_3d_builder_process : public bpro1_process
{
public:

  dbmrf_curve_3d_builder_process();
  ~dbmrf_curve_3d_builder_process();

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

#endif // dbmrf_curve_3d_builder_process_h_
