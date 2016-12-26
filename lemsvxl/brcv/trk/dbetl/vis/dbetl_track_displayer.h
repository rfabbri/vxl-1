// This is brcv/trk/dbetl/vis/dbetl_track_displayer.h
#ifndef dbetl_track_displayer_h_
#define dbetl_track_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau out of dbetl_track_storage_sptr.
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 11/22/04 
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>


class dbetl_track_displayer : public bvis1_displayer {

public:
  dbetl_track_displayer(){}

  //: Return type string "dbetl"
  virtual vcl_string type() const { return "betl"; }

  //: Create a tableau if the storage object is of type dbetl
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

};

#endif // dbetl_track_displayer_h_
