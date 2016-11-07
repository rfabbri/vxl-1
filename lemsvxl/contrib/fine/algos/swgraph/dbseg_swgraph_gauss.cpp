#include "gauss.h"
#include "functions.h"
#include "imgfunctions.h"


double    Gauss_model::logprob_bin(){
    double n=Get_n();
    if (n<=1) return 0;
    double var=Get_var();
    if (var<5*binsize) 
        var=5*binsize;
    return -(n-1.)/2.+n*log(pow(binsize,2)/(2*PI*var))/2.;
//    return -(sum1-1.)*var/var1/2.+sum1*log(pow(binsize,2)/(2*PI*var1))/2.;
}
double    Gauss_model::logprob_bin(double x){
    double m=Get_mean();
    double var=Get_var();
    if (var<5*binsize) 
        var=5*binsize;
    return -(x-m)*(x-m)/2./var+log(pow(binsize,2)/(2*PI*var))/2.;
}
double    Gauss_model::prob_bin(double x){
    double m=Get_mean();
    double var=Get_var();
    if (var<5*binsize) 
        var=5*binsize;
    return exp(-(x-m)*(x-m)/2./var)*binsize/sqrt(2.*PI*var);
}
double    Gauss_model::logprob_norm(){
    double n=Get_n();
    if (n<=1) return 0;
    double var=Get_var();
    return -(n-1.)/2.-n*log(2*PI*var)/2.;
}
double    Gauss_model::logprob_norm(double x){
    double m=Get_mean();
    double var=Get_var();
    return -(x-m)*(x-m)/2./var-log(2*PI*var)/2.;
}
double    Gauss_model::prob_norm(double x){
    double m=Get_mean();
    double var=Get_var();
    return exp(-(x-m)*(x-m)/2./var)/sqrt(2.*PI*var);
}
double    Gauss_model::prob_unnorm(double x){
    double m=Get_mean();
    double var=Get_var();
    if (var<5*binsize) 
        var=5*binsize;
    return exp(-(x-m)*(x-m)/2./var);
}
/*
void Gauss_model::Draw(double min, double max, int n, CDC* dc, int dx, int dy){
    int i,nx=(n-1)*5,ny=50;
    double sum,h=(max-min)/(n-1.);
    dc->FillSolidRect(dx,dy,nx,ny,RGB(255,255,255));
    dc->Rectangle(dx,dy,dx+nx+1,dy+ny+1);
    sum=0;
    for (i=0;i<n;i++){
        sum+=prob(min+i*h);
    }
    dc->MoveTo(dx,dy+ny*(1.-prob(min)/sum));
    for (i=1;i<n;i++){
        dc->LineTo(dx+i*5,dy+ny*(1.-prob(min+i*h)/sum));
    }
}
*/

Matrix<double> Gauss_pdf::sample(){
    Matrix<double> w;
    int i,j,r,c;
    r=mu.rows();c=mu.cols();
    w.SetDimension(r,c);
    for (i=0;i<r;i++)
        for (j=0;j<c;j++)
            w(i,j)=nrand();
    return mu+sigma*w;
}
double Gauss_pdf::prob(Matrix<double> &x){
    Matrix<double> w,a;
    w=sigma.Inv()*(x-mu);
    a=w.T()*w;
    return exp(-a(0,0)/2)/pow(2*PI, sigma.rows()/2.)/sigma.Det();
}
void Gauss_pdf::prints(char *text){
    char buff[255];
    strcat(text,"mu=");
    sprint(buff,mu);
    strcat(text,buff);
    strcat(text,"sigma=");
    sprint(buff,sigma);
    strcat(text,buff);
}

