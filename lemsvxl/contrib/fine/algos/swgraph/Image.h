// Image.h: interface for the CImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGE_H__B0C6A376_E412_4ADD_B1EA_7488350A7F5B__INCLUDED_)
#define AFX_IMAGE_H__B0C6A376_E412_4ADD_B1EA_7488350A7F5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ipl.h"

inline int iplWidth( IplImage* img )
{
    return !img ? 0 : !(img->roi) ? img->width : img->roi->width;
}

inline int iplHeight( IplImage* img )
{
    return !img ? 0 : !(img->roi) ? img->height : img->roi->height;
}

inline IplROI RectToROI( RECT rect )
{
    IplROI roi;
    roi.coi = 0;
    roi.xOffset = rect.left;
    roi.yOffset = rect.top;
    roi.width = rect.right - rect.left;
    roi.height = rect.bottom - rect.top;
    return roi;
}


class CImage  
{
public:
    CImage();
    ~CImage();
    
    /* Create image (BGR or grayscale) */
    bool  Create( int w, int h, int bpp );
    
    /* Load image from specified file */
    bool  Load( const char* filename, int desired_color );

    /* Load rectangle from the file */
    bool  LoadRect( const char* filename,
                    int desired_color, RECT r );
    
    /* Save entire image to specified file. */
    bool  Save( const char* filename );
    
    /* Get copy of input image ROI */
    void  CopyOf( CImage& image, int desired_color = -1 );
    IplImage* GetImage();
    HDC   GetDC();
    void  Destroy(void);
    
    /* width and height of ROI */
    int   Width() { return iplWidth( &m_img ); };
    int   Height() { return iplHeight( &m_img ); };

    /* put part of bitmap to specified destination rectangle */
    void  Show( HDC dc, int x, int y, int w, int h, int from_x, int from_y );

    void  Fill( COLORREF color );

protected:

    void      Clear(void);
    
    HDC       m_memDC;
    HGDIOBJ   m_old;
    IplImage  m_img;
};

bool  LoadGrFmtLib();
void  FillBitmapInfo( BITMAPINFO* bmi, int width, int height, int bpp );

#endif // !defined(AFX_IMAGE_H__B0C6A376_E412_4ADD_B1EA_7488350A7F5B__INCLUDED_)


