#include <dbctrk/vis/dbctrk_write_info_tool.h>
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>
#include <dbctrk/vis/dbctrk_soview2d.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <dbctrk/dbctrk_algs.h>
#include <dbctrk/dbctrk_tracker_curve.h>
#include <dbctrk/dbctrk_utils.h>

dbctrk_write_info_tool::dbctrk_write_info_tool()
{

    gesture_info_ = vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);
}

dbctrk_write_info_tool::~dbctrk_write_info_tool()
{
}

bool
dbctrk_write_info_tool::set_tableau( const vgui_tableau_sptr& tableau )
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
dbctrk_write_info_tool::set_storage ( const bpro1_storage_sptr& storage)
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
dbctrk_write_info_tool::tableau()
{
    return tableau_;
}

dbctrk_storage_sptr
dbctrk_write_info_tool::storage()
{
    dbctrk_storage_sptr dbctrk_storage;
    dbctrk_storage.vertical_cast(storage_);
    return dbctrk_storage;
}

bool
dbctrk_write_info_tool::handle( const vgui_event & e, 
                               const bvis1_view_tableau_sptr& view )
{
    float ix, iy;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

    if ( gesture_info_(e) ) {

        vgui_dialog dt_dlg("Output file for writing clusters");
        static vcl_string file_name = "";
        static vcl_string ext = "*.*";
        dt_dlg.file("File:", ext, file_name);
        if( !dt_dlg.ask())
            return true;

        vcl_map<double,int>::iterator iter;
        vcl_map<int,double>::iterator siter;
        vcl_vector<dbctrk_tracker_curve_sptr> tc;
        storage_->get_tracked_curves(tc);
        vcl_ofstream ofile(file_name.c_str());
        for(unsigned int i=0;i<tc.size();i++)
        {
            if(tc[i]->get_best_match_prev().ptr() )
            {
                vcl_cout<<"\n "<<tc[i]->get_id();
                vcl_map<int,double> temp;    
                for(iter=tc[i]->neighbors_.begin();iter!=tc[i]->neighbors_.end();iter++)
                {
                    temp[(*iter).second]=(*iter).first;
                }
                for(siter=temp.begin();siter!=temp.end();siter++)
                {
                    ofile<<(*siter).second<<" ";
                }


                ofile<<"\n";
            }
        }
        ofile.close();


    }

    if ( e.type == vgui_KEY_PRESS && e.key == 'w' && vgui_SHIFT) {


        vgui_dialog dt_dlg("Output file for writing clusters");
        static vcl_string file_name = "";
        static vcl_string ext = "*.*";
        dt_dlg.file("File:", ext, file_name);
        if( !dt_dlg.ask())
            return true;

        vcl_map<double,int>::iterator iter;
        vcl_map<int,double>::iterator siter;
        vcl_vector<dbctrk_tracker_curve_sptr> tc;

        vcl_ofstream ofile(file_name.c_str());
        vcl_vector<vgui_soview*> all_objects;
        all_objects = tableau_->get_selected_soviews();


        if(all_objects.size()>0)
        {
            for(unsigned int i=0;i<all_objects.size();i++)
            {
                dbctrk_tracker_curve_sptr c1=((dbctrk_soview2D*)all_objects[i])->dbctrk_sptr();
                if(c1->get_best_match_prev().ptr())
                    tc.push_back(c1);
            }

            for(unsigned int i=0;i<tc.size();i++)
            {
                vcl_map<int,double> temp;
                for(iter=tc[i]->neighbors_.begin();iter!=tc[i]->neighbors_.end();iter++)
                {
                    temp[(*iter).second]=(*iter).first;
                }

                for(unsigned int j=0;j<tc.size();j++)
                {
                    siter=temp.find(tc[j]->get_id());
                    ofile<<(*siter).second<<" ";
                }
                ofile<<"\n";
            }

        }
        ofile.close();

        tableau_->deselect_all();
    }
    if (e.type == vgui_KEY_PRESS && e.key == 'p' && vgui_SHIFT ) {
        vgui_dialog dt_dlg("Load Ncut clusters");
        static vcl_string file_name = "";
        static vcl_string ext = "*.*";
        static int num_of_clusters=3;
        dt_dlg.file("File:", ext, file_name);
        dt_dlg.field("No of clusters", num_of_clusters);
        if( !dt_dlg.ask())
            return true;

        vcl_ifstream ifile(file_name.c_str());
        if(!ifile)
            return false;
        else
        {
            vcl_vector<dbctrk_tracker_curve_sptr> tc;
            storage_->get_tracked_curves(tc);

            double num;
            vcl_vector<vcl_pair<dbctrk_tracker_curve_sptr,int> > clusters;
            for(unsigned int i=0;i<tc.size();i++)
            {
                if(tc[i]->get_best_match_prev().ptr() )
                {
                    for(int k=0;k<num_of_clusters;k++)
                    {
                        ifile>>num;
                        if(num>0.5)
                        {
                            clusters.push_back(vcl_make_pair(tc[i],k));  
                        }
                    }
                }
            }

            /*
            vcl_vector<vcl_pair<dbctrk_tracker_curve_sptr,int> >::iterator iter;
            for(iter=clusters.begin();iter!=clusters.end();iter++)
            {
            vdgl_digital_curve_sptr dc1=dbctrk_algs::create_digital_curves((*iter).first->desc->points_);
            float r,g,b;
            utils::set_rank_colors((*iter).second,r,g,b);
            vgui_style_sptr pstyle= vgui_style::new_style(r,g,b,3.0,3.0);
            dcs_.push_back(tableau_->add_edgel_curve(dc1,pstyle));
            }
            */

        }

        bvis1_manager::instance()->post_redraw();
    }
    return false;
}



//: Return the name of this tool
vcl_string 
dbctrk_write_info_tool::name() const
{
    return "write dt graph";
}


