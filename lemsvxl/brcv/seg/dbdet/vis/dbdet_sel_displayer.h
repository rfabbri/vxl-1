// This is brcv/seg/dbdet/vis/dbdet_sel_displayer.h
#ifndef dbdet_sel_displayer_h_
#define dbdet_sel_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau out of dbdet_sel_storage_sptr.
// \author Amir Tamrakar
// \date 03/28/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>


class dbdet_sel_displayer : public bvis1_displayer {

public:
  dbdet_sel_displayer(){}

  //: Return type string "sel"
  virtual vcl_string type() const { return "sel"; }

  //: Create a tableau if the storage object is of type "sel"
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

};

#endif // dbdet_sel_displayer_h_
