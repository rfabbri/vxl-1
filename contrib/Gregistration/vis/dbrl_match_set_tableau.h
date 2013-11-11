#ifndef _dbrl_match_set_tableau_
#define _dbrl_match_set_tableau_
#include <georegister/dbrl_match_set_sptr.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_popup_params.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_tableau.h>
#include <vgl/vgl_line_segment_2d.h>
#include <georegister/dbrl_feature_point.h>
#include <georegister/dbrl_feature_point_tangent.h>
#include <georegister/dbrl_feature_point_tangent_curvature_groupings.h>
#include <georegister/dbrl_feature_point_tangent_curvature.h>
#include <vgui/vgui_event_condition.h>

#include "dbrl_match_set_tableau_sptr.h"
class dbrl_match_set_tableau: public vgui_tableau 
    {
    public:    
        dbrl_match_set_tableau(){};
        dbrl_match_set_tableau(const dbrl_match_set_sptr& dms);
        ~dbrl_match_set_tableau();

        //: Returns the type of this tableau ('dbrl_match_set_tableau').
        vcl_string type_name() const;
        virtual bool handle( vgui_event const& e );
        //: Generate the popup menu
        void get_popup(const vgui_popup_params& params, vgui_menu &menu);

        void draw_correspondence();
        void draw_superimpose();
        virtual bool draw();




    protected:
        void set_changing_colors(int id,float &r,float &g, float &b);

        dbrl_match_set_sptr dms_;
        bool display_correspondence_;
        bool display_superimpose_;
        bool correspondence_inspector_;
        bool point_inspector_;
        bool show_velocity_flow_;
        vcl_vector<vgl_line_segment_2d<double> > clines;
        void draw_a_feature(dbrl_feature_sptr f);
        void draw_a_correspondence(dbrl_feature_sptr f1,dbrl_feature_sptr f2);
        void draw_a_point(dbrl_feature_point *pt);
        void draw_a_point_tangent(dbrl_feature_point_tangent *pt);
        void draw_a_point_tangent_curvature(dbrl_feature_point_tangent_curvature *pt);
        void draw_a_point_tangent_curvature_groupings(dbrl_feature_point_tangent_curvature_groupings *pt,
                                                      bool flag,vcl_vector<dbrl_feature_sptr> map);
void draw_flow();

        int minindex;

double dist_from_feature(dbrl_feature_sptr f1,float x, float y);

          vgui_event_condition gesture0_;

    };

//: Creates a smart-pointer to a dbrl_match_set_tableau.
struct dbrl_match_set_tableau_new
    : public dbrl_match_set_tableau_sptr
    {
    //:  Constructor - creates a tableau displaying the given distribution image.
    dbrl_match_set_tableau_new( const dbrl_match_set_sptr& dms  )
        : dbrl_match_set_tableau_sptr( new dbrl_match_set_tableau(dms) )
        { }
    };

#endif
