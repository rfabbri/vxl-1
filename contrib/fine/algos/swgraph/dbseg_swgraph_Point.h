#ifndef _POINT_H
#define _POINT_H
#pragma warning(disable:4786)

#include <vector>
#include "Matrix.h"  

using namespace std;  

template<class Tp>
class Point{
public:
    Tp x;
    Tp y; 
    Point(){x=0;y=0;};
    Point(Tp x1,Tp y1){x=x1;y=y1;};
    template<class Tp2>
    Point(Point<Tp2> &p){x=p.x;y=p.y;};
    template<class Tp2>
    Point<Tp> & operator=(const Point<Tp2> &p){
        if(this!=(Point<Tp> *)&p){
            x=(Tp)p.x;
            y=(Tp)p.y;
        }
        return *this;
    };
    template<class Tp2>
    Point<Tp> & operator=(const Matrix<Tp2> &M){
        x=(Tp)M.Data(0,0);
        y=(Tp)M.Data(1,0);
        return *this;
    };

    operator Matrix<Tp>(){
        Matrix<Tp> M;
        M.SetDimension(2,1);
        M(0,0)=x;M(1,0)=y;
        return M;
    }

    template<class Tp2>
    inline operator Point<Tp2>(){
        Point<Tp2> P;
        P.x=(Tp2)x;
        P.y=(Tp2)y;
        return P;
    };

    Point operator+(const Point& p){
        Point q;q.x=x+p.x;q.y=y+p.y;
        return q;
    };
    Point operator-(const Point& p){
        Point q;q.x=x-p.x;q.y=y-p.y;
        return q;
    };
    Point operator*(double d){
        Point q;q.x=x*d;q.y=y*d;
        return q;
    };
    Point operator/(double d){
        Point q;q.x=x/d;q.y=y/d;
        return q;
    };
    Point operator+=(const Point& p){
        x+=p.x;y+=p.y;
        return *this;
    };
    Point operator-=(const Point& p){
        x-=p.x;y-=p.y;
        return *this;
    };
    Point operator*=(const Point& p){
        x*=p.x;y*=p.y;
        return *this;
    };

    bool operator==(const Point& p){
        return ((x==p.x)&&(y==p.y));
    }
    bool operator!=(const Point& p){
        return ((x!=p.x)||(y!=p.y));
    }

    bool lessY(const Point& p){ 
        if (y<p.y) return true;
        if (y>p.y) return false;
        return (x<p.x);
    }
    template<class Tp2>
    bool less(const Point<Tp2>& p){ 
        if (x<p.x) return true;
        if (x>p.x) return false;
        return (y<p.y);
    }
    template<class Tp2>
    bool operator<(const Point<Tp2>& p){ 
        return less(p);
    }

    virtual    operator>>(FILE &strm){
        fwrite(&x, sizeof(x), 1, &strm);
        fwrite(&y, sizeof(y), 1, &strm);
    }

    virtual    operator<<(FILE &strm){
        fread(&x, sizeof(x), 1, &strm);
        fread(&y, sizeof(y), 1, &strm);
    }


    // ACCESS
    Tp X() {return x;}
    Tp Y() {return y;}
    Tp GetX() {return x;}
    Tp GetY() {return y;}
      Tp& rX(){return x;}
    Tp& rY(){return y;}
    void SetX(const Tp x1){x=x1;}
    void SetY(const Tp y1){y=y1;}
    void Set(const Tp x1, const Tp y1){x=x1;y=y1;}
    void Get(Tp& x1, Tp& y1){x1=x;y1=y;}
    inline    Tp    Row(void) const {return x;};
    inline    Tp    Col(void) const {return y;};
    inline  void    SetRow(const Tp row1){x=row1;};
    inline    void    SetCol(const Tp col1){y=col1;};
    void    Set(const Point &pt){
        Tp temp1,temp2;
        temp1 = (Tp)ceil(pt.X());
        temp2 = (Tp)floor(pt.X());
        if (fabs(pt.X()-temp1) < fabs(pt.X()-temp2))
            x = temp1;
        else
            x = temp2;
        temp1 = (Tp)ceil(pt.Y());
        temp2 = (Tp)floor(pt.Y());
        if (fabs(pt.Y()-temp1) < fabs(pt.Y()-temp2))
            y = temp1;
        else
            y = temp2;
    }
    // OPERATIONS 
    void Scale(Tp scale){x *= scale;y *= scale;}
    double SqDistance(double x1, double y1){    
        double dx = x-x1;
        double dy = y-y1;
        return dx * dx + dy * dy;
    }
    double Distance(double x1, double y1){    
        double dx = x-x1;
        double dy = y-y1;
        double s = dx * dx + dy * dy;
        return (double)sqrt(s);
    }
    double Distance(const Point& p){    
        return Distance(p.x,p.y);
    }
    double Dist_1(const Point& p){    //norm 1
        double dx = p.x-x;
        double dy = p.y-y;
        return fabs(dx)+fabs(dy);
    }
    double Dist_infty(const Point& p){    //norm infinity
        double dx = fabs(p.x-x);
        double dy = fabs(p.y-y);
        if (dx>dy)
            return dx;
        return dy;
    }
    double Azimuth(double x1, double y1){return (double)atan2(y1-y,x1-x);} 
    double Azimuth(const Point& p){return (double)atan2(p.y-y,p.x-x);} 

