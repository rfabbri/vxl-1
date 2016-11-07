
#include "bvis_video_file_menu.h"
#include <bvis/bvis_video_manager.h>

bvis_video_file_menu::bvis_video_file_menu()
{
}

bvis_video_file_menu::~bvis_video_file_menu()
{
}

void
bvis_video_file_menu::add_to_menu( vgui_menu & menu )
{
  menu.add( "Save as movie" ,   save_view_as_movie );
}

void
bvis_video_file_menu::save_view_as_movie()
{
  bvis_video_manager::instance()->save_view_as_movie();
}


