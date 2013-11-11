//:
// \file
//
#include <testlib/testlib_test.h>
#include "../dbvxm_detect_scale_process.h"
#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/pro/bvxm_roi_init_process.h>

#include <vcl_string.h>
#include <vcl_iostream.h>

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>
#include <vil/vil_save.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_float_3.h>
#include <vgl/vgl_vector_3d.h>

#include <brip/brip_vil_float_ops.h>
#include <vul/vul_file.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_load.h>
#include <vil/file_formats/vil_nitf2_image.h>
#include <vpgl/file_formats/vpgl_nitf_rational_camera.h>

MAIN( test_dbvxm_detect_scale_process )
{

  
  //: now test the process
  REG_PROCESS(dbvxm_detect_scale_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_roi_init_process, bprb_batch_process_manager);


  REGISTER_DATATYPE(bvxm_voxel_world_sptr);
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  REGISTER_DATATYPE(vpgl_camera_double_sptr);
  REGISTER_DATATYPE(vcl_string);
  //REGISTER_DATATYPE(float);
  REGISTER_DATATYPE(unsigned);

  vcl_string image_name="po_39928_pan_0000000_chip_700_700_noICHIPB.nitf";
  //vcl_string image_name="W:/video/ieden/satellite_baghdad/005630620010_01/005630620010_01_P001_PAN/02AUG18074303-P1BS-005630620010_01_P001.NTF";
  brdb_value_sptr imgname = new brdb_value_t<vcl_string>(image_name);

  // extract the camera
  vil_image_resource_sptr img = vil_load_image_resource(image_name.c_str());
  vil_nitf2_image* nitf =  static_cast<vil_nitf2_image*> (img.ptr());


    //get NITF information
     vcl_vector< vil_nitf2_image_subheader* > headers = nitf->get_image_headers();
     vil_nitf2_image_subheader* hdr = headers[0];

     double sun_el;
     double sun_az;

     bool success = hdr->get_sun_params(sun_el, sun_az);

  vpgl_camera_double_sptr camera = new vpgl_nitf_rational_camera(nitf, true);
  brdb_value_sptr v1 = new brdb_value_t<vpgl_camera_double_sptr>(camera);

  // create the voxel world
  vgl_vector_3d<unsigned int> num_voxels(100,100,50);
  float voxel_length = 0.5;
  bgeo_lvcs_sptr lvcs = new bgeo_lvcs(32.716835, -117.163714, 39.0);
  bvxm_world_params_sptr world_params = new bvxm_world_params();
  world_params->set_params("./", vgl_point_3d<float> (0,0,0), num_voxels, voxel_length, lvcs);
  bvxm_voxel_world_sptr world = new bvxm_voxel_world();
  world->set_params(world_params);
  brdb_value_sptr v2 = new brdb_value_t<bvxm_voxel_world_sptr>(world);


  //: inits with the default params
  bool good = bprb_batch_process_manager::instance()->init_process("bvxmRoiInitProcess");
  good = good && bprb_batch_process_manager::instance()->set_input(0,imgname);
  good = good && bprb_batch_process_manager::instance()->set_input(1,v1);
  good = good && bprb_batch_process_manager::instance()->set_input(2,v2);
  good = good && bprb_batch_process_manager::instance()->run_process();
  unsigned id_img;
  unsigned id_cam;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id_cam);
  good = good && bprb_batch_process_manager::instance()->commit_output(1, id_img);


  good = bprb_batch_process_manager::instance()->init_process("dbvxmDetectScaleProcess");
  good = good && bprb_batch_process_manager::instance()->set_input(0, v2);
  good = good && bprb_batch_process_manager::instance()->set_input_from_db(1, id_cam);
  good = good && bprb_batch_process_manager::instance()->set_input_from_db(2, id_img);

  good = good && bprb_batch_process_manager::instance()->run_process();

  unsigned id_scale;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id_scale);
  TEST("run dbvxm detect scale process", good ,true);

  brdb_query_aptr Q_scale = brdb_query_comp_new("id", brdb_query::EQ, id_scale);
  brdb_selection_sptr S_scale = DATABASE->select("unsigned_data", Q_scale);

        brdb_value_sptr value;
      if (!S_scale->get_value(vcl_string("value"), value)) {
        vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
                 << " didn't get value\n";
      }
      bool non_null = (value != 0);
      TEST("bin index output non-null", non_null ,true);

      // check that for zero regions choice, the index is zero
      brdb_value_t<unsigned>* result =
        static_cast<brdb_value_t<unsigned>* >(value.ptr());
      unsigned scale = result->value();

      TEST("Scale should be 1", scale ,1);


  SUMMARY();
}
