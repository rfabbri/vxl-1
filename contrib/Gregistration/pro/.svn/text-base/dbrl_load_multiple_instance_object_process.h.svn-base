/**************************************************************/
/*  Name: Vishal Jain (vj)
//  File: dbrl_load_multiple_instance_object_process.h
//  Asgn: bvis
//  Date: Mon July 12 16:01:53 EDT 2003
***************************************************************/

#ifndef dbrl_load_multiple_instance_object_process_header
#define dbrl_load_multiple_instance_object_process_header

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

#include <vcl_vector.h>
#include <vsol/vsol_point_2d.h>


class dbrl_load_multiple_instance_object_process : public bpro1_process {

public:

  dbrl_load_multiple_instance_object_process();
  ~dbrl_load_multiple_instance_object_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();
  
    virtual void clear_output(int resize = -1);

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();


int  num_frames_;

};

#endif
