// This is breye1/dbvis1/displayer/dbctrk_displayer.h
#ifndef dbctrk_displayer_h_
#define dbctrk_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau out of dbctrk_storage_sptr.
// \author Vishal Jain (vj@lems.brown.edu)
// \date 11/16/03 
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>
#include <dbctrk/dbctrk_tracker_curve_sptr.h>
#include <dbctrk/dbctrk_tracker_curve.h>

class dbctrk_displayer : public bvis1_displayer {

public:
  dbctrk_displayer(){}

  //: Return type string "trackedcurve"
  virtual vcl_string type() const { return "dbctrk"; }

  //: Create a tableau if the storage object is of type "dbctrk"
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

  //: lookup for curves added to the tableau
  //static vcl_map<dbctrk_tracker_curve_sptr, bgui_bmrf_epi_seg_soview2D*> seg_soview_map_;
 

  
};

#endif // dbctrk_displayer_h_
