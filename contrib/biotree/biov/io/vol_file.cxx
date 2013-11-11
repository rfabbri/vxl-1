// This is contrib/biotree/biov/io/vol_file.cpp

#include "vol_file.h"
#if VXL_WIN32 
#include <vil/vil_stream_fstream_64.h>
#endif
#include <vil/vil_stream_fstream.h>
#include <vil/vil_stream_write.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>

#if MY_BIG_ENDIAN
void write_big_endian_uint_32(vil_stream *s, vxl_uint_32 w) {s->write((char *) (&w),4);}
void write_big_endian_float( vil_stream *s, float f ) {s->write((char *) (&f),4);}

#else
void write_big_endian_uint_32(vil_stream *s, vxl_uint_32 w)
{
  vxl_byte bytes[4];
  bytes[0] = w >> 24;
  bytes[1] = w >> 16;
  bytes[2] = w >> 8;
  bytes[3] = w >> 0;
  s->write(bytes, 4);
}

void write_big_endian_float( vil_stream *s, float f )
{
  union
  {
    float f;
    vxl_byte b[4];
  } dat1, dat2;

  dat1.f = f;
  dat2.b[0] = dat1.b[3];
  dat2.b[1] = dat1.b[2];
  dat2.b[2] = dat1.b[1];
  dat2.b[3] = dat1.b[0];
  s->write((char *) (&dat2.f),4);
}

#endif

bool vol_file :: write (const char *filename) {

   assert (sizeof(float)==4);
   
// open file in binary mode and zero the file if it has not been already
  vil_stream* os = new vil_stream_fstream(filename, "w+b");

  if(!os->ok()){
    vcl_cerr << "cannot open file to write \n";
    exit(1);
  }

// output the header

   os->write((char *) (&magic_number_),4);
   write_big_endian_uint_32(os,header_length_);
   write_big_endian_uint_32(os,width_);
   write_big_endian_uint_32(os,height_);
   write_big_endian_uint_32(os,images_);
   write_big_endian_uint_32(os,bits_per_voxel_);
   write_big_endian_uint_32(os,index_bits_);
   write_big_endian_float(os,scaleX_);
   write_big_endian_float(os,scaleY_);
   write_big_endian_float(os,scaleZ_);
   write_big_endian_float(os,rotX_);
   write_big_endian_float(os,rotY_);
   write_big_endian_float(os,rotZ_);
   for (unsigned q = 0; q<data_.size(); q++)
      os->write((char *) (&data_[q]),1);

   os->unref();

return true;
}

