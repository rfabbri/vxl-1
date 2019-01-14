// This is /lemsvxl/contrib/firat/zero_levelset_tracer/zero_levelset_tracer.cxx.

// \file
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
// \date Jul 20, 2011

#include <dbdet/tracer/dbdet_contour_tracer.h>
#include <cmath>
#include <iostream>
#include <vil/vil_image_view.h>
#include <vector>
#include <dbsol/algo/dbsol_geno.h>

void flip_phi(vil_image_view<float >& surface, vil_image_view<float >& surface2, int width, int height)
{
	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			surface2(width-j-1, height-i-1) = surface(j,i);
		}
	}
}

void flip_contours(std::vector<std::vector<vsol_point_2d_sptr> >& contours, int width, int height)
{
	for(int i = 0 ; i < contours.size(); i++)
	{
		std::vector<vsol_point_2d_sptr> contour = contours[i];
		for(int j = 0; j < contour.size(); j++ )
		{
			vsol_point_2d_sptr pt = contour[j];
			pt->set_x(width - 1 - pt->x());
			pt->set_y(height - 1 - pt->y());
		}
	}
}


int main()
{
	int height = 101;
	int width = 101;

	vil_image_view<float > surface(width, height, 1);
	vil_image_view<float > surface2(width, height, 1);

	if(0)
	{
		int center = (width-1)/2;
		for(int i = 0; i < height; i++)
		{
			for(int j = 0; j < width; j++)
			{
				surface(j,i) = std::sqrt((i-center)*(i-center) + (j-center)*(j-center)) - 19.68;
			}
		}
	}
	else if(1)
	{
		double center1 = 19.5;
		double center2 = 39.2;
		double radius1 = 11.15;
		double radius2 = 8.5;
		for(int i = 0; i < height; i++)
		{
			for(int j = 0; j < width; j++)
			{
				double d1 = std::sqrt((i-25)*(i-25) + (j-center1)*(j-center1))-radius1;
				double d2 = std::sqrt((i-25)*(i-25) + (j-center2)*(j-center2))-radius2;
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
			std::cout << surface(i,25) << " ";
		}
		std::cout <<std::endl;
	}
	else
	{
		double center_x = 50, center_y = 50;
		double radius_outer = 41.8;
		double radius_inner = 41.7;
		for(int y = 0; y < height; y++)
		{
			for(int x = 0; x < width; x++)
			{

				double dist_to_center = std::sqrt((x-center_x)*(x-center_x) + (y-center_y)*(y-center_y));
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

	flip_phi(surface, surface2, width, height);

	dbdet_contour_tracer tracer;
	tracer.set_curvature_smooth_nsteps(0);
	tracer.trace_sedt_image_with_shock_capturing(surface);
	std::vector<std::vector<vsol_point_2d_sptr> > contours1 = tracer.contours();

	dbdet_contour_tracer tracer2;
	tracer2.set_curvature_smooth_nsteps(0);
	tracer2.trace_sedt_image_with_shock_capturing(surface2);
	std::vector<std::vector<vsol_point_2d_sptr> > contours2 = tracer2.contours();
	flip_contours(contours2, width, height);

	std::vector<std::vector<vsol_point_2d_sptr> > contours;
	contours.reserve(contours1.size() + contours2.size());
	contours.insert(contours.end(), contours1.begin(), contours1.end());
	contours.insert(contours.end(), contours2.begin(), contours2.end());


	std::ofstream ofs("/home/firat/Desktop/subpixeltrace3.txt");
	for(int i = 0 ; i < contours.size(); i++)
	{
		std::vector<vsol_point_2d_sptr> contour = contours[i];
		ofs << contour.size() << " " << 0 << std::endl;
		for(int j = 0; j < contour.size(); j++)
		{
			ofs << contour[j]->x() << " " << contour[j]->y() << std::endl;
		}

	}

	ofs.close();



	return 0;
}
