// This is breye1/vorl1/examples/vorl1_harris_example.cxx
//:
// \file

#include <vorl1/vorl1.h>
#include <vidpro1/process/vidpro1_harris_corners_process.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>


int main(int argc, char** argv)
{ 
  // Register storage types and processes
  REG_STORAGE(vidpro1_image_storage);
  REG_STORAGE(vidpro1_vsol2D_storage);

  //==========================================================

  // Make the process and add it to the list of program args
  bpro1_process_sptr process = new vidpro1_harris_corners_process();
  process->set_input_names(vcl_vector<vcl_string>(1,"video"));
  process->set_output_names(vcl_vector<vcl_string>(1,"corners"));
  vorl1_manager::instance()->add_process_to_args(process);

  //===========================================================

  // Parse the program arguments to update parameters
  vorl1_manager::instance()->parse_params(argc, argv);
  // Load the video into image storage classes named "video"
  vorl1_manager::instance()->load_video();

  // Run the process on all frames of the video
  while(vorl1_manager::instance()->next_frame())
   vorl1_manager::instance()->run_process_on_current_frame(process);

  // Write the performance file
  vorl1_manager::instance()->write_performance("");

  return 0; 
}




