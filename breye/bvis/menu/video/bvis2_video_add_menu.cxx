//This is bvis2/bvis2_video_add_menu.cxx

#include "bvis2_video_add_menu.h"
#include <bvis2/bvis2_video_manager.h>

bvis2_video_add_menu::bvis2_video_add_menu()
{
}

bvis2_video_add_menu::~bvis2_video_add_menu()
{
}

void
bvis2_video_add_menu::add_to_menu( vgui_menu & menu )
{
  add( "Add new frame" , add_new_frame );

  menu.add( "Add" , (*this) );
}

void
bvis2_video_add_menu::add_new_frame()
{
  bvis2_video_manager::instance()->add_new_frame();
}
