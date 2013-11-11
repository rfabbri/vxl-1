// This is brcv/rec/dbskr/vis/dbskr_shock_patch_displayer.h
#ifndef dbskr_shock_patch_displayer_h_
#define dbskr_shock_patch_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau out of dbskr_shock_patch_storage_sptr.
// \author Ozge Can Ozcanli (ozge@lems.brown.edu)
// \date 03/13/07 
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>

class dbskr_shock_patch_displayer : public bvis1_displayer 
{
public:
  dbskr_shock_patch_displayer(){}

  //: Return type string "shock_match"
  virtual vcl_string type() const { return "shock_patch"; }

  //: Create a tableau if the storage object is of type shock_patch
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;
};

#endif // dbskr_shock_patch_displayer_h_
