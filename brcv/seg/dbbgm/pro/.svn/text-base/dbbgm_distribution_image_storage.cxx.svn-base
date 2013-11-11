// This is brcv/seg/dbbgm/pro/dbbgm_distribution_image_storage.cxx

//:
// \file

#include <dbbgm/pro/dbbgm_distribution_image_storage.h>
#include <dbbgm/dbbgm_distribution_image.h>
#include <dbsta/dbsta_distribution.h>
#include <dbsta/dbsta_gaussian_indep.h>
#include <dbsta/dbsta_gaussian_full.h>
#include <dbsta/dbsta_gaussian_sphere.h>
#include <dbsta/dbsta_mixture.h>

//: Constructor
dbbgm_distribution_image_storage::dbbgm_distribution_image_storage( const dbbgm_distribution_image_sptr& dimg)
 : dimg_(dimg)
{
}


bpro1_storage* 
dbbgm_distribution_image_storage::clone() const
{
  return new dbbgm_distribution_image_storage(*this);
}


//: Return IO version number;
short 
dbbgm_distribution_image_storage::version() const
{
  return 1;
}


//: Binary save self to stream.
void 
dbbgm_distribution_image_storage::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  bpro1_storage::b_write(os);
  vsl_b_write(os, this->dimg_);

}


//: Binary load self from stream.
void 
dbbgm_distribution_image_storage::b_read(vsl_b_istream &is)
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
    vcl_cerr << "I/O ERROR: dbbgm_distribution_image_storage::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


void 
dbbgm_distribution_image_storage::register_binary_io() const {

        vsl_add_to_binary_loader(dbbgm_distribution_image<float>());
        vsl_add_to_binary_loader(dbbgm_distribution_image<double>());


        vsl_add_to_binary_loader(dbsta_gaussian_full<double,3>());
        vsl_add_to_binary_loader(dbsta_gaussian_full<float,3>());

        vsl_add_to_binary_loader(dbsta_gaussian_indep<double,3>());
        vsl_add_to_binary_loader(dbsta_gaussian_indep<double,1>());
        vsl_add_to_binary_loader(dbsta_gaussian_indep<float,3>());
        vsl_add_to_binary_loader(dbsta_gaussian_indep<float,2>());
        vsl_add_to_binary_loader(dbsta_gaussian_indep<float,1>());
        vsl_add_to_binary_loader(dbsta_gaussian_sphere<double,3>());
        vsl_add_to_binary_loader(dbsta_gaussian_sphere<float,3>());
        vsl_add_to_binary_loader(dbsta_mixture<float>());
        vsl_add_to_binary_loader(dbsta_mixture<double>());
    }
