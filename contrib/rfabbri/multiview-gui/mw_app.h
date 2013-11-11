// This is mw_app.h
#ifndef mw_app_h
#define mw_app_h
//:
//\file
//\brief Multiview application
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date Sat Apr 16 22:49:00 EDT 2005
//
#include <vcl_string.h>
#include <vpgl/dvpgl_perspective_camera.h>
#include <vpgl/dvpgl_fundamental_matrix.h>

void mw_misc();
void example_project();
void call_show_contours_process(char *fname);
void test_point_reconstruct();
// same thing but with rig datastructure:
void test_point_reconstruct_rig();

#define MANAGER bvis1_manager::instance()


#endif // mw_app_h

