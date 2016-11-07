#pragma warning(disable:4786)
#include <cmath>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <map>

#include "functions.h"

#include <algorithm>
using namespace std;

#define T_LAST .9

double getlogTemp(double startT, double end_t, long step, double leveloff){//logarithmic temperature
    double b=exp(end_t/startT);
    double x=step/leveloff*(EXP1-b);
    return end_t/log(x+b);
}

double G(double x, double s){
    // g = G(x, s)
    // Gaussian density truncated to (-3, 3) with 0 ending 
    // x: the difference from the mean
    // s: the scale 
    double d,p,x1,g;
    d = 3; p = .9974; 
    x1 = x/s; 
    if (fabs(x1)<d){ 
       g = (exp(-x1*x1/2) - exp(-d*d/2))/sqrt(2*PI)/s; 
       return g/(p-d*2/sqrt(2*PI)*exp(-d*d/2));    
    }
    else return 0; 
}

double nrand()
//standard gaussian distribution 1-d
{
    static int iset=0;
    static double gset;
    double fac,rsq,v1,v2;
   
//    if (*idum < 0) iset=0; 
    if  (iset == 0) {
        do {
            v1=(2.0*rand())/RAND_MAX-1.0;
            v2=(2.0*rand())/RAND_MAX-1.0;
            rsq=v1*v1+v2*v2;
        } while (rsq >= 1.0 || rsq == 0.0);
        fac=sqrt(-2.0*log(rsq)/rsq);
        gset=v1*fac;
        iset=1;
        return v2*fac;
    } else {
        iset=0;
        return gset;
    }
}
int randint(int n){
    //random number btw 0 and n-1
    int k=n*(rand()/(RAND_MAX+1.));
    if (k>n-1)
        k=n-1;
    return k;
}
double randdbl(double d){
    //random number btw 0 and d
    return (rand()*d)/(RAND_MAX+1.);
}
bool SampleProb(double p){
    //returns true with probability p
    if (randdbl(1)<p)
        return true;
    else
        return false;
}
bool inbounds(int rows,int cols,int i,int j){
    if ((i<0 )||(j<0)) return false;
    if ((i>=rows)||(j>=cols)) return false;
    return true;
}
void Get_edge(int pdir,int x, int y, int side, int &x0, int &y0, int &x1, int &y1){
    // edge btw pt (x,y) and its nb. pdir=0 means horz nb, side=1 means before
    //returns x0,y0=beginning of edge, (x1,y1)=end of edge
    x0=x;y0=y;            
    if (side==0){
        if (pdir==0)
            x0++;
        else
            y0++;
    }
    if (pdir==0){
        x1=x0;y1=y0+1;
    }
    else{
        x1=x0+1;y1=y0;
    }
}
void DrawEdges(Matrix<int> &L, Matrix<int> &E,int value){
    int nx=L.nx(),ny=L.ny();
    int x,y,l0,l1,nl=L.GetMax();
    int x0,y0,x1,y1;

    E.SetDimension(ny+1,nx+1);E.Zero();
    for (y=0;y<ny;y++)
        for (x=0;x<nx-1;x++){
            l0=L(y,x)-1;
            l1=L(y,x+1)-1;
            if (l0!=l1){
                Get_edge(0,x,y,0,x0,y0,x1,y1);
                E(y0,x0)=value;
                E(y1,x1)=value;
            }
        }
    for (y=0;y<ny-1;y++)
        for (x=0;x<nx;x++){
            l0=L(y,x)-1;
            l1=L(y+1,x)-1;
            if (l0!=l1){
                Get_edge(1,x,y,0,x0,y0,x1,y1);
                E(y0,x0)=value;
                E(y1,x1)=value;
            }
        }
}

void Merge_Layer_Regions(Matrix<int> &I1, Matrix<int> &I2, Matrix<int> &I){
    int nx=I1.nx(),ny=I1.ny();
    int j,k,x,y;
    Pointi p;
    map<Pointi,int> ma;
    map<Pointi,int>::iterator mi;
    I.SetDimension(ny,nx);
    k=1;ma.clear();
    for (y=0;y<ny;y++)
        for (x=0;x<nx;x++){
            p.x=I1(y,x);p.y=I2(y,x);
            mi=ma.find(p);
            if (mi==ma.end()){
                j=k;
                ma.insert(map<Pointi,int>::value_type(p,k));
                k++;
            }
            else{
                j=(*mi).second;
            }
            I(y,x)=j;
        }
}
void Merge_Layer_Regions1(Matrix<int> &I1, Matrix<int> &I2, Matrix<int> &I){
    //merge regions on places where I1>0
    int nx=I1.nx(),ny=I1.ny();
    int j,k,x,y;
    Pointi p;
    map<Pointi,int> ma;
    map<Pointi,int>::iterator mi;
    I.SetDimension(ny,nx);
    k=1;ma.clear();
    for (y=0;y<ny;y++)
        for (x=0;x<nx;x++){
            p.x=I1(y,x);p.y=I2(y,x);
            if (p.x>0){
                mi=ma.find(p);
                if (mi==ma.end()){
                    j=k;
                    ma.insert(map<Pointi,int>::value_type(p,k));
                    k++;
                }
                else{
                    j=(*mi).second;
                }
            }
            else{
                j=0;
            }
            I(y,x)=j;
        }
}

