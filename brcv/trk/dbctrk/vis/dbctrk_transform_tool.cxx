#include <dbctrk/vis/dbctrk_transform_tool.h>
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>
#include <dbctrk/vis/dbctrk_soview2d.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui.h>
#include <dbctrk/dbctrk_algs.h>
#include <dbctrk/dbctrk_curve_matching.h>
#include <vgui/vgui_dialog.h>
#include <dbctrk/pro/dbctrk_storage_sptr.h>
#include <vidpro1/vidpro1_repository.h>

dbctrk_transform_tool::dbctrk_transform_tool()
{

    neighbor_style_ = vgui_style::new_style(1.0, 0.0, 0.0, 3.0, 3.0);
    e_.set(-610,237);
    history_=2;

}

dbctrk_transform_tool::~dbctrk_transform_tool()
{
}

bool
dbctrk_transform_tool::set_tableau( const vgui_tableau_sptr& tableau )
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
dbctrk_transform_tool::set_storage ( const bpro1_storage_sptr& storage)
{
    if (!storage.ptr())
        return false;
    //make sure its a vsol storage class
    if (storage->type() == "dbctrk"){
        storage_.vertical_cast(storage);
        return true;
    }
    return false;
}

bgui_vsol2D_tableau_sptr
dbctrk_transform_tool::tableau()
{
    return tableau_;
}

dbctrk_storage_sptr
dbctrk_transform_tool::storage()
{
    dbctrk_storage_sptr dbctrk_storage;
    dbctrk_storage.vertical_cast(storage_);
    return dbctrk_storage;
}

