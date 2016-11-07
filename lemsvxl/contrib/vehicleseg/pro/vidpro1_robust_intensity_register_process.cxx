// This is brl/vidpro1/process/vidpro1_kl_affine_register_process.cxx

//:
// \file
#include<vcl_cstdio.h>
#include "vidpro1_robust_intensity_register_process.h"
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <bpro1/bpro1_parameters.h>
#include <vsol/vsol_point_2d.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vimt/vimt_resample_bilin.h>
#include <brip/brip_vil_float_ops.h>
#include <vil/algo/vil_histogram.h>
#include <vil/vil_save.h>
#include <bvis1/bvis1_manager.h>
#include <dbvrl/dbvrl_minimizer.h>
#include <dbvrl/dbvrl_world_roi.h>
#include <dbvrl/dbvrl_transform_2d.h>
#include <dbvrl/dbvrl_transform_2d_sptr.h>
#include <dbvrl/dbvrl_region.h>
#include <dbvrl/dbvrl_region_sptr.h>
#include <dbvrl/pro/dbvrl_region_storage.h>
#include <dbvrl/pro/dbvrl_region_storage_sptr.h>
#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_binary_closing.h>
#include <vil/algo/vil_binary_opening.h>
#include <vil/algo/vil_binary_dilate.h>
#include <vil/algo/vil_binary_erode.h>
#include <vil/algo/vil_histogram.h>
#include <vidpro1/vidpro1_repository.h>

#include <vul/vul_timer.h>
#include <bsta/bsta_otsu_threshold.h>

//: Constructor
vidpro1_robust_intensity_register_process::vidpro1_robust_intensity_register_process(void): bpro1_process(),total_xform()
{

    if(!parameters()->add( "Amount of Motion in Pixels" , "-lmotion" , (float)5.0)||
        !parameters()->add( "Border Size (pixels)" , "-border" , (int)10 ) ||
        !parameters()->add( "First Frame" ,          "-first" ,  (bool)first_frame_ )||
        !parameters()->add( "No of Bins" ,          "-numbins" ,  (int)20) ||
        !parameters()->add( "L range" ,          "-lrange" ,  (float)0.0) ||
        !parameters()->add( "H range" ,          "-hrange" ,  (float)20.0) ||
        !parameters()->add( "Radius for Erosion" ,          "-rerode" ,  (float)2.0) ||
        !parameters()->add( "Radius for Dilation" ,          "-rdilate" ,  (float)7.0) 
        )
    {
        vcl_cerr << "ERROR: Adding parameters in vidpro1_robust_intensity_register_process::vidpro1_kl_affine_register_process()" << vcl_endl;
    }
    else
    {

    }
    total_xform.set_identity();
    first_frame_no=0;
    last_frame_no=0;
}

//: Destructor
vidpro1_robust_intensity_register_process::~vidpro1_robust_intensity_register_process()
{
}


//: Return the name of this process
vcl_string
vidpro1_robust_intensity_register_process::name()
{
    return "Robust Registration";
}


//: Return the number of input frame for this process
int
vidpro1_robust_intensity_register_process::input_frames()
{
    return 2;
}


//: Return the number of output frames for this process
int
vidpro1_robust_intensity_register_process::output_frames()
{
    return 3;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > vidpro1_robust_intensity_register_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "image" );
    return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > vidpro1_robust_intensity_register_process::get_output_type()
{  
    vcl_vector<vcl_string > to_return;
    to_return.push_back( "image" );
    to_return.push_back( "image" );
    //to_return.push_back( "image" );
    to_return.push_back( "bvrl" );
    return to_return;
}