void print(char *filename, vector<double> &m){
    int r=m.size();
    FILE *f=fopen(filename, "w" );
    for(int i=0;i<r-1;i++)
        fprintf(f,"%3.5f,",m[i]);
    fprintf(f,"%3.5f",m[r-1]);
    fclose( f );
}
void print(char *filename, map<int,int> &m){
    Matrixi M;
    map<int,int>::iterator mi;
    int i=0;
    if (m.size()==0)
        return;
    M.SetDimension(2,m.size());
    for (mi=m.begin();mi!=m.end();++mi){
        M(0,i)=(*mi).first;
        M(1,i)=(*mi).second;
        i++;
    }
    print(filename,M);
}
void print(char *filename, vector<int> &m){
    FILE *f=fopen(filename, "w" );
    int i,n=m.size();
    if (n==0)
        return;
    for (i=0;i<n-1;i++)
        fprintf(f,"%d,",m[i]);
    fprintf(f,"%d",m[n-1]);
    fclose( f );
}

void print(char * filename,Matrix<double> &M){
    int r=M.rows(),c=M.cols();
    FILE *f=fopen(filename, "w" );
    for(int i=0;i<r;i++){
        for(int j=0;j<c-1;j++)
            fprintf(f,"%3.6f,",M.Data(i,j));
        fprintf(f,"%3.6f\n",M.Data(i,c-1));
    }
    fclose( f );
}
void print10(char * filename,Matrix<double> &M){
    int r=M.rows(),c=M.cols();
    FILE *f=fopen(filename, "w" );
    for(int i=0;i<r;i++){
        for(int j=0;j<c-1;j++)
            fprintf(f,"%3.10f,",M.Data(i,j));
        fprintf(f,"%3.10f\n",M.Data(i,c-1));
    }
    fclose( f );
}
void print3(char * filename,Matrix<double> &M){
    int r=M.rows(),c=M.cols();
    FILE *f=fopen(filename, "w" );
    for(int i=0;i<r;i++){
        for(int j=0;j<c-1;j++)
            fprintf(f,"%3.3f,",M.Data(i,j));
        fprintf(f,"%3.3f\n",M.Data(i,c-1));
    }
    fclose( f );
}
void print(char * filename,Matrix<float> &M){
    int r=M.rows(),c=M.cols();
    FILE *f=fopen(filename, "w" );
    for(int i=0;i<r;i++){
        for(int j=0;j<c-1;j++)
            fprintf(f,"%3.3f,",M.Data(i,j));
        fprintf(f,"%3.3f\n",M.Data(i,c-1));
    }
    fclose( f );
}
void print(char * filename,Matrix<int> &M){
    int r=M.rows(),c=M.cols();
    FILE *f=fopen(filename, "w" );
    for(int i=0;i<r;i++){
        for(int j=0;j<c-1;j++)
            fprintf(f,"%d,",M.Data(i,j));
        fprintf(f,"%d\n",M.Data(i,c-1));
    }
    fclose( f );
}
void print(char * filename,Matrix<__int64> &M){
    int r=M.rows(),c=M.cols();
    FILE *f=fopen(filename, "w" );
    for(int i=0;i<r;i++){
        for(int j=0;j<c-1;j++)
            fprintf(f,"%I64d,",M.Data(i,j));
        fprintf(f,"%I64d\n",M.Data(i,c-1));
    }
    fclose( f );
}
void print(char * filename,Matrix<uchar> &M){
    int r=M.rows(),c=M.cols();
    FILE *f=fopen(filename, "w" );
    for(int i=0;i<r;i++){
        for(int j=0;j<c-1;j++)
            fprintf(f,"%d,",M.Data(i,j));
        fprintf(f,"%d\n",M.Data(i,c-1));
    }
    fclose( f );
}
void sprint(char * str,Matrix<double> &M){
    int ny=M.ny() ,nx=M.nx();
    char buff[255];
    strcpy(str,"");
    for(int i=0;i<ny;i++){
        for(int j=0;j<nx-1;j++){
            sprintf(buff,"%3.3f,",M.Data(i,j));
            strcat(str,buff);
        }
        sprintf(buff,"%3.3f\r\n",M.Data(i,nx-1));
        strcat(str,buff);
    }
}
void sprint(char * str,Matrix<float> &M){
    int ny=M.ny() ,nx=M.nx();
    char buff[255];
    strcpy(str,"");
    for(int i=0;i<ny;i++){
        for(int j=0;j<nx-1;j++){
            sprintf(buff,"%3.3f,",M.Data(i,j));
            strcat(str,buff);
        }
        sprintf(buff,"%3.3f\r\n",M.Data(i,nx-1));
        strcat(str,buff);
    }
}

