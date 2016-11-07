// This is contrib/biotree/biocts/biolung_3d_Tree.cxx

//:
// \file

#include <vcl_iostream.h>
#include <vcl_cmath.h>

#include "biolung_3D_Tree.h"

/*Airway::Airway()
{
}

Airway::~Airway()
{
}*/


// Stack functions
biolung_AwStack::biolung_AwStack(int maxStack) //Constructor
{
  awst=new biolung_Airway[maxStack];
    if (awst==NULL)
    {
    stackSize = 0;
    message = ERROR;
    }
    else
  {
       stackSize = maxStack;
        message = SUCCESS;
    }
    StackP=-1;

}

biolung_AwStack:: ~biolung_AwStack() 
{ 
  delete awst;
}

void biolung_AwStack::Push(int lab,int check,int gen, double ratio,double flow,double diam, biolung_POINT3D p1,biolung_POINT3D p2,
           biolung_Vector vec, biolung_Vector norm, biolung_Area ar, biolung_Range ra, biolung_Parent par)
{
    if (StackP == stackSize)
        message = ERROR;
      else
        {
          StackP++;
            awst[StackP].FillAw(lab,check,gen,ratio,flow,diam, p1,p2, vec, norm, ar,ra, par);
            message=SUCCESS;
        }
        return;
}

void biolung_AwStack::Push(biolung_Airway A)
{
    if (StackP==stackSize)
            message = ERROR;
        else
        {
            StackP++;
                awst[StackP].FillAw(A);
                message = SUCCESS;
        }
        return;
}

biolung_Airway biolung_AwStack::Pop(void)
{
    biolung_Airway a;
        if (StackP < 0)
        {
            message = ERROR;
                a = awst[0];
        }
        else
        {
            message = SUCCESS;
                a = awst[StackP];
                StackP--;
        }
        return a;
}


biolung_Tree::biolung_Tree()   // constructor
{
  dexp=2.8;
  min=0.05;
  m_ldr=3.3;
  l_len=3;
  r_len=6;  
  excl=3;
  acura=12;
  thresh=0.001; //should be <0.01 && #of branches expected = 3/thresh
  
  overflow=0;ovnum=0;
  overflow1=0;ovnum1=0;

  for(long m=0;m<MAX;m++)     //initializing branch matrix
  {
    bra[m].p1.x=0;  bra[m].p1.y=0;  bra[m].p1.z=0;
    bra[m].p2.x=0;  bra[m].p2.y=0;  bra[m].p2.z=0;  
    bra[m].diam=0;  bra[m].check=0;  bra[m].lab=0;
  }

  //: rahul
  aw = new biolung_Airway;
}

biolung_Tree::~biolung_Tree()
{
  delete aw;
}



int biolung_Tree::shape(biolung_POINT3D p)    // Shape of the whole organ
{
  
  if((p.z*15*15*15+2*(p.x*p.x+2.25*p.y*p.y)*(p.x*p.x+2.25*p.y*p.y)<0)&&(p.x*p.x>0.01*15*15)&&(p.z>-2.3*15)
                &&((p.x*p.x>=0.111*15*15)||((p.x*p.x<0.111*15*15)&&(-p.y>15*0.6*vcl_cos(2*PI*p.x*0.06667)-15*0.7)))
                &&((p.z+1.7*15)*15+0.4*(p.x+0.12*15)*(p.x+0.12*15)+0.4*1.69*(p.y+0.15*15)*(p.y+0.15*15)>0)
          &&((p.z+1.6*15)*(p.z+1.6*15)+8.65*(p.x-0.13*15)*(p.x-0.13*15)+6.25*(p.y+0.32*15)*(p.y+0.32*15)>15*15)
        &&(!(((p.z+0.45*15)*(p.z+0.45*15)+(p.x-0.1*15)*(p.x-0.1*15)+(p.y-0.1*15)*(p.y-0.1*15)<0.01*15*15)
        ||((p.z+15<=0.55*15)&&((p.x-1.5)*(p.x-1.5)+(p.y-1.5)*(p.y-1.5)<0.01*15*15))))
  ) return 1;
    else return 0;  
   
}




double biolung_Tree::com_len(biolung_POINT3D p1,biolung_POINT3D p2)       //compute length of point p1 to p2
{
  if((p1.x==p2.x)&&(p1.y==p2.y)&&(p1.z==p2.z))return 0;
  else return vcl_sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y)+(p1.z-p2.z)*(p1.z-p2.z));
}

biolung_Vector biolung_Tree::com_vec(biolung_POINT3D p1,biolung_POINT3D p2)       //compute vector from p1 to p2
{
  biolung_Vector vec;
  double len;
  if((p1.x==p2.x)&&(p1.y==p2.y)&&(p1.z==p2.z)) {vec.l=0; vec.m=0; vec.n=0;}
  else
  {
    len=vcl_sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y)+(p1.z-p2.z)*(p1.z-p2.z));
    vec.l=(p2.x-p1.x)/len;
    vec.l=(p2.y-p1.y)/len;
    vec.l=(p2.z-p1.z)/len;
  }
  return vec;
}

biolung_POINT3D  biolung_Tree::endpoint(biolung_POINT3D p1,biolung_Vector vec, double len)  //compute endpoint if point,vec and length are specified
{
  biolung_POINT3D p2;
  p2.x=len*vec.l+p1.x;
  p2.y=len*vec.m+p1.y;
  p2.z=len*vec.n+p1.z;
  return p2;
}

double biolung_Tree::distance(biolung_Vector vec, biolung_POINT3D p)
{
  return -vec.l*p.x-vec.m*p.y-vec.n*p.z;
}

biolung_Vector biolung_Tree::product1(biolung_Vector vec1, biolung_Vector vec2)   //cross product of two vectors        
{
  biolung_Vector vec;
  vec.l= vec1.m*vec2.n-vec1.n*vec2.m;
  vec.m= vec1.n*vec2.l-vec1.l*vec2.n;
  vec.n= vec1.l*vec2.m-vec1.m*vec2.l;
  return vec;
}

