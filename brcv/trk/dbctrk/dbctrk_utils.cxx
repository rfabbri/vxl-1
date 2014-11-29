#include <dbctrk/dbctrk_utils.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <vcl_iostream.h>

#define ARCLENSAMPLE 0.1
#define SMALL_VALUE 1E-5



bool utils::almostEqual(double a, double b, double eps)
{
  return vcl_fabs(a-b)<eps;
}

bool utils::almostEqual(double a, double b)
{
  return vcl_fabs(a-b)<SMALL_VALUE;
}

double utils::fixAngleMPiPi(double a)
{
  a = vcl_fmod(a,2*vnl_math::pi);
  if (a < -vnl_math::pi)
    return a+2*vnl_math::pi;
  else if (a >= vnl_math::pi)
    return a-2*vnl_math::pi;
  else
    return a;
}
double utils::fixAngleMPiPibytwo(double a)
{
  a = vcl_fmod(a,2*vnl_math::pi);
  if (a < 0)
    a=a+vnl_math::pi;

  return a;
}
double utils::fixAngleZTPi(double a)
{
  a = vcl_fmod(a,2*vnl_math::pi);
  if (a < 0)
    return a+2*vnl_math::pi;
  else
    return a;
}

#if 0 // unused function
vcl_vector<double> utils::smoothVector(vcl_vector<double> a, vcl_vector<double> kernel)
{
  int N=kernel.size();
  int M=a.size();
  int mid=(N-1)/2;
  vcl_vector<double> sa(M);

  for (int i=0;i<M;i++)
  {
    sa[i]=0;
    for (int j=-mid;j<=mid;j++)
    {
      double b;
      if (i-j < 0)
      {
        b=angleFixForAdd(a[i],a[0]);
        sa[i]+=(kernel[j+mid]*b);
      }
      else if (i-j >= a.size())
      {
        b=angleFixForAdd(a[i],a[M-1]);
        sa[i]+=(kernel[j+mid]*b);
      }
      else
      {
        b=angleFixForAdd(a[i],a[i-j]);
        sa[i]+=(kernel[j+mid]*b);
      }
    }
  }
  return sa;
}
#endif