bool dlmread(char *filename, Matrixf &M, char dlm){

    int i,j,max,rows;
    Matrix<float> Mr[10000];
    char line[20000],buff[20000];

    FILE *f = fopen(filename,"r");
    rows=0;
    if (f == NULL){
        return false;
    }

    // read till eof
    do{    
        fgets(buff,20000,f); 
        strcpy(line,buff);
        lineparse(line,20000,dlm,Mr[rows]);
        if (!feof(f))
            rows++;    
    }while (!feof(f));
    fclose(f);
    max=0;
    for (i=0;i<rows;i++){
        if(Mr[i].cols()>max)
            max=Mr[i].cols();
    }

    M.SetDimension(rows,max);M.Zero();
    for (i=0;i<rows;i++){
        for (j=0;j<Mr[i].cols();j++)
            M(i,j)=Mr[i].Data(0,j);
    }
    return true;
}
void lineparse(char *line,int nmax, char dlm, Matrixf &M){
    char num[255];
    float fl[100000];
    unsigned int i,j,cols;


    i = 0;
    j = 0;
    cols=0;
    while ((i < nmax)&& (line[i] != '\n')&& (line[i] != '\0')){
        // read chars from line, parsing delimiters & numbers
        strcpy(num,"");
        j = 0;

        while ((i <nmax) && (line[i] != dlm) &&(line[i] != '\0') && (line[i] != '\n')){
            // build number string from characters on the line
            num[j] = line[i];
            i ++;    // overall line index
            j ++;    // number string index
        }
        num[j]='\0';
        if(strcmp(num,"")!=0){
            fl[cols]=(float)atof(num);
            cols++;    //find how many columns for current line            
        }
        i++;
    }
    if (cols>0){
        M.SetDimension(1,cols);M.Zero();
        for (j=0;j<cols;j++)
            M(0,j)=fl[j];
    }
}
void lineparse(char *line, int nmax, char dlm, Matrixd &M){
    char num[255];
    vector<double> fl;
    unsigned int i,j,cols;


    i = 0;
    j = 0;
    cols=0;
    while ((i < nmax)&& (line[i] != '\n')&& (line[i] != '\0')){
        // read chars from line, parsing delimiters & numbers
        strcpy(num,"");
        j = 0;

        while ((i <nmax) && (line[i] != dlm) &&(line[i] != '\0') && (line[i] != '\n')){
            // build number string from characters on the line
            num[j] = line[i];
            i ++;    // overall line index
            j ++;    // number string index
        }
        num[j]='\0';
        if(strcmp(num,"")!=0){
            fl.push_back (atof(num));
            cols++;    //find how many columns for current line            
        }
        i++;
    }
    if (cols>0){
        M.SetDimension(1,cols);M.Zero();
        for (j=0;j<cols;j++)
            M(0,j)=fl[j];
    }
}
bool dlmread(char *filename, Matrix<double> &M, char dlm){

    int i,j,max,rows;
    vector<Matrix<double> > Mr;
    char *line,*buff;
    Matrixd Ml;

    FILE *f = fopen(filename,"r");
    rows=0;
    if (f == NULL){
        return false;
    }

    line=new char[20000];
    buff=new char[20000];
    // read till eof
    do{    
        fgets(buff,20000,f); 
        strcpy(line,buff);line[19999]=0;
        lineparse(line,20000,dlm,Ml);
        Mr.push_back(Ml);
        if (!feof(f))
            rows++;    
    }while (!feof(f));
    fclose(f);
    max=0;
    for (i=0;i<rows;i++){
        if(Mr[i].cols()>max)
            max=Mr[i].cols();
    }

    M.SetDimension(rows,max);M.Zero();
    for (i=0;i<rows;i++){
        for (j=0;j<Mr[i].cols();j++)
            M(i,j)=Mr[i].Data(0,j);
    }
    delete line;
    delete buff;
    return true;
}
bool dlmread(char *filename, Matrix<int> &M, char dlm){

    int i,j,max,rows;
    vector<Matrix<int> > Mr;
//    Matrix<int> Mr[10000];
    char line[10000],buff[10000];
    Matrixi Ml;

    FILE *f = fopen(filename,"r");
    rows=0;
    if (f == NULL){
        return false;
    }

    // read till eof
    do{    
        fgets(buff,10000,f); 
        strcpy(line,buff);
        lineparse(line,10000,dlm,Ml);
        Mr.push_back(Ml);
        if (!feof(f))
            rows++;    
    }while (!feof(f));
    fclose(f);
    max=0;
    for (i=0;i<rows;i++){
        if(Mr[i].cols()>max)
            max=Mr[i].cols();
    }

    M.SetDimension(rows,max);M.Zero();
    for (i=0;i<rows;i++){
        for (j=0;j<Mr[i].cols();j++)
            M(i,j)=Mr[i].Data(0,j);
    }
    return true;
}
void lineparse(char *line,int nmax, char dlm, Matrix<int> &M){
    char num[255];
    int fl[100000];
    unsigned int i,j,cols;


    i = 0;
    j = 0;
    cols=0;
    while ((i < nmax)&& (line[i] != '\n')&& (line[i] != '\0')){
        // read chars from line, parsing delimiters & numbers
        strcpy(num,"");
        j = 0;

        while ((i <nmax) && (line[i] != dlm) &&(line[i] != '\0') && (line[i] != '\n')){
            // build number string from characters on the line
            num[j] = line[i];
            i ++;    // overall line index
            j ++;    // number string index
        }
        num[j]='\0';
        if(strcmp(num,"")!=0){
            fl[cols]=atoi(num);
            cols++;    //find how many columns for current line            
        }
        i++;
    }
    if (cols>0){
        M.SetDimension(1,cols);M.Zero();
        for (j=0;j<cols;j++)
            M(0,j)=fl[j];
    }
}

