#ifndef _POINTATTRIBUTES_H
#define _POINTATTRIBUTES_H
#include "point.h"

class PointAttributes{ 
public:
    __int64 sum1,sumx,sumy;
    __int64 sumxx,sumxy,sumyy;
    PointAttributes(){
        Zero();
    }
    void operator+=(const Pointi& a);
    void operator-=(const Pointi& a);
    void operator+=(const PointAttributes& a);
    void operator-=(const PointAttributes& a);
    bool operator==(const PointAttributes& a);
    void Add(int x, int y);
    double    Get_SumSqDist(double cx,double cy,double c);
    void Zero();
};
class PointAttributes3:public PointAttributes{
public:
    __int64 sumxxx,sumxxy,sumxyy,sumyyy;
    PointAttributes3():PointAttributes(){
        sumxxx=sumxxy=sumxyy=sumyyy=0;
    }    
    void operator+=(const Pointi& a);
    void operator-=(const Pointi& a);
    void operator+=(const PointAttributes3& a);
    void operator-=(const PointAttributes3& a);
    bool operator==(const PointAttributes3& a);
    void Zero();
};
class PointAttributes4:public PointAttributes3{
public:
    __int64 sumxxxx,sumxxxy,sumxxyy,sumxyyy,sumyyyy;

    PointAttributes4():PointAttributes3(){
        sumxxxx=sumxxxy=sumxxyy=sumxyyy=sumyyyy=0;
    }    
    void operator+=(const Pointi& a);
    void operator-=(const Pointi& a);
    void operator+=(const PointAttributes4& a);
    void operator-=(const PointAttributes4& a);
    bool operator==(const PointAttributes4& a);
    double    Get_SumSqDist2Circle(double xc,double yc,double r);
    void Zero();
};
class PointValueAttributes{ 
public:
    __int64 sumb,sumbx,sumby,sumbb;
    PointValueAttributes(){
        Zero();
    }
    void Add(const Pointi& a, int b);
    void Add(int x, int y, int b);
    void Subtract(const Pointi& a, int b);
    void operator+=(const PointValueAttributes& a);
    void operator-=(const PointValueAttributes& a);
    bool operator==(const PointValueAttributes& a);
    void Zero();
};

#endif

