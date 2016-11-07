// This is pro/vidpro1_robust_bg_model_process.cxx

//:
// \file
#include<vcl_cstdio.h>
#include "vidpro1_robust_bg_model_process.h"
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include<bpro1/bpro1_parameters.h>
#include<vsol/vsol_point_2d.h>
#include<vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include<vidpro1/storage/vidpro1_vsol2D_storage.h>
#include<dbbgm/pro/dbbgm_storage_sptr.h>
#include<dbbgm/pro/dbbgm_storage.h>
#include<bvis1/bvis1_manager.h>

#include<vtol/vtol_vertex_2d.h>
#include<vtol/vtol_vertex_2d_sptr.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vimt/vimt_resample_bilin.h>
#include <brip/brip_vil_float_ops.h>
#include <vil/vil_save.h>
#include <vidpro1/vidpro1_repository.h>
#include <dbvrl/dbvrl_minimizer.h>
#include <dbvrl/dbvrl_world_roi.h>
#include <dbvrl/dbvrl_transform_2d.h>
#include <dbvrl/dbvrl_region.h>
#include <dbvrl/dbvrl_region_sptr.h>
#include <vul/vul_timer.h>
#include <vul/vul_sprintf.h>
#include <vil/vil_save.h>
//: Constructor
vidpro1_robust_bg_model_process::vidpro1_robust_bg_model_process(void): bpro1_process()
{

    if( !parameters()->add( "L BG model" ,          "-numlframes" ,  (int)10) ||
        !parameters()->add( "No of models or mixtures BG model" ,          "-imodels" ,  (int)3) ||
        !parameters()->add( "intial std deviation" ,          "-istd" ,  (float)15) ||
        !parameters()->add( "Freeze Model Update(Bg building)" ,          "-bgisfrozen" ,  (bool)false)
        //!parameters()->add( "Save model filename ... " ,   "-smodel" ,  bpro1_filepath("bgmodel.dat","*.*"))
        )
    {
        vcl_cerr << "ERROR: Adding parameters in vidpro1_robust_bg_model_process::vidpro1_kl_affine_register_process()" << vcl_endl;
    }
    else
    {

    }
    frame_no=0;
}

//: Destructor
vidpro1_robust_bg_model_process::~vidpro1_robust_bg_model_process()
{
   
}


//: Return the name of this process
vcl_string
vidpro1_robust_bg_model_process::name()
{
    return "BG model builder";
}


//: Return the number of input frame for this process
int
vidpro1_robust_bg_model_process::input_frames()
{
    return 1;
}


//: Return the number of output frames for this process
int
vidpro1_robust_bg_model_process::output_frames()
{
    return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > vidpro1_robust_bg_model_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "image" );

    return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > vidpro1_robust_bg_model_process::get_output_type()
{  
    vcl_vector<vcl_string > to_return;
    to_return.push_back( "dbbgm" );
    return to_return;
}


//: Execute the process
bool
vidpro1_robust_bg_model_process::execute()
{
    if ( input_data_.size() != 1 ){
        vcl_cout << "In vidpro1_robust_bg_model_process::execute() - "
            << "not exactly one input images \n";
        return false;
    }
    clear_output();

    static int lframes;
    parameters()->get_value( "-numlframes" , lframes );
#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
    unsigned max = 255;
#endif

    if(input_data_[0][0]->frame() == 0){


        vidpro1_image_storage_sptr frame_image;
        frame_image.vertical_cast(input_data_[0][0]);
        vil_image_resource_sptr image = frame_image->get_image();
        vil_image_view<float> float_curr_view=brip_vil_float_ops::convert_to_float(*image);
        bgM= new dbbgm_bgmodel<float>(float_curr_view);
        bgM->setlframes(lframes);

        static float intialstd;
        parameters()->get_value( "-istd" , intialstd );
        bgM->setintialstddev(intialstd);

        vcl_cout<<"\nBuilding Background Model, Frame No: "<<input_data_[0][0]->frame();

        dbbgm_storage_sptr bgmstorage = new dbbgm_storage();
        bgmstorage->set_model(bgM);
        output_data_[0].push_back(bgmstorage);
  vcl_cout<<"\nBuilding Background Model, Frame No: "<<input_data_[0][0]->frame();
        return true;
    }
    // get image from the storage class
    vidpro1_image_storage_sptr image;
    image.vertical_cast(input_data_[0][0]);

    vil_image_resource_sptr img_sptr = image->get_image();
    vil_image_view<float> float_curr_view= brip_vil_float_ops::convert_to_float(*img_sptr);


    vcl_cout<<"\nBuilding Background Model, Frame No: "<<input_data_[0][0]->frame();

    static bool frozen;
    parameters()->get_value( "-bgisfrozen" , frozen );
    if(frozen)
        bgM->freeze();
    else
        bgM->unfreeze();

    bgM->updateModel(float_curr_view);

    
    dbbgm_storage_sptr bgmstorage = new dbbgm_storage();
    bgmstorage->set_model(bgM);
    output_data_[0].push_back(bgmstorage);
    /*vcl_string currentname = vul_sprintf("bg%05d.%s", 
                                          input_data_[0][0]->frame(),
                                           "tif");  
             vil_save(brip_vil_float_ops::convert_to_byte(bgM->writebackground(0)),currentname.c_str());*/

    return true;  
}
//: Clone the process
bpro1_process*
vidpro1_robust_bg_model_process::clone() const
{
    return new vidpro1_robust_bg_model_process(*this);
}

bool
vidpro1_robust_bg_model_process::finish()
{

   /* bpro1_filepath bgfile;
    parameters()->get_value("-smodel",bgfile);*/
    vsl_b_ofstream os("bgmodel.dat");
    bgM->model.b_write(os);
    os.close();
    return true;
}

