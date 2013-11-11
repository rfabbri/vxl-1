/*
 * mex_poly_dist_trans.cxx
 *
 *  Created on: Jan 31, 2012
 *      Author: firat
 */

#include "mex.h"
#include "polygon_distance_transform.h"
#include <vcl_vector.h>
/* MATLAB COUNTERPART
function phi = polygon_distance_transform(contours, height, width, hx, hy)
*/

void mexFunction(int nlhs, mxArray *plhs[ ], int nrhs, const mxArray *prhs[ ])
{
	if(nrhs != 5)
	{
		mexErrMsgTxt("5 input arguments required!");
	}
	if(nlhs != 1)
	{
		mexErrMsgTxt("1 output argument required!");
	}
	const mxArray* contours = prhs[0]; //a cell array
	int height = *mxGetPr(prhs[1]);
	int width =  *mxGetPr(prhs[2]);
	double hx = *mxGetPr(prhs[3]);
	double hy =  *mxGetPr(prhs[4]);
	int num_contours = mxGetNumberOfElements(contours);
	int real_height = (height-1)/hy + 1;
	int real_width = (width-1)/hx + 1;
	vnl_matrix<double> phi;
	if(num_contours == 0)
	{
		mexPrintf("There is no contour!\n");
		phi.set_size(real_height, real_width);
		phi.fill(10);
	}
	else
	{
		vcl_vector<double*> xv;
		vcl_vector<double*> yv;
		vcl_vector<int> num_points;
		for(int i = 0; i < num_contours; i++)
		{
			mxArray* contour = mxGetCell(contours, i);
			double* contour_data = mxGetPr(contour);
			int num = mxGetM(contour);
			if(num < 3)
			{
				continue;
			}
			double* x = new double[num];
			double* y = new double[num];

			for(int j = 0; j < num; j++)
			{
				x[j] = contour_data[j]-1;
			}
			for(int j = 0; j < num; j++)
			{
				y[j] = contour_data[num + j]-1;
			}
			xv.push_back(x);
			yv.push_back(y);
			num_points.push_back(num);
		}
		if(xv.empty())
		{
			mexPrintf("There is no contour with more than 2 points!\n");
			phi.set_size(real_height, real_width);
		    phi.fill(10);
		}
		else
		{
			//mexPrintf("Number of polygons = %d\n", xv.size());
			compute_polygon_signed_distance_transform(xv, yv, num_points, phi, height, width, hx, hy);
		}
	}
	plhs[0] = mxCreateDoubleMatrix(real_height, real_width, mxREAL);
	double* out_ptr = mxGetPr(plhs[0]);
	int total_elems = real_height * real_width;
	for(int i = 0; i < total_elems; i++)
	{
		int col_id = i / real_height;
		int row_id = i % real_height;
		out_ptr[i] = phi(row_id, col_id);
	}
}



