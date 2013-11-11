#include "dbrl_match_set_storage.h"
#include <vbl/io/vbl_io_smart_ptr.h>
#include <georegister/dbrl_estimator_point_affine.h>
#include <georegister/dbrl_estimator_thin_plate_spline.h>
#include <georegister/dbrl_affine_transformation.h>
#include <georegister/dbrl_thin_plate_spline_transformation.h>
#include <georegister/dbrl_feature_point.h>
#include <vsl/vsl_vector_io.h>

dbrl_match_set_storage::dbrl_match_set_storage()
    {
        
    }
        
dbrl_match_set_storage::dbrl_match_set_storage(const dbrl_match_set_sptr & dms)
    {
        matchset_=dms;
    }
dbrl_match_set_storage::~dbrl_match_set_storage()
    {
    }
void dbrl_match_set_storage::set_match_set(const dbrl_match_set_sptr dms)
    {
        matchset_=dms;
    }

bpro1_storage* dbrl_match_set_storage::clone() const
    {
    return new dbrl_match_set_storage(*this);
    }

short dbrl_match_set_storage::version() const
    {
        return 1;
    }
void dbrl_match_set_storage::b_write(vsl_b_ostream &os) const
    {
    vsl_b_write(os, version());
    bpro1_storage::b_write(os);
    vsl_b_write(os, this->matchset_);
    }

void dbrl_match_set_storage::b_read(vsl_b_istream &is) 
    {
          if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    bpro1_storage::b_read(is);
    vsl_b_read(is, matchset_);

    break;

  default:
    vcl_cerr << "I/O ERROR: dbrl_match_set_storage::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }

    }

dbrl_match_set_sptr dbrl_match_set_storage::matchset()
    {
        return matchset_;
    }

void 
dbrl_match_set_storage::register_binary_io() const {
    vsl_add_to_binary_loader(dbrl_estimator_point_affine());
    vsl_add_to_binary_loader(dbrl_estimator_point_thin_plate_spline());
    vsl_add_to_binary_loader(dbrl_affine_transformation());
    vsl_add_to_binary_loader(dbrl_thin_plate_spline_transformation());
      vsl_add_to_binary_loader(dbrl_feature_point());
   }