double utils::angleFixForAdd(double ref, double a)
{
  double d=a-ref;

  if (d > vnl_math::pi)
    return a-2*vnl_math::pi;
  else if (d < -vnl_math::pi)
    return a+2*vnl_math::pi;
  else
    return a;
}
void utils::set_changing_colors(int id,float &r,float &g, float &b)
{
  int pattern  = id%16;
  switch(pattern)
  {
    //aqua
  case 0 : (r)=0 ;(g)=255;(b)=255;break;
    //mediumorchid BA 55 D3
  case 1 : (r)=186;(g)=85;(b)=211;break;
    //blueviolet
  case 2 : (r)=138 ;(g)=43;(b)=224;break;
    //brown
  case 3 : (r)=165 ;(g)=42;(b)=42;  break;
    //chartreuse
  case 4 : (r)=127 ;(g)=255;(b)=0;  break;
    //CHOCLATE BROWN
  case 5 : (r)=210 ;(g)=105;(b)=30;break;
    //CRIMSON
  case 6 : (r)=220 ;(g)=20;(b)=60;break;
    //darkgoldenrod
  case 7 : (r)=184;(g)=134;(b)=11;break;
    //darkmagenta
  case 8 : (r)=139;(g)=0;(b)=139;break;
    //darkorange
  case 9 : (r)=255;(g)=140;(b)=0;break;
    //darkslategray
  case 10 :(r)=47;(g)=79;(b)=79;break;
    //deeppink
  case 11 : (r)=255;(g)=20;(b)=147;break;
    //gold
  case 12 : (r)=255;(g)=215;(b)=0;break;
    //greenyellow
  case 13 : (r)=173;(g)=255;(b)=47;break;
    //indianred  CD 5C 5C
  case 14 : (r)=192;(g)=92;(b)=92;break;
    //MAROON  80 00 0
  case 15 : (r)=128;(g)=0;(b)=0;break;
  
    default: (r) = 0; (g) = 0; (b) = 0; break; // this will never happen
  }

   (r)/=255;
   (g)/=255;
   (b)/=255;


  return  ;


}
void utils::set_rank_colors(int num,float &r,float &g, float &b)
{
  int pattern  = num;
  switch(pattern)
  {
    // RED
  case 0 : (r)=1.0 ;(g)=0.0;(b)=0.0;break;
    // GREEN 
  case 1 : (r)=0.0 ;(g)=1.0;(b)=0.0;break;
    //YELLOW
  case 2: (r)=1.0 ;(g)=1.0;(b)=0.0;break;

     //brown
  case 3 : (r)=0.67f ;(g)=0.16f;(b)=0.16f;  break;

    // DEEP PINK 
  case 4 : (r)=1.0f;(g)=.08f;(b)=0.6f;break;
    //darkorange
  case 5 : (r)=1.0f;(g)=.55f;(b)=0.0f;break; 
    // sky blue
  case 6 : (r)=0.55f;(g)=.8f;(b)=0.93f;break; 
  default: (r)=0.92f;(g)=.92f;(b)=0.92f;break;
    /*  case 1 : (r)=.74;(g)=0.3;(b)=.81;break;
   //darkgoldenrod
  case 2 : (r)=.73;(g)=.53;(b)=.045;break;
    //blue
  case 3 : (r)=0 ;(g)=0;(b)=1;  break;
  //deeppink
  case 4 : (r)=1.0;(g)=.08;(b)=0.6;break;
  //greenyellow
  case 5 : (r)=.7;(g)=1;(b)=0.2;break;
    //darkmagenta
    case 6 : (r)=0 ;(g)=1;(b)=1;break;*/
    }



  return  ;


}
//: fucntion to parse a line from file for numbers;
vcl_vector<vcl_string> utils::parseLineForNumbers(vcl_string s)
{
  vcl_vector <vcl_string> vec;

  if (s.size() > 0){

    vcl_string numerics("0123456789.-+e");
    vcl_string::size_type pos1=0,pos2=0;
    
    //cout << s << endl;
    while((pos1=s.find_first_of(numerics,pos1)) != vcl_string::npos) {
      pos2=s.find_first_not_of(numerics,pos1);
      vec.push_back(s.substr(pos1,pos2-pos1));
      pos1=pos2;
    }
  }
  return vec;
}

