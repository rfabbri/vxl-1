
#include "dbrl_rpm_tps_affine_superimpose_process.h"
#include<bpro1/bpro1_parameters.h>
#include<vsol/vsol_point_2d.h>
#include<vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include<vidpro1/storage/vidpro1_vsol2D_storage.h>
#include<georegister/dbrl_feature_sptr.h>
#include<georegister/dbrl_feature_point.h>
#include<vnl/vnl_vector_fixed.h>
#include<georegister/dbrl_rpm_tps.h>
#include "dbrl_match_set_storage.h"
#include "dbrl_match_set_storage_sptr.h"
#include<georegister/dbrl_id_point_2d.h>
#include<georegister/dbrl_id_point_2d_sptr.h>
#include<georegister/dbrl_rpm_affine.h>
#include<georegister/dbrl_rpm_tps.h>

#include "dbrl_id_point_2d_storage.h"
#include "dbrl_id_point_2d_storage_sptr.h"

#include <vil/vil_image_view.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

//: Constructor
dbrl_rpm_tps_affine_superimpose_process::dbrl_rpm_tps_affine_superimpose_process(void): bpro1_process()
    {

    if( !parameters()->add( "Initial temperature " , "-initT" , (float)1) ||
        !parameters()->add( "Annealing Rate" , "-annealrate" , (float) 0.93 )||
        !parameters()->add( "Final Temperature" , "-finalT" , (float) 0.0001 )||
        !parameters()->add( "Initial Lambda 1" , "-initlambda1" , (float) 1000 )||
        !parameters()->add( "Initial Lambda 2" , "-initlambda2" , (float) 50 )||
        !parameters()->add( "Covergence Tolerance" , "-mconvg" , (float) 0.1 )||
        !parameters()->add( "Outlier value" , "-moutlier" , (float) 1e-5 )||
        !parameters()->add( "Initial temperature (affine)" , "-affinitT" , (float)1) ||
        !parameters()->add( "Annealing Rate (affine)" , "-affannealrate" , (float) 0.93 )||
        !parameters()->add( "Final Temperature (affine)" , "-afffinalT" , (float) 0.001 )||
        !parameters()->add( "Initial Lambda (affine)" , "-affinitlambda" , (float) 50 )||
        !parameters()->add( "Covergence Tolerance (affine)" , "-affmconvg" , (float) 0.1 )||
        !parameters()->add( "Outlier valuev (affine)", "-affmoutlier" , (float) 1e-5 )||
        !parameters()->add( "Scale the points to a grid", "-upscale" , (float) 100 )
        ) 
        {
        vcl_cerr << "ERROR: Adding parameters in dbrl_rpm_tps_affine_superimpose_process::dbrl_rpm_tps_affine_superimpose_process()" << vcl_endl;
        }

    }

//: Destructor
dbrl_rpm_tps_affine_superimpose_process::~dbrl_rpm_tps_affine_superimpose_process()
    {

    }


//: Return the name of this process
vcl_string
dbrl_rpm_tps_affine_superimpose_process::name()
    {
    return "Rpm TPS AFFINE Superimpose";
    }


//: Return the number of input frame for this process
int
dbrl_rpm_tps_affine_superimpose_process::input_frames()
    {
    return 1;
    }


//: Return the number of output frames for this process
int
dbrl_rpm_tps_affine_superimpose_process::output_frames()
    {
    return 1;
    }


//: Provide a vector of required input types
vcl_vector< vcl_string > dbrl_rpm_tps_affine_superimpose_process::get_input_type()
    {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "dbrl_id_point_2d" );
    //to_return.push_back( "vsol2D" );

    return to_return;
    }


//: Provide a vector of output types
vcl_vector< vcl_string > dbrl_rpm_tps_affine_superimpose_process::get_output_type()
    {  
    vcl_vector<vcl_string > to_return;
    to_return.push_back( "image" );

    to_return.push_back( "dbrl_match_set" );
    to_return.push_back( "dbrl_match_set" );

    return to_return;
    }


