// This is contrib/biotree/biocts/biocts_Volume3D.cxx

//:
// \file

#include "biocts_Volume3D.h"
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cmath.h>

biocts_Volume3D::biocts_Volume3D(int _xdim, int _ydim, int _zdim):
  xdim(_xdim), ydim(_ydim), zdim(_zdim)
{
  int i,j,k;
  
  //default volume dimensions
  if (xdim==0 && ydim==0 && zdim==0){
    xdim =300;
    ydim =300;
    zdim =500;
  }

  //make a 
  // cast to double by Can to fix error on one of the machines
  Dia= (int) sqrt(double(xdim*xdim+ydim*ydim)); //Dia=(xdim>ydim)?xdim:ydim;
  data = new int**[Dia];
  for (i=0; i<Dia; i++)
  {
    data[i] = new int*[Dia];
    for (j=0; j<Dia; j++) 
    {
      data[i][j] = new int[zdim];
    }
  }

  for(i=0;i<Dia;i++)
     for(j=0;j<Dia;j++)
        for(k=0;k<zdim;k++)
          data[i][j][k]=0;
}

biocts_Volume3D::~biocts_Volume3D()
{
  int i,j;

  //delete volumetric array
  for(i=0;i<Dia;i++){
    for(j=0;j<Dia;j++){
      delete []data[i][j];
    }
    delete []data[i];
  }
  delete []data;
}

biocts_Volume3D::biocts_Volume3D(vcl_string volumedatafile)
{
  int i,j,k;

  vcl_ifstream Instream(volumedatafile.c_str(), vcl_ios::in);

  Instream >> xdim >> ydim >> zdim;

  // cast to double by Can to fix error on one of the machines
  Dia=(int) sqrt(double(xdim*xdim+ydim*ydim)); //Dia=(xdim>ydim)?xdim:ydim;

  data = new int**[Dia];
  for (i=0; i<Dia; i++)
  {
    data[i] =  new int*[Dia];
    for (j=0; j<Dia; j++) 
    {
      data[i][j] = new int[zdim];
    }
  }

 for(i=0;i<Dia;i++)
     for(j=0;j<Dia;j++)
        for(k=0;k<zdim;k++)
          data[i][j][k]=0;

  //read from file
  int xs, ys;
  xs = Dia/2-xdim/2;
  ys = Dia/2-ydim/2;

  for(i=0;i<xdim;i++)
    for(j=0;j<ydim;j++)
      for(k=0;k<zdim;k++)
        {
          Instream>>data[i+xs][j+ys][k];
        }

  Instream.close();
}


void biocts_Volume3D::saveVolumeInFile(vcl_string filename)
{
  int i,j,k;

  vcl_ofstream Ostream(filename.c_str(), vcl_ios::out);

  //volume file size header info
  Ostream << Dia << " " << Dia << " " << zdim << " ";

  for(i=0;i<Dia;i++){
    for(j=0;j<Dia;j++){
      for(k=0;k<zdim;k++){
        if(data[i][j][k]==1)
          Ostream<<1<<" ";
        else
          Ostream<<0<<" ";
      }
    }
  }

  Ostream.close();
}

void biocts_Volume3D::rotate_volume(biocts_Volume3D* new_ob, double ang)
{
  double xx, yy;
  int i,j,k, xp, yp;
  
  for(i=0;i<Dia;i++)
    for(j=0;j<Dia;j++)
      for(k=0;k<zdim;k++)
        new_ob->data[i][j][k]=0;


  for(k=0;k<zdim;k++){
    for(i=0;i<Dia;i++){
      for(j=0;j<Dia;j++){

         xx = i - Dia/2.0;
         yy = j - Dia/2.0;
         
         xp = int(xx*cos(ang) + yy*sin(ang) + Dia/2.0);
         yp = int(-xx*sin(ang) + yy*cos(ang) + Dia/2.0);
         
         if(xp>=0 && xp<Dia && yp>=0 && yp<Dia)
            new_ob->data[i][j][k]=data[xp][yp][k];
      }
    }
  }

}
