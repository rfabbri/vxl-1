//This is contrib/Gregistration/pro/dbrl_edge_image_to_oriented_edgemap_process.cxx

//:
// \file

#include "dbrl_edge_image_to_oriented_edgemap_process.h"
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>


//: Constructor
dbrl_edge_image_to_oriented_edgemap_process::
dbrl_edge_image_to_oriented_edgemap_process()
{
}

//: Destructor
dbrl_edge_image_to_oriented_edgemap_process::
~dbrl_edge_image_to_oriented_edgemap_process()
{
}

//: Clone the process
bpro1_process* dbrl_edge_image_to_oriented_edgemap_process::
clone() const
{
    return new dbrl_edge_image_to_oriented_edgemap_process(*this);
}

//: Return the name of this process
vcl_string dbrl_edge_image_to_oriented_edgemap_process::
name()
{
    return "Edge Image to Oriented Edge Map";
}

//: Return the number of input frame for this process
int dbrl_edge_image_to_oriented_edgemap_process::
input_frames()
{
    return 1;
}

//: Return the number of output frames for this process
int dbrl_edge_image_to_oriented_edgemap_process::
output_frames()
{
    return 1;
}

//: Provide a vector of required input types
vcl_vector<vcl_string> dbrl_edge_image_to_oriented_edgemap_process::
get_input_type()
{
    vcl_vector<vcl_string> to_return;
    to_return.push_back( "image" );
    return to_return;
}

//: Provide a vector of output types
vcl_vector<vcl_string> dbrl_edge_image_to_oriented_edgemap_process::
get_output_type()
{
    vcl_vector<vcl_string> to_return;
    to_return.push_back( "edge_map" );
    return to_return;
}

//: Execute the process
bool dbrl_edge_image_to_oriented_edgemap_process::
execute()
{
    vidpro1_image_storage_sptr input_img_storage;
    input_img_storage.vertical_cast(input_data_[0][0]);
    vil_image_resource_sptr image_rsc = input_img_storage->get_image();

    unsigned width = image_rsc->ni();
    unsigned height = image_rsc->nj();

    vil_image_view<float> image = image_rsc->get_view(0,width,0,height);
    vcl_vector<dbdet_edgel*> edgels;

    for(unsigned i=0; i<width; ++i)
    {
	for(unsigned j=0; j<height; ++j)
	{
	    if(image(i,j,0)!=-100.0)
	    {
		double x = image(i,j,0);
		double y = image(i,j,1);
		double orientation = image(i,j,2);

		vgl_point_2d<double> pt(x,y);
		dbdet_edgel* edgel = new dbdet_edgel(pt,orientation);
		edgels.push_back(edgel);
	    }	    
	}
    }

    dbdet_edgemap_sptr edge_map = new dbdet_edgemap(width,height,edgels);

    dbdet_edgemap_storage_sptr output_edgemap_storage = dbdet_edgemap_storage_new();
    output_edgemap_storage->set_edgemap(edge_map);
    output_data_[0].push_back(output_edgemap_storage);

    return true;
}

//: Finish the process
bool dbrl_edge_image_to_oriented_edgemap_process::
finish()
{
    return true;
}