//: Execute the process
bool
dbrl_rpm_tps_affine_superimpose_process::execute()
    {
    if ( input_data_.size() != 1 ){
        vcl_cout << "In dbrl_rpm_tps_affine_superimpose_process::execute() - "
            << "not exactly two input images \n";
        return false;
        }
clear_output();
dbrl_id_point_2d_storage_sptr frame_pts;
frame_pts.vertical_cast(input_data_[0][0]);

vcl_vector< dbrl_id_point_2d_sptr > list = frame_pts->points();
vcl_vector<dbrl_feature_sptr> feature_list;

for(int i=0;i<list.size();i++)
    {
    vnl_vector_fixed<double,2> pt(list[i]->x(),list[i]->y());
    dbrl_feature_point * fpt=new dbrl_feature_point(pt);
    feature_list.push_back(fpt);
    i+=1;
    }
id_point_set_list_.push_back(list);
point_set_list_.push_back(feature_list);
return true;  
    }
//: Clone the process
bpro1_process*
dbrl_rpm_tps_affine_superimpose_process::clone() const
    {
    return new dbrl_rpm_tps_affine_superimpose_process(*this);
    }

bool
dbrl_rpm_tps_affine_superimpose_process::finish()
    {
    //: affine parameters
    static float affTinit=1.0;
    static float affTfinal=0.01;
    static float affannealrate=0.93;
    static float afflambdainit=1.0;
    static float affmconvg=0.1;
    static float affmoutlier=1e-5;

    parameters()->get_value("-affinitT",affTinit);
    parameters()->get_value("-afffinalT",affTfinal);
    parameters()->get_value("-affannealrate",affannealrate);
    parameters()->get_value("-affinitlambda",afflambdainit);
    parameters()->get_value("-affmconvg",affmconvg);
    parameters()->get_value("-affmoutlier",affmoutlier);

    //: tps parameters
    static float Tinit=1.0;
    static float Tfinal=0.0001;
    static float TChange=0.0009;
    static float annealrate=0.93;
    static float lambdainit1=1000;
    static float lambdainit2=100;
    static float mconvg=0.1;
    static float moutlier=1e-5;
    parameters()->get_value("-initT",Tinit);
    parameters()->get_value("-finalT",Tfinal);
    parameters()->get_value("-changeT",Tchange);
    parameters()->get_value("-annealrate",annealrate);
    parameters()->get_value("-initlambda1",lambdainit1);
    parameters()->get_value("-initlambda2",lambdainit2);
    parameters()->get_value("-mconvg",mconvg);
    parameters()->get_value("-moutlier",moutlier);
    static int pivot_frame_no=(int)(point_set_list_.size()/2);
    vcl_vector<vcl_vector<dbrl_feature_sptr> > xformed_point_set_list=point_set_list_;
    dbrl_rpm_affine_params affineparams(afflambdainit,affmconvg,affTinit,affTfinal,affmoutlier,affannealrate);
    dbrl_rpm_tps_params tpsparams(lambdainit1,lambdainit2,mconvg,Tinit,Tfinal,moutlier,annealrate);
    dbrl_rpm_tps_affine_params tpsaffineparams(lambdainit2,mconvg,Tinit,Tfinal,moutlier,annealrate);

    //: outer loop for computing TPS from each frame to pivot frame
    for(int i=0;i<point_set_list_.size();i++)
        {
        //: approximation alignment of point-sets 
        dbrl_match_set_sptr outset;
        dbrl_match_set_storage_sptr tps_storage = dbrl_match_set_storage_new();
        dbrl_match_set_storage_sptr affine_storage = dbrl_match_set_storage_new();

        if(i<pivot_frame_no)
            {
            for(int j=i+1;j<pivot_frame_no;j++)
                {
                dbrl_rpm_affine affinerpm(affineparams,point_set_list_[j],xformed_point_set_list[i]);
                dbrl_match_set_sptr outset=affinerpm.rpm();
                xformed_point_set_list[i].clear();
                xformed_point_set_list[i]=outset->get_xformed_feature_set2();
                affine_storage->set_match_set(outset);
                }
            double T=Tinit;
            //dbrl_rpm_tps tpsrpm(tpsparams,xformed_point_set_list[i],xformed_point_set_list[pivot_frame_no]);
            //dbrl_match_set_sptr tpsset=tpsrpm.rpm();
            //xformed_point_set_list[i].clear();
            //xformed_point_set_list[i]=tpsset->get_xformed_feature_set1();

            dbrl_rpm_affine *affinerpm=new dbrl_rpm_affine(tpsaffineparams,xformed_point_set_list[pivot_frame_no],xformed_point_set_list[i]);
            dbrl_estimator_point_affine* affine_est= new dbrl_estimator_point_affine();

            dbrl_rpm_tps *tpsrpm=new dbrl_rpm_tps(tpsparams,xformed_point_set_list[pivot_frame_no],xformed_point_set_list[i]);
            dbrl_estimator_point_thin_plate_spline *tps_est= new dbrl_estimator_point_thin_plate_spline();


            //: initialize M 
            dbrl_correspondence *M=new dbrl_correspondence(f1.size(),f2.size());
            M->setinitialoutlier(tpsaffineparams->outlier());

            dbrl_match_set *match_set;
            while(T>Tfinal)
                {
                if(T>Tchange)
                    {
                    affinerpm->rpm_at(T,*M,affine_est,tform,xformed_point_set_list[pivot_frame_no],xformed_point_set_list[i],tpsaffineparams->initlambda()*T);
                    T*=tpsaffineparams->annealrate();
                    match_set=new dbrl_match_set(*M,tform,affine_est);

                    }
                if(T<=Tchange)
                    {   
                    tpsrpm->rpm_at(T,*M,tps_est,tform,xformed_point_set_list[pivot_frame_no],xformed_point_set_list[i],tpsparams->initlambda1()*T,tpsparams->initlambda2()*T);
                    T*=tpsparams->annealrate();
                    match_set=new dbrl_match_set(*M,tform,tps_est);

                    }
                }
                tps_storage->set_match_set(match_set);
            }
        else if(i>pivot_frame_no)
            {
            for(int j=i-1;j>pivot_frame_no;j--)
                {
                dbrl_rpm_affine affinerpm(affineparams,point_set_list_[j],xformed_point_set_list[i]);
                dbrl_match_set_sptr outset=affinerpm.rpm();
                xformed_point_set_list[i].clear();
                xformed_point_set_list[i]=outset->get_xformed_feature_set2();
                affine_storage->set_match_set(outset);
                }
            dbrl_rpm_tps tpsrpm(tpsparams,xformed_point_set_list[pivot_frame_no],xformed_point_set_list[i]);
            dbrl_match_set_sptr tpsset=tpsrpm.rpm();
            xformed_point_set_list[i].clear();
            xformed_point_set_list[i]=tpsset->get_xformed_feature_set1();
            tps_storage->set_match_set(tpsset);
            }
        else
            {
            }
        static float scale=100;
        parameters()->get_value("-upscale",scale);
        vil_image_view<unsigned char> img((int)scale,(int)scale,1);
        img.fill(0);
        for(int p=0;p<xformed_point_set_list[i].size();p++)
            {
            dbrl_feature_point* pt1=dynamic_cast<dbrl_feature_point*>(xformed_point_set_list[i][p].ptr());
            float x=pt1->location()[0];
            float y=pt1->location()[1];
            img((int)(x*scale),(int)(y*scale))=255;
            }
        vidpro1_image_storage_sptr img_storage = vidpro1_image_storage_new();
        img_storage->set_image(vil_new_image_resource_of_view(img));
        output_data_[i].push_back(img_storage);
        output_data_[i].push_back(affine_storage);
        output_data_[i].push_back(tps_storage);
        //: Fine TPS transformation

        }

    return true;
    }









