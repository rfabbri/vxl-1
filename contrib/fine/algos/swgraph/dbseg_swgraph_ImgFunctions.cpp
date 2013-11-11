#pragma warning(disable:4786)
#include <cmath>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include "point.h"
#include "functions.h"
#include "imgfunctions.h"
#include "ipl.h"
#include "image.h"
//eli edit #include "jpeglib.h"
//#include "jerror.h"


#include <algorithm>
using namespace std;

//conversion Ipl<->Matrix<->Image
//Matrif=Matrix<float> is equivalent to Iplf=Ipl of float
//Matrixu=Matrix<unsigned char> is equiv to Ipl= Ipl of unsigned char
//Image is equiv to Matrixu 
//change between
void Iplf2Image(IplImage *I, CImage &J){
    int i,j,h=I->height,w=I->width;
    J.Create(I->width ,I->height ,8);
    IplImage *Ip=J.GetImage();
    unsigned char c;
    float f;
    iplGetPixel(I,0,0,&f);
    for(i=0;i<w;i++)
        for(j=0;j<h;j++){
            iplGetPixel(I,i,j,&f);
            c=(char)(140+f*50); 
            iplPutPixel(Ip,i,j,&c);
        }

}
void Matrix2Ipl(Matrix<int> &I, IplImage *Ip){
    int i,j,r=I.rows(),c=I.cols();
    unsigned char ch;
    double f;
    for(i=0;i<r;i++)
        for(j=0;j<c;j++){
            f=I.Data(i,j);
            if (f<0) f=0;
            if (f>255) f=255;
            ch=(char)(f); 
            iplPutPixel(Ip,j,i,&ch);
        }
}
void Matrix2Ipl_scale(Matrix<int> &I, IplImage *Ip){
    int i,j,r=I.rows(),c=I.cols();
    unsigned char ch;
    double f,max,min;
    min=I.GetMin();
    max=I.GetMax();
    max=max+0.1;
    for(i=0;i<r;i++)
        for(j=0;j<c;j++){
            f=(I.Data(i,j)-min)*255./(max-min);
            if (f<0) f=0;
            if (f>255) f=255;
            ch=(char)(f); 
            iplPutPixel(Ip,j,i,&ch);
        }
}
void Matrix2Ipl(Matrix<__int64> &I, IplImage *Ip){
    int i,j,r=I.rows(),c=I.cols();
    unsigned char ch;
    double f,max,min;
    min=I.GetMin();
    max=I.GetMax();
    max=max+0.1;
    for(i=0;i<r;i++)
        for(j=0;j<c;j++){
            f=(I.Data(i,j)-min)*255./(max-min);
            if (f<0) f=0;
            if (f>255) f=255;
            ch=(char)(f); 
            iplPutPixel(Ip,j,i,&ch);
        }
}
void Matrix2Ipl(Matrix<uchar> &I, IplImage *Ip){
    int i,j,r=I.rows(),c=I.cols();
    unsigned char ch;
    for(i=0;i<r;i++)
        for(j=0;j<c;j++){
            ch=I.Data(i,j); 
            iplPutPixel(Ip,j,i,&ch);
        }
}
void Matrix2Iplf(Matrix<uchar> &I, IplImage *Ip){
    int i,j,h=I.rows(),w=I.cols();
    float f;
    for(i=0;i<w;i++)
        for(j=0;j<h;j++){
            f=(float)((I.Data(j,i)-140.)/50.);
            iplPutPixel(Ip,i,j,&f);
        }
}
IplImage *Matrix2Iplf(Matrix<uchar> &M){
    //returns an IplImage from M, after rescaling
    int i,j,h=M.rows(),w=M.cols();
    float f;
    IplImage *I = iplCreateImageHeader(
        1, // number of channels
        0, // no alpha channel
        IPL_DEPTH_32F, // data of float type
        "GRAY", // color model
        "GRAY", // color order
        IPL_DATA_ORDER_PIXEL, // channel arrangement
        IPL_ORIGIN_TL, // top left orientation
        IPL_ALIGN_QWORD, // 8 bytes align
        M.cols(), // image width
        M.rows(), // image height
        NULL, // no ROI
        NULL, // no mask ROI
        NULL, // no image ID
        NULL); // not tiled;
    iplAllocateImageFP(I,0,0);
    for(i=0;i<w;i++)
        for(j=0;j<h;j++){
            f=(float)((M.Data(j,i)-140.)/50.);
            iplPutPixel(I,i,j,&f);
        }
    return I;
}
void Matrix2Ipl(Matrix<float> &I, IplImage *Ip){
    int i,j,h=I.rows(),w=I.cols();
    unsigned char c;
    double f;
    for(i=0;i<w;i++)
        for(j=0;j<h;j++){
            f=140+(I.Data(j,i)*50);
            if (f<0) f=0;
            if (f>255) f=255;
            c=(char)(f); 
            iplPutPixel(Ip,i,j,&c);
        }
}
void Matrix2Ipl(Matrix<double> &I, IplImage *Ip){
    int i,j,h=I.rows(),w=I.cols();
    unsigned char c;
    double f;
    for(i=0;i<w;i++)
        for(j=0;j<h;j++){
            f=I.Data(j,i);
            if (f<0) f=0;
            if (f>255) f=255;
            c=(char)(f); 
            iplPutPixel(Ip,i,j,&c);
        }
}
void Matrix2Iplf(Matrix<float> &I, IplImage *Ip){
    int i,j,h=I.rows(),w=I.cols();
    float f;
    for(i=0;i<w;i++)
        for(j=0;j<h;j++){
            f=I.Data(j,i);
            iplPutPixel(Ip,i,j,&f);
        }
}
void Matrix2Ipl(Matrixu &R,Matrixu &G,Matrixu &B,IplImage *Ip){
    int i,j,r=R.rows(),c=R.cols();
    unsigned char ch[3];
    for(i=0;i<r;i++)
        for(j=0;j<c;j++){
            ch[2]=R.Data(i,j); 
            ch[1]=G.Data(i,j); 
            ch[0]=B.Data(i,j); 
            iplPutPixel(Ip,j,i,&ch);
        }
}
void SaveBmp(char * filename,Matrixu &R,Matrixu &G,Matrixu &B){
    CImage image;
    image.Create(R.cols(),R.rows(),24);
    Matrix2Ipl(R,G,B,image.GetImage());    
    image.Save (filename);
    image.Destroy();
}

