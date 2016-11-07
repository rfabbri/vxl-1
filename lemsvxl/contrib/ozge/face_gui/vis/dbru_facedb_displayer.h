// This is brcv/rec/dbru/vis/dbru_facedb_displayer.h
#ifndef dbru_facedb_displayer_h_
#define dbru_facedb_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau out of dbru_facedb_storage_sptr.
// \author O. C. Ozcanli  adapted from J. Mundy's dbinfo_facedb_storage class
// \date Aug 01, 2006
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>

class dbru_facedb_displayer : public bvis1_displayer {

public:
  dbru_facedb_displayer(){}

  //: Return type string
  virtual vcl_string type() const { return "dbru_facedb_storage"; }

  //: Create a tableau if the storage object is of type dbru_facedb_storage
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

  //: Return false, the facedb display is global
  virtual bool cacheable() const {return false;}
};

#endif // dbru_facedb_displayer_h_
