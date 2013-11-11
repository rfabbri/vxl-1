#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "dbseg_jseg_xv.h"
#include "dbseg_jseg_ioutil.h"
#include "dbseg_jseg_mathutil.h"
#include "dbseg_jseg_imgutil.h"

extern int conv24;

void outputEdge(char *fname,char *exten,unsigned char *RGB0,unsigned char *rmap,
    int ny,int nx,int status,int type,int dim,float displayintensity)
{
  int iy,ix,i,j,datasize,l1,l2,mapsize;
  char outfname[200];
  unsigned char *RGB;

  mapsize = ny*nx;
  datasize = ny*nx*dim;
  RGB = (unsigned char *) malloc(datasize*sizeof(unsigned char));
  for (i=0;i<datasize;i++) RGB[i] = RGB0[i]*displayintensity;

  l1 = 0;
  for (iy=0;iy<ny;iy++)
  {
    for (ix=0;ix<nx-1;ix++)
    {
      l2 = l1+1;
      if (rmap[l1]!=rmap[l2])
      {
        for (j=0;j<dim;j++) { RGB[dim*l1+j]=255; RGB[dim*l2+j]=255; }
      }
      l1++;
    }
    l1++;
  }
  l1 = 0;
  for (iy=0;iy<ny-1;iy++)
  {
    for (ix=0;ix<nx;ix++)
    {
      l2 = l1+nx;
      if (rmap[l1]!=rmap[l2])
      {
        for (j=0;j<dim;j++) { RGB[dim*l1+j]=255; RGB[dim*l2+j]=255; }
      }
      l1++;
    }
  }

  for (i=0;i<mapsize;i++)
  {
    if (rmap[i]==0)
    {
      for (j=0;j<dim;j++) RGB[dim*i+j]=0;
    }
  }

  if (status==-1) sprintf(outfname,"%s",fname);
  else sprintf(outfname,"%s.%d.seg.%s",fname,status,exten);

  outputresult(type,outfname,RGB,ny,nx,dim);

  free(RGB);
}

void outputresult(int media_type,char *outfname,unsigned char *RGB,int ny,int nx,
    int dim)
{
  /*switch (media_type)
  {
    case I_YUV:
      outputimgyuv(outfname,RGB,ny,nx);
      break;
    case I_RGB:
      outputimgraw(outfname,RGB,ny,nx,dim);
      break;
    case I_GRAY:
      outputimgraw(outfname,RGB,ny,nx,dim);
      break;
    case I_PPM:
      outputimgpm(outfname,RGB,ny,nx,dim);
      break;
    case I_PGM:
      outputimgpm(outfname,RGB,ny,nx,dim);
      break;
    case I_JPG:
      outputimgjpg(outfname,RGB,ny,nx,dim);
      break;
    case I_GIF:
      outputimggif(outfname,RGB,ny,nx,dim);
      break;
    default:
      printf("Unknown media type \n");
      exit (-1);
  }*/
}

int inputimggif(char *infname,unsigned char **RGB,int *ny,int *nx)
{
  PICINFO pinfo;
  int imagesize,i,dim,j,k;

  if (LoadGIF(infname,&pinfo,&imagesize)!=1)
  {
    printf("can not open %s\n",infname);
    exit (0);
  }
  *ny = pinfo.h;
  *nx = pinfo.w;
  imagesize = (*ny)*(*nx);

  for (i=0;i<256;i++)
  {
    if (pinfo.r[i]!=pinfo.g[i] || pinfo.g[i]!=pinfo.b[i] || pinfo.b[i]!=pinfo.r[i])
      break;
  }

  if (i<256) 
  {
    for (j=0;j<imagesize;j++) { if (pinfo.pic[j]>=i) break; }
    if (j<imagesize) dim = 3;
    else dim = 1;
  }
  else dim = 1;

  *RGB = (unsigned char *)calloc(imagesize*dim,sizeof(unsigned char));

  k=0;
  imagesize = (*ny)*(*nx);
  for (i=0; i<imagesize; i++)
  {
    j = pinfo.pic[i];
    (*RGB)[k] = (unsigned char) pinfo.r[j]; k++;
    if (dim==3)
    {
      (*RGB)[k] = (unsigned char) pinfo.g[j]; k++;
      (*RGB)[k] = (unsigned char) pinfo.b[j]; k++;
    }
  }
  free(pinfo.pic);
  if (pinfo.comment!=NULL) free(pinfo.comment);
  return dim;
}

