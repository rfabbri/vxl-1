#if !defined(DET_SUPPRESSION_AND_INTERPOLATION_H_)
#define DET_SUPPRESSION_AND_INTERPOLATION_H_

#include <vgl/vgl_vector_3d.h>
#include <biob/biob_worldpt_field.h>

class det_suppression_and_interpolation {
 public:
 biob_worldpt_field<vgl_vector_3d<double> > 
   apply(biob_worldpt_field<vgl_vector_3d<double> > & field, double width, double height, double min_angle = 2*3.14*40./360,
         double strength_threshold = 10000.);
};


#endif
