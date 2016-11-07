#include <testlib/testlib_test.h>
#include <dt/partial_derivatives.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>



MAIN( test_partial_derivatives )
{
    START ("Partial Derivatives ");

    //: z=xy^2;
    double *x =new double[100];
    double *y =new double[100];
    double *fx =new double[100];
    double *fy =new double[100];
    double *z =new double[100];
    int cnt=0;
    for(double i=0;i<10;i++)
    {
        for(double j=0;j<10;j++)
        {
            x[cnt]=(double)i;
            y[cnt]=(double)j;
            z[cnt]=(double)(i*j*j);
            cnt++;
        }
    }
    compute_partial_derivatives(100,x,y,z,fx,fy);
    SUMMARY();
}
