/*M///////////////////////////////////////////////////////////////////////////////////////
//  
//                      INTEL CORPORATION PROPRIETARY INFORMATION              
//         This software is supplied under the terms of a license agreement or 
//         nondisclosure agreement with Intel Corporation and may not be copied
//         or disclosed except in accordance with the terms of that agreement. 
//               Copyright (c) 1999 Intel Corporation. All Rights Reserved.    
//  
//    RCS:
//       Source:    cvlgrfmts.h$
//       $Revision: 1.1 $
//      Purpose: 
//      Contents:
//      Authors: Vadim Pisarevsky
//  
//M*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

/*
   find appropriate graphic format filter (reader)
   using first few bytes of the image file.
   Returns 0 or filter handle, which is passed to other functions
*/
DLL_API int  gr_fmt_find_filter( const char* file_name );

/* 
   read image header and return base image parameters: size and color information.
   returned color value (it is optional parameter) means:
        0 - grayscale image
        1 - color image
       -1 - image with palette
*/
DLL_API int  gr_fmt_read_header( int filter, int* width, int* height, int* color );

/*
   read image raster data (it has to be allocated by the caller).
   pitch means row width of the image (in bytes).
   color means desired color mode:
       0 - convert image data to grayscale
       1 - convert image data to color
*/
DLL_API int  gr_fmt_read_data( int filter, void* data, int pitch, int color );

/*
   release filter. Must be called after image has been read.
*/
DLL_API int  gr_fmt_close_filter( int filter );

/*
   write raster data to file. pitch - row width of the image.
   color:
      0 - grayscale image is written.
          if the format doesn't support grayscale images directly
          then image with palette is written.
      1 - color image is written.
   format:
      only "bmp" (Windows Bitmap) is supported by now.
*/
DLL_API int  gr_fmt_write_image( void* data, int pitch, 
                                 int width, int height, int color,
                                 const char* filename, const char* format );

#ifdef __cplusplus
}
#endif





