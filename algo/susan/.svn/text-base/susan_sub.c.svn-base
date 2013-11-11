//#include "susan_sub.h"
#include "susan_sub_function.h"
#include "SusanInterface.h"
#define PI 3.1412

struct Susanpixels * susan_subpixel(in,bp,r,max_no,mid,x_size,y_size,a,b,w,do_sym,angle_edges,mask_size)
uchar *in,*mid,*bp,*do_sym;
int * a, *b,*w;
int  *r,  x_size, y_size,mask_size;
int max_no;
char * angle_edges;
{
  double angle;
  int i,j;
  int temp;
  float  *subx,*suby;
  struct Susanpixels * start_edges;
  struct Susanpixels * temp_edges,*stemp_edges;
  
  FILE * fp_out;
  FILE * fp_pix;
  subx= malloc (sizeof(float));
  suby= malloc (sizeof(float));
  
  /*fp_out=fopen("sub.txt","w");
  fp_pix=fopen("pix.txt","w");
  fprintf(fp_out,"width: %d\n", x_size);
  fprintf(fp_out,"height: %d\n", y_size);
  fprintf(fp_out,"number-of-points: %d\n",1345);
  fprintf(fp_pix,"width: %d\n", x_size);
  fprintf(fp_pix,"height: %d\n", y_size);
  fprintf(fp_pix,"number-of-points: %d\n",1345);*/

  start_edges=0;
  temp_edges=0;
  
  for (j=1;j<y_size-1;j++)
    {
      for (i=1;i<x_size-1;i++)
  {
    if (mid[j*x_size+i]<8 )
      {
        if(angle_edges[j*x_size+i]<10)
    {
      angle=atan2(b[j*x_size+i],(a[j*x_size+i]+0.0001));

      if(mask_size==2)
        susan_subpixel_37(in,bp,r,mid,x_size,y_size,j,i,angle,subx,suby,do_sym,angle_edges);
      else if(mask_size==1)
        susan_subpixel_25(in,bp,r,mid,x_size,y_size,j,i,angle,subx,suby,do_sym,angle_edges);
      else if(mask_size==0)
        susan_subpixel_9(in,bp,r,mid,x_size,y_size,j,i,angle,subx,suby,do_sym,angle_edges);
      
      temp_edges=(struct Susanpixels *)malloc(sizeof(struct Susanpixels ));
      temp_edges->xpos=*subx;
      temp_edges->ypos=*suby;
      temp_edges->direction=angle_edges[j*x_size+i];
      temp_edges->strength=(float)(r[j*x_size+i]);
      temp_edges->next=0;
      if(start_edges==0)
        {
          stemp_edges=temp_edges;
          start_edges=stemp_edges;
        }
      else
        {
          stemp_edges->next=temp_edges;
          stemp_edges=stemp_edges->next;
        }
    }

      }
  }
    }
  //  fclose(fp_out);
  //fclose(fp_pix);
  return start_edges;
}

void susan_subpixel1(r,mid,x_size,y_size,a,b,w)
uchar *mid;
int * a, *b,*w;
int  *r,  x_size, y_size;
{
  int x1,x2,x3;
  int y1,y2,y3;
  double B1,A1;
  double costheta,sintheta;
  double pos,xpos,ypos;
  int i,j;

  FILE * fp_out;
  fp_out=fopen("sub.txt","w");
  fprintf(fp_out,"width: %d\n", x_size);
  fprintf(fp_out,"height: %d\n", y_size);
  fprintf(fp_out,"number-of-points: %d\n",1345);
  x1=-1;
  x2=0;
  x3=1;
  printf("\n check");
  
  for (i=1;i<y_size-1;i++)
    {
      for (j=1;j<x_size-1;j++)
  {
    if (mid[i*x_size+j]<8)
      {
        y3=r[(i+a[i*x_size+j])*x_size+j+b[i*x_size+j]];
        y1=r[(i-a[i*x_size+j])*x_size+j-b[i*x_size+j]];
        y2=r[i*x_size+j];
        //
        if(y1!=y2 || y1!=y3)
    {
      A1=y1*0.5-y2+y3*0.5;
      //printf("\n %f",A1);
      //fflush(stdout);
      B1=y3*0.5-y1*0.5;
      //printf("\t %f",B1);
      pos=-B1/(2*A1+0.001);
      //printf("\t %f",pos);
      if(sqrt(a[i*x_size+j]*a[i*x_size+j]+b[i*x_size+j]*b[i*x_size+j])>0)
        {
        costheta=a[i*x_size+j]/(sqrt(a[i*x_size+j]*a[i*x_size+j]+b[i*x_size+j]*b[i*x_size+j]));
        sintheta=b[i*x_size+j]/(sqrt(a[i*x_size+j]*a[i*x_size+j]+b[i*x_size+j]*b[i*x_size+j]));
        
        xpos=i+sintheta*pos;
        ypos=j+costheta*pos;
        if(xpos < 0 || ypos <0)
          {
      
      printf("\t %d %d %d",x3,x1,x2);
      printf("\n A1 %f",A1);
      printf("\tB1 %f",B1);printf("\t %f",pos);
      printf("point-at: (%f %f)\n",xpos,ypos);
      xpos=i;
      ypos=j;
          }
        }
    }
        else
    {
      xpos=i;
      ypos=j;
    }
        
      }
    /*else
      {
      xpos=i;
      ypos=j;
      }*/
    //printf("point-at: (%f %f)\n",xpos,ypos);
    fprintf(fp_out,"point-at: (%f %f)\n",ypos,xpos);
  }
    }
  
  
  fclose(fp_out);
  
  }