bool LoadJpg(char * filename,Matrix<uchar> &I){
    FILE* file = fopen(filename, "rb");
    int i,j,m_BytesPerPixel,m_Width;
    if (!file) return(false);
    jpeg_decompress_struct cinfo;
    jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, file);
    jpeg_read_header(&cinfo, (boolean)true);
    if(cinfo.out_color_space == JCS_GRAYSCALE) {
        cinfo.output_components = 1;
        m_BytesPerPixel = 1;
    }
    else { // convert to 24 bit
        cinfo.out_color_space = JCS_RGB;
        cinfo.output_components = 3;
        m_BytesPerPixel = 3;
    }
    jpeg_calc_output_dimensions(&cinfo);
    jpeg_start_decompress(&cinfo);
    m_Width=cinfo.output_width;
    I.SetDimension(cinfo.output_height, cinfo.output_width);
    const int LineSize = m_Width*m_BytesPerPixel;
    unsigned char *pData = new unsigned char[m_Width*m_BytesPerPixel];
    i=0;
    while(cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, &pData, 1);
        for (j=0;j<m_Width;j++)
            I(i,j)=pData[j*m_BytesPerPixel+1];
        i++;
    }
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(file);
    return true;
}
bool LoadJpg(char * filename,Matrix<uchar> &R,Matrix<uchar> &G,Matrix<uchar> &B){
    FILE* file = fopen(filename, "rb");
    int i,j,m_BytesPerPixel,m_Width;
    if (!file) return(false);
    jpeg_decompress_struct cinfo;
    jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, file);
    jpeg_read_header(&cinfo, (boolean)true);
    if(cinfo.out_color_space == JCS_GRAYSCALE) {
        cinfo.output_components = 1;
        m_BytesPerPixel = 1;
    }
    else { // convert to 24 bit
        cinfo.out_color_space = JCS_RGB;
        cinfo.output_components = 3;
        m_BytesPerPixel = 3;
    }
    jpeg_calc_output_dimensions(&cinfo);
    jpeg_start_decompress(&cinfo);
    m_Width=cinfo.output_width;
    R.SetDimension(cinfo.output_height, cinfo.output_width);
    G.SetDimension(cinfo.output_height, cinfo.output_width);
    B.SetDimension(cinfo.output_height, cinfo.output_width);
    const int LineSize = m_Width*m_BytesPerPixel;
    unsigned char *pData = new unsigned char[m_Width*m_BytesPerPixel];
    i=0;
    while(cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, &pData, 1);
        for (j=0;j<m_Width;j++){
            R(i,j)=pData[j*m_BytesPerPixel+2];
            G(i,j)=pData[j*m_BytesPerPixel+1];
            B(i,j)=pData[j*m_BytesPerPixel];
        }
        i++;
    }
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(file);
    return true;
}
bool LoadBmp(char * filename,Matrix<uchar> &R,Matrix<uchar> &G,Matrix<uchar> &B){
    CImage image;
    bool b;
    b=image.Load (filename,24);
    if (b){
        Ipl2Matrixu(image.GetImage (),R,G,B);
    }
    return b;
}

