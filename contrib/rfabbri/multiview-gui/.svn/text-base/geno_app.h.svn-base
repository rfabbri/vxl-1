// This is geno_app.h
#ifndef geno_app_h
#define geno_app_h
//:
//\file
//\brief GENO interactive application
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 03/24/2005 09:45:40 PM EST
//

#include <vgui/vgui_tableau.h>
#include <bvis1/bvis1_manager.h>
void geno_interactive();
void geno_reset();
void call_geno_process();
void call_eno_process();
void clear_all();

#define MANAGER bvis1_manager::instance()


//------------------------------------------------------------
// A tableau that displays the mouse position
// when left mouse button is pressed.
struct geno_shortcut_tableau : public vgui_tableau
{
  geno_shortcut_tableau() : vgui_tableau(){ }

  ~geno_shortcut_tableau() { }

  bool handle(const vgui_event &e)
  {
    if (e.type == vgui_KEY_PRESS)
    {
      switch(e.key) {
         case 'w':
            geno_reset();
            break;
         case 'g':
            // Interpolate!
            call_geno_process();
            break;
         case 'e':
            // Interpolate!
            call_eno_process();
         default:
            break;
      }
      return true; // event has been used
    }

    //  We are not interested in other events,
    //  so pass event to base class:
    return vgui_tableau::handle(e);
  }
};
//-------------------------------------------------------------
// Make a smart-pointer constructor for our tableau.
struct geno_shortcut_tableau_new : public vgui_tableau_sptr
{
  geno_shortcut_tableau_new() : vgui_tableau_sptr(new geno_shortcut_tableau()) { }
};

#endif // geno_app_h

