#include "biov_transfer_function.h"
#include "biov_examiner_tableau.h"

biov_examiner_tableau * global_examiner;

biov_transfer_function::biov_transfer_function(biov_examiner_tableau* tab)
{
    global_examiner = tab;
}

void biov_transfer_function::execute()
{
    int min = global_examiner->min;
    int max = global_examiner->max;
    int map = global_examiner->map;
    vgui_dialog transfer_dialog("Transfer Function");
    transfer_dialog.field("MIN", min);
    transfer_dialog.field("MAX", max);
    vcl_vector <vcl_string> color_map_labels(8);
    color_map_labels[0] = "NONE";
    color_map_labels[1] = "GREY";
    color_map_labels[2] = "TEMPERATURE";
    color_map_labels[3] = "PHYSICS";
    color_map_labels[4] = "STANDARD";
    color_map_labels[5] = "GLOW";
    color_map_labels[6] = "BLUE_RED";
    color_map_labels[7] = "SEISMIC";
    transfer_dialog.choice("COLOR MAPS", color_map_labels, map);
    global_examiner->transfer_callback(transfer_dialog.ask(), min, max, map);
}

void min_slider_callback( vgui_slider_tableau* tab, void* data )
{
    int* number = static_cast<int*>(data);
    *number = (int)(tab->value() * 255+0.5);
}

void max_slider_callback( vgui_slider_tableau* tab, void* data )
{
    int* number = static_cast<int*>(data);
    *number = (int)(tab->value() * 255+0.5);
}
