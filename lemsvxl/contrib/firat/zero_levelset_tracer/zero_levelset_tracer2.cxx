// This is /lemsvxl/contrib/firat/zero_levelset_tracer/zero_levelset_tracer2.cxx.

// \file
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
// \date Jul 31, 2011

#include "../dbdet_contour_tracer/dbdet_contour_tracer.h"
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vil/vil_image_view.h>
#include <vcl_vector.h>
#include <dbsol/algo/dbsol_geno.h>
#include <vcl_cstdio.h>

int main()
{
	int height =201;
	int width = 201;

	vil_image_view<double > surface(width, height, 1);

	if(0)
	{
		int center = (width-1)/2;
		for(int i = 0; i < height; i++)
		{
			for(int j = 0; j < width; j++)
			{
				surface(j,i) = vcl_sqrt((i-center)*(i-center) + (j-center)*(j-center)) - 2.1;
			}
		}
	}
	else if(0)
	{
		/*double center1 = 19.5;
		double center2 = 39.2;
		double radius1 = 11.1;
		double radius2 = 8.5;*/

		double center1 = 48.3;
		double center2 = 54.9;
		double radius1 = 3.1;
		double radius2 = 3.1;

		for(int i = 0; i < height; i++)
		{
			for(int j = 0; j < width; j++)
			{
				double d1 = vcl_sqrt((i-25)*(i-25) + (j-center1)*(j-center1))-radius1;
				double d2 = vcl_sqrt((i-25)*(i-25) + (j-center2)*(j-center2))-radius2;
				if(d1 < d2)
				{
					surface(j,i) = d1;
					//surface2(width-j-1,height -i -1) = d1;
				}
				else
				{
					surface(j,i) = d2;
					//surface2(width-j-1,height -i -1) = d2;
				}

			}
		}
		for(int i = 0; i < width; i++)
		{
			vcl_cout << surface(i,25) << " ";
		}
		vcl_cout <<vcl_endl;
	}
	else if(1)
	{
		double center_x = 50, center_y = 50;
		double radius_outer = 30.9;
		double radius_inner = 30.7;
		for(int y = 0; y < height; y++)
		{
			for(int x = 0; x < width; x++)
			{

				double dist_to_center = vcl_sqrt((x-center_x)*(x-center_x) + (y-center_y)*(y-center_y));
				double d_outer = radius_outer - dist_to_center;
				double d_inner = dist_to_center - radius_inner;
				if(dist_to_center > radius_outer)
				{
					surface(x,y) = -d_outer;
				}
				else if(dist_to_center < radius_inner)
				{
					surface(x,y) = -d_inner;
				}
				else
				{
					double min_d = d_outer < d_inner ? d_outer : d_inner;
					surface(x,y) = -min_d;
				}
			}
		}
	}
	else
	{

	}

	/*double* surface_array = new double[height*width];
	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			surface_array[y*width + x] = surface(x,y);
		}
	}

	vcl_FILE* f1 = fopen("/home/firat/Desktop/surface.img", "w");
	vcl_fwrite(surface_array,sizeof(double),(height*width),f1);
	vcl_fclose(f1);
	delete[] surface_array;*/


	dbdet_contour_tracer tracer;
	tracer.set_curvature_smooth_nsteps(0);
	tracer.trace_sedt_image_with_shock_capturing(surface);
	vcl_vector<vcl_vector<vsol_point_2d_sptr> > contours = tracer.contours();

	vcl_cout << "Number of Contours = " << contours.size() << vcl_endl;

	vcl_ofstream ofs("/home/firat/Desktop/subpixeltrace4.txt");
	for(int i = 0 ; i < contours.size(); i++)
	{
		vcl_vector<vsol_point_2d_sptr> contour = contours[i];
		//if(contour.size() > 2)
		{
			ofs << contour.size() << " " << 0 << vcl_endl;
			for(int j = 0; j < contour.size(); j++)
			{
				ofs << contour[j]->x() << " " << contour[j]->y() << vcl_endl;
			}
		}

	}

	ofs.close();



	return 0;
}
