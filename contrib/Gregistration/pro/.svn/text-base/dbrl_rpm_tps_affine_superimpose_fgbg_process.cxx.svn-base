
#include "dbrl_rpm_tps_affine_superimpose_fgbg_process.h"
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

#include <vil/vil_image_view.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vnl/vnl_matlab_filewrite.h>
#include <vil/vil_math.h>

//: Constructor
dbrl_rpm_tps_affine_superimpose_fgbg_process::dbrl_rpm_tps_affine_superimpose_fgbg_process(void): bpro1_process()
    {

    if( !parameters()->add( "Scale Down" , "-downscale" , (float)100) ||
        !parameters()->add( "Initial temperature " , "-initT" , (float)1) ||
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
        !parameters()->add( "Outlier valuev (affine)", "-affmoutlier" , (float) 1e-5 )||
        !parameters()->add( "Scale the points to a grid", "-upscale" , (float) 100 )
        ) 
        {
        vcl_cerr << "ERROR: Adding parameters in dbrl_rpm_tps_affine_superimpose_fgbg_process::dbrl_rpm_tps_affine_superimpose_fgbg_process()" << vcl_endl;
        }

    }

//: Destructor
dbrl_rpm_tps_affine_superimpose_fgbg_process::~dbrl_rpm_tps_affine_superimpose_fgbg_process()
    {

    }


//: Return the name of this process
vcl_string
dbrl_rpm_tps_affine_superimpose_fgbg_process::name()
    {
    return "Rpm TPS AFFINE Superimpose BG & FG";
    }


//: Return the number of input frame for this process
int
dbrl_rpm_tps_affine_superimpose_fgbg_process::input_frames()
    {
    return 1;
    }


//: Return the number of output frames for this process
int
dbrl_rpm_tps_affine_superimpose_fgbg_process::output_frames()
    {
    return 1;
    }


//: Provide a vector of required input types
vcl_vector< vcl_string > dbrl_rpm_tps_affine_superimpose_fgbg_process::get_input_type()
    {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "image" );
    to_return.push_back( "image" );
    return to_return;
    }


//: Provide a vector of output types
vcl_vector< vcl_string > dbrl_rpm_tps_affine_superimpose_fgbg_process::get_output_type()
    {  
    vcl_vector<vcl_string > to_return;
    to_return.push_back( "image" );
    to_return.push_back( "dbrl_match_set" );
    to_return.push_back( "dbrl_match_set" );
    to_return.push_back( "vsol2D" );
    to_return.push_back( "vsol2D" );
        to_return.push_back( "image" );

    return to_return;
    }


//: Execute the process
bool
dbrl_rpm_tps_affine_superimpose_fgbg_process::execute()
    {
    if ( input_data_.size() != 1 ){
        vcl_cout << "In dbrl_rpm_tps_affine_superimpose_fgbg_process::execute() - "
            << "not exactly two input images \n";
        return false;
        }
clear_output();
vidpro1_image_storage_sptr imfg,imedge;
imfg.vertical_cast(input_data_[0][0]);
imedge.vertical_cast(input_data_[0][1]);

vil_image_resource_sptr fg_sptr = imfg->get_image();
vil_image_resource_sptr edge_sptr = imedge->get_image();

vil_image_view<unsigned char> fg=fg_sptr->get_view();
vil_image_view<unsigned char> edge=edge_sptr->get_view();
vil_image_view<unsigned char> bg;

vil_math_image_abs_difference(fg,edge,bg);
static float downscale=100;
parameters()->get_value("-downscale",downscale);
vcl_vector<dbrl_feature_sptr> feature_list;
for(unsigned i=0;i<fg.ni();i++)
    {   
        for(unsigned j=0;j<fg.nj();j++)
            {
                if(fg(i,j)>0)
                    {
                    dbrl_feature_point * fpt=new dbrl_feature_point((double)i/downscale,(double)j/downscale);
                    feature_list.push_back(fpt);
                    }
            }
    }
fg_point_set_list_.push_back(feature_list);
framenums_.push_back(imedge->frame());
bg_image_list_.push_back(bg);

return true;  
    }
//: Clone the process
bpro1_process*
dbrl_rpm_tps_affine_superimpose_fgbg_process::clone() const
    {
    return new dbrl_rpm_tps_affine_superimpose_fgbg_process(*this);
    }

