// This is ntrinh/dbsk2d_cmd/dbsk2d_cmd.cxx

//:
// \file
// \brief 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date May 15, 2008



#include <vcl_iostream.h>
#include <vul/vul_arg.h>
#include "dbsk2d_cmd_utils.h"


//: Command line programs for the sks class
int main(int argc, char *argv[]) 
{
  // Arguments
  vul_arg_info_list arg_list;
  
  // GENERATE RANDOM BOUNDARY
  vul_arg<bool > gen_random_bnd(arg_list,"-gen-random-bnd", "Generate a random boundary for shock computation", false);
  vul_arg<bool > help_gen_random_bnd(arg_list,"-help-gen_random-bnd", 
    "print help on using the option -gen_random-bnd", false);

  vul_arg<vcl_string > bnd_file(arg_list,"-bnd-file", "Output boundary file", "sample.bnd");
  vul_arg<int > num_lines(arg_list,"-num-lines", "Number of line segments", 10);
  vul_arg<int > num_arcs(arg_list,"-num-arcs", "Number of circular arc segments", 10);



  // HELP UTILITIES
  vul_arg<bool > print_usage_only(arg_list,"-usage", "print usage info and exit",false);
  vul_arg<bool > print_help(arg_list,"-help", "print usage info and exit",false);


  // Parse the arguments
  vul_arg_include(arg_list);
  vul_arg_parse(argc, argv);


  // Process command line arguments //////////////////////////////////////////////

  // help
  if (print_help() || print_usage_only()) {
    vul_arg_display_usage_and_exit();
    return 0;
  }

  // print usage on converting ps images to jpeg
  if (help_gen_random_bnd())
  {
    vcl_cout << "Usage with --gen_random-bnd option:\n"
      << bnd_file.option() << " " << bnd_file.help() << "\n"
      << num_lines.option() << " " << num_lines.help() << "\n"
      << num_arcs.option() << " " << num_arcs.help() << "\n";
  }

  // convert PS (with bounding box) images to jpeg images
  if (gen_random_bnd())
  {
    vcl_cout << "Output boundary file= " << bnd_file() << "\n"
      << "Numer of line segments = " << num_lines() << "\n"
      << "Numer of arc segments = " << num_arcs() << "\n";
    int image_width = 512;
    int image_height = 512;

    dbsk2d_cmd_generate_random_bnd(bnd_file(), num_lines(), num_arcs(),
                                    image_width, image_height);


  }


  
  return 0;
}
