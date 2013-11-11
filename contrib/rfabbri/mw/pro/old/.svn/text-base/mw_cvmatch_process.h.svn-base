#ifndef mw_cvmatch_process_header
#define mw_cvmatch_process_header
//:
//\file
//\brief Multiview curve matching process
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 05/05/2005 08:38:20 PM EDT
//

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>


#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <dbcvr/pro/dbcvr_curvematch_storage.h>
#include <dbcvr/pro/dbcvr_curvematch_storage_sptr.h>
#include <dvpgl/pro/dvpgl_camera_storage.h>

#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>

class mw_cvmatch_process : public bpro1_process 
{
public:
  mw_cvmatch_process();
  virtual ~mw_cvmatch_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name() {
    return "Multiview Curve Matching";
  }

  vcl_vector< vcl_string > get_input_type() {
    vcl_vector< vcl_string > input_types;
    input_types.push_back( "vsol2D" );
    input_types.push_back( "vpgl perspective camera" );
//    input_types.push_back( "image" );
    return input_types;
  }
  vcl_vector< vcl_string > get_output_type() {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "curvematch" );
    return to_return;
  }

  int input_frames() {
    return 2;
  }
  int output_frames() {
    return 1;
  }

  bool execute();
  bool finish() {
    return true;
  }

  
protected:

private:

};

#endif
