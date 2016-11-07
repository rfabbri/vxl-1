// This is /lemsvxl/contrib/firat/dbcvr_2d_shape_descriptors/dbcvr_2d_shape_descriptors.h.

// \file
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
// \date May 20, 2011

#ifndef DBCVR_2D_SHAPE_DESCRIPTORS_H_
#define DBCVR_2D_SHAPE_DESCRIPTORS_H_

#include <vsol/vsol_digital_curve_2d_sptr.h>
#include <vnl/vnl_matrix.h>
#include <vcl_vector.h>

bool dbcvr_diff_chord_angles(vsol_digital_curve_2d_sptr curve, bool is_closed, vcl_vector<unsigned>& indices, vnl_matrix<double>& alpha_prime);


#endif /* DBCVR_2D_SHAPE_DESCRIPTORS_H_ */
