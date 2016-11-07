
#include "bvis2_video_view_menu.h"
#include <vgui/vgui_dialog.h>
#include <bvis2/bvis2_video_manager.h>

bvis2_video_view_menu::bvis2_video_view_menu()
{
}

bvis2_video_view_menu::~bvis2_video_view_menu()
{
}

void
bvis2_video_view_menu::add_to_menu( vgui_menu & menu )
{
  menu.add( "Add Video View" ,   add_video_view );
}

void bvis2_video_view_menu::add_video_view()
{
  int offset = 0;
  bool absolute = false;
  //Prompt the user to select a frame offset
  vgui_dialog view_dialog( "New View" );
  view_dialog.field( "Frame offset" , offset );
  view_dialog.checkbox( "Absolute Offset", absolute );
  
  if( view_dialog.ask() )
    bvis2_video_manager::instance()->add_new_video_view(offset, absolute);
  bvis2_video_manager::instance()->display_current_frame();
}



