#ifndef histogrampick_manager_h_
#define histogrampick_manager_h_
#include <vil/vil_image_resource_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_viewer2D_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vgui/vgui_deck_tableau.h>
#include <bbas/bgui/bgui_picker_tableau_sptr.h>
#include <vgui/vgui_range_map_params.h>
#include <bbas/bsta/bsta_histogram.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_point_2d.h>


class vgui_soview2D_lineseg;
class vgui_soview2D_polygon;

class histogrampick_manager : public vgui_wrapper_tableau
{
  public:
    ~histogrampick_manager();
    static histogrampick_manager *instance();
    void quit();
    void save_histogram();
    void load_image();
    void create_temporary_tifs(const char* fname,const char* output_prefix);
    void load_image(const char* fname);
    void set_range();
    void init();
    void pick_polygon();
    void write_xml();
    void inside_vessel();
    void inside_boundary();
    void outside_vessel();
    void outside_boundary();

    void set_levelset_params();
    virtual bool handle(vgui_event const &);

    enum region_type {INSIDE_VESSEL,INSIDE_BOUNDARY,OUTSIDE_BOUNDARY,OUTSIDE_VESSEL, N_REGION_TYPES};

    static std::string nameOf(histogrampick_manager::region_type t){
      switch(t){
        case INSIDE_VESSEL : return "INSIDE_VESSEL"; 
        case INSIDE_BOUNDARY : return "INSIDE_BOUNDARY";
        case OUTSIDE_BOUNDARY : return "OUTSIDE_BOUNDARY";
        case OUTSIDE_VESSEL : return "OUTSIDE_VESSEL";
        default: return "Unknown";
      }
    }


  private:

    histogrampick_manager();
    static histogrampick_manager *instance_;

    bool pick_on_;
    region_type region_select_;

    unsigned w_,h_,d_;
    std::string path_;
    std::string prefix_;
    std::string pixel_type_;
    std::string extension_;

    vsol_polygon_2d_sptr polygon_;
    vgl_polygon<float> vglpoly_;

    vgui_deck_tableau_sptr deck_;
    vgui_viewer2D_tableau_sptr viewer_;
    vgui_easy2D_tableau_sptr easy2D_;
    vgui_range_map_params_sptr rmp_;
    bgui_picker_tableau_sptr pick_;
    std::vector<std::string> filenames_;

    vil_image_resource_sptr curimg_;

    std::vector<vgui_soview2D_polygon*> display_polygons_;
    std::vector<std::vector<float > > samples_;
};
#endif
