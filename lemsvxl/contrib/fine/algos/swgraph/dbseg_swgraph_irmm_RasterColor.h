////////////////////////////////////////////////////////////////////////
//irmm_Raster.h
//This is the description of classes of image library including
//======================================================================
//        Gaussian filtering
//        segmentation of color image using mean shift algorithm
//=======================================================================
//    Author: Zhuowen Tu
////////////////////////////////////////////////////////////////////////
#ifndef _IRMM_RASTERCOLOR_H
#define _IRMM_RASTERCOLOR_H

#include "irmm_Raster.h"
#include "Vector.h"
 
// List of color components; the possible cases are:
#define RedColor        (1<<0)
#define GreenColor        (1<<1)
#define BlueColor        (1<<2)
#define Lightness        (1<<3)
#define Ustar           (1<<4)
#define Vstar           (1<<5)


template<class Tp>
class ColorRGB
{
public:
    Tp r;
    Tp g;
    Tp b;

    ColorRGB(){r=0;g=0;b=0;};
    ColorRGB(Tp ir,Tp ig,Tp ib){r=ir;g=ig;b=ib;};
    
    inline void Set(Tp rc,Tp gc,Tp bc){r=rc;g=gc;b=bc;};
};

class ColorLUV
{
public:
    int    L;
    int    U;
    int    V;

    ColorLUV(){L=0;U=0;V=0;};
    ColorLUV(const int iL, const int iU, const int iV){L=iL; U=iU; V=iV;};
    
    inline void Set(const int iL, const int iU, const int iV){L=iL; U=iU; V=iV;};
};

#define MAXV 256

////////////////////////////////////////////////////////////
//this is the class of color image with three channels in r, g, b or
//L, U, V spaces
template<class Tp>
class Raster_Color
{
public:
    // construction
    Raster_Color();
    ~Raster_Color();
    
    // operators
    Raster_Color& operator=(Raster_Color<uchar> &I);
    Raster_Color& operator=(Raster_Color<double> &I);
    void Copy(Raster_Color<uchar> &I, irmm_Rect& rect);

    void    Free_histogram_space(void);

    void SetDimension(const int nrows, const int ncols);

    // access
    inline Tp&        R(const int Row, const int Col);
    inline Tp&        G(const int Row, const int Col);
    inline Tp&        B(const int Row, const int Col);
    inline Tp&        R(const long index);
    inline Tp&        G(const long index);
    inline Tp&        B(const long index);

    inline int&        L(const int Row, const int Col){return LUV[0](Row, Col);};
    inline int&        U(const int Row, const int Col){return LUV[1](Row, Col);};
    inline int&        V(const int Row, const int Col){return LUV[2](Row, Col);};
    inline int&        L(const long index){return LUV[0](index);};
    inline int&        U(const long index){return LUV[1](index);};
    inline int&        V(const long index){return LUV[2](index);};
    
    inline int        gray(const long index);
    inline int        gray(const int Row, const int Col){return gray(cols()*Row + Col);};
    inline int        grey(const long index){return (((((int)R(index))<<8)+(int)G(index))<<8)+(int)B(index);};
    inline int        grey(const int Row, const int Col){return grey(rows() * cols() + Col);};


    inline long        rows() const {return _rows;};
    inline long        cols() const {return _cols;};
    inline bool        Valid(const int j, const int i)
                        {if (j>=0&&j<rows()&&i>=0&&i<cols())return true;else return false;}

    inline long        width(){return cols();};
    inline long        height(){return rows();};

    virtual double    color_dis(const int index, const double mean_L, const double mean_U, const double mean_V);
    virtual void    Zero(void);

    // manipulation
    virtual void    convert_RGB_LUV(long selects=Lightness | Ustar | Vstar);            // converts RGB space to L*u*v space
    virtual void    convert_LUV_RGB(double *, int *, long select=Lightness | Ustar | Vstar);                                // converts L*u*v space to RGB space
    virtual void    histogram(long selects=Lightness | Ustar | Vstar);                    // forms histogram of this color image
    virtual void    CreateLUVspace(void);
    virtual void    SetPixelInLUV(const int row, const int col,
                                  const int l, const int u, const int v);
    virtual void    ToRGB(void);
    
