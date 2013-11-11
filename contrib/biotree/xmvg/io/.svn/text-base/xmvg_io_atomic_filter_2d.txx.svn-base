#ifndef XMVG_IO_ATOMIC_FILTER_2D_TXX_
#define XMVG_IO_ATOMIC_FILTER_2D_TXX_

#include "xmvg_io_atomic_filter_2d.h"
#include <vsl/vsl_binary_io.h>
#include <vbl/io/vbl_io_array_2d.h>
#include <vsl/vsl_vector_io.txx>

//SHOULDN'T THIS INCLUDE <vcl_iostream.h>?  PNK 4/23/05

template<class T>
void vsl_b_write(vsl_b_ostream & os, const xmvg_atomic_filter_2d<T> &f)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);

  vnl_int_2 size = f.size();

  int sx = size[0], sy = size[1];
  
  vnl_int_2 loc = f.location();

  int ox = loc[0], oy = loc[1];
  
  vsl_b_write(os, sx);

  vsl_b_write(os, sy);

  vsl_b_write(os, ox);

  vsl_b_write(os, oy);

  vbl_array_2d<T> v = f.weights();
  vsl_b_write(os, v);
  
}

template<class T>
void vsl_b_read(vsl_b_istream & is, xmvg_atomic_filter_2d<T> &f)
{
  if(!is) return;

  short ver;

  int sx, sy, ox, oy;
  
  vnl_int_2 size, loc;
  
  vsl_b_read(is, ver);

  vbl_array_2d<T> v;

  switch (ver)
  {
    case 1:
      vsl_b_read(is, sx);
      vsl_b_read(is, sy);
      vsl_b_read(is, ox);
      vsl_b_read(is, oy);
      
      vsl_b_read(is, v);

      size[0] = sx; 

      size[1] = sy; 

      loc[0] = ox;

      loc[1] = oy;

      f.set_size(size);

      f.set_location(loc);

      f.set_weights(v);

      break;//added by pnk, 4/25/2005

    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, xmvg_atomic_filter_2d<T>&)\n"
             << "           Unknown version number "<< ver << '\n';
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
      
}

template<class T>
void vsl_print_summary(vcl_ostream & os,const xmvg_atomic_filter_2d<T> & p)
{
  os << "Offset: " << p.location() << vcl_endl
     << "Size: " << p.size() << vcl_endl;

  vbl_array_2d<T> v = p.weights();
  
  vsl_print_summary(os, v);
}

#define XMVG_IO_ATOMIC_FILTER_2D_INSTANTIATE(T) \
template void vsl_b_write(vsl_b_ostream & , const xmvg_atomic_filter_2d<T> &); \
template void vsl_b_read(vsl_b_istream & , xmvg_atomic_filter_2d<T> &); \
template void vsl_print_summary(vcl_ostream & , const xmvg_atomic_filter_2d<T> & );

#endif

