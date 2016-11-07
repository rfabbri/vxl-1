// This is contrib/fine/dbseg_seg_displayer.h
#ifndef dbseg_seg_displayer_h_
#define dbseg_seg_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau out of dbseg_seg_storage_sptr.
// \author Eli Fine (eli_fine@brown.edu)
// \date 8/1/08 
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>
#include <bvis1/bvis1_mapper.h>

#include <structure/dbseg_seg_storage.h>
#include "dbseg_seg_tableau.h"
#include <structure/dbseg_seg_storage_sptr.h>

#include <vgui/vgui_tableau_sptr.h>



class dbseg_seg_displayer : public bvis1_displayer {

public:
  dbseg_seg_displayer(){}

  //: Return type string "seg"
  virtual vcl_string type() const { return "seg"; }

  //: Create a tableau if the storage object is of type seg
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

};

#endif // bvis1_vsol2D_displayer_h_


