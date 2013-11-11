// This is breye1/vidpro1/storage/dbetrk_storage.h

//:
// \file

#include "dbetrk/pro/dbetrk_storage.h"
#include <dbetrk/dbetrk_edge.h>

//: Constructor
dbetrk_storage::dbetrk_storage( const vcl_vector<dbetrk_edge_sptr>& dbetrk_edges):dbetrk_edges_ (dbetrk_edges)
{
}


//: Set the edgetracking storage vector
void
dbetrk_storage::set_dbetrk_edges( const vcl_vector<dbetrk_edge_sptr >& dbetrk_edges)
{
  dbetrk_edges_=  dbetrk_edges;
}

//: Retrieve the edgetracking storage vector
void
dbetrk_storage::get_dbetrk_edges( vcl_vector<dbetrk_edge_sptr>& dbetrk_edges) const
{
  dbetrk_edges=  dbetrk_edges_;
}


bpro1_storage* 
dbetrk_storage::clone() const
{
  return new dbetrk_storage(*this);
}


//: Return IO version number;
short 
dbetrk_storage::version() const
{
  return 1;
}


//: Binary save self to stream.
void 
dbetrk_storage::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  bpro1_storage::b_write(os);
 // vsl_b_write(os, this->dbetrk_edges_);
}


//: Binary load self from stream.
void 
dbetrk_storage::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    bpro1_storage::b_read(is);
//    vsl_b_read(is, this->dbetrk_edges_);


    break;

  default:
    vcl_cerr << "I/O ERROR: vidpro1_ctrk_storage::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

