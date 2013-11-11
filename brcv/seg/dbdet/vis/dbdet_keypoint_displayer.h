// This is brcv/seg/dbdet/vis/dbdet_keypoint_displayer.h
#ifndef dbdet_keypoint_displayer_h_
#define dbdet_keypoint_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau out of dbdet_keypoint_storage_sptr.
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 11/16/03 
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>


class dbdet_keypoint_displayer : public bvis1_displayer {

public:
  dbdet_keypoint_displayer(){}

  //: Return type string "keypoints"
  virtual vcl_string type() const { return "keypoints"; }

  //: Create a tableau if the storage object is of type "keypoints"
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

};

#endif // dbdet_keypoint_displayer_h_
