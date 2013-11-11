
#include "bvis_file_menu.h"
#include <bvis/bvis_video_manager.h>
template <class T>
bvis_file_menu<T>::bvis_file_menu()
{
}
template <class T>
bvis_file_menu<T>::~bvis_file_menu()
{
}

template <class T>
void bvis_file_menu<T>::add_to_menu( vgui_menu & menu )
{
  add( "Quit" , quit );

  menu.add( "File" , (*this) );
}

template <class T>
void bvis_file_menu<T>::quit()
{
  T::instance()->quit();
}


#undef BVIS2_FILE_MENU_INSTANTIATE
#define BVIS2_FILE_MENU_INSTANTIATE(T) \
template class bvis_file_menu<T >;