biolung_Vector biolung_Tree::product2(biolung_Vector vec1, biolung_Vector vec2)   
{
  biolung_Vector vec;
  vec.l= -vec1.m*vec2.n+vec1.n*vec2.m;
  vec.m= -vec1.n*vec2.l+vec1.l*vec2.n;
  vec.n= -vec1.l*vec2.m+vec1.m*vec2.l;
  return vec;
}

biolung_Vector biolung_Tree::rotate1(biolung_Vector vec1, biolung_Vector vec2, double ang)  
{                                                           
  biolung_Vector vec;
  vec.l = vec1.l*vcl_cos(ang)+(vec2.m*vec1.n-vec2.n*vec1.m)*vcl_sin(ang);
  vec.m = vec1.m*vcl_cos(ang)+(vec2.n*vec1.l-vec2.l*vec1.n)*vcl_sin(ang);
  vec.n = vec1.n*vcl_cos(ang)+(vec2.l*vec1.m-vec2.m*vec1.l)*vcl_sin(ang);
  return vec;
}

biolung_Vector biolung_Tree::rotate2(biolung_Vector vec1, biolung_Vector vec2,  double ang)  
{                                                           
  biolung_Vector vec;
  vec.l = -(vec1.m*vec2.n-vec1.n*vec2.m)*vcl_sin(ang)+vec1.l*vcl_cos(ang);
  vec.m = -(vec1.n*vec2.l-vec1.l*vec2.n)*vcl_sin(ang)+vec1.m*vcl_cos(ang);
  vec.n = -(vec1.l*vec2.m-vec1.m*vec2.l)*vcl_sin(ang)+vec1.n*vcl_cos(ang);
  return vec;
}

int biolung_Tree::checkarea(biolung_Area ar,biolung_POINT3D pp,int g,biolung_Parent par)
{
  int h=1;

    if(shape(pp)==0) h=0;
    else
    {
    for(int p=0;p<g;++p)
      {
      if((p<PA)&&(0<par.vec[p].l*(pp.x-par.p[p].x)+par.vec[p].m*(pp.y-par.p[p].y)+par.vec[p].n*(pp.z-par.p[p].z))
        &&(par.vec[p].l*(pp.x-par.p[p].x)+par.vec[p].m*(pp.y-par.p[p].y)+par.vec[p].n*(pp.z-par.p[p].z)<par.len[p])
        &&((pp.x-par.p[p].x)*(pp.x-par.p[p].x)+(pp.y-par.p[p].y)*(pp.y-par.p[p].y)+(pp.z-par.p[p].z)*(pp.z-par.p[p].z)
        -(par.vec[p].l*(pp.x-par.p[p].x)+par.vec[p].m*(pp.y-par.p[p].y)+par.vec[p].n*(pp.z-par.p[p].z))
        *(par.vec[p].l*(pp.x-par.p[p].x)+par.vec[p].m*(pp.y-par.p[p].y)+par.vec[p].n*(pp.z-par.p[p].z))
        <excl*0.25*par.diam[p]*par.diam[p]))
      {h=0;  break;}
      else
      {
          if(ar.c[p]>=0)
        {
          if((ar.a[p].l*pp.x+ar.a[p].m*pp.y+ar.a[p].n*pp.z+ar.a4[p] < 0)
            ||(ar.b[p].l*pp.x+ar.b[p].m*pp.y+ar.b[p].n*pp.z+ar.b4[p] < 0))
          {h=0;  break;}
        }
        if(ar.c[p]<0)
        {
          if((ar.a[p].l*pp.x+ar.a[p].m*pp.y+ar.a[p].n*pp.z+ar.a4[p] < 0)
            &&(ar.b[p].l*pp.x+ar.b[p].m*pp.y+ar.b[p].n*pp.z+ar.b4[p] < 0))
          {h=0;  break;}
        }
      }
        }
    }
    return h;
}


biolung_Range biolung_Tree::comrara(biolung_Area ar,int g,biolung_Range ra,biolung_Parent par)       // Finding the range of the area
{
  biolung_Range rara;
  biolung_POINT3D p;

  int imax=1;
  int imin=9;
  int jmax=1;
  int jmin=9;
  int kmax=1;
  int kmin=9;

  double rax,ray,raz;
  rax=ra.xmax-ra.xmin;
  ray=ra.ymax-ra.ymin;
  raz=ra.zmax-ra.zmin;

  
    for(int i=1;i<10;i++)
    {
        for(int j=1;j<10;j++)
      {
            for(int k=1;k<10;k++)
        {
          p.x=ra.xmin+0.1*rax*i;
          p.y=ra.ymin+0.1*ray*j;
          p.z=ra.zmin+0.1*raz*k;
            if(checkarea(ar,p,g,par)==1)
                {
                   if(i>=imax) imax=i;
                if(i<=imin) imin=i;
              if(j>=jmax) jmax=j;
            if(j<=jmin) jmin=j;
                    if(k>=kmax) kmax=k;
                  if(k<=kmin) kmin=k;
            }
        }
          }
    }
    if(imin>imax)
    {
      rara.xmin=ra.xmin;  rara.xmax=ra.xmax;
      rara.ymin=ra.ymin;  rara.ymax=ra.ymax;
       rara.zmin=ra.zmin;  rara.zmax=ra.zmax;
    }
    else
    {
        rara.xmin=ra.xmin+0.1*(imin-1)*rax;
        rara.xmax=ra.xmin+0.1*(imax+1)*rax;
      rara.ymin=ra.ymin+0.1*(jmin-1)*ray;
        rara.ymax=ra.ymin+0.1*(jmax+1)*ray;
         rara.zmin=ra.zmin+0.1*(kmin-1)*raz;
        rara.zmax=ra.zmin+0.1*(kmax+1)*raz;
    }
    return rara;
}


