// This is brcv/trk/dbinfo/vis/dbinfo_osl_displayer.h
#ifndef dbinfo_osl_displayer_h_
#define dbinfo_osl_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        for generating a tableau out of dbinfo_osl_storage_sptr.
// \author J. L. Mundy
// \date May 11, 2006
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>


class dbinfo_osl_displayer : public bvis1_displayer {

public:
  dbinfo_osl_displayer(){}

  //: Return type string
  virtual vcl_string type() const { return "dbinfo_osl_storage"; }

  //: Create a tableau if the storage object is of type dbinfo_osl_storage
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

  //: Return false, the osl display is global
  virtual bool cacheable() const {return false;}
};

#endif // dbinfo_osl_displayer_h_