bool
dbrl_rpm_tps_affine_superimpose_fgbg_process::finish()
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
    
    static int pivot_frame_no=(int)(fg_point_set_list_.size()/2);
    
    vcl_vector<vcl_vector<dbrl_feature_sptr> > xformed_point_set_list=fg_point_set_list_;
    dbrl_rpm_affine_params affineparams(afflambdainit,affmconvg,affTinit,affTfinal,affmoutlier,affannealrate);
    dbrl_rpm_tps_params tpsparams(lambdainit1,lambdainit2,mconvg,Tinit,Tfinal,moutlier,annealrate);
    dbrl_rpm_affine_params tpsaffineparams(lambdainit2,mconvg,Tinit,Tfinal,moutlier,annealrate);
    
    //: outer loop for computing TPS from each frame to pivot frame
    for(int i=0;i<static_cast<int>(fg_point_set_list_.size());i++)
        {
        //: approximation alignment of point-sets 
        dbrl_match_set_sptr outset;
        dbrl_match_set_storage_sptr tps_storage = dbrl_match_set_storage_new();
        dbrl_match_set_storage_sptr affine_storage = dbrl_match_set_storage_new();
        vidpro1_vsol2D_storage_sptr grid_storage=vidpro1_vsol2D_storage_new();
        vidpro1_vsol2D_storage_sptr warped_grid_storage=vidpro1_vsol2D_storage_new();
        if(i<pivot_frame_no)
            {
            for(int j=i+1;j<pivot_frame_no;j++)
                {
                dbrl_rpm_affine affinerpm(affineparams,fg_point_set_list_[j],xformed_point_set_list[i]);
                dbrl_match_set_sptr outset=affinerpm.rpm();
                xformed_point_set_list[i].clear();
                xformed_point_set_list[i]=outset->get_xformed_feature_set2();
                affine_storage->set_match_set(outset);
                }
            double T=Tinit;
            dbrl_rpm_affine *affinerpm=new dbrl_rpm_affine(tpsaffineparams,xformed_point_set_list[pivot_frame_no],xformed_point_set_list[i]);
            dbrl_estimator_point_affine* affine_est= new dbrl_estimator_point_affine();
            dbrl_rpm_tps *tpsrpm=new dbrl_rpm_tps(tpsparams,xformed_point_set_list[pivot_frame_no],xformed_point_set_list[i]);
            dbrl_estimator_point_thin_plate_spline *tps_est= new dbrl_estimator_point_thin_plate_spline();
            dbrl_transformation_sptr tform;
            //: initialize M 
            dbrl_correspondence *M=new dbrl_correspondence(xformed_point_set_list[pivot_frame_no].size(),xformed_point_set_list[i].size());
            M->setinitialoutlier(tpsaffineparams.outlier());
            dbrl_match_set_sptr match_set;
            while(T>Tfinal)
                {
                if(T>Tchange)
                    {
//                    affinerpm->rpm_at(T,*M,affine_est,tform,xformed_point_set_list[pivot_frame_no],xformed_point_set_list[i],tpsaffineparams.initlambda()*T);
                    T*=tpsaffineparams.annealrate();
                    }
                if(T<=Tchange)
                    {   
                    tpsrpm->rpm_at(T,*M,tps_est,tform,xformed_point_set_list[pivot_frame_no],xformed_point_set_list[i],tpsparams.initlambda1()*T,tpsparams.initlambda2()*T);
                    T*=tpsparams.annealrate();
                    }
                vcl_cout<<"\n Temparutre is "<<T;
                }
            //: in order to solve multiple effects.
            M->binarize(0.51);
            //combining corresponding pairs from both background and foreground
            vcl_vector<dbrl_feature_sptr> f1all;
            vcl_vector<dbrl_feature_sptr> f2all;
            get_corresponding_pairs_bg(bg_image_list_[pivot_frame_no],bg_image_list_[i],f1all,f2all);
                        for(int k=0;k<static_cast<int>(f1all.size());k++)
                {   
                dbrl_feature_point *pt1=dynamic_cast<dbrl_feature_point *>(f1all[k].ptr());
                dbrl_feature_point *pt2=dynamic_cast<dbrl_feature_point *>(f2all[k].ptr());
                vcl_cout<<"("<<pt1->location()[0]<<","<<pt1->location()[1]<<")"
                            <<"\t"<<"("<<pt2->location()[0]<<","<<pt2->location()[1]<<")";
                }
            get_corresponding_pairs_fg(fg_point_set_list_[pivot_frame_no],fg_point_set_list_[i],M,f1all,f2all);
            vnl_matrix<double> Mall(f1all.size(),f1all.size());
            Mall.set_identity();
            dbrl_correspondence Mfgbg(Mall);
            //: retreiving final transform
            tps_est->set_lambda1(0.0);
            tps_est->set_lambda2(0.0);
            tform=tps_est->estimate(f1all,f2all,Mfgbg);
            dbrl_thin_plate_spline_transformation * tpstform=dynamic_cast<dbrl_thin_plate_spline_transformation *> (tform.ptr());
            tpstform->set_from_features(f2all);
            tpstform->transform();
            xformed_point_set_list[i].clear();
            xformed_point_set_list[i]=tpstform->get_to_features();
            match_set=new dbrl_match_set(Mfgbg,tform,tps_est);
            match_set->set_original_features(f1all,f2all);
            match_set->set_mapped_features(f1all, xformed_point_set_list[i]);
            tps_storage->set_match_set(match_set);
            //tpstform->print_transformation(vcl_cout);      

            //: setting grid
            vcl_vector<dbrl_feature_sptr> grid_pts=generate_grid();
            //: build K for the grid
            vcl_vector<dbrl_feature_sptr> warped_grid_pts=warp_grid(tpstform,grid_pts);

            vcl_vector<vsol_spatial_object_2d_sptr> vsol_grid_pts=feature_to_vsol(grid_pts);
            vcl_vector<vsol_spatial_object_2d_sptr> vsol_warped_grid_pts=feature_to_vsol(warped_grid_pts);

            //:  adding grid lines 
            grid_storage->add_objects(draw_grid(grid_pts,100,100),"grid");
            warped_grid_storage->add_objects(draw_grid(warped_grid_pts,100,100),"warped_grid");

            static float scale=100;
            parameters()->get_value("-upscale",scale);
            vil_image_view<unsigned char> img((int)scale,(int)scale,1);
            img.fill(0);
            for(int p=0;p<static_cast<int>(xformed_point_set_list[i].size());p++)
                {
                dbrl_feature_point* pt1=dynamic_cast<dbrl_feature_point*>(xformed_point_set_list[i][p].ptr());
                float x=pt1->location()[0];
                float y=pt1->location()[1];
                if(x<1 && y<1 && x>=0 && y>=0)
                    img((int)(x*scale),(int)(y*scale))=255;
                }
            vidpro1_image_storage_sptr img_storage = vidpro1_image_storage_new();
            img_storage->set_image(vil_new_image_resource_of_view(img));

            vidpro1_image_storage_sptr bg_img_storage = vidpro1_image_storage_new();
            bg_img_storage->set_image(vil_new_image_resource_of_view(bg_image_list_[i]));
            output_data_[framenums_[i]].push_back(img_storage);
            output_data_[framenums_[i]].push_back(affine_storage);
            output_data_[framenums_[i]].push_back(tps_storage);
            output_data_[framenums_[i]].push_back(grid_storage);
            output_data_[framenums_[i]].push_back(warped_grid_storage);
            output_data_[framenums_[i]].push_back(bg_img_storage);
            }
        else if(i>pivot_frame_no)
            {
            for(int j=i-1;j>pivot_frame_no;j--)
                {
                dbrl_rpm_affine affinerpm(affineparams,fg_point_set_list_[j],xformed_point_set_list[i]);
                dbrl_match_set_sptr outset=affinerpm.rpm();
                xformed_point_set_list[i].clear();
                xformed_point_set_list[i]=outset->get_xformed_feature_set2();
                affine_storage->set_match_set(outset);
                }
            double T=Tinit;

            dbrl_rpm_affine *affinerpm=new dbrl_rpm_affine(tpsaffineparams,xformed_point_set_list[pivot_frame_no],xformed_point_set_list[i]);
            dbrl_estimator_point_affine* affine_est= new dbrl_estimator_point_affine();

            dbrl_rpm_tps *tpsrpm=new dbrl_rpm_tps(tpsparams,xformed_point_set_list[pivot_frame_no],xformed_point_set_list[i]);
            dbrl_estimator_point_thin_plate_spline *tps_est= new dbrl_estimator_point_thin_plate_spline();
            dbrl_transformation_sptr tform;
            //: initialize M 
            dbrl_correspondence *M=new dbrl_correspondence(xformed_point_set_list[pivot_frame_no].size(),xformed_point_set_list[i].size());
            M->setinitialoutlier(tpsaffineparams.outlier());

            dbrl_match_set_sptr match_set;
            while(T>Tfinal)
                {
                if(T>Tchange)
                    {
                    affinerpm->rpm_at(T,*M,affine_est,tform,xformed_point_set_list[pivot_frame_no],xformed_point_set_list[i],tpsaffineparams.initlambda()*T);
                    T*=tpsaffineparams.annealrate();
                    }
                if(T<=Tchange)
                    {   
                    tpsrpm->rpm_at(T,*M,tps_est,tform,xformed_point_set_list[pivot_frame_no],xformed_point_set_list[i],tpsparams.initlambda1()*T,tpsparams.initlambda2()*T);
                    T*=tpsparams.annealrate();
                    }
                vcl_cout<<"\n temperature is "<<T;
                }
            M->binarize(0.51);
            //combining corresponding pairs from both background and foreground
            vcl_vector<dbrl_feature_sptr> f1all;
            vcl_vector<dbrl_feature_sptr> f2all;

            get_corresponding_pairs_bg(bg_image_list_[pivot_frame_no],bg_image_list_[i],f1all,f2all);
            get_corresponding_pairs_fg(fg_point_set_list_[pivot_frame_no],fg_point_set_list_[i],M,f1all,f2all);

            vnl_matrix<double> Mall(f1all.size(),f1all.size());
            Mall.set_identity();

            dbrl_correspondence Mfgbg(Mall);
            //: retreiving final transform
            tps_est->set_lambda1(0.0);
            tps_est->set_lambda2(0.0);
            tform=tps_est->estimate(f1all,f2all,Mfgbg);
            dbrl_thin_plate_spline_transformation * tpstform=dynamic_cast<dbrl_thin_plate_spline_transformation *> (tform.ptr());
            tpstform->set_from_features(f2all);
            tpstform->transform();
            xformed_point_set_list[i].clear();
            xformed_point_set_list[i]=tpstform->get_to_features();

            match_set=new dbrl_match_set(Mfgbg,tform,tps_est);
            match_set->set_original_features(f1all,f2all);
            match_set->set_mapped_features(f1all,tpstform->get_to_features());
            tps_storage->set_match_set(match_set);

            tpstform->print_transformation(vcl_cout);
            //: setting grid

            vcl_vector<dbrl_feature_sptr> grid_pts=generate_grid();
            //: build K for the grid
            vcl_vector<dbrl_feature_sptr> warped_grid_pts=warp_grid(tpstform,grid_pts);

            vcl_vector<vsol_spatial_object_2d_sptr> vsol_grid_pts=feature_to_vsol(grid_pts);
            vcl_vector<vsol_spatial_object_2d_sptr> vsol_warped_grid_pts=feature_to_vsol(warped_grid_pts);

            //:  adding grid points
            //grid_storage->add_objects(vsol_grid_pts,"grid");
            //grid_storage->add_objects(vsol_warped_grid_pts,"warped_grid");

            //:  adding grid lines 
            grid_storage->add_objects(draw_grid(grid_pts,100,100),"grid");
            warped_grid_storage->add_objects(draw_grid(warped_grid_pts,100,100),"warped_grid");

            static float scale=100;
            parameters()->get_value("-upscale",scale);
            vil_image_view<unsigned char> img((int)scale,(int)scale,1);
            img.fill(0);
            for(int p=0;p<static_cast<int>(xformed_point_set_list[i].size());p++)
                {
                dbrl_feature_point* pt1=dynamic_cast<dbrl_feature_point*>(xformed_point_set_list[i][p].ptr());
                float x=pt1->location()[0];
                float y=pt1->location()[1];
                if(x<1 && y<1 && x>=0 && y>=0)
                    img((int)(x*scale),(int)(y*scale))=255;
                }
            vidpro1_image_storage_sptr img_storage = vidpro1_image_storage_new();
            img_storage->set_image(vil_new_image_resource_of_view(img));

            vidpro1_image_storage_sptr bg_img_storage = vidpro1_image_storage_new();
            bg_img_storage->set_image(vil_new_image_resource_of_view(bg_image_list_[i]));
            output_data_[framenums_[i]].push_back(img_storage);
            output_data_[framenums_[i]].push_back(affine_storage);
            output_data_[framenums_[i]].push_back(tps_storage);
            output_data_[framenums_[i]].push_back(grid_storage);
            output_data_[framenums_[i]].push_back(warped_grid_storage);
            output_data_[framenums_[i]].push_back(bg_img_storage);

            }
        else
            {
            }

        //: Fine TPS transformation

        }

    return true;
    }


    vcl_vector<vsol_spatial_object_2d_sptr> dbrl_rpm_tps_affine_superimpose_fgbg_process::feature_to_vsol(vcl_vector<dbrl_feature_sptr> & f)
        {
        vcl_vector<vsol_spatial_object_2d_sptr> vpts;
        for(unsigned i=0;i<f.size();i++)
            if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(f[i].ptr()))
                {
                vsol_point_2d_sptr p=new vsol_point_2d(pt->location()[0],pt->location()[1]);
                vpts.push_back(p->cast_to_spatial_object());
                }

            return vpts;
        }


    vcl_vector<dbrl_feature_sptr> dbrl_rpm_tps_affine_superimpose_fgbg_process::generate_grid()
        {
        vcl_vector<dbrl_feature_sptr> grid_pts;    
        for(int i=0;i<100;i++)
                for(int j=0;j<100;j++)
                    {
                        dbrl_feature_point * p=new dbrl_feature_point(i*0.01,j*0.01);
                        grid_pts.push_back(p);
                    }

                return grid_pts;
        }


    vcl_vector<dbrl_feature_sptr> dbrl_rpm_tps_affine_superimpose_fgbg_process::warp_grid(dbrl_thin_plate_spline_transformation * tpstform,vcl_vector<dbrl_feature_sptr> &f)
        {
        tpstform->build_K(f);
        tpstform->set_from_features(f);
        tpstform->transform();
        return tpstform->get_to_features();
        }


    vcl_vector<vsol_spatial_object_2d_sptr> dbrl_rpm_tps_affine_superimpose_fgbg_process::draw_grid(vcl_vector<dbrl_feature_sptr> grid_pts, int rows, int cols)
        {
            //: drawing horizontal lines

            vcl_vector<vsol_spatial_object_2d_sptr> lines;
            for(int i=0;i<rows-1;i++)
                {
                for(int j=0;j<cols-1;j++)
                    {
                    if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(grid_pts[i*cols+j].ptr()))
                        {
                        vsol_point_2d_sptr p=new vsol_point_2d(pt->location()[0],pt->location()[1]);
                        dbrl_feature_point* pt1=dynamic_cast<dbrl_feature_point*>(grid_pts[i*cols+j+1].ptr());
                        dbrl_feature_point* pt2=dynamic_cast<dbrl_feature_point*>(grid_pts[(i+1)*cols+j].ptr());
                        vsol_point_2d_sptr p1=new vsol_point_2d(pt1->location()[0],pt1->location()[1]);
                        vsol_point_2d_sptr p2=new vsol_point_2d(pt2->location()[0],pt2->location()[1]);
                        vsol_line_2d_sptr l1=new vsol_line_2d(p,p1);
                        vsol_line_2d_sptr l2=new vsol_line_2d(p,p2);
                        lines.push_back(l1->cast_to_spatial_object());
                        lines.push_back(l2->cast_to_spatial_object());
                        }
                    }
                }

            return lines;
        }


