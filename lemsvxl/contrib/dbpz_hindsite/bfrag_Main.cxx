#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer3D_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>


#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_adaptor.h>


#include "bfrag_Main_Menu.h"
#include "bfrag_Manager.h"

int main(int argc, char** argv)
{
        
    vgui::init(argc, argv);  

    bfrag_Main_Menu menubar;
    vgui_menu menu_holder = menubar.setup_menu();
    unsigned w = 1024, h = 768;
    vcl_string title = "HindSite - The Fragment Assembler";
    vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
    
    win->set_statusbar(true);
    win->enable_vscrollbar(false);
    win->enable_hscrollbar(false);
    win->set_statusbar(false);

    bfrag_Manager::instance()->setShell( vgui_shell_tableau_new() );
    win->get_adaptor()->set_tableau( bfrag_Manager::instance()->shell() );

    win->show();

    return vgui::run();

}


