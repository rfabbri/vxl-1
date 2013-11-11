// This is dbsksp/vis/dbsksp_xgraph_displayer.h
#ifndef dbsksp_xgraph_displayer_h_
#define dbsksp_xgraph_displayer_h_

//:
// \file
// \brief  A displayer responsible for generating a tableau for dbsksp_xgraph_storage
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date   Oct 21, 2009
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>

#include <dbsksp/vis/dbsksp_xgraph_tableau_sptr.h>
#include <dbsksp/pro/dbsksp_xgraph_storage_sptr.h>

class dbsksp_xgraph_displayer : public bvis1_displayer {

public:
  dbsksp_xgraph_displayer(){}

  //: Return type string "dbsksp_skgraph"
  virtual vcl_string type() const { return "dbsksp_xgraph"; }

  //: Create a tableau if the storage object is of type "dbsksp_shock"
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

  //: Update a tableau with a the storage object if both are of type image
  virtual bool update_tableau( vgui_tableau_sptr, const bpro1_storage_sptr& ) const;
 
protected:
  //: Perform the update once types are known
  virtual bool update_xgraph_tableau(dbsksp_xgraph_tableau_sptr, const dbsksp_xgraph_storage_sptr&) const;


};

#endif // dbsksp_xgraph_displayer_h_
