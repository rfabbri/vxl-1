#ifndef SUSAN_INTERFACE_H
#define SUSAN_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif
  
  
  
  
  typedef unsigned char gray8;
  typedef short gray16;
  typedef int gray32;
  
  typedef struct {
    unsigned int width, height;
    gray8 *data;
  } SusanImage_Gray8;
  
  
  typedef struct 
  {
    unsigned int width;
    unsigned int height;
    
    float  threshold; // 256 
    int    mode; // 1 -- already set ( 0 , 1 , 2 ) 
    int    sub_pixel; // 1 bool
    int    thin; // 1 -- bool
    int    mask_size; // 2 ( 0 , 1 , 2 )
    char   *in;
    char   *out;
  } SusanInterface;
  
  struct Susanpixels{
    
    float xpos;
    float ypos;
    unsigned char direction;
    double strength;
    struct Susanpixels * next;
    
  };
  
  typedef struct 
  {
    struct Susanpixels *pixels;
    struct Susanpixels *subpixels;
    struct Susanpixels *pixel_corners;
  }SusanEdges;
  
  

#ifdef __cplusplus
}
#endif


#endif     /* SUSAN_INTERFACE_H */
