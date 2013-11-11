#ifndef histogrampick_menus_h_
#define histogrampick_menus_h_


//histogrampick_menus.h
class histogrampick_menus
{
  public:
    static void quit_callback();
    static void save_histogram_callback();
    static void load_image_callback();
    static void set_range_callback();
    static void pick_box_callback();

    static void inside_vessel_callback();
    static void inside_boundary_callback();
    static void outside_vessel_callback();
    static void outside_boundary_callback();

    static vgui_menu get_menu();
  private:
    histogrampick_menus(){};
};
#endif
