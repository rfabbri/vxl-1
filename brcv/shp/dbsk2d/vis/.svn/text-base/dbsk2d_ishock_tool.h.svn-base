// This is brcv/dbsk2d/vis/dbsk2d_ishock_tool.h
#ifndef dbsk2d_ishock_tool_h_
#define dbsk2d_ishock_tool_h_
//:
// \file
// \brief Base class for all ishock tools 
// \author Mark Johnson (mrj)
// \date Mon Sep 15 11:25:45 EDT 2003
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis1/bvis1_tool.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_projection_inspector.h>

#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include "dbsk2d_shock_tableau.h"
#include "dbsk2d_shock_tableau_sptr.h"

class dbsk2d_ishock_tool : public bvis1_tool
{
public:
  dbsk2d_ishock_tool();
  virtual ~dbsk2d_ishock_tool();

  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );
  virtual bool set_storage ( const bpro1_storage_sptr& storage);

  virtual vcl_string name() const = 0;

protected:

  dbsk2d_shock_tableau_sptr tableau();
  dbsk2d_shock_storage_sptr storage();

  //: shock tableau
  dbsk2d_shock_tableau_sptr tableau_;
  //: shock storage class
  dbsk2d_shock_storage_sptr storage_;

};

#endif //dbsk2d_ishock_tool_h_
