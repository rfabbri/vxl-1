/**************************************************************/
/*  Name: Vishal Jain (vj)
//  File: edgemodel_image_to_vsol_process..h
//  Asgn: bvis
//  Date: Mon July 12 16:01:53 EDT 2003
***************************************************************/

#ifndef edgemodel_image_to_vsol_process_header
#define edgemodel_image_to_vsol_process_header

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

#include <vcl_vector.h>
#include <vsol/vsol_point_2d.h>


class edgemodel_image_to_vsol_process : public bpro1_process {

public:

  edgemodel_image_to_vsol_process();
  ~edgemodel_image_to_vsol_process();

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