//: Execute the process
bool
vidpro1_robust_intensity_register_process::execute()
{
    if ( input_data_.size() != 2 ){
        vcl_cout << "In vidpro1_robust_intensity_register_process::execute() - "
            << "not exactly two input images \n";
        return false;
    }
    clear_output();
    parameters()->get_value( "-rerode" , eroderadius );
    parameters()->get_value( "-rdilate" , dilateradius );

    parameters()->get_value( "-first" , first_frame_ );
        
        if(first_frame_){
            first_frame_no=input_data_[0][0]->frame();
            last_frame_no=input_data_[0][0]->frame();

        vidpro1_image_storage_sptr frame_image;
        frame_image.vertical_cast(input_data_[0][0]);
        vil_image_resource_sptr image = frame_image->get_image();
        vil_image_view<float> float_curr_view=brip_vil_float_ops::convert_to_float(*image);
        ni_= float_curr_view.ni();
        nj_= float_curr_view.nj();


         vidpro1_image_storage_new input_image_storage;
        input_image_storage->set_image(image);
        output_data_[0].push_back(input_image_storage);
        input_image_storage->set_name("Input");



        vidpro1_image_storage_new output_image_storage1;
        output_image_storage1->set_image(image);
        output_data_[0].push_back(output_image_storage1);
        output_image_storage1->set_name("RegImg");

        //vcl_cout<<output_data_[0][output_data_[0].size()-1]->name();

        float mean;
        vil_math_mean_and_variance<float,float>( mean,sigma_first_image_,float_curr_view,0);


        total_xform.set_identity();
        parameters()->set_value( "-first" , false );

        dbvrl_region_sptr curr_region = new dbvrl_region( ni_, nj_ );

        dbvrl_transform_2d_sptr xform = new dbvrl_transform_2d(total_xform);
        dbvrl_region_storage_sptr xform_storage=new dbvrl_region_storage( curr_region, xform ); 
        output_data_[0].push_back(xform_storage);
        xform_storage->set_name("Gxform");

        box_.update(total_xform(0,0).x(),total_xform(0,0).y());
        box_.update(total_xform(0,nj_).x(),total_xform(0,nj_).y());
        box_.update(total_xform(ni_,0).x(),total_xform(ni_,0).y());
        box_.update(total_xform(ni_,nj_).x(),total_xform(ni_,nj_).y());

        return true;
    }
    // get image from the storage class
    vidpro1_image_storage_sptr curr_image, last_image;

    curr_image.vertical_cast(input_data_[0][0]);
    last_image.vertical_cast(input_data_[1][0]);

   
    vil_image_resource_sptr curr_img_sptr = curr_image->get_image();
    vil_image_resource_sptr last_img_sptr = last_image->get_image();

    

    vil_image_view<float> float_curr_view= brip_vil_float_ops::convert_to_float(*curr_img_sptr);
    vil_image_view<float> float_last_view= brip_vil_float_ops::convert_to_float(*last_img_sptr);

    vidpro1_image_storage_new input_image_storage;
    input_image_storage->set_image(vil_new_image_resource_of_view(brip_vil_float_ops::convert_to_byte(float_curr_view)));
    output_data_[0].push_back(input_image_storage);
    input_image_storage->set_name("Input");

    ni_= float_curr_view.ni();
    nj_= float_curr_view.nj();
   
    vimt_transform_2d init_xform=register_image_with_optical_flow(float_curr_view,float_last_view);
    
    total_xform = total_xform * init_xform.inverse();

    vimt_transform_2d ftxform=total_xform.inverse();//init_xform;
    vimt_image_2d_of<float> sample_im;

    vgl_point_2d<double> p(0,0);
    vgl_vector_2d<double> u(1,0);
    vgl_vector_2d<double> v(0,1);
    
    vimt_image_2d_of<float> curr_img(float_curr_view,ftxform);
    vimt_resample_bilin(curr_img,sample_im,p,
        u,v,ni_,nj_);

    vidpro1_image_storage_sptr regimg_storage = vidpro1_image_storage_new();
    regimg_storage->set_image(vil_new_image_resource_of_view( brip_vil_float_ops::convert_to_byte(sample_im.image())));
    output_data_[0].push_back(regimg_storage);
    regimg_storage->set_name("RegImg");
    
    dbvrl_region_sptr curr_region = new dbvrl_region( ni_, nj_ );

    dbvrl_transform_2d_sptr xform = new dbvrl_transform_2d(ftxform);
    dbvrl_region_storage_sptr xform_storage=new dbvrl_region_storage( curr_region, xform ); 
    output_data_[0].push_back(xform_storage);
    xform_storage->set_name("Gxform");
    
    box_.update(total_xform(0,0).x(),total_xform(0,0).y());
    box_.update(total_xform(0,nj_).x(),total_xform(0,nj_).y());
    box_.update(total_xform(ni_,0).x(),total_xform(ni_,0).y());
    box_.update(total_xform(ni_,nj_).x(),total_xform(ni_,nj_).y());

    last_frame_no++;
    return true;  
}
//: Clone the process
bpro1_process*
vidpro1_robust_intensity_register_process::clone() const
{
    return new vidpro1_robust_intensity_register_process(*this);
}

