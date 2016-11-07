#include "dbrl_id_point_2d_storage.h"
#include <vsl/vsl_vector_io.h>
#include <vbl/io/vbl_io_smart_ptr.h>


dbrl_id_point_2d_storage::dbrl_id_point_2d_storage()
    {
        
    }
        
dbrl_id_point_2d_storage::dbrl_id_point_2d_storage(const vcl_vector<dbrl_id_point_2d_sptr> points)
    {
        idpoints_=points;
    }
dbrl_id_point_2d_storage::~dbrl_id_point_2d_storage()
    {
    }
void dbrl_id_point_2d_storage::set_id_points(const vcl_vector<dbrl_id_point_2d_sptr> points)
    {
        idpoints_=points;
    }

bpro1_storage* dbrl_id_point_2d_storage::clone() const
    {
    return new dbrl_id_point_2d_storage(*this);
    }

short dbrl_id_point_2d_storage::version() const
    {
        return 1;
    }
void dbrl_id_point_2d_storage::b_write(vsl_b_ostream &os) const
    {
        vsl_b_write(os, version());
        bpro1_storage::b_write(os);
        vsl_b_write(os, this->idpoints_);
    }

void dbrl_id_point_2d_storage::b_read(vsl_b_istream &is) 
    {
        if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    bpro1_storage::b_read(is);
    vsl_b_read(is, this->idpoints_);


    break;

  default:
    vcl_cerr << "I/O ERROR: dbctrk_storage::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
    }

vcl_vector<dbrl_id_point_2d_sptr> dbrl_id_point_2d_storage::points()
    {
        return idpoints_;
    }
