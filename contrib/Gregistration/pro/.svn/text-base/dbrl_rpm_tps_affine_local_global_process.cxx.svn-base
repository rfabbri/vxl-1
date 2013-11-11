
#include "dbrl_rpm_tps_affine_local_global_process.h"
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
#include<georegister/dbrl_feature_point_sptr.h>
#include "dbrl_id_point_2d_storage.h"
#include "dbrl_id_point_2d_storage_sptr.h"
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <georegister/dbrl_translation.h>
#include <vil/vil_image_view.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vul/vul_sprintf.h>

//: Constructor
dbrl_rpm_tps_affine_local_global_process::dbrl_rpm_tps_affine_local_global_process(void): bpro1_process()
    {

    if( !parameters()->add( "Initial temperature " , "-initT" , (float)1) ||
        !parameters()->add( "Annealing Rate" , "-annealrate" , (float) 0.93 )||
        !parameters()->add( "Final Temperature" , "-finalT" , (float) 0.0001 )||
        !parameters()->add( "Change Temperature" , "-changeT" , (float) 0.0009 )||
        !parameters()->add( "Initial Lambda 1" , "-initlambda1" , (float) 1000 )||
        !parameters()->add( "Initial Lambda 2" , "-initlambda2" , (float) 50 )||
        !parameters()->add( "Covergence Tolerance" , "-mconvg" , (float) 0.1 )||
        !parameters()->add( "Outlier value" , "-moutlier" , (float) 1e-5 )||
        !parameters()->add( "Initial temperature (affine)" , "-affinitT" , (float)1) ||
        !parameters()->add( "Annealing Rate (affine)" , "-affannealrate" , (float) 0.93 )||
        !parameters()->add( "Final Temperature (affine)" , "-afffinalT" , (float) 0.001 )||
        !parameters()->add( "Initial Lambda (affine)" , "-affinitlambda" , (float) 50 )||
        !parameters()->add( "Covergence Tolerance (affine)" , "-affmconvg" , (float) 0.1 )||
        !parameters()->add( "Outlier value (affine)", "-affmoutlier" , (float) 1e-5 )||
        !parameters()->add( "Scale the points to a grid", "-upscale" , (float) 100 )||
        !parameters()->add( "No of divisions along x and y", "-div" , (int)1 )||
        !parameters()->add( "Expected motion in x", "-expm" , (int)20 )
        ) 
        {
            vcl_cerr << "ERROR: Adding parameters in dbrl_rpm_tps_affine_local_global_process::dbrl_rpm_tps_affine_local_global_process()" << vcl_endl;
        }
    point_set_list_.clear();
    id_point_set_list_.clear();

    }
//: Destructor
dbrl_rpm_tps_affine_local_global_process::~dbrl_rpm_tps_affine_local_global_process()
    {
    }
//: Return the name of this process
vcl_string
dbrl_rpm_tps_affine_local_global_process::name()
    {
        return "Rpm TPS Local and Global affine Superimpose";
    }
//: Return the number of input frame for this process
int
dbrl_rpm_tps_affine_local_global_process::input_frames()
    {
        return 1;
    }
//: Return the number of output frames for this process
int
dbrl_rpm_tps_affine_local_global_process::output_frames()
    {
        return 1;
    }
//: Provide a vector of required input types
vcl_vector< vcl_string > dbrl_rpm_tps_affine_local_global_process::get_input_type()
    {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "dbrl_id_point_2d" );
    return to_return;
    }


//: Provide a vector of output types
vcl_vector< vcl_string > dbrl_rpm_tps_affine_local_global_process::get_output_type()
    {  
    vcl_vector<vcl_string > to_return;
    //to_return.push_back( "image" );
    to_return.push_back( "dbrl_match_set" );
    //to_return.push_back( "dbrl_match_set" );
    //to_return.push_back( "vsol2D" );
    //to_return.push_back( "vsol2D" );
    //to_return.push_back( "vsol2D" );
    return to_return;
    }


//: Execute the process
bool
dbrl_rpm_tps_affine_local_global_process::execute()
    {
    if ( input_data_.size() != 1 ){
        vcl_cout << "In dbrl_rpm_tps_affine_local_global_process::execute() - "
            << "not exactly two input images \n";
        return false;
        }
static float scale=0.0;
parameters()->get_value("-upscale",scale);
clear_output();
dbrl_id_point_2d_storage_sptr frame_pts;
frame_pts.vertical_cast(input_data_[0][0]);

vcl_vector< dbrl_id_point_2d_sptr > list = frame_pts->points();
vcl_vector<dbrl_feature_sptr> feature_list;

for(int i=0;i<static_cast<int>(list.size());i++)
    {
    vnl_vector_fixed<double,2> pt(list[i]->x()/scale,list[i]->y()/scale);
    dbrl_feature_point * fpt=new dbrl_feature_point(pt);
    fpt->setid(list[i]->id());
    feature_list.push_back(fpt);
    }
id_point_set_list_.push_back(list);
point_set_list_.push_back(feature_list);
framenums_.push_back(frame_pts->frame());
return true;  
    }
