// This is brcv/trk/dbctrk/pro/dbctrk_storage.h

//:
// \file

#include <dbctrk/pro/dbctrk_storage.h>
#include <dbctrk/dbctrk_curve_matching.h>

//: Constructor
dbctrk_storage::dbctrk_storage( const vcl_vector<dbctrk_tracker_curve_sptr>& tracked_curves):tracked_curves_ (tracked_curves)
{
}


//: Set the curvetracking storage vector
void
dbctrk_storage::set_tracked_curves( const vcl_vector<dbctrk_tracker_curve_sptr >& tracked_curves)
{
  tracked_curves_=  tracked_curves;
}

//: Retrieve the curvetracking storage vector
void
dbctrk_storage::get_tracked_curves( vcl_vector<dbctrk_tracker_curve_sptr>& tracked_curves) const
{
  tracked_curves=  tracked_curves_;
}


bpro1_storage* 
dbctrk_storage::clone() const
{
  return new dbctrk_storage(*this);
}


//: Return IO version number;
short 
dbctrk_storage::version() const
{
  return 1;
}


//: Binary save self to stream.
void 
dbctrk_storage::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  bpro1_storage::b_write(os);
  vsl_b_write(os, this->tracked_curves_);
}


//: Binary load self from stream.
void 
dbctrk_storage::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    bpro1_storage::b_read(is);
    vsl_b_read(is, this->tracked_curves_);


    break;

  default:
    vcl_cerr << "I/O ERROR: dbctrk_storage::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