double biolung_Tree::volratio(biolung_Area ar, int g,biolung_Range ra,biolung_Parent par)
{
  long p=0,q=0;
    double rax,ray,raz;
  biolung_POINT3D pp;
  
  rax=ra.xmax-ra.xmin;
  ray=ra.ymax-ra.ymin;
  raz=ra.zmax-ra.zmin;

  if(acura<=0) return 0;
  else
  {
    for(int i=0;i<acura;i++)
      {
         for(int j=0;j<acura;j++)
          {
          for(int k=0;k<acura;k++)
          {
              pp.x=rax*i/acura+ra.xmin;
              pp.y=ray*j/acura+ra.ymin;
          pp.z=raz*k/acura+ra.zmin;        
              if(checkarea(ar,pp,g,par)==1)
              {
                p++;
            if(((ar.c[g]>=0)&&(ar.a[g].l*pp.x+ar.a[g].m*pp.y+ar.a[g].n*pp.z +ar.a4[g] >=0) 
                &&(ar.b[g].l*pp.x+ar.b[g].m*pp.y+ar.b[g].n*pp.z +ar.b4[g] >=0))
              ||((ar.c[g]<0)&&((ar.a[g].l*pp.x+ar.a[g].m*pp.y+ar.a[g].n*pp.z +ar.a4[g] >=0) 
              ||(ar.b[g].l*pp.x+ar.b[g].m*pp.y+ar.b[g].n*pp.z +ar.b4[g] >=0))))q++;
          }
        }
      }
        }
    if(p>0) return double(q)/double(p);
    else return 0;
    }
}

double biolung_Tree::volratio2(biolung_Area ar, int g,biolung_Range ra,biolung_Parent par,biolung_Vector vec, biolung_POINT3D pd)
{
  long p=0,q=0;
    
  double rax,ray,raz;
  biolung_POINT3D pp;
  
  rax=ra.xmax-ra.xmin;
  ray=ra.ymax-ra.ymin;
  raz=ra.zmax-ra.zmin;

  if(acura<=0) return 0;
  else
  {
    for(int i=0;i<acura;i++)
      {
         for(int j=0;j<acura;j++)
          {
          for(int k=0;k<acura;k++)
          {
              pp.x=rax*i/acura+ra.xmin;
              pp.y=ray*j/acura+ra.ymin;
          pp.z=raz*k/acura+ra.zmin;
      
              if(checkarea(ar,pp,g,par)==1)
              {
                p++;
            if(vec.l*(pp.x-pd.x)+vec.m*(pp.y-pd.y)+vec.n*(pp.z-pd.z)<0) q++;            
          }
        }
      }
        }
    if(p>0) return double(q)/double(p);
    else return 0;
    }
}

biolung_POINT3D biolung_Tree::GetCenter(biolung_Area ar, int g,biolung_Parent par, biolung_Range ra)
{
  long p=0;
  double rax,ray,raz;
  double sumx=0,sumy=0,sumz=0;
  biolung_POINT3D center,pp;


  rax=ra.xmax-ra.xmin;
  ray=ra.ymax-ra.ymin;
  raz=ra.zmax-ra.zmin;

  
  if(acura<=0) {center.x=-100; center.y=-100;center.z=-100;}
  else
  {
    for(int i=0;i<acura;i++)
      {
         for(int j=0;j<acura;j++)
          {
          for(int k=0;k<acura;k++)
          {
              pp.x=rax*i/acura+ra.xmin;
              pp.y=ray*j/acura+ra.ymin;
          pp.z=raz*k/acura+ra.zmin;        
              if(checkarea(ar,pp,g,par)==1)
              {
                p++;
            sumx+=pp.x;
            sumy+=pp.y;
            sumz+=pp.z;
          }
        }
      }
        }
    if(p>0) 
    {
      center.x=sumx/p;
      center.y=sumy/p;
      center.z=sumz/p;
    }

    else
    {
      center.x=-100;
      center.y=-100;
      center.z=-100;
    }
    }
  return center;
}

double biolung_Tree::DisCenter1(biolung_POINT3D ce, biolung_POINT3D p1,biolung_Vector vec)
{
  double dis2,dis;

  dis2=((ce.x-p1.x)*vec.m-(ce.y-p1.y)*vec.l)*((ce.x-p1.x)*vec.m-(ce.y-p1.y)*vec.l)
    +((ce.y-p1.y)*vec.n-(ce.z-p1.z)*vec.m)*((ce.y-p1.y)*vec.n-(ce.z-p1.z)*vec.m)
    +((ce.z-p1.z)*vec.l-(ce.x-p1.x)*vec.n)*((ce.z-p1.z)*vec.l-(ce.x-p1.x)*vec.n);
  if(dis2>0) dis=vcl_sqrt(dis2);
  else dis=0;
  return dis;
}

double biolung_Tree::DisCenter2(biolung_POINT3D ce, biolung_POINT3D p1,biolung_Vector vec)
{
  double diss,dis1, dis22,dis2;

  diss=(p1.x-ce.x)*(p1.x-ce.x)+(p1.y-ce.y)*(p1.y-ce.y)+(p1.z-ce.z)*(p1.z-ce.z);
  dis1=DisCenter1(ce,p1,vec);
  dis22=diss-dis1*dis1;
  if(dis22<=0) dis2=0;
  else 
  {
    if((ce.x-p1.x)*vec.l+(ce.y-p1.y)*vec.m+(ce.z-p1.z)*vec.n>0) dis2=vcl_sqrt(dis22);
    else dis2=-vcl_sqrt(dis22);
  }
  return dis2;
}

double biolung_Tree::evenplane1(biolung_Area ar, int g,biolung_Range ra,biolung_Parent par,biolung_Vector vec, biolung_POINT3D pp,biolung_Vector norm)
{
  
  double rax,ray,raz;
  double ratio;
  // Can initialized even to get rid of gcc warning
  // There is a possibility that the initialization may not be correct
  double even=0;
  int i;
  biolung_Vector vec_r;


  rax=ra.xmax-ra.xmin;
  ray=ra.ymax-ra.ymin;
  raz=ra.zmax-ra.zmin;

  
  ratio=volratio2(ar,g,ra,par,vec,pp);
  if((ratio>0.45)&&(ratio<0.55)) even =0.5*PI;
  if(ratio<=0.45)
  {
    for(i=1;i<10;i++)
    {
      vec_r.l = vec.l*vcl_cos(i*0.05*PI)-(norm.n*vec.m-norm.m*vec.n)*vcl_sin(i*0.05*PI);
      vec_r.m = vec.m*vcl_cos(i*0.05*PI)-(norm.l*vec.n-norm.n*vec.l)*vcl_sin(i*0.05*PI);
      vec_r.n = vec.n*vcl_cos(i*0.05*PI)-(norm.m*vec.l-norm.l*vec.m)*vcl_sin(i*0.05*PI);
      even=0.5*PI-i*0.05*PI;
      if(volratio2(ar,g,ra,par,vec_r,pp)>0.45)  break;
    }
  }

    
  if(ratio>=0.55)
  {
    for(i=1;i<10;i++)
    {
      vec_r.l = vec.l*vcl_cos(i*0.05*PI)+(norm.n*vec.m-norm.m*vec.n)*vcl_sin(i*0.05*PI);
      vec_r.m = vec.m*vcl_cos(i*0.05*PI)+(norm.l*vec.n-norm.n*vec.l)*vcl_sin(i*0.05*PI);
      vec_r.n = vec.n*vcl_cos(i*0.05*PI)+(norm.m*vec.l-norm.l*vec.m)*vcl_sin(i*0.05*PI);
      even=0.5*PI+i*0.05*PI;
      if(volratio2(ar,g,ra,par,vec_r,pp)<0.55) even=0.5*PI+i*0.05*PI;
      break;
    }
  }
  return even;
}

