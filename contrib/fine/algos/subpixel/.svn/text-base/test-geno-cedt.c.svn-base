#include <postscript.h>

#include "subpixel_bubbles.h"

void draw_tracer2(Tracer *Tr, int sx, int sy, int color, int lines,double radius,
          int sx1,int ex,int sy1,int ey )
{
  int pos,ii,jj=0,kk=0;
  void *data;

  pos=0;
  while(Tr->vert[pos] != ENDOFLIST  && Tr->horiz[pos] != ENDOFLIST ){
    kk=kk+jj;jj=0;
    /* printf("New Contour\n"); */
    while(Tr->vert[pos] != ENDOFCONTOUR  && Tr->horiz[pos] != ENDOFCONTOUR ){
      if (Tr->horiz[pos] >= sx1 && Tr->horiz[pos] <= ex &&
      Tr->vert[pos] >= sy1 && Tr->vert[pos] <= ey){
    circle_draw_ps(Tr->horiz[pos]-sx,Tr->vert[pos]-sy,radius,color,data);
      }
      pos++;jj++;
    }
   /*  printf("pos=%d kk=%d jj=%d\n",pos,kk,jj); */
    if (lines && jj>1){
      /*point_draw_ps((double)Tr->horiz[kk]-sx,(double)Tr->vert[kk]-sy,
    BLUE,data);*/
      for(ii=kk+1;ii<kk+jj;ii++){
    if (Tr->horiz[ii] >= sx1 && Tr->horiz[ii] <= ex &&
        Tr->vert[ii] >= sy1 && Tr->vert[ii] <= ey &&
        Tr->horiz[ii-1] >= sx1 && Tr->horiz[ii-1] <= ex &&
        Tr->vert[ii-1] >= sy1 && Tr->vert[ii-1] <= ey){
    line_draw_ps((double)Tr->horiz[ii]-sx,(double)Tr->vert[ii]-sy,
             (double)Tr->horiz[ii-1]-sx,(double)Tr->vert[ii-1]-sy,
             color+1,data);
    /*printf("P1=(%f,%f) P2=(%f,%f)\n",Tr->horiz[ii],Tr->vert[ii],
           Tr->horiz[ii-1],Tr->vert[ii-1]);*/
    }
      }
      if (pow(Tr->horiz[kk]-Tr->horiz[kk+jj-1],2.0)+
      pow(Tr->vert[kk]-Tr->vert[kk+jj-1],2.0) < 2.0){
    if (Tr->horiz[kk] >= sx1 && Tr->horiz[kk] <= ex &&
        Tr->vert[kk] >= sy1 && Tr->vert[kk] <= ey &&
        Tr->horiz[kk+jj-1] >= sx1 && Tr->horiz[kk+jj-1] <= ex &&
        Tr->vert[kk+jj-1] >= sy1 && Tr->vert[kk+jj-1] <= ey){
      line_draw_ps((double)Tr->horiz[kk]-sx,(double)Tr->vert[kk]-sy,
               (double)Tr->horiz[kk+jj-1]-sx,(double)Tr->vert[kk+jj-1]-sy,
               color+1,data);
    }
      }
      /*point_draw_ps((double)Tr->horiz[kk+jj-1]-sx,(double)Tr->vert[kk+jj-1]-sy,
    LIGHT_BLUE,data);*/
    }
    if (lines && jj == 1)
      for(ii=kk;ii<kk+jj;ii++){
    if (Tr->horiz[ii] >= sx1 && Tr->horiz[ii] <= ex &&
        Tr->vert[ii] >= sy1 && Tr->vert[ii] <= ey){
      point_draw_ps((double)Tr->horiz[ii]-sx,(double)Tr->vert[ii]-sy,
            PURPLE,data);
    /* printf("P=(%f,%f)\n",Tr->horiz[ii],Tr->vert[ii]); */
    }
      }
    
    pos++;jj++;
  }
}



