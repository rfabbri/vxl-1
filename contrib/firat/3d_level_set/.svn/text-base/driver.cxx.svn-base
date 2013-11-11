// This is /lemsvxl/contrib/firat/3d_level_set/driver.cxx.

// \file
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
// \date Jun 28, 2011

#include <vtk-5.2/vtkStructuredPointsReader.h>
#include <vtk-5.2/vtkStructuredPoints.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_save.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil3d/vil3d_slice.h>

#include <vcl_iostream.h>

int main()
{
	vtkStructuredPointsReader *reader = vtkStructuredPointsReader::New();
	reader->SetFileName("/vision/images/medical/elbow-dog/data/TransverseCT_rightSide.vtk");
	vtkStructuredPoints* out = vtkStructuredPoints::New();
	reader->SetOutput(out);
	reader->Update();
	int* dims = out->GetDimensions();
	vcl_cout<< dims[0] << " " << dims[1] << " " << dims[2]<<vcl_endl;
	unsigned short* v = (unsigned short*)out->GetScalarPointer();
	vcl_cout << v[0] << " " << v[1] << " " << v[3499] << " " << v[5672] << " " << v[3150012] << vcl_endl;
	vil3d_image_view<unsigned short> image(dims[1],dims[0], dims[2]);
	int num_pts_in_a_slice = dims[1]*dims[0];
	for(int i = 0; i < out->GetNumberOfPoints(); i++)
	{
		int slice_index = i / num_pts_in_a_slice;
		int j = i % num_pts_in_a_slice;
		int column_index = j / dims[0];
		int row_index = j % dims[0];
		image(column_index, row_index, slice_index) = v[i];
	}
	vil_image_view<unsigned short> slice = vil3d_slice_ij(image, 0);
	vil_save(slice, "/home/firat/Desktop/abc.png");
	return 0;
}