double Gauss_pdf::pmf(double precision){
    //returns the pmf as if gaussian was discrete with accuracy given by precision
    int cx,cy,imux,imuy,relx,rely;
    double rmux,rmuy,pmux,pmuy;
    Matrixf g;

    pmux=mu.Data(0,0)/precision;
    pmuy=mu.Data(1,0)/precision;
    imux=(int)(pmux);
    imuy=(int)(pmuy);
    rmux=pmux-imux;
    rmuy=pmuy-imuy;
    g=gausspmf(rmux,rmuy,sigma/precision);
    cx=(g.nx()-1)/2;
    cy=(g.ny()-1)/2;
    relx=cx-imux;
    rely=cy-imuy;
    if (!inbounds(g.ny(),g.nx(),rely,relx)) 
        return 0;
    else
        return g.Data(rely,relx);
}

void mixgauss::create(int ng){
    if (g!=NULL)     delete[]g;
    if (coeff!=NULL) delete[]coeff;
    n=ng;
    g=new Gauss_pdf[ng];
    coeff=new double[ng];
}
Matrix<double> mixgauss::sample(){
    Matrix<double> w;
    double sum1,d;
    int i,j,k,r,c;
    d=(rand()*1.)/0x8000;            //select which gaussian will choose from
    sum1=0;
    k=n-1;
    for (i=0;i<n;i++){
        sum1+=coeff[i];
        if (d<=sum1){
            k=i;
            break;
        }
    }
    r=g[k].mu.rows();c=g[k].mu.cols();
    w.SetDimension(r,c);
    for (i=0;i<r;i++)
        for (j=0;j<c;j++)
            w(i,j)=nrand();
    return g[k].mu+g[k].sigma*w;
}
double mixgauss::prob(Matrix<double> &x){
    double sum=0;
    Matrix<double> w,a;
    int k;
    for (k=0;k<n;k++){
        w=g[k].sigma.Inv()*(x-g[k].mu);
        a=w.T()*w;
        sum+=coeff[k]*exp(-a(0,0)/2)/pow(2*PI, g[k].sigma.rows()/2.)/g[k].sigma.Det();
    }
    return sum;
}

void Multiply(Matrixd &mu1,Matrixd &var1inv, Matrixd &mu2, Matrixd &var2inv, Matrixd &mu, Matrixd &varinv){
//Gauss_pdf Multiply(Gauss_pdf &g1, double coeff1, Gauss_pdf &g2, double coeff2){
    // the product of two gaussians is a gaussian, find mu, var
    Matrix<double> var;
//    C=(var1*coeff2+var2*coeff1).Inv();var=var1*C*var2;
//    mu=C*(var2*mu1*coeff1+var1*mu2*coeff2);
    varinv=var1inv+var2inv;
    var=varinv.Inv();
    mu=var*(var1inv*mu1+var2inv*mu2);
}
void Multiply(Matrixd &mu1,Matrixd &var1inv,double coeff1, Matrixd &mu2, Matrixd &var2inv, Matrixd &mu, Matrixd &varinv){
    // the product of two gaussians is a gaussian, find mu, var ->
    Matrix<double> var;
    varinv=var1inv*coeff1+var2inv*(1.-coeff1);
    var=varinv.Inv();
    mu=var*(var1inv*mu1*coeff1+var2inv*mu2*(1.-coeff1));
}
void getImageParams(int mbx, int mby, int xc, int yc, double Temp, Matrixu &M, Matrixu &I1, Gauss_pdf &s){
    //finds the log prob of moving an image M arround in I1 in a window mbx x mby centered at (xc,yc)
    int x1,y1,nx=M.nx(),ny=M.ny(),wsx=(nx-1)/2,wsy=(ny-1)/2;
    int mb2x=2*mbx+1,mb2y=2*mby+1;
    double d, sum,sumx,sumy,mux,muy,sumxx,sumxy,sumyy,max;
    Matrix<double > ip,a;

    ip.SetDimension(mb2y,mb2x);
    max=-100000000;
    for(y1=-mby;y1<=mby;y1++)
        for(x1=-mbx;x1<=mbx;x1++){
d=0;//-SSEImage(M,I1,0,0,nx,ny,xc+x1-wsx,yc+y1-wsy);
            ip(y1+mby,x1+mbx)=d;
            if (max<d)
                max=d;
        }
//    print("ip.txt",ip);
    sumx=0;sumy=0;sum=0;        //find center of image gaussian
    for(y1=-mby;y1<=mby;y1++)
        for(x1=-mbx;x1<=mbx;x1++){
            ip(y1+mby,x1+mbx)=exp((ip(y1+mby,x1+mbx)-max)/Temp);
            sumx+=ip(y1+mby,x1+mbx)*x1;
            sumy+=ip(y1+mby,x1+mbx)*y1;
            sum +=ip(y1+mby,x1+mbx);
        }
    s.mu.SetDimension(2,1);
    mux=sumx/sum;muy=sumy/sum;
    s.mu(0,0)=mux+xc;
    s.mu(1,0)=muy+yc;
    sumxx=0;sumxy=0;sumyy=0;
    for(y1=-mby;y1<=mby;y1++)
        for(x1=-mbx;x1<=mbx;x1++){
            sumxx+=ip(y1+mby,x1+mbx)*(x1-mux)*(x1-mux);
            sumxy+=ip(y1+mby,x1+mbx)*(x1-mux)*(y1-muy);
            sumyy+=ip(y1+mby,x1+mbx)*(y1-muy)*(y1-muy);
        }
    a.SetDimension(2,2);a.Identity();
    a(0,0)=sumxx/sum+0.01;a(0,1)=a(1,0)=sumxy/sum;a(1,1)=sumyy/sum+0.01;
    s.sigma=Sqrt(a);
//    if (a(1,0)>0.01){
//        print("ip.txt",ip);
//        print("ip1.txt",s.sigmai);
//    }
}


