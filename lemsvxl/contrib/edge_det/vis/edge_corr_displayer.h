// This is vis/edge_corr_displayer.h
#ifndef edge_corr_displayer_h_
#define edge_corr_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau from edge_corr_storage_sptr.
// \author Amir Tamrakar
// \date 12/14/07
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>


class edge_corr_displayer : public bvis1_displayer {

public:
  edge_corr_displayer(){}

  //: Return type string "edge_map_corr"
  virtual vcl_string type() const { return "edge_map_corr"; }

  //: Create a tableau if the storage object is of type "edge_map_corr"
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

};

#endif // edge_corr_displayer_h_
