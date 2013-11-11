// This is breye1/dbctrk/vis/dbctrk_matching_tools.cxx
//:
// \file

#include "dbctrk/vis/dbctrk_matching_tool.h"
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>
#include <dbctrk/vis/dbctrk_displayer.h>
#include <vgui/vgui_projection_inspector.h>
#include <vcl_iostream.h>
#include <vgui/vgui.h> 
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <dbctrk/pro/dbctrk_storage.h> 
#include <dbctrk/dbctrk_tracker_curve_sptr.h>
#include <dbctrk/dbctrk_utils.h>
#include <dbctrk/dbctrk_algs.h>
#include <dbctrk/dbctrk_curve_matching.h>
#include <vidpro1/vidpro1_repository_sptr.h>
#include <vidpro1/vidpro1_repository.h>
#include <dbctrk/pro/dbctrk_storage_sptr.h>




//: Constructor - protected
dbctrk_matching_tool::dbctrk_matching_tool()
:  tableau_(NULL), storage_(NULL)
{

}


//: Set the tableau to work with
bool
dbctrk_matching_tool::set_tableau ( const vgui_tableau_sptr& tableau )
{
    if( !this->set_storage(bvis1_manager::instance()->storage_from_tableau(tableau)) )
        return false;

    if( tableau.ptr() != NULL && tableau->type_name() == "bgui_vsol2D_tableau" ){
        tableau_.vertical_cast(tableau);
        return true;
    }

    tableau_ = NULL;
    return false;
}


//: Set the storage class for the active tableau
bool
dbctrk_matching_tool::set_storage ( const bpro1_storage_sptr& storage )
{
    if (!storage.ptr())
        return false;
    //make sure its a bmrf storage class
    if (storage->type() == "dbctrk"){
        storage_.vertical_cast(storage);
        return true;
    }
    return false;
}

//----------------------dbctrk_minspector_tool---------------------------


//: Constructor
dbctrk_minspector_tool::dbctrk_minspector_tool()
: object_(NULL)
{
    neighbor_style_ = vgui_style::new_style(1.0, 0.0, 0.0, 3.0, 3.0);
    numtopmatches=6;
    threshold=1.0;
    thresh=false;
    topmatches=true;

}


//: Destructor
dbctrk_minspector_tool::~dbctrk_minspector_tool()
{
}


//: Return the name of this tool
vcl_string
dbctrk_minspector_tool::name() const
{
    return "Match Inspector"; 
}