void SaveBmpu(char * filename,Matrix<uchar> &M){
    CImage image;
    image.Create(M.cols(),M.rows(),8);
    Matrix2Ipl(M,image.GetImage());    
    image.Save (filename);
    image.Destroy();
}
bool LoadBmp(char * filename,Matrix<uchar> &M){
    CImage image;
    bool b;
    b=image.Load (filename,8);
    if (b)
        M=Ipl2Matrixu(image.GetImage ());
    return b;
}
bool LoadImg(char * filename,Matrix<uchar> &M){
    char *ext=filename+(strlen(filename)-3);
    if (strcmp(ext,"bmp")==0){
        return LoadBmp(filename,M);
    }
    return LoadJpg(filename,M);
}

void Ipl2Matrixu(IplImage *I, Matrixu &R, Matrixu&G, Matrixu&B){
    int i,j,h=I->height,w=I->width;
    unsigned char c[3];
    R.SetDimension(h,w);
    G.SetDimension(h,w);
    B.SetDimension(h,w);
    for(i=0;i<w;i++)
        for(j=0;j<h;j++){
            iplGetPixel(I,i,j,&c);
            R(j,i)=c[2];
            G(j,i)=c[1];
            B(j,i)=c[0];
        }
}
Matrix<float> Ipl2Matrixf(IplImage *I){
    int i,j,h=I->height,w=I->width;
    Matrix<float> M;
    unsigned char c;
    M.SetDimension(h,w);
    for(i=0;i<w;i++)
        for(j=0;j<h;j++){
            iplGetPixel(I,i,j,&c);
            M(j,i)=(float)((c-140.)/50.);
        }
    return M;
}
Matrix<unsigned char> Ipl2Matrixu(IplImage *I){
    int i,j,h=I->height,w=I->width;
    Matrix<unsigned char> M;
    unsigned char c;
    M.SetDimension(h,w);
    for(i=0;i<w;i++)
        for(j=0;j<h;j++){
            iplGetPixel(I,i,j,&c);
            M(j,i)=c;
        }
    return M;
}
Matrix<float> Iplf2Matrixf(IplImage *I){
    int i,j,h=I->height,w=I->width;
    Matrix<float> M;
    float f;
    M.SetDimension(h,w);
    for(i=0;i<w;i++)
        for(j=0;j<h;j++){
            iplGetPixel(I,i,j,&f);
            M(j,i)=f;
        }
    return M;
}

