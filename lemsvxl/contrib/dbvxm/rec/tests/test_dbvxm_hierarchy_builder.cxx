#include <testlib/testlib_test.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <vul/vul_file.h>

#include <rec/dbvxm_part_base.h>
#include <rec/dbvxm_part_base_sptr.h>
#include <rec/dbvxm_part_hierarchy.h>
#include <rec/dbvxm_part_hierarchy_sptr.h>
#include <rec/dbvxm_hierarchy_edge.h>
#include <rec/dbvxm_hierarchy_edge_sptr.h>
#include <rec/dbvxm_part_hierarchy_builder.h>

#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>

#include <brip/brip_vil_float_ops.h>

#include <rec/dbvxm_part_gaussian.h>
#include <bsta/algo/bsta_gaussian_updater.h>

static void test_dbvxm_hierarchy_builder()
{
  std::string file = "C:\\projects\\roi_1\\normalized0_cropped.tif";
  vil_image_resource_sptr img = vil_load_image_resource(file.c_str());
  TEST("test load img", !img, false);

  if (!img)
    return;

  unsigned ni = img->ni(); unsigned nj = img->nj();
  if (ni != 523 || nj != 460)
    return;

  std::cout << "image ni: " << ni << " nj: " << nj << std::endl;

  dbvxm_part_hierarchy_sptr h = dbvxm_part_hierarchy_builder::construct_vehicle_detector_roi1_2();
  TEST("test hierarchy", !h, false);
  std::cout << "constructed: " << h->number_of_vertices() << " vertices in the vehicle detector for roi1\n";
  std::cout << "constructed: " << h->number_of_edges() << " edges in the vehicle detector for roi1\n";

  std::vector<dbvxm_part_instance_sptr> dumm_ins = h->get_dummy_primitive_instances();
  
  std::vector<dbvxm_part_instance_sptr> parts_prims;
  for (unsigned i = 0; i < dumm_ins.size(); i++) {
    if (dumm_ins[i]->kind_ == dbvxm_part_instance_kind::GAUSSIAN) {
      dbvxm_part_gaussian_sptr p = dumm_ins[i]->cast_to_gaussian();
      if (!extract_gaussian_primitives(img, p->lambda0_, p->lambda1_, p->theta_, p->bright_, 0.1f, p->type_, parts_prims))
        std::cout << "problems in extracting gaussian primitives!!\n";
    }
  }

  std::cout << "\t extracted " << parts_prims.size() << " primitives\n";

  unsigned highest = h->highest_layer_id();
  std::vector<dbvxm_part_instance_sptr> parts_upper_most(parts_prims);
  for (unsigned l = 1; l <= highest; l++) {
    std::vector<dbvxm_part_instance_sptr> parts_current;
    h->extract_upper_layer(parts_upper_most, ni, nj, 0.1f, parts_current);
    std::cout << "extracted " << parts_current.size() << " parts of layer " << l << "\n";
    parts_upper_most.clear();
    parts_upper_most = parts_current;
  }

  std::cout << "\t extracted " << parts_upper_most.size() << " of highest layer: " << highest << " parts\n";

  vil_image_view<float> output_map_float(ni, nj);
  dbvxm_part_hierarchy::generate_output_map(parts_upper_most, output_map_float);
  float min, max;
  vil_math_value_range(output_map_float, min, max);
  std::cout << "\t output map float value range, min: " << min << " max: " << max << std::endl;

  vil_image_view<vxl_byte> output_map_byte(ni, nj);
  vil_convert_stretch_range_limited(output_map_float, output_map_byte, 0.0f, 1.0f);
  vil_save(output_map_byte, "./map_output_receptive_field_1_0.png");

  vil_image_view<vxl_byte> output_img(ni, nj, 3);
  vil_image_view<vxl_byte> input_img = img->get_view(0, ni, 0, nj);
  dbvxm_part_hierarchy::generate_output_img(parts_upper_most, input_img, output_img);
  vil_save(output_img, "./img_output_receptive_field_1_0.png");

}

TESTMAIN( test_dbvxm_hierarchy_builder );





