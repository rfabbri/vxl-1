#include "dbvxm_rec_vehicles_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_convert.h>
#include <vil/io/vil_io_image_view.h>
#include <brip/brip_vil_float_ops.h>

#include <vil/vil_new.h>
#include <brip/brip_vil_float_ops.h>
#include <vul/vul_timer.h>

#include <rec/dbvxm_part_hierarchy_builder.h>
#include <rec/dbvxm_part_hierarchy.h>
#include <rec/dbvxm_part_gaussian_sptr.h>
#include <rec/dbvxm_part_gaussian.h>

dbvxm_rec_vehicles_process::dbvxm_rec_vehicles_process()
{
  //inputs
  input_data_.resize(1,brdb_value_sptr(0));
  input_types_.resize(1);
  input_types_[0] = "vil_image_view_base_sptr";      // input orig view

  //output
  output_data_.resize(2,brdb_value_sptr(0));
  output_types_.resize(2);
  output_types_[0]= "vil_image_view_base_sptr";      // output prob map
  output_types_[1]= "vil_image_view_base_sptr";      // output map overlayed on orig view as a byte image
}


bool dbvxm_rec_vehicles_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr orig_view = input0->value();

  if (orig_view->pixel_format() != VIL_PIXEL_FORMAT_BYTE)
    return false;

  vil_image_view<vxl_byte> orig_img(orig_view);

  unsigned ni = orig_img.ni();
  unsigned nj = orig_img.nj();

  vil_image_resource_sptr img = vil_new_image_resource_of_view(orig_img);

  vul_timer t2;
  t2.mark();

  dbvxm_part_hierarchy_sptr h = dbvxm_part_hierarchy_builder::construct_vehicle_detector();
  
  //: now extract instances of primitive part types in h
  vcl_vector<dbvxm_part_instance_sptr> parts_0;
  vcl_vector<dbvxm_part_instance_sptr>& d_ins = h->get_dummy_primitive_instances();
  unsigned prev_size = parts_0.size();
  for (unsigned i = 0; i < d_ins.size(); i++) {
    if (d_ins[i]->kind_ != dbvxm_part_instance_kind::GAUSSIAN) 
      return false;
    
    dbvxm_part_gaussian_sptr gp = d_ins[i]->cast_to_gaussian();
    if (!gp)
      return false;

    if (!extract_gaussian_primitives(img, gp->lambda0_, gp->lambda1_, gp->theta_, gp->bright_, 0.1f, gp->type_, parts_0))
      return false;

    vcl_cout << "extracted " << parts_0.size()-prev_size << " primitive parts of type: " << d_ins[i]->type_ << vcl_endl;
    prev_size = parts_0.size();
  }

  vcl_vector<dbvxm_part_instance_sptr> parts_1;
  h->extract_upper_layer(parts_0, ni, nj, 0.1f, parts_1);
  vcl_cout << "extracted " << parts_1.size() << " parts of type 1\n";

  vil_image_view<float> map(ni, nj);
  vil_image_view<unsigned> type_map(ni, nj);
  h->generate_map(parts_1, map, type_map);

  vil_image_view_base_sptr out_map_sptr = new vil_image_view<float>(map);
  brdb_value_sptr output = new brdb_value_t<vil_image_view_base_sptr>(out_map_sptr);
  output_data_[0] = output;

  //: generate second output
  vil_image_view<vxl_byte> out_img_G(ni, nj, 1);
  out_img_G.fill(0);
  vil_image_view<vxl_byte> out_img_B(ni, nj, 1);
  out_img_B.fill(0);

  for (unsigned i = 0; i < parts_1.size(); i++) {
    parts_1[i]->mark_center(out_img_G, 0);
    parts_1[i]->mark_receptive_field(out_img_B, 0);
  }

  //vil_image_view<vil_rgb<vxl_byte> > out_img = brip_vil_float_ops::combine_color_planes(orig_img, out_img_G, out_img_B);
  //vil_image_view<vxl_byte> out_img2(out_img);
  //vil_image_view_base_sptr out_map_sptr2 = new vil_image_view<vil_rgb<vxl_byte> >(out_img);
  vil_image_view<vxl_byte> out_img(orig_img.ni(), orig_img.nj(), 3);
  for (unsigned i = 0; i < orig_img.ni(); i++)
    for (unsigned j = 0; j < orig_img.nj(); j++) {
      out_img(i,j,0) = orig_img(i,j);
      out_img(i,j,1) = out_img_G(i,j);
      out_img(i,j,2) = out_img_B(i,j);
    }

  vil_image_view_base_sptr out_map_sptr2 = new vil_image_view<vxl_byte>(out_img);
  brdb_value_sptr output1 = new brdb_value_t<vil_image_view_base_sptr>(out_map_sptr2);
  output_data_[1] = output1;

  vcl_cout << " whole process took: " << t2.real() / (60*1000.0f) << " mins.\n";

  return true;
}

