// This is pro/vidpro1_fgdetector_process.cxx
//:
// \file
#include<vcl_cstdio.h>
#include "vidpro1_fgdetector_process.h"
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
#include <dbbgm/pro/dbbgm_storage_sptr.h>
#include <dbbgm/pro/dbbgm_storage.h>
#include <bvis1/bvis1_manager.h>
#include <brip/brip_vil_float_ops.h>
#include <vidpro1/vidpro1_repository.h>
#include <vul/vul_timer.h>
#include<vsol/vsol_point_2d.h>
#include<vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include<vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vul/vul_sprintf.h>
#include <vil/vil_save.h>
//: Constructor
vidpro1_fgdetector_process::vidpro1_fgdetector_process(void): bpro1_process()
{

    if( //!parameters()->add( "Input model filename" ,          "-fname" ,  bpro1_filepath("","*.*")) ||
        !parameters()->add( "Freeze Model Update" ,          "-isfrozen" ,  (bool)true)||
        !parameters()->add( "Min Weight Threshold for Mode" ,  "-wmode" ,  (float)0.75)
        )
    {
        vcl_cerr << "ERROR: Adding parameters in vidpro1_fgdetector_process::vidpro1_kl_affine_register_process()" << vcl_endl;
    }
    else
    {

    }
    frame_no=0;
    ismodelset_=false;
}

//: Destructor
vidpro1_fgdetector_process::~vidpro1_fgdetector_process()
{
    //  delete(bgM);
}


//: Return the name of this process
vcl_string
vidpro1_fgdetector_process::name()
{
    return "FG detector";
}


//: Return the number of input frame for this process
int
vidpro1_fgdetector_process::input_frames()
{
    return 2;
}


//: Return the number of output frames for this process
int
vidpro1_fgdetector_process::output_frames()
{
    return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > vidpro1_fgdetector_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "image" );
    to_return.push_back( "dbbgm" );

    return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > vidpro1_fgdetector_process::get_output_type()
{  
    vcl_vector<vcl_string > to_return;
    to_return.push_back( "image" );

    return to_return;
}


//: Execute the process
bool
vidpro1_fgdetector_process::execute()
{
    if ( input_data_[0].size() != 2 ){
        vcl_cout << "In vidpro1_fgdetector_process::execute() - "
            << "not exactly two input images \n";
        return false;
    }
    clear_output();
    if(!ismodelset_){

        /*bpro1_filepath bgfile;
        parameters()->get_value("-fname",bgfile);
        vsl_b_ifstream is(bgfile.path.c_str());
        dbbgm_model_data model;
        model.b_read(is);
        is.close();*/

        dbbgm_storage_sptr bgmodel;
        for(int i=0;i<input_data_[0].size();i++)
        {
            if( input_data_[0][i]->type()=="dbbgm")
            bgmodel.vertical_cast(input_data_[0][i]);
        }

        bgmodel->get_model(bgM);

        //bgM= new dbbgm_bgmodel<float>(model);
        ismodelset_=true;

        static bool frozen;
        parameters()->get_value( "-isfrozen" , frozen );
        if(frozen)
            bgM->freeze();
        else
            bgM->unfreeze();
        float thresh;
        parameters()->get_value("-wmode",thresh);
        

        bgM->setminweight(thresh);
        
        ismodelset_=true;

    }
    if(ismodelset_)
    {
        // get image from the storage class
        vidpro1_image_storage_sptr image;
        for(int i=0;i<input_data_[0].size();i++)
        {
            if( input_data_[0][i]->type()=="image")
            image.vertical_cast(input_data_[0][i]);
        }
        //image.vertical_cast(input_data_[0][1]);
        vcl_cout<<"\nProcessing Frame No: "<<input_data_[0][0]->frame();

        vil_image_resource_sptr img_sptr = image->get_image();
        vil_image_view<float> float_curr_view= brip_vil_float_ops::convert_to_float(*img_sptr);
        bgM->updateModel(float_curr_view);
        vidpro1_image_storage_sptr output_storage3 = vidpro1_image_storage_new();
        output_storage3->set_image(vil_new_image_resource_of_view(brip_vil_float_ops::convert_to_byte(bgM->detectforeground(),0,255) ));
        output_data_[0].push_back(output_storage3);
        output_storage3->set_name("ForegroundDetected");
        // vcl_string currentname = vul_sprintf("fg%05d.%s", 
//                                           input_data_[0][0]->frame(),
//                                           "tif");  
//   vil_save(brip_vil_float_ops::convert_to_byte(bgM->detectforeground(),0,255),currentname.c_str());
        return true;
    }
    else 
        return false;  

}
//: Clone the process
bpro1_process*
vidpro1_fgdetector_process::clone() const
{
    return new vidpro1_fgdetector_process(*this);
}

bool
vidpro1_fgdetector_process::finish()
{

    return true;
}
void
vidpro1_fgdetector_process::setmodel(dbbgm_bgmodel_sptr bgm)
{
    bgM=bgm;
    ismodelset_=true;


}
