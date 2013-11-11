// This is ctspheres_io.h
#ifndef ctspheres_io_h
#define ctspheres_io_h
//:
//\file
//\brief Routines for reading, writing, and defining parameters for CT spheres dataset
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 02/24/2006 11:42:31 AM EST
//

#include <vil/vil_image_view.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vnl/vnl_math.h>

void trace_contours(const vcl_vector<vil_image_view<float> > &imgv, 
    vcl_vector<vcl_vector<vsol_point_2d_sptr> >  &final_contours,
    unsigned slice_idx);

void load_ct_spheres_dataset();

void read_levelset(vcl_vector<vil_image_view<float> > &imgv);

#define MANAGER bvis1_manager::instance()


#endif // ctspheres_io_h

