//This is bvis/bvis_process_macro_menu.h

#include "bvis_process_macro_menu.h"
#include <bvis/bvis_manager_traits.h>
template <class T>
bvis_process_macro_menu<T>::bvis_process_macro_menu()
{
}
template <class T>
bvis_process_macro_menu<T>::~bvis_process_macro_menu()
{
}
template <class T>
void bvis_process_macro_menu<T>::add_to_menu( vgui_menu & menu )
{
  add( "Start Recording" , start_recording );
  add( "Stop Recording", stop_recording );
  separator();
  add( "Delete Last Step" , delete_last );
  add( "Clear All" , clear_all );
  separator();
  add( "Display Process List", view_process_list);

  menu.add( "Process Macro" , (*this) );
}

template <class T>
void bvis_process_macro_menu<T>::start_recording()
{
  T::instance()->start_recording_macro();
}

template <class T>
void bvis_process_macro_menu<T>::stop_recording()
{
  T::instance()->stop_recording_macro();
}
template <class T>
void bvis_process_macro_menu<T>::clear_all()
{
  bvis_manager_traits<T>::process_manager()->clear_process_queue();
}

template <class T>
void bvis_process_macro_menu<T>::delete_last()
{
  bvis_manager_traits<T>::process_manager()->delete_last_process();
}

template <class T>
void bvis_process_macro_menu<T>::view_process_list()
{
  vgui_dialog display_dialog("Current process Queue" );

  //display current process list
  display_dialog.message("Current Process Queue:");

  vcl_vector<vcl_string> process_list = 
    bvis_manager_traits<T>::process_manager()->get_process_queue_list();
  vcl_vector<vcl_string>::iterator i = process_list.begin();
  for (; i!=process_list.end(); i++)
    display_dialog.message( (*i).c_str()); 

  //onlyshow ok button
  display_dialog.set_cancel_button(0);

  display_dialog.ask();
}
#undef BVIS2_PROCESS_MACRO_MENU_INSTANTIATE
#define BVIS2_PROCESS_MACRO_MENU_INSTANTIATE(T) \
template class bvis_process_macro_menu<T >;
