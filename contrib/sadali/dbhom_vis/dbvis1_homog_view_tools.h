#ifndef dbvis1_homog_view_tools_header
#define dbvis1_homog_view_tools_header


#include <bvis1/tool/bvis1_vsol2D_tools.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_soview2D.h>
#include <bgui/bgui_vsol_soview2D.h>

#include <vsol/vsol_point_2d_sptr.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>





#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <bgui/bgui_vsol2D_tableau_sptr.h>
#include <bgui/bgui_vsol2D_tableau.h>

#include <bprt/bprt_homog_interface.h>

//A tool for finding homography between two sets of point features
//EITHER
// select four points on each image to calculate the homography for these 4 pts
//OR
// select a single feature point for each image to randomly sample among possible
//correspondences between all pts in both images. The possible correspondences are
//found by crosscorrelating 7x7 region around the feature point and its corresponding 
//point in the other image and choosing those correspondences for which this crosscorrelation
// is higher than a threshold
//OR 
//Using SHIFT and left mouse button draw a polygon around the region including those points
//which you want selected.When you're done click middle mouse button. Again homographies are 
//calculated using possible correspondences from these points
//To select images click SHIFT and middle mouse button(middle =left+right) while the image is
// the active storage
// When all selecting is done click middle mouse button while the feature point storage is
// the active storage

class  dbvis1_homog_view_tool: public bvis1_vsol2D_tool {
public:
    //: Constructor
    dbvis1_homog_view_tool( const vgui_event_condition& lift     = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true)     ,
        const vgui_event_condition& end_sel  = vgui_event_condition(vgui_MIDDLE, vgui_MODIFIER_NULL, true),
        const vgui_event_condition& draw_box = vgui_event_condition(vgui_LEFT, vgui_SHIFT, true)         ,
        const vgui_event_condition& img_sel  = vgui_event_condition(vgui_MIDDLE, vgui_SHIFT, true)
        );
    //: Destructor
    virtual ~dbvis1_homog_view_tool();


    //: Returns the string name of this tool
    vcl_string name() const;

    //: Set the tableau to work with
    virtual bool set_tableau( const vgui_tableau_sptr& tableau );
    //void activate();
    void deactivate();

    //: handle events
    bool handle( const vgui_event & e,const bvis1_view_tableau_sptr& selector );

    void 
     get_popup( const vgui_popup_params& params, 
                                     vgui_menu &menu );

protected:
    vgui_event_condition gesture_lift_;
    vgui_event_condition gesture_end_sel_;
    vgui_event_condition  gesture_draw_box_;
    vgui_event_condition  gesture_img_sel_;
    bool end_sel_flag;


private:

    bool four_pts();
    bool Ransac_all_pts_in_image();
    bool Ransac_selected_pts_in_image();
    void putative_correspondences(vcl_vector<vsol_point_2d_sptr > & ,vcl_vector<vsol_point_2d_sptr > & , vil_image_resource_sptr& ,
        vil_image_resource_sptr& , vcl_vector< vsol_point_2d_sptr >&, vcl_vector< vsol_point_2d_sptr > & );
    double thresh_;
    bool active_;
    vgui_soview2D * object_;
    vgui_easy2D_tableau_sptr tableau_vsol;
    vgui_easy2D_tableau_sptr tableau_im;
    float last_x, last_y ;
    float left_cornerx_, left_cornery_;
    vcl_vector < vgui_tableau_sptr > tableaus_vsol_;
    vcl_vector < vgui_tableau_sptr > tableaus_im_;
    vcl_vector < vcl_vector < vsol_point_2d_sptr > > pts_;
    vidpro1_vsol2D_storage_sptr storage_,storage2_;
    vidpro1_vsol2D_storage_sptr linestorage_,linestorage2_,linestorage3_,linestorage4_;
    vcl_vector < bpro1_storage_sptr > images_;
    vcl_vector < vsol_point_2d_sptr > drawnpt;
    vcl_vector < vgl_point_2d<double> > drawnpt_vgl;
    vcl_vector <vcl_vector< vsol_line_2d_sptr > > epilines;

    
    vcl_vector<vgl_homg_point_2d<double> > epip;
    int num_pop;


    bool imgsel;
    bool use_correlation_;
    bool use_epipolar_const_;

};



#endif
