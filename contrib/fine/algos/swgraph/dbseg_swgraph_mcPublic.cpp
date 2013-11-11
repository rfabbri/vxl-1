/**********************************************************************
*MyMath.CPP - Functions for geranl caculation in math and graphcs
*
*Author : Zhuowen Tu
*
*Purpose:
*       This file contains constant definitions and external subroutine
*       declarations for the math subroutine library.
*
*       [Public]
*        ItstOfLines (McPoint2D *ppt2dCrossing, McPoint2D xy11, McPoint2D xy12, 
                             McPoint2D xy21, McPoint2D xy22);
        xysa(double x1,double y1,double *x2,double *y2);
        AngleOfTwoLines(McPoint2D xy11, McPoint2D xy12, McPoint2D xy21, McPoint2D xy22);
        Distance(double x1, double y1, double x2, double y2);
        CutHeadBlank(char * lpszDest, char * lpszSour);
        CutEndBlank(char * lpszDest, char * lpszsour);
        CutBlank(char * lpszDest, char * lpszsour);
        PointOnLine(McPoint2D point, McPoint2D ptVertex1, McPoint2D ptVertex2)
***********************************************************************/
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>
//#include "nrutil.h"
//#include "rc_common.h"

#include "mcPublic.h"

#define FINITE_DECISION 1e8

//Caculate the dDisance of two points
double    Distance(double x1, double y1, double x2, double y2)
{
    double dDist;

    dDist = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
//    dDist = floor(dDist*FINITE_DECISION)/FINITE_DECISION;
    return dDist;
}

double    Distance2(double x1, double y1, double x2, double y2)
{
    double dist=Distance(x1,y1,x2,y2);
    dist = floor(dist*FINITE_DECISION)/FINITE_DECISION;
    return dist;
}

double   Distance(double x1, double y1, double z1, double x2, double y2, double z2)
{
    double dDist;

    dDist = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1));
//    dDist = floor(dDist*FINITE_DECISION)/FINITE_DECISION;
    return dDist;
}

//calculation of perpendicular distance between a point and a line
double PerDistance(const McPoint2D &pt2d, const McPoint2D &ptLnBg, const McPoint2D &ptLnEd)
{
    double dEdge1, dEdge2, dEdge3, dAng, dist;

    //calculation of the distances of three edges
    dEdge1 = Distance(pt2d, ptLnBg);
    dEdge2 = Distance(ptLnBg, ptLnEd);
    dEdge3 = Distance(ptLnEd, pt2d);
    //calculation of angle
    dAng = fabs(xysa(pt2d.x, pt2d.y, ptLnBg.x, ptLnBg.y) -
                xysa(pt2d.x, pt2d.y, ptLnEd.x, ptLnEd.y));
    if (dEdge2 > EBSHLON)
        dist = fabs(dEdge1 * dEdge3 * sin(dAng) / dEdge2);
    else
        dist = 0.0;
//    dist = floor(dist*FINITE_DECISION)/FINITE_DECISION;
    return dist;
}

double PerDistance2(McPoint2D &pt2d, McPoint2D &ptLnBg, McPoint2D &ptLnEd)
{
    double dist = PerDistance(pt2d,ptLnBg,ptLnEd);
    dist = floor(dist*FINITE_DECISION)/FINITE_DECISION;
    return dist;
}

bool PointOnLeftSide(McPoint2D &pt2d, McPoint2D &ptLnBg, McPoint2D &ptLnEd)
{
    double angle1,angle2;

    angle1 = xysa(ptLnBg.x, ptLnBg.y, ptLnEd.x, ptLnEd.y);
    angle2 = xysa(ptLnBg.x, ptLnBg.y, pt2d.x, pt2d.y);

    if (AngleO2PI(angle1-angle2)<PI)
        return false;    // on the right side of the segment
    else
        return true;    // on the left side of the segment
}

