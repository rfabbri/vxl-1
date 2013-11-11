#ifndef RGB_IMAGE_H_INCLUDED
#define RGB_IMAGE_H_INCLUDED

#include <assert.h>

class Image {
public:
  virtual ~Image() {}

  virtual int height() const = 0;
  virtual int width() const = 0;
  virtual unsigned int glFormat() const = 0;
  virtual unsigned int glType() const = 0;
  virtual bool dirty() const = 0;
  virtual void setDirty(bool d=true) = 0;

  virtual unsigned char* data() = 0;
  virtual const unsigned char* data() const = 0;
};

class RGBImage;

class GrayscaleImage : public Image {
protected:
  int w, h;
  unsigned char *image;
  bool _dirty;

public:
  enum NO_COPY { NoCopy };
  GrayscaleImage(int width, int height, unsigned char *data);
  GrayscaleImage(int width, int height, unsigned char *data, NO_COPY);
    // with NoCopy, a new buffer will not be allocated, only the pointer
    // will be copied. it must be allocated by new[]
  GrayscaleImage(RGBImage* rgbimage);
  GrayscaleImage(const char *file);
  ~GrayscaleImage();

  unsigned char *data() { return image; }
  const unsigned char *data() const { return image; }

  virtual bool dirty() const { return _dirty; }
  virtual void setDirty(bool d=true) { _dirty = d; }

  unsigned int glFormat() const;
  unsigned int glType() const;

  int height() const { return h; }
  int width() const { return w; }

  unsigned char pix(int x, int y) const { 
    assert(x >= 0 && x < w && y >= 0 && y < h); 
    return image[w*y + x];   
  }

  void setPix(int x, int y, unsigned char v) 
  { 
    assert(x >= 0 && x < w && y >= 0 && y < h); 
    _dirty = true; 
    image[w*y + x] = v;  
  }
  // type can be wxBITMAP_TYPE_BMP, wxBITMAP_TYPE_JPEG, wxBITMAP_TYPE_PNG, wxBITMAP_TYPE_PCX,
  // wxBITMAP_TYPE_PNM, or wxBITMAP_TYPE_XPM 
  // tiff & gifs are broken because of patent issues
  void write(const char *file, int type);
};

class EdgeImage : public GrayscaleImage {
private:
  float* edgeTangent;
  bool _edge_dirty;
public:
  EdgeImage (int width, int height);
  EdgeImage (int width, int height, unsigned char *newimage, float *newedgetan);
  EdgeImage (int width, int height, unsigned char *newimage, float *newedgetan, NO_COPY);
  ~EdgeImage ();

  float tan (int x, int y) const
  { 
    assert(x >= 0 && x < w && y >= 0 && y < h);
        //NOTE: Casted the return type to int. But why ?????
        return edgeTangent[w*y + x]; //removed the stupid casting
  }

  void setTan (int x, int y, float newtan)
  {
    assert(x >= 0 && x < w && y >= 0 && y < h); 
    _edge_dirty = true;
    edgeTangent[w*y + x] = newtan;  
  }
};

class RGBImage : public Image {
private:
  int w, h;
  unsigned char *image;
  bool _dirty;

public:
  enum NO_COPY { NoCopy };
  RGBImage(int width, int height);
  RGBImage(int width, int height, unsigned char *data);
  RGBImage(int width, int height, unsigned char *data, NO_COPY);
    // with NoCopy, a new buffer will not be allocated, only the pointer
    // will be copied. it must be allocated by new[]
  RGBImage(GrayscaleImage* grayimage);

  RGBImage(const char *file);
  ~RGBImage();

  unsigned char *data() { return image; }
  const unsigned char *data() const { return image; }

  virtual bool dirty() const { return _dirty; }
  virtual void setDirty(bool d=true) { _dirty = d; }

  unsigned int glFormat() const;
  unsigned int glType() const;

  int height() const { return h; }
  int width() const { return w; }