bool dlmread(char *filename, Matrix<__int64> &M, char dlm){

    int i,j,max,rows;
    Matrix<__int64> Mr[5000];
    char line[10000],buff[10000];

    FILE *f = fopen(filename,"r");
    rows=0;
    if (f == NULL){
        return false;
    }

    // read till eof
    do{    
        fgets(buff,10000,f); 
        strcpy(line,buff);
        lineparse(line,10000,dlm,Mr[rows]);
        if (!feof(f))
            rows++;    
    }while (!feof(f));
    fclose(f);
    max=0;
    for (i=0;i<rows;i++){
        if(Mr[i].cols()>max)
            max=Mr[i].cols();
    }

    M.SetDimension(rows,max);M.Zero();
    for (i=0;i<rows;i++){
        for (j=0;j<Mr[i].cols();j++)
            M(i,j)=Mr[i].Data(0,j);
    }
    return true;
}

void lineparse(char *line,int nmax, char dlm, Matrix<__int64> &M){
    char num[255];
    __int64 fl[100000];
    unsigned int i,j,cols;


    i = 0;
    j = 0;
    cols=0;
    while ((i < nmax)&& (line[i] != '\n')&& (line[i] != '\0')){
        // read chars from line, parsing delimiters & numbers
        strcpy(num,"");
        j = 0;

        while ((i <nmax) && (line[i] != dlm) &&(line[i] != '\0') && (line[i] != '\n')){
            // build number string from characters on the line
            num[j] = line[i];
            i ++;    // overall line index
            j ++;    // number string index
        }
        num[j]='\0';
        if(strcmp(num,"")!=0){
            fl[cols]=_atoi64(num);
            cols++;    //find how many columns for current line            
        }
        i++;
    }
    if (cols>0){
        M.SetDimension(1,cols);M.Zero();
        for (j=0;j<cols;j++)
            M(0,j)=fl[j];
    }
}
void lineparse(char *line, char dlm, vector<int> &v){
    char num[255];
    unsigned int i,j,cols,fl;

    i = 0;
    j = 0;
    cols=0;v.clear();
    while ((i <= strlen(line))&& (line[i] != '\n')){
        // read chars from line, parsing delimiters & numbers
        strcpy(num,"");
        j = 0;

        while ((i <= strlen(line)) && (line[i] != dlm) && (line[i] != '\n')){
            // build number string from characters on the line
            num[j] = line[i];
            i ++;    // overall line index
            j ++;    // number string index
        }
        num[j]='\0';
        if(strcmp(num,"")!=0){
            fl=atoi(num);
            v.push_back(fl);
            cols++;    //find how many columns for current line            
        }
        i++;
    }
}

void    log2file(char * filename,Matrixf &M){
    long i,j;
    if (M.size()<=0)
        return;
    FILE *f=fopen(filename,"a+");
    if (f!=NULL){
        for (i=0;i<M.rows();i++){
            for (j=0;j<M.cols()-1;j++){
                fprintf(f,"%1.3f,",M(i,j));        
            }
            fprintf(f,"%1.3f\r\n",M(i,M.cols()-1));        
        }
        fclose(f);
    }
}
void    log2file(char * filename,Simple_Matrix<double> &M){
    long i,j;
    if (M.size()<=0)
        return;
    FILE *f=fopen(filename,"a+");
    if (f!=NULL){
        for (i=0;i<M.rows();i++){
            for (j=0;j<M.cols()-1;j++){
                fprintf(f,"%1.5f,",M(i,j));        
            }
            fprintf(f,"%1.5f\r\n",M(i,M.cols()-1));        
        }
        fclose(f);
    }
}
void    log2file(char * filename,Matrixi &M){
    long i,j;
    if (M.size()<=0)
        return;
    FILE *f=fopen(filename,"a+");
    if (f!=NULL){
        for (i=0;i<M.rows();i++){
            for (j=0;j<M.cols()-1;j++){
                fprintf(f,"%d,",M(i,j));        
            }
            fprintf(f,"%d\r\n",M(i,M.cols()-1));        
        }
        fclose(f);
    }
}
void    log2file(char * filename,Matrix<__int64> &M){
    long i,j;
    if (M.size()<=0)
        return;
    FILE *f=fopen(filename,"a+");
    if (f!=NULL){
        for (i=0;i<M.rows();i++){
            for (j=0;j<M.cols()-1;j++){
                fprintf(f,"%I64d,",M(i,j));        
            }
            fprintf(f,"%I64d\r\n",M(i,M.cols()-1));        
        }
        fclose(f);
    }
}
void log2file(char *filename, vector<double> &m){
    FILE *f=fopen(filename, "a+" );
    if (f!=NULL){
        int r=m.size();
        for(int i=0;i<r-1;i++)
            fprintf(f,"%3.3f,",m[i]);
        fprintf(f,"%3.3f\r\n",m[r-1]);
        fclose( f );
    }
}

