// This is brcv/seg/dbbgm/pro/dbbgm_storage.h

//:
// \file

#include <dbbgm/pro/dbbgm_storage.h>

//: Constructor
dbbgm_storage::dbbgm_storage( const dbbgm_bgmodel_sptr &bgm):bgm_(bgm)
{
}


//: Set the curvetracking storage vector
void
dbbgm_storage::set_model( const dbbgm_bgmodel_sptr & bgm)
{
  bgm_=  bgm;
}

//: Retrieve the curvetracking storage vector
void
dbbgm_storage::get_model( dbbgm_bgmodel_sptr &bgm) const
{
  bgm=  bgm_;
}


bpro1_storage* 
dbbgm_storage::clone() const
{
  return new dbbgm_storage(*this);
}


//: Return IO version number;
short 
dbbgm_storage::version() const
{
  return 1;
}


//: Binary save self to stream.
void 
dbbgm_storage::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  bpro1_storage::b_write(os);
}


//: Binary load self from stream.
void 
dbbgm_storage::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    bpro1_storage::b_read(is);
    break;

  default:
    vcl_cerr << "I/O ERROR: dbbgm_storage::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

