// This is brcv/rec/dber/vis/dber_edge_match_displayer.h
#ifndef dber_edge_match_displayer_h_
#define dber_edge_match_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau out of dber_edge_match_storage_sptr.
// \author Ozge Can Ozcanli (ozge@lems.brown.edu)
// \date 10/23/06 
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>

class dber_edge_match_displayer : public bvis1_displayer 
{
public:
  dber_edge_match_displayer(){}

  //: Return type string "shock_match"
  virtual vcl_string type() const { return "edge_match"; }

  //: Create a tableau if the storage object is of type shock_match
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;
};

#endif // dber_edge_match_displayer_h_
