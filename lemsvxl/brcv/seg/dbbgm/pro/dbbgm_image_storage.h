// This is brcv/seg/dbbgm/pro/dbbgm_image_storage.h
#ifndef dbbgm_image_storage_h_
#define dbbgm_image_storage_h_

//:
// \file
// \brief The vidpro1 storage class for dbbgm_image
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 1/27/06
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_storage.h>
#include <dbbgm/pro/dbbgm_image_storage_sptr.h>
#include <bbgm/bbgm_image_sptr.h>
#include <vcl_vector.h>

//: Storage class for bbgm_image_sptr
class dbbgm_image_storage : public bpro1_storage {
public:

  //: Constructor
  dbbgm_image_storage() : dimg_(NULL) {}
  //: Constructor
  dbbgm_image_storage( const bbgm_image_sptr& dimg);
  //: Destructor
  virtual ~dbbgm_image_storage(){}

  virtual void register_binary_io() const ;


  //: Returns the type string "dbbgm_image"
  virtual vcl_string type() const { return "dbbgm_image"; }

  //: Returns the type string "dbbgm_image"
  virtual vcl_string name() const { return "dbbgm_image"; }

  //: Set the distribution image
  void set_dist_image(const bbgm_image_sptr &dimg) { dimg_ = dimg; }

  //: Retrieve the distribution image
  bbgm_image_sptr dist_image() const { return dimg_; }

  virtual vcl_string is_a() const {return "dbbgm_image_storage";}

  virtual bpro1_storage* clone() const;

  //: Return IO version number;
  short version() const;

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);
 private:
  //: A smart pointer to a base distribution image
  bbgm_image_sptr dimg_;
};


//: Create a smart-pointer to a dbbgm_image_storage.
struct dbbgm_image_storage_new : public dbbgm_image_storage_sptr
{
  typedef dbbgm_image_storage_sptr base;

  //: Constructor - creates a default dbbgm_image_storage_sptr.
  dbbgm_image_storage_new() : base(new dbbgm_image_storage()) { }

  //: Constructor - creates a dbbgm_image_storage_sptr with data.
  dbbgm_image_storage_new(const bbgm_image_sptr& dimg)
   : base(new dbbgm_image_storage(dimg )) { }
};


#endif //dbbgm_image_storage_h_
