#pragma once
#include "bbgm_image_of.h"
#include <vnl/vnl_math.h>
template <class image_,class T>
class bbgm_interp_ftr_base
{
public:
	virtual bool operator()(float x,float y,image_& data, T& return_data)const
	{
		return false;
	}
};

template <class image_,class T>
class bbgm_interp_nearest_ftr:public bbgm_interp_ftr_base<image_,T>
{
	public:
	
	bool operator()(float x,float y,image_& data, T& return_data) const 
	{  
		
		unsigned int p1x=int(x);
		float normx = x-p1x;
		unsigned int p1y=int(y);
		float normy = y-p1y;

		if ((p1x<1)||(p1y<1)||(p1x>data.ni()-2)||(p1y>data.nj()-2)) 
	 {
		// vcl_cerr<<"location "<<x<<" , "<<y<<" is not inside the interpolable area!"<<vcl_endl;
		 return false;
	 }
	
		T i1 = data(p1x,p1y);
		return_data= i1;
		return true;
	}


};

template <class image_,class T>
class bbgm_interp_bicubic_ftr:public bbgm_interp_ftr_base<image_,T>
{
	public:
	
	bool operator()(float x,float y,image_& data, T& return_data) const 
	{

	 
    unsigned int p1x=int(x);
    float normx = x-p1x;
    unsigned int p1y=int(y);
    float normy = y-p1y;

	 if ((p1x<1)||(p1y<1)||(p1x>data.ni()-3)||(p1y>data.nj()-3)) 
	 {
		 //vcl_cerr<<"location "<<x<<" , "<<y<<" is not inside the interpolable area!"<<vcl_endl;
		 return false;
	 }
	if (normx == 0.0 && normy == 0.0)  
	{
	 	return_data=data(p1x,p1y);
		return true;
	}
   
	 // coefficients for interpolation
    float s0=-1.0, s1=-1.0, s2=-1.0, s3=-1.0;      // in the x-direction
    float t0=-1.0, t1=-1.0, t2=-1.0, t3=-1.0;      // in the y-direction

    if (normx != 0.0) {
        s0 = ((2-normx)*normx-1)*normx;    // -1
        s1 = (3*normx-5)*normx*normx+2;    //  0
        s2 = ((4-3*normx)*normx+1)*normx;  // +1
        s3 = (normx-1)*normx*normx;        // +2
    }

    if (normy != 0.0) {
        t0 = ((2-normy)*normy-1)*normy;    // -1
        t1 = (3*normy-5)*normy*normy+2;    //  0
        t2 = ((4-3*normy)*normy+1)*normy;  // +1
        t3 = (normy-1)*normy*normy;        // +2
    }

#define bbgm_I(dx,dy) (data(p1x+(dx),p1y+(dy)))

	if (normy == 0.0) {
        return_data = (bbgm_I(-1,+0)*s0+bbgm_I(+0,+0)*s1+bbgm_I(+1,+0)*s2+bbgm_I(+2,+0)*s3)*0.5f;
        return true;
    }

    if (normx == 0.0) {
       return_data = (bbgm_I(+0,-1)*t0 + bbgm_I(+0,+0)*t1 + bbgm_I(+0,+1)*t2 +bbgm_I(+0,+2)*t3)*0.5f;
       return true;
    }

    T xi0 = bbgm_I(-1,-1)*s0 + bbgm_I(+0,-1)*s1 + bbgm_I(+1,-1)*s2 + bbgm_I(+2,-1)*s3;
    T xi1 = bbgm_I(-1,+0)*s0 + bbgm_I(+0,+0)*s1 + bbgm_I(+1,+0)*s2 + bbgm_I(+2,+0)*s3;
    T xi2 = bbgm_I(-1,+1)*s0 + bbgm_I(+0,+1)*s1 + bbgm_I(+1,+1)*s2 + bbgm_I(+2,+1)*s3;
    T xi3 = bbgm_I(-1,+2)*s0 + bbgm_I(+0,+2)*s1 + bbgm_I(+1,+2)*s2 + bbgm_I(+2,+2)*s3;
	
#undef bbgm_I

    return_data = ( xi0*t0 + xi1*t1 + xi2*t2 + xi3*t3 )*0.25f;
	return true;
	
	}
};

template <class image_,class T>
class bbgm_interp_bilinear_ftr:public bbgm_interp_ftr_base<image_,T>
{
	public:
	
	bool operator()(float x,float y,image_& data, T& return_data) const 
	{  
		
		unsigned int p1x=int(x);
		float normx = x-p1x;
		unsigned int p1y=int(y);
		float normy = y-p1y;

		if ((p1x<1)||(p1y<1)||(p1x>data.ni()-2)||(p1y>data.nj()-2)) 
	 {
		// vcl_cerr<<"location "<<x<<" , "<<y<<" is not inside the interpolable area!"<<vcl_endl;
		 return false;
	 }
	
		T i1 = data(p1x,p1y)+(data(p1x,p1y+1)-data(p1x,p1y))*normy;
		T i2 = data(p1x+1,p1y)+(data(p1x+1,p1y+1)-data(p1x+1,p1y))*normy;
		return_data= i1+(i2-i1)*normx;
		return true;
	}


};

template <class image_,class T>
class bbgm_interp_lanczos_ftr:public bbgm_interp_ftr_base<image_,T>
{
	public:
	
	bool operator()(float x,float y,image_& data, T& return_data) const 
	{  
		
		unsigned int p1x=int(x);
		float normx = x-p1x;
		unsigned int p1y=int(y);
		float normy = y-p1y;

		if((p1x<1)||(p1y<1)||(p1x>data.ni()-3)||(p1y>data.nj()-3)) 
	 {
		// vcl_cerr<<"location "<<x<<" , "<<y<<" is not inside the interpolable area!"<<vcl_endl;
		 return false;
	 }
		 
		  float s0=lncz3(normx+1), s1=lncz3(normx), s2=lncz3(normx-1), s3=lncz3(normx-2) ;
		  float t0=lncz3(normy+1), t1=lncz3(normy), t2=lncz3(normy-1), t3=lncz3(normy-2);
		  #define bbgm_I(dx,dy) (data(p1x+(dx),p1y+(dy)))
		   
		   T xi0 = bbgm_I(-1,-1)*s0 + bbgm_I(+0,-1)*s1 + bbgm_I(+1,-1)*s2 + bbgm_I(+2,-1)*s3;
		   T xi1 = bbgm_I(-1,+0)*s0 + bbgm_I(+0,+0)*s1 + bbgm_I(+1,+0)*s2 + bbgm_I(+2,+0)*s3;
		   T xi2 = bbgm_I(-1,+1)*s0 + bbgm_I(+0,+1)*s1 + bbgm_I(+1,+1)*s2 + bbgm_I(+2,+1)*s3;
           T xi3 = bbgm_I(-1,+2)*s0 + bbgm_I(+0,+2)*s1 + bbgm_I(+1,+2)*s2 + bbgm_I(+2,+2)*s3;
		   return_data = ( xi0*t0 + xi1*t1 + xi2*t2 + xi3*t3 );
		  #undef bbgm_I
		  return true;
	
	}


};

static inline float lncz3(float x)
{
	 #define eps 2.2204e-016f
	 #define Pi	(float)vnl_math::pi
	 float f= float(sin(x*Pi)*sin(x*Pi/3)+eps)/(Pi*Pi*x*x/3+eps);
	 #undef eps
	 #undef Pi
	 return abs(f)< 3? f :0;

}
