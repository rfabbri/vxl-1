// This is brcv/dbsk2d/vis/dbsk2d_shock_displayer.h
#ifndef dbsk2d_shock_displayer_h_
#define dbsk2d_shock_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau out of vidpro1_shock_storage_sptr.
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 11/7/03 
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>


class dbsk2d_shock_displayer : public bvis1_displayer 
{
public:
  dbsk2d_shock_displayer(){}

  //: Return type string "shock"
  virtual vcl_string type() const { return "shock"; }

  //: Create a tableau if the storage object is of type image
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

};

#endif // dbvis1_shock_displayer_h_