void    clearfile(char * filename){
    FILE *f=fopen(filename, "w" );
    if (f!=NULL)
        fclose( f );
}
void log2file(char * filename,char *st){
    FILE *f=fopen(filename,"a+");
    if (f!=NULL){
        fprintf(f,"%s",st);
        fclose(f);
    }
}
void log2filei(char * filename,int d){
    FILE *f=fopen(filename,"a+");
    if (f!=NULL){
        fprintf(f,"%d\r\n",d);
        fclose(f);
    }
}
void log2filei(char * filename,int d,int d1){
    FILE *f=fopen(filename,"a+");
    if (f!=NULL){
        fprintf(f,"%d,%d\r\n",d,d1);
        fclose(f);
    }
}
void log2file(char * filename,double d){
    FILE *f=fopen(filename,"a+");
    if (f!=NULL){
        fprintf(f,"%1.4f\r\n",d);
        fclose(f);
    }
}
void log2file(char * filename,double d1,double d2){
    FILE *f=fopen(filename,"a+");
    if (f!=NULL){
        fprintf(f,"%1.4f,%1.4f\r\n",d1,d2);
        fclose(f);
    }
}
void log2file(char * filename,double d1,double d2, double d3){
    FILE *f=fopen(filename,"a+");
    if (f!=NULL){
        fprintf(f,"%1.4f,%1.4f,%1.4f\r\n",d1,d2,d3);
        fclose(f);
    }
}
void log2file(char * filename,double d1,double d2, double d3, double d4){
    FILE *f=fopen(filename,"a+");
    if (f!=NULL){
        fprintf(f,"%1.4f,%1.4f,%1.4f,%1.4f\r\n",d1,d2,d3,d4);
        fclose(f);
    }
}
void log2file(char * filename,set<int> &s){
    FILE *f=fopen(filename,"a+");
    if (f!=NULL){
        set<int>::iterator si,si1,se=s.end();
        si1=s.begin();++si1;
        for (si=s.begin();si!=se;++si){
            if (si1!=se){
                fprintf(f,"%d,",*si);
                ++si1;
            }
            else
                fprintf(f,"%d\r\n",*si);

        }
        fclose(f);
    }
} 
void log2file(char * filename,vector<Point3di> &p){
    int i,n=p.size();
    FILE *f=fopen(filename,"a+");
    if (f!=NULL){
        for (i=0;i<n;i++){
            fprintf(f,"%d,%d,%d",p[i].x,p[i].y,p[i].z);
            if (i<n-1)
                fprintf(f,",");
            else
                fprintf(f,"\r\n");
        }
        fclose(f);
    }
}

Matrix<double> Sqrt(Matrix<double> &M){
    //the square root of a symetric positive definite matrix M

    int i;
    Matrix<double> E,D,S;
    Eig(M, D, E);
    S.SetDimension(D.rows(),D.rows());
    S.Zero();
    for (i=0;i<D.rows();i++)
        if (D.Data(i,0)>0)
            S(i,i)=sqrt(D.Data(i,0));
        else
            S(i,i)=0.1;
    return E*S*E.T();
}


#define RTOL 1.0e-18

/* Solve Ax=B with partial pivoting */

int ppgauss(Matrix<double>& A, Matrix<double>& x, Matrix<double>& B){
    double factor,pivot,temp,sum;
    int pivpos,i,j,k,n=A.rows();
    Matrix<double> a1,b1;

    //copy working matrix and rhs
    a1=A;
    b1=B;

    for(j=0; j<n-1; j++){

        //search for biggest pivot on column j
        pivot=fabs(a1(j,j));
        pivpos=j;
        for(i=j+1; i<n; i++){
            if (fabs(a1(i,j))>pivot){
                pivpos=i;pivot=fabs(a1(i,j));
            }
        }
        if (pivot<RTOL){
            printf("\n Singular matrix! \n ");
//            fprintf(fp, "\n Singular matrix! \n ");
            return 1;
        }

        //found pivot, switch lines  +rhs
        for(k=j; k<n; k++){
            temp=a1(j,k);a1(j,k)=a1(pivpos,k);a1(pivpos,k)=temp;
        }
        temp=b1(j);b1(j)=b1(pivpos);b1(pivpos)=temp;
        
        //make zeros on j column
        for(i=j+1; i<n; i++)
        {
             factor = a1(i,j)/a1(j,j);
             for(k=j; k<n; k++){
                a1(i,k) =a1(i,k)- factor*a1(j,k);
                
             }
             b1(i) = b1(i)-factor*b1(j);
        }
    }

    //done triangularization, backsolve
    if (fabs(a1(n-1,n-1))<RTOL){
        printf("\n Singular matrix! \n ");
        return -1;
    }
    x.SetDimension(n,1);
    x(n-1) = b1(n-1)/a1(n-1,n-1);
    for(j=n-2;j>=0;j--){
        sum=0.;
        for(i=j+1; i<n; i++)
             sum = sum + a1(j,i)*x(i);
        x(j) = (b1(j)-sum)/a1(j,j);
    }
    return 0;
}
int    ppgauss(Matrix<long double>& A, Matrix<long double>& x, Matrix<long double>& B){
    long double factor,pivot,temp,sum;
    int pivpos,i,j,k,n=A.rows();
    Matrix<long double> a1,b1;

    //copy working matrix and rhs
    a1=A;
    b1=B;

    for(j=0; j<n-1; j++){

        //search for biggest pivot on column j
        pivot=fabs(a1(j,j));
        pivpos=j;
        for(i=j+1; i<n; i++){
            if (fabs(a1(i,j))>pivot){
                pivpos=i;pivot=fabs(a1(i,j));
            }
        }
        if (pivot<RTOL){
            printf("\n Singular matrix! \n ");
//            fprintf(fp, "\n Singular matrix! \n ");
            return 1;
        }

        //found pivot, switch lines  +rhs
        for(k=j; k<n; k++){
            temp=a1(j,k);a1(j,k)=a1(pivpos,k);a1(pivpos,k)=temp;
        }
        temp=b1(j);b1(j)=b1(pivpos);b1(pivpos)=temp;
        
        //make zeros on j column
        for(i=j+1; i<n; i++)
        {
             factor = a1(i,j)/a1(j,j);
             for(k=j; k<n; k++){
                a1(i,k) =a1(i,k)- factor*a1(j,k);
                
             }
             b1(i) = b1(i)-factor*b1(j);
        }
    }

    //done triangularization, backsolve
    if (fabs(a1(n-1,n-1))<RTOL){
        printf("\n Singular matrix! \n ");
        return -1;
    }
    x.SetDimension(n,1);
    x(n-1) = b1(n-1)/a1(n-1,n-1);
    for(j=n-2;j>=0;j--){
        sum=0.;
        for(i=j+1; i<n; i++)
             sum = sum + a1(j,i)*x(i);
        x(j) = (b1(j)-sum)/a1(j,j);
    }
    return 0;
}