IplImage *Matrix2Ipl(Matrix<float> &M){
    //returns an IplImage from M, after rescaling
    int i,j,h=M.rows(),w=M.cols();
    unsigned char c;
    float f,max,min;
    f=M.Data(0,0);
    max=f,min=f;
    IplImage *I = iplCreateImageHeader(
        1, // number of channels
        0, // no alpha channel
        IPL_DEPTH_8U, // data of byte type
        "GRAY", // color model
        "GRAY", // color order
        IPL_DATA_ORDER_PIXEL, // channel arrangement
        IPL_ORIGIN_TL, // top left orientation
        IPL_ALIGN_QWORD, // 8 bytes align
        M.cols(), // image width
        M.rows(), // image height
        NULL, // no ROI
        NULL, // no mask ROI
        NULL, // no image ID
        NULL); // not tiled;
    iplAllocateImage(I,0,0);
    for(i=0;i<w;i++)
        for(j=0;j<h;j++){
            f=M.Data(j,i);
            c=(unsigned char)(140+f*50);//(f-min)*255/(max-min)); 
            iplPutPixel(I,i,j,&c);
        }
    return I;
}
IplImage *Matrix2Iplf(Matrix<float> &M){
    //returns an IplImage from M, after rescaling
    int i,j,h=M.rows(),w=M.cols();
    float f,max,min;
    f=M.Data(0,0);
    max=f,min=f;
    IplImage *I = iplCreateImageHeader(
        1, // number of channels
        0, // no alpha channel
        IPL_DEPTH_32F, // data of float type
        "GRAY", // color model
        "GRAY", // color order
        IPL_DATA_ORDER_PIXEL, // channel arrangement
        IPL_ORIGIN_TL, // top left orientation
        IPL_ALIGN_QWORD, // 8 bytes align
        M.cols(), // image width
        M.rows(), // image height
        NULL, // no ROI
        NULL, // no mask ROI
        NULL, // no image ID
        NULL); // not tiled;
    iplAllocateImageFP(I,0,0);
    for(i=0;i<w;i++)
        for(j=0;j<h;j++){
            f=M.Data(j,i);
            iplPutPixel(I,i,j,&f);
        }
    return I;
}
void print(char * name,IplConvKernelFP *k){
     int r=k->nRows,c=k->nCols;
     float *val=k->values;
    FILE *f=fopen(name, "w" );
    for(int i=0;i<r;i++){
        for(int j=0;j<c-1;j++)
            fprintf(f,"%3.3f,",val[i*c+j]);
        fprintf(f,"%3.3f\n",val[i*c+c-1]);
    }
    fclose( f );
}
void print(char * name,IplImage *I){
     int ny=I->height ,nx=I->width;
     char val;
    FILE *f=fopen(name, "w" );
    for(int i=0;i<ny;i++){
        for(int j=0;j<nx-1;j++){
            iplGetPixel(I,j,i,&val);
            fprintf(f,"%d,",val);
        }
        iplGetPixel(I,nx-1,i,&val);
        fprintf(f,"%d\n",val);
    }
    fclose( f );
}



IplConvKernelFP *ipldog(double xd, double yd,double sigma){
//    function [B, C] = dog(sigma)
//    [B, C] = dog(sigma)
// Center-sorround DOG base
// sigma: scale
// B: the base matrix
// C: the symbolic representation
    long i,j,size;
    double cx,cy,r,s,x,y,d;
    float *B;
    IplConvKernelFP *ker;

    size = (int) (sigma*6);     // the size of the base matrix 
    if ((size%2)==0)         // the size of B should be odd 
        size++;
    cx = (size-1)/2+xd;        // the half size
    cy = (size-1)/2+yd;
    
    B=new float[size*size];

    r = .9; s = 1;          // the sizes of center and sorround 
    for (i = 0;i<size;i++){
        for (j = 0;j<size;j++){
            x = (j-cx)/sigma;
            y = (i-cy)/sigma;    // scale x and y
            B[size*size-1-(i*size+j)] = (float)(G(x, r)*G(y, r) - G(x, s)*G(y, s));
                            // difference between center and sorround
        }
    }
    d=0;
    for (i=0; i<size*size; i++)
            d += B[i];
    d=d/size/size;    //mean
    for (i=0; i<size*size; i++)
            B[i]-=(float)d;
    d=0;
    for (i=0; i<size*size; i++)
            d += B[i]*B[i];
    d=sqrt(d)+0.00001;        //var
    for (i=0; i<size*size; i++)
            B[i] = (float)(B[i]/d);    // normalized to norm 1

    ker=iplCreateConvKernelFP(size,size,(size-1)/2,(size-1)/2,B);
    delete []B;
    return ker;
}


