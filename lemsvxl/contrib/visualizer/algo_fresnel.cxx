#include <vcl_iostream.h>

#include <vcl_cmath.h>
#include <vcl_complex.h>

#include <base_points.h>
#include <algo_euler.h>


#define EPS 6.0e-8
#define MAXIT 100
#define FPMIN 1.0e-30
#define XMIN 1.5

/* 
 * EPS   is the relative error; 
 * MAXIT is the maximum number of iterations allowed; 
 * FPMIN is a number near the smallest representable floating-point number;
 * XMIN  is the dividing line between using the series and continued fraction.
 *
 * */

#define TRUE 1

Point2D<double> EulerSpiral::get_fresnel_integral(double x)
  { 
    /*
     * Computes the Fresnel integrals S(x) and C(x) for all real x.
     * 
     * */

    
    int k,n,odd;
    double a,ax,fact,pix2,sign,sum,sumc,sums,term,test;

    vcl_complex<double> b,cc,d,h,del,cs;
    Point2D<double> result;
    
    ax=fabs(x);
    if (ax < sqrt(FPMIN)) 
      { 
        /*Special case: avoid failure of convergence test because of undeflow. */
        result.setY(0.0);
        result.setX(ax);
       }
    else 
      {
        if (ax <= XMIN)
          {
            /*Evaluate both series simultaneously. */
            sum=sums=0.0;
            sumc=ax;
            sign=1.0;
            fact=(M_PI/2.0)*ax*ax;
            odd=TRUE;
            term=ax;
            n=3;
            for (k=1;k<=MAXIT;k++)
              { 
                term *= fact/k;
                sum  += sign*term/n;
                test=fabs(sum)*EPS;
                if (odd) 
                  {
                    sign = -sign;
                    sums=sum;
                    sum=sumc;
                   }
                else
                  {
                    sumc=sum;
                    sum=sums;
                   }

                if (term < test) break;
                odd=!odd;
                n +=2;
               }
            if (k > MAXIT) 
                vcl_cout<<"  series failed in fresnel";
            result.setY(sums); 
            result.setX(sumc);
           }
        else 
          {
            /*Evaluate continued fraction by modified Lentz's method */
            pix2=M_PI*ax*ax;
            b   = vcl_complex<double>(1.0,-pix2);
            cc  = vcl_complex<double>(1.0/FPMIN,0.0);
            d=h = vcl_complex<double>(1.0,0.0)/b;
            n = -1;

            for (k=2;k<=MAXIT;k++) 
              {
                n +=2;
                a = -n*(n+1);
                b= b+vcl_complex<double>(4.0,0.0);
                d=(vcl_complex<double>(1.0,0.0)/((a*d)+b));


                /*Denominators cannot be zero.*/

                cc=(b+(vcl_complex<double>(a,0.0)/cc));

                del=(cc*d);
                h=h*del;
                if ((fabs(del.real()-1.0)+fabs(del.imag())) < EPS)
                    break;
               }
            if (k > MAXIT) 
                vcl_cout<<"cf failed in frenel";
            
            h=vcl_complex<double>(ax,-ax)*h;
            //cs=(vcl_complex<double>(0.5,0.5)*(vcl_complex<double>(1.0,0.0)-(vcl_complex<double>(cos(0.5*pix2),sin(0.5*pix2))*h)));
            
            result.setX(cs.real());
            result.setY(cs.imag());
           }
       }
    if (x < 0.0) 
      { 
        /*Use antisymmetry.*/
        result.setX(result.getX() * -1);
        result.setY(result.getY() * -1);
       }
    return result;
   }