bool
vidpro1_robust_intensity_register_process::finish()
{
    vcl_cout<<box_;

    int bimg_ni=(int)vcl_ceil(box_.max()[0]-box_.min()[0]);
    int bimg_nj=(int)vcl_ceil(box_.max()[1]-box_.min()[1]);

    int offset_i=(int)vcl_ceil(0-box_.min()[0]);
    int offset_j=(int)vcl_ceil(0-box_.min()[1]);


    for(int i=first_frame_no;i<=last_frame_no;i++)
    {
        vidpro1_image_storage_sptr im;
        im.vertical_cast(bvis1_manager::instance()->repository()->get_data_by_name_at("RegImg",i));
        
        vidpro1_image_storage_sptr iminput;
        iminput.vertical_cast(bvis1_manager::instance()->repository()->get_data_by_name_at("Input",i));
        vil_image_resource_sptr curr_img_sptr = iminput->get_image();
        vil_image_view<float> float_curr_view= brip_vil_float_ops::convert_to_float(*curr_img_sptr);

        dbvrl_region_storage_sptr xform;
        xform.vertical_cast(bvis1_manager::instance()->repository()->get_data_by_name_at("Gxform",i));



         if(xform && curr_img_sptr)
        {
            vimt_transform_2d ftxform=*(xform->transform().ptr());
            vimt_image_2d_of<float> sample_im;

            vgl_point_2d<double> p(-offset_i,-offset_j);
            vgl_vector_2d<double> u(1,0);
            vgl_vector_2d<double> v(0,1);


            vimt_image_2d_of<float> curr_img(float_curr_view,ftxform);
            vimt_resample_bilin(curr_img,sample_im,p,
                u,v,bimg_ni,bimg_nj);

            vcl_cout<<sample_im.image().ni()<<" "<<sample_im.image().nj()<<"\n";

            vidpro1_image_storage_sptr mosaic=vidpro1_image_storage_new();
            mosaic->set_image(vil_new_image_resource_of_view(brip_vil_float_ops::convert_to_byte(sample_im.image())));
            //bvis1_manager::instance()->repository()->new_data_at("image","Mosaic",i);
            mosaic->set_name("Mosaic");
            bvis1_manager::instance()->repository()->store_data_at(mosaic,i);

            vil_save(brip_vil_float_ops::convert_to_byte(sample_im.image()),"d:\\temp.tif");
            bvis1_manager::instance()->add_to_display(mosaic);
        }
    }

    return true;
}




