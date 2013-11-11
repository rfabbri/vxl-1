
#include "bvis_view_menu.h"
#include <vgui/vgui_dialog.h>
template <class T>
void bvis_view_menu<T>::add_to_menu( vgui_menu & menu )
{
  add( "Add View" , add_view );
  add( "Remove View" , remove_view );
  add( "Layer per View", layer_per_view );
  
  menu.add( "View" , (*this));
}


template <class T>
void bvis_view_menu<T>::add_view()
{
  T::instance()->add_new_view();
}

template <class T>
void bvis_view_menu<T>::remove_view()
{
  T::instance()->remove_active_view();
  T::instance()->display_visible_data();
}

template <class T>
void bvis_view_menu<T>::layer_per_view()
{
  T::instance()->layer_per_view();
  T::instance()->display_visible_data();
}
#undef BVIS2_VIEW_MENU_INSTANTIATE
#define BVIS2_VIEW_MENU_INSTANTIATE(T) \
template class bvis_view_menu<T >;