    // for color indexing
    virtual Matrix<int> Chromaticity(int r_g_b=RedColor);
    virtual void    MeanVariance(Matrix<int> &mask_label, irmm_Rect &bound,
                                 const int num_label,
                                 Matrix<double>    *pmeans,
                                 Matrix<double> *pvariances,
                                 Matrix<long> &counts);
    virtual void    Compute2DColor(Matrix<int> &mask_label, irmm_Rect &bound,
                                 const int num_label, Vector<double> *pvc_N);

public:
    Raster<Tp>        RGB[3];
    Raster<int>        LUV[3];
    long            count;            //pixel count as row * col in this image
    Raster<uchar>    EdgeMap;        //edge map of this color image

protected:
    int            _rows, _cols;    //row number and column number of this image

    int          _n_colors;      //# of colors
    int**       _col_all;       //colors in the image (LUV)
    int*        _col0,*_col1,*_col2;      //to speed up
    int*        _col_RGB;       //colors in the image (RGB)
    int*        _col_index;     //color address
    int*        _col_misc;      //misc use in histogram and conversion
    int*        _m_colors;      //how many of each color
    int*        _col_remain;    //the map of remaining colors 
    int*        _m_col_remain;  //table of remaining colors
    int         _n_col_remain;  //# of remaining colors
};

void    SaveBmp    (char * filename,Raster_Color<uchar> &R);
bool    LoadBmp    (char * filename,Raster_Color<uchar> &R);
bool    LoadJpg(char * filename,Raster_Color<uchar> &R);
bool    LoadImg(char * filename,Raster_Color<uchar> &M);
__int64 SSEImage(Raster_Color<uchar> &I0,Raster_Color<uchar> &I1,Matrix<int> &G,int x, int y, int dx, int dy);
double Correlation(Raster_Color<uchar> &I0,Raster_Color<uchar> &I1, vector<Pointi> &N,int dx, int dy);
void Get_corr_sums(Raster_Color<uchar> &I0,Raster_Color<uchar> &I1,vector<Pointi> &N,int dx,int dy,int &sum00,int &sum01,int &sum11);


template<class Tp>
inline int Raster_Color<Tp>::gray(const long index)
{
    double a1,a2,a3;
    a1 = R(index);
    a2 = G(index);
    a3 = B(index);
    return (int)(a1*0.299+a2*0.587+a3*0.114);
};

template<class Tp1, class Tp2>
void assign_raster_color(Raster_Color<Tp1>& x1,
                         Raster_Color<Tp2>& x2)
{
    x1.SetDimension(x2.rows(),x2.cols());
    for (int i=0; i<3; i++)
        assign_raster(x1.RGB[i], x2.RGB[i]);
}

///////////////////////////////////////////////////////////
// operators
template<class Tp>
Raster_Color<Tp>& Raster_Color<Tp>::operator=(Raster_Color<uchar> &I)
{
    assign_raster_color(*this,I);
    return *this;
}

template<class Tp>
Raster_Color<Tp>& Raster_Color<Tp>::operator=(Raster_Color<double> &I)
{
    assign_raster_color(*this,I);
    return *this;
}

template<class Tp>
void Raster_Color<Tp>::Copy(Raster_Color<uchar> &I, irmm_Rect& rect)
{
    SetDimension(rect.Height(), rect.Width());
    for (int Row=rect.top; Row<=rect.bottom; Row++)
        for (int Col=rect.left; Col<=rect.right; Col++)
        {
            R(Row-rect.top,Col-rect.left) = (Tp)I.R(Row,Col);
            G(Row-rect.top,Col-rect.left) = (Tp)I.G(Row,Col);
            B(Row-rect.top,Col-rect.left) = (Tp)I.B(Row,Col);
        }
}

///////////////////////////////////////////////////////////
// inline functions
template<class Tp>
inline Tp& Raster_Color<Tp>::R(const int Row, const int Col)
{
    return RGB[0](Row, Col);
};

