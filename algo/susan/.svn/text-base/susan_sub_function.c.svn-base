
#include "susan_sub_function.h"
#include "susan.h"
#define ZSGN(a)( ((a)<0)? -1 : (((a)>0)? 1 : 0)  )     
#define M_PI 3.1412


/*void susan_subpixel_37(in,bp,r,mid,x_size,y_size,j,i,angle,subx,suby,do_sym,angle_edges)
     unsigned char *in,*bp,*mid,*do_sym;
     int x_size,y_size,i,j;
     float angle;
     float *subx,*suby;
     char * angle_edges;
     int *r;*/
void susan_subpixel_37( uchar * in,uchar *bp,int *r,uchar *mid,int x_size,int y_size,int j,int i,float angle,float *subx,float *suby,uchar *do_sym,char * angle_edges)
{
  int verbose=0;
  float ctemp;
  unsigned char c,*p,*cp;
  int  max_no = 2650;
  float n=100;
  float slope1,slope2;
  double ratio;
  int setbit1center=0;
  int setbit2center=0;
  int setbitleft=0;
  int setbitright=0;
  float xpos,ypos;
  float dx,dy,dabsx,dabsy;
  float n_new_1,n_new_2,n_new_3,n_curr;
  float normal1;
  int angle1;
  int npix;
  int dir1,dir2,d;
  int k,l,m,w;
  int pos_normals[6];
  float sub_min;
  float min_xpos,min_ypos;
  float min=0;
  int temp;
  int x_intensity,y_intensity;
  int xpos_int,ypos_int;
  float dxpos,dypos;
  FILE * fp_out;
  float arrayleft[11];
  float arrayleftright[11];
  float array1center[11];
  float arrayright[11];
  float arrayrightleft[11];
  float array2center[11];
  float temp_array[11];
  int min_m;
  float n_org;
  float x_cen,y_cen;
  int mask[37];
  int x_mask[37]={-1,0,1,-2,-1,0,1,2,-3,-2,-1,0,1,2,3,-3,-2,-1,0,1,2,3,-3,-2,-1,0,1,2,3,-2,-1,0,1,2,-1,0,1};
  int y_mask[37]={-3,-3,-3,-2,-2,-2,-2,-2,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,2,2,2,2,2,3,3,3};
//  mask of 37 pixels
  
  mask[0]=-3*x_size-1;
  mask[1]=-3*x_size;
  mask[2]=-3*x_size+1;
  mask[3]=-2*x_size-2;
  mask[4]=-2*x_size-1;
  mask[5]=-2*x_size;
  mask[6]=-2*x_size+1;
  mask[7]=-2*x_size+2;
  mask[8]=-x_size-3;
  mask[9]=-x_size-2;
  mask[10]=-x_size-1;
  mask[11]=-x_size;
  mask[12]=-x_size+1;
  mask[13]=-x_size+2;
  mask[14]=-x_size+3;
  mask[15]=-3;
  mask[16]=-2;
  mask[17]=-1;
  mask[18]=0;
  mask[19]=1;
  mask[20]=2;
  mask[21]=3;
  mask[22]=x_size-3;
  mask[23]=x_size-2;
  mask[24]=x_size-1;
  mask[25]=x_size;
  mask[26]=x_size+1;
  mask[27]=x_size+2;
  mask[28]=x_size+3;
  mask[29]=2*x_size-2;
  mask[30]=2*x_size-1;
  mask[31]=2*x_size;
  mask[32]=2*x_size+1;
  mask[33]=2*x_size+2;
  mask[34]=3*x_size-1;
  mask[35]=3*x_size;
  mask[36]=3*x_size+1;
  

 

  cp=bp + in[j*x_size+i];
  //n_curr=*(cp-*p);
  n=0;
  
  for(l=0;l<=36;l++)
    {
      p=in+j*x_size+i+mask[l];
      n_curr=*(cp-*p);
      n+=n_curr;
    }
  
  n_org=n;
  if(verbose==1)
    printf("\n %f",n_org);
  angle1=angle_edges[j*x_size+i];
  angle=angle_edges[j*x_size+i]*M_PI/8;
  
  for(k=0;k>=-10;k--)
    {
      if(angle1==1 || angle1==7)
  {
    dx=ZSGN(sin(angle))*0.05*k;
    dy=ZSGN(cos(angle))*0.1*k;

  }
      else if(angle1==3 || angle1==5)
  {
    dx=ZSGN(sin(angle))*0.1*k;
    dy=ZSGN(cos(angle))*0.05*k;
  }
      else if(angle1==2 || angle==6)
  {
    dx=ZSGN(sin(angle))*0.1*k;
    dy=ZSGN(cos(angle))*0.1*k;
  }
      else
  {
    dx=(sin(angle))*0.1*k;
    dy=(cos(angle))*0.1*k;
  }
      ypos = j+dy;
      xpos = i+dx;
      
      normal1=angle-1.5705;

      if(angle1==7 || angle1==6 || angle1==5)
  {
    dir1=6;dir2=2;
    pos_normals[0]=i;
    pos_normals[1]=j;
    pos_normals[2]=i-1;
    pos_normals[3]=j+1;
    pos_normals[4]=i+1;
    pos_normals[5]=j-1;
    
  }
      if(angle1==4)
  {
    dir1=5;dir2=1;

    pos_normals[0]=i;
    pos_normals[1]=j;
    pos_normals[2]=i-1;
    pos_normals[3]=j;
    pos_normals[4]=i+1;
    pos_normals[5]=j;
  }
      if(angle1==1 || angle1==2 || angle1==3)//sin(normal1)>-0.8 && sin(normal1)<-0.6)
  {
    dir1=4;dir2=0;
    pos_normals[0]=i;
    pos_normals[1]=j;
    pos_normals[2]=i-1;
    pos_normals[3]=j-1;
    pos_normals[4]=i+1;
    pos_normals[5]=j+1;
  }
      if(angle1==0)
  {
    dir1=3;dir2=7;

    pos_normals[0]=(int)i;
    pos_normals[1]=(int)j;
    pos_normals[2]=i;
    pos_normals[3]=j-1;
    pos_normals[4]=i;
    pos_normals[5]=j+1;
  }

      
      dabsx=fabs(dx);
      dabsy=fabs(dy);
      
      xpos_int=(int)xpos;
      ypos_int=(int)ypos;
      
      dxpos=xpos-xpos_int;
      dypos=ypos-ypos_int;
      
      for(m=0;m<3;m++)
  {
    //printf("\n %d %d %d %d %d %d %d",i,j,pos_normals[1],pos_normals[0],pos_normals[2*m+1]*x_size+pos_normals[2*m],angle1,m);
    
    cp=bp + in[pos_normals[2*m+1]*x_size+pos_normals[2*m]];
    n_curr=*(cp-*p);
    n=0;

    x_cen=0;
    y_cen=0;
    ctemp=0;
    for(l=0;l<=36;l++)
      {
        p=in+ypos_int*x_size+xpos_int+mask[l];
        n_curr=*(cp-*p);
        n_new_2=*(cp-*(p+x_size+1));
        n_new_3=*(cp-*(p+x_size));
        n_new_1=*(cp-*(p+1));
        
        ctemp=n_new_2*dxpos*dypos+n_new_1*dxpos*(1-dypos)+n_new_3*dypos*(1-dxpos)+n_curr*(1-dxpos-dypos+dxpos*dypos);
        n+=ctemp;
        }
    if(m==0)
      array1center[abs(k)]=n;
    if(m==1)
      arrayleft[abs(k)]=n;
    if(m==2)
      arrayleftright[abs(k)]=n;

  }
      if(verbose==1)
  printf("\n %f %f %f %f %f %f %f",xpos,ypos,angle,n,array1center[abs(k)],arrayleft[abs(k)],arrayleftright[abs(k)]); 
    }
  
  
  //checking increasing or decreasing
  for(m=0;m<11;m++)
    {
      if(array1center[m]-array1center[0]<0)
  setbit1center=setbit1center-1;
      else if (array1center[m]-array1center[0]>0)
  setbit1center=setbit1center+1;
      
    }
  for(m=0;m<11;m++)
    {
      if(arrayleft[m]-arrayleft[0]<0)
  setbitleft=setbitleft-1;
      else if(arrayleft[m]-arrayleft[0]>0)
  setbitleft=setbitleft+1;
    }
  if(verbose==1)
    {
      if(setbit1center==-10)
  printf("\n center1 decreasing");
      else if(setbit1center==10)
  printf("\n center1 increasing");
      else
  printf("\n center 1 neither");
  
      if(setbitleft==-10)
  printf("\n left decreasing ");
      else if(setbitleft==10)
  printf("\n left increasing ");
      else
  printf("\n left neither");
    }
  for(k=0;k<=10;k++)
    {
      if(angle1==1 && angle1==7)
  {
    dx=ZSGN(sin(angle))*0.05*k;
    dy=ZSGN(cos(angle))*0.1*k;
  }
      else if(angle1==3 && angle1==5)
  {
    dx=ZSGN(sin(angle))*0.1*k;
    dy=ZSGN(cos(angle))*0.05*k;
  }
      else if(angle1==2 && angle1==6)
  {
    dx=ZSGN(sin(angle))*0.1*k;
    dy=ZSGN(cos(angle))*0.1*k;
  }
      else
  {
    dx=(sin(angle))*0.1*k;
    dy=(cos(angle))*0.1*k;
  }
      ypos = j+dy;
      xpos = i+dx;
      
      normal1=angle-1.5705;
      
      if(angle1==7 || angle1==6 || angle1==5)//sin(normal1)>0.65 && sin(normal1)<0.8)
  {
    dir1=6;dir2=2;
    
    pos_normals[0]=i;
    pos_normals[1]=j;
    pos_normals[2]=i+1;
    pos_normals[3]=j-1;
    pos_normals[4]=i-1;
    pos_normals[5]=j+1;
    
  }
      if(angle1==4)//sin(normal1)>-0.l && sin(normal1)<0.2)
  {
    dir1=5;dir2=1;

    pos_normals[0]=i;
    pos_normals[1]=j;
    pos_normals[2]=i+1;
    pos_normals[3]=j;
    pos_normals[4]=i-1;
    pos_normals[5]=j;
  }
      if(angle1==1 || angle1==2 || angle1==3)//sin(normal1)>-0.8 && sin(normal1)<-0.6)
  {
    dir1=4;dir2=0;

    pos_normals[0]=i;
    pos_normals[1]=j;
    pos_normals[2]=i+1;
    pos_normals[3]=j+1;
    pos_normals[4]=i-1;
    pos_normals[5]=j-1;
  }
      if(angle1==0)//sin(normal1)>-1.l && sin(normal1)<-0.9)
  {
    dir1=3;dir2=7;
    pos_normals[0]=i;
    pos_normals[1]=j;
    pos_normals[2]=i;
    pos_normals[3]=j+1;
    pos_normals[4]=i;
    pos_normals[5]=j-1;
  }
      dabsx=fabs(dx);
      dabsy=fabs(dy);
      
      xpos_int=(int)xpos;
      ypos_int=(int)ypos;
      
      dxpos=xpos-xpos_int;
      dypos=ypos-ypos_int;
      
      
      for(m=0;m<3;)
  {
      cp=bp + in[pos_normals[2*m+1]*x_size+pos_normals[2*m]];
      n_curr=*(cp-*p);
      n=0;
      for(l=0;l<=36;l++)
        {
    p=in+ypos_int*x_size+xpos_int+mask[l];
    n_curr=*(cp-*p);
    n_new_2=*(cp-*(p+x_size+1));
    n_new_3=*(cp-*(p+x_size));
    n_new_1=*(cp-*(p+1));
    
    n+=n_new_2*dxpos*dypos+n_new_1*dxpos*(1-dypos)+n_new_3*dypos*(1-dxpos)+n_curr*(1-dxpos-dypos+dxpos*dypos);
        }
    
      if(m==0)
        array2center[k]=n;
      if(m==1)
        arrayright[k]=n;
      if(m==2)
        arrayrightleft[k]=n;
      m++;
  }
      if(verbose==1)
  printf("\n %f %f %f %f %f %f %f",xpos,ypos,angle,n,array2center[k],arrayright[k],arrayrightleft[k]);     
    }
  
 for(m=0;m<11;m++)
    {
      if(array2center[m]-array2center[0]<0)
  setbit2center=setbit2center-1;
      else if(array2center[m]-array2center[0]>0)
  setbit2center=setbit2center+1;
    }
  for(m=0;m<11;m++)
    {
      if(arrayright[m]-arrayright[0]<0)
  setbitright=setbitright-1;
      else if(arrayright[m]-arrayright[0]>0)
  setbitright=setbitright+1;
    }
  if(verbose==1)
    {
      if(setbit2center==-10)
  printf("\n center2 decreasing");
      else   if(setbit2center==10)
  printf("\n center2 increasing");
      else
  printf("\n center2 neither");
      if(setbitright==-10)
  printf("\n right decreasing ");
      else  if(setbitright==10)
  printf("\n right increasing ");
      else
  printf("\n right niether ");
    }
  if(do_sym[j*x_size+i]==0)
    {
      // printf("\n I am normal point") ;
      if(setbit1center*setbitleft==-100 && setbit2center*setbitright==100 )
  {
    min=10000;
    for(m=0;m<11;m++)
      {
        temp_array[m]=fabs(array1center[m]-arrayleft[m]);
        if(temp_array[m]<min)
    {
      min=temp_array[m];
      min_m=m;
    }
      }
    //printf("\n %d",min_m);
    if(angle1==1 && angle1==7)
      {
        dx=ZSGN(sin(angle))*0.05*min_m;
        dy=ZSGN(cos(angle))*0.1*min_m;
      }
    else if(angle1==3 && angle1==5)
      {
        dx=ZSGN(sin(angle))*0.1*min_m;
        dy=ZSGN(cos(angle))*0.05*min_m;
      }
    else if(angle1==2 && angle1==6)
      {
        dx=ZSGN(sin(angle))*0.1*min_m;
        dy=ZSGN(cos(angle))*0.1*min_m;
      }
    else
      {
        dx=(sin(angle))*0.1*min_m;
        dy=(cos(angle))*0.1*min_m;
      }
    min_xpos=i-dx;
    min_ypos=j-dy;

  }
      
      
      else if (setbit2center*setbitright==-100 && setbit1center*setbitleft==100)
  {
     
    min=10000;
    for(m=0;m<11;m++)
      {
        temp_array[m]=fabs(array2center[m]-arrayright[m]);
        if(temp_array[m]<min)
    {
      min=temp_array[m];
      min_m=m;
      
    }
      }
    //printf("\n %d",min_m);      
    if(angle1==1 && angle1==7)
      {
        dx=ZSGN(sin(angle))*0.05*min_m;
        dy=ZSGN(cos(angle))*0.1*min_m;
      }
    else if(angle1==3 && angle1==5)
      {
        dx=ZSGN(sin(angle))*0.1*min_m;
        dy=ZSGN(cos(angle))*0.05*min_m;
      }
    else if(angle1==2 && angle1==6)
      {
        dx=ZSGN(sin(angle))*0.1*min_m;
        dy=ZSGN(cos(angle))*0.1*min_m;
      }
    else
      {
        dx=(sin(angle))*0.1*min_m;
        dy=(cos(angle))*0.1*min_m;
      }
    min_xpos=i+dx;
    min_ypos=j+dy;
    
  }
      else
  {
    min_xpos=i;
    min_ypos=j;
  }
    }
  else if(do_sym[j*x_size+i]==1)
    {
      //printf("\n I am the funny case");
      if(arrayleft[0]<arrayright[0])
  {
    min=10000;
    for(m=0;m<6;m++)
      {
        temp_array[m]=fabs(arrayleftright[m]-arrayleft[m]);
        if(temp_array[m]<min)
    {
      min=temp_array[m];
      min_m=m;
    }
      }
    //printf("\n %d",min_m);
    if(angle1==1 && angle1==7)
      {
        dx=ZSGN(sin(angle))*0.05*min_m;
        dy=ZSGN(cos(angle))*0.1*min_m;
      }
    else if(angle1==3 && angle1==5)
      {
        dx=ZSGN(sin(angle))*0.1*min_m;
        dy=ZSGN(cos(angle))*0.05*min_m;
      }
    else if(angle1==2 && angle1==6)
      {
        dx=ZSGN(sin(angle))*0.1*min_m;
        dy=ZSGN(cos(angle))*0.1*min_m;
      }
    else
      {
        dx=(sin(angle))*0.1*min_m;
        dy=(cos(angle))*0.1*min_m;
      }
    min_xpos=i-dx;
    min_ypos=j-dy;
    
    
  }
      else if(arrayleft[0]<arrayright[0])
  {
    min=10000;
    for(m=0;m<6;m++)
      {
        temp_array[m]=fabs(arrayrightleft[m]-arrayright[m]);
        if(temp_array[m]<min)
    {
      min=temp_array[m];
      min_m=m;
    }
      }
    //printf("\n %d",min_m);      
    if(angle1==1 && angle1==7)
      {
        dx=ZSGN(sin(angle))*0.05*min_m;
        dy=ZSGN(cos(angle))*0.1*min_m;
      }
    else if(angle1==3 && angle1==5)
      {
        dx=ZSGN(sin(angle))*0.1*min_m;
        dy=ZSGN(cos(angle))*0.05*min_m;
      }
    else if(angle1==2 && angle1==6)
      {
        dx=ZSGN(sin(angle))*0.1*min_m;
        dy=ZSGN(cos(angle))*0.1*min_m;
      }
    else
      {
        dx=(sin(angle))*0.1*min_m;
        dy=(cos(angle))*0.1*min_m;
      }
    min_xpos=i+dx;
    min_ypos=j+dy;
  }
  else
  {
    //printf("\n part2");
    min_xpos=i;
    min_ypos=j;
  }
    
    }
  
  //printf("\n %d %d %f %f",i,j,min_xpos,min_ypos);
  //  scanf("%d",&m);
  *subx=min_xpos;
  *suby=min_ypos;
}
void susan_subpixel_25( uchar * in,uchar *bp,int *r,uchar *mid,int x_size,int y_size,int j,int i,float angle,float *subx,float *suby,uchar *do_sym,char * angle_edges)
     /*void susan_subpixel_25(in,bp,r,mid,x_size,y_size,j,i,angle,subx,suby,do_sym,angle_edges)
    unsigned char *in,*bp,*mid,*do_sym;
     int x_size,y_size,i,j;
     float angle;
     float *subx,*suby;
     char * angle_edges;
     int *r;*/
{
  int verbose=0;
  float ctemp;
  unsigned char c,*p,*cp;
  int  max_no = 1775;
  float n=100;
  double ratio;
  int setbit1center=0;
  int setbit2center=0;
  int setbitleft=0;
  int setbitright=0;
  float xpos,ypos;
  float dx,dy,dabsx,dabsy;
  float n_new_1,n_new_2,n_new_3,n_curr;
  float normal1;
  int angle1;
  int npix;
  int dir1,dir2,d;
  int k,l,m,w;
  int pos_normals[6];
  float sub_min;
  float min_xpos,min_ypos;
  float min=0;
  int temp;
  int x_intensity,y_intensity;
  int xpos_int,ypos_int;
  float dxpos,dypos;
  FILE * fp_out;
  float arrayleft[11];
  float arrayleftright[11];
  float array1center[11];
  float arrayright[11];
  float arrayrightleft[11];
  float array2center[11];
  float temp_array[11];
  int min_m;
  float n_org;
  float x_cen,y_cen;
  int mask[25];
  int x_mask[25]={-2,-1,0,1,2,-2,-1,0,1,2,-2,-1,0,1,2,-2,-1,0,1,2,-2,-1,0,1,2};
  int y_mask[25]={-2,-2,-2,-2,-2,-1,-1,-1,-1,-1,0,0,0,0,0,1,1,1,1,1,2,2,2,2,2};

  cp=bp + in[j*x_size+i];
  n=0;
  for(l=0;l<=24;l++)
    {
      p=in+j*x_size+i+x_mask[l]+y_mask[l]*x_size;
      n_curr=*(cp-*p);
      n+=n_curr;
    }
  
  n_org=n;
  //printf("\n %f",n_org);
  angle1=angle_edges[j*x_size+i];
  angle=angle_edges[j*x_size+i]*M_PI/8;

  
  for(k=0;k>=-10;k--)
    {
      if(angle1==1 || angle1==7)
  {
    dx=ZSGN(sin(angle))*0.05*k;
    dy=ZSGN(cos(angle))*0.1*k;

  }
      else if(angle1==3 || angle1==5)
  {
    dx=ZSGN(sin(angle))*0.1*k;
    dy=ZSGN(cos(angle))*0.05*k;
  }
      else if(angle1==2 || angle==6)
  {
    dx=ZSGN(sin(angle))*0.1*k;
    dy=ZSGN(cos(angle))*0.1*k;
  }
      else
  {
    dx=(sin(angle))*0.1*k;
    dy=(cos(angle))*0.1*k;
  }
      ypos = j+dy;
      xpos = i+dx;
      
      normal1=angle-1.5705;

      if(angle1==7 || angle1==6 || angle1==5)
  {
    dir1=6;dir2=2;
    pos_normals[0]=i;
    pos_normals[1]=j;
    pos_normals[2]=i-1;
    pos_normals[3]=j+1;
    pos_normals[4]=i+1;
    pos_normals[5]=j-1;
    
  }
      if(angle1==4)
  {
    dir1=5;dir2=1;

    pos_normals[0]=i;
    pos_normals[1]=j;
    pos_normals[2]=i-1;
    pos_normals[3]=j;
    pos_normals[4]=i+1;
    pos_normals[5]=j;
  }
      if(angle1==1 || angle1==2 || angle1==3)
  {
    dir1=4;dir2=0;
    pos_normals[0]=i;
    pos_normals[1]=j;
    pos_normals[2]=i-1;
    pos_normals[3]=j-1;
    pos_normals[4]=i+1;
    pos_normals[5]=j+1;
  }
      if(angle1==0)
  {
    dir1=3;dir2=7;

    pos_normals[0]=(int)i;
    pos_normals[1]=(int)j;
    pos_normals[2]=i;
    pos_normals[3]=j-1;
    pos_normals[4]=i;
    pos_normals[5]=j+1;
  }

      
      dabsx=fabs(dx);
      dabsy=fabs(dy);
      
      xpos_int=(int)xpos;
      ypos_int=(int)ypos;
      
      dxpos=xpos-xpos_int;
      dypos=ypos-ypos_int;
      
      for(m=0;m<3;m++)
  {
    
    cp=bp + in[pos_normals[2*m+1]*x_size+pos_normals[2*m]];
    n_curr=*(cp-*p);
    n=0;

    x_cen=0;
    y_cen=0;
    ctemp=0;
    for(l=0;l<=24;l++)
      {
        p=in+ypos_int*x_size+xpos_int+x_mask[l]+y_mask[l]*x_size;
        n_curr=*(cp-*p);
        n_new_2=*(cp-*(p+x_size+1));
        n_new_3=*(cp-*(p+x_size));
        n_new_1=*(cp-*(p+1));
        
        ctemp=n_new_2*dxpos*dypos+n_new_1*dxpos*(1-dypos)+n_new_3*dypos*(1-dxpos)+n_curr*(1-dxpos-dypos+dxpos*dypos);
        n+=ctemp;
        }
    if(m==0)
      array1center[abs(k)]=n;
    if(m==1)
      arrayleft[abs(k)]=n;
    if(m==2)
      arrayleftright[abs(k)]=n;

  }
      if(verbose==1)      
  printf("\n %f %f %f %f %f %f %f",xpos,ypos,angle,n,array1center[abs(k)],arrayleft[abs(k)],arrayleftright[abs(k)]); 
    }
  
  //checking increasing or decreasing
  for(m=0;m<11;m++)
    {
      if(array1center[m]-array1center[0]<0)
  setbit1center=setbit1center-1;
      else if (array1center[m]-array1center[0]>0)
  setbit1center=setbit1center+1;
      
    }
  for(m=0;m<11;m++)
    {
      if(arrayleft[m]-arrayleft[0]<0)
  setbitleft=setbitleft-1;
      else if(arrayleft[m]-arrayleft[0]>0)
  setbitleft=setbitleft+1;
    }
  if(verbose==1)
    {
      if(setbit1center==-10)
  printf("\n center1 decreasing");
      else if(setbit1center==10)
  printf("\n center1 increasing");
      else
  printf("\n center 1 neither");
      
      if(setbitleft==-10)
  printf("\n left decreasing ");
      else if(setbitleft==10)
  printf("\n left increasing ");
      else
  printf("\n left neither");
    }
  for(k=0;k<=10;k++)
    {
      if(angle1==1 && angle1==7)
  {
    dx=ZSGN(sin(angle))*0.05*k;
    dy=ZSGN(cos(angle))*0.1*k;
  }
      else if(angle1==3 && angle1==5)
  {
    dx=ZSGN(sin(angle))*0.1*k;
    dy=ZSGN(cos(angle))*0.05*k;
  }
      else if(angle1==2 && angle1==6)
  {
    dx=ZSGN(sin(angle))*0.1*k;
    dy=ZSGN(cos(angle))*0.1*k;
  }
      else
  {
    dx=(sin(angle))*0.1*k;
    dy=(cos(angle))*0.1*k;
  }
      ypos = j+dy;
      xpos = i+dx;
      
      normal1=angle-1.5705;
      
      if(angle1==7 || angle1==6 || angle1==5)
  {
    dir1=6;dir2=2;
    
    pos_normals[0]=i;
    pos_normals[1]=j;
    pos_normals[2]=i+1;
    pos_normals[3]=j-1;
    pos_normals[4]=i-1;
    pos_normals[5]=j+1;
    
  }
      if(angle1==4)
  {
    dir1=5;dir2=1;

    pos_normals[0]=i;
    pos_normals[1]=j;
    pos_normals[2]=i+1;
    pos_normals[3]=j;
    pos_normals[4]=i-1;
    pos_normals[5]=j;
  }
      if(angle1==1 || angle1==2 || angle1==3)
  {
    dir1=4;dir2=0;

    pos_normals[0]=i;
    pos_normals[1]=j;
    pos_normals[2]=i+1;
    pos_normals[3]=j+1;
    pos_normals[4]=i-1;
    pos_normals[5]=j-1;
  }
      if(angle1==0)
  {
    dir1=3;dir2=7;
    pos_normals[0]=i;
    pos_normals[1]=j;
    pos_normals[2]=i;
    pos_normals[3]=j+1;
    pos_normals[4]=i;
    pos_normals[5]=j-1;
  }
      dabsx=fabs(dx);
      dabsy=fabs(dy);
      
      xpos_int=(int)xpos;
      ypos_int=(int)ypos;
      
      dxpos=xpos-xpos_int;
      dypos=ypos-ypos_int;
      
      
      for(m=0;m<3;)
  {
      cp=bp + in[pos_normals[2*m+1]*x_size+pos_normals[2*m]];
      n_curr=*(cp-*p);
      n=0;
      for(l=0;l<=24;l++)
        {
    p=in+ypos_int*x_size+xpos_int+x_mask[l]+y_mask[l]*x_size;
    n_curr=*(cp-*p);
    n_new_2=*(cp-*(p+x_size+1));
    n_new_3=*(cp-*(p+x_size));
    n_new_1=*(cp-*(p+1));
    
    n+=n_new_2*dxpos*dypos+n_new_1*dxpos*(1-dypos)+n_new_3*dypos*(1-dxpos)+n_curr*(1-dxpos-dypos+dxpos*dypos);
        }
    
      if(m==0)
        array2center[k]=n;
      if(m==1)
        arrayright[k]=n;
      if(m==2)
        arrayrightleft[k]=n;
      m++;
  }
      if(verbose==1)
  printf("\n %f %f %f %f %f %f %f",xpos,ypos,angle,n,array2center[k],arrayright[k],arrayrightleft[k]);     
    }
  
 for(m=0;m<11;m++)
    {
      if(array2center[m]-array2center[0]<0)
  setbit2center=setbit2center-1;
      else if(array2center[m]-array2center[0]>0)
  setbit2center=setbit2center+1;
    }
  for(m=0;m<11;m++)
    {
      if(arrayright[m]-arrayright[0]<0)
  setbitright=setbitright-1;
      else if(arrayright[m]-arrayright[0]>0)
  setbitright=setbitright+1;
    }
  if(verbose==1)
    {
      if(setbit2center==-10)
  printf("\n center2 decreasing");
      else   if(setbit2center==10)
  printf("\n center2 increasing");
      else
  printf("\n center2 neither");
      if(setbitright==-10)
  printf("\n right decreasing ");
      else  if(setbitright==10)
  printf("\n right increasing ");
      else
  printf("\n right niether ");
    }
  if(do_sym[j*x_size+i]==0)
    {
      //printf("\n I am normal point") ;
      if(setbit1center*setbitleft==-100 && setbit2center*setbitright==100 )
  {
    min=10000;
    for(m=0;m<11;m++)
      {
        temp_array[m]=fabs(array1center[m]-arrayleft[m]);
        if(temp_array[m]<min)
    {
      min=temp_array[m];
      min_m=m;
    }
      }
    //printf("\n %d",min_m);
    if(angle1==1 && angle1==7)
      {
        dx=ZSGN(sin(angle))*0.05*min_m;
        dy=ZSGN(cos(angle))*0.1*min_m;
      }
    else if(angle1==3 && angle1==5)
      {
        dx=ZSGN(sin(angle))*0.1*min_m;
        dy=ZSGN(cos(angle))*0.05*min_m;
      }
    else if(angle1==2 && angle1==6)
      {
        dx=ZSGN(sin(angle))*0.1*min_m;
        dy=ZSGN(cos(angle))*0.1*min_m;
      }
    else
      {
        dx=(sin(angle))*0.1*min_m;
        dy=(cos(angle))*0.1*min_m;
      }
    min_xpos=i-dx;
    min_ypos=j-dy;

  }
      
      
      else if (setbit2center*setbitright==-100 && setbit1center*setbitleft==100)
  {
     
    min=10000;
    for(m=0;m<11;m++)
      {
        temp_array[m]=fabs(array2center[m]-arrayright[m]);
        if(temp_array[m]<min)
    {
      min=temp_array[m];
      min_m=m;
      
    }
      }
    //printf("\n %d",min_m);      
    if(angle1==1 && angle1==7)
      {
        dx=ZSGN(sin(angle))*0.05*min_m;
        dy=ZSGN(cos(angle))*0.1*min_m;
      }
    else if(angle1==3 && angle1==5)
      {
        dx=ZSGN(sin(angle))*0.1*min_m;
        dy=ZSGN(cos(angle))*0.05*min_m;
      }
    else if(angle1==2 && angle1==6)
      {
        dx=ZSGN(sin(angle))*0.1*min_m;
        dy=ZSGN(cos(angle))*0.1*min_m;
      }
    else
      {
        dx=(sin(angle))*0.1*min_m;
        dy=(cos(angle))*0.1*min_m;
      }
    min_xpos=i+dx;
    min_ypos=j+dy;
    
  }
      else
  {
    min_xpos=i;
    min_ypos=j;
  }
    }
  else if(do_sym[j*x_size+i]==1)
    {
      //printf("\n I am the funny case");
      if(arrayleft[0]<arrayright[0])
  {
    min=10000;
    for(m=0;m<6;m++)
      {
        temp_array[m]=fabs(arrayleftright[m]-arrayleft[m]);
        if(temp_array[m]<min)
    {
      min=temp_array[m];
      min_m=m;
    }
      }
    //printf("\n %d",min_m);
    if(angle1==1 && angle1==7)
      {
        dx=ZSGN(sin(angle))*0.05*min_m;
        dy=ZSGN(cos(angle))*0.1*min_m;
      }
    else if(angle1==3 && angle1==5)
      {
        dx=ZSGN(sin(angle))*0.1*min_m;
        dy=ZSGN(cos(angle))*0.05*min_m;
      }
    else if(angle1==2 && angle1==6)
      {
        dx=ZSGN(sin(angle))*0.1*min_m;
        dy=ZSGN(cos(angle))*0.1*min_m;
      }
    else
      {
        dx=(sin(angle))*0.1*min_m;
        dy=(cos(angle))*0.1*min_m;
      }
    min_xpos=i-dx;
    min_ypos=j-dy;
    
    
  }
      else if(arrayleft[0]<arrayright[0])
  {
    min=10000;
    for(m=0;m<6;m++)
      {
        temp_array[m]=fabs(arrayrightleft[m]-arrayright[m]);
        if(temp_array[m]<min)
    {
      min=temp_array[m];
      min_m=m;
    }
      }
    //printf("\n %d",min_m);      
    if(angle1==1 && angle1==7)
      {
        dx=ZSGN(sin(angle))*0.05*min_m;
        dy=ZSGN(cos(angle))*0.1*min_m;
      }
    else if(angle1==3 && angle1==5)
      {
        dx=ZSGN(sin(angle))*0.1*min_m;
        dy=ZSGN(cos(angle))*0.05*min_m;
      }
    else if(angle1==2 && angle1==6)
      {
        dx=ZSGN(sin(angle))*0.1*min_m;
        dy=ZSGN(cos(angle))*0.1*min_m;
      }
    else
      {
        dx=(sin(angle))*0.1*min_m;
        dy=(cos(angle))*0.1*min_m;
      }
    min_xpos=i+dx;
    min_ypos=j+dy;
  }
  else
  {
    //printf("\n part2");
    min_xpos=i;
    min_ypos=j;
  }
    
    }
  
  //printf("\n %d %d %f %f",i,j,min_xpos,min_ypos);
  //scanf("%d",&m);
  *subx=min_xpos;
  *suby=min_ypos;
}
/*void susan_subpixel_9(in,bp,r,mid,x_size,y_size,j,i,angle,subx,suby,do_sym,angle_edges)
     uchar *in,*bp,*mid,*do_sym;
     int x_size,y_size,i,j;
     float angle;
     float *subx,*suby;
     char * angle_edges;
     int *r;*/
