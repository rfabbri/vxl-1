// This is brl/bseg/bbgm/pro/dbvxm_prob_map_area_process.cxx
#include "dbvxm_prob_map_area_process.h"
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>
#include <brdb/brdb_value.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vbl/vbl_array_2d.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_list.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <vil/vil_save.h>
#include <brip/brip_vil_float_ops.h>
#include <core/vidl_pro/vidl_pro_utils.h>


//: Constructor
dbvxm_prob_map_area_process::dbvxm_prob_map_area_process()
{
  //input
  input_data_.resize(2,brdb_value_sptr(0));
  input_types_.resize(2);
  input_types_[0]= "vil_image_view_base_sptr"; //input probability frame
  input_types_[1]= "vil_image_view_base_sptr"; //input probability frame's mask

  //output 
  output_data_.resize(2, brdb_value_sptr(0));
  output_types_.resize(2);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";
}

//: Execute the process
bool
dbvxm_prob_map_area_process::execute()
{
  // Sanity check
  if (!this->verify_inputs()){
    vcl_cerr << "In dbvxm_prob_map_area_process::execute() -"
             << " invalid inputs\n";
    return false;
  }

  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr temp = input0->value();
  vil_image_view<float> map = *vil_convert_cast(float(), temp);
  if (temp->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;
  unsigned ni = map.ni(), nj = map.nj();

  brdb_value_t<vil_image_view_base_sptr>* input1 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[1].ptr());
  temp = input1->value();
  vil_image_view<bool> input_mask = *vil_convert_cast(bool(), temp);

  vil_image_view<float> out(ni, nj, 1);
  out.fill(0);

  vcl_vector<vcl_pair<int, int> > neighborhood;
  for (int l = -2; l < 3; l++)
    for (int m = -2; m < 3; m++)
      neighborhood.push_back(vcl_pair<int, int>(l,m));

  int count = 0;
  for(unsigned j = 0; j<nj; ++j)
    for(unsigned i = 0; i<ni; ++i)
      {
        if (input_mask(i,j)) {
          float sum = 0.0f;
          for (unsigned t = 0; t < neighborhood.size(); t++) {
            int ii = i+neighborhood[t].first;
            int jj = j+neighborhood[t].second;
            if (ii >= 0 && jj >= 0 && ii < (int)ni && jj < (int)nj) 
              sum += map(ii,jj);
          }
          out(i,j) = sum;
        }
      }

   vil_image_view<vxl_byte> out_byte(ni, nj, 1);
  float min, max;
  vil_math_value_range(out, min, max);
  vcl_cout << "\t area map min: " << min << " max: " << max << vcl_endl;
  vil_convert_stretch_range_limited(out, out_byte, 0.0f, max);
  
  //vil_image_view<float> dummy(ni, nj, 1), dummy2(ni, nj, 1);
  //dummy.fill(max);
  //vil_math_image_difference(dummy, out, dummy2);
  //vil_convert_stretch_range_limited(dummy2, out_byte, 0.0f, max);

  brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(out));
  //brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(dummy2));
  output_data_[0] = output0;

  brdb_value_sptr output1 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(out_byte));
  output_data_[1] = output1;

  return true;
}

  
