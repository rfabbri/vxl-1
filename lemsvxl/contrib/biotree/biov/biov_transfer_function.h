#ifndef biov_transfer_function_
#define biov_transfer_function_

#include <vgui/vgui_command.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_slider_tableau.h>
#include <vgui/vgui_poly_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>

class biov_examiner_tableau;

void min_slider_callback( vgui_slider_tableau* tab, void* data );
void max_slider_callback( vgui_slider_tableau* tab, void* data );

//: A vgui command to open a popup slider to change the transfer function
class biov_transfer_function : public vgui_command {

public:

  biov_transfer_function(biov_examiner_tableau* tab);
  void execute();

public:
};

#endif