template<class Tp>
inline Tp& Raster_Color<Tp>::R(const long index)
{
    return RGB[0](index);
};

template<class Tp>
inline Tp& Raster_Color<Tp>::G(const int Row, const int Col)
{
    return RGB[1](Row, Col);
};

template<class Tp>
inline Tp& Raster_Color<Tp>::G(const long index)
{
    return RGB[1](index);
};

template<class Tp>
inline Tp& Raster_Color<Tp>::B(const int Row, const int Col)
{
    return RGB[2](Row, Col);
};

template<class Tp>
inline Tp& Raster_Color<Tp>::B(const long index)
{
    return RGB[2](index);
};

//==========================================================
// Class : Raster_Color
//==========================================================
//----------------------------------------------------------
// construction
//----------------------------------------------------------
////////////////////////////////////////////////////////////
template<class Tp>
Raster_Color<Tp>::Raster_Color()
{
    _rows = 0;
    _cols = 0;
    count = 0;
    _n_colors = 0;

    _col_all = NULL;
    _col0 = NULL;
    _col1 = NULL;
    _col2 = NULL;
    _col_index = NULL;
    _col_misc = NULL;
    _col_RGB = NULL;
    _m_colors = NULL;
}

////////////////////////////////////////////////////////////
template<class Tp>
Raster_Color<Tp>::~Raster_Color()
{
    int i = 0;
    Free_histogram_space();
}

template<class Tp>
void Raster_Color<Tp>::Free_histogram_space(void)
{
    if (_col_all != NULL)
    {
        delete [](_col_all[2]);
        delete [](_col_all[1]);
        delete [](_col_all[0]);
        delete [] _col_all;
    }
    if (_col_index != NULL)
        delete [] _col_index;
    if (_col_misc != NULL)
        delete [] _col_misc;
    if (_col_RGB != NULL)
        delete [] _col_RGB;
    if (_m_colors != NULL)
        delete [] _m_colors;
    _m_colors = NULL;
    _col_RGB = NULL;

    _col_misc = NULL;
    _col_index = NULL;
    _col_all = NULL;
}

template<class Tp>
void Raster_Color<Tp>::SetDimension(const int nrows, const int ncols)
{
    _rows = nrows;
    _cols = ncols;
    count = ((long)_rows) * _cols;
    _n_colors = 3;

    for (int i = 0; i < 3; i++)
        RGB[i].SetDimension(_rows, _cols);
}

template<class Tp>
void Raster_Color<Tp>::Zero(void)
{
    for (int i=0; i<3; i++)
    {
        LUV[i].Zero();
        RGB[i].Zero();
    }
}


//----------------------------------------------------------
// access
//----------------------------------------------------------
template<class Tp>
double Raster_Color<Tp>::color_dis(const int index, const double mean_L,
                                     const double mean_U, const double mean_V)
{
    double fL, fU, fV;
    
    fL = L(index) - mean_L;
    fU = U(index) - mean_U;
    fV = V(index) - mean_V;
    return (fL * fL + fU * fU + fV * fV);
}

//---------------------------------------------------------------------
// manipulation

///////////////////////////////////////////////////////////////////////
// RGB to LUV conversion
// To gain speed the conversion works on a table of colors (_col_RGB[])
// rather than on the whole image
// Coefficient matrix for xyz and rgb spaces
static const int    XYZ[3][3] = { { 4125, 3576, 1804 },
                                  { 2125, 7154,  721 },
                                  {  193, 1192, 9502 } };
static const int    SEC_SIZE=64;  // 2^6

