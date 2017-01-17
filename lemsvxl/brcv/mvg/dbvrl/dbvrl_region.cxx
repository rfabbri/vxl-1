// This is algo/dbvrl/dbvrl_world_roi.cxx
//:
// \file

#include "dbvrl_region.h"
#include <vgl/vgl_vector_2d.h>


//: Default Constructor
dbvrl_region::dbvrl_region()
 : ni_(100), nj_(100)
{
}

//: Constructor
dbvrl_region::dbvrl_region( int ni, int nj )
 : ni_(ni), nj_(nj)
{
}


//: Set the size of the world region
void 
dbvrl_region::set_world_size(int ni, int nj)
{
  ni_ = ni;
  nj_ = nj;
}

 
//: Return the widthin world coords
int 
dbvrl_region::ni() const
{
  return ni_;
}


//: Return the height in world coords
int 
dbvrl_region::nj() const
{
  return nj_;
}


//: Returns a vector of four region corners in image coords 
vcl_vector<vgl_point_2d<double> > 
dbvrl_region::corners() const
{
  vcl_vector<vgl_point_2d<double> > corners;
  corners.push_back( vgl_point_2d<double>(0.0, 0.0) );
  corners.push_back( vgl_point_2d<double>(1.0, 0.0) );
  corners.push_back( vgl_point_2d<double>(1.0, 1.0) );
  corners.push_back( vgl_point_2d<double>(0.0, 1.0) );
  return corners;
}


//: Return the transformation from world to image coords
vimt_transform_2d 
dbvrl_region::xform() const
{
  vimt_transform_2d xform;
  xform.set_zoom_only(1.0/ni_, 1.0/nj_, 0.0, 0.0);
  return xform;
}


//: Return IO version number;
short 
dbvrl_region::version() const
{
  return 1;
}


//: Binary save self to stream.
void 
dbvrl_region::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, ni_);
  vsl_b_write(os, nj_);
}


//: Binary load self from stream.
void 
dbvrl_region::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
  {
    vsl_b_read(is, ni_);
    vsl_b_read(is, nj_);
    break;
  }

  default:
    vcl_cerr << "I/O ERROR: dbvrl_region::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}




//: Binary save dbvrl_region to stream.
void
vsl_b_write(vsl_b_ostream &os, const dbvrl_region* r)
{
  if (r==0) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    r->b_write(os);
  }
}


//: Binary load dbvrl_region from stream.
void
vsl_b_read(vsl_b_istream &is, dbvrl_region* &r)
{
  delete r;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    r = new dbvrl_region();
    r->b_read(is);
  }
  else
    r = 0;
}


//: Print an ASCII summary to the stream
void
vsl_print_summary(vcl_ostream &os, const dbvrl_region* r)
{
  os << "dbvrl_region("<<r->ni()<<","<<r->nj()<<")";
}
