/*
 * mex_third_order_edge_detector.cxx
 *
 *  Created on: Feb 17, 2012
 *      Author: firat
 */

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_cstdlib.h>
#include <dbdet/algo/dbdet_third_order_edge_det.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>


#include "../mex/vxl_mex_utils.h"

//function edg = mex_third_order_edge_detector(I, sigma, grad_thresh, N)

void mexFunction(int nlhs, mxArray *plhs[ ], int nrhs, const mxArray *prhs[ ])
{
	if(nrhs != 4)
	{
		mexErrMsgTxt("4 input arguments required! (image, sigma, grad_thresh, N)");
	}
	if(nlhs == 0)
	{
		mexWarnMsgTxt("No output argument. Skipping!");
		return;
	}
	if(nlhs > 1)
	{
		mexErrMsgTxt("Too many output arguments! Required: 1");
	}

	const mxArray* mxImg = prhs[0];
	double sigma = *mxGetPr(prhs[1]);
	double grad_thresh = *mxGetPr(prhs[2]);
	int N = *mxGetPr(prhs[3]);

	vil_image_view<vxl_byte> image;
	if(mxIsDouble(mxImg))
	{
		matlab_double_matrix_to_vil_image_view_vxl_byte(mxImg, image);
	}
	else if(mxIsUint8(mxImg))
	{
		matlab_uint8_matrix_to_vil_image_view_vxl_byte(mxImg, image);
	}
	else
	{
		mexErrMsgTxt("Input image type should be either double or uint8!");
	}
	dbdet_edgemap_sptr edgemap = dbdet_detect_third_order_edges(image,
	                                                  sigma, grad_thresh,
	                                                  N, 0,
	                                                  0, 0,
	                                                  false,
	                                                  false, //output edges on the interpolated (finer) grid
	                                                  false);
	int num_edges = edgemap->num_edgels();
	plhs[0] = mxCreateDoubleMatrix(num_edges, 4, mxREAL);
	double* out_ptr = mxGetPr(plhs[0]);
	vcl_vector<dbdet_edgel*> edges = edgemap->edgels;
	for(int i = 0; i < num_edges; i++)
	{
		out_ptr[i] = edges[i]->pt.x()+1;
		out_ptr[i + num_edges] = edges[i]->pt.y()+1;
		out_ptr[i + 2*num_edges] = edges[i]->tangent;
		out_ptr[i + 3*num_edges] = edges[i]->strength;
	}
}