#define SWAP(a,b) {double temp=(a);(a)=(b);(b)=temp;}
int SolveSystem(Matrix<double>& A, Matrix<double>& x, Matrix<double>& B){
    int *indxc,*indxr,*ipiv;
    int i,icol,irow,j,k,l,ll,*ivector();
    int n=A.rows(),m=B.cols();
    double big,dum,pivinv;
    Matrix<double> a;

    indxc=new int[n];
    indxr=new int [n];
    ipiv=new int[n];
    x=B;a=A;
    for (j=0;j<n;j++) ipiv[j]=0;
    for (i=0;i<n;i++) {
        big=0.0;
        for (j=0;j<n;j++)
            if (ipiv[j] != 1)
                for (k=0;k<n;k++) {
                    if (ipiv[k] == 0) {
                        if (fabs(a(j,k)) >= big) {
                            big=fabs(a(j,k));
                            irow=j;
                            icol=k;
                        }
                    } else if (ipiv[k] > 1) return -1;
                }
        ++(ipiv[icol]);
        if (irow != icol) {
            for (l=0;l<n;l++) SWAP(a(irow,l),a(icol,l))
            for (l=0;l<m;l++) SWAP(x(irow,l),x(icol,l))
        }
        indxr[i]=irow;
        indxc[i]=icol;
        if (a(icol,icol)+1. == 1.0) return -2;
        pivinv=1.0/a(icol,icol);
        a(icol,icol)=1.0;
        for (l=0;l<n;l++) a(icol,l) *= pivinv;
        for (l=0;l<m;l++) x(icol,l) *= pivinv;
        for (ll=0;ll<n;ll++)
            if (ll != icol) {
                dum=a(ll,icol);
                a(ll,icol)=0.0;
                for (l=0;l<n;l++) a(ll,l) -= a(icol,l)*dum;
                for (l=0;l<m;l++) x(ll,l) -= x(icol,l)*dum;
            }
    }
    for (l=n-1;l>=0;l--) {
        if (indxr[l] != indxc[l])
            for (k=0;k<n;k++)
                SWAP(a(k,indxr[l]),a(k,indxc[l]));
    }
    delete []indxc;
    delete []indxr;
    delete []ipiv;
    return 0;
}
#undef SWAP


#define ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);\
    a[k][l]=h+s*(g-h*tau);

double *vecto(long nl, long nh)
/* allocate a double vector with subscript range v[nl..nh] */
{
    double *v;

    v=(double *)malloc((size_t) ((nh-nl+2)*sizeof(double)));
    return v-nl+1;
}
void free_vector(double *v, long nl, long nh)
/* free a double vector allocated with vector() */
{
    free((char *) (v+nl-1));
}

