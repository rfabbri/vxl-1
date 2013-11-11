#include "dbil_osl_canny_subpixel_edges_process.h"
#include<vdgl/vdgl_digital_curve_sptr.h>
#include<vdgl/vdgl_interpolator_sptr.h>
#include<vdgl/vdgl_digital_curve.h>
#include<vdgl/vdgl_edgel_chain.h>
#include<vtol/vtol_edge_2d.h>
#include<bseg/brip/brip_vil_float_ops.h>
#include<vdgl/vdgl_digital_curve_sptr.h>
#include<vdgl/vdgl_interpolator_sptr.h>
#include<vdgl/vdgl_interpolator.h>
#include<vdgl/vdgl_digital_curve.h>
#include<vsol/vsol_polyline_2d_sptr.h>
#include<vsol/vsol_line_2d_sptr.h>
#include<vsol/vsol_line_2d.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_vil.h>
#include<vsol/vsol_point_2d.h>
#include<vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include<vidpro1/storage/vidpro1_vsol2D_storage.h>
#include<georegister/dbrl_id_point_2d_sptr.h>
#include<georegister/dbrl_id_point_2d.h>
#include<pro/dbrl_id_point_2d_storage.h>
#include<pro/dbrl_id_point_2d_storage_sptr.h>
#include <osl/osl_edge.h>
#include <osl/osl_canny_ox.h>
#include <osl/osl_canny_ox_params.h>
#include <osl/osl_edgel_chain.h>
#include <sdet/sdet_nonmax_suppression.h>
#include <sdet/sdet_nonmax_suppression_params.h>
#include <vnl/vnl_math.h>
#include <vil/vil_plane.h>
#include <vil/algo/vil_sobel_3x3.h>

dbil_osl_canny_subpixel_edges_process::dbil_osl_canny_subpixel_edges_process() : bpro1_process(),dp()
{
    if( //!parameters()->add( "Sigma" ,"-sigma" ,(float)1.0 ) ||
        !parameters()->add( "Threshold" ,  "-thresh" ,  float(10) ) 
        //!parameters()->add( "High threshold" , "-howthresh" ,  float(12.0) ) ||
        //!parameters()->add( "Border Size" , "-bordersize" ,  int(2) ) ||
        //!parameters()->add( "Border Intesnity Value" , "-borderint" ,  float(0.0) ) ||
        //!parameters()->add( "Scale" , "-scale" ,  float(5.0) ) ||
        //!parameters()->add( "follow strategy" , "-follow strategy" ,  int(2) ) || 
        //!parameters()->add( "Min length of curves" ,      "-lenofcurves" ,       (int)10 )||
        //!parameters()->add( "Min Edgel Intensity" ,      "-minintensity" ,       (int)60 )  
        )
    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }
    
}


/*************************************************************************
* Function Name: dbil_osl_canny_subpixel_edges_process::~dbil_osl_canny_subpixel_edges_process
* Parameters: 
* Effects: 
*************************************************************************/

dbil_osl_canny_subpixel_edges_process::~dbil_osl_canny_subpixel_edges_process()
{
   
}


//: Clone the process
bpro1_process*
dbil_osl_canny_subpixel_edges_process::clone() const
{
    return new dbil_osl_canny_subpixel_edges_process(*this);
}


/*************************************************************************
* Function Name: dbil_osl_canny_subpixel_edges_process::name
* Parameters: 
* Returns: vcl_string
* Effects: 
*************************************************************************/
vcl_string
dbil_osl_canny_subpixel_edges_process::name()
{
    return "Binary Subpixel Tangent Osl Canny edges";
}


/*************************************************************************
* Function Name: ddbil_osl_canny_edges_process::get_input_type
* Parameters: 
* Returns: vcl_vector< vcl_string >
* Effects: 
*************************************************************************/
vcl_vector< vcl_string > dbil_osl_canny_subpixel_edges_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "image" );
    return to_return;
}