/***************************************************************************************************
 Name     : ItstOfLines (McPoint2D *ppt2dCrossing, McPoint2D xy11, McPoint2D xy12, 
                                 McPoint2D xy21, McPoint2D xy22)
 Function : Caculate the Intersection of two lines
            ppt2dCrossing -- The coordinates of the intersection of these two lines
            xy11          -- The first point of first line    
            xy12          -- The second point of first line
            xy21          -- The first point of second line
            xy22          -- The second point of second line
 Return   : CROSSING      -- Has intersection
            DONT_INTERSERT-- Don't intersect
            PARALLE        -- Two paralle lines
            ONE_POINT      -- Either first or seconde is actually a point
 Date      : 2/19/98
****************************************************************************************************/
int  ItstOfLines(McPoint2D *ppt2dCrossing, McPoint2D xy11, McPoint2D xy12, McPoint2D xy21, McPoint2D xy22)
{ 
    double   dK1,dK2;
    McPoint2D  pt2dResult;
    
    if (fabs(xy11.x-xy12.x) < EBSHLON && fabs(xy21.x-xy22.x) < EBSHLON)
        return PARALLE;                            //  two lines vertical
    if (fabs(xy11.x - xy12.x) > EBSHLON && fabs(xy21.x - xy22.x)> EBSHLON)
    {
        // no line vertical
        dK1 = (xy12.y - xy11.y) / (xy12.x-xy11.x);
        dK2 = (xy22.y - xy21.y) / (xy22.x-xy21.x);
        if (fabs(dK1 - dK2) < EBSHLON)
            return PARALLE;                         // two lines paralle
        else
        {
            pt2dResult.x = (-dK2 * xy21.x + xy21.y + dK1 * xy11.x - xy11.y)
                            / (dK1 - dK2);
            pt2dResult.y= dK1 * (pt2dResult.x - xy11.x) + xy11.y;
            if (ppt2dCrossing != NULL)
                (*ppt2dCrossing) = pt2dResult;

            if ((pt2dResult.x - xy11.x) * (pt2dResult.x - xy12.x) <= EBSHLON
                &&(pt2dResult.x - xy21.x) * (pt2dResult.x - xy22.x) <= EBSHLON
                && (pt2dResult.y - xy11.y) * (pt2dResult.y - xy12.y) <= EBSHLON
                && (pt2dResult.y - xy21.y) * (pt2dResult.y - xy22.y) <= EBSHLON)
                return CROSSING;                       // cross
            else
                return DONT_INTERSERT;                  // donot cross
        }
    }
    else
    {
        if (fabs(xy11.x - xy12.x) < EBSHLON)           // line vertical
        {
            dK2 = (xy22.y - xy21.y) / (xy22.x - xy21.x);
            if (fabs(xy11.y - xy12.y) < EBSHLON)
                return ONE_POINT;                      // a pointer
            pt2dResult.x = xy11.x;
            pt2dResult.y = dK2 * (pt2dResult.x - xy21.x) + xy21.y;
            if (ppt2dCrossing != NULL)
                (*ppt2dCrossing) = pt2dResult;
            if ((pt2dResult.x-xy21.x) * (pt2dResult.x-xy22.x) <= EBSHLON
                &&(pt2dResult.y-xy11.y) * (pt2dResult.y-xy12.y) <= EBSHLON
                &&(pt2dResult.y-xy21.y) * (pt2dResult.y-xy22.y) <= EBSHLON)
                return CROSSING;
            else
                return DONT_INTERSERT;                      // outline
        }
        else
        {
            dK1 = (xy12.y - xy11.y) / (xy12.x - xy11.x);
            if (fabs(xy21.y - xy22.y) < EBSHLON)
                return ONE_POINT;                    // a pointer
            pt2dResult.x = xy21.x;
            pt2dResult.y = dK1*(pt2dResult.x-xy11.x)+xy11.y;
            if (ppt2dCrossing != NULL)
                (*ppt2dCrossing) = pt2dResult;

            if ((pt2dResult.x-xy11.x) * (pt2dResult.x-xy12.x) <= EBSHLON
                &&(pt2dResult.x-xy21.x) * (pt2dResult.x-xy22.x) <= EBSHLON
                &&(pt2dResult.y-xy11.y) * (pt2dResult.y-xy12.y) <= EBSHLON
                &&(pt2dResult.y-xy21.y) * (pt2dResult.y-xy22.y) <= EBSHLON)
                return CROSSING;
            else
                return DONT_INTERSERT;
        }
     }
}

/***************************************************************************************************
 Name     : xysa(double x1,double y1,double x2,double y2)
 Function : Caculate the angle between inputed line and x axis with the consideration
            of direction
            x1    -- The x coordinate of first point
            y1    -- The y coordinate of first point
            x2  -- The x coordinate of second point
            y2    -- The x coordinate of second point
 Return   : caculated angle (0 ~ 2PI)
 Date      : 2/19/98
****************************************************************************************************/
double xysa(double x1, double y1, double x2, double y2)
{
    double angle,dDis;

    dDis = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    if(dDis <= EBSHLON)
    {
        angle=0.0;
        return(angle);
    }

    angle = acos((x2 - x1) / dDis);
    if(y2 < y1)
        angle = 2 * PI - angle;
    return(angle);
}