IplConvKernelFP *ipldoog(double xd, double yd,double sigma,double orientation,double aspect){
    // [B,C] = doog(sigma, orientation, aspect)
    // Even DOOG base, i.e., second derivative 
    // sigma: scale of the Gaussian in x-direction 
    // orientation: in degree
    // aspect ratio: y:x
    // B: the base matrix
    // C: the symbolic representation (in symbdoog)
    
    long i,j,size;
    double cx,cy,d,teta,costeta,sinteta,r,x,y,x0,y0;
    float *B;
    IplConvKernelFP *ker;

    size = (int)(sigma * 4 * aspect)+1;     // size of the base
    if ((size%2)==0)         // the size of B should be odd 
        size++;
    cx = (size-1)/2+xd ;        // half-size
    cy = (size-1)/2+yd ;        // half-size
    teta = (PI * orientation)/180.;    // orientation in radians
    costeta=cos(teta);
    sinteta=sin(teta);
    
    B=new float[size*size];

    r = aspect;           // aspect ratio
    for (i = 0;i<size;i++){
        for (j = 0;j<size;j++){
            x0=j-cx;y0=i-cy;
            x = (x0*costeta-y0*sinteta)/sigma;
            y = (x0*sinteta+y0*costeta)/sigma;    // scale x and y
                              // rotate by -teta and scale x and y
            B[size*size-1-(i*size+j)] = (float)((2*G(x, 1)-G(x-1, 1)-G(x+1, 1))*G(y, r)); 
                              // second derivative 
        }
    }
    
    d=0;
    for (i=0; i<size*size; i++)
            d += B[i];
    d=d/size/size;    //mean
    for (i=0; i<size*size; i++)
            B[i]-=(float)d;
    d=0;
    for (i=0; i<size*size; i++)
            d += B[i]*B[i];
    d=sqrt(d)+0.00001;        //var
    for (i=0; i<size*size; i++)
            B[i] = (float)(B[i]/d);    // normalized to norm 1

    ker=iplCreateConvKernelFP(size,size,(size-1)/2,(size-1)/2,B);
    delete []B;
    return ker;
}


IplConvKernelFP *ipldoog1(double xd, double yd,double sigma,double orientation,double aspect){
// [B,C] = doog1(sigma, orientation, aspect)
// Odd DOOG base, i.e., first derivative 
// sigma: scale of the Gaussian in x-direction 
// orientation: in degree
// aspect ratio: y:x
// B: the base matrix
// C: the symbolic representation 

    long i,j,size;
    double cx,cy,d,teta,costeta,sinteta,r,x,y,x0,y0;
    float *B;
    IplConvKernelFP *ker;

    size = (int)(sigma * 4 * aspect)+1;     // size of the base
    if ((size%2)==0)         // the size of B should be odd 
        size++;
    cx = (size-1)/2+xd ;        // half-size
    cy = (size-1)/2+yd ;        // half-size
    teta = (PI * orientation)/180;    // orientation in radians
    costeta=cos(teta);
    sinteta=sin(teta);
    
    B=new float[size*size];

    r = aspect;           // aspect ratio
    for (i = 0;i<size;i++){
        for (j = 0;j<size;j++){
            x0=j-cx;y0=i-cy;
            x = (x0*costeta-y0*sinteta)/sigma;
            y = (x0*sinteta+y0*costeta)/sigma;    
                                 // rotate by -teta and scale x and y
            B[size*size-1-(i*size+j)] = (float)((G(x-1, 1)-G(x+1, 1))*G(y, r));      
                         // first derivative 
        }
    }
    
    d=0;
    for (i=0; i<size*size; i++)
            d += B[i];
    d=d/size/size;    //mean
    for (i=0; i<size*size; i++)
            B[i]-=(float)d;
    d=0;
    for (i=0; i<size*size; i++)
            d += B[i]*B[i];
    d=sqrt(d)+0.00001;        //var
    for (i=0; i<size*size; i++)
            B[i] = (float)(B[i]/d);    // normalized to norm 1

    ker=iplCreateConvKernelFP(size,size,(size-1)/2,(size-1)/2,B);
    delete []B;
    return ker;
}
Matrix<double> dog(double sigma){
//    function [B, C] = dog(sigma)
//    [B, C] = dog(sigma)
// Center-sorround DOG base
// sigma: scale
// B: the base matrix
// C: the symbolic representation
    long i,j,size;
    double c,r,s,x,y;
    Matrix<double> B;

    size = (int) (sigma*6);     // the size of the base matrix 
    c = sigma*3;        // the half size
    
    if ((size%2)==0)         // the size of B should be odd 
        size++;
    B.SetDimension(size,size);// the base matrix    
    B.Zero();

    r = .9; s = 1;          // the sizes of center and sorround 
    for (i = 0;i<size;i++){
        for (j = 0;j<size;j++){
            x = (i-c)/sigma;
            y = (j-c)/sigma;    // scale x and y
            B(i,j) = G(x, r)*G(y, r) - G(x, s)*G(y, s);
                            // difference between center and sorround
        }
    }
    c=0;
    for (i=0; i<B.rows(); i++)
        for (j=0; j<B.cols(); j++)
            c += B.Data(i,j)*B.Data(i,j);

    B = B/sqrt(c);    // normalized to norm 1

    return B;
}

