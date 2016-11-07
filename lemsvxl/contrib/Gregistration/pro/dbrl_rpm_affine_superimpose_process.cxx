
#include "dbrl_rpm_affine_superimpose_process.h"
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
#include <vnl/vnl_matlab_filewrite.h>
#include <vul/vul_sprintf.h>
#include <vul/vul_timer.h>

//: Constructor
dbrl_rpm_affine_superimpose_process::dbrl_rpm_affine_superimpose_process(void): bpro1_process()
    {

    if( !parameters()->add( "Initial temperature " , "-initT" , (float)1) ||
        !parameters()->add( "Annealing Rate" , "-annealrate" , (float) 0.93 )||
        !parameters()->add( "Final Temperature" , "-finalT" , (float) 0.0001 )||
        !parameters()->add( "Initial Lambda " , "-initlambda" , (float) 1000 )||
        !parameters()->add( "Covergence Tolerance" , "-mconvg" , (float) 0.1 )||
        !parameters()->add( "Outlier value" , "-moutlier" , (float) 1e-5 )||
        !parameters()->add( "Scale the points to a grid", "-upscale" , (float) 100 )||
        !parameters()->add( "From Points", "-from_points" , (bool) true )||
        !parameters()->add( "From Match Sets", "-from_match_set" , (bool) true )
        ) 
        {
        vcl_cerr << "ERROR: Adding parameters in dbrl_rpm_affine_superimpose_process::dbrl_rpm_affine_superimpose_process()" << vcl_endl;
        }

    }

//: Destructor
dbrl_rpm_affine_superimpose_process::~dbrl_rpm_affine_superimpose_process()
    {

    }


//: Return the name of this process
vcl_string
dbrl_rpm_affine_superimpose_process::name()
    {
    return "Rpm AFFINE Superimpose";
    }


//: Return the number of input frame for this process
int
dbrl_rpm_affine_superimpose_process::input_frames()
    {
    return 1;
    }


//: Return the number of output frames for this process
int
dbrl_rpm_affine_superimpose_process::output_frames()
    {
    return 1;
    }


//: Provide a vector of required input types
vcl_vector< vcl_string > dbrl_rpm_affine_superimpose_process::get_input_type()
    {
    vcl_vector< vcl_string > to_return;
    parameters()->get_value("-from_points",from_points_);
    parameters()->get_value("-from_match_set",from_match_set_);
    if(from_points_ && from_match_set_)
        {
        to_return.push_back( "dbrl_id_point_2d" );
        to_return.push_back( "dbrl_match_set" );
        }

    else if(from_match_set_) 
        to_return.push_back( "dbrl_match_set" );
    else if(from_points_) 
        to_return.push_back( "dbrl_id_point_2d" );
    
    return to_return;
    }


//: Provide a vector of output types
vcl_vector< vcl_string > dbrl_rpm_affine_superimpose_process::get_output_type()
    {  
    vcl_vector<vcl_string > to_return;
    to_return.push_back( "dbrl_match_set" );
    return to_return;
    }


//: Execute the process
bool
dbrl_rpm_affine_superimpose_process::execute()
    {
    if ( input_data_.size() != 1 ){
        vcl_cout << "In dbrl_rpm_affine_superimpose_process::execute() - "
            << "not exactly two input images \n";
        return false;
        }
static float scale=0.0;
parameters()->get_value("-upscale",scale);
clear_output();
dbrl_id_point_2d_storage_sptr frame_pts;
dbrl_match_set_storage_sptr frame_match_sets;

for(unsigned i=0;i<input_data_[0].size();i++)
    {
    if(input_data_[0][i]->type()=="dbrl_id_point_2d")
        frame_pts.vertical_cast(input_data_[0][i]);
    if(input_data_[0][i]->type()=="dbrl_match_set")
        frame_match_sets.vertical_cast(input_data_[0][i]);

    }

if(from_points_)
    {
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
    }
if(from_match_set_)
    {
        match_set_list_.push_back(frame_match_sets->matchset());
    }
framenums_.push_back(input_data_[0][0]->frame());
return true;  
    }
//: Clone the process
bpro1_process*
dbrl_rpm_affine_superimpose_process::clone() const
    {
    return new dbrl_rpm_affine_superimpose_process(*this);
    }

