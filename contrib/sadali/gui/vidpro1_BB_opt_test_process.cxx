// This is contrib/sadali/vidpro1_my_process.cxx

//:
// \file

#include "vidpro1_BB_opt_test_process.h"
#include <bpro1/bpro1_parameters.h>

// include storage classes needed
// such as ...
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

// other includes needed
#include <bseg/brip/brip_vil_float_ops.h>
#include <bprt/bprt_plane_reconst.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d.h>

#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vnl/vnl_math.h>

#include <vil/vil_new.h>





//: Constructor
vidpro1_BB_opt_test_process::vidpro1_BB_opt_test_process()
{
    // Set up the parameters for this process
    if( 

        (!parameters()->add( "Homography Matrix1"       , "-hom1" , 1.0f  ) ) ||
        (!parameters()->add( "Homography Matrix2"       , "-hom2" , 0.0f    ) ) ||
        (!parameters()->add( "Homography Matrix3"       , "-hom3" , -4.0f    ) ) ||
        (!parameters()->add( "Homography Matrix4"       , "-hom4" , 1.0f    ) ) ||
        (!parameters()->add( "Homography Matrix5"       , "-hom5" , 1.0f    ) ) ||
        (!parameters()->add( "Homography Matrix6"       , "-hom6" , 2.0f    ) ) ||
        (!parameters()->add( "Homography Matrix7"       , "-hom7" , 0.0f    ) ) ||
        (!parameters()->add( "Homography Matrix8"       , "-hom8" , 0.0f    ) ) ||
        (!parameters()->add( "Homography Matrix9"       , "-hom9" , 1.0f    ) )

        )

    {
        vcl_cerr << "ERROR: Adding parameters in vidpro1_BB_opt_test_process::vidpro1_BB_opt_test_process()" << vcl_endl;
    }
}



//: Destructor
vidpro1_BB_opt_test_process::~vidpro1_BB_opt_test_process()
{
}


//: Return the name of this process
vcl_string
vidpro1_BB_opt_test_process::name()
{
    return "Homography Test_Img";
}


bpro1_process *
vidpro1_BB_opt_test_process::clone() const
{
    return new vidpro1_BB_opt_test_process(*this);

};
//: Return the number of input frame for this process
int
vidpro1_BB_opt_test_process::input_frames()
{
    // input from this frame 
    return 1;
}


//: Return the number of output frames for this process
int
vidpro1_BB_opt_test_process::output_frames()
{
    // output to this frame only
    return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > 
vidpro1_BB_opt_test_process::get_input_type()
{
    // this process looks for an image and vsol2D storage class
    // at each input frame
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "image" );

    return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > 
vidpro1_BB_opt_test_process::get_output_type()
{  
    // this process produces a vsol2D storage class
    vcl_vector<vcl_string > to_return;

    to_return.push_back( "image" );
    to_return.push_back( "image" );

    return to_return;
}


//: Execute the process
bool
vidpro1_BB_opt_test_process::execute()
{
    // verify that the number of input frames is correct
    if ( input_data_.size() != 1 ){
        vcl_cout << "In vidpro1_BB_opt_test_process::execute() - not exactly two"
            << " input frames" << vcl_endl;
        return false;
    }
    clear_output();

    // get vsol2d from the storage classes
    vidpro1_image_storage_sptr curr_frame_corn; 

    curr_frame_corn.vertical_cast(input_data_[0][0]);
    
    vil_image_resource_sptr origimg;
    origimg =  curr_frame_corn->get_image();
    
    vil_image_view<float> origimg_view  = origimg->get_view();
    vil_image_view<float> corr_img_view;
  



    //get parameters
    float param1;
    float param2;
    float param3;
    int   param4;
    float homparam1,homparam2,homparam3,homparam4,
        homparam5,homparam6,homparam7,homparam8,homparam9;


    parameters()->get_value( "-p1" , param1 );
    parameters()->get_value( "-p2" , param2 );
    parameters()->get_value( "-p3" , param3 );
    parameters()->get_value( "-p4" , param4 );
    parameters()->get_value( "-hom1" , homparam1 );
    parameters()->get_value( "-hom2" , homparam2 );
    parameters()->get_value( "-hom3" , homparam3 );
    parameters()->get_value( "-hom4" , homparam4 );
    parameters()->get_value( "-hom5" , homparam5 );
    parameters()->get_value( "-hom6" , homparam6 );
    parameters()->get_value( "-hom7" , homparam7 );
    parameters()->get_value( "-hom8" , homparam8 );
    parameters()->get_value( "-hom9" , homparam9 );
    vnl_double_3x3 homog_mat;
    homog_mat[0][0]=(double)homparam1;
    homog_mat[0][1]=(double)homparam2;
    homog_mat[0][2]=(double)homparam3;
    homog_mat[1][0]=(double)homparam4;
    homog_mat[1][1]=(double)homparam5;
    homog_mat[1][2]=(double)homparam6;
    homog_mat[2][0]=(double)homparam7;
    homog_mat[2][1]=(double)homparam8;
    homog_mat[2][2]=(double)homparam9;
    vgl_h_matrix_2d<double> test_homog_mat(homog_mat);
    vcl_cout<<"Homog mat:"<<homog_mat<<"\n";

    brip_vil_float_ops::homography(origimg_view,test_homog_mat,corr_img_view);
    vil_image_view<vxl_byte> byte_img(corr_img_view.ni(),corr_img_view.nj() );
    byte_img = brip_vil_float_ops::convert_to_byte(corr_img_view,0.0,1.0);


    vidpro1_image_storage_sptr output_img1 = vidpro1_image_storage_new();
    vidpro1_image_storage_sptr output_img2 = vidpro1_image_storage_new();
    vil_image_resource_sptr output_rsrc_img1 =  vil_new_image_resource_of_view(corr_img_view);
    vil_image_resource_sptr output_rsrc_img2 = vil_new_image_resource_of_view(byte_img);


    output_img1->set_image(output_rsrc_img1);
    output_img2->set_image(output_rsrc_img2);
    output_data_[0].push_back(output_img1);
    output_data_[0].push_back(output_img2);

    return true;
}


//: Finish
bool
vidpro1_BB_opt_test_process::finish()
{
    // I'm not really sure what this is for, ask Amir
    // - mleotta
    return true;
}
