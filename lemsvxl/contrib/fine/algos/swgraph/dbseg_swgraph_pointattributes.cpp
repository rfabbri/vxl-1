#include "pointattributes.h"


void PointAttributes::operator+=(const Pointi& a){
    sum1+=1; 
    sumx+=a.x; 
    sumy+=a.y; 
    sumxx+=a.x*a.x; 
    sumxy+=a.x*a.y; 
    sumyy+=a.y*a.y; 
}
void PointAttributes::Add(int x, int y){
    sum1+=1; 
    sumx+=x; 
    sumy+=y; 
    sumxx+=x*x; 
    sumxy+=x*y; 
    sumyy+=y*y; 
}

void PointAttributes::operator-=(const Pointi& a){
    sum1-=1; 
    sumx-=a.x; 
    sumy-=a.y; 
    sumxx-=a.x*a.x; 
    sumxy-=a.x*a.y; 
    sumyy-=a.y*a.y; 
}
void PointAttributes::operator+=(const PointAttributes& a){
    sum1+=a.sum1; 
    sumx+=a.sumx; 
    sumy+=a.sumy; 
    sumxx+=a.sumxx; 
    sumxy+=a.sumxy; 
    sumyy+=a.sumyy; 
}
void PointAttributes::operator-=(const PointAttributes& a){
    sum1-=a.sum1; 
    sumx-=a.sumx; 
    sumy-=a.sumy; 
    sumxx-=a.sumxx; 
    sumxy-=a.sumxy; 
    sumyy-=a.sumyy; 
}
bool PointAttributes::operator==(const PointAttributes& a){
    return (sum1==a.sum1&&sumx==a.sumx&&sumy==a.sumy&&sumxx==a.sumxx&&sumxy==a.sumxy&&sumyy==a.sumyy);
}
void PointAttributes::Zero(){
    sum1=0;sumx=sumy=0;sumxx=sumxy=sumyy=0;
}
double    PointAttributes::Get_SumSqDist(double cx,double cy,double c){
    // the sum of squares of the distances from the points to the line of eq cx*x+cy*y+c=0
    return cx*cx*sumxx+cy*cy*sumyy+2*cx*cy*sumxy+2*cx*c*sumx+2*cy*c*sumy+c*c*sum1;
}
void PointAttributes3::operator+=(const Pointi& a){
    *(PointAttributes *)this+=a;
    sumxxx+=a.x*a.x*a.x; 
    sumxxy+=a.x*a.x*a.y; 
    sumxyy+=a.x*a.y*a.y; 
    sumyyy+=a.y*a.y*a.y; 
}
void PointAttributes3::operator-=(const Pointi& a){
    *(PointAttributes *)this-=a;
    sumxxx-=a.x*a.x*a.x; 
    sumxxy-=a.x*a.x*a.y; 
    sumxyy-=a.x*a.y*a.y; 
    sumyyy-=a.y*a.y*a.y; 
}
void PointAttributes3::operator+=(const PointAttributes3& a){
    *(PointAttributes *)this+=a;
    sumxxx+=a.sumxxx; 
    sumxxy+=a.sumxxy; 
    sumxyy+=a.sumxyy; 
    sumyyy+=a.sumyyy; 
}
void PointAttributes3::operator-=(const PointAttributes3& a){
    *(PointAttributes *)this-=a;
    sumxxx-=a.sumxxx; 
    sumxxy-=a.sumxxy; 
    sumxyy-=a.sumxyy; 
    sumyyy-=a.sumyyy; 
}
void PointAttributes3::Zero(){
    PointAttributes::Zero();
    sumxxx=0; 
    sumxxy=0; 
    sumxyy=0; 
    sumyyy=0; 
}
bool PointAttributes3::operator==(const PointAttributes3& a){
    return (*(PointAttributes *)this==a&&sumxxx==a.sumxxx&&sumxxy==a.sumxxy&&sumxyy==a.sumxyy&&sumyyy==a.sumyyy);
}
void PointAttributes4::operator+=(const Pointi& a){
    *(PointAttributes3 *)this+=a;
    sumxxxx+=a.x*a.x*a.x*a.x; 
    sumxxxy+=a.x*a.x*a.x*a.y; 
    sumxxyy+=a.x*a.x*a.y*a.y; 
    sumxyyy+=a.x*a.y*a.y*a.y; 
    sumyyyy+=a.y*a.y*a.y*a.y; 
}
void PointAttributes4::operator-=(const Pointi& a){
    *(PointAttributes3 *)this-=a;
    sumxxxx-=a.x*a.x*a.x*a.x; 
    sumxxxy-=a.x*a.x*a.x*a.y; 
    sumxxyy-=a.x*a.x*a.y*a.y; 
    sumxyyy-=a.x*a.y*a.y*a.y; 
    sumyyyy-=a.y*a.y*a.y*a.y; 
}
void PointAttributes4::operator+=(const PointAttributes4& a){
    *(PointAttributes3 *)this+=a;
    sumxxxx+=a.sumxxxx; 
    sumxxxy+=a.sumxxxy; 
    sumxxyy+=a.sumxxyy; 
    sumxyyy+=a.sumxyyy; 
    sumyyyy+=a.sumyyyy; 
}
void PointAttributes4::operator-=(const PointAttributes4& a){
    *(PointAttributes3 *)this-=a;
    sumxxxx-=a.sumxxxx; 
    sumxxxy-=a.sumxxxy; 
    sumxxyy-=a.sumxxyy; 
    sumxyyy-=a.sumxyyy; 
    sumyyyy-=a.sumyyyy; 
}
void PointAttributes4::Zero(){
    PointAttributes3::Zero();
    sumxxxx=0; 
    sumxxxy=0; 
    sumxxyy=0; 
    sumxyyy=0; 
    sumyyyy=0; 
}
bool PointAttributes4::operator==(const PointAttributes4& a){
    return (*(PointAttributes3 *)this==a&&sumxxxx==a.sumxxxx&&sumxxxy==a.sumxxxy&&
        sumxxyy==a.sumxxyy&&sumxyyy==a.sumxyyy&&sumyyyy==a.sumyyyy);
}

