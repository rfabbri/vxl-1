#include "edgemodel_image_to_vsol_process.h"
#include<bseg/brip/brip_vil_float_ops.h>
#include<vsol/vsol_point_2d.h>
#include<vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include<vidpro1/storage/vidpro1_vsol2D_storage.h>
#include<vul/vul_sprintf.h>
#include<vcl_cstdio.h>
#include<georegister/dbrl_id_point_2d_sptr.h>
#include<georegister/dbrl_id_point_2d.h>
#include"dbrl_id_point_2d_storage_sptr.h"
#include"dbrl_id_point_2d_storage.h"
edgemodel_image_to_vsol_process::edgemodel_image_to_vsol_process() : bpro1_process()
{
    if( !parameters()->add( "Intensity Threshold" ,           "-thresh" ,        (float)128) ||
        !parameters()->add( "Ouput file" ,                    "-name" ,           bpro1_filepath("","*"))||
        !parameters()->add( "Scale" ,           "-scale" ,        (float)1)
      )
    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }
    
}


/*************************************************************************
* Function Name: edgemodel_image_to_vsol_process::~edgemodel_image_to_vsol_process
* Parameters: 
* Effects: 
*************************************************************************/

edgemodel_image_to_vsol_process::~edgemodel_image_to_vsol_process()
{
   
}


//: Clone the process
bpro1_process*
edgemodel_image_to_vsol_process::clone() const
{
    return new edgemodel_image_to_vsol_process(*this);
}


/*************************************************************************
* Function Name: edgemodel_image_to_vsol_process::name
* Parameters: 
* Returns: vcl_string
* Effects: 
*************************************************************************/
vcl_string
edgemodel_image_to_vsol_process::name()
{
    return "Image to Vsol";
}


/*************************************************************************
* Function Name: dedgemodel_image_to_vsol_process::get_input_type
* Parameters: 
* Returns: vcl_vector< vcl_string >
* Effects: 
*************************************************************************/
vcl_vector< vcl_string > edgemodel_image_to_vsol_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "image" );
    return to_return;
}


/*************************************************************************
* Function Name: edgemodel_image_to_vsol_process::get_output_type
* Parameters: 
* Returns: vcl_vector< vcl_string >
* Effects: 
*************************************************************************/
vcl_vector< vcl_string > edgemodel_image_to_vsol_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;

    to_return.push_back( "vsol2D" );
    to_return.push_back( "dbrl_id_point_2d" );

    return to_return;
}
//: Returns the number of input frames to this process
int
edgemodel_image_to_vsol_process::input_frames()
{
    return 1;
}


//: Returns the number of output frames from this process
int
edgemodel_image_to_vsol_process::output_frames()
{
    return 1;
}


/*************************************************************************
* Function Name: dedgemodel_image_to_vsol_process::execute
* Parameters: 
* Returns: bool
* Effects: 
*************************************************************************/
bool
edgemodel_image_to_vsol_process::execute()
{
    if ( input_data_.size() != 1 ){
        vcl_cout << "In edgemodel_image_to_vsol_process::execute() - not exactly one"
            << " input image \n";
        return false;
    }
    clear_output();

    // get image from the storage class

    vidpro1_image_storage_sptr frame_image;
    frame_image.vertical_cast(input_data_[0][0]);

    vil_image_resource_sptr image_sptr = frame_image->get_image();

    vil_image_view< unsigned char > image_view = image_sptr->get_view(0, image_sptr->ni(), 0, image_sptr->nj() );
    vil_image_view< unsigned char > greyscale_view;

    

    if( image_view.nplanes() == 3 ) {
        vil_convert_planes_to_grey( image_view , greyscale_view );
    } 
    else if ( image_view.nplanes() == 1 ) {
        greyscale_view = image_view;
    }
    else
    {
        vcl_cerr << "Returning false. nplanes(): " << image_view.nplanes() << vcl_endl;
        return false;
    }
    bpro1_filepath filepath;
    parameters()->get_value("-name",filepath);

    char  filename[1000];//=vul_sprintf("%s%05d.%s",filepath.path,input_data_[0][0]->frame(),"txt");
    vcl_sprintf(filename, "%s%05d.%s", filepath.path.c_str(),input_data_[0][0]->frame(),"txt");
    vcl_ofstream ofile(filename);
    static float thresh=128.0;
    static float scale=1.0;
    parameters()->get_value( "-thresh" , thresh);
    parameters()->get_value( "-scale" , scale);
    vcl_vector<vsol_spatial_object_2d_sptr> points;
    vcl_vector<dbrl_id_point_2d_sptr> idpoints;
    int id=1;
    for(unsigned j=0;j<greyscale_view.nj();j++)
        for(unsigned i=0;i<greyscale_view.ni();i++)
        {
            if(greyscale_view(i,j)>thresh)
            {
                vsol_point_2d_sptr point=new vsol_point_2d(i/scale,j/scale);
                dbrl_id_point_2d_sptr idpoint=new dbrl_id_point_2d(i/scale,j/scale,id);
                vsol_spatial_object_2d_sptr spatial_point = point->cast_to_spatial_object();
                points.push_back(spatial_point);
                idpoints.push_back(idpoint);
                ofile<<i<<" "<<j<<"\n";
                id++;
            }
        }
     

    vcl_cout<<"No of points are "<<points.size();
     ofile.close();
     vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
     output_vsol->add_objects(points, "vsolpoints");
     output_data_[0].push_back(output_vsol);

     dbrl_id_point_2d_storage_sptr output_id=dbrl_id_point_2d_storage_new();
     output_id->set_id_points(idpoints);
     //output_id->mark_global();
     output_data_[0].push_back(output_id);
    //clear_input();
    return true;
}


/*************************************************************************
* Function Name: edgemodel_image_to_vsol_process::finish
* Parameters: 
* Returns: bool
* Effects: 
*************************************************************************/
bool
edgemodel_image_to_vsol_process::finish()
{
    return true;
}


