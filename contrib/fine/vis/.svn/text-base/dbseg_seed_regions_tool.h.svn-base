// This is contrib/fine/dbseg_seed_regions_tool.h
#ifndef dbseg_seed_regions_tool_h
#define dbseg_seed_regions_tool_h
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


/*#include "dbseg_seg_tableau_sptr.h"
#include <structure/dbseg_seg_storage.h>
#include "dbseg_seg_tableau.h"
#include <structure/seg_object.h>*/

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>

#include <vidpro1/vidpro1_repository_sptr.h>
#include <vidpro1/vidpro1_repository.h>

#include <vgui/vgui_dialog.h>

#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_image_tableau.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vgl/vgl_polygon.h>
using namespace std;
#include <vector>
#include <list>

//: This tool allows higher level regions to be created
class dbseg_seed_regions_tool : public bvis1_tool 
{
public:
    //: Constructor
    dbseg_seed_regions_tool();

    //: Destructor
    ~dbseg_seed_regions_tool() {}

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
    void create_seed();

    //process the drawn polygon by selecting the surrounded regions
    void save_seeds();

protected:

    vgui_image_tableau_sptr tab_;
    //seg_object* seg_object_;

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

    vector<list<pair<double, double>>> seeds;
  
    

};


#endif // dbseg_seed_regions_tool_h