template<class Tp>
void Raster_Color<Tp>::convert_RGB_LUV(long selects)
{
    int        x, y, z, my_temp;
    double    l_star, u_star, v_star;
    double    u_prime, v_prime;
     int temp_col, temp_index, temp_ind;
     long j,k;
  
    int a00 = XYZ[0][0], a01 = XYZ[0][1], a02 = XYZ[0][2];
    int a10 = XYZ[1][0], a11 = XYZ[1][1], a12 = XYZ[1][2];
    int a20 = XYZ[2][0], a21 = XYZ[2][1], a22 = XYZ[2][2];
  
    int *A00 = new int[MAXV]; int *A01 = new int[MAXV]; int *A02 = new int[MAXV];
    int *A10 = new int[MAXV]; int *A11 = new int[MAXV]; int *A12 = new int[MAXV];
    int *A20 = new int[MAXV]; int *A21 = new int[MAXV]; int *A22 = new int[MAXV];
  
    for(j=0; j<MAXV; j++)
    {
        A00[j] = a00 * j; A01[j] = a01 * j; A02[j] = a02 * j;
        A10[j] = a10 * j; A11[j] = a11 * j; A12[j] = a12 * j;
        A20[j] = a20 * j; A21[j] = a21 * j; A22[j] = a22 * j;
    }
 
    double *my_pow = new double[MAXV];
    for (j = 0; j < MAXV; j++)
        my_pow[j]= (double)(116.0 * pow(j/255.0, 0.3333333) - 16);
 
    for (j = 0; j < _n_colors; j++)
    {
        temp_col = _col_RGB[j];
        int R = temp_col>>16; 
        int G = (temp_col>>8) & 255; 
        int B = temp_col & 255;

        x = A00[R] + A01[G] + A02[B];
        y = A10[R] + A11[G] + A12[B];
        z = A20[R] + A21[G] + A22[B];
    
        double  tval = (double)(y / 2550000.0); //Yn==1
        if ( tval >  Lt)  
            l_star = my_pow[(int)(tval*255+0.5)];
        else
            l_star = 903.3 * tval;

        my_temp = x + 15 * y + 3 * z;
        if(my_temp)
        {
            u_prime = (double)(x << 2) / (double)(my_temp);
            v_prime = (double)(9 * y) / (double)(my_temp);
        }
        else
        {
            u_prime = 4.0;
            v_prime = 9.0 / 15.0;
        }
    
        tval = 13 * l_star;
        u_star = tval * (u_prime - Un_prime); // Un_prime = 0.1978
        v_star = tval * (v_prime - Vn_prime); // Vn_prime = 0.4683

        _col0[j] = (int)(l_star+0.5);
        if(u_star>0) 
            _col1[j] = (int)(u_star+0.5);
        else 
            _col1[j] = (int)(u_star-0.5);
  
        if(v_star>0) 
            _col2[j] = (int)(v_star+0.5);
        else
            _col2[j] = (int)(v_star-0.5);
    }
    
    for (j = 0; j < count; j++)
    {
        temp_col = grey(j);
        temp_ind = _col_misc[temp_col>>6];
        for (k = temp_ind; k < temp_ind + SEC_SIZE; k++)
            if(_col_RGB[k] == temp_col)
            {
                temp_index = _col_index[j] = k;
                break;
            }
        L(j) = _col0[temp_index];
        U(j) = _col1[temp_index];
        V(j) = _col2[temp_index];
    }   
    delete [] my_pow;
    delete [] A22; delete [] A21; delete [] A20;
    delete [] A12; delete [] A11; delete [] A10;
    delete [] A02; delete [] A01; delete [] A00;
//    cerr<<":";
}

