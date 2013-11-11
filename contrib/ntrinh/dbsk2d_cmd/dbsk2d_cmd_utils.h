// This is ntrinh/dbsk2d_cmd/dbsk2d_cmd_utils.h

#ifndef dbsksp_morph_image_utils_h_
#define dbsksp_morph_image_utils_h_

//:
// \file
// \brief 
// 
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date May 15, 2008
//
// \verbatim
//  Modifications:
// \endverbatim



#include <vgl/vgl_point_2d.h>
#include <vcl_string.h>
#include <dbsksp/dbsksp_shapelet_sptr.h>
#include <dbgl/algo/dbgl_eulerspiral.h>


//: A point on a circle
bool dbsk2d_cmd_generate_random_bnd(const vcl_string& bnd_file,
                                    int num_lines,
                                    int num_arcs,
                                    int image_width = 512,
                                    int image_height = 512);




#endif // ntrinh/dbsk2d_cmd/dbsk2d_cmd_utils.h

