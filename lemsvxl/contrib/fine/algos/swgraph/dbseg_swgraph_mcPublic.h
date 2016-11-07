/**********************************************************************
*math.h - definitions and declarations for my math library
*
*Author : Zhuowen Tu
*
*Purpose:
*       This file contains constant definitions and external subroutine
*       declarations for the math subroutine library.
*
*       [Public]
*
***********************************************************************/
#ifndef _MC_PUBLIC_H
#define _MC_PUBLIC_H

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <ios>
#include <iostream>

#include "Point.h"
//#include "mcPoint3D.h"
//#include "mcImagePoint.h"

#define EBSHLON            1e-10                                //definition of minim value    
#define    INFINITE_D        1e+99                                //definition of maxim value
#define    INCH_TO_METER    0.3048

#ifndef PI
#define    PI                3.1415926535897931
#endif

#define INTENSITY_DELTA        6.0
#define INTENSITY_LOW        35.0
#define INTENSITY_HIGH        255.0

#define DIS_SMALL        0.05

//typedef class DpMcPoint3D Coord3D;

// tansfer degree to radious
inline double DegToRad(double dDeg) {return dDeg * PI / 180;}

// tansfer degree to radious
inline double RadToDeg(double dRad) {return dRad * 180 / PI;}

inline double round(const double a)
{
    double b,c;

    b = floor(a);
    c = fmod(a,1.0);
    if (c>=0.5)
        b = b+1;
    return b;
}

//get the not less integer of a floating value
inline int    NotLessInt(double Value)
{
    int Rtn;
    
    Rtn = (int)Value;
    if (Rtn < Value)
        Rtn++;
    return Rtn;
}

//crossing situation of two lines
#define            CROSSING        1
#define            DONT_INTERSERT    0
#define            PARALLE         -1
#define            ONE_POINT        -2

//calculation of distance between two points
double    Distance(double x1, double y1, double x2, double y2);
double    Distance2(double x1, double y1, double x2, double y2);
inline double    Distance(McPoint2D xy1, McPoint2D xy2) {return Distance(xy1.x, xy1.y, xy2.x, xy2.y);};
inline double    Distance(McImagePoint xy1, McImagePoint xy2) {return Distance(xy1.X(), xy1.Y(), xy2.X(), xy2.Y());};
double  Distance(double x1, double y1, double z1, double x2, double y2, double z2);
inline double   Distance(McPoint3D xyz1, McPoint3D xyz2) {return Distance(xyz1.x, xyz1.y, xyz1.z, xyz2.x, xyz2.y, xyz2.z);};

//calculation of two lines
int        ItstOfLines(McPoint2D *ppt2dCrossing, McPoint2D xy11, McPoint2D xy12, 
                    McPoint2D xy21, McPoint2D xy22);

//calculation of perpendicular distance between a point and a line
double  PerDistance(const McPoint2D &pt2d, const McPoint2D &ptLnBg, const McPoint2D &ptLnEd);
double  PerDistance2(McPoint2D &pt2d, McPoint2D &ptLnBg, McPoint2D &ptLnEd);
bool    PointOnLeftSide(McPoint2D &pt2d, McPoint2D &ptLnBg, McPoint2D &ptLnEd);

//calculation of the angle between x axis and line
double    xysa(double x1, double y1, double x2, double y2);

//calculation of the angle between inputed two lines 
double    AngleOfTwoLines(McPoint2D xy11, McPoint2D xy12, McPoint2D xy21, McPoint2D xy22);
//calculation of area
double    area(double *x, double *y, int cnt);

//cut blanks in string
void    CutBlank(char * lpszDest, char * lpszSour);
void    CutEndBlank(char * lpszDest, char * lpszSour);
void    CutHeadBlank(char * lpszDest, char * lpszSour);
void    GetFirstWord(char * lpszDest, char * lpszSour, char cSeprator, bool bChangeSour=false);

//point stays on input line or not
bool    PointOnLine(McPoint2D point, McPoint2D ptVertex1, McPoint2D ptVertex2);

//point lies inside or ouside the given polygon
#define OUTSIDE            0    //outside
#define INSIDE_CW        1    //inside of a clockwise polygon
#define INSIDE_CCW        2     //inside of a counterclockwise polygon
int        VeriPoint(McPoint2D *pVertices, int iVrtcCt, double x, double y);
bool    IsLineCrossPlgn(const McPoint2D ptFirst, const McPoint2D ptEnd, 
                        McPoint2D* pPts, const int iPtCnt);

inline    double    InchToMeter(double a){return a*12*0.0254;};

void DoubleToString(char *buf, double value);

#define min(a, b)  (((a) < (b)) ? (a) : (b))
#define max(a, b)  (((a) > (b)) ? (a) : (b))

template <class Tp>
int Sign(Tp s)
{
    if (s>0) return 1;
    if (s<0) return -1;
    return 0;
}

// arctg function
inline double Atan2(const double y, const double x)
{
    double theta=atan2(y,x);
    if (theta<0)
        theta += 2*PI;
    return theta;
}

// perpendicular angle
// x 0~2*PI
inline double PerPenAngle(const double x)
{
    double theta = PI/2+x;
    if (theta>=2*PI)
        theta -= 2*PI;
    return theta;
}

inline double AngleOPI(const double alfa)
{
    double theta = alfa;
    if (theta<0)
        theta += PI*2;
    if (theta>=PI)
        theta = theta-PI;
    return theta;
}

inline double AngleO2PI(const double alfa)
{
    double theta = alfa;
    while (theta<0)
        theta += PI*2;
    while (theta>2*PI)
        theta = theta-2*PI;
    return theta;
}


template <class Tp>
Tp ABS(const Tp x)
{
    if (x>0) return x;
    else     return -x;
}


inline double factorial(const int i)
{
    int k;
    double d=1.0;

    if (i>0)
    {
        for (k=1; k<=i; k++)
            d = d*((double)i);
    }
    return d;
}

inline double choose_i_k(const int i, const int k)
{
    double a1,a2,a3;

    if (k==1 || k==i-1)
        return (double)i;
    else if (k==2)
    {
        return ((double)i)*(i-1)/2.0;
    }

    a1 = factorial(i);
    a2 = factorial(k);
    a3 = factorial(i-k);
    
    return a1/a2/a3;
}

inline int IntClose(double x)
{
    double y1 = (double)ceil(x);
    double y2 = (double)floor(x);
    if (fabs(y1-x)<fabs(y2-x))
        return (int)y1;
    else
        return (int)y2;
}


inline double    ProbFromEnergy(const double energy)
{    if (energy>700)    return 0.0;
    else            return exp(-energy);
};

#endif

