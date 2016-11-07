#pragma once
#include <vsl/vsl_binary_io.h>
#include <dbbgm/bbgm_wavelet_image_traits.h>
/*
//: Binary save self to stream.
template<class image_>
void vsl_b_write(vsl_b_ostream & os, const image_ &p)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  
  int array_rows = p.ni();
  int array_cols = p.nj();
  vsl_b_write(os, array_rows);
  vsl_b_write(os, array_cols);
  for (int i=0; i<array_rows; i++)
  {
    for (int j=0; j<array_cols; j++)
      vsl_b_write(os, p(i,j));
  }
}

//=======================================================================
//: Binary load self from stream.
template<class image_>
void vsl_b_read(vsl_b_istream &is, image_ &p)
{
  if (!is) return;

  short ver;
  int array_rows, array_cols;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
    vsl_b_read(is, array_rows);
    vsl_b_read(is, array_cols);
	{
    image_* tmp =bbgm_wavelet_image_traits<image_>::init(array_rows,array_cols); 
	 for (int i=0; i<array_rows; i++)
    {
      for (int j=0; j<array_cols; j++)
        vsl_b_read(is, (*tmp)(i,j));
    }
	p=*tmp;
	delete tmp;
	}
    break;

   default:
	image_* tmp=0;
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vbl_array_2d<T>&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
} 
 */
