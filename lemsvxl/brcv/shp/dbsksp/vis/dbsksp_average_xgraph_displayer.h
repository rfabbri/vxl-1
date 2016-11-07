// This is dbsksp/vis/dbsksp_average_xgraph_displayer.h
#ifndef dbsksp_average_xgraph_displayer_h_
#define dbsksp_average_xgraph_displayer_h_

//:
// \file
// \brief  A displayer to generate a tableau for dbsksp_average_xgraph_storage
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date   Mar 3, 2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>
#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>


class dbsksp_average_xgraph_displayer : public bvis1_displayer 
{
public:
  dbsksp_average_xgraph_displayer(){}

  //: Return type string "dbsksp_skgraph"
  virtual vcl_string type() const { return "dbsksp_average_xgraph"; }

  //: Create a tableau if the storage object is of type "dbsksp_xgraph"
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

};

#endif // dbsksp_average_xgraph_displayer_h_
