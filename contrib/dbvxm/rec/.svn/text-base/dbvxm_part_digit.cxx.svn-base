//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/16/08
//
//

#include <rec/dbvxm_part_digit_sptr.h>
#include <rec/dbvxm_part_digit.h>

#include <vil/vil_convert.h>
#include <vil/vil_save.h>
#include <vil/algo/vil_threshold.h>
#include <vil/vil_new.h>
#include <brip/brip_vil_float_ops.h>

//strength_threshold in [0,1] - min strength to declare the part as detected
bool extract_digit_primitives(vil_image_resource_sptr img, float lambda0, float lambda1, float theta, bool bright, float strength_threshold, unsigned type, vcl_vector<dbvxm_part_instance_sptr>& parts)
{
  vil_image_view<float> fimg = brip_vil_float_ops::convert_to_float(img);
  vil_image_view<float> extr = brip_vil_float_ops::extrema(fimg, lambda0, lambda1, theta, bright, true);
  if (extr.nplanes() != 2)
    return false;

  unsigned ni = fimg.ni();
  unsigned nj = fimg.nj();

  vil_image_view<float> res(ni, nj), mask(ni, nj);
  for(unsigned j = 0; j<nj; ++j)
    for(unsigned i = 0; i<ni; ++i)
    {
      res(i,j) = extr(i,j,0);
      mask(i,j) = extr(i,j,1);
    }

  float min, max;
  vil_math_value_range(res, min, max);
  
#if 1
  vcl_cout << "res min: " << min << " max: " << max << vcl_endl;
  vil_image_view<vxl_byte> res_o(ni, nj);
  vil_convert_stretch_range_limited(res, res_o, min, max);
  vil_save(res_o, "./temp.png");
#endif

  //: find the top 10 percentile of the output map and convert it into a prob map (scale to [0,1] range) accordingly
  //float val;
  //vil_math_value_range_percentile(res, 1.0, val);
  //vcl_cout << "res top 10 percentile value: " << val << vcl_endl;
  vil_image_view<float> strength_map(ni, nj);
  //vil_convert_stretch_range_limited(res, strength_map, 0.0f, val, 0.0f, 1.0f);
  vil_convert_stretch_range_limited(res, strength_map, 0.0f, max, 0.0f, 1.0f);
#if 1
  vil_math_value_range(strength_map, min, max);
  vcl_cout << "strength_map min: " << min << " max: " << max << vcl_endl;
  vil_convert_stretch_range_limited(strength_map, res_o, min, max);
  vil_save(res_o, "./strength_map.png");
#endif

  //: extract all the parts from the responses
  for(unsigned j = 0; j<nj; ++j)
    for(unsigned i = 0; i<ni; ++i)
    {
      if (strength_map(i,j) > strength_threshold) {
        dbvxm_part_digit_sptr dp = new dbvxm_part_digit((float)i, (float)j, strength_map(i,j), lambda0, lambda1, theta, type);
        parts.push_back(dp->cast_to_instance());
      }
      
    }

  
#if 0
  vil_image_resource_sptr img_resc = vil_new_image_resource_of_view(img);
  vil_image_resource_sptr res_resc = vil_new_image_resource_of_view(res);
  vil_image_resource_sptr msk_resc = vil_new_image_resource_of_view(mask);
  vil_image_view<vil_rgb<vxl_byte> > rgb =   
      brip_vil_float_ops::combine_color_planes(img_resc, res_resc, msk_resc);
    vil_save(rgb, "./temp.png");
  vil_math_value_range(fimg, min, max);
  vcl_cout << "img min: " << min << " max: " << max << vcl_endl;
  vil_math_value_range(mask, min, max);
  vcl_cout << "mask min: " << min << " max: " << max << vcl_endl;
#endif
  

  vil_image_view<bool> res_bool;
  vil_threshold_above(res, res_bool, max/2);
  vil_image_view<float> res_bool_f;
  vil_convert_cast(res_bool, res_bool_f);
  vil_convert_stretch_range_limited(res_bool_f, res_o, 0.0f, 1.0f);
  vil_save(res_o, "./temp_thresholded.png");

  return true;
}

dbvxm_part_digit* dbvxm_part_digit::cast_to_digit(void)
{
  return this;
}