/***************************************************************************************************
 Name     : double AngleOfTwoLines(McPoint2D xy11, McPoint2D xy12, McPoint2D xy21, McPoint2D xy22)
 Function : Caculate the angle between inputed two lines 
            xy11          -- The first point of first line    
            xy12          -- The second point of first line
            xy21          -- The first point of second line
            xy22          -- The second point of second line
 Return   : caculated angle (-PI ~ +PI)
 Date      : 2/19/98
****************************************************************************************************/
double AngleOfTwoLines(McPoint2D xy11, McPoint2D xy12, McPoint2D xy21, McPoint2D xy22)
{
    
    double dAngle1, dAngle2, dAngleResult;
    
    //caculate the angle between first line and x axis
    dAngle1 = xysa(xy11.x, xy11.y, xy12.x, xy12.y);

    //caculate the angle between sencond line and x axis
    dAngle2 = xysa(xy21.x, xy21.y, xy22.x, xy22.y);

    //caculate the angle between first line and second line
    dAngleResult = dAngle1 - dAngle2;
    if(dAngleResult < -PI)
        dAngleResult += 2 * PI;
    if(dAngleResult > PI)
        dAngleResult -= 2 * PI;
    return(dAngleResult);
}

/***************************************************************************************************
 Name     : bool  PointOnLine(McPoint2D point, McPoint2D ptVertex1, McPoint2D ptVertex2)
 Function : Determine if the input point stays on input line
            point     -- input point
            ptVertex1 -- the first vertex
            ptVertex2 -- the second vertex
 Return   : true  if stays
            false if not
 Date      : 5/2/98
**************************************************************************************************/
bool    PointOnLine(McPoint2D point, McPoint2D ptVertex1, McPoint2D ptVertex2)
{
    double    dDstPtVx1, dAng, dDstPtLn = 100;

    if (Distance(point,ptVertex1)<DIS_SMALL || Distance(point,ptVertex2)<DIS_SMALL)
        return true;
    if(point.x <= __max(ptVertex1.x, ptVertex2.x) &&
       point.x >= __min(ptVertex1.x, ptVertex2.x) &&
       point.y <= __max(ptVertex1.y, ptVertex2.y) &&
       point.y >= __min(ptVertex1.y, ptVertex2.y))
    {
        dDstPtVx1 = Distance(ptVertex1.x, ptVertex1.y, point.x, point.y);
    
        dAng = AngleOfTwoLines(ptVertex1, ptVertex2, ptVertex1, point);

        dDstPtLn = fabs(dDstPtVx1 * sin(dAng));
    }
    if(dDstPtLn < DIS_SMALL)
        return    true;
    else
        return    false;
}

/***************************************************************************************************
 Name     : CutHeadBlank(char * lpszDest, char * lpszSour);
            CutEndBlank(char * lpszDest, char * lpszsour);
            CutBlank(char * lpszDest, char * lpszsour);
 Function : Cut blanks which are at the beginning or end of a string
            lpszDest : string holding final result
            lpszSour : string having source 
 Date      : 4/15/98
****************************************************************************************************/
void CutHeadBlank(char * lpszDest, char * lpszSour)
{
    char * lpszMove;

    if(lpszDest == NULL || lpszSour == NULL)
        return;
    
    lpszMove = lpszSour;
    while(strlen(lpszMove) > 0 && lpszMove[0] == ' ')
        lpszMove ++;
    strcpy(lpszDest, lpszMove);
}
void CutEndBlank(char * lpszDest, char * lpszSour)
{
    if(lpszDest == NULL || lpszSour == NULL)
        return;

    strcpy(lpszDest, lpszSour);

    //Delete balnk at the end of string
    while(strlen(lpszDest) >0 && lpszDest[strlen(lpszDest) - 1] == ' ')
        lpszDest[strlen(lpszDest) - 1] = '\0';
}
void CutBlank(char * lpszDest, char * lpszSour)
{
    if(lpszDest == NULL || lpszSour == NULL)
        return;
    CutEndBlank(lpszDest, lpszSour);
    CutHeadBlank(lpszDest, lpszDest);
}