double biolung_Tree::evenplane2(biolung_Area ar, int g,biolung_Range ra,biolung_Parent par,biolung_Vector vec, biolung_POINT3D pp,biolung_Vector norm)
{
  
  double rax,ray,raz;
  double ratio;
  // Can initialized even to get rid of gcc warning
  // There is a possibility that the initialization may not be correct
  double even=0;
  int i;
  biolung_Vector vec_r;

  rax=ra.xmax-ra.xmin;
  ray=ra.ymax-ra.ymin;
  raz=ra.zmax-ra.zmin;

  ratio=volratio2(ar,g,ra,par,vec,pp);
  if((ratio>0.45)&&(ratio<0.55)) even =0.5*PI;
  if(ratio<=0.45)
  {
    for(i=1;i<10;i++)
    {
      vec_r.l = vec.l*vcl_cos(i*0.05*PI)+(norm.n*vec.m-norm.m*vec.n)*vcl_sin(i*0.05*PI);
      vec_r.m = vec.m*vcl_cos(i*0.05*PI)+(norm.l*vec.n-norm.n*vec.l)*vcl_sin(i*0.05*PI);
      vec_r.n = vec.n*vcl_cos(i*0.05*PI)+(norm.m*vec.l-norm.l*vec.m)*vcl_sin(i*0.05*PI);
      even=0.5*PI-i*0.05*PI;
      if(volratio2(ar,g,ra,par,vec_r,pp)>0.45) break;
    }
  }
  if(ratio>=0.55)
  {
    for(i=1;i<10;i++)
    {
      vec_r.l = vec.l*vcl_cos(i*0.05*PI)-(norm.n*vec.m-norm.m*vec.n)*vcl_sin(i*0.05*PI);
      vec_r.m = vec.m*vcl_cos(i*0.05*PI)-(norm.l*vec.n-norm.n*vec.l)*vcl_sin(i*0.05*PI);
      vec_r.n = vec.n*vcl_cos(i*0.05*PI)-(norm.m*vec.l-norm.l*vec.m)*vcl_sin(i*0.05*PI);
      even=0.5*PI+i*0.05*PI;
      if(volratio2(ar,g,ra,par,vec_r,pp)<0.55) break;
    }
  }
  return even;
}

double biolung_Tree::seeklength(double diam, double llen, double rlen, biolung_POINT3D p1, biolung_Vector vec, biolung_Area ar, int g, biolung_Parent par)
{ 
  double len;
  int check=0;
  biolung_POINT3D p,pl,pr;

  for(int i=0;i<23;i++)
  {
    len=(3+0.125*i*pow(-1,i))*diam;
    if(len<0.5*diam)len=0.5*diam;
    p=endpoint(p1,vec,len);      
    pl=endpoint(p1,vec,llen*len);
    pr=endpoint(p1,vec,rlen*len);
    if((checkarea(ar,p,g,par)==1)&&(checkarea(ar,pl,g,par)==1)
      &&(checkarea(ar,pr,g,par)==0))
    {
      check=1;
      break;
    }
  }
  if(check>0)return len;
  else return 0;
}

double biolung_Tree::seeklengthC(double diam, double llen,double rlen,biolung_POINT3D p1, biolung_Vector vec, biolung_Area ar, int g, biolung_Parent par,biolung_Range ra)
{ 
  double len,discen1,discen2;
  int check=0;
  biolung_POINT3D p,pl,pr,ce;

  ce=GetCenter(ar,g,par,ra);
  discen1=DisCenter1(ce,p1,vec);
  discen2=DisCenter2(ce,p1,vec);

  if((discen1>4*diam))//||((discen1>3*diam)&&(discen1<=4*diam)&&(discen1<=5*diam)))
  {
        
    for(int i=0;i<11;i++)
    {
      len=(4*diam/discen1-0.125*i)*diam;
      if(len<0.5*diam)len=0.5*diam;
        
      p=endpoint(p1,vec,len);      
      
      if((checkarea(ar,p,g,par)==1))
      {
              check=1;
              break;
      }      
    }
  }
  else
  {
      for(int i=0;i<20;i++)
      {
        len=(3.3-0.125*i*pow(-1,i))*diam;
        if(len<0.5*diam)len=0.5*diam;
        
          
            p=endpoint(p1,vec,len);      
            pl=endpoint(p1,vec,llen*len);
            pr=endpoint(p1,vec,rlen*len);
            if((checkarea(ar,p,g,par)==1)&&(checkarea(ar,pl,g,par)==1)
              &&(checkarea(ar,pr,g,par)==-1)) 
            {
              check=1;
              break;
            }
          
        
      
      }
  }
  if(check>0)return len;
  else return 0;
}


double biolung_Tree::seeklength2(double diam, biolung_POINT3D p1, biolung_Vector vec, biolung_Area ar, int g, biolung_Parent par)
{ 
  double len;
  int check=0;
  biolung_POINT3D p;

  for(int i=0;i<23;i++)
  {
    len=(3+0.125*i*pow(-1,i))*diam;
    if(len<0.5*diam)len=0.5*diam;
    p=endpoint(p1,vec,len);      
    if(checkarea(ar,p,g,par)==1)
    {
      check=1;
      break;
    }
  }
  if(check>0)return len;
  else return 0;
}

