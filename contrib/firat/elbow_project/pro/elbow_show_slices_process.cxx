// This is dbsksp/pro/elbow_show_slices_process.cxx

//:
// \file

#include "elbow_show_slices_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vtkStructuredPointsReader.h>
#include <vtkStructuredPoints.h>
#include <vil3d/vil3d_image_view.h>
#include "../storage/elbow_vil3d_storage_sptr.h"
#include "../storage/elbow_vil3d_storage.h"
#include <vil/vil_new.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vil3d/vil3d_slice.h>

//: Constructor
elbow_show_slices_process::
elbow_show_slices_process()
{
	num_frames_ = 0;
	vcl_vector<vcl_string> choices;
	choices.push_back("X-Y Slices");
	choices.push_back("X-Z Slices");
	choices.push_back("Y-Z Slices");
	if( !parameters()->add( "2D Slice Type" , "-slicetype",
			choices, 0 ))
	{
		vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
	}
}


//: Destructor
elbow_show_slices_process::
~elbow_show_slices_process()
{

}


//: Clone the process
bpro1_process* elbow_show_slices_process::
clone() const
{
	return new elbow_show_slices_process(*this);
}

//: Returns the name of this process
vcl_string elbow_show_slices_process::
name()
{ 
	return "Show 2D slices";
}

//: Provide a vector of required input types
vcl_vector< vcl_string > elbow_show_slices_process::
get_input_type()
{
	vcl_vector< vcl_string > to_return;
	to_return.push_back("3d_dataset");
	return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > elbow_show_slices_process::
get_output_type()
{
	vcl_vector<vcl_string > to_return;
	to_return.push_back("image");
	return to_return;
}

//: Return the number of input frames for this process
int elbow_show_slices_process::
input_frames()
{
	return 1;
}


//: Return the number of output frames for this process
int elbow_show_slices_process::
output_frames()
{
	return num_frames_;
}

//: Execute this process
bool elbow_show_slices_process::
execute()
{
	// 1. parse process parameters

	unsigned slicetype;
	parameters()->get_value( "-slicetype" , slicetype );

	elbow_vil3d_storage_sptr data_storage;
	data_storage.vertical_cast(input_data_[0][0]);
	vil3d_image_view<double>& image = data_storage->image();
	num_frames_ = image.nk();
	this->clear_input();
	this->clear_output();
	vcl_cout << "Num frames = " << num_frames_ << vcl_endl;

	for(int i = 0; i < num_frames_; i++)
	{
		vil_image_view<double> slice = vil3d_slice_ij(image, num_frames_-i-1);
		vil_image_resource_sptr slice_im = vil_new_image_resource_of_view(slice);
		vidpro1_image_storage_sptr image_storage = vidpro1_image_storage_new();
		image_storage->set_image( slice_im );
		this->output_data_[i].push_back(image_storage);
	}
	return true;
}

bool elbow_show_slices_process::
finish()
{
	return true;
}





