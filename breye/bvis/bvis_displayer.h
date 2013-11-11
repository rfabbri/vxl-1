// This is breye/bvis/bvis_displayer.h
#ifndef bvis_displayer_h_
#define bvis_displayer_h_

//:
// \file
// \brief This file defines the abstract base class for bvis displayers.
//        A displayer takes a bpro_storage_sptr and creates the appropriate
//        vgui_tableau_sptr for display.
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 10/30/03 
//
// \verbatim
//  Modifications
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vcl_string.h>

#include <vgui/vgui_tableau_sptr.h>
#include <bpro/bpro_storage_sptr.h>
#include <bvis/bvis_mapper_sptr.h>

class bvis_displayer : public vbl_ref_count{

public:
  bvis_displayer(): mapper_(0){}
  //: Abstract function to return a string description of the type
  virtual vcl_string type() const = 0;

  //: Abstract function to generate a tableau from a storage object
  virtual vgui_tableau_sptr make_tableau( bpro_storage_sptr ) const = 0;

  //: Abstract function to update a tableau from a storage object
  virtual bool update_tableau( vgui_tableau_sptr, const bpro_storage_sptr& ) const { return false; }

  //: Return true if this displayer produces tableau that are safe to cache
  virtual bool cacheable() const { return true; }

  //: Designate a mapping for storage attributes
  void set_mapper(bvis_mapper_sptr const& mapper){mapper_ = mapper;}

 protected:
  bvis_mapper_sptr mapper_;
};

#endif // bvis_displayer_h_