  unsigned char r(int x, int y) const { assert(x >= 0 && x < w && y >= 0 && y < h); return image[3*(w*y + x)];   }
   unsigned char g(int x, int y) const { assert(x >= 0 && x < w && y >= 0 && y < h); return image[3*(w*y + x)+1]; }
  unsigned char b(int x, int y) const { assert(x >= 0 && x < w && y >= 0 && y < h); return image[3*(w*y + x)+2]; }

    unsigned char paddedR(int x, int y) const 
      {
        if ((x >= 0)&&(x < w)&&(y >= 0)&&(y < h))
          {
            return image[3*(w*y + x)];
           }
        else
          {
            return 0;
           }
        return 0;
       };
  
    unsigned char paddedG(int x, int y) const 
      {
        if ((x >= 0)&&(x < w)&&(y >= 0)&&(y < h))
          {
            return image[3*(w*y + x)+1];
           }
        else
          {
            return 0;
           }
        return 0;
       };


        
  void setR(int x, int y, unsigned char v) { assert(x >= 0 && x < w && y >= 0 && y < h); _dirty = true; image[3*(w*y + x)] = v;   }
   void setG(int x, int y, unsigned char v) { assert(x >= 0 && x < w && y >= 0 && y < h); _dirty = true; image[3*(w*y + x)+1] = v; }
  void setB(int x, int y, unsigned char v) { assert(x >= 0 && x < w && y >= 0 && y < h); _dirty = true; image[3*(w*y + x)+2] = v; }

  // type can be wxBITMAP_TYPE_BMP, wxBITMAP_TYPE_JPEG, wxBITMAP_TYPE_PNG, wxBITMAP_TYPE_PCX,
  // wxBITMAP_TYPE_PNM, or wxBITMAP_TYPE_XPM 
  // tiff & gifs are broken because of patent issues
  void write(const char *file, int type);
    
    //Get the Gray Scale equivalent of the RGB Image.
    //Does NOT change the image itself.
    virtual unsigned char *grayData() const;

    int convertToGrayScale();
    

};

class RGBAImage : public Image {
private:
  int w, h;
  unsigned char *image;
  bool _dirty;

public:
  enum NO_COPY { NoCopy };
  RGBAImage(int width, int height, unsigned char *data);
  RGBAImage(int width, int height, unsigned char *data, NO_COPY);
    // with NoCopy, a new buffer will not be allocated, only the pointer
    // will be copied. it must be allocated by new[]

  RGBAImage(const char *file);
  ~RGBAImage();

  unsigned char *data() { return image; }
  const unsigned char *data() const { return image; }

  unsigned int glFormat() const;
  unsigned int glType() const;

  int height() const { return h; }
  int width() const { return w; }

  virtual bool dirty() const { return _dirty; }
  virtual void setDirty(bool d=true) { _dirty = d; }

  unsigned char r(int x, int y) const { assert(x >= 0 && x < w && y >= 0 && y < h); return image[4*(w*y + x)];   }
   unsigned char g(int x, int y) const { assert(x >= 0 && x < w && y >= 0 && y < h); return image[4*(w*y + x)+1]; }
  unsigned char b(int x, int y) const { assert(x >= 0 && x < w && y >= 0 && y < h); return image[4*(w*y + x)+2]; }
  unsigned char a(int x, int y) const { assert(x >= 0 && x < w && y >= 0 && y < h); return image[4*(w*y + x)+3]; }

  void setR(int x, int y, unsigned char v) { assert(x >= 0 && x < w && y >= 0 && y < h); _dirty = true; image[4*(w*y + x)] = v;   }
   void setG(int x, int y, unsigned char v) { assert(x >= 0 && x < w && y >= 0 && y < h); _dirty = true; image[4*(w*y + x)+1] = v; }
  void setB(int x, int y, unsigned char v) { assert(x >= 0 && x < w && y >= 0 && y < h); _dirty = true; image[4*(w*y + x)+2] = v; }
  void setA(int x, int y, unsigned char v) { assert(x >= 0 && x < w && y >= 0 && y < h); _dirty = true; image[4*(w*y + x)+3] = v; }

  void setMask(Image *img);
};

#endif
