// This is brcv/rec/dbcvr/vis/dbcvr_curvematch_displayer.h
#ifndef dbcvr_curvematch_displayer_h_
#define dbcvr_curvematch_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau out of dbcvr_curvematch_storage_sptr.
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 11/10/03 
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>


class dbcvr_curvematch_displayer : public bvis1_displayer {

public:
  dbcvr_curvematch_displayer(){}

  //: Return type string "curvematch"
  virtual vcl_string type() const { return "curvematch"; }

  //: Create a tableau if the storage object is of type image
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

};

#endif // dbcvr_curvematch_displayer_h_