int utils::rgb2lab(double r,double g,double b, double &L , double &a, double &lb)
{
    double var_R = ( r / 255.0 )     ;   //R = From 0 to 255
    double var_G = ( g / 255.0 )     ;   //G = From 0 to 255
    double var_B = ( b / 255.0 )     ;   //B = From 0 to 255


    if ( var_R > 0.04045 ) 
        var_R = vcl_pow(( ( var_R + 0.055 ) / 1.055 ),2.4);
    else                   
        var_R = var_R / 12.92;
    if ( var_G > 0.04045 ) 
        var_G = vcl_pow(( ( var_G + 0.055 ) / 1.055 ),2.4);
    else                   
        var_G = var_G / 12.92;
    if ( var_B > 0.04045 ) 
        var_B = vcl_pow(( ( var_B + 0.055 ) / 1.055 ),2.4);
    else                   
        var_B = var_B / 12.92;

    var_R = var_R * 100;
    var_G = var_G * 100;
    var_B = var_B * 100;

    //Observer. = 2°, Illuminant = D65
    double X = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
    double Y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
    double Z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;

    double var_X = X /  95.047   ;       //Observer = 2°, Illuminant = D65
    double var_Y = Y / 100.000   ;
    double var_Z = Z / 108.883   ;

    if ( var_X > 0.008856 ) 
        var_X = vcl_pow(var_X ,( (double)1/3 ));
    else                    
        var_X = ( 7.787 * var_X ) + ( 16 / 116 );
    if ( var_Y > 0.008856 ) 
        var_Y = vcl_pow(var_Y ,( (double)1/3 ));
    else                    
        var_Y = ( 7.787 * var_Y ) + ( 16 / 116 );
    if ( var_Z > 0.008856 ) 
        var_Z = vcl_pow(var_Z ,( (double)1/3 ));
    else                    
        var_Z = ( 7.787 * var_Z ) + ( 16 / 116 );

    L = ( 116 * var_Y ) - 16;
    a = 500 * ( var_X - var_Y );
    lb = 200 * ( var_Y - var_Z );
    return 1;
}
int utils::IHS2lab(double I,double H,double S, double &L , double &a, double &lb)
{
   
      double hue =H;
      double sat = 2.f*S;
      if (sat<0)
        sat = 0.f;
      if (sat>255)
        sat = 255.f;
      double ang = (vnl_math::pi/180)*hue;
      double cs = vcl_cos(ang), si = vcl_fabs(vcl_sin(ang));
      double r,g,b;
      g = si*sat;
      if (cs>=0)
      {
        r = cs*sat;
        b = 0;
      }
      else
      {
        r = 0;
        b = sat*(-cs);
      }
      
    double var_R = ( r / 255 )     ;   //R = From 0 to 255
    double var_G = ( g / 255 )     ;   //G = From 0 to 255
    double var_B = ( b / 255 )     ;   //B = From 0 to 255

    if ( var_R > 0.04045 ) 
        var_R = vcl_pow(( ( var_R + 0.055 ) / 1.055 ),2.4);
    else                   

        var_R = var_R / 12.92;
    if ( var_G > 0.04045 ) 
        var_G = vcl_pow(( ( var_G + 0.055 ) / 1.055 ),2.4);
    else                   
        var_G = var_G / 12.92;
    if ( var_B > 0.04045 ) 
        var_B = vcl_pow(( ( var_B + 0.055 ) / 1.055 ),2.4);
    else                   
        var_B = var_B / 12.92;

    var_R = var_R * 100;
    var_G = var_G * 100;
    var_B = var_B * 100;

    //Observer. = 2°, Illuminant = D65
    double X = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
    double Y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
    double Z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;

    double var_X = X /  95.047   ;       //Observer = 2°, Illuminant = D65
    double var_Y = Y / 100.000   ;
    double var_Z = Z / 108.883   ;

    if ( var_X > 0.008856 ) 
        var_X = vcl_pow(var_X ,0.3333);
    else                    
        var_X = ( 7.787 * var_X ) + ( 16 / 116 );
    if ( var_Y > 0.008856 ) 
        var_Y = vcl_pow(var_Y ,0.3333);
    else                    
        var_Y = ( 7.787 * var_Y ) + ( 16 / 116 );
    if ( var_Z > 0.008856 ) 
        var_Z = vcl_pow(var_Z ,0.3333);
    else                    
        var_Z = ( 7.787 * var_Z ) + ( 16 / 116 );


    L = ( 116 * var_Y ) - 16;
    a = 500 * ( var_X - var_Y );
    lb = 200 * ( var_Y - var_Z );

    return 1;
} 
 void utils::rgb_to_ihs(double r,double g, double b,
                       double& i, double& h, double& s)
{
  // Reference: figure 13.36, page 595 of Foley & van Dam

  i = r*0.299+0.587*g+0.114*b;


  double maxval = vnl_math::max(r, vnl_math::max(g, b));
  double minval = vnl_math::min(r, vnl_math::min(g, b));

  //lightness
  double la = (maxval + minval) / 2.f;
  // Achromatic case, intensity is grey or near black or white
  if (maxval == minval||i<20||i>235)
  {
    s = -1.f;
    h = 0.f;
  }
  else//the chromatic case
  {
    // Calculate the saturation.
    if (la <= 127)
    {
      s = (255.f*(maxval - minval))/(maxval + minval);
    }
    else
    {
      s = (255.f*(maxval - minval)) / (512.f - maxval - minval);
      if (s<0)
        s = 0.f;
      if (s>255)
        s = 255.f;
    }

    // Calculate the hue.
    double delta = maxval - minval;
    if (r == maxval)
    {
      // The resulting color is between yellow and magenta.
      h = (60*(g - b))/ delta;
    }
    else if (g == maxval)
    {
      // The resulting color is between cyan and yellow.
      h = 120.f + (60*(b - r))/delta;
    }
    else
    {
      // The resulting color is between magenta and cyan.
      h = 240.f + (60*(r - g))/delta;
    }
    // Be sure 0 <= hue <= 360
    if (h < 0)
      h += 360.f;
    if (h > 360)
      h -= 360.f;
  }
}