double biolung_Tree::seeklength3(double high, double diam, biolung_POINT3D p1, biolung_Vector vec, biolung_Area ar, int g, biolung_Parent par)
{ 
  double len;
  
  biolung_POINT3D p;

  p=endpoint(p1,vec,diam);  
  if(checkarea(ar,p,g,par)==1)return diam;
  else
  {    
    for(int i=1;i<21;i++)
    {
      len=(1+0.125*i)*diam;
      if(len>high*diam) return 0;      
      else 
      {
        p=endpoint(p1,vec,len);  
          if(checkarea(ar,p,g,par)==1)return len;
      }
    }
    return 0;
  }
  
}

double biolung_Tree::com_ang(double r,double n)    //branching angle theta1 for given volume ratio
{
  double s, coss;
  
  if(r==0) s=0.5*PI;
  else 
  {
    if(n>0) 
    {
      coss=0.5*(1-pow(1-r,4/n)+pow(r,4/n))/pow(r, 2/n);
            s=vcl_acos(coss);
      if(s>0.5*PI) s=0.5*PI;
    }
    else s=0;
  }
  return s;          
}

double biolung_Tree::revers_com_ang(double ang,double n)    //  compute volume_ratio for ang
{    
  double cosa,r,comr;
  int i;

  cosa=vcl_cos(ang);
  
  for(i=1;i<20;i++)
  {
    r=i*0.025;
    comr=0.5*(1-pow(1-r,4/n)+pow(r,4/n))/pow(r, 2/n);
    if(cosa-comr<0) break;
  
  }

  return r;          
}


double biolung_Tree::correctdiam(double diam,double r,double ab)
{
  double d, r_ang,ab_r;

  r_ang=com_ang(r,dexp);
  ab_r=revers_com_ang(ab,dexp);
  if((ab_r>0)&&(r>ab_r))
  {
    d=pow((r/ab_r),1/dexp)*diam;
    if(d>1.5*diam) d=1.5*diam;
  }
  else d=diam;

    return d;

}


