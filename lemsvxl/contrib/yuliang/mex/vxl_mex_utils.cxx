/*
 * vxl_mex_utils.cxx
 *
 *  Created on: Feb 17, 2012
 *      Author: firat
 */

#include "vxl_mex_utils.h"
#include <vil/vil_convert.h>
//#include <vil/vil_save.h>

#ifdef __cplusplus
extern "C" {
#endif

mxArray* mxTranspose(const mxArray *, int);

#ifdef __cplusplus
}
#endif

template<class T>
void matlab_matrix_to_vil_image_view(const mxArray* img, vil_image_view<T>& vxl_img)
{
	int N = mxGetN(img); //num of columns: width -> ni
	int M = mxGetM(img); //num of rows: height -> nj
//	Transpose based implementation:
//	mxArray* trans_img = mxTranspose((mxArray*)img, 1);
//	T* img_data = (T*) (void*) mxGetPr(trans_img);
//	vxl_img.set_to_memory(img_data, N, M, 1, 1, N, N*M);

//  Regular implementation:
	T* img_data = (T*) (void*) mxGetPr(img);
	vxl_img.set_to_memory(img_data, N, M, 1, M, 1, N*M);
}

void matlab_uint8_matrix_to_vil_image_view_vxl_byte(const mxArray* img, vil_image_view<vxl_byte>& vxl_img)
{
	matlab_matrix_to_vil_image_view(img, vxl_img);
	//vil_save(vxl_img, "/home/firat/Desktop/debug.png");
}

void matlab_double_matrix_to_vil_image_view_double(const mxArray* img, vil_image_view<double>& vxl_img)
{
	matlab_matrix_to_vil_image_view(img, vxl_img);
}

void matlab_double_matrix_to_vil_image_view_vxl_byte(const mxArray* img, vil_image_view<vxl_byte>& vxl_img)
{
	vil_image_view<double> temp;
	matlab_matrix_to_vil_image_view(img, temp);
	vil_convert_cast(temp, vxl_img);
	//vil_save(vxl_img, "/home/firat/Desktop/debug.png");
}

void matlab_double_matrix_to_vil_image_view_float(const mxArray* img, vil_image_view<float>& vxl_img)
{
	vil_image_view<double> temp;
	matlab_matrix_to_vil_image_view(img, temp);
	vil_convert_cast(temp, vxl_img);
	//vil_save(vxl_img, "/home/firat/Desktop/debug.png");
}

void matlab_uint8_matrix_to_vil_image_view_float(const mxArray* img, vil_image_view<float>& vxl_img)
{
	vil_image_view<vxl_byte> temp;
	matlab_matrix_to_vil_image_view(img, temp);
	vil_convert_cast(temp, vxl_img);
	//vil_save(vxl_img, "/home/firat/Desktop/debug.png");
}



