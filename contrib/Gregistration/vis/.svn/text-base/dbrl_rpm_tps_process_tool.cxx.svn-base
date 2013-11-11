#include <vis/dbrl_rpm_tps_process_tool.h>
#include <vis/dbrl_match_set_tableau.h>

#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui.h>
#include <vcl_algorithm.h>
#include <vidpro1/vidpro1_repository.h>

#include <georegister/dbrl_rpm_tps.h>
#include <georegister/dbrl_rpm_affine.h>
#include <georegister/dbrl_translation.h>

#include <georegister/dbrl_feature_point.h>

#include <pro/dbrl_id_point_2d_storage_sptr.h>
#include <pro/dbrl_id_point_2d_storage.h>

dbrl_rpm_tps_process_tool::dbrl_rpm_tps_process_tool()
    {
    }

dbrl_rpm_tps_process_tool::~dbrl_rpm_tps_process_tool()
    {
    }

bool
dbrl_rpm_tps_process_tool::set_tableau( const vgui_tableau_sptr& tableau )
    {
    bgui_vsol2D_tableau_sptr temp_tab;
    temp_tab.vertical_cast(tableau);
    if (!temp_tab.ptr())
        return false;

    if (this->set_storage(bvis1_manager::instance()->storage_from_tableau(tableau))){
        tableau_ = temp_tab;
        return true;
        }
return false;
    }

bool
dbrl_rpm_tps_process_tool::set_storage ( const bpro1_storage_sptr& storage)
    {
    if (!storage.ptr())
        return false;
    //make sure its a vsol storage class
    if (storage->type() == "dbrl_id_point_2d"){
        storage_.vertical_cast(storage);
        return true;
        }
return false;
    }

bgui_vsol2D_tableau_sptr
dbrl_rpm_tps_process_tool::tableau()
    {
    return tableau_;
    }

dbrl_id_point_2d_storage_sptr
dbrl_rpm_tps_process_tool::storage()
    {
    dbrl_id_point_2d_storage_sptr match_storage;
    match_storage.vertical_cast(storage_);
    return match_storage;
    }