Matrix<double> doog(double sigma,double orientation,double aspect){
    // [B,C] = doog(sigma, orientation, aspect)
    // Even DOOG base, i.e., second derivative 
    // sigma: scale of the Gaussian in x-direction 
    // orientation: in degree
    // aspect ratio: y:x
    // B: the base matrix
    // C: the symbolic representation (in symbdoog)
    
    long i,j,size;
    double c,theta,costheta,sintheta,r,x,y;
    Matrix<double> B;

    size = (int)(sigma * 4 * aspect)+1;     // size of the base
    c = sigma * 2 * aspect ;        // half-size
    theta = (PI * orientation)/180.;    // orientation in radians
    costheta=cos(theta);
    sintheta=sin(theta);
    
    if ((size%2)==0)         // the size of B should be odd 
        size++;
    B.SetDimension(size,size);// the base matrix    
    B.Zero();

    r = aspect;           // aspect ratio
    for (i = 0;i<size;i++){
        for (j = 0;j<size;j++){
            x = ((i-c) * costheta + (j-c) * sintheta)/sigma;
            y = ((c-i) * sintheta + (j-c) * costheta)/sigma;
                              // rotate and scale x and y
            B(i,j) = (2*G(x, 1)-G(x-1, 1)-G(x+1, 1))*G(y, r); 
                              // second derivative 
        }
    }
    
    c=0;
    for (i=0; i<B.rows(); i++)
        for (j=0; j<B.cols(); j++)
            c += B.Data(i,j)*B.Data(i,j);

    B = B/sqrt(c);    // normalized to norm 1

    return B;
}

Matrix<double> doog1(double sigma, double orientation,double aspect){
// [B,C] = doog1(sigma, orientation, aspect)
// Odd DOOG base, i.e., first derivative 
// sigma: scale of the Gaussian in x-direction 
// orientation: in degree
// aspect ratio: y:x
// B: the base matrix
// C: the symbolic representation 

    long i,j,size;
    double c,theta,costheta,sintheta,r,x,y;
    Matrix<double> B;

    size = (int)(sigma * 4 * aspect)+1;     // size of the base
    c = sigma * 2 * aspect ;        // half-size
    theta = (PI * orientation)/180;    // orientation in radians
    costheta=cos(theta);
    sintheta=sin(theta);
    
    if ((size%2)==0)         // the size of B should be odd 
        size++;
    B.SetDimension(size,size);// the base matrix    
    B.Zero();

    r = aspect;           // aspect ratio
    for (i = 0;i<size;i++){
        for (j = 0;j<size;j++){
            x = ((i-c) * costheta + (j-c) * sintheta);
            y = ((c-i) * sintheta + (j-c) * costheta);
                                 // rotate and scale x and y
            B(i,j) = (G(x-1, 1)-G(x+1, 1))*G(y, r);      
                         // first derivative 
        }
    }
    
    c=0;
    for (i=0; i<B.rows(); i++)
        for (j=0; j<B.cols(); j++)
            c += B.Data(i,j)*B.Data(i,j);

    B = B/sqrt(c);    // normalized to norm 1

    return B;
}




