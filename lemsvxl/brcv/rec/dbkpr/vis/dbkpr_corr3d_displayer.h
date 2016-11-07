// This is brcv/rec/dbkpr/vis/dbkpr_corr3d_displayer.h
#ifndef dbkpr_corr3d_displayer_h_
#define dbkpr_corr3d_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau out of dbkpr_corr3d_storage_sptr.
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 4/26/05 
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>


class dbkpr_corr3d_displayer : public bvis1_displayer {

public:
  dbkpr_corr3d_displayer(){}

  //: Return type string "keypoints_corr3d"
  virtual vcl_string type() const { return "keypoints_corr3d"; }

  //: Create a tableau if the storage object is of type keypoints_corr3d
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

};

#endif // dbkpr_corr3d_displayer_h_
