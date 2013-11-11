// This is breye/bvis/bvis_mview_manager.h
#ifndef bvis_mview_manager_h_
#define bvis_mview_manager_h_

//:
// \file
// \brief This file defines a gui manager class for multiple view geometry
// \author J.L. Mundy
// \date 04/14/07
//
// \verbatim
//  Modifications
// none
// \endverbatim


#include <bvis/bvis_manager.h>
#include <vidpro/vidpro_repository_sptr.h>
class bvis_mview_manager : public bvis_manager<bvis_mview_manager> 
{

public:
  
  vidpro_repository_sptr repository();
  //-------------------------------------------------------------
  //: Load the repository from a binary data file
  void load_repository();

  //: Save the repository from a binary data file
  void save_repository();

  //: Add additional storage items to the repository from a binary data file
  void add_to_repository();


  //: Display the structure of the repository
  void view_repository();

  //: Handle events
  bool handle( const vgui_event & e );

  //: Regenerate all tableau from the repository
  void regenerate_all_tableaux();

  virtual void add_new_view(){this->add_new_mview_view();}

  void add_new_mview_view();

  void add_new_camera_view();

  void remove_selected_camera_view();

  //: Create a new empty storage class
  virtual void make_empty_storage();

  //: Adjust the tableau smart pointers for the current frame and refresh the display
  virtual void display_visible_data(bool clear_old = false);

  friend class bvis_manager<bvis_mview_manager>;
 protected: 
  virtual void configure();

 private:
  //: Constructor
  bvis_mview_manager();
  //: Destructor
  ~bvis_mview_manager();

  virtual bvis_view_tableau_sptr view_tableau(bvis_proc_selector_tableau_sptr& selector);

};

#endif // bvis_mview_manager_h_
