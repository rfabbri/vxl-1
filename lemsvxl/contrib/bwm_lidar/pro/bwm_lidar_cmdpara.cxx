//: This is bwm_lidar_cmdpara.cxx
//  Creation: Dec 13, 2007   Ming-Ching Chang

#include <dbmsh3d/dbmsh3d_utils.h>
#include <dbmsh3d/pro/dbmsh3d_cmdpara.h>

//#####################################################################
//: dbmsh3d_cmdproc command-line arguments.
//     Type     Variable     Flag      Help text        Default  


// Mesh lidar image
vul_arg<int>          bwm_lidar_cmd_run ("-run", "Running option", 2);
vul_arg<char const*>  bwm_lidar_cmd_dir ("-dir", "Input .xyz file directory");
vul_arg<int>          bwm_lidar_cmd_lidar ("-lidar", "Mesh lidar image", 0);
vul_arg<char const*>  bwm_lidar_cmd_ft ("-ft", "Texture image file prefix");

//#####################################################################