bool
dbctrk_transform_tool::handle( const vgui_event & e, 
                              const bvis1_view_tableau_sptr& view )
{
    float ix, iy;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

    if( !tableau_.ptr() )
        return false;


    if( e.type == vgui_MOTION){
        tableau_->motion(e.wx, e.wy);
        vgui_soview2D* curr_obj =  (vgui_soview2D*)tableau_->get_highlighted_soview();

        object_ = (dbctrk_soview2D*)curr_obj;
        curr_curve_ = NULL;
        if( curr_obj && curr_obj->type_name() == "dbctrk_soview2D"){   
            curr_curve_ = object_->dbctrk_sptr();
        }
        vgui::out.clear();
        bvis1_manager::instance()->post_overlay_redraw();
    }

    //: tool to display T_1\bar{C_2}  and  T_2\bar{C_1}
    if( e.type == vgui_KEY_PRESS && e.key == 'i' && vgui_SHIFT){
        vcl_vector<vgui_soview*> all_objects;
        all_objects = tableau_->get_selected_soviews();
        if(all_objects.size()==2)
        {
            dbctrk_tracker_curve_sptr c1=((dbctrk_soview2D*)all_objects[0])->dbctrk_sptr();
            dbctrk_tracker_curve_sptr c2=((dbctrk_soview2D*)all_objects[1])->dbctrk_sptr();

            if(c1->frame_number == c2->frame_number)
            {
                if(c1->get_best_match_prev().ptr() && c2->get_best_match_prev().ptr())
                {
                    //: get T_1\bar{C_2}
                    vcl_vector<vgl_point_2d<double> > transformed_T1_C2;
                    dbctrk_algs::compute_transformation(c2->get_best_match_prev()->match_curve_set[0]->desc->curve_->pointarray(),
                        transformed_T1_C2,
                        c1->get_best_match_prev()->R_,
                        c1->get_best_match_prev()->Tbar,
                        c1->get_best_match_prev()->scale_);

                    vdgl_digital_curve_sptr dc1=dbctrk_algs::create_digital_curves(transformed_T1_C2);
                    bgui_vsol_soview2D_edgel_curve pcurve1(dc1);
                    neighbor_style_->rgba[0] =1 ;
                    neighbor_style_->rgba[1] =1;
                    neighbor_style_->rgba[2] =0;

                    neighbor_style_->apply_all();
                    dcs_.push_back(tableau_->add_edgel_curve(dc1));




                    //: get T_1\bar{C_2}
                    vcl_vector<vgl_point_2d<double> > transformed_T2_C1;
                    dbctrk_algs::compute_transformation(c1->get_best_match_prev()->match_curve_set[0]->desc->curve_->pointarray(),
                        transformed_T2_C1,
                        c2->get_best_match_prev()->R_,
                        c2->get_best_match_prev()->Tbar,
                        c2->get_best_match_prev()->scale_);

                    vdgl_digital_curve_sptr dc2=dbctrk_algs::create_digital_curves(transformed_T2_C1);
                    bgui_vsol_soview2D_edgel_curve pcurve2(dc2);
                    dcs_.push_back(tableau_->add_edgel_curve(dc2));

                }
                else
                    vcl_cout<<"\n one or both of the curves do not have match";
            }
            else
                vcl_cout<<"\n curves should be selected from the same frame";
        }
        else
        {
            vgui::out<<"requires only two curves \n";
        }
        tableau_->deselect_all();
    }

    if( e.type == vgui_KEY_PRESS && e.key == 'j' && vgui_SHIFT){
        vcl_vector<vgui_soview*> all_objects;
        all_objects = tableau_->get_selected_soviews();
        if(all_objects.size()>2)
        {
            dbctrk_tracker_curve_sptr c1=((dbctrk_soview2D*)all_objects[0])->dbctrk_sptr();
            for(unsigned int i=1;i<all_objects.size();i++)
            {
                dbctrk_tracker_curve_sptr c2=((dbctrk_soview2D*)all_objects[i])->dbctrk_sptr();

                if(c1->frame_number == c2->frame_number)
                {
                    if(c1->get_best_match_prev().ptr() && c2->get_best_match_prev().ptr())
                    {
                        //: get T_1\bar{C_2}
                        vcl_vector<vgl_point_2d<double> > transformed_T1_C2;
                        dbctrk_algs::compute_transformation(c2->get_best_match_prev()->match_curve_set[0]->desc->curve_->pointarray(),
                            transformed_T1_C2,
                            c1->get_best_match_prev()->R_,
                            c1->get_best_match_prev()->Tbar,
                            c1->get_best_match_prev()->scale_);

                        vdgl_digital_curve_sptr dc1=dbctrk_algs::create_digital_curves(transformed_T1_C2);
                        bgui_vsol_soview2D_edgel_curve pcurve1(dc1);
                        neighbor_style_->rgba[0] =1 ;
                        neighbor_style_->rgba[1] =1;
                        neighbor_style_->rgba[2] =0;

                        neighbor_style_->apply_all();
                        dcs_.push_back(tableau_->add_edgel_curve(dc1));       
                    }
                    else
                        vcl_cout<<"\n one or both of the curves do not have match";
                }
                else
                    vcl_cout<<"\n curves should be selected from the same frame";
            }
        }
        else
        {
            vgui::out<<"requires two or ore than curves \n";
        }
        tableau_->deselect_all();
    }

    //: set the history
    if( e.type == vgui_KEY_PRESS && e.key == 'k' && vgui_SHIFT){
        vgui_dialog his_dlg("No of previous frames");
        his_dlg.field("No of Previous frames ", history_);
        if(!his_dlg.ask())
            return false;
    }
    //: delete the digital curves added
    if( e.type == vgui_KEY_PRESS && e.key == 'u' ){
        for(unsigned int i=0;i<dcs_.size();i++)
        {
            tableau_->remove(dcs_[i]);
        }
        dcs_.clear();
    }
    // tool to compare T_2^3 T_1^2 C_1 - T_1^3 C_1 
    if( e.type == vgui_KEY_PRESS && e.key == 'f' && vgui_SHIFT){

        if(!curr_curve_ || curr_curve_->frame_number<2)
            return false;
        if(curr_curve_->get_best_match_prev().ptr())
        {
            if(curr_curve_->get_best_match_prev()->match_curve_set[0]->get_best_match_prev().ptr())
            {
                dbctrk_tracker_curve_sptr c1=curr_curve_->get_best_match_prev()->match_curve_set[0]->get_best_match_prev()->match_curve_set[0];
                dbctrk_tracker_curve_sptr c11=curr_curve_->get_best_match_prev()->match_curve_set[0];
                dbctrk_tracker_curve_sptr c111=curr_curve_;

                vcl_vector<vgl_point_2d<double> > Tc1;
                dbctrk_algs::compute_transformation(c1->desc->curve_->pointarray(),
                    Tc1,
                    c1->get_best_match_next()->R_,
                    c1->get_best_match_next()->Tbar,
                    c1->get_best_match_next()->scale_);
                vcl_vector<vgl_point_2d<double> > Tc2;
                dbctrk_algs::compute_transformation(Tc1,
                    Tc2,
                    c11->get_best_match_next()->R_,
                    c11->get_best_match_next()->Tbar,
                    c11->get_best_match_next()->scale_);



                vdgl_digital_curve_sptr dc=dbctrk_algs::create_digital_curves(Tc2);
                vgui_style_sptr temp_style_ = vgui_style::new_style(0.0, 1.0, 0.0, 2.0, 2.0);
                dcs_.push_back(tableau_->add_edgel_curve(dc,temp_style_));

                //: get T_1^3 C_1
                match_data_sptr m=new match_data;
                double cost1=curveMatch(c1->desc->curve_->pointarray(),
                    c111->desc->curve_->pointarray(),
                    m,e_,1,1,1);
                vcl_vector<vgl_point_2d<double> > Tc13_1;
                dbctrk_algs::compute_transformation(c1->desc->curve_->pointarray(),
                    Tc13_1,
                    m->R_,
                    m->Tbar,
                    m->scale_);

                vdgl_digital_curve_sptr dc1=dbctrk_algs::create_digital_curves(Tc13_1);

                vcl_reverse(c1->desc->curve_->pointarray().begin(),c1->desc->curve_->pointarray().end());
                // reverse one of the curves
                double cost2=curveMatch(c1->desc->curve_->pointarray(),
                    c111->desc->curve_->pointarray(),
                    m,e_,1,1,1);
                vcl_vector<vgl_point_2d<double> > Tc13_2;
                dbctrk_algs::compute_transformation(c1->desc->curve_->pointarray(),
                    Tc13_2,
                    m->R_,
                    m->Tbar,
                    m->scale_);
                vcl_reverse(c1->desc->curve_->pointarray().begin(),c1->desc->curve_->pointarray().end());
                vdgl_digital_curve_sptr dc2=dbctrk_algs::create_digital_curves(Tc13_2);



                neighbor_style_->rgba[0] =1 ;
                neighbor_style_->rgba[1] =0;
                neighbor_style_->rgba[2] =0;

                neighbor_style_->apply_all();
                if(cost1<cost2)
                    dcs_.push_back(tableau_->add_edgel_curve(dc1,neighbor_style_));

                else
                    dcs_.push_back(tableau_->add_edgel_curve(dc2,neighbor_style_));

                bvis1_manager::instance()->post_redraw();

            }  

        }
    }

    // tool to "generalize" T_2^3 T_1^2 C_1 - T_1^3 C_1 
    //: this tool acts in a forward direction
    if( e.type == vgui_KEY_PRESS && e.key == 'h' && vgui_SHIFT ){



        if(!curr_curve_ )
            return false;

        dbctrk_tracker_curve_sptr chaincurve=curr_curve_;
        vcl_vector<vgl_point_2d<double> > Tci;
        vcl_vector<vgl_point_2d<double> > Tcf;
        Tci=chaincurve->desc->curve_->pointarray();
        vcl_vector<dbctrk_tracker_curve_sptr> Tcurves; 
        for(int i=0;i<history_;i++)
        {
            if(chaincurve->get_best_match_prev().ptr())
            {

                chaincurve=chaincurve->get_best_match_prev()->match_curve_set[0];
                Tcurves.push_back(chaincurve);
            }
            else
            {
                vcl_cout<<"\nThe track is not long enough";
                return false;
            }


        }
        Tci=Tcurves[Tcurves.size()-1]->desc->curve_->pointarray();

        for(int i=Tcurves.size()-1;i>=0;i--)
        {


            dbctrk_algs::compute_transformation(Tci,
                Tcf,
                Tcurves[i]->get_best_match_next()->R_,
                Tcurves[i]->get_best_match_next()->Tbar,
                Tcurves[i]->get_best_match_next()->scale_);

            Tci=Tcf;
        }



        vdgl_digital_curve_sptr dc=dbctrk_algs::create_digital_curves(Tcf);
        vgui_style_sptr temp_style_ = vgui_style::new_style(0.0, 1.0, 0.0, 2.0, 2.0);
        neighbor_style_->rgba[0] =0;
        neighbor_style_->rgba[1] =1;
        neighbor_style_->rgba[2] =0;

        //neighbor_style_->apply_all();
        dcs_.push_back(tableau_->add_edgel_curve(dc,temp_style_));

        //: T_n C_1 - T_1 T_2 T_3 C_1
        match_data_sptr m1=new match_data;
        double cost1=curveMatch(Tcurves[Tcurves.size()-1]->desc->curve_->pointarray(),
            curr_curve_->desc->curve_->pointarray(),
            m1,e_,1,1,1);
        vcl_vector<vgl_point_2d<double> > Tc1n_1;
        dbctrk_algs::compute_transformation(Tcurves[Tcurves.size()-1]->desc->curve_->pointarray(),
            Tc1n_1,
            m1->R_,
            m1->Tbar,
            m1->scale_);

        vdgl_digital_curve_sptr dc1=dbctrk_algs::create_digital_curves(Tc1n_1);
        match_data_sptr m2=new match_data;
        vcl_reverse(Tcurves[Tcurves.size()-1]->desc->curve_->pointarray().begin(),Tcurves[Tcurves.size()-1]->desc->curve_->pointarray().end());
        // reverse one of the curves
        double cost2=curveMatch(Tcurves[Tcurves.size()-1]->desc->curve_->pointarray(),
            curr_curve_->desc->curve_->pointarray(),
            m2,e_,1,1,1);
        vcl_vector<vgl_point_2d<double> > Tc1n_2;
        dbctrk_algs::compute_transformation(Tcurves[Tcurves.size()-1]->desc->curve_->pointarray(),
            Tc1n_2,
            m2->R_,
            m2->Tbar,
            m2->scale_);
        vcl_reverse(Tcurves[Tcurves.size()-1]->desc->curve_->pointarray().begin(),Tcurves[Tcurves.size()-1]->desc->curve_->pointarray().end());
        vdgl_digital_curve_sptr dc2=dbctrk_algs::create_digital_curves(Tc1n_2);



        neighbor_style_->rgba[0] =1 ;
        neighbor_style_->rgba[1] =0;
        neighbor_style_->rgba[2] =0;

        //neighbor_style_->apply_all();
        if(cost1<cost2)
        {
            dcs_.push_back(tableau_->add_edgel_curve(dc1,neighbor_style_));
            vgui::out<<"Tx= "<<m1->Tbar(0,0)<<" Ty= "<<m1->Tbar(1,0)<<" scale= "<<m1->scale_<<"\n";
        }

        else
        {
            dcs_.push_back(tableau_->add_edgel_curve(dc2,neighbor_style_));
            vgui::out<<"Tx= "<<m2->Tbar(0,0)<<" Ty= "<<m2->Tbar(1,0)<<" scale= "<<m2->scale_<<"\n";
        }


        bvis1_manager::instance()->post_redraw();



    }


    int frame = view->frame();
    // draws neighbors in the transformation domain of a curve 
    if( e.type == vgui_DRAW_OVERLAY){


        if(!curr_curve_)
            return false;


        dbctrk_storage_sptr p;
        p.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("dbctrk",frame));
        vcl_vector<dbctrk_tracker_curve_sptr> tc;
        p->get_tracked_curves(tc);

        //if(curr_curve_->frame_number==frame)
        //{
        for(unsigned int i=0;i<tc.size();i++)
        {
            if(tc[i]->frame_number==frame)
            {
                dbctrk_soview2D curve(tc[i]);
                neighbor_style_->rgba[0] =0.92f;
                neighbor_style_->rgba[1] =0.92f;
                neighbor_style_->rgba[2] =0.92f;

                neighbor_style_->apply_all();  
                curve.draw();
            }
        }
        //}
        if(curr_curve_->get_best_match_prev().ptr() )
        {
            if(curr_curve_->frame_number==frame)
            {
                dbctrk_tracker_curve_sptr prev_curve_=curr_curve_->get_best_match_prev()->match_curve_set[0];

                vcl_vector<vgl_point_2d<double> > transformed_prev_curve;
                //: draw the alignment
                            vcl_map<int,int>::iterator iter;
            vcl_vector<vsol_spatial_object_2d_sptr > lines;
            vcl_vector<vsol_spatial_object_2d_sptr > Spoints;
            int cnt=0;
            for(iter=curr_curve_->get_best_match_prev()->mapping_.begin();iter!=curr_curve_->get_best_match_prev()->mapping_.end();iter++)
            {
                vsol_point_2d_sptr p1=new vsol_point_2d(prev_curve_->desc->curve_->point((*iter).first));
                vsol_point_2d_sptr p2=new vsol_point_2d(curr_curve_->desc->curve_->point((*iter).second));
                

                //vsol_point_2d_sptr p2_offsetted=new vsol_point_2d(p2->x()+40,p2->y()+40);
                if(cnt==0)
                {
                    Spoints.push_back(p1->cast_to_spatial_object());
                    Spoints.push_back(p2->cast_to_spatial_object());
                }
                vsol_line_2d_sptr l= new vsol_line_2d(p1,p2);

                vgui_style_sptr sty=vgui_style::new_style(1.0,1.0,1.0,2.0,3.0);
                lines.push_back((vsol_spatial_object_2d *) l.ptr());
                
                cnt++;
            } 
            tableau_->add_spatial_objects(lines);
  
                dbctrk_algs::compute_transformation(curr_curve_->get_best_match_prev()->match_curve_set[0]->desc->curve_->pointarray(),
                    transformed_prev_curve,
                    curr_curve_->get_best_match_prev()->R_,
                    curr_curve_->get_best_match_prev()->Tbar,
                    curr_curve_->get_best_match_prev()->scale_);

                vdgl_digital_curve_sptr dc=dbctrk_algs::create_digital_curves(transformed_prev_curve);

                vcl_vector<vgl_point_2d<double> > temp_curve = prev_curve_->desc->curve_->pointarray();
                
                vdgl_digital_curve_sptr prev_dc=dbctrk_algs::create_digital_curves(temp_curve);

                bgui_vsol_soview2D_edgel_curve pcurve(dc);
                bgui_vsol_soview2D_edgel_curve prev_curve_to_draw(prev_dc);
                neighbor_style_->rgba[0] =1  ;
                neighbor_style_->rgba[1] =0;
                neighbor_style_->rgba[2] =0;

                neighbor_style_->apply_all();
                pcurve.draw();
                neighbor_style_->rgba[0] =1  ;
                neighbor_style_->rgba[1] =0;
                neighbor_style_->rgba[2] =1;

                neighbor_style_->apply_all();

                prev_curve_to_draw.draw();
            }
            if(curr_curve_->get_best_match_prev()->match_curve_set[0]->frame_number==frame)
            {
                dbctrk_soview2D curve(curr_curve_->get_best_match_prev()->match_curve_set[0]);
                neighbor_style_->rgba[0] =1  ;
                neighbor_style_->rgba[1] =0;
                neighbor_style_->rgba[2] =0;

                neighbor_style_->apply_all();  
                curve.draw();
            }
        }
        else
        {
            //vgui::out<<"Prev match does not exist ";
        }
        //if(curr_curve_->get_best_match_next().ptr() )
        //{

        //    if(curr_curve_->frame_number==frame)
        //    {
        //        vcl_vector<vgl_point_2d<double> > transformed_next_curve;
        //        dbctrk_algs::compute_transformation_next(curr_curve_->get_best_match_next()->match_curve_set[0]->desc->curve_->pointarray(),
        //            transformed_next_curve,
        //            curr_curve_->get_best_match_next()->R_,
        //            curr_curve_->get_best_match_next()->Tbar,
        //            curr_curve_->get_best_match_next()->scale_);

        //        vdgl_digital_curve_sptr dc=dbctrk_algs::create_digital_curves(transformed_next_curve);

        //        bgui_vsol_soview2D_edgel_curve pcurve(dc);
        //        neighbor_style_->rgba[0] =0  ;
        //        neighbor_style_->rgba[1] =1;
        //        neighbor_style_->rgba[2] =0;

        //        neighbor_style_->apply_all();
        //        pcurve.draw();
        //    }
        //    if(curr_curve_->get_best_match_next()->match_curve_set[0]->frame_number==frame)
        //    {
        //        dbctrk_soview2D curve(curr_curve_->get_best_match_next()->match_curve_set[0]);
        //        neighbor_style_->rgba[0] =0  ;
        //        neighbor_style_->rgba[1] =1;
        //        neighbor_style_->rgba[2] =0;

        //        neighbor_style_->apply_all();  
        //        curve.draw();
        //    }

        //}
        //else
        //{
        //    //vgui::out<<"Next match does not exist ";
        //}

    }

    return false;
}



//: Return the name of this tool
vcl_string 
dbctrk_transform_tool::name() const

{

    return "Transform Best match";

}


