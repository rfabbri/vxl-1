// This is brcv/seg/dbbgm/pro/dbbgm_image_storage.cxx

//:
// \file

#include <dbbgm/pro/dbbgm_image_storage.h>
#include <bbgm/bbgm_image_of.h>
#include <vbl/io/vbl_io_smart_ptr.h>

#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_gauss_sf1.h>
//: Constructor
dbbgm_image_storage::dbbgm_image_storage( const bbgm_image_sptr& dimg )
 : dimg_(dimg)
{
}


bpro1_storage* 
dbbgm_image_storage::clone() const
{
  return new dbbgm_image_storage(*this);
}


//: Return IO version number;
short 
dbbgm_image_storage::version() const
{
  return 1;
}


//: Binary save self to stream.
void 
dbbgm_image_storage::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  bpro1_storage::b_write(os);
  vsl_b_write(os, this->dimg_);

}


//: Binary load self from stream.
void 
dbbgm_image_storage::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    bpro1_storage::b_read(is);
    vsl_b_read(is, dimg_);

    break;

  default:
    vcl_cerr << "I/O ERROR: dbbgm_image_storage::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


void 
dbbgm_image_storage::register_binary_io() const
{
  typedef bsta_num_obs<bsta_gauss_if3> gauss_type;
  typedef bsta_num_obs<bsta_mixture<gauss_type> > mix_gauss_type;
  vsl_add_to_binary_loader(bbgm_image_of<mix_gauss_type>());

  typedef bsta_num_obs<bsta_gauss_sf1> sph_gauss_type;
  typedef bsta_num_obs<bsta_mixture<sph_gauss_type> > sph_mix_gauss_type;
  vsl_add_to_binary_loader(bbgm_image_of<sph_mix_gauss_type>());
}

