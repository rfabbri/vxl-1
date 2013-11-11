// This is brcv/seg/dbbgm/pro/dbbgm_distribution_image_storage.h
#ifndef dbbgm_distribution_image_storage_h_
#define dbbgm_distribution_image_storage_h_

//:
// \file
// \brief The vidpro1 storage class for dbbgm_distribution_image
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 7/22/05
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_storage.h>
#include <dbbgm/pro/dbbgm_distribution_image_storage_sptr.h>
#include <dbbgm/dbbgm_distribution_image_sptr.h>
#include <vcl_vector.h>
#include <vbl/io/vbl_io_smart_ptr.h>

//: Storage class for ctrk 
class dbbgm_distribution_image_storage : public bpro1_storage {
public:

  //: Constructor
  dbbgm_distribution_image_storage(){}
  //: Constructor
  dbbgm_distribution_image_storage( const dbbgm_distribution_image_sptr &dimg);
  //: Destructor
  virtual ~dbbgm_distribution_image_storage(){}

  virtual void register_binary_io() const ;


  //: Returns the type string "dbbgm_distribution_image"
  virtual vcl_string type() const { return "dbbgm_distribution_image"; }

  //: Returns the type string "dbbgm_distribution_image"
  virtual vcl_string name() const { return "dbbgm_distribution_image"; }

  //: Set the distribution image
  void set_dist_image(const dbbgm_distribution_image_sptr &dimg) { dimg_ = dimg; }

  //: Retrieve the distribution image
  dbbgm_distribution_image_sptr dist_image() const { return dimg_; }

  virtual vcl_string is_a() const {return "dbbgm_distribution_image_storage";}

  virtual bpro1_storage* clone() const;

  //: Return IO version number;
  short version() const;
  
  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);
 private:
  //: A smart pointer to a base distribution image
  dbbgm_distribution_image_sptr dimg_;  
};


//: Create a smart-pointer to a dbbgm_distribution_image_storage.
struct dbbgm_distribution_image_storage_new : public dbbgm_distribution_image_storage_sptr
{
  typedef dbbgm_distribution_image_storage_sptr base;

  //: Constructor - creates a default dbbgm_distribution_image_storage_sptr.
  dbbgm_distribution_image_storage_new() : base(new dbbgm_distribution_image_storage()) { }

  //: Constructor - creates a dbbgm_distribution_image_storage_sptr with data.
  dbbgm_distribution_image_storage_new(const dbbgm_distribution_image_sptr& dimg)
   : base(new dbbgm_distribution_image_storage(dimg )) { }
};


#endif //dbbgm_distribution_image_storage_h_
