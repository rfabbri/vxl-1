
#include "bvis_tableau_menu.h"
#include <vgui/vgui_dialog.h>

template <class T>
void bvis_tableau_menu<T>::add_to_menu( vgui_menu & menu )
{
  add( "New Storage" , make_empty_storage );
  add( "Get Info on Active Tableau" , get_info_on_active_tableau );
  add( "Regenerate All Tableaux" , regenerate_tableaux );
  add( "Cache Tableaux" , cache_tableaux );
   
  menu.add( "Tableaux" , (*this));
}
template <class T>
void bvis_tableau_menu<T>::get_info_on_active_tableau()
{
  T::instance()->get_info_on_active_tableau();
}

template <class T>
void bvis_tableau_menu<T>::regenerate_tableaux()
{
  T::instance()->regenerate_all_tableaux();
  T::instance()->display_visible_data(true);
}

template <class T>
void bvis_tableau_menu<T>::cache_tableaux()
{
  T::instance()->toggle_tableau_cache();
}

template <class T>
void bvis_tableau_menu<T>::make_empty_storage()
{
  T::instance()->make_empty_storage();
}

#undef BVIS2_TABLEAU_MENU_INSTANTIATE
#define BVIS2_TABLEAU_MENU_INSTANTIATE(T) \
template class bvis_tableau_menu<T >;
