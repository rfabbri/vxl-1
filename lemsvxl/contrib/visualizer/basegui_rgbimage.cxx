#include <basegui_xpgl.h>
#include <wx/wx.h>
#include <wx/image.h>
#include "basegui_rgbimage.h"
#include <iostream>

/////////////////////////////////////////////////////////////
//GrayscaleImage

GrayscaleImage::GrayscaleImage(int width, int height, unsigned char *data) { 
  w = width;
  h = height;
  image = new unsigned char[w*h];
  memcpy(image, data, w*h);
  _dirty = true;
}

GrayscaleImage::GrayscaleImage(int width, int height, unsigned char *data, NO_COPY) {
  w = width;
  h = height;
  image = data;
  _dirty = true;
}

//Convert the given RGBImage to Grayscale
GrayscaleImage::GrayscaleImage(RGBImage* rgbimage)
{
  w = rgbimage->width();
  h = rgbimage->height();
  image = new unsigned char[w*h];

  unsigned char v;
  for (int y=0; y<h; y++)
    for (int x=0; x<w; x++) 
          {
            //
            //GrayScale Value = 0.3*R + 0.59*G + 0.11*B
            //Found this formula on : http://www.dfanning.com/ip_tips/color2gray.html
            //
            v = (unsigned char)(0.3*rgbimage->r(x,y)+0.59*rgbimage->g(x,y)+0.11*rgbimage->b(x,y));
            setPix (x, y, v);
           }
    _dirty = true;
 }

GrayscaleImage::GrayscaleImage(const char *file) {
  wxImage img(file);
  if(!img.Ok()) {
    w = h = 0;
    image = NULL;
    return;
  }

  w = img.GetWidth();
  h = img.GetHeight();
  image = new unsigned char[w*h];
  memcpy(image, img.GetData(), w*h);
  _dirty = true;
}

GrayscaleImage::~GrayscaleImage() {
  delete []image;
}

void GrayscaleImage::write(const char *file, int type) {
  wxImage img(w, h, image, true);
  img.SaveFile(wxString(file), type);
}

unsigned int GrayscaleImage::glFormat() const { return GL_DEPTH; }
unsigned int GrayscaleImage::glType() const { return GL_UNSIGNED_BYTE; }

/////////////////////////////////////////////////////////////
//EdgeImage

EdgeImage::EdgeImage (int width, int height)
  : GrayscaleImage (width, height, NULL, NoCopy)
{
  image = new unsigned char[w*h];
  edgeTangent = new float[w*h];
  _edge_dirty = false;
}

EdgeImage::EdgeImage (int width, int height, 
               unsigned char *newimage, float *newedgetan)
  : GrayscaleImage (width, height, newimage)
{ 
  edgeTangent = new float[w*h];
  memcpy (edgeTangent, newedgetan, w*h*sizeof(float));
  _edge_dirty = true;
}

EdgeImage::EdgeImage(int width, int height, 
              unsigned char *newimage, float *newedgetan, NO_COPY)
   : GrayscaleImage (width, height, newimage)
{
  edgeTangent = newedgetan;
  _dirty = true;
}

EdgeImage::~EdgeImage() 
{
  delete []edgeTangent;
}

/////////////////////////////////////////////////////////////
// RGBImage
RGBImage::RGBImage(int width, int height) { 
  w = width;
  h = height;
  image = new unsigned char[3*w*h];
  memset(image, 255, 3*w*h);
  _dirty = true;
}

RGBImage::RGBImage(int width, int height, unsigned char *data) { 
  w = width;
  h = height;
  image = new unsigned char[3*w*h];
  memcpy(image, data, 3*w*h);
  _dirty = true;
}

RGBImage::RGBImage(int width, int height, unsigned char *data, NO_COPY) {
  w = width;
  h = height;
  image = data;
  _dirty = true;
}

//Slow constructor to copy the grayscale image into the RGB buffer...
RGBImage::RGBImage(GrayscaleImage* grayimage)
{
  w = grayimage->width();
  h = grayimage->height();
  image = new unsigned char[3*w*h];

  for (int y=0; y<h; y++)
    for (int x=0; x<w; x++) {
      setR (x, y, grayimage->pix(x,y));
      setG (x, y, grayimage->pix(x,y));
      setB (x, y, grayimage->pix(x,y));
    }
  _dirty = true;
}

