// This is contrib/fine/dbseg_seg_create_region_tool.h
#ifndef dbseg_seg_create_region_tool_h
#define dbseg_seg_create_region_tool_h
//:
//\file
//\brief Tool for creating a region in the hierarchical structure
//\      with 2 or more regions at depth level one as its children
//\author Eli fine
//\date 8/11/08
//
// \verbatim
//  Modifications                            
// \endverbatim

#include <bpro1/bpro1_storage_sptr.h>
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_tool.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>

#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_projection_inspector.h>

#include "dbseg_seg_tableau_sptr.h"
#include <structure/dbseg_seg_storage.h>
#include "dbseg_seg_tableau.h"
#include <structure/dbseg_seg_object.h>

//: This tool allows higher level regions to be created
class dbseg_seg_create_region_tool : public bvis1_tool 
{
public:
    //: Constructor
    dbseg_seg_create_region_tool();

    //: Destructor
    ~dbseg_seg_create_region_tool() {}

    //: Returns the string name of this tool
    vcl_string name() const;

    //: Handle events.
    bool handle( const vgui_event & e, const bvis1_view_tableau_sptr& view );

    //: Set the tableau associated with the current view
    virtual bool set_tableau ( const vgui_tableau_sptr& tableau );

    //clear things when it gets deactivated
    void deactivate();

    //clear the outlines when it gets activated
    void activate();

    //create the region
    void create_region();

    //process the drawn polygon by selecting the surrounded regions
    void select_regions();

protected:

    dbseg_seg_tableau_sptr seg_tab_;
    dbseg_seg_object<vxl_byte>* seg_object_;

    vgui_event_condition left_click;
    vgui_event_condition middle_click;
    vgui_event_condition home_key;
    vgui_event_condition shift_right_click;

private:

    //related to the polygon
    bool active;
    vcl_vector<vgl_point_2d<double> > point_list;
    double last_x;
    double last_y;
  
    //whether or not to display the line pointing to the discontinuous region
    bool dis;

    //coordinates related to discontinuous region
    double disx1, disx2, disy1, disy2;
  

};


#endif // dbseg_seg_create_region_tool_h