vimt_transform_2d vidpro1_robust_intensity_register_process::register_image_with_optical_flow(vil_image_view<float> float_curr_view,
                                                                                   vil_image_view<float> float_last_view)
{

    ni_=float_curr_view.ni();
    nj_=float_curr_view.nj();

    vil_image_view<float> vx(ni_,nj_);
    vil_image_view<float> vy(ni_,nj_);
    vil_image_view<float> vxvx(ni_,nj_);
    vil_image_view<float> vyvy(ni_,nj_);
    vil_image_view<float> vsum(ni_,nj_);

    brip_vil_float_ops::Horn_SchunckMotion(float_curr_view,
        float_last_view,
        vx,vy,0.5,10);

    //: computing the mean of the velocities of the pixels
    float vxmean,vymean;
    vil_math_mean<float,float>(vxmean,vx,0);
    vil_math_mean<float,float>(vymean,vy,0);
    //: subtracting the mean
    vil_math_scale_and_offset_values<float,float>(vx,1.0,-vxmean);
    vil_math_scale_and_offset_values<float,float>(vy,1.0,-vymean);
    //: computing the motion magnitude
    vil_math_image_product<float,float,float>(vx,vx,vxvx);
    vil_math_image_product<float,float,float>(vy,vy,vyvy);
    vil_math_image_sum<float,float,float>(vxvx,vyvy,vsum);
    vil_math_sqrt<float>(vsum);


    parameters()->get_value("-lrange",lrange_);
    parameters()->get_value("-hrange",hrange_);
    parameters()->get_value("-numbins",no_of_bins);
    //: using otsu thresholding method to select the threshold for motion
    vcl_vector<double> hist;
    vil_histogram<float>(vsum,hist,lrange_,hrange_,no_of_bins);
    bsta_otsu_threshold<double> ot(hist,lrange_,hrange_);
    double motionthresh=ot.threshold();

    vil_image_view< float > vmag(ni_,nj_);
    vil_image_view< bool > outvmagbin(ni_,nj_);
    vil_image_view< bool > vmagbin(ni_,nj_);

    for(unsigned i=0;i<vsum.ni();i++)
        for(unsigned j=0;j<vsum.nj();j++)
            if(vsum(i,j)<motionthresh)
                vmagbin(i,j)=0;
            else
                vmagbin(i,j)=1;
    vil_structuring_element se;
    se.set_to_disk(eroderadius);
    vil_binary_erode(vmagbin,outvmagbin,se);
    se.set_to_disk(dilateradius);
    vil_binary_dilate(outvmagbin,vmagbin,se);
    //: vmagbin is the binary mask
    for(unsigned i=0;i<vmagbin.ni();i++)
        for(unsigned j=0;j<vmagbin.nj();j++)
            if(!vmagbin(i,j))
                vmag(i,j)=1.0;
            else
                vmag(i,j)=0;
    //: do registration
    vimt_transform_2d init_xform;
    vnl_matrix<double> A(2,3);

    A[0][0] = 1.0;  A[0][1] = 0.0;  A[0][2] = 0.0;
    A[1][0] = 0.0;  A[1][1] = 1.0;  A[1][2] = 0.0;


    init_xform.set_affine(A);

    vil_math_scale_values<float>(float_curr_view,1/sigma_first_image_);
    vil_math_scale_values<float>(float_last_view,1/sigma_first_image_);

    int border = 10;
    parameters()->get_value( "-border" , border );
    dbvrl_world_roi roi(ni_- 2*border, nj_- 2*border,
        vgl_point_2d<double>(border,border),
        vgl_vector_2d<double>(0.99,0.0),
        vgl_vector_2d<double>(0.0,0.99));
    vimt_image_2d_of<float> last_img(float_last_view, vimt_transform_2d());
    vimt_image_2d_of<float> curr_img(float_curr_view, init_xform);
    vimt_image_2d_of<float> curr_mask_img(vmag, init_xform);
    dbvrl_minimizer minimizer(last_img, curr_img,curr_mask_img, roi);

    vul_timer time;
    minimizer.minimize(init_xform,true);

    /* vidpro1_image_storage_new output_image_storage3;
    output_image_storage3->set_image(vil_new_image_resource_of_view(brip_vil_float_ops::convert_to_byte(vmag)));
    output_data_[0].push_back(output_image_storage3);
    output_image_storage3->set_name("OpflowMask");*/
    return init_xform;
}



