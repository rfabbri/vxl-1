// This is brcv/trk/dbmrf/vis/dbmrf_curvel_3d_displayer.h
#ifndef dbmrf_curvel_3d_displayer_h_
#define dbmrf_curvel_3d_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau out of dbmrf_curvel_3d_storage_sptr.
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 3/24/04 
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>


class dbmrf_curvel_3d_displayer : public bvis1_displayer {

public:
  dbmrf_curvel_3d_displayer(){}

  //: Return type string "curvel_3d"
  virtual vcl_string type() const { return "curvel_3d"; }

  //: Create a tableau if the storage object is of type curvel_3d
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

};

#endif // dbmrf_curvel_3d_displayer_h_
