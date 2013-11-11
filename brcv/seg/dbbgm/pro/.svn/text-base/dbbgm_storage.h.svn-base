// This is brcv/trk/dbctrk/dbbgm_storage.h
#ifndef dbbgm_storage_h_
#define dbbgm_storage_h_

//:
// \file
// \brief The vidpro1 storage class for dbbgm
// \author Vishal Jain, (vj@lems.brown.edu)
// \date Sat MAy 4 2005
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_storage.h>
#include <dbbgm/pro/dbbgm_storage_sptr.h>
#include <dbbgm/dbbgm_bgmodel_sptr.h>
#include <vcl_vector.h>

//: Storage class for ctrk 
class dbbgm_storage : public bpro1_storage {
public:

  //: Constructor
  dbbgm_storage(){}
  //: Constructor
  dbbgm_storage( const dbbgm_bgmodel_sptr &bgm);
  //: Destructor
  virtual ~dbbgm_storage(){}

  //: Returns the type string "dbbgm"
  virtual vcl_string type() const { return "dbbgm"; }

  //: Returns the type string "dbbgm"
  virtual vcl_string name() const { return "dbbgm"; }

  //: Set the model
  void set_model(const dbbgm_bgmodel_sptr &bgm);

  //: Retrieve the model
  void get_model(dbbgm_bgmodel_sptr & bgm) const;

  virtual vcl_string is_a() const {return "dbbgm_storage";}

  virtual bpro1_storage* clone() const;

  //: Return IO version number;
  short version() const;

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);
 private:
  //: The vector of  ctrk smart pointers
  dbbgm_bgmodel_sptr bgm_;  
};


//: Create a smart-pointer to a dbbgm_storage.
struct dbbgm_storage_new : public dbbgm_storage_sptr
{
  typedef dbbgm_storage_sptr base;

  //: Constructor - creates a default dbbgm_storage_sptr.
  dbbgm_storage_new() : base(new dbbgm_storage()) { }

  //: Constructor - creates a dbbgm_storage_sptr with ctrks.
  dbbgm_storage_new(const dbbgm_bgmodel_sptr bgm)
   : base(new dbbgm_storage(bgm )) { }
};


#endif //dbbgm_storage_h_