RGBImage::RGBImage(const char *file) {
  wxImage img(file);
  if(!img.Ok()) {
    w = h = 0;
    image = NULL;
    return;
  }

  w = img.GetWidth();
  h = img.GetHeight();

  image = new unsigned char[3*w*h];
  memcpy(image, img.GetData(), 3*w*h);

  _dirty = true;
}

unsigned char * RGBImage::grayData() const
{
  int size = width()*height();

  //GrayScale Value = 0.3*R + 0.59*G + 0.11*B
  unsigned char *gray_data = new unsigned char[size];
  //cout<<" Getting Gray Data. Size = "<<size<<endl;
  int w = width();
  int h = height();

  for (int i=0;i<h;i++){
    for (int j=0;j<w;j++){
      if (!((image[3*(i*w+j)+0]==image[3*(i*w+j)+1])&&((image[3*(i*w+j)+0]==image[3*(i*w+j)+2]))))
        gray_data[i*w+j]=(unsigned char)((image[3*(i*w+j)+0]*0.3+
                               image[3*(i*w+j)+1]*0.59+
                                   image[3*(i*w+j)+2]*0.11));
      else
        gray_data[i*w+j]=(unsigned char)(image[3*(i*w+j)]);
    }
  }
  return gray_data;
}

int RGBImage::convertToGrayScale() 
  {
    //GrayScale Value = 0.3*R + 0.59*G + 0.11*B
    int w = width();
    int h = height();
    
    unsigned char val=0;

    for (int i=0;i<h;i++)
      {
        for (int j=0;j<w;j++)
          {
            if (!((image[3*(i*w+j)+0]==image[3*(i*w+j)+1])&&((image[3*(i*w+j)+0]==image[3*(i*w+j)+2]))))
              {
                val = (unsigned char)((image[3*(i*w+j)+0]*0.3+
                            image[3*(i*w+j)+1]*0.59+
                            image[3*(i*w+j)+2]*0.11));
                
                image[3*(i*w+j)+0]= val;
                image[3*(i*w+j)+1]= val;
                image[3*(i*w+j)+2]= val;
               }
           }
       }

    return 0;
   }

RGBImage::~RGBImage() {
  delete []image;
}

void RGBImage::write(const char *file, int type) {
  wxImage img(w, h, image, true);
  img.SaveFile(wxString(file), type);
}

unsigned int RGBImage::glFormat() const { return GL_RGB; }
unsigned int RGBImage::glType() const { return GL_UNSIGNED_BYTE; }

/////////////////////////////////////////////////////////////
// RGBA


RGBAImage::RGBAImage(int width, int height, unsigned char *data) { 
  w = width;
  h = height;
  image = new unsigned char[4*w*h];
  memcpy(image, data, 4*w*h);
  _dirty = true;
}

RGBAImage::RGBAImage(int width, int height, unsigned char *data, NO_COPY) {
  w = width;
  h = height;
  image = data;
  _dirty = true;
}

RGBAImage::RGBAImage(const char *file) {
  wxImage img(file);
  if(!img.Ok()) {
    w = h = 0;
    image = NULL;
    return;
  }

  w = img.GetWidth();
  h = img.GetHeight();
  image = new unsigned char[4*w*h];

  unsigned char *wxData = img.GetData();
  for(int y = 0; y < h; ++y) {
    for(int x = 0; x < w; ++x) {
      image[4*(w*y + x)  ] = wxData[3*(w*y+x)];
      image[4*(w*y + x)+1] = wxData[3*(w*y+x)+1];
      image[4*(w*y + x)+2] = wxData[3*(w*y+x)+2];
      image[4*(w*y + x)+3] = 128;//255
    }
  }
  _dirty = true;
}

RGBAImage::~RGBAImage() {
  delete []image;
}

unsigned int RGBAImage::glFormat() const { return GL_RGBA; }
unsigned int RGBAImage::glType() const { return GL_UNSIGNED_BYTE; }

void RGBAImage::setMask(Image *img) {
  if(img->width() != w || img->height() != h) {
    std::cerr << "RGBAImage::setMask: image and mask aren't the same size" << std::endl;
    return;
  }

  unsigned char *data = img->data();
  int offset = img->glFormat() == GL_RGBA ? 4 : 3;

  for(int y = 0; y < h; ++y) {
    for(int x = 0; x < w; ++x) {
      int grey =
        (int(data[offset*(w*y+x)+0]) +
        int(data[offset*(w*y+x)+1]) +
        int(data[offset*(w*y+x)+2]))/3;
      image[4*(w*y + x)+3] = grey > 127 ? 255 : 0;
    }
  }
  _dirty = true;
}

