/*************************************************************************
 *    NAME: pradeep yarlagadda
 *    FILE: shape_articulation_process.h
 *    DATE: Mon June 11 2007
 *************************************************************************/

#ifndef shape_articulation_process_header
#define shape_articulation_process_header

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>


#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>

class shape_articulation_process : public bpro1_process 
{
protected:
    vcl_vector<vsol_point_2d_sptr> closed_articulated_structure(vcl_vector<vsol_point_2d_sptr> final_points);

public:
  shape_articulation_process();
  virtual ~shape_articulation_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name() {
    return "shape articulation";
  }

  vcl_vector< vcl_string > get_input_type() {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "vsol2D" );
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


