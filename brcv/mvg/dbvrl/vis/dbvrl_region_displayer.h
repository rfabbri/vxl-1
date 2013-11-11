// This is brcv/mvg/dbvrl/vis/dbvrl_region_displayer.h
#ifndef dbvrl_region_displayer_h_
#define dbvrl_region_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau out of dbvrl_region_storage_sptr.
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 4/27/04 
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>


class dbvrl_region_displayer : public bvis1_displayer {

public:
  dbvrl_region_displayer(){}

  //: Return type string "dbvrl"
  virtual vcl_string type() const { return "bvrl"; }

  //: Create a tableau if the storage object is of type "dbvrl"
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

};

#endif // dbvrl_region_displayer_h_