Matrix<float> gaussker(double xd, double yd,double sigmax, double sigmay){
    // sigmax, sigmay: scale
    // xd,yd move the representation of the token a little, for subpixel accuracy
    long i,j,sizex, sizey;
    double cx,cy,r,s,x,y,d;
    Matrix<float> B;

    sizex = (int) (sigmax*6);     // the size of the base matrix 
    sizey = (int) (sigmay*6);     // the size of the base matrix 
    
    if ((sizex%2)==0)         // the size of B should be odd 
        sizex++;
    if ((sizey%2)==0)         // the size of B should be odd 
        sizey++;
    cx = (sizex-1)/2+xd;        // the half size
    cy = (sizey-1)/2+yd;

    B.SetDimension(sizey,sizex);// the base matrix    
    B.Zero();

    r = .9; s = 1;          // the sizes of center and surround 
    for (i = 0;i<sizey;i++){
        for (j = 0;j<sizex;j++){
            x = (j-cx)/sigmax;
            y = (i-cy)/sigmay;    // scale x and y
            B(i,j) = (float)(G(x, s)*G(y, s));
                            // difference between center and surround
        }
    }
    d=B.Data((sizey-1)/2,(sizex-1)/2);
    B = B/(float)(d);    // normalized to norm 1
    return B;
}
Matrix<float> gaussker(double xd, double yd,double sigmax, double sigmay, double angle){
    // sigmax, sigmay: scale
    // xd,yd move the representation of the token a little, for subpixel accuracy
    long i,j,sizex, sizey;
    double cx,cy,r,s,x,y,d,x0,y0;
    double costeta,sinteta;
    Matrix<float> B;

    sizex = (int) (sigmax*6);     // the size of the base matrix 
    sizey = (int) (sigmay*6);     // the size of the base matrix 
    if (sizex>sizey)
        sizey=sizex;
    else
        sizex=sizey;
    if ((sizex%2)==0)         // the size of B should be odd 
        sizex++;
    if ((sizey%2)==0)         // the size of B should be odd 
        sizey++;
    cx = (sizex-1)/2+xd;        // the half size
    cy = (sizey-1)/2+yd;
    

    B.SetDimension(sizey,sizex);// the base matrix    
    B.Zero();

    costeta=cos((PI * angle)/180.);
    sinteta=sin((PI * angle)/180.);

    r = .9; s = 1;          // the sizes of center and surround 
    for (i = 0;i<sizey;i++){
        for (j = 0;j<sizex;j++){
            x0=j-cx;y0=i-cy;
            x = (x0*costeta-y0*sinteta)/sigmax;
            y = (x0*sinteta+y0*costeta)/sigmay;    // scale x and y
            B(i,j) = (float)(G(x, s)*G(y, s));
        }
    }
    d=B.Data((sizey-1)/2,(sizex-1)/2);
    B = B/(float)(d);    // normalized to norm 1
    return B;
}

