#include <dbctrk/vis/dbctrk_alignment_viewer_tool.h>
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>
#include <dbctrk/vis/dbctrk_soview2d.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <dbctrk/pro/dbctrk_storage_sptr.h>
#include <vidpro1/vidpro1_repository.h>
#include <dbctrk/dbctrk_algs.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vgui/vgui.h>
#include <dbctrk/dbctrk_curve_clustering.h>
#include <vcl_algorithm.h>
#include <dbctrk/dbctrk_utils.h> 
#include <dbctrk/dbctrk_curveMatch.h> 
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_menu.h>


dbctrk_alignment_viewer_tool::dbctrk_alignment_viewer_tool()
{ 
    neighbor_style_ = vgui_style::new_style(1.0, 0.0, 0.0, 3.0, 3.0);

}

dbctrk_alignment_viewer_tool::~dbctrk_alignment_viewer_tool()
{
}

bool
dbctrk_alignment_viewer_tool::set_tableau( const vgui_tableau_sptr& tableau )
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
dbctrk_alignment_viewer_tool::set_storage ( const bpro1_storage_sptr& storage)
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
dbctrk_alignment_viewer_tool::tableau()
{
    return tableau_;
}

dbctrk_storage_sptr
dbctrk_alignment_viewer_tool::storage()
{
    dbctrk_storage_sptr dbctrk_storage;
    dbctrk_storage.vertical_cast(storage_);
    return dbctrk_storage;
}

bool
dbctrk_alignment_viewer_tool::handle( const vgui_event & e, 
                                     const bvis1_view_tableau_sptr& view )

