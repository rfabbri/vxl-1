// This is dbsksp/pro/elbow_load_3d_volume_process.cxx

//:
// \file

#include "elbow_load_3d_volume_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vtkStructuredPointsReader.h>
#include <vtkStructuredPoints.h>
#include <vil3d/vil3d_image_view.h>
#include "../storage/elbow_vil3d_storage_sptr.h"
#include "../storage/elbow_vil3d_storage.h"
#include <vil3d/vil3d_slice.h>

//: Constructor
elbow_load_3d_volume_process::
elbow_load_3d_volume_process()
{

	if( !parameters()->add( "Input 3D volume file (*.vtk)" , "-volfile",
			bpro1_filepath("",".vtk")) )
	{
		vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
	}
}


//: Destructor
elbow_load_3d_volume_process::
~elbow_load_3d_volume_process()
{

}


//: Clone the process
bpro1_process* elbow_load_3d_volume_process::
clone() const
{
	return new elbow_load_3d_volume_process(*this);
}

//: Returns the name of this process
vcl_string elbow_load_3d_volume_process::
name()
{ 
	return "Load 3D volume";
}

//: Provide a vector of required input types
vcl_vector< vcl_string > elbow_load_3d_volume_process::
get_input_type()
{
	vcl_vector< vcl_string > to_return;
	return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > elbow_load_3d_volume_process::
get_output_type()
{
	vcl_vector<vcl_string > to_return;
	to_return.push_back("3d_dataset");
	return to_return;
}

//: Return the number of input frames for this process
int elbow_load_3d_volume_process::
input_frames()
{
	return 0;
}


//: Return the number of output frames for this process
int elbow_load_3d_volume_process::
output_frames()
{
	return 1;
}

//: Execute this process
bool elbow_load_3d_volume_process::
execute()
{
	// 1. parse process parameters

	// filename

	bpro1_filepath volfile;
	parameters()->get_value( "-volfile" , volfile );

	vtkStructuredPointsReader *reader = vtkStructuredPointsReader::New();
	reader->SetFileName(volfile.path.c_str());
	vtkStructuredPoints* out = vtkStructuredPoints::New();
	reader->SetOutput(out);
	reader->Update();
	int* dims = out->GetDimensions();
	vcl_cout << "Image dimensions: " << dims[0] << " x " << dims[1] << " x " << dims[2] << vcl_endl;
	unsigned short* v = (unsigned short*)out->GetScalarPointer();
	vil3d_image_view<double> image(dims[1],dims[0], dims[2]);
	int num_pts_in_a_slice = dims[1]*dims[0];

	double maxVal = v[0];
	double minVal = v[0];
	for(int i = 0; i < out->GetNumberOfPoints(); i++)
	{
		int slice_index = i / num_pts_in_a_slice;
		int j = i % num_pts_in_a_slice;
		int column_index = j / dims[0];
		int row_index = j % dims[0];
		double tmp = double(v[i]);
		image(column_index, row_index, slice_index) = tmp;
		if(maxVal <  tmp)
		{
			maxVal = tmp;
		}
		if(minVal > tmp)
		{
			minVal = tmp;
		}
	}
	for(int i = 0; i < dims[1]; i++)
	{
		for(int j = 0; j < dims[0]; j++)
		{
			for(int k = 0; k < dims[2]; k++)
			{
				image(i,j,k) = 255*(image(i,j,k) - minVal)/(maxVal - minVal);
			}
		}
	}


	elbow_vil3d_storage_sptr vil3d_storage = elbow_vil3d_storage_new();
	vil3d_storage->set_image(image);
	this->output_data_[0].push_back(vil3d_storage);
}


bool elbow_load_3d_volume_process::
finish()
{
	return true;
}