//: Handle events
bool
dbctrk_minspector_tool::handle( const vgui_event & e, 
                               const bvis1_view_tableau_sptr& view )
{


    if( !tableau_.ptr() )
        return false;

    if( e.type == vgui_KEY_PRESS && e.key == 'q' && vgui_SHIFT){

        int framen=bvis1_manager::instance()->repository()->current_frame();
        vcl_cout<<"\n current frame no is "<<framen;
    }
    if( e.type == vgui_KEY_PRESS && e.key == 't'){
        vgui_dialog topmatches_dlg("Threshold or no of top matches");
        topmatches_dlg.checkbox("Threshold(y/n)",thresh);
        topmatches_dlg.field("Threshold",threshold);
        topmatches_dlg.field("No of Top matches",numtopmatches);
        if(!topmatches_dlg.ask())
            return true;
        if(thresh)
            topmatches=false;
        else
            topmatches=true;
    }


    if( e.type == vgui_MOTION){
        tableau_->motion(e.wx, e.wy);
        vgui_soview2D* curr_obj =  (vgui_soview2D*)tableau_->get_highlighted_soview();

        object_ = (dbctrk_soview2D*)curr_obj;
        curr_curve_ = NULL;
        if( curr_obj && curr_obj->type_name() == "dbctrk_soview2D"){   
            curr_curve_ = object_->dbctrk_sptr();
        }
        //vgui::out<<"\n";
        bvis1_manager::instance()->post_overlay_redraw();

    }


    if( e.type == vgui_KEY_PRESS && e.key == 'v' && vgui_SHIFT){
        int frame=bvis1_manager::instance()->repository()->current_frame();
        if(frame>1)
        {
            dbctrk_storage_sptr p;
            p.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("dbctrk",frame));
            vcl_vector<dbctrk_tracker_curve_sptr> tc;
            p->get_tracked_curves(tc);

            p.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("dbctrk",frame-1));
            vcl_vector<dbctrk_tracker_curve_sptr> tcprev;
            p->get_tracked_curves(tcprev);

            vgui_dialog ndlg("No of levels");
            static int n=3;
            ndlg.field("No of levels",n);
            if(!ndlg.ask())
                return false;

            vcl_map<int,vcl_vector<vcl_pair<dbctrk_tracker_curve_sptr, dbctrk_tracker_curve_sptr> > > transitives_;
            for(unsigned int i=0;i<tc.size();i++)
            {
                tc[i]->isprevclosure_=-100;
                tc[i]->isnextclosure_=-100;

            }
            for(unsigned int i=0;i<tcprev.size();i++)
            {
                tcprev[i]->isprevclosure_=-100;
                tcprev[i]->isnextclosure_=-100;

            }
            dbctrk_curve_matching::best_topn_matches(n,
                &tc,
                &tcprev,
                transitives_);

            for(unsigned int i=0;i<tc.size();i++)
            {
                if(tc[i]->frame_number==frame)
                {

                    vcl_vector<vgl_point_2d<double> > temp=tc[i]->desc->curve_->pointarray();
                    vdgl_digital_curve_sptr dc=dbctrk_algs::create_digital_curves(temp);

                    neighbor_style_->rgba[0] =0.92f;
                    neighbor_style_->rgba[1] =0.92f;
                    neighbor_style_->rgba[2] =0.92f;
                    dcs_.push_back(tableau_->add_edgel_curve(dc,neighbor_style_));
                }

            }
            vcl_map<int,vcl_vector<vcl_pair<dbctrk_tracker_curve_sptr, dbctrk_tracker_curve_sptr> > >::iterator iter;

            int cnt=0;
            for(iter=transitives_.begin();iter!=transitives_.end();iter++,cnt++)
            {
                for(unsigned int i=0;i<(*iter).second.size();i++)
                {
                    float r,g,b;
                    utils::set_rank_colors(cnt,r,g,b);
                    vcl_vector<vgl_point_2d<double> > temp=(*iter).second[i].first->desc->curve_->pointarray();
                    vdgl_digital_curve_sptr dc=dbctrk_algs::create_digital_curves(temp);
                    neighbor_style_ = vgui_style::new_style(r,g,b,2.0,2.0);
                    dcs_.push_back(tableau_->add_edgel_curve(dc,neighbor_style_));
                }
            }

            bvis1_manager::instance()->post_redraw();

        }
    }
    if( e.type == vgui_KEY_PRESS && e.key == 'r' && vgui_SHIFT){
        vcl_vector<vgui_soview*> all_objects;
        all_objects = tableau_->get_selected_soviews();
        if(all_objects.size()==2)
        {
            dbctrk_tracker_curve_sptr c1=((dbctrk_soview2D*)all_objects[0])->dbctrk_sptr();
            dbctrk_tracker_curve_sptr c2=((dbctrk_soview2D*)all_objects[1])->dbctrk_sptr();

            if(c1->get_best_match_prev().ptr() && c2->get_best_match_prev().ptr())
            {
                vcl_map<double,int>::iterator iter;
                for(iter=c1->neighbors_.begin();iter!=c1->neighbors_.end();iter++)
                {
                    if((*iter).second==c2->get_id())
                        vcl_cout<<"\n the distance between curves "<<c1->get_id() <<" and "<<c2->get_id()
                        <<" is "<<(*iter).first;
                }
            }
            else
                vcl_cout<<"\n one or both of the curves do not have match";
        }
        else
        {
            vgui::out<<"requires only two curves \n";
        }
        tableau_->deselect_all();

    }

    if( e.type == vgui_KEY_PRESS && e.key == 'u'){
        for(unsigned int i=0;i<dcs_.size();i++)
        {
            tableau_->remove((vgui_soview*)dcs_[i]);
        }
        dcs_.clear();
    }

    float ix, iy;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

    int frame = view->frame();
    if( e.type == vgui_DRAW_OVERLAY){
        if(!curr_curve_)
            return false;

        dbctrk_storage_sptr p;
        p.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("dbctrk",frame));
        vcl_vector<dbctrk_tracker_curve_sptr> tc;
        p->get_tracked_curves(tc);

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

        if(curr_curve_->get_best_match_prev().ptr())
        {

            dbctrk_soview2D tempp(curr_curve_->get_best_match_prev()->match_curve_set[0]);


            vcl_vector<match_data_sptr>::iterator iter;
            int cnt=0;
            if(numtopmatches>6)
                numtopmatches=6;

            for(iter=curr_curve_->prev_.begin();iter!=curr_curve_->prev_.end() && cnt<numtopmatches;iter++,cnt++)
            {
                if((*iter)->match_curve_set[0]->frame_number==frame)
                {  
                    float r,g,b;
                    //utils::set_changing_colors(cnt,r,g,b);
                    utils::set_rank_colors(cnt,r,g,b);
                    neighbor_style_->rgba[0] =r  ;
                    neighbor_style_->rgba[1] =g  ;
                    neighbor_style_->rgba[2] =b   ;
                    neighbor_style_->line_width=4.0;
                    neighbor_style_->apply_all();  
                    dbctrk_soview2D((*iter)->match_curve_set[0]).draw();
                    //vgui::out<<cnt<<"=>"<<(*iter)->cost_<<"::";
                }
            }

            if(tempp.dbctrk_sptr()->frame_number==frame)
            {
                neighbor_style_->rgba[0] =0.0  ;
                neighbor_style_->rgba[0] =0.0  ;
                neighbor_style_->rgba[2] = 1.0;
                neighbor_style_->line_width=2.0;
                neighbor_style_->apply_all();  
                tempp.draw();
            }
        }

        if(curr_curve_->get_best_match_next().ptr())
        {

            dbctrk_soview2D tempp(curr_curve_->get_best_match_next()->match_curve_set[0]);


            vcl_vector<match_data_sptr>::iterator iter;
            int cnt=0;
            if(numtopmatches>6)
                numtopmatches=6;

            for(iter=curr_curve_->next_.begin();iter!=curr_curve_->next_.end() && cnt<numtopmatches;iter++,cnt++)
            {
                vcl_cout<<"\n"<<(*iter)->match_curve_set[0]->frame_number<<"....";
                if((*iter)->match_curve_set[0]->frame_number==frame)
                {  
                    float r,g,b;
                    utils::set_rank_colors(cnt,r,g,b);
                    neighbor_style_->rgba[0] =r  ;
                    neighbor_style_->rgba[1] =g  ;
                    neighbor_style_->rgba[2] =b   ;
                    neighbor_style_->line_width=4.0;
                    neighbor_style_->apply_all();  
                    dbctrk_soview2D((*iter)->match_curve_set[0]).draw();
                    //vgui::out<<cnt<<"=>"<<(*iter)->cost_<<"::";
                }
            }

            if(tempp.dbctrk_sptr()->frame_number==frame)
            {
                neighbor_style_->rgba[0] =0.0  ;
                neighbor_style_->rgba[0] =0.0  ;
                neighbor_style_->rgba[2] = 1.0;
                neighbor_style_->line_width=2.0;
                neighbor_style_->apply_all();  
                tempp.draw();
            }
        }

        vcl_vector<match_data_sptr>::iterator iter;
        vgui::out.precision(2);
        int cnt=0;
        if(numtopmatches>6)
            numtopmatches=6;

        if(curr_curve_->prev_.size()>0)
            vgui::out<<"{";
        for(iter=curr_curve_->prev_.begin();iter!=curr_curve_->prev_.end()&& cnt<numtopmatches;iter++,cnt++)
        {
            vgui::out<<(*iter)->cost_<<",";  
        }
        if(curr_curve_->prev_.size()>0)
            vgui::out<<"}";

        cnt=0;
        if(curr_curve_->next_.size()>0)
            vgui::out<<"{";
        for(iter=curr_curve_->next_.begin();iter!=curr_curve_->next_.end()&& cnt<numtopmatches;iter++,cnt++)
        {
            vgui::out<<(*iter)->cost_<<","; 
        }
        if(curr_curve_->next_.size()>0)
            vgui::out<<"}";

        vgui::out<<"\n";

    }


    // to show transform also
    if( e.type == vgui_KEY_PRESS && e.key == 'l' && vgui_SHIFT){

        vcl_vector<vgui_soview*> all_objects;
        all_objects = tableau_->get_selected_soviews();

        if(all_objects.size()<=0)
            return false;

        curr_curve_=((dbctrk_soview2D*)all_objects[0])->dbctrk_sptr();


        if(curr_curve_->get_best_match_prev().ptr() )
        {
            if(curr_curve_->frame_number==frame)
            {
                vcl_vector<vgl_point_2d<double> > transformed_prev_curve;
                dbctrk_algs::compute_transformation(curr_curve_->get_best_match_prev()->match_curve_set[0]->desc->curve_->pointarray(),
                    transformed_prev_curve,
                    curr_curve_->get_best_match_prev()->R_,
                    curr_curve_->get_best_match_prev()->Tbar,
                    curr_curve_->get_best_match_prev()->scale_);

                vdgl_digital_curve_sptr dc=dbctrk_algs::create_digital_curves(transformed_prev_curve);

                neighbor_style_->rgba[0] =1  ;
                neighbor_style_->rgba[1] =0;
                neighbor_style_->rgba[2] =0;
                dcs_.push_back(tableau_->add_edgel_curve(dc,neighbor_style_));
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
        if(curr_curve_->get_best_match_next().ptr() )
        {

            if(curr_curve_->frame_number==frame)
            {
                vcl_vector<vgl_point_2d<double> > transformed_next_curve;
                dbctrk_algs::compute_transformation_next(curr_curve_->get_best_match_next()->match_curve_set[0]->desc->curve_->pointarray(),
                    transformed_next_curve,
                    curr_curve_->get_best_match_next()->R_,
                    curr_curve_->get_best_match_next()->Tbar,
                    curr_curve_->get_best_match_next()->scale_);

                vdgl_digital_curve_sptr dc=dbctrk_algs::create_digital_curves(transformed_next_curve);

                neighbor_style_->rgba[0] =0;
                neighbor_style_->rgba[1] =1;
                neighbor_style_->rgba[2] =0;
                dcs_.push_back(tableau_->add_edgel_curve(dc,neighbor_style_));
            }
            if(curr_curve_->get_best_match_next()->match_curve_set[0]->frame_number==frame)
            {
                dbctrk_soview2D curve(curr_curve_->get_best_match_next()->match_curve_set[0]);
                neighbor_style_->rgba[0] =0  ;
                neighbor_style_->rgba[1] =1;
                neighbor_style_->rgba[2] =0;

                neighbor_style_->apply_all();  
                curve.draw();
            }

        }
        else
        {
            //vgui::out<<"Next match does not exist ";
        }
        bvis1_manager::instance()->post_redraw();
        tableau_->deselect_all();
    }
    return false;


}