{
    float ix, iy;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

    //: tool to plot a intensity profile of matched curves using matlab
    if (e.type == vgui_KEY_PRESS && e.key == 's' ) {
        vcl_vector< vcl_map<vcl_string, vgui_tableau_sptr> > seq;//=bvis1_manager::instance()->tableau_sequence();
        vcl_map<vcl_string, vgui_tableau_sptr>::iterator iter;
        vcl_vector<vgui_soview*>  all_objects;
        vcl_vector<vgui_soview*>  selected_objects;
#if 0
        //this variable is not used in the code.  PLEASE FIX!  -MM
        vcl_map<int,vcl_vector<dbctrk_soview2D* > >::iterator curveiter;
#endif
        curr_tableau_=tableau_;
        //: getting the curves selected across all the tableau's
        for(unsigned int j=0;j<seq.size();j++)
        {    
            for(iter=seq[j].begin();iter!=seq[j].end();iter++)
            {
                if(set_tableau((*iter).second))
                { 
                    all_objects=tableau_->get_selected_soviews();
                    for(unsigned int i=0;i<all_objects.size();i++)
                    {
                        if(((dbctrk_soview2D*)all_objects[i])->type_name()=="dbctrk_soview2D" && tableau_->is_selected(all_objects[i]->get_id()))
                        {   
                            tableau_->deselect(all_objects[i]->get_id());
                            selected_objects.push_back(all_objects[i]);
                        }
                    }
                }
            }
        }

        vgui_dialog m_dlg("Match curves");

        static bool reverse=false;
        static int alpha=0;
        static int beta=0;
        static int gamma=0;
        static double eta=0;
        static double zeta=0;
        static double theta=0;
        static bool onesided=false;
        static int Lchoice=0;

        m_dlg.checkbox("Reverse",reverse);
        m_dlg.checkbox("OneSided",onesided);
        m_dlg.field("alpha(length)",alpha);
        m_dlg.field("beta(curvature)",beta);
        m_dlg.field("gamma(epipole)",gamma);
        m_dlg.field("eta(I)",eta);
        m_dlg.field("zeta(H)",zeta);
        m_dlg.field("theta(S)",theta);
        m_dlg.choice("Lp(intensity)","p=1","p=2","p=3",Lchoice);
        if(!m_dlg.ask())
            return false;


        vnl_matrix <double> R;
        vnl_matrix <double> Tbar;              
        tableau_=curr_tableau_;
#if 0
        //this variable is not used in the code.  PLEASE FIX!  -MM
        double scale;
#endif
        bvsolalignment=vidpro1_vsol2D_storage_new();
        bvsolalignment->set_name("Alignment");
        bvis1_manager::instance()->repository()->store_data(bvsolalignment);
        mapping.clear();
        if(selected_objects.size()==2)
        {
            c1=((dbctrk_soview2D*)selected_objects[0])->dbctrk_sptr();
            c2=((dbctrk_soview2D*)selected_objects[1])->dbctrk_sptr();

            //this variable is not used in the code.  PLEASE FIX!  -MM
//            double cost=IcurveMatch(c1,c2,mapping,R,Tbar,scale,alpha,beta,eta,gamma,zeta,theta,onesided,reverse,Lchoice);
            int off_set=40;

            vdgl_digital_curve_sptr dc1=dbctrk_algs::create_digital_curves(c1->desc->curve_);
            vdgl_digital_curve_sptr dc2=dbctrk_algs::create_digital_curves(c2->desc->curve_);
            vcl_vector<vsol_point_2d_sptr> samples;
            for(int i=0;i<c2->desc->curve_->numPoints();i++)
            {
                samples.push_back(new vsol_point_2d(c2->desc->curve_->x(i)+40,
                    c2->desc->curve_->y(i)+40));
            }
            vsol_digital_curve_2d_sptr vc2= new vsol_digital_curve_2d(samples);
            bgui_vsol_soview2D_edgel_curve pcurve1(dc1);
            bgui_vsol_soview2D_digital_curve pcurve2(vc2);

            neighbor_style_->rgba[0] =1 ;
            neighbor_style_->rgba[1] =1;
            neighbor_style_->rgba[2] =0;
            neighbor_style_->apply_all();

            bvsolalignment->add_object(pcurve1.sptr()->cast_to_spatial_object());
            bvsolalignment->add_object(pcurve2.sptr()->cast_to_spatial_object());
            vcl_map<int,int>::iterator iter;
            vcl_vector<vsol_spatial_object_2d_sptr > lines;
            vcl_vector<vsol_spatial_object_2d_sptr > Spoints;
            int cnt=0;
            for(iter=mapping.begin();iter!=mapping.end();iter++)
            {
                vsol_point_2d_sptr p1=new vsol_point_2d(c1->desc->curve_->point((*iter).first));
                vsol_point_2d_sptr p2=new vsol_point_2d(c2->desc->curve_->point((*iter).second));
                vcl_cout<<(*p1)<<"\t"<<(*p2);

                vsol_point_2d_sptr p2_offsetted=new vsol_point_2d(p2->x()+40,p2->y()+40);
                if(cnt==0)
                {
                    Spoints.push_back(p1->cast_to_spatial_object());
                    Spoints.push_back(p2_offsetted->cast_to_spatial_object());
                }
                vsol_line_2d_sptr l= new vsol_line_2d(p1,p2_offsetted);
                vgui_style_sptr sty=vgui_style::new_style(1.0,1.0,1.0,2.0,3.0);
                lines.push_back((vsol_spatial_object_2d *) l.ptr());
                cnt++;
            } 


            neighbor_style_->rgba[0] =1;
            neighbor_style_->rgba[1] =0;
            neighbor_style_->rgba[2] =0;
            neighbor_style_->point_size=4.0;
            neighbor_style_->apply_all();

            bvsolalignment->add_objects(Spoints);
            bvsolalignment->add_objects(lines,"lines");
            bvis1_manager::instance()->add_to_display(bvsolalignment);
        }
        bvis1_manager::instance()->display_current_frame();
        tableau_->deselect_all();
        return true;
    }
    return false;
}


vgui_menu 
dbctrk_alignment_viewer_tool::create_menus()
{
    vgui_menu file;
    file.add("Quit",quit);


    return file;
}
void
dbctrk_alignment_viewer_tool::quit()
{
    vgui_dialog quit_dl("Quit");
    quit_dl.message(" ");
    quit_dl.message("   Are you sure you want to quit viewer?   ");
    quit_dl.message(" ");

    if (quit_dl.ask())
        vgui::quit();
}
//: Return the name of this tool
vcl_string 
dbctrk_alignment_viewer_tool::name() const
{

    return "Alignment viewer";

}