//: Clone the process
bpro1_process*
dbrl_rpm_tps_affine_local_global_process::clone() const
    {
    return new dbrl_rpm_tps_affine_local_global_process(*this);
    }

bool
dbrl_rpm_tps_affine_local_global_process::finish()
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
    static float Tchange=0.0009;
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
    //: initialize the functions
    static int pivot_frame_no=(int)(point_set_list_.size()/2);
    dbrl_rpm_affine_params affineparams(afflambdainit,affmconvg,affTinit,affTfinal,affmoutlier,affannealrate);
    dbrl_rpm_tps_params tpsparams(lambdainit1,lambdainit2,mconvg,Tinit,Tfinal,moutlier,annealrate);
    dbrl_rpm_affine_params tpsaffineparams(lambdainit2,mconvg,Tinit,Tfinal,moutlier,annealrate);

    static int div=1;
    static int expm=20;
    static float scale=0.0;
    parameters()->get_value("-div",div);
    parameters()->get_value("-expm",expm);
    parameters()->get_value("-upscale",scale);

    float scaledmotion=(float)expm/scale;
    vcl_vector<vcl_vector<vcl_vector<dbrl_feature_sptr> > > pointset_gridified;
    //: divide the pointsets into grid
    vcl_vector<float> i0s;
    vcl_vector<float> j0s;
    for(int k=0;k<static_cast<int>(point_set_list_.size());k++)
        {
        vcl_vector<vcl_vector<dbrl_feature_sptr> > temp;
        temp.clear();
        for(float i=0;i<1;)
            {
            for(float j=0;j<1;)
                {
                float i0=i;
                float j0=j;
                float imax=i+1/(float)div;
                float jmax=j+1/(float)div;
                if(k!=pivot_frame_no)
                    temp.push_back(gridify_points(point_set_list_[k],i0-scaledmotion,j0-scaledmotion,imax+scaledmotion,jmax+scaledmotion));
                else
                    temp.push_back(gridify_points(point_set_list_[k],i0-scaledmotion,j0-scaledmotion,imax+scaledmotion,jmax+scaledmotion));

                i0s.push_back(i0);
                j0s.push_back(j0);
                j+=1.0/(float)div;
                }
            i+=1.0/(float)div;
            }
        pointset_gridified.push_back(temp);  
        }
    vcl_vector<vcl_vector<vcl_vector<dbrl_feature_sptr> > > xformed_point_set_list=pointset_gridified;
    //: outer loop for computing TPS from each frame to pivot frame
    for(int i=0;i<static_cast<int>(point_set_list_.size());i++)
        {

        //: approximation alignment of point-sets 
        dbrl_match_set_sptr outset;
        dbrl_match_set_storage_sptr tps_storage = dbrl_match_set_storage_new();
        dbrl_match_set_storage_sptr affine_storage = dbrl_match_set_storage_new();

        if(i<pivot_frame_no)
            {
            vcl_vector<dbrl_match_set_sptr> matchsets;
            for(int p=0;p<static_cast<int>(pointset_gridified[i].size());p++)
                {
                double T=Tinit;
                dbrl_rpm_affine *affinerpm;//=new dbrl_rpm_affine(tpsaffineparams,xformed_point_set_list[pivot_frame_no][p],xformed_point_set_list[i][p]);
                dbrl_estimator_point_affine* affine_est= new dbrl_estimator_point_affine();

                dbrl_rpm_tps *tpsrpm=new dbrl_rpm_tps(tpsparams,xformed_point_set_list[pivot_frame_no][p],xformed_point_set_list[i][p]);
                dbrl_estimator_point_thin_plate_spline *tps_est= new dbrl_estimator_point_thin_plate_spline();
                dbrl_transformation_sptr tform;
                dbrl_match_set_sptr match_set=new dbrl_match_set();
                //: initialize M 
                if(xformed_point_set_list[pivot_frame_no][p].size()>0 && xformed_point_set_list[i][p].size()>0)
                    {
                    dbrl_correspondence *M=new dbrl_correspondence(xformed_point_set_list[pivot_frame_no][p].size(),xformed_point_set_list[i][p].size());
                    M->setinitialoutlier(tpsaffineparams.outlier());
                    while(T>Tfinal)
                        {
                        if(T>Tchange)
                            {
                            //affinerpm->rpm_at(T,*M,affine_est,tform,xformed_point_set_list[pivot_frame_no][p],xformed_point_set_list[i][p],tpsaffineparams.initlambda()*T);
                            T*=tpsaffineparams.annealrate();
                            }
                        if(T<=Tchange)
                            {   
                            tpsrpm->rpm_at(T,*M,tps_est,tform,xformed_point_set_list[pivot_frame_no][p],xformed_point_set_list[i][p],tpsparams.initlambda1()*T,tpsparams.initlambda2()*T);
                            T*=tpsparams.annealrate();
                            }
                        vcl_cout<<"\n Temparutre is "<<T;
                        }
                    //: in order to solve multiple effects.
                    //M->binarize(0.51);
                    //: retreiving final transform
                    tps_est->set_lambda1(0.0);
                    tps_est->set_lambda2(0.0);
                    vcl_vector<dbrl_feature_sptr> f1xform=pointset_gridified[pivot_frame_no][p];
                    tpsrpm->normalize_point_set(M->M(),f1xform );
                    tform=tps_est->estimate(f1xform,pointset_gridified[i][p],*M);
                    dbrl_thin_plate_spline_transformation * tpstform=dynamic_cast<dbrl_thin_plate_spline_transformation *> (tform.ptr());
                    tpstform->set_from_features(pointset_gridified[i][p]);
                    tpstform->transform();
                    xformed_point_set_list[i][p].clear();
                    xformed_point_set_list[i][p]=tpstform->get_to_features();
                    remove_points_on_the_border(pointset_gridified[pivot_frame_no][p],pointset_gridified[i][p],i0s[p],j0s[p],i0s[p]+1.0/(float)div,j0s[p]+1.0/(float)div,M);
                    match_set=new dbrl_match_set(*M,tform,tps_est);
                    match_set->set_original_features(pointset_gridified[pivot_frame_no][p],pointset_gridified[i][p]);
                    match_set->set_mapped_features(xformed_point_set_list[pivot_frame_no][p],xformed_point_set_list[i][p]);
                    tpstform->print_transformation(vcl_cout);   

                    }
                matchsets.push_back(match_set);
                }
            //: initialize Mglobal
            vnl_matrix<double> Mglobal(point_set_list_[pivot_frame_no].size(),point_set_list_[i].size());
            Mglobal.fill(1/(float)point_set_list_[pivot_frame_no].size());
            //: create the global matrix
            for(int p=0;p<static_cast<int>(matchsets.size());p++)
                {
                   dbrl_match_set_sptr s=matchsets[p];
                   for(int k=0;k<s->size_set1();k++)
                       {
                       dbrl_feature_sptr pivot_feature=s->feature_set1_i(k);
                       for(int l=0;l<s->size_set2();l++)
                           {
                                dbrl_feature_sptr current_feature=s->feature_set2_i(l);
                                double weight=(*s)(k,l);
                                if(Mglobal(pivot_feature->getid()-1,current_feature->getid()-1)<weight)
                                    Mglobal(pivot_feature->getid()-1,current_feature->getid()-1)=weight;
                           }
                       }
                }
            dbrl_correspondence Mg(Mglobal);
            Mg.normalize();
            Mg.setinitialoutlier(tpsaffineparams.outlier());
            dbrl_match_set_sptr msp=new dbrl_match_set();
            msp->set_correspondence_matrix(Mg);
            msp->set_original_features(point_set_list_[pivot_frame_no],point_set_list_[i]);
            msp->set_mapped_features(point_set_list_[pivot_frame_no],point_set_list_[i]);
            tps_storage->set_match_set(msp);

            }
        else if(i>pivot_frame_no)
            {
            vcl_vector<dbrl_match_set_sptr> matchsets;
            for(int p=0;p<static_cast<int>(pointset_gridified[i].size());p++)
                {
                double T=Tinit;
                dbrl_rpm_affine *affinerpm=new dbrl_rpm_affine(tpsaffineparams,xformed_point_set_list[pivot_frame_no][p],xformed_point_set_list[i][p]);
                dbrl_estimator_point_affine* affine_est= new dbrl_estimator_point_affine();
                dbrl_rpm_tps *tpsrpm=new dbrl_rpm_tps(tpsparams,xformed_point_set_list[pivot_frame_no][p],xformed_point_set_list[i][p]);
                dbrl_estimator_point_thin_plate_spline *tps_est= new dbrl_estimator_point_thin_plate_spline();
                dbrl_transformation_sptr tform;
                //: initialize M 
                dbrl_correspondence *M=new dbrl_correspondence(xformed_point_set_list[pivot_frame_no][p].size(),xformed_point_set_list[i][p].size());
                M->setinitialoutlier(tpsaffineparams.outlier());
                dbrl_match_set_sptr match_set=new dbrl_match_set();
                while(T>Tfinal)
                    {
                    if(T>Tchange)
                        {
//                        affinerpm->rpm_at(T,*M,affine_est,tform,xformed_point_set_list[pivot_frame_no][p],xformed_point_set_list[i][p],tpsaffineparams.initlambda()*T);
                        T*=tpsaffineparams.annealrate();
                        }
                    if(T<=Tchange)
                        {   
                        tpsrpm->rpm_at(T,*M,tps_est,tform,xformed_point_set_list[pivot_frame_no][p],xformed_point_set_list[i][p],tpsparams.initlambda1()*T,tpsparams.initlambda2()*T);
                        T*=tpsparams.annealrate();
                        }
                    vcl_cout<<"\n temperature is "<<T;
                    }
                //M->binarize(0.51);
                tps_est->set_lambda1(0.0);
                tps_est->set_lambda2(0.0);
                vcl_vector<dbrl_feature_sptr> f1xform=point_set_list_[pivot_frame_no];
                tpsrpm->normalize_point_set(M->M(),f1xform );
                tform=tps_est->estimate(f1xform,point_set_list_[i],*M);
                dbrl_thin_plate_spline_transformation * tpstform=dynamic_cast<dbrl_thin_plate_spline_transformation *> (tform.ptr());
                tpstform->set_from_features(point_set_list_[i]);
                tpstform->transform();
                xformed_point_set_list[i][p].clear();
                xformed_point_set_list[i][p]=tpstform->get_to_features();
                //remove_points_on_the_border(pointset_gridified[pivot_frame_no][p],pointset_gridified[i][p],i0s[p],j0s[p],i0s[p]+1.0/(float)div,j0s[p]+1.0/(float)div,M);
                match_set=new dbrl_match_set(*M,tform,tps_est);
                match_set->set_original_features(pointset_gridified[pivot_frame_no][p],pointset_gridified[i][p]);
                match_set->set_mapped_features(xformed_point_set_list[pivot_frame_no][p],xformed_point_set_list[i][p]);
                tpstform->print_transformation(vcl_cout);
                matchsets.push_back(match_set);
                }
            //: initialize Mglobal
            vnl_matrix<double> Mglobal(point_set_list_[pivot_frame_no].size(),point_set_list_[i].size());
            Mglobal.fill(1/(float)point_set_list_[pivot_frame_no].size());
            //: create the global matrix
            for(int p=0;p<static_cast<int>(matchsets.size());p++)
                {
                   dbrl_match_set_sptr s=matchsets[p];
                   for(int k=0;k<s->size_set1();k++)
                       {
                       dbrl_feature_sptr pivot_feature=s->feature_set1_i(k);
                       for(int l=0;l<s->size_set2();l++)
                           {
                                dbrl_feature_sptr current_feature=s->feature_set2_i(l);
                                double weight=(*s)(k,l);
                                if(Mglobal(pivot_feature->getid()-1,current_feature->getid()-1)<weight)
                                    Mglobal(pivot_feature->getid()-1,current_feature->getid()-1)=weight;
                           }
                       }
                }
            dbrl_correspondence Mg(Mglobal);
            Mg.normalize();
            Mg.setinitialoutlier(tpsaffineparams.outlier());
            dbrl_match_set_sptr msp=new dbrl_match_set();
            msp->set_correspondence_matrix(Mg);
            msp->set_original_features(point_set_list_[pivot_frame_no],point_set_list_[i]);
            msp->set_mapped_features(point_set_list_[pivot_frame_no],point_set_list_[i]);
            tps_storage->set_match_set(msp);
            
            }
        else
            {
            //unused dbrl_estimator_point_thin_plate_spline *tps_est= new dbrl_estimator_point_thin_plate_spline();
            //unused dbrl_rpm_tps *tpsrpm=new dbrl_rpm_tps(tpsparams,point_set_list_[pivot_frame_no],point_set_list_[pivot_frame_no]);
            dbrl_transformation_sptr tform;
            //: initialize Mglobal
            vnl_matrix<double> Mglobal(point_set_list_[pivot_frame_no].size(),point_set_list_[pivot_frame_no].size());
            Mglobal.set_identity();
            dbrl_correspondence Mg(Mglobal);
            Mg.setinitialoutlier(tpsaffineparams.outlier());
            Mg.normalize();
            
            
            dbrl_match_set_sptr msp=new dbrl_match_set();
            msp->set_correspondence_matrix(Mg);
            msp->set_original_features(point_set_list_[pivot_frame_no],point_set_list_[i]);
            msp->set_mapped_features(point_set_list_[pivot_frame_no],point_set_list_[i]);
            tps_storage->set_match_set(msp);
            
            }
        static float scale=100;
        parameters()->get_value("-upscale",scale);
        vil_image_view<unsigned char> img((int)scale,(int)scale,1);
        img.fill(0);
        vcl_vector<vsol_spatial_object_2d_sptr> xformedpoints;
        vidpro1_vsol2D_storage_sptr xformedpoints_storage=vidpro1_vsol2D_storage_new();
        
        for(int p=0;p<static_cast<int>(xformed_point_set_list[i].size());p++)
            {
            for(int m=0;m<static_cast<int>(xformed_point_set_list[i][p].size());m++)
                {
                dbrl_feature_point* pt1=dynamic_cast<dbrl_feature_point*>(xformed_point_set_list[i][p][m].ptr());
                float x=pt1->location()[0];
                float y=pt1->location()[1];
                if(x<1 && y<1 && x>=0 && y>=0)
                    img((int)(x*scale),(int)(y*scale))=255;
                vsol_point_2d_sptr ptvsol=new vsol_point_2d(x*scale,y*scale);
                xformedpoints.push_back(ptvsol->cast_to_spatial_object());
                }
            }
        vcl_string pointsetname=vul_sprintf("points%i",i);
        xformedpoints_storage->add_objects(xformedpoints,pointsetname);
        //vidpro1_image_storage_sptr img_storage = vidpro1_image_storage_new();
        //img_storage->set_image(vil_new_image_resource_of_view(img));
        //output_data_[framenums_[i]].push_back(img_storage);
        //output_data_[framenums_[i]].push_back(affine_storage);
        output_data_[framenums_[i]].push_back(tps_storage);
        //output_data_[framenums_[i]].push_back(grid_storage);
        //output_data_[framenums_[i]].push_back(warped_grid_storage);
        //output_data_[framenums_[i]].push_back(warped_grid_storage);
        xformedpoints_storage->mark_global();
        output_data_[framenums_[i]].push_back(xformedpoints_storage);
        //: Fine TPS transformation
        }
    return true;
    }


