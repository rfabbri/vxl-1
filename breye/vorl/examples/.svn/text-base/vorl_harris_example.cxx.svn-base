// This is breye/vorl/examples/vorl_harris_example.cxx
//:
// \file

#include <vorl/vorl.h>
#include <vidpro/process/vidpro_harris_corners_process.h>
#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_vsol2D_storage.h>


int main(int argc, char** argv)
{ 
  // Register storage types and processes
  REG_STORAGE(vidpro_image_storage);
  REG_STORAGE(vidpro_vsol2D_storage);

  //==========================================================

  // Make the process and add it to the list of program args
  bpro_process_sptr process = new vidpro_harris_corners_process();
  process->set_input_names(vcl_vector<vcl_string>(1,"video"));
  process->set_output_names(vcl_vector<vcl_string>(1,"corners"));
  vorl_manager::instance()->add_process_to_args(process);

  //===========================================================

  // Parse the program arguments to update parameters
  vorl_manager::instance()->parse_params(argc, argv);
  // Load the video into image storage classes named "video"
  vorl_manager::instance()->load_video();

  // Run the process on all frames of the video
  while(vorl_manager::instance()->next_frame())
   vorl_manager::instance()->run_process_on_current_frame(process);

  // Write the performance file
  vorl_manager::instance()->write_performance("");

  return 0; 
}




