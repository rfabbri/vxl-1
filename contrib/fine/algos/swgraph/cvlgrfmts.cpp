/*M///////////////////////////////////////////////////////////////////////////////////////
//  
//                      INTEL CORPORATION PROPRIETARY INFORMATION              
//         This software is supplied under the terms of a license agreement or 
//         nondisclosure agreement with Intel Corporation and may not be copied
//         or disclosed except in accordance with the terms of that agreement. 
//               Copyright (c) 1999 Intel Corporation. All Rights Reserved.    
//  
//    RCS:
//       Source:    cvlrgfmts.cpp$
//       $Revision: 1.1 $
//      Purpose: 
//      Contents:
//      Authors: Vadim Pisarevsky
//  
//M*/

#include <windows.h>

#include "cvlgrfmts.h"
#include "grfmt.h"
#include "rd_rle.h"
#include "rd_jpeg.h"

static graphics_format_readers_list* fmts_list = 0;

static void InitObjects()
{
    if( fmts_list == 0 )
    {
        fmts_list = new graphics_format_readers_list;    
        fmts_list->add_reader( new grfmt_bmp_reader() );
        fmts_list->add_reader( new grfmt_sun_raster_reader() );
        fmts_list->add_reader( new grfmt_jpeg_reader() );
    }
}


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved )
{
    hModule, lpReserved, ul_reason_for_call; // unref. params
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            InitObjects();
            break;
        case DLL_PROCESS_DETACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE;
}

///////////////// API ///////////////////

DLL_API int gr_fmt_find_filter( const char* file_name )
{
    graphics_format_reader* filter = fmts_list->find_reader( file_name );
    if( filter )
    {
        filter->set_file( file_name );
    }
    return (int)filter;
}


DLL_API int gr_fmt_read_header( int filter, int* width, int* height, int* color )
{
    graphics_format_reader* reader = (graphics_format_reader*)filter;
    int result = 0;

    if( filter != 0 && width != 0 && height != 0 && reader->read_header())
    {
        *width  = reader->get_width();
        *height = reader->get_height();
        if( color ) *color  = reader->get_color();
        result = 1;
    }

    return result;
}


DLL_API int gr_fmt_read_data( int filter, void* data, int pitch, int color )
{
    graphics_format_reader* reader = (graphics_format_reader*)filter;
    int result = 0;

    if( filter != 0 && data != 0 && pitch != 0 && (color&~1) == 0 ||
        pitch >= ((reader->get_width()*(color ? 3 : 1) + 3) & -4))
    {
        result = reader->read_data( (unsigned char*)data, pitch, color );
    }

    return result;
}

DLL_API int gr_fmt_close_filter( int filter )
{
    if( filter != 0 ) ((graphics_format_reader*)filter)->close();
    return 1;
}


DLL_API int gr_fmt_write_image( void* src_data, int src_pitch,
                                int width, int height, int color,
                                const char* filename, const char* format )
{
    int result = 0;
    FILE* f = 0;
    int bpp = color ? 24 : 8;
    int dst_pitch = (width*(bpp/8) + 3) & -4;

    if( strcmp( format, "bmp" ) && strcmp( format, "BMP" )) return 0;

    if( !src_data || width <= 0 || height <= 0 || (color&~1) ||
        dst_pitch > src_pitch || !filename ) return 0;
    
    f = fopen( filename, "wb" );
    if( f )
    {
        int bitmap_header_size = 40;
        int palette_size = color ? 0 : 1024;
        int header_size = (14 /* fileheader */ + bitmap_header_size +
                           palette_size + 3) & -4;
        char signature[2] = { 'B', 'M' };
        unsigned char palette[256*4];
        char* data = (char*)src_data;
        int file_header[] = { dst_pitch*height + header_size, 0, header_size };
        int bitmap_header[] = { bitmap_header_size, width, height,
                                1|(bpp << 16), 0, 0, 0, 0, 0, 0 };
        int i;

        fwrite( signature, 1, sizeof(signature), f );
        fwrite( file_header, 1, sizeof(file_header), f );
        fwrite( bitmap_header, 1, bitmap_header_size, f );

        if( !color )
        {
            for( i = 0; i < 256; i++ )
            {
                palette[i*4] = palette[i*4 + 1] = palette[i*4 + 2] = (unsigned char)i;
                palette[i*4 + 3] = 0;
            }
            fwrite( palette, 1, sizeof(palette), f );
        }

        // for alignment
        fwrite( signature, 1, sizeof(signature), f );
        
        data += src_pitch*(height - 1);
        for( i = height; i > 0; i--, data -= src_pitch )
        {
            fwrite( data, 1, dst_pitch, f );
        }
        fclose(f);
        result = 1;
    }
    return result;
}