double PointAttributes4::Get_SumSqDist2Circle(double xc,double yc,double r){
    return sumxxxx-4*xc*sumxxx+6*xc*xc*sumxx-4*pow(xc,3)*sumx+pow(xc,4)+
        sumyyyy-4*yc*sumyyy+6*yc*yc*sumyy-4*pow(yc,3)*sumy+pow(yc,4)+pow(r,4)+
        2*sumxxyy+8*xc*yc*sumxy+2*yc*yc*sumxx+2*xc*xc*sumyy+2*pow(xc*yc,2)*sum1
        -4*yc*sumxxy-4*xc*sumxyy-4*xc*yc*yc*sumx-4*xc*xc*yc*sumy
        -2*r*r*(sumxx-2*xc*sumx-xc*xc*sum1+sumyy-2*yc*sumy-yc*yc*sum1);
}

void PointValueAttributes::Add(const Pointi& a, int b){
    sumb+=b; 
    sumbx+=b*a.x; 
    sumby+=b*a.y; 
    sumbb+=b*b; 
}
void PointValueAttributes::Add(int x, int y, int b){
    sumb+=b; 
    sumbx+=b*x; 
    sumby+=b*y; 
    sumbb+=b*b; 
}
void PointValueAttributes::Subtract(const Pointi& a, int b){
    sumb-=b; 
    sumbx-=b*a.x; 
    sumby-=b*a.y; 
    sumbb-=b*b; 
}
void PointValueAttributes::operator+=(const PointValueAttributes& a){
    sumb+=a.sumb; 
    sumbx+=a.sumbx; 
    sumby+=a.sumby; 
    sumbb+=a.sumbb; 
}
void PointValueAttributes::operator-=(const PointValueAttributes& a){
    sumb-=a.sumb; 
    sumbx-=a.sumbx; 
    sumby-=a.sumby; 
    sumbb-=a.sumbb; 
}
bool PointValueAttributes::operator==(const PointValueAttributes& a){
    return (sumb==a.sumb&&sumbx==a.sumbx&&sumby==a.sumby&&sumbb==a.sumbb);
}
void PointValueAttributes::Zero(){
    sumb=0;sumbx=sumby=0;sumbb=0;
}