bool
dbrl_rpm_affine_superimpose_process::finish()
    {

    vul_timer timerec;
    timerec.mark();
    //: tps parameters
    static float Tinit=1.0;
    static float Tfinal=0.0001;
    static float annealrate=0.93;
    static float lambdainit=1000;
    static float mconvg=0.1;
    static float moutlier=1e-5;

    parameters()->get_value("-initT",Tinit);
    parameters()->get_value("-finalT",Tfinal);
    parameters()->get_value("-annealrate",annealrate);
    parameters()->get_value("-initlambda",lambdainit);
    parameters()->get_value("-mconvg",mconvg);
    parameters()->get_value("-moutlier",moutlier);
    
    dbrl_rpm_affine_params affineparams(lambdainit,mconvg,Tinit,Tfinal,moutlier,annealrate);


    //: intialize from match_sets
    if(from_match_set_)
        {
        for(unsigned i=0;i<match_set_list_.size();i++)
            {
            dbrl_match_set_sptr m=match_set_list_[i];
            dbrl_match_set_storage_sptr tps_storage = dbrl_match_set_storage_new();


            double T=Tinit;
            vcl_vector<dbrl_feature_sptr> xformed_pivot=m->get_feature_set1();
            vcl_vector<dbrl_feature_sptr> xformed_curr=m->get_feature_set2();

            dbrl_rpm_affine *affinerpm=new dbrl_rpm_affine(affineparams,xformed_pivot,xformed_curr);
            dbrl_estimator_point_affine* affine_est= new dbrl_estimator_point_affine();
            dbrl_transformation_sptr tform;

            //: initialize M 
            dbrl_correspondence M=m->get_correspondence();
            M.setinitialoutlier(affineparams.outlier());
            while(T>Tfinal)
                {
                affinerpm->rpm_at(T,M,affine_est,tform,xformed_pivot,xformed_curr,affineparams.initlambda()*T);
                T*=affineparams.annealrate();
                vcl_cout<<"\n Temparutre is "<<T;
                }
            //: in order to solve multiple effects.
            M.binarize(0.51);
            //: retreiving final transform
            affine_est->set_lambda(0.0);

            vcl_vector<dbrl_feature_sptr> f1xform=m->get_feature_set1();
            tform=affine_est->estimate(f1xform,m->get_feature_set2(),M);
            dbrl_affine_transformation * affinetform=dynamic_cast<dbrl_affine_transformation *> (tform.ptr());
            affinetform->set_from_features(m->get_feature_set2());
            affinetform->transform();
            xformed_curr.clear();
            xformed_curr=affinetform->get_to_features();
            m->set_mapped_features(xformed_pivot,xformed_curr);
            tps_storage->set_match_set(m);
            affinetform->print_transformation(vcl_cout);      

            static float scale=100;
            parameters()->get_value("-upscale",scale);
            vil_image_view<unsigned char> img((int)scale,(int)scale,1);
            img.fill(0);
            vcl_vector<vsol_spatial_object_2d_sptr> xformedpoints;
            vidpro1_vsol2D_storage_sptr xformedpoints_storage=vidpro1_vsol2D_storage_new();
            for(int p=0;p<static_cast<int>(xformed_curr.size());p++)
                {
                dbrl_feature_point* pt1=dynamic_cast<dbrl_feature_point*>(xformed_curr[p].ptr());
                float x=pt1->location()[0];
                float y=pt1->location()[1];
                if(x<1 && y<1 && x>=0 && y>=0)
                    img((int)(x*scale),(int)(y*scale))=255;

                vsol_point_2d_sptr ptvsol=new vsol_point_2d(x*scale,y*scale);
                xformedpoints.push_back(ptvsol->cast_to_spatial_object());
                }
            vcl_string pointsetname=vul_sprintf("points%i",i);
            xformedpoints_storage->add_objects(xformedpoints,pointsetname);
            vidpro1_image_storage_sptr img_storage = vidpro1_image_storage_new();
            img_storage->set_image(vil_new_image_resource_of_view(img));
            output_data_[framenums_[i]].push_back(tps_storage);

            }
        }
   
    else{

        static int pivot_frame_no=(int)(point_set_list_.size()/2);
        vcl_vector<vcl_vector<dbrl_feature_sptr> > xformed_point_set_list=point_set_list_;

        //: computing CM for pivot frame
        vnl_vector<double> cm_p=center_of_mass(point_set_list_[pivot_frame_no]);
        //: outer loop for computing TPS from each frame to pivot frame
        for(int i=0;i<static_cast<int>(point_set_list_.size());i++)
            {

            vnl_vector<double> cm_i=center_of_mass(point_set_list_[i]);


            dbrl_translation tr(cm_p-cm_i);
            //: approximation alignment of point-sets 
            dbrl_match_set_sptr outset;
            dbrl_match_set_storage_sptr tps_storage = dbrl_match_set_storage_new();

            vidpro1_vsol2D_storage_sptr grid_storage=vidpro1_vsol2D_storage_new();
            vidpro1_vsol2D_storage_sptr warped_grid_storage=vidpro1_vsol2D_storage_new();

            if(i<pivot_frame_no)
                {
                tr.set_from_features(point_set_list_[i]);
                tr.transform();
            xformed_point_set_list[i].clear();
            xformed_point_set_list[i]=tr.get_to_features();
        
            

            double T=Tinit;
        
            dbrl_rpm_affine *affinerpm=new dbrl_rpm_affine(affineparams,xformed_point_set_list[pivot_frame_no],xformed_point_set_list[i]);
            dbrl_estimator_point_affine* affine_est= new dbrl_estimator_point_affine();

            dbrl_transformation_sptr tform;

            //: initialize M 
            dbrl_correspondence *M=new dbrl_correspondence(xformed_point_set_list[pivot_frame_no].size(),xformed_point_set_list[i].size());
            M->setinitialoutlier(affineparams.outlier());

            dbrl_match_set_sptr match_set;
            while(T>Tfinal)
                {
                affinerpm->rpm_at(T,*M,affine_est,tform,xformed_point_set_list[pivot_frame_no],xformed_point_set_list[i],affineparams.initlambda()*T);
                T*=affineparams.annealrate();
                vcl_cout<<"\n Temparutre is "<<T;
                }
            //: retreiving final transform
            affine_est->set_lambda(0.0);
            vcl_vector<dbrl_feature_sptr> f1xform=point_set_list_[pivot_frame_no];
            tform=affine_est->estimate(f1xform,point_set_list_[i],*M);
            dbrl_affine_transformation * affinetform=dynamic_cast<dbrl_affine_transformation *> (tform.ptr());
            affinetform->set_from_features(point_set_list_[i]);
            affinetform->transform();
            xformed_point_set_list[i].clear();
            xformed_point_set_list[i]=affinetform->get_to_features();
            match_set=new dbrl_match_set(*M,tform,affine_est);
            match_set->set_original_features(point_set_list_[pivot_frame_no],point_set_list_[i]);
            match_set->set_mapped_features(xformed_point_set_list[pivot_frame_no],xformed_point_set_list[i]);
            tps_storage->set_match_set(match_set);
            affinetform->print_transformation(vcl_cout);      

            }
        else if(i>pivot_frame_no)
            {
            tr.set_from_features(point_set_list_[i]);
            tr.transform();
            xformed_point_set_list[i].clear();
            xformed_point_set_list[i]=tr.get_to_features();
            double T=Tinit;

            dbrl_rpm_affine *affinerpm=new dbrl_rpm_affine(affineparams,xformed_point_set_list[pivot_frame_no],xformed_point_set_list[i]);
            dbrl_estimator_point_affine* affine_est= new dbrl_estimator_point_affine();
            dbrl_transformation_sptr tform;
            //: initialize M 
            dbrl_correspondence *M=new dbrl_correspondence(xformed_point_set_list[pivot_frame_no].size(),xformed_point_set_list[i].size());
            M->setinitialoutlier(affineparams.outlier());

            dbrl_match_set_sptr match_set;
            while(T>Tfinal)
                {
                    affinerpm->rpm_at(T,*M,affine_est,tform,xformed_point_set_list[pivot_frame_no],xformed_point_set_list[i],affineparams.initlambda()*T);
                    T*=affineparams.annealrate();
                    vcl_cout<<"\n temperature is "<<T;
                }
            //M->binarize(0.51);
            affine_est->set_lambda(0.0);
            
            vcl_vector<dbrl_feature_sptr> f1xform=point_set_list_[pivot_frame_no];
            tform=affine_est->estimate(f1xform,point_set_list_[i],*M);
            dbrl_affine_transformation * affinetform=dynamic_cast<dbrl_affine_transformation *> (tform.ptr());
            affinetform->set_from_features(point_set_list_[i]);
            affinetform->transform();
            xformed_point_set_list[i].clear();
            xformed_point_set_list[i]=affinetform->get_to_features();

            match_set=new dbrl_match_set(*M,tform,affine_est);
            match_set->set_original_features(point_set_list_[pivot_frame_no],point_set_list_[i]);
            match_set->set_mapped_features(xformed_point_set_list[pivot_frame_no],xformed_point_set_list[i]);
            tps_storage->set_match_set(match_set);

            affinetform->print_transformation(vcl_cout);
            }
        else
            {
            dbrl_estimator_point_affine *affine_est= new dbrl_estimator_point_affine();
            //unused dbrl_rpm_affine *affinerpm=new dbrl_rpm_affine(affineparams,xformed_point_set_list[pivot_frame_no],xformed_point_set_list[i]);

            dbrl_transformation_sptr tform;
            //: initialize M 
            vnl_matrix<double> Mdiag(xformed_point_set_list[pivot_frame_no].size(),xformed_point_set_list[pivot_frame_no].size());
            Mdiag.set_identity();
            dbrl_correspondence *M=new dbrl_correspondence(Mdiag);
            M->setinitialoutlier(affineparams.outlier());
            dbrl_match_set_sptr match_set;
           
            
            affine_est->set_lambda(0.0);
            
            vcl_vector<dbrl_feature_sptr> f1xform=point_set_list_[pivot_frame_no];
            tform=affine_est->estimate(f1xform,point_set_list_[i],*M);
            dbrl_affine_transformation * affinetform=dynamic_cast<dbrl_affine_transformation *> (tform.ptr());
            affinetform->set_from_features(point_set_list_[i]);
            affinetform->transform();
            xformed_point_set_list[i].clear();
            xformed_point_set_list[i]=affinetform->get_to_features();

            match_set=new dbrl_match_set(*M,tform,affine_est);
            match_set->set_original_features(point_set_list_[pivot_frame_no],point_set_list_[i]);
            match_set->set_mapped_features(xformed_point_set_list[pivot_frame_no],xformed_point_set_list[i]);
            tps_storage->set_match_set(match_set);

            affinetform->print_transformation(vcl_cout);
            }
        static float scale=100;
        parameters()->get_value("-upscale",scale);
        vil_image_view<unsigned char> img((int)scale,(int)scale,1);
        img.fill(0);
        vcl_vector<vsol_spatial_object_2d_sptr> xformedpoints;
        vidpro1_vsol2D_storage_sptr xformedpoints_storage=vidpro1_vsol2D_storage_new();
        for(int p=0;p<static_cast<int>(xformed_point_set_list[i].size());p++)
            {
            dbrl_feature_point* pt1=dynamic_cast<dbrl_feature_point*>(xformed_point_set_list[i][p].ptr());
            float x=pt1->location()[0];
            float y=pt1->location()[1];
            if(x<1 && y<1 && x>=0 && y>=0)
                img((int)(x*scale),(int)(y*scale))=255;

            vsol_point_2d_sptr ptvsol=new vsol_point_2d(x*scale,y*scale);
            xformedpoints.push_back(ptvsol->cast_to_spatial_object());
            }
        vcl_string pointsetname=vul_sprintf("points%i",i);
        xformedpoints_storage->add_objects(xformedpoints,pointsetname);
        vidpro1_image_storage_sptr img_storage = vidpro1_image_storage_new();
        img_storage->set_image(vil_new_image_resource_of_view(img));
        //output_data_[framenums_[i]].push_back(img_storage);
        output_data_[framenums_[i]].push_back(tps_storage);
        //: Fine TPS transformation

        }
}
vcl_cout<<"\nThe time taken to compute match set is: "<<timerec.all();
    return true;
    }


    vcl_vector<vsol_spatial_object_2d_sptr> dbrl_rpm_affine_superimpose_process::feature_to_vsol(vcl_vector<dbrl_feature_sptr> & f)
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


    vnl_vector<double> dbrl_rpm_affine_superimpose_process::center_of_mass(vcl_vector<dbrl_feature_sptr> & f)
        {
        vcl_vector<vsol_spatial_object_2d_sptr> vpts;
        double cx=0.0;
        double cy=0.0;

        for(unsigned i=0;i<f.size();i++)
            if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(f[i].ptr()))
                {
                cx+=pt->location()[0];
                cy+=pt->location()[1];
                }
        vnl_vector_fixed<double,2> p(cx/f.size(),cy/f.size());
           
        return p;
        }

    vcl_vector<dbrl_feature_sptr> dbrl_rpm_affine_superimpose_process::generate_grid()
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


    vcl_vector<dbrl_feature_sptr> dbrl_rpm_affine_superimpose_process::warp_grid(dbrl_thin_plate_spline_transformation * tpstform,vcl_vector<dbrl_feature_sptr> &f)
        {
        tpstform->build_K(f);
        tpstform->set_from_features(f);
        tpstform->transform();
        return tpstform->get_to_features();
        }


    vcl_vector<vsol_spatial_object_2d_sptr> dbrl_rpm_affine_superimpose_process::draw_grid(vcl_vector<dbrl_feature_sptr> grid_pts, int rows, int cols)
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
