// This is brl/bpro/bpro_storage.h
#ifndef bpro_storage_h_
#define bpro_storage_h_

//:
// \file
// \brief The base storage class
// \author Mark Johnson (mrj@lems.brown.edu)
// \date 7/15/03
//
//
// \verbatim
//  Modifications
//    Matt Leotta  12/15/04  Migrated from vidpro
//    Edu Almeida  04/25/07  Removed frame dependency
//    Edu Almeida  04/25/07  Binary I/O: updated version 1
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vcl_string.h>
#include <vsl/vsl_fwd.h>
#include <vsl/vsl_binary_loader.h>

class bpro_storage : public vbl_ref_count
{
public:
  //: Destructor
  virtual ~bpro_storage();

  //: Return a string that indicates the type of data
  virtual vcl_string type() const = 0;

  //: Returns the name of this storage class instance
  vcl_string name() const;
  //: Set the name of this storage class instance
  void set_name(const vcl_string& name);
  
  //: Implement this function if your storage class loads
  //  objects by base class pointer
  virtual void register_binary_io() const {}

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const = 0;
 
  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro_storage* clone() const = 0;

  //: No primary storage instance is involved
  virtual bpro_storage* merge(const bpro_storage* sa,
                              const bpro_storage* sb)
    {return 0;} //for now, but should be implemented on all storage types
  
  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(vcl_ostream &os) const;
  

protected:
  //: Constructor
  bpro_storage();
  //: Constructor
  bpro_storage(const vcl_string& name);

private:
  //: The name of this instance
  vcl_string name_;

};


//: Allows derived class to be loaded by base-class pointer
//  A loader object exists which is invoked by calls
//  of the form "vsl_b_read(os,base_ptr)".  This loads derived class
//  objects from the disk, places them on the heap and
//  returns a base class pointer.
//  In order to work the loader object requires
//  an instance of each derived class that might be
//  found.  This function gives the model class to
//  the appropriate loader.
void vsl_add_to_binary_loader(const bpro_storage& b);


//: Print an ASCII summary to the stream
void vsl_print_summary(vcl_ostream &os, const bpro_storage* n);

#endif // bpro_storage_h_