/******************************************************************************/
/* Function     : int GSShowLightPole(MENU_ARGS)                              */
/* Descriptions :  Display a light pole templte on the image                  */
/*                 MENU_ARGS is explained  in .def file. Always re-           */
/*                turn true.                                                  */
/******************************************************************************/
char * GetValue(char *str)
{
    char *tmp, *tmp1;

    tmp = strchr(str, ':');
    if(tmp != NULL)
    {
        tmp++;
        while(*tmp == ' ') tmp++;
        tmp1 = tmp + strlen(tmp) - 1;
        while((*tmp1 == '\n') || (*tmp1 == ' ') )
        {
            *tmp1 = '\0';
            tmp1--;
        }
    }
    else
        return str;
    return tmp;
}


void GetFirstWord(char * lpszDest, char * lpszSour, char cSeprator,
                  bool bChangeSour)
{
    char  *pBuf, *pSearch;

    if (strlen(lpszSour) > 0 && lpszDest != NULL)
    {
        pBuf = new char[strlen(lpszSour) + 1];
        strcpy(pBuf, lpszSour);

        pSearch = strchr(pBuf, cSeprator);
        if (pSearch != NULL)
        {
            pSearch[0] = '\0';    //Found this character
            if(bChangeSour)
                strcpy(lpszSour, pSearch + 1);
        }
        else
            pBuf[0] = '\0';        //Didn't find this character
        CutBlank(pBuf, pBuf);

        strcpy(lpszDest, pBuf);

        delete pBuf;
    }

}

/***************************************************************************************************
 Name     : VeriPoint(PP *p,int iVrtcCt,double x,double y)
 Function : Does this point ly within supplied polygon?
            pVertices : stores data of vertices
            iVrtcCt   : count of vertices
            x          : x coordinate of testing point
            y          : y coordinate of testing point
 return   : PTOUTSIDE  testing point is outside of this polygon
            PTINSIDE_CLK testing point is inside a clockwise polygon
            PTINSIDE_ANTI testing point is inside an anticlockwise polygon
 Date      : 5/22/98
****************************************************************************************************/
int VeriPoint(McPoint2D *pVertices, int iVrtcCt, double x, double y)
{
    double    anglep, preangle, sumangle=0, curangle, angle0;
    int        i=0;
    McPoint2D    s;

    preangle=xysa(x, y, pVertices[0].x, pVertices[0].y);
    anglep = preangle;
    angle0 = preangle;
    i++;
    while (i < iVrtcCt)
    {
        s.Set(x,y);
        if (PointOnLine(s, pVertices[i], pVertices[(i+1)%iVrtcCt]))
            return INSIDE_CW;
        anglep = xysa(x, y, pVertices[i].x, pVertices[i].y);
        curangle = anglep - preangle;
        if (curangle > PI)
            curangle = curangle-2 * PI;
        else if (curangle <= -PI)
            curangle = 2 * PI + curangle;
        sumangle += curangle;
        preangle = anglep;
        i++;
    } 
    curangle = angle0-preangle;
    if (curangle >= PI)
        curangle = curangle-2*PI;
    else if (curangle <= -PI)
        curangle = 2 * PI + curangle;
    sumangle += curangle;
    if ((sumangle <= PI) && (sumangle >= -PI))
        return OUTSIDE;        //outside;
    else if (sumangle>PI)
         return INSIDE_CW;    //inside, clockwise
    else return INSIDE_CCW; //inside, counterclockwise
}

bool IsLineCrossPlgn(const McPoint2D ptFirst, const McPoint2D ptEnd,
                     McPoint2D* pPts, const int iPtCnt)
{
    int            i;
    McPoint2D    pt1, pt2;

    pt1 = ptFirst;
    pt2 = ptEnd;

    for (i = 0; i < iPtCnt; i++)
        if (ItstOfLines(NULL, pt1, pt2, pPts[i], pPts[(i+1)%iPtCnt]) == CROSSING)
            true;

    return false;
}

//calculation of area of a region
double area(double *x, double *y, int cnt)
{
    double    dResult = 0;
    int        i;

    if (x != NULL && y != NULL && cnt > 2)
    {
        // valid region
        for (i = 0; i < cnt; i++)
            dResult += x[i]*(y[(i+cnt-1)%cnt]-y[(i+1)%cnt]);
        dResult = fabs(dResult)/2;
    }
    return dResult;
}

#define ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);\
    a[k][l]=h+s*(g-h*tau);



void DoubleToString(char *buf, double value)
{
    if (fabs(value)>1e10 || fabs(value)<1e-10)
        sprintf(buf, "%5.3e", value);
    else
        sprintf(buf,"%.7f",value);
}




