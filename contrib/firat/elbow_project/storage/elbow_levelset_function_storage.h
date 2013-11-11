/*
 * elbow_levelset_function_storage.h
 *
 *  Created on: Feb 24, 2012
 *      Author: firat
 */

#ifndef ELBOW_LEVELSET_FUNCTION_STORAGE_H_
#define ELBOW_LEVELSET_FUNCTION_STORAGE_H_

//:
// \file
// \brief A storage class for levelset segmentation output
// \author Firat Kalaycilar, (firat_kalaycilar@brown.edu)
// \date Oct 11, 2011
//
//
// \verbatim
//  Modifications
// \endverbatim

#include "elbow_levelset_function_storage_sptr.h"

#include <bpro1/bpro1_storage.h>
#include <vil3d/vil3d_image_view.h>

//: Storage class for dbsksp_xgraph
class elbow_levelset_function_storage : public bpro1_storage
{
public:
  // Initialization-------------------------------------------------------------


  //: Destructor
  virtual ~elbow_levelset_function_storage(){}

  // Inherited from bpro1-------------------------------------------------------

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const
  { return "elbow_levelset_function_storage"; }


  // Data access----------------------------------------------------------------
  //: Get image
  vil3d_image_view<double>& phi()
  {return this->phi_; }

  //: Set image
  void set_image(vil3d_image_view<double>& phi)
  { this->phi_ = phi; }

  // Binary IO -----------------------------------------------------------------

  //: Returns the type string
  virtual vcl_string type() const { return "levelset_function"; }

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
  vil3d_image_view<double> phi_;

};


//------------------------------------------------------------------------------
//: Create a smart-pointer to a elbow_levelset_function_storage.
struct elbow_levelset_function_storage_new : public elbow_levelset_function_storage_sptr
{
  typedef elbow_levelset_function_storage_sptr base;

  //: Constructor - creates a elbow_levelset_function_storage_sptr.
  elbow_levelset_function_storage_new(): base(new elbow_levelset_function_storage()) { }
};


#endif /* ELBOW_LEVELSET_FUNCTION_STORAGE_H_ */
