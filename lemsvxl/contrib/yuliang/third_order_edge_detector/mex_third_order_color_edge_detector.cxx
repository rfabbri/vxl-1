/*
 * mex_third_order_color_edge_detector.cxx
 *
 *  This is a second version dealing with image border and provide the option to use LAB space
 *  Created on: March, 2017
 *      Author: Yuliang
 *      function edg = mex_third_order_color_edge_detector(imgRGB, use_lab, sigma, grad_thresh, N, w, h)
 */

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_cstdlib.h>
#include <dbdet/algo/dbdet_third_order_color_detector.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_plane.h>
#include <vil/vil_convert.h>
#include <vil/vil_border.h>
#include <vil/vil_fill.h>
#include <bil/algo/bil_color_conversions.h>

#include "../mex/vxl_mex_utils.h"


void mexFunction(int nlhs, mxArray *plhs[ ], int nrhs, const mxArray *prhs[ ])
{
    /*
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
    //bool use_lab = *mxGetPr(prhs[3]);
    double sigma = *mxGetPr(prhs[3]);
    double grad_thresh = *mxGetPr(prhs[4]);
    int N = *mxGetPr(prhs[5]);

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
    */


    if(nrhs != 7)
    {
        mexErrMsgTxt("7 input arguments required! (imgRGB, use_lab, sigma, grad_thresh, N, w, h)");
    }
    const mxArray* mxImgRGB = prhs[0];
    bool use_lab = *mxGetPr(prhs[1]);
    double sigma = *mxGetPr(prhs[2]);
    double grad_thresh = *mxGetPr(prhs[3]);
    int N = *mxGetPr(prhs[4]);
    int ni = *mxGetPr(prhs[5]);//num of columns: width -> ni
    int nj = *mxGetPr(prhs[6]);//num of rows: height -> nj

    //int *pDims;
    //pDims = mxGetDimensions(prhs[0]);

    //int ni = pDims[1];//num of columns: width -> ni
    //int nj = pDims[0];//num of rows: height -> nj
    vil_image_view<vxl_byte> col_image;
    col_image.set_size(ni, nj, 3);
    vxl_byte* img_data = (vxl_byte*) (void*) mxGetPr(mxImgRGB);
    col_image.set_to_memory(img_data, ni, nj, 3, nj, 1, ni*nj);
    if (col_image.nplanes() != 3)
          mexErrMsgTxt("In dbdet_third_order_color_edge_detector_process::execute() - image must be trichromatic! \n");

    // add padding so that to recover edge on the border of the image
    int padding=10;
      vil_image_view<vxl_byte> padded_img;
      padded_img.set_size(
          col_image.ni()+2*padding,col_image.nj()+2*padding,
          col_image.nplanes());
      vil_fill(padded_img, vxl_byte(0));

      vil_border_accessor<vil_image_view<vxl_byte> >
        accessor = vil_border_create_accessor(
            col_image,
            vil_border_create_geodesic(col_image));

      int j_max = (int)(padded_img.nj())-padding;
      int i_max = (int)(padded_img.ni())-padding;

      for (int p=0;p<padded_img.nplanes();++p)
      {
          for (int j = -padding ; j < j_max;++j)
          {
              for (int i=-padding;i < i_max;++i)
              {
                  padded_img(i+padding,j+padding,p)=accessor(i,j,p);
              }
          }
      }

    //3) convert to the desired color space
    vil_image_view<float> comp1, comp2, comp3;
    if(use_lab) // LAB space
         convert_RGB_to_Lab(padded_img, comp1, comp2, comp3);
    else{ // RGB space
        vil_convert_cast(vil_plane(padded_img, 0), comp1);
        vil_convert_cast(vil_plane(padded_img, 1), comp2);
        vil_convert_cast(vil_plane(padded_img, 2), comp3);
    }


	dbdet_edgemap_sptr edgemap = dbdet_third_order_color(0, 0,
	                                           N, sigma, grad_thresh, 0,
	                                           comp1,
	                                           comp2,
	                                           comp3,
	                                           false);

    //create a new edgemap from the tokens collected from NMS
    dbdet_edgemap_sptr padded_edge_map = new dbdet_edgemap(col_image.ni(),
                                                  col_image.nj());

    vcl_vector<dbdet_edgel*> padded_edges=edgemap->edgels;
    for ( unsigned int i=0; i < padded_edges.size() ; ++i)
    {
        vgl_point_2d<double> new_location;
        new_location.set(padded_edges[i]->pt.x()-(double)padding,
                       padded_edges[i]->pt.y()-(double)padding);
        padded_edges[i]->pt.set(new_location.x(),new_location.y());

        if ( (new_location.x() >= 0) &&
           (new_location.x() <= (double)col_image.ni()-1) &&
           (new_location.y() >= 0) &&
           (new_location.y() <= (double)col_image.nj()-1))
        {

          padded_edge_map->
              insert(new dbdet_edgel(padded_edges[i]->pt,
                                     padded_edges[i]->tangent,
                                     padded_edges[i]->strength,
                                     padded_edges[i]->deriv));

        }
    }

    // output in matlab coordinates
    edgemap=0;
    int num_edges = padded_edge_map->num_edgels();
	plhs[0] = mxCreateDoubleMatrix(num_edges, 4, mxREAL);
	double* out_ptr = mxGetPr(plhs[0]);
    vcl_vector<dbdet_edgel*> edges = padded_edge_map->edgels;
	for(int i = 0; i < num_edges; i++)
	{
		out_ptr[i] = edges[i]->pt.x()+1;
		out_ptr[i + num_edges] = edges[i]->pt.y()+1;
		out_ptr[i + 2*num_edges] = edges[i]->tangent;
		out_ptr[i + 3*num_edges] = edges[i]->strength;
	}
}






