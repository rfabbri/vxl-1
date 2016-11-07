#if !defined(BIOB_WORLDPT_TRANSFORMATION_H_)
#define BIOB_WORLDPT_TRANSFORMATION_H_

#include "../bio_defs.h"
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vgl/vgl_homg_point_3d.h>

// \file biob_worldpt_transformation.h
// \brief A tranformation from worldpts to worldpts
// \author P. N. Klein
// \date March 9, 2005
//
//convert a worldpt to a homogeneous point
//then apply transformation
//then convert back to worldpt

class biob_worldpt_transformation {
 public:
  biob_worldpt_transformation inverse() const;
  worldpt operator()(worldpt pt);
  worldpt preimage(worldpt pt);
  biob_worldpt_transformation(vgl_h_matrix_3d<double> matrix);
  biob_worldpt_transformation();
 private:
  vgl_h_matrix_3d<double> matrix_;
  vgl_homg_point_3d<double> convert(worldpt pt);
  worldpt convert(vgl_homg_point_3d<double> pt);
 
};

#endif
