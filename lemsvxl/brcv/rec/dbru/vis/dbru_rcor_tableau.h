// This is brcv/rec/dbru/vis/dbru_rcor_tableau.h
#ifndef dbru_rcor_tableau_h_
#define dbru_rcor_tableau_h_

//:
// \file
// \brief A tableau to display dbsk2d_shock_storage objects.
// \author Amir Tamrakar
// \date 11/30/2005
//
// \verbatim
//  Modifications
//     Amir Tamrakar      added a menu to change the grid interval
//
// \endverbatim

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_parent_child_link.h>
#include <vgui/vgui_gl.h>

#include "dbru_rcor_tableau_sptr.h"
#include <dbru/dbru_rcor_sptr.h>

//: A tableau to display dbsk2d_shock_storage objects
class dbru_rcor_tableau : public vgui_tableau 
{
public:

  //: Constructor
  dbru_rcor_tableau();
  virtual ~dbru_rcor_tableau();

  //: handle the render and select events
  virtual bool handle( const vgui_event & );

  //: return the region correspondence
  dbru_rcor_sptr get_rcor() { return rcor_; }
  //: set the region correspondence
  void set_rcor( dbru_rcor_sptr new_rcor ) { rcor_ = new_rcor; }

  //: Generate the popup menu for this tableau
  void get_popup(const vgui_popup_params& params, vgui_menu &menu);
 
  // rendering funtions

  //: render the mapping
  void draw_grid_mapping();

private:

  short CM[7][3];                         ///< colormap to draw the grids

  bool display_grid_map_;                 ///< display the grid map showing region correspondence
  bool display_grid1_;
  bool display_grid2_;
  bool display_continuous_grid_;
  bool display_colored_grid_;             ///< colored grid or just plain grey level

  int grid_int_;                          ///< interval of the grid lines (in pixels)
  dbru_rcor_sptr rcor_;                   ///< region correspondence data structure to display

};


//: Create a smart-pointer to a dbru_rcor_tableau.
struct dbru_rcor_tableau_new : public dbru_rcor_tableau_sptr
{
  typedef dbru_rcor_tableau_sptr base;

  //: Constructor - creates a pointer to a dbru_rcor_tableau
  dbru_rcor_tableau_new() : base(new dbru_rcor_tableau()) { }
};

#endif //dbru_rcor_tableau_h_