int jacobi(double **a, int n, double *d, double **v, int *nrot)
{
    int j,iq,ip,i;
    double tresh,theta,tau,t,sm,s,h,g,c,*b,*z;

    b = vecto(1, n);
    if (b == (double *)NULL) {
        fprintf(stderr, "jacobi():  vector() failed!\n");
        return(-1);
    }

    z = vecto(1, n);
    if (z == (double *)NULL) {
        fprintf(stderr, "jacobi():  vector() failed!\n");
        return(-1);
    }

    /* Initialize to the identity matrix.*/
    for (ip=1;ip<=n;ip++) {
        for (iq=1;iq<=n;iq++) v[ip][iq]=0.0;
        v[ip][ip]=1.0;
    }
    /* Initialize b and d to the diagonal of a.*/
    for (ip=1;ip<=n;ip++) {
        b[ip]=d[ip]=a[ip][ip];
        z[ip]=0.0;
    }
    *nrot=0;
    for (i=1;i<=50;i++) {
        sm=0.0;
        /* Sum off-diagonal elements.*/
        for (ip=1;ip<=n-1;ip++) {
            for (iq=ip+1;iq<=n;iq++)
                sm += fabs(a[ip][iq]);
        }
        /* The normal return, which relies on quadratic */
        /* convergence to machine underflow.*/
        if (sm == 0.0) {
            free_vector(b, 1, n);
            free_vector(z, 1, n);
            return(0);
        }
        if (i < 4)
            tresh=0.2*sm/(n*n);
        else
            tresh=0.0;
        for (ip=1;ip<=n-1;ip++) {
            for (iq=ip+1;iq<=n;iq++) {
                g=100.0*fabs(a[ip][iq]);
                /* After four sweeps, skip the rotation */
                /* if the off-diagonal element is small.*/
                if (i > 4 && (fabs(d[ip])+g) == fabs(d[ip])
                    && (fabs(d[iq])+g) == fabs(d[iq]))
                    a[ip][iq]=0.0;
                else if (fabs(a[ip][iq]) > tresh) {
                    h=d[iq]-d[ip];
                    if ((fabs(h)+g) == fabs(h))
                        t=(a[ip][iq])/h;
                    else {
                        theta=0.5*h/(a[ip][iq]);
                        t=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
                        if (theta < 0.0) t = -t;
                    }
                    c=1.0/sqrt(1+t*t);
                    s=t*c;
                    tau=s/(1.0+c);
                    h=t*a[ip][iq];
                    z[ip] -= h;
                    z[iq] += h;
                    d[ip] -= h;
                    d[iq] += h;
                    a[ip][iq]=0.0;
                    /* Case of rotations 1 <= j < p.*/
                    for (j=1;j<=ip-1;j++) {
                        ROTATE(a,j,ip,j,iq)
                    }
                    /* Case of rotations p < j < q.*/
                    for (j=ip+1;j<=iq-1;j++) {
                        ROTATE(a,ip,j,j,iq)
                    }
                    /* Case of rotations q < j <= n.*/
                    for (j=iq+1;j<=n;j++) {
                        ROTATE(a,ip,j,iq,j)
                    }
                    for (j=1;j<=n;j++) {
                        ROTATE(v,j,ip,j,iq)
                    }
                    ++(*nrot);
                }
            }
        }
        for (ip=1;ip<=n;ip++) {
            b[ip] += z[ip];
            d[ip]=b[ip];
            z[ip]=0.0;
        }
    }

    free_vector(b,1,n);
    free_vector(z,1,n);
    /*nrerror("Too many iterations in routine jacobi");*/
    fprintf(stderr, "jacobi():  Too many iterations!\n");
    return(-1);
}
static double at,bt,ct;
#define PYTHAG(a,b) ((at=fabs(a)) > (bt=fabs(b)) ? \
(ct=bt/at,at*sqrt(1.0+ct*ct)) : (bt ? (ct=at/bt,bt*sqrt(1.0+ct*ct)): 0.0))

static double maxarg1,maxarg2;
#ifdef MAX
#undef MAX
#endif
#define MAX(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ?\
    (maxarg1) : (maxarg2))
#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

#define TOL 1.0e-5
void    SolveMeanSquares(Matrixd &A, Matrixd &x, Matrixd &B){
    //solve Ax=B when A is overdetermined
    int j,i,ndata=A.rows(),ma=A.cols();
    double wmax,thresh,*b;
    double **u,**v,*w,*a;

    u = new pdouble[ndata+1];
    v = new pdouble[ndata+1];
    w = new double[ndata+1];
    a = new double[ndata+1];
    for (j=0; j<ndata+1; j++){
        u[j] = new double[ma+1];
        v[j] = new double[ma+1];
    }

    b=vecto(1,ndata);
    for (i=1;i<=ndata;i++) {
        for (j=1;j<=ma;j++){
            u[i][j]=A(i-1,j-1);
        }
        b[i]=B(i-1);
    }
    svdcmp(u,ndata,ma,w,v);
    wmax=0.0;
    for (j=1;j<=ma;j++)
        if (w[j] > wmax) wmax=w[j];
    thresh=TOL*wmax;
    for (j=1;j<=ma;j++)
        if (w[j] < thresh) w[j]=0.0;
    svbksb(u,w,v,ndata,ma,b,a);
    for (i=1;i<=ndata;i++) {
        x(i-1)=a[i];
    }
    free_vector(b,1,ndata);
    for (j=0; j<ndata; j++)
    {
        delete []u[j];
        delete []v[j];
    }
    delete []u;
    delete []v;
    delete []w;
    delete []a;
}

void svbksb(double **u,double *w,double **v,int m,int n,double *b,double *x){
    int jj,j,i;
    double s,*tmp;

    tmp=vecto(1,n);
    for (j=1;j<=n;j++) {
        s=0.0;
        if (w[j]) {
            for (i=1;i<=m;i++) s += u[i][j]*b[i];
            s /= w[j];
        }
        tmp[j]=s;
    }
    for (j=1;j<=n;j++) {
        s=0.0;
        for (jj=1;jj<=n;jj++) s += v[j][jj]*tmp[jj];
        x[j]=s;
    }
    free_vector(tmp,1,n);
}


