#include <dbetrk/dbetrk_edge.h>
#include <vsl/vsl_binary_io.h>
#include <vgl/io/vgl_io_point_2d.h>


dbetrk_edge::dbetrk_edge()
{
}

dbetrk_edge::~dbetrk_edge()
{
}


void
dbetrk_edge::print_summary(vcl_ostream& os) const
{
  os << "Edge Node: ("<<point_.x()<<","<<point_.y()<<")";
  os << "Gradient : "<<grad_;
  os << "Orientation : "<<theta_;
  os << " P colors "<<pcolor[0]<<" "<<pcolor[1]<<" "<<pcolor[2];
  os << " N colors "<<ncolor[0]<<" "<<ncolor[1]<<" "<<ncolor[2];

}

//: Print a summary of the edge node to a stream
vcl_ostream& operator<< (vcl_ostream& os, dbetrk_edge const & k)
{
  k.print_summary(os);
  return os;
}

short dbetrk_edge::version() const
{
  return 1;
}


//: Binary save self to stream.
void dbetrk_edge::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, this->point_);
  vsl_b_write(os, this->grad_);
  vsl_b_write(os, this->theta_);

}
  
//: Binary load self from stream.
void dbetrk_edge::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
  
    vsl_b_read(is, this->point_);
    vsl_b_read(is, this->grad_);
    vsl_b_read(is, this->theta_);
  
    break;

  default:
    vcl_cerr << "I/O ERROR: dbetrk_edge::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

void vsl_b_write(vsl_b_ostream & os, const dbetrk_edge p)
{
  p.b_write(os);

}
void vsl_b_read(vsl_b_istream &is, dbetrk_edge &p)
{
  p.b_read(is);
}