/*************************************************************************
* Function Name: dbil_osl_canny_subpixel_edges_process::get_output_type
* Parameters: 
* Returns: vcl_vector< vcl_string >
* Effects: 
*************************************************************************/
vcl_vector< vcl_string > dbil_osl_canny_subpixel_edges_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "image" );
    to_return.push_back( "dbrl_id_point_2d" );
    to_return.push_back( "vsol2D" );
    return to_return;
}
//: Returns the number of input frames to this process
int
dbil_osl_canny_subpixel_edges_process::input_frames()
{
    return 1;
}


//: Returns the number of output frames from this process
int
dbil_osl_canny_subpixel_edges_process::output_frames()
{
    return 1;
}


/*************************************************************************
* Function Name: ddbil_osl_canny_edges_process::execute
* Parameters: 
* Returns: bool
* Effects: 
*************************************************************************/
bool
dbil_osl_canny_subpixel_edges_process::execute()
{
    if ( input_data_.size() != 1 ){
        vcl_cout << "In dbil_osl_canny_subpixel_edges_process::execute() - not exactly one"
            << " input image \n";
        return false;
    }
    clear_output();


    // get image from the storage class

    vidpro1_image_storage_sptr frame_image;
    frame_image.vertical_cast(input_data_[0][0]);
    vil_image_resource_sptr image_sptr = frame_image->get_image();
    vil_image_view<unsigned char> grey_img=brip_vil_float_ops::convert_to_byte(image_sptr);
    // Convert to greyscale images if needed
    
    static float sigma;
    static float thresh;

    //parameters()->get_value( "-sigma" ,  sigma);
    parameters()->get_value( "-thresh" ,thresh);
    sdet_nonmax_suppression_params nonmax_paprams(thresh,3);
    vil_image_view< double > Ix(image_sptr->ni(),image_sptr->nj());
    vil_image_view< double > Iy(image_sptr->ni(),image_sptr->nj());
    vil_sobel_3x3<unsigned char,double>(grey_img,Ix,Iy);

    sdet_nonmax_suppression det(nonmax_paprams,Ix,Iy);
    det.apply();
    
    vil_image_view<float> dir_img(image_sptr->ni(),image_sptr->nj(),3);
    dir_img.fill(-100.0);
    //vcl_vector<dbrl_id_point_2d_sptr> pointids;

    vcl_vector<vsol_point_2d_sptr> edges=det.get_points();
    //vcl_vector<double> tangents=det.get_tangents();
    vcl_vector<vgl_vector_2d<double> > dirs=det.get_directions();

    vcl_vector<vsol_spatial_object_2d_sptr> objs;
    for(unsigned i=0;i<edges.size();i++)
        {
        dir_img((unsigned int)vcl_floor(edges[i]->x()),(unsigned int)vcl_floor(edges[i]->y()),0)=edges[i]->x();
        dir_img((unsigned int)vcl_floor(edges[i]->x()),(unsigned int)vcl_floor(edges[i]->y()),1)=edges[i]->y();
        double dir=vcl_atan2(dirs[i].y(),dirs[i].x());
        dir_img((unsigned int)vcl_floor(edges[i]->x()),(unsigned int)vcl_floor(edges[i]->y()),2)=dir;
        }

vcl_cout<<"No fo Edges are "<<edges.size()<<"\n";
vidpro1_image_storage_sptr output_img_storage = vidpro1_image_storage_new();
output_img_storage->set_image(vil_new_image_resource_of_view(dir_img ) );
output_data_[0].push_back(output_img_storage);
output_img_storage->set_name("Dir OSL Canny Edges");

//dbrl_id_point_2d_storage_sptr id_point_2d_storage=  dbrl_id_point_2d_storage_new();
//id_point_2d_storage->set_id_points(pointids);
//output_data_[0].push_back(id_point_2d_storage); 


//vidpro1_vsol2D_storage_sptr vsol_storage=vidpro1_vsol2D_storage_new();
//vsol_storage->add_objects(objs);
//output_data_[0].push_back(vsol_storage); 

clear_input();
return true;
}


/*************************************************************************
* Function Name: dbil_osl_canny_subpixel_edges_process::finish
* Parameters: 
* Returns: bool
* Effects: 
*************************************************************************/
bool
dbil_osl_canny_subpixel_edges_process::finish()
{
    return true;
}


