/*
 * vxl_mex_utils.h
 *
 *  Created on: Feb 17, 2012
 *      Author: firat
 */

#ifndef VXL_MEX_UTILS_H_
#define VXL_MEX_UTILS_H_

#include "mex.h"
#include <vil/vil_image_view.h>

void matlab_uint8_matrix_to_vil_image_view_vxl_byte(const mxArray* img, vil_image_view<vxl_byte>& vxl_img);
void matlab_double_matrix_to_vil_image_view_double(const mxArray* img, vil_image_view<double>& vxl_img);
void matlab_double_matrix_to_vil_image_view_vxl_byte(const mxArray* img, vil_image_view<vxl_byte>& vxl_img);
void matlab_double_matrix_to_vil_image_view_float(const mxArray* img, vil_image_view<float>& vxl_img);
void matlab_uint8_matrix_to_vil_image_view_float(const mxArray* img, vil_image_view<float>& vxl_img);

#endif /* VXL_MEX_UTILS_H_ */
