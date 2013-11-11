/*************************************************************************
 *    NAME: Alexander K. Bowman
 *    USER: akb
 *    FILE: dbsk3dapp_manager.cxx
 *    DATE: 
 *************************************************************************/

#include <vgui/vgui.h>

#include "dbsk3dapp_manager.h"

dbsk3dapp_manager* dbsk3dapp_manager::instance_ = 0;

dbsk3dapp_manager* dbsk3dapp_manager::instance()
{
  if (!instance_) 
    instance_ = new dbsk3dapp_manager();
  return dbsk3dapp_manager::instance_;
}

dbsk3dapp_manager::dbsk3dapp_manager()
{
  file_opened_cache_ = "";
  file_saved_cache_ = "";
  
  pv1_ = new dbsk3d_pro_vis ();
  view_manager_ = new dbsk3dapp_view_manager ();
  action_ = new dbsk3dapp_action (view_manager_);
}

dbsk3dapp_manager::~dbsk3dapp_manager()
{
  delete action_;
  delete pv1_;
  delete view_manager_;
}


