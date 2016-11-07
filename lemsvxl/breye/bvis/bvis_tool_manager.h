// This is breye/bvis/bvis_tool_manager.h
#ifndef bvis_tool_manager_h_
#define bvis_tool_manager_h_

//:
// \file
// \brief This file defines a class that acts as a gui manager.
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 10/30/03
//
// \verbatim
//  Modifications
//  \author J.L. Mundy converted tool portions to separate manager
//  \date 03/24/2007
// \endverbatim

#include <vcl_vector.h>
#include <vcl_map.h>
#include <vgui/vgui_tableau_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vidpro/vidpro_repository_sptr.h>
#include <vidpro/vidpro_process_manager.h>
#include <bvis/bvis_tool_sptr.h>
#include <bvis/bvis_tool.h>
#include <bvis/bvis_manager.h>
#include <bvis/bvis_video_manager.h>
template <class T>
class bvis_tool_manager
{

public:
  
  //: Access the static instance of the tableau manager
  static bvis_tool_manager * instance();


  vidpro_repository_sptr repository();
  //-------------------------------------------------------------

  //: Set the current active tool
  void set_active_tool(bvis_tool_sptr tool);

  //: Return the active tool
  bvis_tool_sptr active_tool() const
    {return active_tool_;}

  //: Stop the tool
  void deactivate_tool();

  //: Return the active_tableau (convenience)
  vgui_tableau_sptr active_tableau() const
    {return vis_manager_instance_->active_tableau();}

protected:
   //: Utilities
  void assign_input();
  void assign_output();
  void set_active_tableau();
private:
  //: Constructor
  bvis_tool_manager();
  //: Destructor
  ~bvis_tool_manager();

  //: Initialize the static instance
  void initialize();

  //: A static instance of the manager
  static bvis_tool_manager * instance_;

  //: A smart pointer to the repository
  vidpro_repository_sptr repository_;

  //: Currently active tool
  bvis_tool_sptr active_tool_;

  //: vis manager
  T* vis_manager_instance_;
};

#endif // bvis_tool_manager_h_