///////////////////////////////////////////////////////////////////
// LUV (final_T[]) to RGB (TI[]) conversion
template<class Tp>
void Raster_Color<Tp>::convert_LUV_RGB(double *pLUV, int *pRGB, long selects)
{
    int        i;
    double    R_G_B[3][3] = { {(double)3.2405, (double)-1.5371, (double)-0.4985 },
                            {(double)-0.9693, (double)1.8760, (double)0.0416 },
                            {(double)0.0556, (double)-0.2040, (double)1.0573 }};

    // this condition is always true
    if (selects & Lightness && selects & Ustar && selects & Vstar)
    {
        if(pLUV[0] < 0.1)
        {
            pRGB[0]=0;
            pRGB[1]=0;
            pRGB[2]=0;
        }
        else
        {
            double my_x, my_y, my_z;
            if (pLUV[0] < 8.0)
                my_y = Yn * pLUV[0] / 903.3;
            else
                my_y = Yn * pow((pLUV[0] + 16.0) / 116.0, 3);

            double u_prime = pLUV[1] / (13 * pLUV[0]) + Un_prime;
            double v_prime = pLUV[2] / (13 * pLUV[0]) + Vn_prime;

            my_x = 9 * u_prime * my_y / (4 * v_prime);
            my_z = (12 - 3 * u_prime - 20 * v_prime) * my_y / (4 * v_prime);

            pRGB[0] = int((R_G_B[0][0]*my_x + R_G_B[0][1]*my_y + R_G_B[0][2]*my_z)*255.0);
            pRGB[1] = int((R_G_B[1][0]*my_x + R_G_B[1][1]*my_y + R_G_B[1][2]*my_z)*255.0);
            pRGB[2] = int((R_G_B[2][0]*my_x + R_G_B[2][1]*my_y + R_G_B[2][2]*my_z)*255.0);
 
            for (i = 0; i < 3; i++)
            {
                if (pRGB[i] > 255) pRGB[i] = 255;
                else if (pRGB[i] < 0) pRGB[i] = 0;
            }
        }
    }
    else
    {
        pRGB[0] = (int)pLUV[0];
        pRGB[1] = (int)pLUV[1];
        pRGB[2] = (int)pLUV[2];
    }
}

template<class Tp>
void Raster_Color<Tp>::ToRGB(void)
{
    int i,j,pRGB[3];
    double pLUV[3];

    for (j=0; j<rows(); j++)
        for (i=0; i<cols(); i++)
        {
            pLUV[0] = (double)L(j,i);
            pLUV[1] = (double)U(j,i);
            pLUV[2] = (double)V(j,i);
            
            convert_LUV_RGB(pLUV, pRGB);

            R(j,i) = (Tp)(pRGB[0]);
            G(j,i) = (Tp)(pRGB[1]);
            B(j,i) = (Tp)(pRGB[2]);
        }
}


//////////////////////////////////////////////////////////////
// 3-D Histogram computation
// Implement a trade-off between speed and required memory
// Used in 3-D histogram computation
static const int FIRST_SIZE=262144; // 2^18

