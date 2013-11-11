//:
// \file
#include<vcl_cstdio.h>
#include "dbvrl_register_images_process.h"
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
#include <vul/vul_sprintf.h>
#include <vul/vul_timer.h>
#include <bsta/bsta_otsu_threshold.h>

//: Constructor
dbvrl_register_images_process::dbvrl_register_images_process(void): bpro1_process(),total_xform()
{

    if( !parameters()->add( "Border Size (pixels)" , "-border" , (int)10 ) ||
        !parameters()->add( "First Frame" ,          "-first" ,  (bool)true )||
       !parameters()->add( "Homography file - filaname .." ,   "-fhmg" ,  bpro1_filepath("","*.*"))


        )
    {
        vcl_cerr << "ERROR: Adding parameters in dbvrl_register_images_process::vidpro1_kl_affine_register_process()" << vcl_endl;
    }
    else
    {

    }
    total_xform.set_identity();
    last_frame_no=0;
}

//: Destructor
dbvrl_register_images_process::~dbvrl_register_images_process()
{
}


//: Return the name of this process
vcl_string
dbvrl_register_images_process::name()
{
    return "Register Images";
}


//: Return the number of input frame for this process
int
dbvrl_register_images_process::input_frames()
{
    return 1;
}


//: Return the number of output frames for this process
int
dbvrl_register_images_process::output_frames()
{
    return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbvrl_register_images_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "image" );
    return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbvrl_register_images_process::get_output_type()
{  
    vcl_vector<vcl_string > to_return;
    to_return.push_back( "image" );
    return to_return;
}


//: Execute the process
bool
dbvrl_register_images_process::execute()
{
    if ( input_data_.size() !=1 ){
        vcl_cout << "In dbvrl_register_images_process::execute() - "
            << "not exactly one input images \n";
        return false;
    }
    last_frame_no=input_data_[0][0]->frame();

    clear_output();
    vcl_cout<<"\n Registering  frame no "<<last_frame_no;

    parameters()->get_value( "-first" , first_frame_ );

    if(input_data_[0][0]->frame()==0){
      vcl_cout<<"\n inside ";
        vidpro1_image_storage_sptr frame_image;
        frame_image.vertical_cast(input_data_[0][0]);
        vil_image_resource_sptr image = frame_image->get_image();
        vil_image_view<float> float_curr_view=brip_vil_float_ops::convert_to_float(*image);
  
        ni_= float_curr_view.ni();
        nj_= float_curr_view.nj();

        //: reading the homographies from a text file;
        bpro1_filepath hmgfile;
        bpro1_filepath outfile;

        parameters()->get_value("-fhmg",hmgfile);
        parameters()->get_value("-fout",outfile);
        outfilename=outfile.path;
        read_homographies(hmgfile.path);
        
        vcl_string currentname = vul_sprintf("%s%05d.%s", outfilename.c_str(),
                                         last_frame_no,
                                         "tif");
        if(homographies_.size()<=0)
            return false;

        for(unsigned i=0;i<homographies_.size();i++)
        {
            vimt_transform_2d p;
            p.set_affine(homographies_[i].extract(2,3));
            xforms_.push_back(p);
            box_.update(p(0,0).x(),p(0,0).y());
            box_.update(p(0,nj_).x(),p(0,nj_).y());
            box_.update(p(ni_,0).x(),p(ni_,0).y());
            box_.update(p(ni_,nj_).x(),p(ni_,nj_).y());
        }
        bimg_ni=(int)vcl_ceil(box_.max()[0]-box_.min()[0]);
        bimg_nj=(int)vcl_ceil(box_.max()[1]-box_.min()[1]);

        offset_i=(int)vcl_ceil(0-box_.min()[0]);
        offset_j=(int)vcl_ceil(0-box_.min()[1]);

        vimt_transform_2d ftxform=xforms_[last_frame_no].inverse();
        vimt_image_2d_of<float> sample_im;

        vgl_point_2d<double>  p(-offset_i,-offset_j);
        vgl_vector_2d<double> u(1,0);
        vgl_vector_2d<double> v(0,1);

        vimt_image_2d_of<float> curr_img(float_curr_view,ftxform);
        vimt_resample_bilin(curr_img,sample_im,p,u,v,bimg_ni,bimg_nj);

        //vil_save(brip_vil_float_ops::convert_to_byte(sample_im.image()),currentname.c_str());
        vidpro1_image_storage_new regimage_storage;
        regimage_storage->set_image(vil_new_image_resource_of_view(brip_vil_float_ops::convert_to_byte(sample_im.image())));
        output_data_[0].push_back(regimage_storage);


        parameters()->set_value( "-first" , false );
        return true;
    }
    // get image from the storage class
    vidpro1_image_storage_sptr curr_image;

    curr_image.vertical_cast(input_data_[0][0]);
    vil_image_resource_sptr curr_img_sptr = curr_image->get_image();
    vil_image_view<float> float_curr_view= brip_vil_float_ops::convert_to_float(*curr_img_sptr);

    ni_= float_curr_view.ni();
    nj_= float_curr_view.nj();
    // total_xform = total_xform * init_xform.inverse();
    if(static_cast<unsigned>(last_frame_no)<homographies_.size())
    {
        vimt_transform_2d ftxform=xforms_[last_frame_no].inverse();
        vimt_image_2d_of<float> sample_im;

        vgl_point_2d<double> p(-offset_i,-offset_j);
        vgl_vector_2d<double> u(1,0);
        vgl_vector_2d<double> v(0,1);


        vimt_image_2d_of<float> curr_img(float_curr_view,ftxform);
        vimt_resample_bilin(curr_img,sample_im,p,u,v,bimg_ni,bimg_nj);

        vidpro1_image_storage_new regimage_storage;
        regimage_storage->set_image(vil_new_image_resource_of_view(brip_vil_float_ops::convert_to_byte(sample_im.image())));
        output_data_[0].push_back(regimage_storage);

        return true;  
    }
    else
        return false;
}
//: Clone the process
bpro1_process*
dbvrl_register_images_process::clone() const
{
    return new dbvrl_register_images_process(*this);
}

bool
dbvrl_register_images_process::finish()
{

    return true;
}

bool
dbvrl_register_images_process::read_homographies(vcl_string filename)
{
    vcl_ifstream ifile(filename.c_str(),vcl_ios::in);
    vcl_cout<<"\n Reading Homographies "<<filename;

    if(!ifile)
    {
        vcl_cout<<"\n error opening file";
        vcl_cout.flush();
        return false;

    }
    char buffer[100];
    while(ifile.getline(buffer,100))
    {
        vnl_matrix<double> p(3,3);
        ifile>>p;
        vcl_cout<<p;
        homographies_.push_back(p);
        ifile.getline(buffer,100);
    }
    vcl_cout.flush();
    return true;
}