void outputimggif(char *outfname,unsigned char *RGB,int ny,int nx,int dim)
{
  int i,j,imagesize;
  FILE *fp;
  int ptype,numcols,colorstyle;
  char comment[200];
  byte *pic,*rmap,*gmap,*bmap;

  imagesize = ny*nx;
  pic = RGB; 

  if (dim==3)
  {
    conv24=CONV24_BEST;
    numcols=256;
    colorstyle=0;
    ptype = PIC24;
  }
  else if (dim==1)
  {
    rmap=(byte *)calloc(256,sizeof(byte));
    gmap=(byte *)calloc(256,sizeof(byte));
    bmap=(byte *)calloc(256,sizeof(byte));

    numcols=0;
    for (i=0;i<imagesize;i++)
    {
      for (j=0;j<numcols;j++) { if (pic[i]==rmap[j]) break; }
      if (j==numcols)
      {
        rmap[j]=pic[i]; gmap[j]=pic[i]; bmap[j]=pic[i]; numcols++;
      }
      pic[i]=j;
    }
    colorstyle=1;
    ptype = PIC8;
  }
  fp = fopen(outfname,"wb");
  WriteGIF(fp,pic,ptype,nx,ny,rmap,gmap,bmap,numcols,colorstyle,comment);
  fclose(fp);

  if (dim==1) { free(rmap); free(gmap); free(bmap); }
}

void inputimgraw(char *fname,unsigned char *img,int ny,int nx,int dim)
{
  FILE *fimg;

  fimg=fopen(fname,"rb");
  if (!fimg)
  {
    printf("unable to read %s\n",fname);
    exit(-1);
  }
  fread(img, sizeof(unsigned char), ny*nx*dim, fimg);
  fclose (fimg);
}

void outputimgraw(char *fname,unsigned char *img,int ny,int nx,int dim)
{
  FILE *fimg;

  fimg=fopen(fname,"wb");
  fwrite(img, sizeof(unsigned char), ny*nx*dim, fimg);
  fclose(fimg);
}

void inraw2float (char *fname,float *img,int ny,int nx,int dim)
{
  FILE *fimg;
  int iy,ix,i,j,imagesize;
  unsigned char *tmp;

  fimg=fopen(fname,"rb");
  if (!fimg)
  {
    printf("unable to read %s\n",fname);
    exit(-1);
  }
  imagesize = ny*nx*dim;
  tmp=(unsigned char *)malloc(imagesize*sizeof(unsigned char));
  fread(tmp, sizeof(unsigned char), imagesize, fimg);
  fclose (fimg);
  for (i=0;i<imagesize;i++) img[i] = (float) tmp[i];
  free(tmp);
}

void outfloat2raw(char *fname,float *img,int ny,int nx,int dim)
{
  FILE *fimg;
  int i,j,imagesize;
  unsigned char *tmp;

  imagesize = ny*nx*dim;
  tmp=(unsigned char *)malloc(imagesize*sizeof(unsigned char));
  for (i=0;i<imagesize;i++)
  {
    if (img[i]>255) img[i]=255;
    if (img[i]<0)   img[i]=0;
    tmp[i]=(unsigned char) round(img[i]);
  }
  fimg=fopen(fname,"wb");
  fwrite(tmp, sizeof(unsigned char), imagesize, fimg);
  fclose(fimg);
  free(tmp);
}

void inputimgyuv(char *fname,unsigned char *img,int ny,int nx)
{
  FILE *fimg;
  int iy,ix,i,j,imagesize,loc;
  unsigned char *tmp;

  imagesize = ny*nx;
  tmp=(unsigned char *)malloc(imagesize*sizeof(unsigned char));
  fimg=fopen(fname,"rb");
  if (!fimg)
  {
    printf("unable to read %s\n",fname);
    exit(-1);
  }

  fread(tmp, sizeof(unsigned char), imagesize, fimg);
  for (i=0,j=0;i<imagesize;i++,j+=3) img[j] = tmp[i];

  fread(tmp, sizeof(unsigned char), imagesize/4, fimg);
  i = 0;
  for (iy=0;iy<ny;iy+=2)
  {
    for (ix=0;ix<nx;ix+=2)
    {
      loc = LOC(iy,ix,1,nx,3); 
      img[loc] = tmp[i]; img[loc+3] = tmp[i];
      loc += 3*nx;
      img[loc] = tmp[i]; img[loc+3] = tmp[i];
      i++;
    }
  }

  fread(tmp, sizeof(unsigned char), imagesize/4, fimg);
  i=0;
  for (iy=0;iy<ny;iy+=2)
  {
    for (ix=0;ix<nx;ix+=2)
    {
      loc = LOC(iy,ix,2,nx,3); 
      img[loc] = tmp[i]; img[loc+3] = tmp[i];
      loc += 3*nx;
      img[loc] = tmp[i]; img[loc+3] = tmp[i];
      i++;
    }
  }

  free(tmp);
  fclose (fimg);
  yuv2rgb(img,ny*nx*3);
}