template<class Tp>
void Raster_Color<Tp>::histogram(long selects)
{
    // free assigned space
    Free_histogram_space();

    int *first_tab= new int[FIRST_SIZE];
    int **third_tab;
    int *fourth_tab;
    int *fifth_tab=new int[SEC_SIZE];
    uchar oR,oG,oB;
    
    _n_colors=0;
  
    long k,j,p,r;
    int temp_ind, sec_ind, third_ind;
  
    int first_contor=0, third_contor=0;
  
    memset(first_tab, 0, sizeof(int) * FIRST_SIZE);
    _col_misc= new int[FIRST_SIZE]; 
    memset(_col_misc, 0, sizeof(int) * FIRST_SIZE);
  
    //first_tab -> how many
    for(k = 0; k < count; k++)
    {
        oR = (uchar)R(k);
        oG = (uchar)G(k);
        oB = (uchar)B(k);

        temp_ind = (((oR<<8)+oG)<<2)+(oB>>6);
        first_tab[temp_ind]++;
    }
    //_col_misc -> memo position
    for(k = 0; k < FIRST_SIZE; k++)
        if(first_tab[k])
        {
            _col_misc[k]=first_contor;
            first_contor++;
        }
    //contors
    fourth_tab = new int[first_contor];
    memset(fourth_tab, 0, sizeof(int) * first_contor);
    //tab of pointers to reduced colors
    third_tab = new int *[first_contor]; 
    first_contor = 0;
    for (k =0 ; k < FIRST_SIZE; k++)
        if(first_tab[k])
        { 
            third_tab[first_contor] = new int[first_tab[k]];       
            first_contor++;
        }
        
    for (k = 0; k < count; k++)
    {
        oR = (uchar)R(k);
        oG = (uchar)G(k);
        oB = (uchar)B(k);

        temp_ind = (((oR<<8)+oG)<<2)+(oB>>6);
        sec_ind = oB & 63;
        third_ind=_col_misc[temp_ind];      
        third_tab[third_ind][fourth_tab[third_ind]]  = sec_ind;
        fourth_tab[third_ind]++;
    }

    for (k = 0; k < first_contor; k++)
    {
        memset(fifth_tab, 0, sizeof(int)*SEC_SIZE);
        for (j=0;j<fourth_tab[k];j++)
            fifth_tab[third_tab[k][j]]++;
        for (j = 0; j < SEC_SIZE; j++)
            if(fifth_tab[j])
                _n_colors++;
    }

    _col_RGB=new int[_n_colors];
    _m_colors=new int[_n_colors];
  
    k = 0; p = 0;
    for (r = 0; r < FIRST_SIZE; r++)
        if(first_tab[r])
        {
            memset(fifth_tab,0,sizeof(int)*SEC_SIZE);
            for(j = 0; j < fourth_tab[k]; j++)
                fifth_tab[third_tab[k][j]]++;
            _col_misc[r] = p;
            for(j = 0; j < SEC_SIZE; j++)
                if(fifth_tab[j])
                {         
                    _col_RGB[p] = (r<<6) + j;
                    _m_colors[p] = fifth_tab[j];
                    p++;
                }
            delete [] third_tab[k];
            k++;
        }
    
    delete [] third_tab;
    delete [] fourth_tab;
    delete [] fifth_tab;
    delete [] first_tab;
  
    _col_all = new int*[3];
    _col0=_col_all[0] = new int[_n_colors];
    _col1=_col_all[1] = new int[_n_colors];
    _col2=_col_all[2] = new int[_n_colors];
    _col_index = new int[count];
//    cerr<<":";
}

template<class Tp>
void Raster_Color<Tp>::CreateLUVspace(void)
{
    long    i;

    // get histogram of this image
    histogram();
    for (i = 0; i < 3; i++)
        LUV[i].SetDimension(height(), width());
    convert_RGB_LUV();
}

// for color indexing
template<class Tp>
Matrix<int> Raster_Color<Tp>::Chromaticity(int r_g_b)
{
    double        d_chr;
    Matrix<int>    chr(rows(),cols());
    int            i,j,R_G_B;

    for (j=0; j<rows(); j++)
        for (i=0; i<cols(); i++)
        {
            R_G_B = (int)(R(j,i)+G(j,i)+B(j,i));
            if (R_G_B == 0)
                d_chr = 1.0;
            else
            {
                if (r_g_b == RedColor)
                    d_chr = ((double)R(j,i))/((double)(R_G_B));
                else
                    d_chr = ((double)G(j,i))/((double)(R_G_B));
            }

            chr(j,i) = (int)(d_chr*255);
        }
    return chr;
}

template<class Tp>
void Raster_Color<Tp>::SetPixelInLUV(const int row, const int col,
                            const int l, const int u, const int v)
{
    double    luv[3];
    int        rgb[3];

    luv[0] = double(l);
    luv[1] = double(u);
    luv[2] = double(v);
    
    convert_LUV_RGB(luv, rgb);
    R(row,col) = (Tp)rgb[0];
    G(row,col) = (Tp)rgb[1];
    B(row,col) = (Tp)rgb[2];
}

