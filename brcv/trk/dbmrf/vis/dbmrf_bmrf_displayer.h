// This is breye1/bvis1/displayer/dbmrf_bmrf_displayer.h
#ifndef dbmrf_bmrf_displayer_h_
#define dbmrf_bmrf_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau out of dbmrf_bmrf_storage_sptr.
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 1/23/04 
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <vcl_map.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>

#include <vgui/vgui_style.h>

// forward declarations
class bmrf_epi_seg;
class bgui_bmrf_epi_seg_soview2D;

class dbmrf_bmrf_displayer : public bvis1_displayer {

public:
  // Constructor
  dbmrf_bmrf_displayer();

  //: Return type string "bmrf"
  virtual vcl_string type() const { return "bmrf"; }

  //: Create a tableau if the storage object is of type bmrf
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

  //: This defines the default style used to draw all epi_segments
  static vgui_style_sptr seg_style;

};


#endif // dbmrf_bmrf_displayer_h_
