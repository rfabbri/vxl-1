// This is contrib/biotree/biocts/biocts_ctsim.cxx

//:
// \file

#include "biocts_rngs.h"      //random number generator
#include "biocts_rvgs.h"      //generates a random variable with poisson distribution contains other functions also

#include "biocts_Volume3D.h"
#include "biocts_ctsim.h"
#include <vcl_cmath.h>

biocts_ctsim::biocts_ctsim(int _lambda, int _backgrnd_noise, int _gain, int _NViews, biocts_Volume3D* volume):
   lambda(_lambda), Backgrnd_Noise(_backgrnd_noise), gain(_gain), xdim(volume->xdim), ydim(volume->ydim), 
   zdim(volume->zdim), Dia(volume->Dia), NViews(_NViews), orig_vol(volume)
{

  int i,j,k,scsizeZ,scsizeX;

  //add background noise
  for(i=0;i<Dia;i++)
    for(j=0;j<Dia;j++)
      for(k=0;k<zdim;k++)
      {
         orig_vol->data[i][j][k]=orig_vol->data[i][j][k]+Backgrnd_Noise;
      }

  rot_vol = new biocts_Volume3D(xdim,ydim,zdim);

  float scdis,obdis;
  float vobcX,vobcY,vobcZ,orshiftX,orshiftY,orshiftZ,
        vecX,vecY,vecZ,posX,posY,posZ,sum=0,max_pixel_value=0;
  double theta,phi,r, rmin,rmax,rotation;
  int viewi;
  int x_vox,y_vox,z_vox;

  scdis=10300; //sreen distance from source
  obdis=10000; //Volume3D distance from source

  scsizeX=int(Dia*scdis/(obdis-Dia/2)+10); //warning fixed by Can
  scsizeZ=int(zdim*scdis/(obdis-Dia/2)+10);//warning fixed by Can


  //setup NView screens
  screens.resize(NViews);
  for (viewi=0;viewi<NViews;viewi++)
    screens[viewi].set_size(scsizeX, scsizeZ, 1);

  rmin=obdis-Dia/2;    //range of r to lie withinn Volume3D region 
  rmax=sqrt((obdis+Dia/2.0)*(obdis+Dia/2.0)+(Dia*Dia/4.0+zdim*zdim/4.0));    /*****/

  vobcX=0;
  vobcY=obdis;
  vobcZ=0;

  orshiftX=Dia/2;
  orshiftY=Dia/2;
  orshiftZ=zdim/2;

  for (viewi=0;viewi<NViews;viewi++)
  {
    rotation = viewi*PI/NViews;

    orig_vol->rotate_volume(rot_vol, rotation);

    //
    for (k=0;k<scsizeZ;k++)
    {
      theta=atan((k-scsizeZ/2)/scdis);

      for(i=0;i<scsizeX;i++)
      {
        sum=0;
        phi=atan((i-scsizeX/2)/scdis);
      
        for (r=rmin;r<=rmax;r++)
        { 
           vecX=r*cos(theta)*sin(phi);
           vecY=r*cos(theta)*cos(phi);
           vecZ=r*sin(theta);

           posX=vecX-vobcX;
           posY=vecY-vobcY;
           posZ=vecZ-vobcZ;

           x_vox=int(posX+orshiftX); //warning fixed by Can
           y_vox=int(posY+orshiftY); //warning fixed by Can
           z_vox=int(posZ+orshiftZ); //warning fixed by Can

             
           if((x_vox>0)&(x_vox<Dia))
             if((y_vox>0)&(y_vox<Dia))
              if((z_vox>0)&(z_vox<zdim))
                sum=sum + rot_vol->data[x_vox][y_vox][z_vox];
        }
        
        double pixel_val;

        if(max_pixel_value<sum)
          max_pixel_value=sum;

        if (lambda!=0)//ideal case
          pixel_val = gain*(sum*float(Poisson(lambda))/lambda);
        else
          pixel_val = gain*sum;
         
         if (pixel_val<255)
            screens[viewi](i,k,0) = int(pixel_val); //warning fixed by Can
          else
           screens[viewi](i, k,0) = 255;
     

      }
    }
  } //end of viewi loop

}

biocts_ctsim::~biocts_ctsim()
{
  //delete temp data
  delete rot_vol;
}
