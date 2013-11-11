// This is breye1/dbetrk/vis/dbvis1_edgetrk_displayer.h
#ifndef dbvis1_edgetrk_displayer_h_
#define dbvis1_edgetrk_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau out of dbetrk_storage_sptr.
// \author Vishal Jain (vj@lems.brown.edu)
// \date 09/23/04
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>
#include <dbetrk/dbetrk_edge_sptr.h>

class dbvis1_edgetrk_displayer : public bvis1_displayer {

public:
  dbvis1_edgetrk_displayer(){}

  //: Return type string "trackededge"
  virtual vcl_string type() const { return "dbetrk"; }

  //: Create a tableau if the storage object is of type "edgetrk"
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

  
  
};

#endif // dbvis1_ctrk_displayer_h_
