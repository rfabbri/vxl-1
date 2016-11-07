
#ifndef vehicle_model_menus_h_
#define vehicle_model_menus_h_

class vgui_menu;

//: Menu callbacks are represented as static methods
//  The key method is ::get_menu, which does the work of
//  assembling the menu bar for the main executable 
class vehicle_model_menus
    {
    public:
        static void quit_callback();
        static void load_image_callback();
        static void pick_model_callback();
        static void adjust_model_callback();
        static void translate_model_callback();
        static void save_ref_model_callback();
        static void save_adjusted_model_callback();
        static void load_ref_model_callback();
        static void load_intrinsic_mean_model_callback();

        static void set_range_params_callback();
        static vgui_menu get_menu();
    private:
        vehicle_model_menus() {}
    };

#endif 


