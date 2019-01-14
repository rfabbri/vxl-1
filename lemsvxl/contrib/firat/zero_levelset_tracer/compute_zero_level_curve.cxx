// This is /lemsvxl/contrib/firat/zero_levelset_tracer/compute_zero_level_curve.cxx.

// \file
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
// \date Aug 15, 2011

#include "../dbdet_contour_tracer/dbdet_contour_tracer.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vil/vil_image_view.h>
#include <vector>
#include <cstdio>
#include <string>
#include <cstdlib>
#include <vnl/vnl_matrix.h>

int main(int argn, char* args[])
{


	std::string surface_file = args[1];
	int height = std::atoi(args[2]);
	int width = std::atoi(args[3]);
	std::string out_file = args[4];
	vnl_matrix<double> surface_matrix(height,width);
	std::ifstream ifs(surface_file.c_str());
	surface_matrix.read_ascii(ifs);
	ifs.close();
	vil_image_view<double > surface(width, height, 1);
	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			surface(j,i) = surface_matrix(i,j);
		}
	}
	dbdet_contour_tracer tracer;
	tracer.set_curvature_smooth_nsteps(0);
	tracer.trace_sedt_image_with_shock_capturing(surface);
	std::vector<std::vector<vsol_point_2d_sptr> > contours = tracer.contours();
	std::ofstream ofs(out_file.c_str());
	for(int i = 0 ; i < contours.size(); i++)
	{
		std::vector<vsol_point_2d_sptr> contour = contours[i];
		if(contour.size() > 2)
		{
			ofs << contour.size() << " " << 0 << std::endl;
			for(int j = 0; j < contour.size(); j++)
			{
				ofs << contour[j]->x() << " " << contour[j]->y() << std::endl;
			}
		}

	}
	ofs.close();

	return 0;
}
