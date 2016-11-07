#ifndef breg3d_lm_direct_homography_generator_h_
#define breg3d_lm_direct_homography_generator_h_

#include <vimt/vimt_transform_2d.h>

#include "breg3d_homography_generator.h"


class breg3d_lm_direct_homography_generator : public breg3d_homography_generator
{
public:
  breg3d_lm_direct_homography_generator(){};
  
  virtual vimt_transform_2d compute_homography();


};




#endif
