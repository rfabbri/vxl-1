//: This is bwm_lidar_cmdproc.h
//  Creation: Dec 13, 2007   Ming-Ching Chang

#ifndef bwm_lidar_cmdproc_h_
#define bwm_lidar_cmdproc_h_

///#include <Inventor/nodes/SoSeparator.h>
///class SoSeparator;

#include <vcl_vector.h>
#include <dbmsh3d/dbmsh3d_mesh_mc.h>
#include <dbmsh3d/dbmsh3d_textured_mesh_mc.h>
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_proj_camera.h>


//#####################################################################
// bwm_lidar command-line process execution.

bool bwm_lidar_check_cmdproc (int argc, char **argv);

SoSeparator* bwm_lidar_cmdproc_execute ();


#endif