void outputimgyuv(char *fname,unsigned char *img,int ny,int nx)
{
  FILE *fimg;
  int iy,ix,i,j,imagesize,loc1,loc2,loc3,loc0;
  unsigned char *tmp;

  rgb2yuv(img,ny*nx*3);

  imagesize = ny*nx;
  tmp=(unsigned char *)malloc(imagesize*sizeof(unsigned char));
  fimg=fopen(fname,"wb");

  for (i=0,j=0;i<imagesize;i++,j+=3) tmp[i] = img[j];
  fwrite(tmp, sizeof(unsigned char), imagesize, fimg);

  i=0;
  for (iy=0;iy<ny;iy+=2)
  {
    for (ix=0;ix<nx;ix+=2)
    {
      loc0 = LOC(iy,ix,1,nx,3);
      loc1 = loc0+3;
      loc2 = loc0+3*nx;
      loc3 = loc2+3;
      tmp[i] = (unsigned char) round(( ((float) img[loc0]) + 
          img[loc1]+img[loc2]+img[loc3]) /4);
      i++;
    }
  }
  fwrite(tmp, sizeof(unsigned char), imagesize/4, fimg);

  i=0;
  for (iy=0;iy<ny;iy+=2)
  {
    for (ix=0;ix<nx;ix+=2)
    {
      loc0 = LOC(iy,ix,2,nx,3);
      loc1 = loc0+3;
      loc2 = loc0+3*nx;
      loc3 = loc2+3;
      tmp[i] = (unsigned char) round(( ((float) img[loc0]) + 
          img[loc1]+img[loc2]+img[loc3]) /4);
      i++;
    }
  }
  fwrite(tmp, sizeof(unsigned char), imagesize/4, fimg);

  free(tmp);
  fclose (fimg);
}

void inputimgpm(char *fname,unsigned char **img,int *ny,int *nx)
{
  FILE *fimg;
  int i,c,bufsize=1000,imagesize;
  char buf[1000];

  fimg=fopen(fname,"rb");
  if (!fimg)
  {
    printf("unable to read %s\n",fname);
    exit(-1);
  }

/* get the header, lines of discription */
  fgets(buf,bufsize,fimg);
  if (buf[0] == 'P')
  {
    if (buf[1] == '5') imagesize = 1;
    else if (buf[1] == '6') imagesize = 3;
    else 
    {
      printf("input image %s, unknown type\n",fname);
      exit (-1);
    }
  }
  else
  {
    printf("input image %s, unknown type\n",fname);
    exit (-1);
  }

  c=fgetc(fimg);
  while (c=='#')
  {
    fgets(buf,bufsize,fimg); /* remarks */
    c=fgetc(fimg);
  }
  fseek(fimg,-1,SEEK_CUR);
  fscanf(fimg,"%d %d\n",nx,ny);
  fgets(buf,bufsize,fimg); /* color map info */

  imagesize = imagesize * (*ny) * (*nx);
  *img = (unsigned char *) malloc(imagesize*sizeof(unsigned char));
  fread(*img, sizeof(unsigned char), imagesize, fimg);
  fclose (fimg);
}

void outputimgpm(char *fname,unsigned char *img,int ny,int nx,int dim)
{
  FILE *fimg;

  fimg=fopen(fname,"wb");
  if (dim==3)
  {
    fprintf(fimg,"P6\n");
    fprintf(fimg,"%d %d\n",nx,ny);
    fprintf(fimg,"255\n");
  }
  else if (dim==1)
  {
    fprintf(fimg,"P5\n");
    fprintf(fimg,"%d %d\n",nx,ny);
    fprintf(fimg,"255\n");
  }
  else 
  {
    printf("output image, unknown type\n");
    exit (-1);
  }
  fwrite(img, sizeof(unsigned char), ny*nx*dim, fimg);
  fclose(fimg);
}



#ifdef __cplusplus
}
#endif