    double Norm(){return sqrt(x*x+y*y);}//distance to zero
    double sumsq(){return x*x+y*y;}
    Matrix<Tp> toMatrix(){
        Matrix<Tp> M;
        M.SetDimension(2,1);
        M(0,0)=x;M(1,0)=y;
        return M;
    }
    void print(){
        if(Tp==int)
            printf("%d,%d\n",x,y);
        if(Tp==double)
            printf("%f,%f\n",x,y);
    }
    void sprint(char *buff){
        if(Tp==int)
            sprintf(buff,"%d,%d",x,y);
        if(Tp==double)
            sprintf(buff,"%2.1f,%2.1f",x,y);
    }
    bool    bound(int nx,int ny){
        bool b;
        if (x<0){
            x=0;
            b=false;
        }
        if (x>nx-1){
            x=nx-1;
            b=false;
        }
        if (y<0){
            y=0;
            b=false;
        }
        if (y>ny-1){
            y=ny-1;
            b=false;
        }
        return b;
    }
    void zero(){x=0;y=0;}
};

template<class Tp>
class Point3d:public Point<Tp>{
public:
    Tp z; //=value
    Point3d(){x=0;y=0;z=0;};
    Point3d(Tp x1,Tp y1){x=x1;y=y1;z=0;};
    Point3d(Tp x1,Tp y1,Tp z1){x=x1;y=y1;z=z1;};
    template<class Tp2>
    Point3d& operator=(const Point3d<Tp2> &p){
        if(this!=&p){
            x=(Tp)p.x;
            y=(Tp)p.y;
            z=(Tp)p.z;
        }
        return *this;
    };
    template<class Tp2>
    Point<Tp> & operator=(const Matrix<Tp2> &M){
        if(this!=&M){
            x=(Tp)M(0,0);
            y=(Tp)M(1,0);
            z=(Tp)M(2,0);
        }
        return *this;
    };
/*    template<class Tp2>
    operator Point3d<Tp2>(){
        Point3d<Tp2> P;
        P.x=(Tp2)x;
        P.y=(Tp2)y;
        P.z=(Tp2)z;
        return P;
    }
*/    operator Matrix<Tp>(){
        Matrix M;
        M.SetDimension(3,1);
        M(0,0)=x;M(1,0)=y;M(2,0)=z;
        return M;
    }
    Point3d operator+(const Point3d& p){
        Point3d q;q.x=x+p.x;q.y=y+p.y;q.z=z+p.z;
        return q;
    };
    Point3d operator-(const Point3d& p){
        Point3d q;q.x=x-p.x;q.y=y-p.y;q.z=z-p.z;
        return q;
    };
    Point3d operator*(double d){
        Point3d q;q.x=x*d;q.y=y*d;q.z=z*d;
        return q;
    };
    Point3d operator/(double d){
        Point3d q;q.x=x/d;q.y=y/d;q.z=z/d;
        return q;
    };
    Point operator+=(const Point& p){
        x+=p.x;y+=p.y;z+=p.z;
        return *this;
    };
    Point operator-=(const Point& p){
        x-=p.x;y-=p.y;z-=p.z;
        return *this;
    };
    Point operator*=(const Point& p){
        x*=p.x;y*=p.y;z*=p.z;
        return *this;
    };

