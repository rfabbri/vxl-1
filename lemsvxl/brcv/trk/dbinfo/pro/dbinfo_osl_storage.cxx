// This is brl/blem/brcv/trk/dbinfo/pro/dbinfo_osl_storage.cxx

//:
// \file
#include <vcl_iostream.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <dbinfo/pro/dbinfo_osl_storage.h>
#include <dbinfo/dbinfo_osl.h>

//: Constructor
dbinfo_osl_storage::dbinfo_osl_storage()
{
  osl_ = new dbinfo_osl();
}


//: Destructor
dbinfo_osl_storage::~dbinfo_osl_storage()
{
}


//: Return IO version number;
short 
dbinfo_osl_storage::version() const
{
  return 1;
}


//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* 
dbinfo_osl_storage::clone() const
{
  return new dbinfo_osl_storage(*this);
}


//: Binary save self to stream.
void 
dbinfo_osl_storage::b_write(vsl_b_ostream &os) const
{
  bool flag = true;
  vsl_b_write(os, version());
  bpro1_storage::b_write(os);
  if(osl_)
    {
      vsl_b_write(os, flag);
      vsl_b_write(os, osl_);
    }
  else
    vsl_b_write(os, !flag);
}

//: Binary load self from stream.
void 
dbinfo_osl_storage::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
  {
    bpro1_storage::b_read(is);
    bool flag = false;
    vsl_b_read(is, flag);
    if(flag)
      vsl_b_read(is, osl_);
    break;
  }
  default:
    vcl_cerr << "I/O ERROR: dbinfo_osl_storage::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

