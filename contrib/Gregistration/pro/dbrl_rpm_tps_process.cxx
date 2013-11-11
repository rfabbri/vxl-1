
#include "dbrl_rpm_tps_process.h"
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

#include "dbrl_id_point_2d_storage.h"
#include "dbrl_id_point_2d_storage_sptr.h"

//: Constructor
dbrl_rpm_tps_process::dbrl_rpm_tps_process(void): bpro1_process()
    {

    if( !parameters()->add( "Initial temperature" , "-initT" , (float)1) ||
        !parameters()->add( "Annealing Rate" , "-annealrate" , (float) 0.93 )||
        !parameters()->add( "Final Temperature" , "-finalT" , (float) 0.01 )||
        !parameters()->add( "Initial Lambda 1" , "-initlambda1" , (float) 1.0 )||
        !parameters()->add( "Initial Lambda 2" , "-initlambda2" , (float) 0.01 )||
        !parameters()->add( "Covergence Tolerance" , "-mconvg" , (float) 0.1 )||
        !parameters()->add( "Outlier value" , "-moutlier" , (float) 1e-5 )) 
        {
        vcl_cerr << "ERROR: Adding parameters in dbrl_rpm_tps_process::dbrl_rpm_tps_process()" << vcl_endl;
        }

    }

//: Destructor
dbrl_rpm_tps_process::~dbrl_rpm_tps_process()
    {

    }


//: Return the name of this process
vcl_string
dbrl_rpm_tps_process::name()
    {
    return "RPM THIN PLATE SPLINE";
    }


//: Return the number of input frame for this process
int
dbrl_rpm_tps_process::input_frames()
    {
    return 2;
    }


//: Return the number of output frames for this process
int
dbrl_rpm_tps_process::output_frames()
    {
    return 1;
    }


//: Provide a vector of required input types
vcl_vector< vcl_string > dbrl_rpm_tps_process::get_input_type()
    {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "dbrl_id_point_2d" );
    //to_return.push_back( "vsol2D" );

    return to_return;
    }


//: Provide a vector of output types
vcl_vector< vcl_string > dbrl_rpm_tps_process::get_output_type()
    {  
    vcl_vector<vcl_string > to_return;
    to_return.push_back( "dbrl_match_set" );
    return to_return;
    }


//: Execute the process
bool
dbrl_rpm_tps_process::execute()
    {
    if ( input_data_.size() != 2 ){
        vcl_cout << "In dbrl_rpm_tps_process::execute() - "
            << "not exactly two input images \n";
        return false;
        }
clear_output();
//vidpro1_vsol2D_storage_sptr frame_pts1, frame_pts2;
dbrl_id_point_2d_storage_sptr frame_pts1, frame_pts2;
frame_pts1.vertical_cast(input_data_[1][0]);
frame_pts2.vertical_cast(input_data_[0][0]);

vcl_vector< dbrl_id_point_2d_sptr > list1 = frame_pts1->points();
vcl_vector< dbrl_id_point_2d_sptr > list2 = frame_pts2->points();

vcl_vector<dbrl_feature_sptr> feature_list1;
vcl_vector<dbrl_feature_sptr> feature_list2;

for(int i=0;i<static_cast<int>(list1.size());i++)
    {
    vnl_vector_fixed<double,2> pt(list1[i]->x(),list1[i]->y());
    dbrl_feature_point * fpt=new dbrl_feature_point(pt);
    feature_list1.push_back(fpt);
    //i+=4;
    }
for(int i=0;i<static_cast<int>(list2.size());i++)
    {
       vnl_vector_fixed<double,2> pt(list2[i]->x(),list2[i]->y());
    dbrl_feature_point * fpt=new dbrl_feature_point(pt);
    feature_list2.push_back(fpt);
    //i+=4;
 }

static float Tinit=1.0;
static float Tfinal=0.01;
static float annealrate=0.93;
static float lambdainit1=1.0;
static float lambdainit2=0.01;
static float mconvg=0.1;
static float moutlier=1e-5;

parameters()->get_value("-initT",Tinit);
parameters()->get_value("-finalT",Tfinal);
parameters()->get_value("-annealrate",annealrate);
parameters()->get_value("-initlambda1",lambdainit1);
parameters()->get_value("-initlambda2",lambdainit2);
parameters()->get_value("-mconvg",mconvg);
parameters()->get_value("-moutlier",moutlier);



dbrl_rpm_tps_params params(lambdainit1,lambdainit2,mconvg,Tinit,Tfinal,moutlier,annealrate);

dbrl_rpm_tps tpsrpm(params,feature_list1,feature_list2);
dbrl_match_set_sptr outset=tpsrpm.rpm();

for(int i=0;i<static_cast<int>(list1.size());i++)
    {
    int id1=0;
    int id2=0;
    int j=outset->cindex1to2(i);
    id1=list1[i]->id();
    if(j>0)
        id2=list2[j]->id();
    else id2=-1;

    vcl_cout<<"Match: "<<id1<<" "<<id2<<"\n";

    }
dbrl_match_set_storage_sptr output_storage = dbrl_match_set_storage_new();
output_storage->set_match_set(outset);

output_data_[0].push_back(output_storage);
//output_storage->set_name("Match Set");
return true;  
    }
//: Clone the process
bpro1_process*
dbrl_rpm_tps_process::clone() const
    {
    return new dbrl_rpm_tps_process(*this);
    }

bool
dbrl_rpm_tps_process::finish()
    {
    return true;
    }