bool
dbrl_rpm_tps_process_tool::handle( const vgui_event & e, 
                                  const bvis1_view_tableau_sptr& view )
    {
    float ix, iy;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

    int frame = view->frame();

    if ( e.type == vgui_KEY_PRESS && e.key == 'i' && vgui_SHIFT)
        {
        pt1_.clear();
        //: points from current frame
        pt2_.clear();

        static int data_frame_no;
        static float scale;
        match_type="TPS";
        vgui_dialog dlg("Match Points(RPM)");
        dlg.field("Frame No(Relative)",data_frame_no);
        dlg.field("Match Type",match_type);
        dlg.field("Scale",scale);


        if(!dlg.ask())
            return true;

        dbrl_id_point_2d_storage_sptr pcurr;
        pcurr.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("dbrl_id_point_2d",frame));
        pt2_=pcurr->points();

        dbrl_id_point_2d_storage_sptr pdata;
        pdata.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("dbrl_id_point_2d",frame+data_frame_no));
        pt1_=pdata->points();

        vcl_vector< dbrl_id_point_2d_sptr >::iterator itr;
        for(itr=pt1_.begin();itr!=pt1_.end();itr++)
            {
            vsol_point_2d_sptr point=new vsol_point_2d((*itr)->x()/scale,(*itr)->y()/scale);
            vgui_style_sptr sty= vgui_style::new_style(0.0,1.0,0.0,2.0,4.0);
            tableau_->add_vsol_point_2d(point,sty);
            }

        for(itr=pt2_.begin();itr!=pt2_.end();itr++)
            {
            vsol_point_2d_sptr point=new vsol_point_2d((*itr)->x()/scale,(*itr)->y()/scale);
            vgui_style_sptr sty= vgui_style::new_style(1.0,0.0,0.0,2.0,4.0);
            tableau_->add_vsol_point_2d(point,sty);
            }

        for(int i=0;i<static_cast<int>(pt2_.size());i++)
            {
            vnl_vector_fixed<double,2> pt(pt2_[i]->x()/scale,pt2_[i]->y()/scale);
            dbrl_feature_point * fpt=new dbrl_feature_point(pt);
            f2.push_back(fpt);
            }
        for(int i=0;i<static_cast<int>(pt1_.size());i++)
            {
            vnl_vector_fixed<double,2> pt(pt1_[i]->x()/scale,pt1_[i]->y()/scale);
            dbrl_feature_point * fpt=new dbrl_feature_point(pt);
            f1.push_back(fpt);
            }

        if(match_type=="TPS")
            {
            static float Tinit=1.0;
            static float Tfinal=0.01;
            static float annealrate=0.93;
            static float lambdainit1=1.0;
            static float lambdainit2=0.01;
            static float mconvg=0.1;
            static float moutlier=1e-5;

            vgui_dialog dlgtps("TPS parameters");
            dlgtps.field("-initT",Tinit);
            dlgtps.field("-finalT",Tfinal);
            dlgtps.field("-annealrate",annealrate);
            dlgtps.field("-initlambda1",lambdainit1);
            dlgtps.field("-initlambda2",lambdainit2);
            dlgtps.field("-mconvg",mconvg);
            dlgtps.field("-moutlier",moutlier);

            if(!dlgtps.ask())
                return false;
            tpsparams=new dbrl_rpm_tps_params(lambdainit1,lambdainit2,mconvg,Tinit,Tfinal,moutlier,annealrate);
            tpsrpm=new dbrl_rpm_tps(*tpsparams,f1,f2);
            tps_est= new dbrl_estimator_point_thin_plate_spline();

            //: initialize M 
            M=new dbrl_correspondence(f1.size(),f2.size());
            M->setinitialoutlier(tpsparams->outlier());

            //: intializing annealing parameters
            T=tpsparams->initialT();
            f1x=f1;
            f2x=f2;
            Tf=tpsparams->finalT();
            }
        if(match_type=="AFFINE")
            {
            static float Tinit=1.0;
            static float Tfinal=0.01;
            static float annealrate=0.93;
            static float lambdainit=1.0;
            static float mconvg=0.1;
            static float moutlier=1e-5;

            vgui_dialog dlgtps("Affine parameters");
            dlgtps.field("-initT",Tinit);
            dlgtps.field("-finalT",Tfinal);
            dlgtps.field("-annealrate",annealrate);
            dlgtps.field("-initlambda",lambdainit);
            dlgtps.field("-mconvg",mconvg);
            dlgtps.field("-moutlier",moutlier);

            if(!dlgtps.ask())
                return false;
            affineparams=new dbrl_rpm_affine_params(lambdainit,mconvg,Tinit,Tfinal,moutlier,annealrate);
            affinerpm=new dbrl_rpm_affine(*affineparams,f1,f2);
            affine_est= new dbrl_estimator_point_affine();


            //: initialize M 
            M=new dbrl_correspondence(f1.size(),f2.size());
            M->setinitialoutlier(affineparams->outlier());

            //: intializing annealing parameters
            T=affineparams->initialT();
            Tf=affineparams->finalT();

            f1x=f1;
            f2x=f2;
            }
        if(match_type=="AFFINE+TPS")
            {
            static float Tinit=0.5;
            static float Tfinal=0.0001;
            Tchange=0.01;
            static float annealrate=0.93;
            static float lambdainit1=1.0;
            static float lambdainit2=0.01;
            static float mconvg=0.1;
            static float moutlier=1e-5;

            vgui_dialog dlgtps("AFFINE+TPS parameters");
            dlgtps.field("initT",Tinit);
            dlgtps.field("finalT",Tfinal);
            dlgtps.field("changeT",Tchange);
            dlgtps.field("annealrate",annealrate);
            dlgtps.field("initlambda1",lambdainit1);
            dlgtps.field("initlambda2",lambdainit2);
            dlgtps.field("mconvg",mconvg);
            dlgtps.field("moutlier",moutlier);

            if(!dlgtps.ask())
                return false;



            affineparams=new dbrl_rpm_affine_params(lambdainit2,mconvg,Tinit,Tchange,moutlier,annealrate);
            affinerpm=new dbrl_rpm_affine(*affineparams,f1,f2);
            affine_est= new dbrl_estimator_point_affine();

            tpsparams=new dbrl_rpm_tps_params(lambdainit1,lambdainit2,mconvg,Tchange,Tfinal,moutlier,annealrate);
            tpsrpm=new dbrl_rpm_tps(*tpsparams,f1,f2);
            tps_est= new dbrl_estimator_point_thin_plate_spline();


            //: initialize M 
            M=new dbrl_correspondence(f1.size(),f2.size());
            M->setinitialoutlier(affineparams->outlier());

            //: intializing annealing parameters
            T=Tinit;
            Tf=Tfinal;

            f1x=f1;
            f2x=f2;

            vnl_vector<double> cm_p=center_of_mass(f1x);
            vnl_vector<double> cm_i=center_of_mass(f2x);

            cm_i[0]+=0.1;
            dbrl_translation tr(cm_p-cm_i);
            tr.set_from_features(f2x);
            tr.transform();
            f2x.clear();
            f2x=tr.get_to_features();
            
            }
        //bvis1_manager::instance()->post_redraw();
        }
    if ( e.type == vgui_KEY_PRESS && e.key == 'u' && vgui_SHIFT)
        {  
        if(T>Tf)
            {
            if(match_type=="TPS")
                {
                tpsrpm->rpm_at(T,*M,tps_est,tform,f1x,f2x,tpsparams->initlambda1()*T,tpsparams->initlambda2()*T);
                T*=tpsparams->annealrate();
                match_set=new dbrl_match_set(*M,tform,tps_est);
                }
            else if(match_type=="AFFINE")
                {
                affinerpm->rpm_at(T,*M,affine_est,tform,f1x,f2x,affineparams->initlambda()*T);
                T*=affineparams->annealrate();
                match_set=new dbrl_match_set(*M,tform,affine_est);
                }
            else if(match_type=="AFFINE+TPS")
                {
                if(T>Tchange)
                    {
                    affinerpm->rpm_at(T,*M,affine_est,tform,f1x,f2x,affineparams->initlambda()*T);
                    T*=affineparams->annealrate();
                    match_set=new dbrl_match_set(*M,tform,affine_est);

                    }
                if(T<=Tchange)
                    {   
                    tpsrpm->rpm_at(T,*M,tps_est,tform,f1x,f2x,tpsparams->initlambda1()*T,tpsparams->initlambda2()*T);
                    T*=tpsparams->annealrate();
                    match_set=new dbrl_match_set(*M,tform,tps_est);

                    }
                }
            vcl_cout<<"\n Temperature = "<<T;

            match_set->set_original_features(f1,f2);
            match_set->set_mapped_features(f1x,f2x);
            }
bvis1_manager::instance()->post_redraw();
        }
  /*  if ( e.type == vgui_KEY_PRESS && e.key == 'd' && vgui_SHIFT)
        { 
                    bvis1_manager::instance()->post_redraw();

        }*/

    if ( e.type == vgui_KEY_PRESS && e.key == 'f' && vgui_SHIFT)
        {  
        f1.clear();
        f2.clear();

        f1x.clear();
        f2x.clear();

        pt1_.clear();
        pt2_.clear();

        tpsrpm=0;
        tpsparams=0;
        affineparams=0;
        M=0;
        tps_est=0;
        affine_est=0;
        tform=0;

        }

    if( e.type == vgui_DRAW){
        if(f1.size()>0 && f2.size()> 0 && match_set.ptr())
            {
            for(int i=0;i<static_cast<int>(f1.size());i++)
                {
                if(dbrl_feature_point *pt=dynamic_cast<dbrl_feature_point *>(match_set->feature_set1_i(i).ptr()))

                    {
                    
                    }
                }
            glBegin(GL_POINTS);
            
            glColor3f(0.0,0.0,1.0);
            for(int i=0;i<static_cast<int>(f2.size());i++)
                {
                        dbrl_feature_point *mappedpt1=dynamic_cast<dbrl_feature_point *>((match_set->xformed_feature_set2_i(i)).ptr());
                        glVertex2f(mappedpt1->location()[0], mappedpt1->location()[1]);
                }
            glEnd();
                    }
        }


return false;
    }



//: Return the name of this tool
vcl_string 
dbrl_rpm_tps_process_tool::name() const
    {

    return "Visualize Matching";

    }

 vnl_vector<double> dbrl_rpm_tps_process_tool::center_of_mass(vcl_vector<dbrl_feature_sptr> & f)
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
