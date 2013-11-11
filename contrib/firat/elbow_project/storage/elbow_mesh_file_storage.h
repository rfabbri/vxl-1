/*
 * elbow_filename_storage.h
 *
 *  Created on: Oct 23, 2011
 *      Author: firat
 */

#ifndef ELBOW_FILENAME_STORAGE_H_
#define ELBOW_FILENAME_STORAGE_H_

#include "elbow_mesh_file_storage_sptr.h"

#include <bpro1/bpro1_storage.h>
#include <vcl_string.h>
#include <vcl_vector.h>

//: Storage class for dbsksp_xgraph
class elbow_mesh_file_storage : public bpro1_storage
{
public:
  // Initialization-------------------------------------------------------------


  //: Destructor
  virtual ~elbow_mesh_file_storage(){}

  // Inherited from bpro1-------------------------------------------------------

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const
  { return "elbow_filename_storage"; }


  // Data access----------------------------------------------------------------
  //: Get image
  vcl_vector<vcl_string>& files()
  {return this->mesh_files_; }

  vcl_string& folder()
  {
	  return this->folder_;
  }

  //: Set image
  void add_file(vcl_string& filename)
  { this->mesh_files_.push_back(filename); }

  void set_folder(vcl_string& folder)
    { this->folder_ = folder; }

  // Binary IO -----------------------------------------------------------------

  //: Returns the type string
  virtual vcl_string type() const { return "mesh_file"; }

  //: Return IO version number;
  short version() const;

  //: Binary save self to stream.
  // Need rewrite
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  // Need rewrite
  void b_read(vsl_b_istream &is);

protected:
  //: filename
  vcl_vector<vcl_string> mesh_files_;
  vcl_string folder_;
};


//------------------------------------------------------------------------------
//: Create a smart-pointer to a elbow_filename_storage.
struct elbow_mesh_file_storage_new : public elbow_mesh_file_storage_sptr
{
  typedef elbow_mesh_file_storage_sptr base;

  //: Constructor - creates a elbow_filename_storage_sptr.
  elbow_mesh_file_storage_new(): base(new elbow_mesh_file_storage()) { }
};



#endif /* ELBOW_FILENAME_STORAGE_H_ */