Matrix<float> gausspmf(double xd, double yd, Matrix<double> sigma){
    long i,j,sizex, sizey;
    double cx,cy,r,s,x,y,d,x0,y0;
    double a,b,c,a1,b1,c1;
    Matrix<float> B;
    Matrix<double> S,S1;
    
    S1=sigma.Inv();
    S=(sigma.T()*sigma).Inv();
    a=S(0,0);b=(S(0,1)+S(1,0))/2.;c=S(1,1);
    a1=S1(0,0);b1=(S1(0,1)+S1(1,0))/2.;c1=S1(1,1);
    sizex = (int) (6*sqrt(c/(3*b*b+a*c)));     // the size of the base matrix 
    sizey = (int) (6*sqrt(a/(3*b*b+a*c)));     // the size of the base matrix 
    if ((sizex%2)==0)         // the size of B should be odd 
        sizex++;
    if ((sizey%2)==0)         // the size of B should be odd 
        sizey++;
    cx = (sizex-1)/2+xd;        // center coords
    cy = (sizey-1)/2+yd;
    

    B.SetDimension(sizey,sizex);// the base matrix    
    B.Zero();

    r = .9; s = 1;          // the sizes of center and surround 
    for (i = 0;i<sizey;i++){
        for (j = 0;j<sizex;j++){
            x0=j-cx;y0=i-cy;
            x = a1*x0+b1*y0;
            y = b1*x0+c1*y0;    // scale x and y
            B(i,j) = (float)(G(x, s)*G(y, s));
        }
    }
    d=B.Sum();
    B = B/(float)(d);    // normalized to norm 1
    return B;
}
double GaussDotProd (double mu1, double var1, double mu2, double var2){
    double mu,j,sumsq;
    sumsq=var1+var2;
    mu=(mu1*var2+mu2*var1)/sumsq;
    j=(mu1*mu1*var2+mu2*mu2*var1)/sumsq;
    return exp((mu*mu-j)*sumsq/2./var1/var2)/sqrt(2.*PI*sumsq);
}

double KL(double mu1, double var1, double mu2, double var2){
    return 0.5*((mu1-mu2)*(mu1-mu2)/var2+(var1/var2-1.)-log(var1/var2));
}
double SymKL(double mu1, double var1, double mu2, double var2){
    return (mu1-mu2)*(mu1-mu2)*(1./var1+1./var2)/2+(var1-var2)*(var1-var2)/var1/var2/2;
}

double KL(Matrixd &mu1, Matrixd &var1, Matrixd &mu2, Matrixd &var2){
    //KL of multivariate gaussians
    Matrixd var2inv=var2.Inv();
    Matrixd mu=mu1-mu2,tt=var1*var2inv;
    int d=var2inv.cols();
    Matrixd rez=mu.T()*var2inv*mu;
    return rez(0)+tt.Trace()-d-log(fabs(tt.Det()));
}
double KL1(Matrixd &mu1, Matrixd &var1, Matrixd &mu2, Matrixd &var2inv){
    //KL of multivariate gaussians
//    Matrixd var2i=var2.Inv();
    Matrixd mu=mu1-mu2,tt=var1*var2inv;
    int d=var2inv.cols();
    Matrixd rez=mu.T()*var2inv*mu;
    return rez(0)+tt.Trace()-d-log(fabs(tt.Det()));
}
double KL(Matrixd &prob, double x0, double y0, Matrixd &mu, Matrixd &varinv){
    // KL between prob as a 2d histogram centered at (x0,y0) and the gauss (mu,var)
    int x,y,nx=prob.nx(),ny=prob.ny();
    double p,sum=0;
    double detvarinv=varinv(0)*varinv(3)-varinv(1)*varinv(2);
    Matrixd X(2,1),m;
    for (y=0;y<ny;y++){
        X(1)=y+y0-mu(1);
        for (x=0;x<nx;x++){
            X(0)=x+x0-mu(0);
            p=prob(y,x);
            if (p>0){
                m=X.T()*varinv*X;
                sum+=p*(log(p*2.*PI/detvarinv)+0.5*m(0));
//                lnq=-0.5*m(0)-log(2.*PI/detvarinv);
//                sum+=p*(log(p)-lnq);
            }
        }
    }
    return sum;
}
double KL(Matrixd &prob, double mu, double var){
    // KL between prob as a 1d histogram and the gauss (mu,var)
    int x,nx=prob.size();
    double p,sum=0;
    double u,m,s=sqrt(2.*PI*var);
    for (x=0;x<nx;x++){
        u=x-mu;
        p=prob(x);
        if (p>0){
            m=u*u/2./var;
            sum+=p*(log(p*s)+m);
        }
    }
    return sum;
}

