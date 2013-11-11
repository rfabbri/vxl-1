// This is brcv/seg/dbdet/pro/dbdet_lvwr_process.h
#ifndef dbdet_lvwr_process_h_
#define dbdet_lvwr_process_h_
//:
// \file
// \brief Vpro process for computing livewire contour from samples along object contour.
// \author Ozge Can Ozcanli
// \date Mar 14 2005
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include <dbdet/lvwr/dbdet_lvwr_params.h>
#include <osl/osl_canny_ox_params.h>

#include <vil/vil_image_resource_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>

class dbdet_lvwr_process : public bpro1_process 
{
public:

  dbdet_lvwr_process();
  virtual ~dbdet_lvwr_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

  bool process_frame(vil_image_resource_sptr image_sptr, 
                     vcl_vector<float> inp_x, vcl_vector<float> inp_y,
                     vcl_vector<vsol_spatial_object_2d_sptr> &out_pts_smt,
                     vcl_vector<vsol_spatial_object_2d_sptr> &out_pts, float sigma);

  
protected:
  vcl_string input_file;
  float sigma;

  osl_canny_ox_params canny_params;
  dbdet_lvwr_params iparams;

private:

  bool get_intscissors_params(dbdet_lvwr_params* iparams, osl_canny_ox_params* params);
};

#endif //dbdet_lvwr_process_h_
