#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>

#include <vcl_iostream.h>

#include "pairMatchMainMenu.h"
#include "pairMatchManager.h"

int main(int argc, char **argv){

    vgui::init(argc, argv);

    pairMatchMainMenu menuBar;
    vgui_menu menuHolder = menuBar.setupMenu();

    //set up main window
    vcl_string title = "pairMatch - From The Ground Up";
    vgui_window* win = vgui::produce_window(800, 600, menuHolder, title);
    win->get_adaptor()->set_tableau( pairMatchManager::instance()->getTheShell() );
    win->show();

    //run gui
    return vgui::run();

}
