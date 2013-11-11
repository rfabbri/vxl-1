// This is dbdet_mask_edges_process_h_
#ifndef dbdet_mask_edges_process_h_
#define dbdet_mask_edges_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Process that takes a vsol storage class and a mask image as input and outputs another vsol2D 
//        with masked edges
//        mask image should have zeros in the region of interest
//        another option is to give a rectangular polygon in a vsol2d storage
//        then edges outside the rectangular region are masked out
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/16/06
//
// \verbatim
//  Modifications
//
// \endverbatim
//--------------------------------------------------------------------------------
#include <vcl_vector.h>
#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <vsol/vsol_point_2d.h>
#include <vil/vil_image_view.h>

class dbdet_mask_edges_process : public bpro1_process
{
public:
  dbdet_mask_edges_process();
  ~dbdet_mask_edges_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name() {
    return "Mask Edges";
  }

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

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
  vil_image_view<vxl_byte> mask; int ni, nj;

  bool valid_point(vsol_point_2d* point);
  bool valid_point(vsol_point_2d_sptr point);
  bool valid_point(float xx, float yy);

};

#endif // dbdet_mask_edges_process_h_
