// This is brcv/seg/dbdet/vis/dbdet_edgemap_displayer.h
#ifndef dbdet_edgemap_displayer_h_
#define dbdet_edgemap_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau from dbdet_edgemap_storage_sptr.
// \author Amir Tamrakar
// \date 09/11/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>


class dbdet_edgemap_displayer : public bvis1_displayer {

public:
  dbdet_edgemap_displayer(){}

  //: Return type string "edge_map"
  virtual vcl_string type() const { return "edge_map"; }

  //: Create a tableau if the storage object is of type "sel"
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

};

#endif // dbdet_edgemap_displayer_h_