void susan_subpixel_9( uchar * in,uchar *bp,int *r,uchar *mid,int x_size,int y_size,int j,int i,float angle,float *subx,float *suby,uchar *do_sym,char * angle_edges)
{
  int verbose=0;
  float ctemp;
  unsigned char c,*p,*cp;
  int  max_no = 900;
  float n=100;
  float slope1,slope2;
  double ratio;
  int setbit1center=0;
  int setbit2center=0;
  int setbitleft=0;
  int setbitright=0;
  float xpos,ypos;
  float dx,dy,dabsx,dabsy;
  float n_new_1,n_new_2,n_new_3,n_curr;
  float normal1,angle1;
  int npix;
  int dir1,dir2,d;
  int k,l,m,w;
  int pos_normals[6];
  float sub_min;
  float min_xpos,min_ypos;
  float min=0;
  int temp;
  int x_intensity,y_intensity;
  int xpos_int,ypos_int;
  float dxpos,dypos;
  FILE * fp_out;
  float arrayleft[11];
  float arrayleftright[11];
  float array1center[11];
  float arrayright[11];
  float arrayrightleft[11];
  float array2center[11];
  float temp_array[11];
  int min_m;
  float n_org;
  float x_cen,y_cen;
  int mask[37];
  int x_mask[9]={-1,0,1,-1,0,1,-1,0,1};
  int y_mask[9]={-1,-1,-1,0,0,0,1,1,1};
//  mask of 37 pixels
  
  mask[0]=-3*x_size-1;
  mask[1]=-3*x_size;
  mask[2]=-3*x_size+1;
  mask[3]=-2*x_size-2;
  mask[4]=-2*x_size-1;
  mask[5]=-2*x_size;
  mask[6]=-2*x_size+1;
  mask[7]=-2*x_size+2;
  mask[8]=-x_size-3;
  mask[9]=-x_size-2;
  mask[10]=-x_size-1;
  mask[11]=-x_size;
  mask[12]=-x_size+1;
  mask[13]=-x_size+2;
  mask[14]=-x_size+3;
  mask[15]=-3;
  mask[16]=-2;
  mask[17]=-1;
  mask[18]=0;
  mask[19]=1;
  mask[20]=2;
  mask[21]=3;
  mask[22]=x_size-3;
  mask[23]=x_size-2;
  mask[24]=x_size-1;
  mask[25]=x_size;
  mask[26]=x_size+1;
  mask[27]=x_size+2;
  mask[28]=x_size+3;
  mask[29]=2*x_size-2;
  mask[30]=2*x_size-1;
  mask[31]=2*x_size;
  mask[32]=2*x_size+1;
  mask[33]=2*x_size+2;
  mask[34]=3*x_size-1;
  mask[35]=3*x_size;
  mask[36]=3*x_size+1;
  

 

  cp=bp + in[j*x_size+i];
  //n_curr=*(cp-*p);
  n=0;
  
  for(l=0;l<=8;l++)
    {
      p=in+j*x_size+i+x_mask[l]+y_mask[l]*x_size;
      n_curr=*(cp-*p);
      n+=n_curr;
    }
  
  n_org=n;
  printf("\n %f",n_org);
  angle1=angle_edges[j*x_size+i];
  angle=angle_edges[j*x_size+i]*M_PI/8;

  for(k=0;k>=-10;k--)
    {
      if(angle1==1 || angle1==7)
  {
    dx=ZSGN(sin(angle))*0.05*k;
    dy=ZSGN(cos(angle))*0.1*k;

  }
      else if(angle1==3 || angle1==5)
  {
    dx=ZSGN(sin(angle))*0.1*k;
    dy=ZSGN(cos(angle))*0.05*k;
  }
      else if(angle1==2 || angle==6)
  {
    dx=ZSGN(sin(angle))*0.1*k;
    dy=ZSGN(cos(angle))*0.1*k;
  }
      else
  {
    dx=(sin(angle))*0.1*k;
    dy=(cos(angle))*0.1*k;
  }
      ypos = j+dy;
      xpos = i+dx;
      
      normal1=angle-1.5705;
      if(angle1==7 || angle1==6 || angle1==5)
  {
    dir1=6;dir2=2;
    pos_normals[0]=i;
    pos_normals[1]=j;
    pos_normals[2]=i-1;
    pos_normals[3]=j+1;
    pos_normals[4]=i+1;
    pos_normals[5]=j-1;
    
  }
      if(sin(normal1)>-0.l && sin(normal1)<0.2)
  {
    dir1=5;dir2=1;

    pos_normals[0]=i;
    pos_normals[1]=j;
    pos_normals[2]=i-1;
    pos_normals[3]=j;
    pos_normals[4]=i+1;
    pos_normals[5]=j;
  }
      if(angle1==1 || angle1==2 || angle1==3)//sin(normal1)>-0.8 && sin(normal1)<-0.6)
  {
    dir1=4;dir2=0;
    pos_normals[0]=i;
    pos_normals[1]=j;
    pos_normals[2]=i-1;
    pos_normals[3]=j-1;
    pos_normals[4]=i+1;
    pos_normals[5]=j+1;
  }
      if(angle1==0)
  {
    dir1=3;dir2=7;

    pos_normals[0]=i;
    pos_normals[1]=j;
    pos_normals[2]=i;
    pos_normals[3]=j-1;
    pos_normals[4]=i;
    pos_normals[5]=j+1;
  }

      
      dabsx=fabs(dx);
      dabsy=fabs(dy);
      
      xpos_int=(int)xpos;
      ypos_int=(int)ypos;
      
      dxpos=xpos-xpos_int;
      dypos=ypos-ypos_int;
      
      for(m=0;m<3;m++)
  {
    
    cp=bp + in[(int)pos_normals[2*m+1]*x_size+(int)pos_normals[2*m]];
      
    n_curr=*(cp-*p);
    n=0;
    x_cen=0;
    y_cen=0;
    ctemp=0;
    for(l=0;l<=8;l++)
      {
        p=in+ypos_int*x_size+xpos_int+x_mask[l]+y_mask[l]*x_size;
        n_curr=*(cp-*p);
        n_new_2=*(cp-*(p+x_size+1));
        n_new_3=*(cp-*(p+x_size));
        n_new_1=*(cp-*(p+1));
        
        ctemp=n_new_2*dxpos*dypos+n_new_1*dxpos*(1-dypos)+n_new_3*dypos*(1-dxpos)+n_curr*(1-dxpos-dypos+dxpos*dypos);
        n+=ctemp;
        }
    if(m==0)
      array1center[abs(k)]=n;
    if(m==1)
      arrayleft[abs(k)]=n;
    if(m==2)
      arrayleftright[abs(k)]=n;
    
  }
      if(verbose==1)
  printf("\n %f %f %f %f %f %f %f",xpos,ypos,angle,n,array1center[abs(k)],arrayleft[abs(k)],arrayleftright[abs(k)]); 
    }
  
  
  //checking increasing or decreasing
  for(m=0;m<11;m++)
    {
      if(array1center[m]-array1center[0]<0)
  setbit1center=setbit1center-1;
      else if (array1center[m]-array1center[0]>0)
  setbit1center=setbit1center+1;
      
    }
  for(m=0;m<11;m++)
    {
      if(arrayleft[m]-arrayleft[0]<0)
  setbitleft=setbitleft-1;
      else if(arrayleft[m]-arrayleft[0]>0)
  setbitleft=setbitleft+1;
    }
  if(verbose==1)
    {
      if(setbit1center==-10)
  printf("\n center1 decreasing");
      else if(setbit1center==10)
  printf("\n center1 increasing");
      else
  printf("\n center 1 neither");
      
      if(setbitleft==-10)
  printf("\n left decreasing ");
      else if(setbitleft==10)
  printf("\n left increasing ");
      else
  printf("\n left neither");
    }
  for(k=0;k<=10;k++)
    {
      if(angle1==1 && angle1==7)
  {
    dx=ZSGN(sin(angle))*0.05*k;
    dy=ZSGN(cos(angle))*0.1*k;
  }
      else if(angle1==3 && angle1==5)
  {
    dx=ZSGN(sin(angle))*0.1*k;
    dy=ZSGN(cos(angle))*0.05*k;
  }
      else if(angle1==2 && angle1==6)
  {
    dx=ZSGN(sin(angle))*0.1*k;
    dy=ZSGN(cos(angle))*0.1*k;
  }
      else
  {
    dx=(sin(angle))*0.1*k;
    dy=(cos(angle))*0.1*k;
  }
      ypos = j+dy;
      xpos = i+dx;
      
      normal1=angle-1.5705;
      
      if(angle1==7 || angle1==6 || angle1==5)//sin(normal1)>0.65 && sin(normal1)<0.8)
  {
    dir1=6;dir2=2;
    
    pos_normals[0]=i;
    pos_normals[1]=j;
    pos_normals[2]=i+1;
    pos_normals[3]=j-1;
    pos_normals[4]=i-1;
    pos_normals[5]=j+1;
    
  }
      if(angle1==4)
  {
    dir1=5;dir2=1;

    pos_normals[0]=i;
    pos_normals[1]=j;
    pos_normals[2]=i+1;
    pos_normals[3]=j;
    pos_normals[4]=i-1;
    pos_normals[5]=j;
  }
      if(angle1==1 || angle1==2 || angle1==3)
  {
    dir1=4;dir2=0;

    pos_normals[0]=i;
    pos_normals[1]=j;
    pos_normals[2]=i+1;
    pos_normals[3]=j+1;
    pos_normals[4]=i-1;
    pos_normals[5]=j-1;
  }
      if(angle1==0)//sin(normal1)>-1.l && sin(normal1)<-0.9)
  {
    dir1=3;dir2=7;
    pos_normals[0]=i;
    pos_normals[1]=j;
    pos_normals[2]=i;
    pos_normals[3]=j+1;
    pos_normals[4]=i;
    pos_normals[5]=j-1;
  }
      dabsx=fabs(dx);
      dabsy=fabs(dy);
      
      xpos_int=(int)xpos;
      ypos_int=(int)ypos;
      
      dxpos=xpos-xpos_int;
      dypos=ypos-ypos_int;
      
      
      for(m=0;m<3;)
  {
      cp=bp + in[pos_normals[2*m+1]*x_size+pos_normals[2*m]];
      n_curr=*(cp-*p);
      n=0;
      for(l=0;l<=8;l++)
        {
    p=in+ypos_int*x_size+xpos_int+x_mask[l]+y_mask[l]*x_size;
    n_curr=*(cp-*p);
    n_new_2=*(cp-*(p+x_size+1));
    n_new_3=*(cp-*(p+x_size));
    n_new_1=*(cp-*(p+1));
    
    n+=n_new_2*dxpos*dypos+n_new_1*dxpos*(1-dypos)+n_new_3*dypos*(1-dxpos)+n_curr*(1-dxpos-dypos+dxpos*dypos);
        }
    
      if(m==0)
        array2center[k]=n;
      if(m==1)
        arrayright[k]=n;
      if(m==2)
        arrayrightleft[k]=n;
      m++;
  }
      if(verbose==1)
  printf("\n %f %f %f %f %f %f %f",xpos,ypos,angle,n,array2center[k],arrayright[k],arrayrightleft[k]);     
    }

 for(m=0;m<11;m++)
    {
      if(array2center[m]-array2center[0]<0)
  setbit2center=setbit2center-1;
      else if(array2center[m]-array2center[0]>0)
  setbit2center=setbit2center+1;
      
    }
  for(m=0;m<11;m++)
    {
      if(arrayright[m]-arrayright[0]<0)
  setbitright=setbitright-1;
      else if(arrayright[m]-arrayright[0]>0)
  setbitright=setbitright+1;
    }
  if(verbose==1)
    {
      if(setbit2center==-10)
  printf("\n center2 decreasing");
      else   if(setbit2center==10)
  printf("\n center2 increasing");
      else
  printf("\n center2 neither");
      if(setbitright==-10)
  printf("\n right decreasing ");
      else  if(setbitright==10)
  printf("\n right increasing ");
      else
  printf("\n right niether ");
    }
      if(do_sym[j*x_size+i]==0)
  {
    //printf("\n I am normal point") ;
      if(setbit1center*setbitleft==-100 && setbit2center*setbitright==100 )
  {
    min=10000;
    for(m=0;m<11;m++)
      {
        temp_array[m]=fabs(array1center[m]-arrayleft[m]);
        if(temp_array[m]<min)
    {
      min=temp_array[m];
      min_m=m;
    }
      }
    //printf("\n %d",min_m);
    if(angle1==1 && angle1==7)
      {
        dx=ZSGN(sin(angle))*0.05*min_m;
        dy=ZSGN(cos(angle))*0.1*min_m;
      }
    else if(angle1==3 && angle1==5)
      {
        dx=ZSGN(sin(angle))*0.1*min_m;
        dy=ZSGN(cos(angle))*0.05*min_m;
      }
    else if(angle1==2 && angle1==6)
      {
        dx=ZSGN(sin(angle))*0.1*min_m;
        dy=ZSGN(cos(angle))*0.1*min_m;
      }
    else
      {
        dx=(sin(angle))*0.1*min_m;
        dy=(cos(angle))*0.1*min_m;
      }
    min_xpos=i-dx;
    min_ypos=j-dy;

  }
      
      
      else if (setbit2center*setbitright==-100 && setbit1center*setbitleft==100)
  {
     
    min=10000;
    for(m=0;m<11;m++)
      {
        temp_array[m]=fabs(array2center[m]-arrayright[m]);
        if(temp_array[m]<min)
    {
      min=temp_array[m];
      min_m=m;
      
    }
      }
    //printf("\n %d",min_m);      
    if(angle1==1 && angle1==7)
      {
        dx=ZSGN(sin(angle))*0.05*min_m;
        dy=ZSGN(cos(angle))*0.1*min_m;
      }
    else if(angle1==3 && angle1==5)
      {
        dx=ZSGN(sin(angle))*0.1*min_m;
        dy=ZSGN(cos(angle))*0.05*min_m;
      }
    else if(angle1==2 && angle1==6)
      {
        dx=ZSGN(sin(angle))*0.1*min_m;
        dy=ZSGN(cos(angle))*0.1*min_m;
      }
    else
      {
        dx=(sin(angle))*0.1*min_m;
        dy=(cos(angle))*0.1*min_m;
      }
    min_xpos=i+dx;
    min_ypos=j+dy;
   

  }
      else
  {
    //printf("\n don`t know wat to do ");
  }
    }
  else if(do_sym[j*x_size+i]==1)
    {
      //printf("\n I am the funny case");
      
      if(arrayleft[0]<arrayright[0])
  {
    min=10000;
    for(m=0;m<6;m++)
      {
        temp_array[m]=fabs(arrayleftright[m]-arrayleft[m]);
        if(temp_array[m]<min)
    {
      min=temp_array[m];
      min_m=m;
    }
      }
    //printf("\n %d",min_m);
    if(angle1==1 && angle1==7)
      {
        dx=ZSGN(sin(angle))*0.05*min_m;
        dy=ZSGN(cos(angle))*0.1*min_m;
      }
    else if(angle1==3 && angle1==5)
      {
        dx=ZSGN(sin(angle))*0.1*min_m;
        dy=ZSGN(cos(angle))*0.05*min_m;
      }
    else if(angle1==2 && angle1==6)
      {
        dx=ZSGN(sin(angle))*0.1*min_m;
        dy=ZSGN(cos(angle))*0.1*min_m;
      }
    else
      {
        dx=(sin(angle))*0.1*min_m;
        dy=(cos(angle))*0.1*min_m;
      }
    min_xpos=i-dx;
    min_ypos=j-dy;
  }
      else if(arrayleft[0]>arrayright[0])
  {
    min=10000;
    for(m=0;m<6;m++)
      {
        temp_array[m]=fabs(arrayrightleft[m]-arrayright[m]);
        if(temp_array[m]<min)
    {
      min=temp_array[m];
      min_m=m;
    }
      }
    //printf("\n %d",min_m);      
    if(angle1==1 && angle1==7)
      {
        dx=ZSGN(sin(angle))*0.05*min_m;
        dy=ZSGN(cos(angle))*0.1*min_m;
      }
    else if(angle1==3 && angle1==5)
      {
        dx=ZSGN(sin(angle))*0.1*min_m;
        dy=ZSGN(cos(angle))*0.05*min_m;
      }
    else if(angle1==2 && angle1==6)
      {
        dx=ZSGN(sin(angle))*0.1*min_m;
        dy=ZSGN(cos(angle))*0.1*min_m;
      }
    else
      {
        dx=(sin(angle))*0.1*min_m;
        dy=(cos(angle))*0.1*min_m;
      }
    min_xpos=i+dx;
    min_ypos=j+dy;
  }
      else
  {
    
    min_xpos=i;
    min_ypos=j;
  }
    
    }
      if(verbose==1)
  printf("\n %d %d %f %f",i,j,min_xpos,min_ypos);
      *subx=min_xpos;
      *suby=min_ypos;
}
