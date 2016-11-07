// aerial_vehicle_segmentation.cpp : Defines the entry point for the DLL application.
//
#include <vehicleseg/exe/aerial_vehicle_segmentation.h>
#include <dbbgm/pro/dbbgm_distribution_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <dbbgm/pro/dbbgm_aerial_fg_uncertainity_detect_process.h>
#include <dbbgm/pro/dbbgm_aerial_bg_model_process.h>
#include <dbdet/pro/dbdet_blob_finder_process.h>

#include <vil/vil_image_resource.h>
#include <bpro1/bpro1_parameters_sptr.h>
#include <bpro1/bpro1_parameters.h>
#include <bpro1/bpro1_process_sptr.h>
#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/process/vidpro1_load_video_process.h>
#include <vidpro1/vidpro1_process_manager.h>
#include <vil/vil_save.h>
#include <dbsta/dbsta_basic_functors.h>
#include <dbbgm/dbbgm_distribution_image.h>
#include <dbbgm/dbbgm_distribution_image_sptr.h>
#include <brip/brip_vil_float_ops.h>
void detectvehicle(char *inputdir,unsigned int maxComp, float initialstd,float intialweight, float sigmathresh,float minweightthresh,
                     int min_no_of_pixels,int max_no_of_pixels,float erosionrad,float postdilationerosionrad,float dilationrad, char *polyfile)
{

    vcl_string fname(inputdir);
    //: setting up the repository and process manager
    vidpro1_process_manager process_manager;
    process_manager.get_repository_sptr()->initialize(1);

    process_manager.get_repository_sptr()->register_type(new vidpro1_image_storage());
    process_manager.get_repository_sptr()->register_type(new dbbgm_distribution_image_storage());
    process_manager.get_repository_sptr()->register_type(new vidpro1_vsol2D_storage());
    //: loading the video 
    bpro1_process_sptr load_video_proc = new vidpro1_load_video_process;
    process_manager.register_process(load_video_proc);
    load_video_proc->set_output_names(vcl_vector<vcl_string>(1,"video"));

    load_video_proc->parameters()->set_value("-video_filename",bpro1_filepath(fname));
    process_manager.run_process_on_current_frame(load_video_proc);
    vidpro1_repository_sptr rep = process_manager.get_repository_sptr();
    int first_frame_ = 0;
    int last_frame_ = rep->num_frames()-1;

    rep->go_to_frame(first_frame_);


    // Make each process and add it to the list of program args
    bpro1_process_sptr bgbuilder(new dbbgm_aerial_bg_model_process());
    bgbuilder->set_input_names(vcl_vector<vcl_string>(1,"video"));
    bgbuilder->set_output_names(vcl_vector<vcl_string>(1,"bgmodel"));
    
    process_manager.register_process(bgbuilder);
    bgbuilder->parameters()->set_value("-maxcmp",maxComp);
    bgbuilder->parameters()->set_value("-initv",initialstd);
    bgbuilder->parameters()->set_value("-initw",intialweight);

    vcl_ofstream ofile("temp.txt");
    ofile<<"No of frames loaded "<<last_frame_;
    ofile.close();

    do {
     process_manager.run_process_on_current_frame(bgbuilder);
    }while(rep->go_to_next_frame());

    rep->go_to_frame(first_frame_);

    //: foreground detection code
  bpro1_process_sptr fgdetector(new dbbgm_aerial_fg_uncertainity_detect_process());
  vcl_vector<vcl_string> inputnames;
  inputnames.push_back("video");
  inputnames.push_back("bgmodel");

  fgdetector->set_input_names(inputnames);
  fgdetector->set_output_names(vcl_vector<vcl_string>(1,"fgimage"));

  process_manager.register_process(fgdetector);
  fgdetector->parameters()->set_value("-dist",sigmathresh);
  fgdetector->parameters()->set_value("-minweightthresh",minweightthresh);

  //: blob finder
  bpro1_process_sptr  blobfinder(new dbdet_blob_finder_process());
  blobfinder->set_input_names(vcl_vector<vcl_string>(1,"fgimage"));
  blobfinder->set_output_names(vcl_vector<vcl_string>(1,"blobs"));
  process_manager.register_process(blobfinder);

  blobfinder->parameters()->set_value("-maxregion",max_no_of_pixels);
  blobfinder->parameters()->set_value("-minregion",min_no_of_pixels);
  blobfinder->parameters()->set_value("-rerode",erosionrad);
  blobfinder->parameters()->set_value("-rdilate",dilationrad);
  blobfinder->parameters()->set_value("-secerode",postdilationerosionrad);
  blobfinder->parameters()->set_value("-polyfile",bpro1_filepath(vcl_string(polyfile)));


   do {
     process_manager.run_process_on_current_frame(fgdetector);
     process_manager.run_process_on_current_frame(blobfinder);
    }while(rep->go_to_next_frame());


   blobfinder->finish();

}
