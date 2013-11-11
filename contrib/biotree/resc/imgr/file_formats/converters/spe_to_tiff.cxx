//: 
// \file  spe_to_tiff.cxx
// \brief  a convert to converting from spe to tiff
// \author    Kongbin Kang
// \date        2006-06-07
// 

#include <vil/vil_stream_fstream.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vcl_iostream.h>
#include <vcl_limits.h>

float read_little_endian_float(vil_stream* vs)
{
  union
  {
    float f;
    vxl_byte bytes[4];
  } buf;
  
  if(vs->read(buf.bytes,sizeof buf.bytes) != sizeof buf.bytes) 
    return 0;

#if VXL_BIG_ENDIAN 
      vcl_swap(buf.bytes[0], buf.bytes[3]);
      vcl_swap(buf.bytes[1], buf.bytes[2]);
#endif

  return buf.f;
}

// (byte) type @ 108 
// 0: float, 1: long, 2:int, 3:usigned, 
// 4:string/byte, 5:double, 6:byte, 7:u_byte
unsigned pixel_type(vil_stream_fstream *is)
{
  is->seek(108);
  vxl_byte buf;  
  is->read(&buf, 1);

  return static_cast<unsigned>(buf);
}

// (ishort) width @ 42
unsigned short x_dim(vil_stream_fstream *is)
{
  is->seek(42);
  unsigned short xdim;  
  is->read(&xdim, sizeof(xdim));

  return xdim;
}

// (ishort) heigh @ 656
unsigned short y_dim(vil_stream_fstream *is)
{
  is->seek(656);
  unsigned short ydim;  
  is->read(&ydim, sizeof(ydim));

  return ydim;
}


int main(int argc, char ** argv)
{

  if(argc < 3)
  {
    vcl_cout << "usage: "<<argv[0] << " input.spe output\n";
    exit(1);
  }

  vil_stream_fstream *spe = new vil_stream_fstream(argv[1], "r");


  if(!spe->ok())
  {
    vcl_cout << "opening file fails\n";
    exit(2);
  }

  unsigned type = pixel_type(spe);
  unsigned xdim = x_dim(spe);
  unsigned ydim = y_dim(spe);

  vcl_cout << "data type: " << type << " size (" << xdim <<", "<< ydim << ")\n";
  
  vil_image_view<vxl_uint_16> image(xdim, ydim, 1);

 
  vil_image_view<vxl_uint_16>::iterator it ;
  
  vil_image_view<vxl_uint_16>::const_iterator end_it = image.end();
  
  // offset of the image data
  spe->seek(4100);

  float * data = new float[xdim*ydim];
  
  float min_value=vcl_numeric_limits<float>::max();
  
  float max_value=vcl_numeric_limits<float>::min();
  
  for(unsigned long pos = 0; pos < xdim*ydim; pos ++ )
  {
    data[pos] = read_little_endian_float(spe);
    if(data[pos] < min_value)
      min_value = data[pos];

    if(data[pos] > max_value)
      max_value = data[pos];
  }
 
  unsigned long pos = 0;
  for(it = image.begin(); it != end_it; it++)
  {
    switch(type)
    {
      case 0:
        *it = static_cast<unsigned short>((data[pos++]-min_value)/(max_value -min_value)*65000); 

        break;

      default:
        vcl_cout << "pixel format is not implemented yet\n";
        break;
    }


  }
  
  
  vil_save(image, argv[2]);

  spe->unref();
  delete data;
  
  return 0;

}
