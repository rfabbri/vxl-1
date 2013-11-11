// This is breye/bvis/menu/bvis_tool_command.h
#ifndef bvis_command_h_
#define bvis_command_h_

//:
// \file
// \brief This file defines vgui_command classes for bvis2
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 2/09/04
//
// Each class in this file is derived from vgui_command and
// each must define an execute member function which is called
// when the menu item is selected.
//
// \verbatim
//  Modifications
//      Amir Tamrakar 06/02/04: added the file load/save commands for the new file menu
//      Matt Leotta   07/08/04: merged the load/save commands back into the original command
//                              with a new parameter to disable the queue
// \endverbatim

#include <vgui/vgui_command.h>
#include <bvis/bvis_tool_sptr.h>
#include <bpro/bpro_process_sptr.h>
#include <bvis/bvis_manager_traits.h>
// Macros
#define MENU_ADD_TOOL(tool, menu, T) { bvis_tool_sptr t = new tool; menu.add(t->name(), new bvis_tool_command<T > (t)); }

#define MENU_ADD_TOOL_USER_IO(tool, menu, T) { bvis_tool_sptr t = new tool; menu.add(t->name()+"(I/O)", new bvis_user_io_tool_command<T > (t)); }

#define MENU_ADD_PROCESS_NAME(name, menu, T) menu.add( name, \
            new bvis_process_command<T > ( bvis_manager_traits< T >::process_manager()->get_process_by_name(name)) )

#define MENU_ADD_PROCESS_NAME2(name, process_name, menu, T) menu.add( name, \
            new bvis_process_command<T > ( bvis_manager_traits< T >::process_manager() ->get_process_by_name( process_name ) ) )

#define FILE_MENU_ADD_PROCESS_NAME(name, process_name, menu, T) menu.add(name, \
            new bvis_process_command<T > ( bvis_manager_traits< T >::process_manager()->get_process_by_name(process_name), false))



//: \brief The command for selecting the active tool from a menu with
// default input and output storage
template <class T>
class bvis_tool_command : public vgui_command
{
 public:
  bvis_tool_command(const bvis_tool_sptr& tool) : tool_(tool) {}
  void execute();

  bvis_tool_sptr tool_;
};

//: \brief The command for selecting the active tool from a menu with
// user defined inputs and outputs rather than defaults
template <class T>
class bvis_user_io_tool_command : public vgui_command
{
 public:
  bvis_user_io_tool_command(const bvis_tool_sptr& tool) : tool_(tool) {}
  void execute();
  bvis_tool_sptr tool_;
};

//: \brief The command for selecting a process from a menu
template <class T>
class bvis_process_command : public vgui_command
{
 public:
  bvis_process_command(const bpro_process_sptr& process, bool allow_queue = true)
   : process_(process), allow_queue_(allow_queue) {}
  void execute();
  void execute_mview_process();
  void execute_video_process();
  bpro_process_sptr process_;
  bool allow_queue_;
};


#endif //bvis_command_h_
