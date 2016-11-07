// This is contrib/ntrinh/v2_gui/dbsksp_shock_displayer.h
#ifndef dbsksp_shock_displayer_h_
#define dbsksp_shock_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau for dbsksp_shock_storage.
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Sep 28, 2006
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>


class dbsksp_shock_displayer : public bvis1_displayer {

public:
  dbsksp_shock_displayer(){}

  //: Return type string "dbsksp_skgraph"
  virtual vcl_string type() const { return "dbsksp_shock"; }

  //: Create a tableau if the storage object is of type "dbsksp_shock"
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

};

#endif // dbsksp_shock_displayer_h_
