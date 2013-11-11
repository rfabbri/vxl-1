#include "point.h"



Point<double> InvProjTransf(Point<double> &P, Matrix<double> &R, Matrix<double> &T){
    // given an observed pt that has been obtained by transf R,T
    // gives orig. object point (x,y,0);
    Matrix<double> L,B,K,P1,P2;
    Point<double> Q;
    int i,j;

    L.SetDimension(2,2);
    B.SetDimension(2,1);
    K.SetDimension(1,2);
    P1.SetDimension(2,1);
    B(0,0)=T.Data(0,0);
    B(1,0)=T.Data(1,0);
    for (j=0;j<2;j++){
        for (i=0;i<2;i++){
            L(i,j)=R.Data(i,j);
        }
        K(0,j)=R.Data(2,j);
    }
    P1(0,0)=P.x;
    P1(1,0)=P.y;
    L=L-P1*K;
    P2=L.Inv()*(P1*T.Data(2,0)-B);
    Q.x=P2.Data(0,0);
    Q.y=P2.Data(1,0);
    return Q;
}
Point<double> ProjTransf(Point<double> &P, Matrix<double> &R, Matrix<double> &T){
    //transform a point (x,y,0) by R and T and then project it.
    double den;
    Matrix<double> L,K,M,P1;
    Point<double> Q;
    int i,j;

    L.SetDimension(2,3);
    K.SetDimension(1,3);
    P1.SetDimension(3,1);
    for (j=0;j<3;j++){
        for (i=0;i<2;i++){
            L(i,j)=R.Data(i,j);
        }
        K(0,j)=R.Data(2,j);
    }
    P1(0,0)=P.x;
    P1(1,0)=P.y;
    P1(2,0)=0;
    den=(K*P1).Data(0,0)+T.Data(2,0);
    M=L*P1;
    Q.x=(M(0,0)+T.Data(0,0))/den;
    Q.y=(M(1,0)+T.Data(1,0))/den;
    return Q;
}
Point<double> InvAffineTransf(Point<double> &P, Matrix<double> &R, Matrix<double> &T){
    Matrix<double> R2,P1,X;
    Point<double> Q;
    int i,j;

    P1.SetDimension(3,1);
    P1(0,0)=P.x-T.Data(0,0);
    P1(1,0)=P.y-T.Data(1,0);
    R2.SetDimension(2,2);
    for (i=0;i<2;i++){
        for(j=0;j<2;j++)
            R2(i,j)=R.Data(i,j);
    }
    X=R2.Inv()*P1;
    Q.x=X(0,0);
    Q.y=X(1,0);
    return Q;
}
Point<double> AffineTransf(Point<double> &P, Matrix<double> &R, Matrix<double> &T){
    Matrix<double> M,P1;
    Point<double> Q;
    P1.SetDimension(3,1);
    P1(0,0)=P.x;
    P1(1,0)=P.y;
    P1(2,0)=0;
    M=R*P1;
    Q.x=M(0,0)+T.Data(0,0);
    Q.y=M(1,0)+T.Data(1,0);
    return Q;
}
void Transf(Matrix<double> &r0, Matrix<double> &x0, Matrix<double> &r1,Matrix<double> &x1, Matrix<double> &r, Matrix<double> &x){
    Matrix<double> R2,R3;
    int i,j;
    R2.SetDimension(2,2);
    for (i=0;i<2;i++){
        for(j=0;j<2;j++)
            R2(i,j)=r0.Data(i,j);
    }
    R2=R2.Inv();
    R3.SetDimension(3,3);R3.Zero();
    for (i=0;i<2;i++){
        for(j=0;j<2;j++)
            R3(i,j)=R2.Data(i,j);
    }
    r=r1*R3;
    x=x1-r*x0;
}
void Transf1(Matrix<double> &r0, Matrix<double> &x0, Matrix<double> &r1,Matrix<double> &x1, Matrix<double> &r, Matrix<double> &x){
    r=r1*r0.Inv();
    x=x1-r*x0;
}