long biolung_Tree::computeTree()
{   
    long i=0;
  int j;
  // Can initialized lab_r and lab_l to get rid of gcc warning
  // There is a possibility that the initialization may not be correct
  int lab_p,lab_r=0,lab_l=0;
  int check_p,check_r,check_l;
  int g_p,g_r,g_l;
  // Can initialized r_r and r_l to get rid of gcc warning
  // There is a possibility that the initialization may not be correct
  double r_p,r_r=0,r_l=0,f_p, f_r, f_l,diam_p,diam_r,diam_l;
  biolung_POINT3D p1_p, p1_r, p1_l,p2_p, p2_r, p2_l;
  biolung_Vector vec_p={0,0,0}, vec_r={0,0,0},vec_l={0,0,0}, norm_p={0,0,0},norm_r={0,0,0},norm_l={0,0,0};
  biolung_Area ar_p,ar_r,ar_l;   
  biolung_Range ra_p,ra_r,ra_l;
  biolung_Parent par_p,par_r,par_l;
 
  // Can initialized variables to get rid of gcc warning
  // There is a possibility that the initialization may not be correct
  double ab_r=0, ab_l=0,ab,len_r=0,len_l=0;
  // Can initialized rot_r and rot_l to get rid of gcc warning
  // There is a possibility that the initialization may not be correct
  double theta,theta_r,theta_l,rot_r=0,rot_l=0,rot;

  double the_lim=0.08;

  biolung_AwStack awstack(50); // get a stack of 50
    if ( awstack.message == ERROR ) return 0;


  //initialising the starting point

  lab_p=-1;  g_p=0;   f_p=1.0; diam_p=0.12*15;  
  r_p=0.52;  
  p1_p.z=0; p1_p.x=0; p1_p.y=-0.09*15; 
  p2_p.z=-0.62*15;   p2_p.x=0;   p2_p.y=0;
  vec_p.n=-0.9903; vec_p.l=0; vec_p.m=vcl_sqrt(1-0.9903*0.9903);
  norm_p.n=-vcl_sqrt(1-0.9903*0.9903); norm_p.l=0; norm_p.m=-0.9903;


  ar_p.a[0].n=0;
  ar_p.a[0].l=-1;
  ar_p.a[0].m=0;
  ar_p.a4[0]=0;
  ar_p.b[0].l=0;
  ar_p.b[0].m=0;
  ar_p.b[0].n=0;
  ar_p.b4[0]=0;
  ar_p.c[0]=0;
   
  for (int p=1; p<LM;++p)    //intializing area
    {
    ar_p.a[p].l=0;
        ar_p.a[p].m=0;
        ar_p.a[p].n=0;
        ar_p.a4[p]=0;
    ar_p.b[p].l=0;
      ar_p.b[p].m=0;
    ar_p.b[p].n=0;
    ar_p.b4[p]=0;
    ar_p.c[p]=0;
  }

  //intializing range
  ra_p.zmax=0; ra_p.zmin=-2.3*15; ra_p.xmax=1.05*15; ra_p.xmin=-1.05*15;
   ra_p.ymax=0.72*15;ra_p.ymin=-0.72*15;


   for(int a=0;a<PA;a++)   //intializing parent
   {
     par_p.p[a].x=0;
     par_p.p[a].y=0;
     par_p.p[a].z=0;
     par_p.vec[a].l=0;
     par_p.vec[a].m=0;
     par_p.vec[a].n=0;
     par_p.len[a]=0;
     par_p.diam[a]=0;
   }


    //putting first branch into the array bra

  bra[0].p1=p1_p;   bra[0].p2=p2_p;  
  bra[0].diam=diam_p; bra[0].check=0; bra[0].lab=lab_p;

  overflow=0;ovnum=0;
  overflow1=0;ovnum1=0;

  ++i;

  do                      //  Algorithm starts here
    {
        g_r=g_p+1;
        g_l=g_p+1;
        f_r = r_p*f_p;
        f_l = (1-r_p)*f_p;
        diam_r=pow(r_p,1./dexp)*diam_p;
        diam_l=pow(1-r_p,1./dexp)*diam_p;
    p1_r=p2_p; p1_l=p2_p;
    ar_r=ar_p;
    ar_l=ar_p;
        ar_l.a[g_l-1].l=-ar_p.a[g_p].l;
        ar_l.a[g_l-1].m=-ar_p.a[g_p].m;
        ar_l.a[g_l-1].n=-ar_p.a[g_p].n;
        ar_l.a4[g_l-1]=-ar_p.a4[g_p];
    ar_l.b[g_l-1].l=-ar_p.b[g_p].l;
        ar_l.b[g_l-1].m=-ar_p.b[g_p].m;
        ar_l.b[g_l-1].n=-ar_p.b[g_p].n;
        ar_l.b4[g_l-1]=-ar_p.b4[g_p];
    ar_l.c[g_l-1]=-ar_p.c[g_p];
  
    ra_r=comrara(ar_r,g_r,ra_p,par_p);
    ra_l=comrara(ar_l,g_l,ra_p,par_p);

    if(lab_p<27)    // Assignment of proximal branches down to segmental branches
    {
        theta_r=0; theta_l=0;    
        if(lab_p==-1) //Trachea
        {
          len_r=1.8*diam_r;  ab_r=0.28*PI;  rot_r=0; lab_r=0; //RMain
          len_l=4*diam_l;  ab_l=0.23*PI;  rot_l=0; lab_l=0;     //LMain
          theta_r=0.12*PI;
        }
        if((lab_p==0)&&(i==2)) // RMain
        {  
          len_r=2*diam_r;      ab_r=0.42*PI;  rot_r=0.68; lab_r=1; //RU
          len_l=2.7*diam_l;  ab_l=0.1*PI;  rot_l=0.57; lab_l=10;  //RI
          theta_l=-0.2*PI;  theta_r=0.2*PI;
        }
        
        if(lab_p==1) // RU
        {
          len_l=1*diam_l;  ab_l=0.3*PI;  rot_l=0.6, lab_l=13; //R13
          len_r=2*diam_r;  ab_r=0.35*PI;  rot_r=0.5; lab_r=102;  //R2
        }
        if(lab_p==13) //R13
        {
          len_r=2*diam_r;  ab_r=0.22*PI;  rot_r=0.3;  lab_r=103;//R3
          len_l=1.5*diam_l;  ab_l=0.25*PI;  rot_l=0.8;  lab_l=101;//R1 
          theta_r=0.1*PI;
        }
        if(lab_p==10)  // RI
        {        
          len_l=2.7*diam_l;  ab_l=0.35*PI;  rot_l=0.65;lab_l=2; //RM
          len_r=1*diam_r;  ab_r=0.1*PI;  rot_r=0.85;  lab_r=11;  //RL
          theta_r=-0.15*PI; theta_l=0*PI;
        }
        if(lab_p==2) //RM
        {
          len_r=3*diam_r;  ab_r=0.15*PI;  rot_r=0.2;  lab_r=105;//R5
          len_l=1.5*diam_l;  ab_l=0.15*PI;  rot_l=0.5; lab_l=104;//R4

        }
        if(lab_p==11) // RL
        {        
          len_r=1.2*diam_r;  ab_r=0.07*PI;  rot_r=0.6; lab_r=3;
          len_l=1.5*diam_l;  ab_l=0.43*PI;  rot_l=0.7; lab_l=106;
          theta_r=-0.2*PI;
        }
        
      
        if(lab_p==3)// RBasal
        {
          len_r=1*diam_r;  ab_r=0.08*PI;  rot_r=0.65; lab_r=4;
          len_l=4*diam_l;  ab_l=0.25*PI;  rot_l=0.5; lab_l=107;
          theta_r=-0.1*PI;
        
        }
        if(lab_p==4) //R8910
        {
          len_r=1*diam_r;  ab_r=0.1*PI;  rot_r=0.35;  lab_r=5;
          len_l=1.5*diam_l;  ab_l=0.25*PI;  rot_l=0.55; lab_l=108;//R8
          theta_r=0.03*PI;
        }
        if(lab_p==5) // R910
        {        
          len_l=3*diam_l;    ab_l=0.13*PI;  rot_l=0.35;  lab_l=109;//R9
          len_r=2.5*diam_r;  ab_r=0.21*PI;  rot_r=0.35;  lab_r=110; //R10
          theta_l=0.05*PI;
        }
      
        if((lab_p==0)&&(i!=2)) // LMain
        {
          len_r=1.5*diam_r;  ab_r=0.35*PI;  rot_r=0.55;  lab_r=21; //LL
          len_l=2*diam_l;  ab_l=0.28*PI;  rot_l=0.8;  lab_l=22; //LU
          theta_r=0.08*PI; theta_l=-0.22*PI;
        }
        if(lab_p==22)  //LU
        {
          len_r=2*diam_r;  ab_r=0.2*PI;  rot_r=0.6; lab_r=23; //L123
          len_l=3*diam_l;  ab_l=0.4*PI;  rot_l=0.6; lab_l=27; //L45
          theta_r=0*PI, theta_l=0.12*PI;
        }
        if(lab_p==23) //L123
        {
          len_l=2*diam_l;  ab_l=0.27*PI;  rot_l=0.5;  lab_l=201; //L1+2
          len_r=1*diam_r;  ab_r=0.23*PI;  rot_r=0.1;  lab_r=203; //L3
          theta_r=-0.1*PI;
        }
    
        if(lab_p==21) //LL
        {
          len_l=2*diam_l;  ab_l=0.15*PI;  rot_l=0.8;    lab_l=25; //LBasal
          len_r=1.5*diam_r;  ab_r=0.4*PI;  rot_r=0.65;   lab_r=206; //L6
          theta_l=0.08*PI;
        }
        if(lab_p==25) //LBasal
        {
          len_r=1.5*diam_r;  ab_r=0.25*PI;  rot_r=0.6;  lab_r=208;//L8
          len_l=1*diam_l;  ab_l=0.14*PI;  rot_l=0.8; lab_l=26;//L910
        //  theta_r=0.08*PI;
        }
        
        if(lab_p==26) //LBasal
        {
          len_r=2.5*diam_r;  ab_r=0.18*PI;  rot_r=0.6;  lab_r=210;//L10
          len_l=2.5*diam_l;  ab_l=0.22*PI;  rot_l=0.5; lab_l=209;//L9
        }
        
      
        vec_r=rotate1(vec_p, norm_p, -ab_r);         
        p2_r=endpoint(p1_r, vec_r,len_r);
        norm_r=rotate2(norm_p,vec_r, rot_r*PI);
        ar_r.a[g_r]=product1(vec_r,norm_r);
        ar_r.a4[g_r]=distance(ar_r.a[g_r], p1_r);
        ar_r.b[g_r]=rotate1(ar_r.a[g_r], norm_r,-theta_r);
        ar_r.b4[g_r]=distance(ar_r.b[g_r], p2_r);
        if(theta_r>0)ar_r.c[g_r]=1;
        if(theta_r==0)ar_r.c[g_r]=0;
        if(theta_r<0)ar_r.c[g_r]=-1;
        r_r=volratio(ar_r,g_r,ra_r,par_r);
        
        vec_l=rotate1(vec_p, norm_p, ab_l);
        p2_l=endpoint(p1_l, vec_l,len_l);                 
        if(g_p==0)
        {
            vec_l.n=0; vec_l.l=1; vec_l.m=0;
            ar_l.a[g_l].n =-0.50;  //35degree
               ar_l.a[g_l].l =0.0;
             ar_l.a[g_l].m =vcl_sqrt(0.750);
            ar_l.a4[g_l]=distance(ar_l.a[g_l],p2_l)-0.08;          
              norm_l.n=-0.5736;  norm_l.l=0.0;   norm_l.m=-vcl_sqrt(1-0.5736*0.5736);
            r_l=volratio(ar_l,g_l,ra_l,par_l);
        }
        else
        {
          norm_l=rotate2(norm_p,vec_l, rot_l*PI);
          ar_l.a[g_l]=product1(vec_l,norm_l);
          ar_l.a4[g_l]=distance(ar_l.a[g_l], p1_l);
          ar_l.b[g_l]=rotate1(ar_l.a[g_l], norm_l,-theta_l);
          ar_l.b4[g_l]=distance(ar_l.b[g_l], p2_l);
          if(theta_l>0)ar_l.c[g_l]=1;
          if(theta_l==0)ar_l.c[g_l]=0;
          if(theta_l<0)ar_l.c[g_l]=-1;
          r_l=volratio(ar_l,g_l,ra_l,par_l);
        }
        check_r=1; check_l=1;

      /*  if(r_r<=0.5)diam_r=correctdiam(diam_r,r_r,ab_r);
        else diam_r=correctdiam(diam_r,1-r_r,ab_r);
        if(diam_r>diam_p)diam_r=diam_p; 
        if(r_l<=0.5)
                    diam_l=correctdiam(diam_l,r_l,ab_l);
                    else diam_l=correctdiam(diam_l,1-r_l,ab_l);
                    if(diam_l>diam_p)diam_l=diam_p; */

    }

    else
    {
          
      if(lab_p==27){lab_r=205; lab_l=204;}
      if(lab_p>27) {lab_r=lab_p; lab_l=lab_p;}

      ab_r=com_ang(r_p,dexp);
      ab_l=com_ang(1-r_p,dexp);
      check_r=-8;check_l=-8;

      if(f_r<=thresh)
      {
        vec_r=rotate1(vec_p, norm_p, -ab_r);         
        len_r=m_ldr*diam_r;
        p2_r=endpoint(p1_r, vec_r,len_r);
        if(shape(p2_r)!=1) p2_r=p1_r;    
        r_r=0; check_r=-1;  norm_r=norm_p;
      }
      if(f_l<=thresh)
      {
        vec_l=rotate1(vec_p, norm_p, ab_l);
        len_l=m_ldr*diam_l;
        p2_l=endpoint(p1_l, vec_l,len_l);
        if(shape(p2_l)!=1) p2_l=p1_l;            
        r_l=0; check_l=-1; norm_l=norm_p;
      }

      if(f_r>thresh)
      {
        check_r=-8;
        ab=evenplane1(ar_r, g_r,ra_r,par_r,vec_p, p2_p, norm_p);
        if(ab_r<ab)
        {
           if(ab_r<ab)
          {
              check_r=-10;
            if(ab_r-ab<-0.16*PI) {check_r=-18;}
            ab_r=0.5*ab_r+0.5*ab;
            if(ab_r>0.5*PI) ab_r=0.5*PI; 
          }
        }    
        vec_r=rotate1(vec_p, norm_p, -ab_r);
        
        if(check_r==-18) len_r=seeklength3(2.5,diam_r, p1_r, vec_r, ar_r,g_r,par_r);
        else
        {
              len_r=seeklength(diam_r, l_len, r_len, p1_r, vec_r, ar_r,g_r,par_r);
          if(len_r==0) len_r=seeklength2(diam_r, p1_r, vec_r, ar_r,g_r,par_r);
        }
          if(len_r>0)
          {
            p2_r=endpoint(p1_r, vec_r, len_r);
            for(j=0;j<21;j++)
            {
              rot=(0.5-0.025*j*pow(-1,j))*PI;
               norm_r=rotate2(norm_p,vec_r, rot);
              ar_r.a[g_r]=product1(vec_r,norm_r);
              ar_r.a4[g_r]=distance(ar_r.a[g_r], p1_r);
              
              r_r=volratio(ar_r,g_r,ra_r,par_r);
              if((r_r>min)&&(r_r<1-min))
              {
                theta=0.5*(com_ang(r_r,dexp)-com_ang(1-r_r,dexp));
                if(theta>0.24*PI) theta=0.24*PI;
                if(theta<-0.24*PI)theta=-0.24*PI;
                if((theta<the_lim*PI)&&(theta>-the_lim*PI))theta=0;
                if(theta!=0)
                {
                  ar_r.b[g_r]=rotate1(ar_r.a[g_r], norm_r,theta);
                  ar_r.b4[g_r]=distance(ar_r.b[g_r], p2_r);
                  if(theta>0)ar_r.c[g_r]=1;
                  if(theta<0)ar_r.c[g_r]=-1;
                  r_r=volratio(ar_r,g_r,ra_r,par_r);
                }        
              }
              if((r_r>min)&&(r_r<1-min))
              { 
                if(check_r==-10)
                {
                  if(r_r<=0.5)diam_r=correctdiam(diam_r,r_r,ab_r);
                  else diam_r=correctdiam(diam_r,1-r_r,ab_r);
                  if(diam_r>diam_p)diam_r=diam_p; 
                }
                
                    check_r=1; 
                    if(j>0) ovnum1++;
                    break;
              }
            }
        }

        if(check_r<-1)
        {
          len_r=diam_r;
          p2_r=endpoint(p1_r, vec_r,len_r);
          if(shape(p2_r)!=1) p2_r=p1_r;  
          r_r=0; norm_r=norm_p;
          overflow+=f_r-thresh;
          ovnum++;
        }
      }        
      
      if(f_l>thresh)
      {
        check_l=-8; 
        ab=evenplane2(ar_l, g_l,ra_l,par_l,vec_p, p2_p,norm_p);
        if(ab_l<ab)
        {
          if(ab_l-ab<-0.16*PI){ check_l=-18; }
          else check_l=-10;
          ab_l=0.5*ab_l+0.5*ab;  
          if(ab_l>0.5*PI) ab_l=0.5*PI;
        }
      
        vec_l=rotate1(vec_p, norm_p, ab_l);

          if(check_l==-18)len_l=seeklength3(2.5,diam_l, p1_l, vec_l, ar_l,g_l,par_l);
          else 
          {
            len_l=seeklength(diam_l, l_len, r_len, p1_l, vec_l, ar_l,g_l,par_l);
            if(len_l==0) len_l=seeklength2(diam_l, p1_l, vec_l, ar_l,g_l,par_l);
          }
          if(len_l>0)  
          {
            p2_l=endpoint(p1_l, vec_l, len_l);
            for(j=0; j<21; j++)
            {
              rot=(0.5-0.025*j*pow(-1,j))*PI;
               norm_l=rotate2(norm_p,vec_l, rot);
              ar_l.a[g_l]=product1(vec_l,norm_l);
              ar_l.a4[g_l]=distance(ar_l.a[g_l],p1_l);
          
              r_l=volratio(ar_l,g_l,ra_l,par_l);
              if((r_l>min)&&(r_l<1-min))
              {
                theta=0.5*(com_ang(r_l,dexp)-com_ang(1-r_l,dexp));
                if(theta>0.24*PI) theta=0.24*PI;
                if(theta<-0.24*PI)theta=-0.24*PI;
                if((theta<the_lim*PI)&&(theta>-the_lim*PI))theta=0;
                if(theta!=0)
                {          
                  ar_l.b[g_l]=rotate1(ar_l.a[g_l], norm_l,theta);
                  ar_l.b4[g_l]=distance(ar_l.b[g_l],p2_l);
                  if(theta>0)ar_l.c[g_l]=1;
                  if(theta<0)ar_l.c[g_l]=-1;
                  r_l=volratio(ar_l,g_l,ra_l,par_l);
                }
              }
              if((r_l>min)&&(r_l<1-min))
              {
                if(check_l==-10)
                {
                  if(r_l<=0.5)
                    diam_l=correctdiam(diam_l,r_l,ab_l);
                    else diam_l=correctdiam(diam_l,1-r_l,ab_l);
                    if(diam_l>diam_p)diam_l=diam_p; 
                }
                 check_l=1; 
                 if(j>0) ovnum1++;
                 break;
              }
            }
        }
        if(check_l<-1)
        {
          len_l=diam_l;
          p2_l=endpoint(p1_l,vec_l,len_l);
          if(shape(p2_l)!=1) p2_l=p1_l;  
          r_l=0; norm_l=norm_p;
          overflow+=f_l-thresh;
          ovnum++;
    
        }
      }
    }

    if(g_p<PA)
    {
      par_r.p[g_p]=p1_r;
      par_r.vec[g_p]=vec_r;
      par_r.len[g_p]=len_r;
      par_r.diam[g_p]=diam_r;
      par_l.p[g_p]=p1_l;
      par_l.vec[g_p]=vec_l;
      par_l.len[g_p]=len_l;
      par_l.diam[g_p]=diam_l;
    }
  
    bra[i].p1=p1_r;
    bra[i].p2=p2_r;
    bra[i].diam=diam_r;
    if(lab_r<100) bra[i].lab=0;
    if((lab_r>100)&&(lab_r<200)) bra[i].lab=lab_r-100;
    if((lab_r>200)&&(lab_r<300)) bra[i].lab=lab_r-190;
    bra[i].check=check_r;

    i++;
    if (i== MAX)  break;

    awstack.Push(lab_l,check_l,g_l,r_l,f_l,diam_l,p1_l,p2_l,vec_l,norm_l,ar_l,ra_l,par_l);

        if (awstack.message == ERROR)
        {
            vcl_cout << "\nStack full!!" << vcl_endl;
            return i-1;
        }


      // rightward daughter becomes parent

            lab_p=lab_r; check_p=check_r; g_p=g_r; r_p=r_r; f_p=f_r; diam_p=diam_r; 
      p1_p=p1_r; p2_p=p2_r; vec_p=vec_r; norm_p=norm_r; ar_p=ar_r;  ra_p=ra_r; par_p=par_r;

            while (check_p<0)
            {
              if ( awstack.GetNumber() <0 ) return i;   // end of stack and algorithm                 
                aw->FillAw( awstack.Pop() );

                lab_p=aw->GetLab();  check_p=aw->GetCheck();  g_p = aw->GetGen(); 
        r_p=aw->GetRatio();  f_p = aw->GetFlow();     diam_p=aw->GetDiam(); 
                p1_p=aw->GetP1();   p2_p=aw->GetP2();        vec_p=aw->GetVec(); norm_p=aw->GetNorm();   
        ar_p=aw->GetAr();    ra_p=aw->GetRa();        par_p=aw->GetPar(); 
        
        bra[i].p1=p1_p;
        bra[i].p2=p2_p;
           bra[i].diam=diam_p;
          if(lab_p<100) bra[i].lab=0;
        if((lab_p>100)&&(lab_p<200)) bra[i].lab=lab_p-100;
        if((lab_p>200)&&(lab_p<300)) bra[i].lab=lab_p-190;
        bra[i].check=check_p;
        
                if ( ++i == MAX)  break;
           }
    } while (i<MAX);
  return i;
}