void dbrl_rpm_tps_affine_superimpose_fgbg_process::get_corresponding_pairs_fg
                                                   (vcl_vector<dbrl_feature_sptr> &f1in,vcl_vector<dbrl_feature_sptr> &f2in, 
                                                    dbrl_correspondence * M,vcl_vector<dbrl_feature_sptr> &f1,
                                                    vcl_vector<dbrl_feature_sptr> &f2)
    {
    for(int i=0;i<static_cast<int>(M->rows());i++)
        {
        for(int j=0;j<static_cast<int>(M->cols());j++)
            {
            if((*M)(i,j)==1)
                {
                f1.push_back(f1in[i]);
                f2.push_back(f2in[j]);
                }
            }
        }
    }

void dbrl_rpm_tps_affine_superimpose_fgbg_process::
    get_corresponding_pairs_bg(vil_image_view<unsigned char> & img1,vil_image_view<unsigned char> & img2,
                               vcl_vector<dbrl_feature_sptr> &f1,vcl_vector<dbrl_feature_sptr> &f2)
    {
    vil_image_view<unsigned char> im;
    static float downscale=100;
    parameters()->get_value("-downscale",downscale);
    for(unsigned i=0;i<img1.ni();i++)
        {
        for(unsigned j=0;j<img1.nj();j++)
            {
            if(img1(i,j)==255 && img2(i,j)==255)
                {
                f1.push_back(new dbrl_feature_point((double)i/downscale,(double)j/downscale));
                f2.push_back(new dbrl_feature_point((double)i/downscale,(double)j/downscale));
                }
            }
        }
    }