template<class Tp>
void Raster_Color<Tp>::MeanVariance(Matrix<int> &mask_label, irmm_Rect &bound,
                                 const int num_label,
                                 Matrix<double>    *pmeans,
                                 Matrix<double> *pvariances,
                                 Matrix<long> &counts)
{
    int i,j,m,n,k;

    if (pmeans == NULL) return;
    long *pnum_pixel=new long[num_label];

    for (i=0; i<num_label; i++)
    {
        pnum_pixel[i] = 0;
        pmeans[i].SetDimension(3,1);
        pmeans[i].init_value(0.0);
        if (pvariances != NULL)
        {
            pvariances[i].SetDimension(3,3);
            pvariances[i].init_value(0.0);
        }
    }
    counts.SetDimension(num_label,1);

    // compute the mean value
    for (j=bound.top; j<=bound.bottom; j++)
        for (i=bound.left; i<=bound.right; i++)
        {
            if (mask_label(j,i)>0 && mask_label(j,i)<=num_label)
            {
                k = mask_label(j,i)-1;
                pnum_pixel[k]++;
                pmeans[k](0) += (double)L(j,i);
                pmeans[k](1) += (double)U(j,i);
                pmeans[k](2) += (double)V(j,i);
            }
        }
    for (i=0; i<num_label; i++)
    {
        if (pnum_pixel[i]>0)
        {
            pmeans[i](0) = pmeans[i](0)/((double)pnum_pixel[i]);
            pmeans[i](1) = pmeans[i](1)/((double)pnum_pixel[i]);
            pmeans[i](2) = pmeans[i](2)/((double)pnum_pixel[i]);
        }
        else
        {
            pmeans[i](0) = 0.0;
            pmeans[i](1) = 0.0;
            pmeans[i](2) = 0.0;
        }
    }
    
    if (pvariances != NULL)
    {
        for (j=bound.top; j<=bound.bottom; j++)
            for (i=bound.left; i<=bound.right; i++)
            {
                if (mask_label(j,i)>0 && mask_label(j,i)<=num_label)
                {
                    k = mask_label(j,i)-1;
                    for (n=0; n<3; n++)
                        for (m=0; m<3; m++)
                            pvariances[k](n,m) += ((double)(LUV[n](j,i)-pmeans[k](n)))*
                                                  ((double)(LUV[m](j,i)-pmeans[k](m)));
                }
            }
        for (i=0; i<num_label; i++)
        {
            for (n=0; n<3; n++)
                for (m=0; m<3; m++)
                {
                    if (pnum_pixel[i]>1)
                        pvariances[i](n,m) = pvariances[i](n,m)/(pnum_pixel[i]-1);
                }
            counts(i) = (long)pnum_pixel[i];
        }
        // make sure the determinate of covariance matrix is not zero
        for (i=0; i<num_label; i++)
        {
            if (pvariances[i].Det()==0)
                pvariances[i].Identity();
        }
    }
    delete []pnum_pixel;
}

template<class Tp>
void Raster_Color<Tp>::Compute2DColor(Matrix<int> &mask_label, irmm_Rect &bound,const int num_label, Vector<double> *pvc_N)
{
    int                i,j,m,k;
    double            din;
    Vector<double>    vc_weights,vc_temp(3);

    if (pvc_N == NULL) return;

    for (i=0; i<num_label; i++)
    {
        pvc_N[i].SetRowVector(3);
        pvc_N[i].init_value(0.0);
    }
    vc_weights.SetRowVector(num_label);
    vc_weights.init_value(0.0);

    // compute the major direction
    for (j=bound.top; j<=bound.bottom; j++)
        for (i=bound.left; i<=bound.right; i++)
        {
            if (mask_label(j,i)>=0 && mask_label(j,i)<num_label)
            {
                k = mask_label(j,i);

                vc_temp(0) = (double)R(j,i);
                vc_temp(1) = (double)G(j,i);
                vc_temp(2) = (double)B(j,i);
                
                // get intensity
                din = vc_temp.Norm(2.0);
                // normalize it
                if (din>0.0)
                {
                    for (m=0; m<3; m++)
                    {
                        vc_temp(m) = vc_temp(m)/din;
                        pvc_N[k](m) = (pvc_N[k](m)*vc_weights(k)+din*vc_temp(m))/(vc_weights(k)+din);
                    }
                    vc_weights(k) += din;
                }
            }
        }

    for (k=0; k<num_label; k++)
    {
        din = pvc_N[k].Norm(2.0);
        if (din>0.0)
            pvc_N[k] = pvc_N[k]/din;
    }
}
void Matrixu2RasterColoru(Matrixu &I, Raster_Color<uchar> &R);
#endif

