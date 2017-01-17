
#include "bvis2_video_file_menu.h"
#include <bvis2/bvis2_video_manager.h>

bvis2_video_file_menu::bvis2_video_file_menu()
{
}

bvis2_video_file_menu::~bvis2_video_file_menu()
{
}

void
bvis2_video_file_menu::add_to_menu( vgui_menu & menu )
{
  menu.add( "Save as movie" ,   save_view_as_movie );
}

void
bvis2_video_file_menu::save_view_as_movie()
{
  bvis2_video_manager::instance()->save_view_as_movie();
}