//: gaussian 2d kernel and assuming an odd dimension
void utils::gauss2d(float sigma,vbl_array_2d<float>  & kernel)
  {
      if(kernel.rows()!=kernel.cols() && sigma <= 0.0)
      {
          return;
      }
      int center=(kernel.rows()-1)/2;
      float sum=0;
      for(int i=0;i<kernel.rows();i++)
          for(int j=0;j<kernel.cols();j++)
          {
              kernel(i,j)=vcl_exp(-((i-center)*(i-center)+(j-center)*(j-center))/2*sigma*sigma)/vcl_sqrt(2*vnl_math::pi*sigma);
              sum+=kernel(i,j);
          }
    
      for(int i=0;i<kernel.rows();i++)
         for(int j=0;j<kernel.cols();j++)
             kernel.put(i,j,kernel(i,j)/sum);
    

  }
double utils::mean(vcl_vector<double> t)
{
  double sum=0;
  for (unsigned int i=0;i<t.size();++i)
    sum+=t[i];
  if(t.size()>0)
    return sum/=t.size();
  else
    return 0.0;
}


double utils::std(vcl_vector<double> t)
{
  double sum=mean(t);
  double var=0;
  for (unsigned int i=0;i<t.size();++i)
    var+=(t[i]-sum)*(t[i]-sum);
  if(t.size()>0)
    return vcl_sqrt(var/t.size());
  else
    return 0.0;
  
}

bool utils::norm3dpolar(vcl_vector<double> & dim1,vcl_vector<double> & dim2,vcl_vector<double> & dim3, vbl_array_3d<double> &polarhist,int thetabins,int rbins,int valuebins,double r1,double v1)
{
    polarhist.fill(0.0);
    if(dim1.size()!=dim2.size() && dim1.size()!=dim3.size() && dim1.size()>0)
    return false;
  
  for(unsigned int i=0;i<dim1.size();i++)
    {
      float h,s,v;
      if(dim1[i]<0.0 || dim1[i]>255.0)
  v=0.0;
      else
  v=(float)dim1[i];
      if(dim2[i]<0.0 || dim2[i]>255.0)
  h=0.0;
      else
  h=(float)dim2[i];
      if(dim3[i]<0.0 || dim3[i]>255.0)
  s=0.0;
      else
  s=(float)dim3[i];


      int thetaindex=(int)vcl_floor((h*360/255)/(360/thetabins));
      int satindex=(int)vcl_floor(vcl_pow((s/255)/r1,2));
      int valindex=(int)vcl_floor(vcl_pow((v/255)/v1,3));
      if(thetaindex>=thetabins)
  thetaindex=thetabins-1;
      if(satindex>=rbins)
  satindex=rbins-1;
      if(valindex>=valuebins)
  valindex=valuebins-1;

      polarhist(thetaindex,satindex,valindex)= polarhist(thetaindex,satindex,valindex)+1;
    }
    for(int i=0;i<thetabins;i++)
    {
      for(int j=0;j<rbins;j++)
  {
    for(int k=0;k<valuebins;k++)
      {
        polarhist(i,j,k)= polarhist(i,j,k)/dim1.size();
      
      }
  } 
    }  

    return true;
}
double utils::dist3pdf_bhat(vbl_array_3d<double> hist1,vbl_array_3d<double> hist2, int xbins,int ybins,int zbins)
{
  double dist=0;

  for(int i=0;i<xbins;i++)
      for(int j=0;j<ybins;j++)
        for(int k=0;k<zbins;k++)
            {
                dist+=vcl_sqrt(hist1(i,j,k)*hist2(i,j,k));
             }
  
  if(dist!=0.0)
    return (-vcl_log(dist));
  else
    return 1e6;
}