void do_cedt_tracer_file(int argc, char *argv[])
{
  
  int verbose=1;
  int pos,h,w;
  int i,heap_size;
  int sx,sy,ex,ey,ox,oy;
  unsigned char *seed;
  char in_file[200],tr_file[200],out_tr_file[200];
  void *data;
  FILE *fp;
  Tracer Tr,Tr1;
  SPCedt cedt;
  CEDTWaves waves;
  Heap heap;
 

  /*********************************************************************
   Reading input data and initializing data structures.
  *********************************************************************/
  if (argc != 6){
    printf("Usage: test-cedt in_file tr_file out_tr_file h w\n");
    exit(1);
  }
  i=1;
  strcpy(in_file,argv[i]);i++;
  strcpy(tr_file,argv[i]);i++;
  strcpy(out_tr_file,argv[i]);i++;
  h=atoi(argv[i]);i++;
  w=atoi(argv[i]);i++;
  printf("H,W=(%d,%d)\n",h, w);


  /*Initializing data structures and memory*/
  allocate_data_SP_Cedt(&cedt, h, w);
  cedt.boundary_end = 0;
  cedt.boundary_start = 0;
  allocate_cedt_boundary(&cedt.boundary,MAX_BOUNDARY_LENGTH);
  
  heap_size = h*w+5.0*MAX_BOUNDARY_LENGTH;
  alloc_mem_for_heap_SPCedt(&heap,heap_size);
  initialize_heap_SPCedt(&heap, heap_size);

  cedt.ray=(Ray *)calloc(5.0*MAX_BOUNDARY_LENGTH,sizeof(Ray));
  waves.qwaves=(QWaves *)calloc(h*w,sizeof(QWaves)); 
  waves.qorder=(QuenchOrder *)calloc(h*w,sizeof(QuenchOrder));
  waves.inter_boundary = 1;
  for (i=0;i<h*w;i++)
    waves.qwaves[i].quench = NULL;
  seed = (unsigned char*)malloc(h*w*sizeof(unsigned char));

  /* Read the Tracer data*/
  printf("I am in tracer from file option\n");
  getTracerFromFile2(tr_file, h,w, &Tr);
  
  if (verbose==1){
    pos=0;
    while(Tr.vert[pos] != ENDOFLIST  && Tr.horiz[pos] != ENDOFLIST ){
      printf("New Contour\n");
      while(Tr.vert[pos] != ENDOFCONTOUR  && Tr.horiz[pos] != ENDOFCONTOUR ){
    printf("%f,%f\n",Tr.horiz[pos],Tr.vert[pos]);
    pos++;
      } 
      pos++;
    }
  }

  
  /* Read the Image data*/
  fp=fopen(in_file,"r");
  fread(seed,sizeof(int),h*w,fp);
  fclose(fp);

  printf("Done reading data\n");
  fflush(stdout);
  /*read_raw_tracer_data_file(tr_file,&Tr,ev.h,ev.w);*/


  cedt.boundary_end=cedt_boundary_with_geno(&Tr, cedt.boundary, h,w);

  if (verbose){
    /*init_draw_ps_original("init-boundary.ps",5,h,w);
    draw_gridlines(0,0,h,w, YELLOW, data);
    draw_cedt_boundary_segment_zoom(cedt.boundary,0,0,RED);
    close_draw_ps();*/
  }

  free_data_QWaves_list(waves.qwaves,h*w);
  initial_wave_prop_cedt_strucs(&heap,&cedt,&waves,h,w);
  wave_propagation_via_cedt(&heap, &cedt, &waves, h, w);
  make_signed_distance_surface(cedt.surface,seed,h,w,1);
  if (verbose){
    printf("Done with cedt\n");
    fflush(stdout);
    /*fp=fopen("cedt-dist.raw","w");
    fwrite(cedt.surface,sizeof(double),(h)*(w),fp);
    fclose(fp);*/
    /* exit(1); */
  }
  trace_eno_zero_xings(cedt.surface, &Tr1, h,  w);  
  writeTracerToFile2(out_tr_file, h, w, &Tr1);

  sx=0;
  ex=w;
  sy=0;
  ey=h;
  ox=-0.0;
  oy=-0.0;
  init_draw_ps_original("Xings.ps",10,ey-sy,ex-sx);
  draw_gridlines_skip(-ox,-oy,2,2,ex-sx,ey-sy,YELLOW,data); 
  draw_tracer2(&Tr,sx+ox,sy+oy,RED,1,0.15,sx,ex,sy,ey); 
  draw_tracer2(&Tr1,sx+ox,sy+oy,LIGHT_BLUE,1,0.15,sx,ex,sy,ey); 
  close_draw_ps();
}


void do_surface_tracer(int argc, char *argv[])
{
  
  int verbose=1;
  int pos,h,w;
  int i;
  int sx,sy,ex,ey,ox,oy;
  int tr_flag=1;
  double *surf;
  char in_file[200],tr_file[200],out_tr_file[200];
  void *data;
  FILE *fp;
  Tracer Tr,Tr1;
 

  /*********************************************************************
   Reading input data and initializing data structures.
  *********************************************************************/
  if (argc != 6){
    printf("Usage: test-cedt in_file out_tr_file h w\n");
    exit(1);
  }
  i=1;
  strcpy(in_file,argv[i]);i++;
  strcpy(tr_file,argv[i]);i++;
  strcpy(out_tr_file,argv[i]);i++;
  h=atoi(argv[i]);i++;
  w=atoi(argv[i]);i++;
  printf("H,W=(%d,%d)\n",h, w);


  /*Initializing data structures and memory*/
  surf = (double*)malloc(h*w*sizeof(double));

  /* Read the Image data*/
  fp=fopen(in_file,"r");
  fread(surf,sizeof(double),h*w,fp);
  fclose(fp);

  if (tr_flag){
    /* Read the Tracer data*/
    printf("Reading tracer data\n");
    fflush(stdout);
    getTracerFromFile2(tr_file, h,w, &Tr);
  }
  printf("Done reading data\n");
  fflush(stdout);
  /*read_raw_tracer_data_file(tr_file,&Tr,ev.h,ev.w);*/


  trace_eno_zero_xings(surf, &Tr1, h,  w);
  writeTracerToFile2(out_tr_file, h, w, &Tr1);

  sx=0;
  ex=w;
  sy=0;
  ey=h;
  ox=-0.0;
  oy=-0.0;
  init_draw_ps_original("Xings.ps",10,ey-sy,ex-sx);
  draw_gridlines_skip(-ox,-oy,1,1,ex-sx,ey-sy,9,data); 
  if (tr_flag){
    draw_tracer2(&Tr,sx+ox,sy+oy,RED,1,0.0,sx,ex,sy,ey); 
  }
  draw_tracer2(&Tr1,sx+ox,sy+oy,LIGHT_BLUE,1,0.1,sx,ex,sy,ey); 
  close_draw_ps();
}


void main(int argc, char *argv[])
{
  /* do_cedt_tracer_file(argc, argv);*/
  do_surface_tracer(argc, argv);
}




