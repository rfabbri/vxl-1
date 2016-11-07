// This is breye/bvis/bvis_manager.h
#ifndef bvis_manager_h_
#define bvis_manager_h_

//:
// \file
// \brief This file defines a class that acts as a gui manager.
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 10/30/03
//
// \verbatim
//  Modifications
//   30-OCT-2003  Matt Leotta - Initial version.
//   27-MAR-2005  Ozge Ozcanli - added get_views() method
//   19-MAR-2007  Eduardo Almeida - Removed video related members
// \endverbatim

#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_set.h>
#include <vcl_string.h>
#include <vbl/vbl_ref_count.h>

#include <vgui/vgui_tableau_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_dialog.h>

#include <bvis/bvis_proc_selector_tableau.h>

#include <bvis/bvis_displayer_sptr.h>
#include <bpro/bpro_storage_sptr.h>
#include <bpro/bpro_storage.h>
#include <bpro/bpro_parameters.h>

#include <vidpro/vidpro_repository_sptr.h>
#include <vidpro/vidpro_repository.h>
#include <vidpro/vidpro_process_manager.h>

#include <bvis/bvis_view_tableau_sptr.h>
#include <bvis/bvis_tool_sptr.h>
#include <bvis/bvis_tool.h>

template <class T>
class bvis_manager : public vgui_wrapper_tableau 
{

public:
  static T * instance();

  //: Handle events
  virtual bool handle( const vgui_event & e )=0;

  //: Registers a displayer type
  //  Call this function with an instance of each type you plan to use
  static void register_displayer(bvis_displayer_sptr displayer);

  //: Access the selector tableau in the active view
  bvis_proc_selector_tableau_sptr active_selector();

  //: Access the active tableau at the current frame
  vgui_tableau_sptr active_tableau();

  //: Set the active tableau in the active view
  void set_active_tableau(const vcl_string& name);

  //: Find the names of the visible data in the active view
  vcl_set<vcl_string> visible_storage();

  //: Look up the storage class that created the given tableau
  bpro_storage_sptr storage_from_tableau(const vgui_tableau_sptr& tab);

  //: Create a tableau for storage class and cache it
  bool add_to_display(const bpro_storage_sptr& storage);

  //: Regenerate all tableau from the repository
  virtual void regenerate_all_tableaux()=0;

  //: Construct a new view
  virtual void add_new_view()=0;

  ////: Create a new empty storage class
  virtual void make_empty_storage()=0;

    //: Remove the view in the active part of the grid
  void remove_active_view();


  //: Create one view for each visible layer of the current frame
  void layer_per_view();

  //: Display info about the active tableau
  void get_info_on_active_tableau();

  //-------------------------------------------------------------

  ////: Save OpenGL render areas as a movie
  //void save_view_as_movie() const;

  //-------------------------------------------------------------

  //: Set the current active tool
  void set_active_tool(bvis_tool_sptr tool);

  //: Return the active tool
  bvis_tool_sptr active_tool() const;

  //: quit the application
  void quit();

  //--------------------------------------------------------------

  //: Macro functions
  bool recording_macro();
  void start_recording_macro();
  void stop_recording_macro();

  //--------------------------------------------------------------

  //: Toggle caching of tableaux
  void toggle_tableau_cache();

  //: Calls the appropriate type of displayer to make a tableau
  //: \param cacheable returns true by reference if we are allowed to cache this tableau
  vgui_tableau_sptr make_tableau(const bpro_storage_sptr& storage, bool& cacheable) const;

  //: Calls the appropriate type of displayer to update a tableau
  bool update_tableau(const vgui_tableau_sptr& tableau, 
    const bpro_storage_sptr& storage ) const;

  //:return the view vector
  vcl_vector<bvis_view_tableau_sptr> get_views() {return view_tabs_;}

  //:get a new view tableau of the appropriate sub_class
  virtual bvis_view_tableau_sptr view_tableau(bvis_proc_selector_tableau_sptr& selector)=0;

  //: get a displayer by type name e.g. vsol2D
  bvis_displayer_sptr displayer(  vcl_string const& type );

  //: dialog content methods
  bool parameter_dialog(vcl_vector< bpro_param* >& param_vector,
                        vgui_dialog& param_dialog);
protected:
  static T * instance_;
  //: Constructor
  bvis_manager();

  ~bvis_manager(){}
  //: Initialize the static instance
  void initialize();

  //: Configure the child if necessary
  virtual void configure(){}

  //: The grid tableau at the root
  vgui_grid_tableau_sptr grid_tab_;

  //: The view tableaux that provided a view of the data
  vcl_vector<bvis_view_tableau_sptr> view_tabs_;

  //: A map from storage object pointers to tableaux
  vcl_map< bpro_storage*, vgui_tableau_sptr > tableau_map_;

  //: Vector of maps from type to displayer
  static vcl_map< vcl_string, bvis_displayer_sptr > displayers_;

  //: Display all visible tableaux
  virtual void display_visible_data(bool clear_old = false)=0;

  //: Is the manager recording processes
  bool recording_macro_bool_;

  //: This flag indicates whether or not to cache tableau 
  bool cache_tableau_;

};

#endif // bvis_manager_h_