void svdcmp(double **a,int m,int n,double *w,double **v)
{
    int flag,i,its,j,jj,k,l,nm;
    double c,f,h,s,x,y,z;
    double anorm=0.0,g=0.0,scale=0.0;
    double *rv1;

    if (m < n) 
        return;//nrerror("SVDCMP: You must augment A with extra zero rows");
    rv1=vecto(1,n);
    for (i=1;i<=n;i++) {
        l=i+1;
        rv1[i]=scale*g;
        g=s=scale=0.0;
        if (i <= m) {
            for (k=i;k<=m;k++) scale += fabs(a[k][i]);
            if (scale) {
                for (k=i;k<=m;k++) {
                    a[k][i] /= scale;
                    s += a[k][i]*a[k][i];
                }
                f=a[i][i];
                g = -SIGN(sqrt(s),f);
                h=f*g-s;
                a[i][i]=f-g;
                if (i != n) {
                    for (j=l;j<=n;j++) {
                        for (s=0.0,k=i;k<=m;k++) s += a[k][i]*a[k][j];
                        f=s/h;
                        for (k=i;k<=m;k++) a[k][j] += f*a[k][i];
                    }
                }
                for (k=i;k<=m;k++) a[k][i] *= scale;
            }
        }
        w[i]=scale*g;
        g=s=scale=0.0;
        if (i <= m && i != n) {
            for (k=l;k<=n;k++) scale += fabs(a[i][k]);
            if (scale) {
                for (k=l;k<=n;k++) {
                    a[i][k] /= scale;
                    s += a[i][k]*a[i][k];
                }
                f=a[i][l];
                g = -SIGN(sqrt(s),f);
                h=f*g-s;
                a[i][l]=f-g;
                for (k=l;k<=n;k++) rv1[k]=a[i][k]/h;
                if (i != m) {
                    for (j=l;j<=m;j++) {
                        for (s=0.0,k=l;k<=n;k++) s += a[j][k]*a[i][k];
                        for (k=l;k<=n;k++) a[j][k] += s*rv1[k];
                    }
                }
                for (k=l;k<=n;k++) a[i][k] *= scale;
            }
        }
        anorm=MAX(anorm,(fabs(w[i])+fabs(rv1[i])));
    }
    for (i=n;i>=1;i--) {
        if (i < n) {
            if (g) {
                for (j=l;j<=n;j++)
                    v[j][i]=(a[i][j]/a[i][l])/g;
                for (j=l;j<=n;j++) {
                    for (s=0.0,k=l;k<=n;k++) s += a[i][k]*v[k][j];
                    for (k=l;k<=n;k++) v[k][j] += s*v[k][i];
                }
            }
            for (j=l;j<=n;j++) v[i][j]=v[j][i]=0.0;
        }
        v[i][i]=1.0;
        g=rv1[i];
        l=i;
    }
    for (i=n;i>=1;i--) {
        l=i+1;
        g=w[i];
        if (i < n)
            for (j=l;j<=n;j++) a[i][j]=0.0;
        if (g) {
            g=1.0/g;
            if (i != n) {
                for (j=l;j<=n;j++) {
                    for (s=0.0,k=l;k<=m;k++) s += a[k][i]*a[k][j];
                    f=(s/a[i][i])*g;
                    for (k=i;k<=m;k++) a[k][j] += f*a[k][i];
                }
            }
            for (j=i;j<=m;j++) a[j][i] *= g;
        } else {
            for (j=i;j<=m;j++) a[j][i]=0.0;
        }
        ++a[i][i];
    }
    for (k=n;k>=1;k--) {
        for (its=1;its<=30;its++) {
            flag=1;
            for (l=k;l>=1;l--) {
                nm=l-1;
                if ((fabs(rv1[l])+anorm) == anorm) {
                    flag=0;
                    break;
                }
                if ((fabs(w[nm])+anorm) == anorm) break;
            }
            if (flag) {
                c=0.0;
                s=1.0;
                for (i=l;i<=k;i++) {
                    f=s*rv1[i];
                    rv1[i]=c*rv1[i];
                    if ((fabs(f)+anorm) == anorm) break;
                    g=w[i];
                    h=PYTHAG(f,g);
                    w[i]=h;
                    h=1.0/h;
                    c=g*h;
                    s=(-f*h);
                    for (j=1;j<=m;j++) {
                        y=a[j][nm];
                        z=a[j][i];
                        a[j][nm]=y*c+z*s;
                        a[j][i]=z*c-y*s;
                    }
                }
            }
            z=w[k];
            if (l == k) {
                if (z < 0.0) {
                    w[k] = -z;
                    for (j=1;j<=n;j++) v[j][k]=(-v[j][k]);
                }
                break;
            }
            if (its == 30) 
                return;//nrerror("No convergence in 30 SVDCMP iterations");
            x=w[l];
            nm=k-1;
            y=w[nm];
            g=rv1[nm];
            h=rv1[k];
            f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
            g=PYTHAG(f,1.0);
            f=((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
            c=s=1.0;
            for (j=l;j<=nm;j++) {
                i=j+1;
                g=rv1[i];
                y=w[i];
                h=s*g;
                g=c*g;
                z=PYTHAG(f,h);
                rv1[j]=z;
                c=f/z;
                s=h/z;
                f=x*c+g*s;
                g=g*c-x*s;
                h=y*s;
                y=y*c;
                for (jj=1;jj<=n;jj++) {
                    x=v[jj][j];
                    z=v[jj][i];
                    v[jj][j]=x*c+z*s;
                    v[jj][i]=z*c-x*s;
                }
                z=PYTHAG(f,h);
                w[j]=z;
                if (z) {
                    z=1.0/z;
                    c=f*z;
                    s=h*z;
                }
                f=(c*g)+(s*y);
                x=(c*y)-(s*g);
                for (jj=1;jj<=m;jj++) {
                    y=a[jj][j];
                    z=a[jj][i];
                    a[jj][j]=y*c+z*s;
                    a[jj][i]=z*c-y*s;
                }
            }
            rv1[l]=0.0;
            rv1[k]=f;
            w[k]=x;
        }
    }
    free_vector(rv1,1,n);
}



#undef SIGN
#undef MAX
#undef PYTHAG



