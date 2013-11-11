// This is brcv/rec/dber/vis/dber_instance_displayer.h
#ifndef dber_instance_displayer_h_
#define dber_instance_displayer_h_

//:
// \file
// \brief This file defines the displayer class that is responsible
//        to generate a tableau for dber_instance_storage_sptr.
// \author O. C. Ozcanli  adapted from J. Mundy's dbinfo_osl_storage class
// \date Aug 01, 2006
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bvis1/bvis1_displayer.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>

class dber_instance_displayer : public bvis1_displayer {

public:
  dber_instance_displayer(){}

  //: Return type string
  virtual vcl_string type() const { return "dber_instance_storage"; }

  //: Create a tableau if the storage object is of type dber_instance_storage
  virtual vgui_tableau_sptr make_tableau( bpro1_storage_sptr ) const;

  //: Return false, the osl display is global
  virtual bool cacheable() const {return false;}
};

#endif // dber_instance_displayer_h_
