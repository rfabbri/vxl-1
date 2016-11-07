// This is contrib/mleotta/cmd/modrec_exec.cxx
//:
// \file

#include <vorl1/vorl1.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/vidpro1_repository.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <dbbgm/pro/dbbgm_image_storage.h>

#include <dbbgm/pro/dbbgm_init_model_process.h>
#include <modrec/pro/modrec_classify_process.h>

#include <vil/vil_convert.h>
#include <vil/vil_new.h>

#include <vidl1/vidl1_movie.h>
#include <vidl1/vidl1_clip.h>
#include <vidl1/vidl1_io.h>



// The Main Function
int main(int argc, char** argv)
{
  // Register storage types and processes
  REG_STORAGE(vidpro1_image_storage);
  REG_STORAGE( dbbgm_image_storage );

  //==========================================================

  // Make each process and add them to the list of program args
  bpro1_process_sptr bg_modeller(new dbbgm_init_model_process());
  bg_modeller->set_input_names(vcl_vector<vcl_string>(1,"video"));
  bg_modeller->set_output_names(vcl_vector<vcl_string>(1,"model"));
  vorl1_manager::instance()->add_process_to_args(bg_modeller);

  bpro1_process_sptr classifier(new modrec_classify_process());
  vcl_vector<vcl_string> classifier_input_names;
  classifier_input_names.push_back("video");
  classifier_input_names.push_back("model");
  classifier->set_input_names(classifier_input_names);
  vcl_vector<vcl_string> classifier_output_names;
  classifier_output_names.push_back("image1");
  classifier_output_names.push_back("image2");
  classifier_output_names.push_back("image3");
  classifier->set_output_names(classifier_output_names);
  vorl1_manager::instance()->add_process_to_args(classifier);

  //===========================================================

  // Parse the arguments and load the video
  vorl1_manager::instance()->parse_params(argc, argv);
  vorl1_manager::instance()->load_video();


  //===========================================================

  // Set up the process queue to be run on all frames
  vorl1_manager::instance()->add_process_to_queue(bg_modeller);

  vcl_cout << "Modelling background" << vcl_endl;
  // run the process queue on the whole video
  do {
    float status = vorl1_manager::instance()->status()/2.0;
    vorl1_manager::instance()->write_status(status);
    vorl1_manager::instance()->run_process_queue_on_current_frame();
  }  while(vorl1_manager::instance()->next_frame());
  vorl1_manager::instance()->finish_process_queue();

  vcl_cout << "Model complete" << vcl_endl;

  //===========================================================

  // Rewind to the first frame processed and set a new queue
  vorl1_manager::instance()->rewind();
  vorl1_manager::instance()->clear_process_queue();
  vorl1_manager::instance()->add_process_to_queue(classifier);

  vcl_cout << "Aligning the models" << vcl_endl;
  // run the process queue on the whole video
  do {
    float status = vorl1_manager::instance()->status()/2.0 + 0.5;
    vorl1_manager::instance()->write_status(status);
    vorl1_manager::instance()->run_process_queue_on_current_frame();
  }  while(vorl1_manager::instance()->next_frame());
  vorl1_manager::instance()->finish_process_queue();
  vorl1_manager::instance()->write_status(1.0);
  vcl_cout << "Models aligned" << vcl_endl;

  //===========================================================


  // Write out the debugging images
  vorl1_manager::instance()->rewind();
  vidpro1_repository_sptr rep = vorl1_manager::instance()->repository();
  vcl_string out_dir = vorl1_manager::instance()->get_output_dir();
  vidpro1_image_storage_sptr image_storage;
  vil_image_resource_sptr byte_image;
  vcl_vector<vil_image_resource_sptr> frames1, frames2, frames3;
  do {
    image_storage.vertical_cast(rep->get_data_by_name("image1"));
    byte_image = vil_new_image_resource_of_view(
        *vil_convert_stretch_range(vxl_byte(), image_storage->get_image()->get_view()));
    frames1.push_back(byte_image);

    image_storage.vertical_cast(rep->get_data_by_name("image2"));
    byte_image = vil_new_image_resource_of_view(
        *vil_convert_stretch_range(vxl_byte(), image_storage->get_image()->get_view()));
    frames2.push_back(byte_image);

    image_storage.vertical_cast(rep->get_data_by_name("image3"));
    byte_image = vil_new_image_resource_of_view(
        *vil_convert_stretch_range(vxl_byte(), image_storage->get_image()->get_view()));
    frames3.push_back(byte_image);
  } while(vorl1_manager::instance()->next_frame());

  vidl1_movie_sptr movie1 = new vidl1_movie(new vidl1_clip(frames1));
  vidl1_movie_sptr movie2 = new vidl1_movie(new vidl1_clip(frames2));
  vidl1_movie_sptr movie3 = new vidl1_movie(new vidl1_clip(frames3));

  vcl_string outvid_dir = vorl1_manager::instance()->get_output_video_dir();
  vidl1_io::save_images(movie1,outvid_dir+"/object1_","png");
  vidl1_io::save_images(movie2,outvid_dir+"/object2_","png");
  vidl1_io::save_images(movie3,outvid_dir+"/object3_","png");

  // Save part of the repository
  vcl_set<vcl_string> names;
  names.insert("model");
  //names.insert("image1");
  //names.insert("image2");
  //names.insert("image3");
  vcl_string output_dir = vorl1_manager::instance()->get_output_dir();
  vcl_string binfile=output_dir+"/bin.rep";
  vorl1_manager::instance()->save_repository(binfile, names);


  return 0; 
}