double SymKL(Matrixd &mu1, Matrixd &var1, Matrixd &mu2, Matrixd &var2){
    //SymKL of multivariate gaussians
    Matrixd var1i=var1.Inv();
    Matrixd var2i=var2.Inv(),mu=mu1-mu2,tt=var1*var2i+var2*var1i;
    int d=var2.cols();
    Matrixd rez=mu.T()*(var2i+var1i)*mu;
    return rez(0)+tt.Trace()-2*d;
}
Matrixd Adjust_var(Matrixd &var, double min){
    Matrixd U,lambda,V,t;
    int ndim=var.cols();
    SVD(var,U,lambda,V);
    for (int i=0;i<ndim;i++){
        if (lambda(i,i)<min)
            lambda(i,i)=min;
    }
    return U*lambda*V.T();
}
Matrixd Adjust_var(Matrixd &var, double *min){
    Matrixd U,lambda,V,t;
    int ndim=var.cols();
    SVD(var,U,lambda,V);
    for (int i=0;i<ndim;i++){
        if (lambda(i,i)<min[i])
            lambda(i,i)=min[i];
    }
    return U*lambda*V.T();
}

void Project_Affine(Matrixd &mu6, Matrixd &var6inv, Matrixd &x, Matrixd &mu, Matrixd &var){
    Matrixd M(6,4),N(6,2),MT,NT;
    Matrixd S,Si,B(4,1),Ai,vari;
    
    M.Zero();M(0,0)=M(1,1)=M(3,2)=M(4,3)=1;M(2,0)=M(5,2)=-x(0);M(2,1)=M(5,3)=-x(1);
    N.Zero();N(2,0)=N(5,1)=1;MT=M.T();NT=N.T();
    Si=MT*var6inv*M;S=Si.Inv();
    Ai=var6inv-var6inv*M*S*MT*var6inv;
    vari=NT*Ai*N;var=vari.Inv();
    mu=x*(-1.)+var*NT*Ai*mu6;
/*    print ("m.txt",M);
    print ("n.txt",N);
    print ("si.txt",Si);
    print ("ai.txt",Ai);
    print ("vari.txt",vari);
    print ("var.txt",var);
    print ("mu.txt",mu);
*/
}
void Project_Affine1(Matrixd &mu6, Matrixd &var6inv, Matrixd &x, Matrixd &mu, Matrixd &vari){
    Matrixd M(6,4),N(6,2),MT,NT;
    Matrixd S,Si,B(4,1),Ai,var;
    
    M.Zero();M(0,0)=M(1,1)=M(3,2)=M(4,3)=1;M(2,0)=M(5,2)=-x(0);M(2,1)=M(5,3)=-x(1);
    N.Zero();N(2,0)=N(5,1)=1;MT=M.T();NT=N.T();
    Si=MT*var6inv*M;S=Si.Inv();
    Ai=var6inv-var6inv*M*S*MT*var6inv;
    vari=NT*Ai*N;var=vari.Inv();
    mu=x*(-1.)+var*NT*Ai*mu6;
}