    bool operator==(const Point& p){
        return ((x==p.x)&&(y==p.y)&&(z==p.z));
    }

    virtual    operator>>(FILE &strm){
        fwrite(&x, sizeof(x), 1, &strm);
        fwrite(&y, sizeof(y), 1, &strm);
        fwrite(&z, sizeof(z), 1, &strm);
    }

    virtual    operator<<(FILE &strm){
        fread(&x, sizeof(x), 1, &strm);
        fread(&y, sizeof(y), 1, &strm);
        fread(&z, sizeof(z), 1, &strm);
    }


    // ACCESS
    Tp Z() {return z;}
    Tp GetZ() {return z;}
      Tp& rZ(){return z;}
    void SetZ(const Tp z1){z=z1;}
    void Set(const Tp x1, const Tp y1, const Tp z1){x=x1;y=y1;z=z1;}
    void Get(Tp& x1, Tp& y1, Tp& z1){x1=x;y1=y;z1=z;}

    double Norm(){
        return sqrt(x*x+y*y+z*z);
    }
    double Sumsq(){
        return x*x+y*y+z*z;
    }
    double SqDistance(const Point3d& p){    
        return SqDistance(p.x,p.y,p.z);
    }    
    double SqDistance(double x1, double y1, double z1){    
        double dx = x-x1;
        double dy = y-y1;
        double dz = z-z1;
        return dx * dx + dy * dy+dz*dz;
    }
    double Distance(double x1, double y1, double z1){    
        double dx = x-x1;
        double dy = y-y1;
        double dz = z-z1;
        double s = dx * dx + dy * dy+dz*dz;
        return (double)sqrt(s);
    }
    double Distance(const Point3d& p){    
        return Distance(p.x,p.y,p.z);
    }
    double Dot(Point3d &p){
        return (x*p.x+y*p.y+z*p.z);
    }
    Matrix<Tp> toMatrix(){
        Matrix<Tp> M;
        M.SetDimension(3,1);
        M(0,0)=x;M(1,0)=y;M(2,0)=z;
        return M;
    }
    void print(){
        if(Tp==int)
            printf("%d,%d,%d\n",x,y,z);
        if(Tp==double)
            printf("%f,%f,%f\n",x,y,z);
    }
    void sprint(char *buff){
        if(Tp==int)
            sprintf(buff,"%d,%d,%d",x,y,z);
        if(Tp==double)
            sprintf(buff,"%2.1f,%2.1f,%2.1f",x,y,z);
    }
};

typedef Point<double>    Pointd;
typedef Point<int>        Pointi;
typedef Point3d<double>    Point3dd;
typedef Point3d<int>    Point3di;
typedef Point<double>    McPoint2D;
typedef Point3d<double>    McPoint3D;
typedef Point<long>        McImagePoint;
typedef std::vector<Pointi> PointList;
typedef std::vector<Point3di> Point3dList;

template<class Tp>
bool lex_order(const Point<Tp>& p,const Point<Tp>& q){ 
        if (p.x<q.x) return true;
        if (p.x>q.x) return false;
        return (p.y<q.y);
}
template<class Tp>
bool operator<(const Point<Tp>& p,const Point<Tp>& q){ 
//        return (p.x<q.x);
    return lex_order(p,q);
}

void    Transf(Matrix<double> &r0, Matrix<double> &x0, Matrix<double> &r1,Matrix<double> &x1, Matrix<double> &r, Matrix<double> &x);
void    Transf1(Matrix<double> &r0, Matrix<double> &x0, Matrix<double> &r1,Matrix<double> &x1, Matrix<double> &r, Matrix<double> &x);
Point<double>    InvProjTransf(Point<double> &P, Matrix<double> &R, Matrix<double> &T);
Point<double>    ProjTransf(Point<double> &P, Matrix<double> &R, Matrix<double> &T);
Point<double>    InvAffineTransf(Point<double> &P, Matrix<double> &R, Matrix<double> &T);
Point<double>    AffineTransf(Point<double> &P, Matrix<double> &R, Matrix<double> &T);
#endif

