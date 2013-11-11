/*
 * elbow_vil3d_storage.h
 *
 *  Created on: Oct 11, 2011
 *      Author: firat
 */

#ifndef ELBOW_VIL3D_STORAGE_H_
#define ELBOW_VIL3D_STORAGE_H_

//:
// \file
// \brief A storage class for vil3d image view
// \author Firat Kalaycilar, (firat_kalaycilar@brown.edu)
// \date Oct 11, 2011
//
//
// \verbatim
//  Modifications
// \endverbatim

#include "elbow_vil3d_storage_sptr.h"

#include <bpro1/bpro1_storage.h>
#include <vil3d/vil3d_image_view.h>

//: Storage class for dbsksp_xgraph
class elbow_vil3d_storage : public bpro1_storage
{
public:
  // Initialization-------------------------------------------------------------


  //: Destructor
  virtual ~elbow_vil3d_storage(){}

  // Inherited from bpro1-------------------------------------------------------

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const
  { return "elbow_vil3d_storage"; }


  // Data access----------------------------------------------------------------
  //: Get image
  vil3d_image_view<double>& image()
  {return this->image_; }

  //: Set image
  void set_image(vil3d_image_view<double>& image)
  { this->image_ = image; }

  // Binary IO -----------------------------------------------------------------

  //: Returns the type string
  virtual vcl_string type() const { return "3d_dataset"; }

  //: Return IO version number;
  short version() const;

  //: Binary save self to stream.
  // Need rewrite
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  // Need rewrite
  void b_read(vsl_b_istream &is);

protected:
  //: 3d Image
  vil3d_image_view<double> image_;

};


//------------------------------------------------------------------------------
//: Create a smart-pointer to a elbow_vil3d_storage.
struct elbow_vil3d_storage_new : public elbow_vil3d_storage_sptr
{
  typedef elbow_vil3d_storage_sptr base;

  //: Constructor - creates a elbow_vil3d_storage_sptr.
  elbow_vil3d_storage_new(): base(new elbow_vil3d_storage()) { }
};


#endif /* ELBOW_VIL3D_STORAGE_H_ */
