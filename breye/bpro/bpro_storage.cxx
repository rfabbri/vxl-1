// This is brl/bpro/bpro_storage.cxx

//:
// \file

#include "bpro_storage.h"


//: Destructor
bpro_storage::~bpro_storage()
{
}


//: Constructor
bpro_storage::bpro_storage()
 : name_("defaultname")
{
}


//: Constructor
bpro_storage::bpro_storage(const vcl_string& name)
 : name_(name)
{
}

//: Set the name of this storage class instance
void 
bpro_storage::set_name(const vcl_string& name)
{
  name_ = name;
}


//: Returns the name of this storage class instance
vcl_string
bpro_storage::name() const
{
  return name_;
}

//: Binary save self to stream.
void 
bpro_storage::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, this->name_);
}


//: Binary load self from stream.
void 
bpro_storage::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    // updated version 1
    vsl_b_read(is, this->name_);
    break;

  default:
    vcl_cerr << "I/O ERROR: bpro_storage::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//: Return IO version number;
short 
bpro_storage::version() const
{
  return 1;
}


//==============================================
//: Allows derived class to be loaded by base-class pointer.
//  A loader object exists which is invoked by calls
//  of the form "vsl_b_read(os,base_ptr);".  This loads derived class
//  objects from the stream, places them on the heap and
//  returns a base class pointer.
//  In order to work the loader object requires
//  an instance of each derived class that might be
//  found.  This function gives the model class to
//  the appropriate loader.
void vsl_add_to_binary_loader(const bpro_storage& b)
{
  vsl_binary_loader<bpro_storage>::instance().add(b);
}


//==============================================
//: Stream summary output for base class pointer
void vsl_print_summary(vcl_ostream& os,const bpro_storage* s)
{
  if (s)
    os << "bpro_storage{type:"<<s->type()<<"}";
  else
    os << "No bpro_storage defined.\n";
}