vcl_vector<dbrl_feature_sptr> dbrl_rpm_tps_affine_local_global_process::gridify_points(vcl_vector<dbrl_feature_sptr> points, float i0,float j0,float i1,float j1)
    {
    vcl_vector<dbrl_feature_sptr> gpoints;
    for(int i=0;i<static_cast<int>(points.size());i++)
        {
        if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(points[i].ptr()))
            {
            if(pt->location()[0]>i0 &&  pt->location()[0]<i1 && pt->location()[1]>j0 && pt->location()[1]<j1)
                gpoints.push_back(points[i]);
            }
        }
    return gpoints;
    }


void dbrl_rpm_tps_affine_local_global_process::remove_points_on_the_border(vcl_vector<dbrl_feature_sptr> point1,
                                                                           vcl_vector<dbrl_feature_sptr> point2,
                                                                           float i0,float j0,float i1,float j1,
                                                                           dbrl_correspondence *M)
    {
    vnl_matrix<double> Mcorr= M->M();
    for(int i=0;i<static_cast<int>(point1.size());i++)
        {
        if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(point1[i].ptr()))
            {
            if(pt->location()[0]<i0 ||  pt->location()[0]>i1 || pt->location()[1]<j0 || pt->location()[1]>j1)
                for(int j=0;j<static_cast<int>(Mcorr.cols());j++)
                    Mcorr[i][j]=0;
            }
        }
    for(int i=0;i<static_cast<int>(point2.size());i++)
        {
        if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(point2[i].ptr()))
            {
            if(pt->location()[0]<i0 ||  pt->location()[0]>i1 || pt->location()[1]<j0 || pt->location()[1]>j1)
                for(int j=0;j<static_cast<int>(Mcorr.rows());j++)
                    Mcorr[j][i]=0;
            }
        }
    M->updateM(Mcorr);
    }
