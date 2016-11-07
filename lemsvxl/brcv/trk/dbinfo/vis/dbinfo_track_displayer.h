// This is brcv/trk/dbinfo/vis/dbinfo_track_displayer.h
#ifndef dbinfo_track_displayer_h_
#define dbinfo_track_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau out of dbinfo_track_storage_sptr.
// \author J. L. Mundy
// \date May 22, 2005
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>


class dbinfo_track_displayer : public bvis1_displayer {

public:
  dbinfo_track_displayer(){}

  //: Return type string
  virtual vcl_string type() const { return "dbinfo_track_storage"; }

  //: Create a tableau if the storage object is of type dbinfo
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

  //: Return false, the track display should be updated on every frame
  virtual bool cacheable() const {return false;}
};

#endif // dbinfo_track_displayer_h_
