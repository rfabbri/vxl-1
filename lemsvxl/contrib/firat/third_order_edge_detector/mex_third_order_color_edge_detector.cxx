/*
 * mex_third_order_color_edge_detector.cxx
 *
 *  Created on: May 16, 2012
 *      Author: firat
 */

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_cstdlib.h>
#include <dbdet/algo/dbdet_third_order_color_detector.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>


#include "../mex/vxl_mex_utils.h"

//function edg = mex_third_order_color_edge_detector(comp1, comp2, comp3, use_lab, sigma, grad_thresh, N)

void mexFunction(int nlhs, mxArray *plhs[ ], int nrhs, const mxArray *prhs[ ])
{
	if(nrhs != 6)
	{
		mexErrMsgTxt("6 input arguments required! (R, G, B, sigma, grad_thresh, N)");
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

	const mxArray* mxImgR = prhs[0];
	const mxArray* mxImgG = prhs[1];
	const mxArray* mxImgB = prhs[2];
	bool use_lab = *mxGetPr(prhs[3]);
	double sigma = *mxGetPr(prhs[4]);
	double grad_thresh = *mxGetPr(prhs[5]);
	int N = *mxGetPr(prhs[6]);

	vil_image_view<float> comp1, comp2, comp3;
	if(mxIsDouble(mxImgR))
	{
		matlab_double_matrix_to_vil_image_view_float(mxImgR, comp1);
	}
	else if(mxIsUint8(mxImgR))
	{
		matlab_uint8_matrix_to_vil_image_view_float(mxImgR, comp1);
	}
	else
	{
		mexErrMsgTxt("Input image type should be either double or uint8!");
	}

	if(mxIsDouble(mxImgG))
	{
		matlab_double_matrix_to_vil_image_view_float(mxImgG, comp2);
	}
	else if(mxIsUint8(mxImgG))
	{
		matlab_uint8_matrix_to_vil_image_view_float(mxImgG, comp2);
	}
	else
	{
		mexErrMsgTxt("Input image type should be either double or uint8!");
	}

	if(mxIsDouble(mxImgB))
	{
		matlab_double_matrix_to_vil_image_view_float(mxImgB, comp3);
	}
	else if(mxIsUint8(mxImgB))
	{
		matlab_uint8_matrix_to_vil_image_view_float(mxImgB, comp3);
	}
	else
	{
		mexErrMsgTxt("Input image type should be either double or uint8!");
	}


	dbdet_edgemap_sptr edgemap = dbdet_third_order_color(0, 0,
	                                           N, sigma, grad_thresh, 0,
	                                           comp1,
	                                           comp2,
	                                           comp3,
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






