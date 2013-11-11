//This is bvis/bvis_video_add_menu.cxx

#include "bvis_video_add_menu.h"
#include <bvis/bvis_video_manager.h>

bvis_video_add_menu::bvis_video_add_menu()
{
}

bvis_video_add_menu::~bvis_video_add_menu()
{
}

void
bvis_video_add_menu::add_to_menu( vgui_menu & menu )
{
  add( "Add new frame" , add_new_frame );

  menu.add( "Add" , (*this) );
}

void
bvis_video_add_menu::add_new_frame()
{
  bvis_video_manager::instance()->add_new_frame();
}
