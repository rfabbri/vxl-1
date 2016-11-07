// This is brcv/rec/dbskr/vis/dbskr_shock_match_displayer.h
#ifndef dbskr_shock_match_displayer_h_
#define dbskr_shock_match_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau out of dbskr_shock_match_storage_sptr.
// \author Ozge Can Ozcanli (ozge@lems.brown.edu)
// \date 09/30/05 
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>

class dbskr_shock_match_displayer : public bvis1_displayer 
{
public:
  dbskr_shock_match_displayer(){}

  //: Return type string "shock_match"
  virtual vcl_string type() const { return "shock_match"; }

  //: Create a tableau if the storage object is of type shock_match
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;
};

#endif // dbskr_shock_match_displayer_h_
