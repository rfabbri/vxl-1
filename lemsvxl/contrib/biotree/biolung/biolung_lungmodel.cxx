// This is contrib/biotree/biocts/biolung_lungmodel.cxx

//:
// \file

#include "biolung_lungmodel.h"

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cmath.h>

void getVector(double x1,double y1,double z1,double x2,double y2,double z2,double &x,double &y,double &z);
double getDotProduct(double vx1,double vy1,double vz1,double vx2,double vy2,double vz2);
double getLength(double x1,double y1,double z1,double x2,double y2,double z2);

biolung_lungmodel::biolung_lungmodel(int _zdim)
{
  int i,j,k;

  zdim = _zdim;     
  xdim = 500*zdim/700;  //assigning volume to the lung approximating its size
  ydim = 400*zdim/700;  //approximate ratio of xdim:ydim:zdim 5:4:7

  double f = 700.0/double(_zdim);  //facotor by which the coordinates have to be scaled

  g = new int**[xdim+1];  //used dim+1 to accomodate hollow tubular model
  for (i=0; i<xdim+1; i++){
    g[i] = new int*[ydim+1];
    for (j=0; j<ydim+1; j++){
      g[i][j] = new int[zdim+1];
    }
  }

  //initializing grid
  for(i=0;i<xdim+1;i++)
    for(j=0;j<ydim+1;j++)
      for(k=0;k<zdim+1;k++)
        g[i][j][k]=0;


  //compute the lung tree
  long num_branches = tree.computeTree();
  
  double d=0,x1=0,y1=0,z1=0,x2=0,y2=0,z2=0,rmin=1;
  double px=0,py=0,pz=0,r=0,t=0,vx1=0,vy1=0,vz1=0,vx2=0,vy2=0,vz2=0,fpx=0,fpy=0,fpz=0,dist;
  int X_min,X_max,Y_min,Y_max,Z_min,Z_max;

  for (int count=0; count<num_branches; count++){

    d = tree.bra[count].diam;
    x1 = tree.bra[count].p1.x;
    y1 = tree.bra[count].p1.y;
    z1 = tree.bra[count].p1.z;
    x2 = tree.bra[count].p2.x;
    y2 = tree.bra[count].p2.y;
    z2 = tree.bra[count].p2.z;

    //creating bounding box

    if(x1>x2)
    {X_min=int((x2-rmin+12.5)/(0.05*f));  //warning fixed by Can
     X_max=int((x1+rmin+12.5)/(0.05*f));} //warning fixed by Can}
    else
    {X_min=int((x1-rmin+12.5)/(0.05*f));  //warning fixed by Can
     X_max=int((x2+rmin+12.5)/(0.05*f));} //warning fixed by Can

    if(y1>y2)
    {Y_min=int((y2-rmin+10)/(0.05*f));  //warning fixed by Can
     Y_max=int((y1+rmin+10)/(0.05*f));} //warning fixed by Can
    else
    {Y_min=int((y1-rmin+10)/(0.05*f));  //warning fixed by Can
     Y_max=int((y2+rmin+10)/(0.05*f));} //warning fixed by Can

    if(z1>z2)
    {Z_min=int((z2-rmin+35)/(0.05*f));  //warning fixed by Can
     Z_max=int((z1+rmin+35)/(0.05*f));} //warning fixed by Can
    else
    {Z_min=int((z1-rmin+35)/(0.05*f));  //warning fixed by Can
     Z_max=int((z2+rmin+35)/(0.05*f));} //warning fixed by Can

    if(X_min<0)
      X_min=0;

    if(Z_min<0)
      Z_min=0;

    if(Y_min<0)
      Y_min=0;

    if(X_max>xdim)
      X_max=xdim;

    if(Y_max>ydim)
      Y_max=ydim;

    if(Z_max>zdim)
      Z_max=zdim;


    for(i=X_min;i<X_max;i++){
      for(j=Y_min;j<Y_max;j++){
        for(k=Z_min;k<Z_max;k++){
          
          if(g[i][j][k]!=1)
          {
            r=d/2;
            //point equivalent of voxel
            px=0.05*f*i-12.5;         //reason for 0.05 is the lung in Z direction is 35 cms long
            py=0.05*f*j-10;           //so 35/700=0.05 each voxel has size 0.05cm and f scales it
            pz=0.05*f*k-35;           //actual dimension of voxel =0.05*f
        
            getVector(x1,y1,z1,
                  x2,y2,z2,
                  vx1,vy1,vz1);
        
            getVector(x1,y1,z1,
                    px,py,pz,
                  vx2,vy2,vz2);

            t=(getDotProduct(vx1,vy1,vz1,vx2,vy2,vz2)/(getLength(x1,y1,z1,x2,y2,z2)*getLength(x1,y1,z1,x2,y2,z2)));
            
            if((t<=1)&&(t>=0))
            {
              //get footpoint
              fpx=x1+t*vx1;
              fpy=y1+t*vy1;
              fpz=z1+t*vz1;

              dist=getLength(fpx,fpy,fpz,    //get distance of point from footpoint
                      px,py,pz);
              if(dist<=r)            //see if voxel is inside the lung
                g[i][j][k]=1;        //if yes then fill voxel
            }
          }
        }
      }
    }

  } //end of branches loop
 
}

void
biolung_lungmodel::saveVolumeInFile(vcl_string filename)
{
  int i,j,k;

  vcl_ofstream Ostream(filename.c_str(), vcl_ios::out);

  //volume file size header info
  Ostream << xdim-1 << " " << ydim-1 << " " << zdim-1 << " ";

  for(i=1;i<xdim;i++){
    for(j=1;j<ydim;j++){
      for(k=1;k<zdim;k++){
      //use condition if(g[i][j][k]==1) for solid lung model
        if((g[i][j][k]==1)&&((g[i][j][k-1]==0)||(g[i][j][k+1]==0)||(g[i+1][j][k]==0)||(g[i-1][j][k]==0)||(g[i][j+1][k]==0)||(g[i][j-1][k]==0))) //This condition is for hollow tubular model
          Ostream<<100<<" ";  //assuming 100 to be CT number of the lung tissues
        else
          Ostream<<0<<" ";
      }
    }
  }

}

biolung_lungmodel::~biolung_lungmodel()
{
  int i,j;

  //delete volumetric array
  for(i=0;i<xdim+1;i++){
    for(j=0;j<ydim+1;j++){
      delete []g[i][j];
    }
    delete []g[i];
  }
  delete []g;
}

void getVector(double x1,double y1,double z1,
              double x2,double y2,double z2,
              double &x,double &y,double &z)
{
  x=x2-x1;
  y=y2-y1;
  z=z2-z1;
  return;
}


double getDotProduct(double vx1,double vy1,double vz1,
          double vx2,double vy2,double vz2)
{
  double dp;
  dp=vx1*vx2+vy1*vy2+vz1*vz2;
  return dp;
}

double getLength(double x1,double y1,double z1,
        double x2,double y2,double z2)
{
  double len;
  len=pow((pow((x1-x2),2)+pow((y1-y2),2)+pow((z1-z2),2)),.5);
  return len;
}
