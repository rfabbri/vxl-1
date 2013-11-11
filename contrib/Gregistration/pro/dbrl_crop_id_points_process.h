/**************************************************************/
/*  Name: Vishal Jain (vj)
//  File: dbil_osl_canny_subpixel_edges_process.h
//  Date:  Nov 28   2005
***************************************************************/

#ifndef dbrl_crop_id_points_process_header
#define dbrl_crop_id_points_process_header

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include <pro/dbrl_id_point_2d_storage.h>
#include <pro/dbrl_id_point_2d_storage_sptr.h>

class dbrl_crop_id_points_process : public bpro1_process {
public:
  dbrl_crop_id_points_process();
  ~dbrl_crop_id_points_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();
  
  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

private:

};

#endif
