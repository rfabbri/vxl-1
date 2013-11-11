// This is breye1/vorl1/exec/vorl1_compute_homography_exec.cxx
//:
// \file

#include <vorl1/vorl1.h>
#include <dbbgm/pro/dbbgm_aerial_bg_model_process1.h>
#include <dbbgm/pro/dbbgm_save_bg_model_process1.h>
#include <dbbgm/pro/dbbgm_aerial_fg_uncertainity_detect_process1.h>
#include <dbdet/pro/dbdet_blob_finder_process.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <bpro1/bpro1_parameters_sptr.h>
#include <bpro1/bpro1_process_sptr.h>
#include <dbbgm/pro/dbbgm_image_storage.h>
#include <vidpro1/vidpro1_repository.h>
#include <vnl/vnl_matrix.h>
#include <vimt/vimt_transform_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vcl_fstream.h>
#include <vbl/vbl_bounding_box.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>

int main(int argc, char** argv)
{
    REG_STORAGE(dbbgm_image_storage);
    REG_STORAGE(vidpro1_image_storage);
  
  // Make each process and add it to the list of program args


  //bpro1_process_sptr regimg(new dbvrl_register_images_process()); 
  //regimg->set_input_names(vcl_vector<vcl_string>(1,"video"));
  //regimg->set_output_names(vcl_vector<vcl_string>(1,"RegImg"));
  //vorl1_manager::instance()->add_process_to_args(regimg); 

    vcl_vector<vcl_string> input_names_bg;
    input_names_bg.push_back("video");
    input_names_bg.push_back("bgmodel");
    bpro1_process_sptr bgbuilder(new dbbgm_aerial_bg_model_process1()); //dbbgm_distribution_image
    bgbuilder->set_input_names(input_names_bg);
    vorl1_manager::instance()->add_process_to_args(bgbuilder);


  bpro1_process_sptr savebgmodel(new dbbgm_save_bg_model_process1()); 
  savebgmodel->set_input_names(vcl_vector<vcl_string>(1,"bgmodel"));
  vorl1_manager::instance()->add_process_to_args(savebgmodel);

  

  vorl1_manager::instance()->parse_params(argc, argv); 
  vorl1_manager::instance()->load_video();

  // building background model
  vorl1_manager::instance()->add_process_to_queue(bgbuilder);

  vcl_cout<<"\n building background model";
  vcl_cout.flush();

  int n=vorl1_manager::instance()->get_no_skip_frames();
  do {
      float status = vorl1_manager::instance()->status()/2.0;
      vorl1_manager::instance()->write_status(status);
      vorl1_manager::instance()->run_process_queue_on_current_frame();
  }  while(vorl1_manager::instance()->next_nth_frame(n));
  vorl1_manager::instance()->finish_process_queue();
  vorl1_manager::instance()->run_process_on_current_frame(savebgmodel);

return 0; 
}


