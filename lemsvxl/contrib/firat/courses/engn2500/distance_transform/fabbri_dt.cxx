// This is /lemsvxl/contrib/firat/courses/engn2500/distance_transform/fabbri_dt.cxx.

// \file
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
// \date May 11, 2011

#include <dbil/algo/dbil_exact_distance_transform.h>
#include <vcl_iostream.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vcl_string.h>

void print(vil_image_view<vxl_uint_32>& image)
{
	for(int k = 0; k < image.nplanes(); k++)
	{
		for(int i = 0; i < image.ni(); i++)
		{
			for(int j = 0; j < image.nj(); j++)
			{

				vcl_cout << image(i,j,k) << " ";
			}
			vcl_cout << vcl_endl;
		}
		vcl_cout << vcl_endl << vcl_endl;
	}


}

void init(vil_image_view<vxl_uint_32>& image)
{
	for(int i = 0; i < image.ni(); i++)
	{
		for(int j = 0; j < image.nj(); j++)
		{
			for(int k = 0; k < image.nplanes(); k++)
			{
				image(i,j,k) = 1;
			}
		}
	}
	for(int i = 1; i < 10; i++)
		{
			for(int j = 1; j < 10; j++)
			{
				for(int k = 1; k < 4; k++)
				{
					image(i,j,k) = 0;
				}
			}
		}

	for(int i = 3; i < 8; i++)
			{
				for(int j = 3; j < 8; j++)
				{
					for(int k = 1; k < 4; k++)
					{
						image(i,j,k) = 1;
					}
				}
			}

}


int main(int argn, char* args[])
{
	unsigned ni=11;
	unsigned nj=11;
	unsigned nplanes=5;
	vil_image_view<vxl_uint_32> image(ni,nj,nplanes);
	init(image);
	print(image);
	bool status = dbil_exact_distance_transform_saito_3D(image);
	if(status)
	{
		vcl_cout << "calisti" << vcl_endl;
	}
	else
	{
		vcl_cout << "calismadi" << vcl_endl;
	}
	print(image);
	return 0;
}
