
#include "bvis_repository_menu.h"
#include <vgui/vgui_dialog.h>
#include <vidpro/vidpro_repository.h>

template <class T>
void bvis_repository_menu<T>::add_to_menu( vgui_menu & menu )
{
  add( "Clear All" , clear_all);
  separator();
  add( "Load Repository" , load_repository );
  add( "Add Repository" , add_to_repository );
  add( "Save Repository" , save_repository );
  separator();
  add( "View Repository" , view_repository );
  
  menu.add( "Repository" , (*this));
}

template <class T>
void bvis_repository_menu<T>::load_repository()
{
  clear_all();
  T::instance()->load_repository();
}

template <class T>
void bvis_repository_menu<T>::add_to_repository()
{
  T::instance()->add_to_repository();
}


template <class T>
void bvis_repository_menu<T>::save_repository()
{
  T::instance()->save_repository();
}
template <class T>
void bvis_repository_menu<T>::view_repository()
{
  T::instance()->view_repository();
}
template <class T>
void bvis_repository_menu<T>::clear_all()
{
   T::instance()->repository()->initialize();
   T::instance()->regenerate_all_tableaux();
   T::instance()->display_visible_data(true);
}

#undef BVIS2_REPOSITORY_MENU_INSTANTIATE
#define BVIS2_REPOSITORY_MENU_INSTANTIATE(T) \
template class bvis_repository_menu<T >;
