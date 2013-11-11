/*************************************************************************
 *    NAME: pradeep yarlagadda
 *    FILE: Flip_spoke_process.h
 *    DATE: Mon September 17 2007

 // brief:  
 //process to flip the spokes of a vehicle model

 *************************************************************************/

#ifndef Flip_spoke_process_header
#define Flip_spoke_process_header

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>


#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>

class Flip_spoke_process : public bpro1_process 
{

public:
  Flip_spoke_process();
  virtual ~Flip_spoke_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name() {
    return "Flip spokes";
  }

  vcl_vector< vcl_string > get_input_type() {
    vcl_vector< vcl_string > to_return;
    // to_return.push_back( "vsol2D" );
    return to_return;
  }
  vcl_vector< vcl_string > get_output_type() {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "vsol2D" );
    return to_return;
  }

  int input_frames() {
    return 1;
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


