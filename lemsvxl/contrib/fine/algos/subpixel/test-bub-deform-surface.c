#include <postscript.h>

#include "subpixel_bubbles.h"


void test_scdm_raph(int argc, char *argv[])
{
  
  FILE *fp;
  int in_region[50*50];
  unsigned char in_seed[50*50];
  double in_surface[50*50], in_image[50*50];
  
  fp=fopen("/home/tbs/bones/subpixel-bubbles/synthetic-images/test-image5_d.raw","r");
  fread(in_image,sizeof(double),50*50,fp);
  fclose(fp);

  fp=fopen("/home/tbs/bones/subpixel-bubbles/synthetic-images/test-image5-bad-seed.raw","r");
  fread(in_seed,sizeof(unsigned char),50*50,fp);
  fclose(fp);

  fp=fopen("/home/tbs/bones/subpixel-bubbles/synthetic-images/test-image5-bad-seed-region.raw","r");
  fread(in_region,sizeof(int),50*50,fp);
  fclose(fp);


  
  run_scdm_raph(-0.2,0.01,0.0,100,1,0,50,50,0,5,0.0,0,1,"test",in_region,
        in_seed,in_surface,in_image);

}

void run_bubbles(int argc, char *argv[])
{
  SP_Bubble_Deform spbub_deform;
  FILE *fp;
  int i,j;
  int height,width;
  
  printf("Running Bubbles\n");
  initialize_sp_bub_deform_args(argc,argv,&spbub_deform);
  height=spbub_deform.params.height;
  width=spbub_deform.params.width;
  print_parameters(&spbub_deform.params);
  
  for (j=0;j<MAX_NO_REGIONS;j++){
    if (spbub_deform.bubbles.region[j].no_of_pixels > 0)
      print_region_stats(&spbub_deform.bubbles.region[j],stdout);
  }
  
  subpixel_bubble_evolution(&spbub_deform); 
  
  /*//fp=fopen("dist.raw","w");
  //fwrite(spbub_deform.bubbles.surface,sizeof(double),height*width,fp);
  fclose(fp);*/
  free_sp_bub_deform(&spbub_deform);

}

void test_shocks(int argc, char *argv[])
{
  
  SP_Bubble_Deform spbub_deform;
  Point grid_point, boundary_point,Sol;
  Point start_pt,end_pt,second_boundary_point;
  FILE *fp;
  double *nothing;
  int geno_boundary_id,shock_boundary_id,second_boundary_id;
  int i,j,kk,num_shocks,tmp_id;
  int rsh_label,lsh_label;
  int xstart=0,ystart=0;
  int verbose=0;
  void *data;
  int OFFSET=50;
  int height,width;

  /*int *back_shock_ptr;
    Boundary *boundary;*/

  /*initialize_sp_bub_params_args(&spbub_deform.params,argc,argv);
  usage();
  print_parameters(&spbub_deform.params);

  exit(1);*/
  
  initialize_sp_bub_deform_args(argc,argv,&spbub_deform);
  height=spbub_deform.params.height;
  width=spbub_deform.params.width;
  print_parameters(&spbub_deform.params);

  convert_boundary_to_linear(spbub_deform.cedt.boundary,spbub_deform.cedt.boundary);
  inter_boundary_shocks(&spbub_deform.cedt,&spbub_deform.waves,
            &spbub_deform.shocks,1,height,width);

  /*kk=0;
    while(spbub_deform.shocks.grouplist[kk] != ENDOFSHOCKS){
    printf("%d \n",spbub_deform.shocks.grouplist[kk]);
    kk++;
    }*/
  printf("Done with shocks\n");
  convert_higher_shocks(&spbub_deform.shocks,spbub_deform.cedt.boundary);

  free_memory_for_hosh(&spbub_deform.shocks,MAX_SHOCK_LENGTH);
  

  init_draw_ps_original("line-shocks.ps", 4, width,height);
  draw_gridlines(xstart,ystart, width,height, YELLOW, data);
 
  draw_shock_and_boundary(&spbub_deform.shocks, spbub_deform.cedt.boundary,0);
  close_draw_ps();
  printf("Done drawing line shocks.ps\n");
  fflush(stdout);

  /* exit(1); */

  init_draw_ps_original("shocks.ps", 4, width,height);
  draw_gridlines(xstart,ystart, width,height, YELLOW, data);
 
  draw_shock_and_boundary(&spbub_deform.shocks, spbub_deform.cedt.boundary,1);
  close_draw_ps();
  printf("Done drawing shocks.ps\n");
  fflush(stdout);

  /*   init_draw_ps_original("coupled.ps", 50, width,height); */
  /* draw_gridlines(xstart,ystart, width,height, YELLOW, data); */
  init_draw_ps_original("coupled.ps", 10, OFFSET,OFFSET);
  xstart=64;ystart=20;
  draw_gridlines(0,0,OFFSET,OFFSET, YELLOW, data);

  draw_shock_and_boundary_crop(&spbub_deform.shocks, spbub_deform.cedt.boundary,0,xstart,ystart,OFFSET); 
  printf("Done drawing shocks for coupled.ps\n");
  fflush(stdout);

  update_shock_labels_on_boundary(&spbub_deform.shocks,&spbub_deform.cedt);
  printf("Done updating boundary labels\n");
  fflush(stdout);

  /*allocate_cedt_boundary(&boundary,height*width);
    back_shock_ptr=(int *)calloc(height*width,sizeof(int));*/
  geno_approx_of_shocks(&spbub_deform.shocks,spbub_deform.shock_boundary, 
            spbub_deform.back_shock_ptr, height,width);
  printf("Done geno approx. for labels\n");
  fflush(stdout);

  if (verbose){
    print_shock_labels_all(spbub_deform.cedt.boundary);
  }
  /*kk=0;num_shocks=0;
    while(spbub_deform.shocks.grouplist[kk] != ENDOFSHOCKS) {
    while(spbub_deform.shocks.grouplist[kk] != ENDOFSHGROUP) {
    kk++;
    num_shocks++;
    }
    kk++;
    }
    convert_boundary_to_linear(spbub_deform.cedt.boundary,spbub_deform.cedt.boundary);
    save_shocks(&spbub_deform.shocks, &spbub_deform.cedt,"jj",nothing,num_shocks,
    width,height);*/

  fp=fopen("finald.dat","w");

  /*Find the Zero level set */
  get_zero_level_set_cedt(&spbub_deform.cedt,spbub_deform.bubbles.zero_level_set, 
              height,width);

  for (j=1;j<height-1;j++){
    for (i=1;i<width-1;i++){
      if (  /*   i>=10 && i<=10 && j>=15 && j<=16  &&  */  /*i==8 && j==37 && */
      spbub_deform.bubbles.zero_level_set[j*width+i] == 0){ 
    if (i> xstart && i< xstart+OFFSET && j> ystart && j< ystart+OFFSET )
      point_draw_ps(i-xstart,j-ystart,PURPLE,data);
    grid_point.x=(double)i;
    grid_point.y=(double)j;
    if (verbose)
      printf("Grid Point %f %f\n",grid_point.x,grid_point.y); 
    geno_boundary_id=spbub_deform.cedt.label[j*width+i];
    find_distance(&spbub_deform.cedt,(double)i,(double)j,geno_boundary_id,&boundary_point);
    shock_boundary_id=
      find_shock_point_for_boundary(spbub_deform.shock_boundary,spbub_deform.cedt.boundary,&spbub_deform.shocks,spbub_deform.back_shock_ptr,
                    grid_point,&boundary_point,geno_boundary_id,&Sol,
                    height,width);
    start_pt=get_start_pt(&spbub_deform.cedt.boundary[geno_boundary_id]);
    end_pt=get_end_pt(&spbub_deform.cedt.boundary[geno_boundary_id]);
    
    if (!is_valid_point(Sol)){
      if (almost_equal_with_tolerance(boundary_point.x,start_pt.x,END_POINT_TOLERANCE) &&
          almost_equal_with_tolerance(boundary_point.y,start_pt.y,END_POINT_TOLERANCE) &&
          is_valid_label(spbub_deform.cedt.boundary[geno_boundary_id].left)){
        tmp_id=spbub_deform.cedt.boundary[geno_boundary_id].left;
        find_distance(&spbub_deform.cedt,(double)i,(double)j,tmp_id,&boundary_point);
        shock_boundary_id=
          find_shock_point_for_boundary(spbub_deform.shock_boundary,spbub_deform.cedt.boundary,
                        &spbub_deform.shocks,spbub_deform.back_shock_ptr,
                        grid_point,&boundary_point,tmp_id,&Sol,
                        height,width);
        if (is_valid_point(Sol))
          geno_boundary_id=tmp_id;
      }
      else if (almost_equal_with_tolerance(boundary_point.x,end_pt.x,END_POINT_TOLERANCE) &&
           almost_equal_with_tolerance(boundary_point.y,end_pt.y,END_POINT_TOLERANCE) &&
           is_valid_label(spbub_deform.cedt.boundary[geno_boundary_id].right)){
        tmp_id=spbub_deform.cedt.boundary[geno_boundary_id].right;
        find_distance(&spbub_deform.cedt,(double)i,(double)j,tmp_id,&boundary_point);
        shock_boundary_id=
          find_shock_point_for_boundary(spbub_deform.shock_boundary,spbub_deform.cedt.boundary,
                        &spbub_deform.shocks,spbub_deform.back_shock_ptr,
                        grid_point,&boundary_point,tmp_id,&Sol,
                        height,width);
        if (is_valid_point(Sol))
        geno_boundary_id=tmp_id;
      }
    }
    if (verbose){
      printf("Start: Dx=%f,Dy=%f\n",boundary_point.x-start_pt.x,boundary_point.y-start_pt.y);
      printf("End: Dx=%f,Dy=%f\n",boundary_point.x-end_pt.x,boundary_point.y-end_pt.y);
    }
    if (verbose)
      printf("shock_point=(%f,%f)\n",Sol.x,Sol.y);
    /*       if (Sol.x != INVALID_POINT && Sol.y != INVALID_POINT){ */
    if (is_valid_point(Sol)){
      if (i> xstart && i< xstart+OFFSET && j> ystart && j< ystart+OFFSET ){
       /*  point_draw_ps(i-xstart,j-xstart,LIGHT_BLUE,data); */
        line_draw_ps(Sol.x-xstart,Sol.y-ystart,grid_point.x-xstart,grid_point.y-ystart,GREEN,data);
      }
      second_boundary_id=
        find_boundary_point_for_shock(spbub_deform.shock_boundary,&spbub_deform.cedt,&spbub_deform.shocks,Sol,
                      shock_boundary_id,boundary_point,geno_boundary_id,
                      &second_boundary_point);
      if (verbose)
        printf("2nd point: ID=%d Pt=(%f,%f)\n",
           second_boundary_id,second_boundary_point.x,second_boundary_point.y);
      if (second_boundary_id != NOLABEL){
        /*if (second_boundary_point.x != INVALID_POINT && 
          second_boundary_point.y != INVALID_POINT){*/
        if (is_valid_point(second_boundary_point)){
          if (i> xstart && i< xstart+OFFSET && j> ystart && j< ystart+OFFSET &&
          second_boundary_point.x>= xstart && second_boundary_point.x <= xstart+OFFSET &&
          second_boundary_point.y>= ystart && second_boundary_point.y <= ystart+OFFSET){
        line_draw_ps(Sol.x-xstart,Sol.y-ystart,second_boundary_point.x-xstart,second_boundary_point.y-ystart,
                 PURPLE,data);
          }
          fprintf(fp, "Grid=(%f,%f) D1=%f,D2=%f,Diff=%f\n",grid_point.x,grid_point.y,
              euclidean_distance(Sol,boundary_point),
              euclidean_distance(Sol,second_boundary_point),
              euclidean_distance(Sol,boundary_point)-euclidean_distance(Sol,second_boundary_point));
        }
      }
    }
    /* printf("\n"); */
    /* exit(1); */
      }
    }
  }
  fclose(fp);
  
  
  close_draw_ps();
  
  /*kk=0;
  while(spbub_deform.shocks.grouplist[kk] != ENDOFSHOCKS) {
    while(spbub_deform.shocks.grouplist[kk] != ENDOFSHGROUP) {
      printf("%d \n",spbub_deform.shocks.grouplist[kk]);
      kk++;
    }
    kk++;
    }*/
  

  for (i=0;i<spbub_deform.bubbles.no_of_regions;i++)
    print_region_stats(&spbub_deform.bubbles.region[i],stdout);
 
  /* subpixel_bubble_evolution(&spbub_deform);  */
  
  fp=fopen("dist.raw","w");
  fwrite(spbub_deform.bubbles.surface,sizeof(double),
     height*width,fp);
  fclose(fp);
  free_sp_bub_deform(&spbub_deform);
  /* free(back_shock_ptr); */
  /* free(boundary); */
}

void test_curvature_normal(int argc, char *argv[])
{
  Tracer Tr;
  Boundary *boundary;
  void *data;
  int height,width;
  int i;
  double normal,rad_vect;
  char fname[200];

  i=1;
  strcpy(fname,argv[i]);i++;
  height=atoi(argv[i]);i++;
  width=atoi(argv[i]);i++;
  
  getTracerFromFile(fname,height,width,&Tr);
  printf("Size=%d\n",Tr.size);
  fflush(stdout);
  allocate_cedt_boundary(&boundary, Tr.size);
  cedt_boundary_with_geno(&Tr,boundary,height,width);
  

  init_draw_ps_original("coupled-shocks.ps", 15, height,width);
  draw_gridlines(0,0,height,width, YELLOW, data);
  draw_cedt_boundary_segment_zoom(boundary,0,0,RED);
  i=0;
  while(boundary[i].id != ENDOFBOUNDARY){
    draw_boundary_tangents_zoom(boundary,i,0,0,BLUE);
    if (boundary[i].type == ARC){
      printf("Start Normal=%f ToRadius=%f AwayRadius=%f\n", 
         boundary[i].arc.tangent1-M_PI/2,
         -atan2(-(double)(boundary[i].arc.y1-boundary[i].arc.center_y),
            (double)(boundary[i].arc.x1-boundary[i].arc.center_x)),
         M_PI+atan2(-(double)(boundary[i].arc.y1-boundary[i].arc.center_y),
            (double)(boundary[i].arc.x1-boundary[i].arc.center_x)));
      printf("End Normal=%f ToRadius=%f AwayRadius=%f\n", 
         boundary[i].arc.tangent2-M_PI/2,
         -atan2(-(double)(boundary[i].arc.y2-boundary[i].arc.center_y),
            (double)(boundary[i].arc.x2-boundary[i].arc.center_x)),
         M_PI+atan2(-(double)(boundary[i].arc.y2-boundary[i].arc.center_y),
            (double)(boundary[i].arc.x2-boundary[i].arc.center_x)));
      printf("\n");
      normal=boundary[i].arc.tangent2-M_PI/2;
      rad_vect=-atan2(-(double)(boundary[i].arc.y2-boundary[i].arc.center_y),
            (double)(boundary[i].arc.x2-boundary[i].arc.center_x));
      if (almost_equal_with_tolerance(normal,rad_vect,0.1)){
    draw_one_boundary_segment_zoom(boundary,i,0,0,PURPLE);
      }
      else{
    draw_one_boundary_segment_zoom(boundary,i,0,0,BLUE);
      }
    }
   i++; 
  }
  close_draw_ps();
  
  free_cedt_boundary(boundary, Tr.size);
  free_data_tracer(&Tr);
}


void test_curvatures(int argc, char *argv[])
{
  Tracer Tr;
  /* Boundary *boundary; */
  SPCedt cedt;
  GENO_Interval intv;
  Region_Stats *region;
  Point grid_point,boundary_point,start_pt,end_pt;
  FILE *fp;
  void *data;
  int height,width;
  int i,j,ii,boundary_id;
  int surf_flag=1;
  double normal,rad_vect,curvature;
  double *array;
  double *darray;
  char fname[200];
  unsigned char *image;

  i=1;
  strcpy(fname,argv[i]);i++;
  height=atoi(argv[i]);i++;
  width=atoi(argv[i]);i++;
  
  if (surf_flag){
    array=(double *)malloc(height*width*sizeof(double));
    darray=(double *)malloc(height*width*sizeof(double));
    fp=fopen(fname,"r");
    fread(array,sizeof(double),height*width,fp);
    fclose(fp);
    CopyIm_DoubleToDouble(array,darray,height,width);
    trace_eno_zero_xings(darray,&Tr,height,width);
  }
  else{
    getTracerFromFile(fname,height,width,&Tr);
  }
  printf("Size=%d\n",Tr.size);
  fflush(stdout);
  /* allocate_cedt_boundary(&boundary, Tr.size); */
  /* cedt_boundary_with_geno(&Tr,boundary,height,width); */
  allocate_data_cedt(&cedt,height,width,Tr.size);
  initialize_cedt(&cedt,  height,width);
  image=(unsigned char *)malloc(height*width*sizeof(unsigned char));
  for (j=0;j<height*width;j++){
    if (darray[j] > 0)
      image[j]=1;
    else
      image[j]=0;
  }
  subpixel_cedt(&cedt,&Tr,image, height,width);

  init_draw_ps_original("curvature.ps", 15, height,width);
  draw_gridlines(0,0,height,width, YELLOW, data);

  draw_cedt_boundary_segment_zoom(cedt.boundary,0,0,RED);

  for (j=0;j<height;j++){
    for (i=0;i<width;i++){
      if (/* cedt.tag[j*width+i] == 1 || */ cedt.tag[j*width+i] == 0){
    boundary_id=cedt.label[j*width+i];
    find_distance(&cedt,(double)i,(double)j,boundary_id,&boundary_point);
    grid_point.x=(double)i;
    grid_point.y=(double)j;
    curvature=compute_curvature_anti_geno_fit(cedt.boundary,boundary_point,boundary_id,
                          grid_point, array,region,height,width);
    printf("Curvature=%f\n",curvature);
    point_draw_ps(grid_point.x,grid_point.y,LIGHT_BLUE,data);
    /* point_draw_ps(boundary_point.x,boundary_point.y,BLUE,data); */
    if (array[j*width+i] > 0)
      force_draw_ps(boundary_point,grid_point,-2*curvature,RED);
    else
    force_draw_ps(boundary_point,grid_point,2*curvature,RED);     
      }
    }
  }
  close_draw_ps();
}


void test_geno_shocks(int argc, char *argv[])
{
  
  Shocks shocks;
  Boundary *boundary;
  int back_shock_ptr[400];
  int height=20,width=20,i;
  int kk;
  void *data;

  shocks.id = (ShId *)  calloc(10.0*height*width,sizeof(ShId));
  shocks.grid = (ShGrid *)  calloc(height*width,sizeof(ShGrid));
  shocks.grouplist = (int *) calloc(height*width,sizeof(int));
  allocate_cedt_boundary(&boundary, height*width);

  i=0;
  shocks.grouplist[i]=92;i++;
  shocks.id[92].group_id=1;
  shocks.id[92].x=1;
  shocks.id[92].y=1;
  shocks.grouplist[i]=97;i++;
  shocks.id[97].group_id=1;
  shocks.id[97].x=2;
  shocks.id[97].y=2;
  shocks.grouplist[i]=72;i++;
  shocks.id[72].group_id=1;
  shocks.id[72].x=3;
  shocks.id[72].y=3;
  shocks.grouplist[i]=47;i++;
  shocks.id[47].group_id=1;
  shocks.id[47].x=4;
  shocks.id[47].y=4;
  shocks.grouplist[i]=49; i++;
  shocks.id[49].group_id=1;
  shocks.id[49].x=4.2;
  shocks.id[49].y=4.3;
  shocks.grouplist[i]=55;i++;
  shocks.id[55].group_id=1;
  shocks.id[55].x=3.0;
  shocks.id[55].y=4.2;
  shocks.grouplist[i]=67;i++;
  shocks.id[67].group_id=1;
  shocks.id[67].x=2.0;
  shocks.id[67].y=3.2;
  shocks.grouplist[i]=41;i++;
  shocks.id[41].group_id=1;
  shocks.id[41].x=1.2;
  shocks.id[41].y=2.2;
  shocks.grouplist[i]=48;i++;
  shocks.id[48].group_id=1;
  shocks.id[48].x=1.1;
  shocks.id[48].y=1.1;
  shocks.grouplist[i]=ENDOFSHGROUP;i++;

  shocks.grouplist[i]=52;i++;
  shocks.id[52].group_id=5;
  shocks.id[52].x=11;
  shocks.id[52].y=11;
  shocks.grouplist[i]=57;i++;
  shocks.id[57].group_id=5;
  shocks.id[57].x=12;
  shocks.id[57].y=12;
  shocks.grouplist[i]=82;i++;
  shocks.id[82].group_id=5;
  shocks.id[82].x=13;
  shocks.id[82].y=13;
  shocks.grouplist[i]=58;i++;
  shocks.id[58].group_id=5;
  shocks.id[58].x=14;
  shocks.id[58].y=14;
  shocks.grouplist[i]=79; i++;
  shocks.id[79].group_id=5;
  shocks.id[79].x=14.2;
  shocks.id[79].y=14.3;
  shocks.grouplist[i]=75;i++;
  shocks.id[75].group_id=5;
  shocks.id[75].x=13.0;
  shocks.id[75].y=14.2;
  shocks.grouplist[i]=77;i++;
  shocks.id[77].group_id=5;
  shocks.id[77].x=12.0;
  shocks.id[77].y=13.2;
  shocks.grouplist[i]=71;i++;
  shocks.id[71].group_id=5;
  shocks.id[71].x=11.0;
  shocks.id[71].y=12.8;
  shocks.grouplist[i]=78;i++;
  shocks.id[78].group_id=5;
  shocks.id[78].x=9.7;
  shocks.id[78].y=12.9;
  shocks.grouplist[i]=50;i++;
  shocks.id[50].x=9.4;
  shocks.id[50].y=13.0;
  shocks.id[50].order=1;
  shocks.grouplist[i]=ENDOFSHGROUP;i++;




  shocks.grouplist[i]=22;i++;
  shocks.id[22].group_id=2;
  shocks.id[22].x=5;
  shocks.id[22].y=5;
  shocks.grouplist[i]=27;i++;
  shocks.id[27].group_id=2;
  shocks.id[27].x=6;
  shocks.id[27].y=6;
  shocks.grouplist[i]=7;i++;
  shocks.id[7].group_id=2;
  shocks.id[7].x=5.7;
  shocks.id[7].y=5.4;

  shocks.grouplist[i]=ENDOFSHGROUP;i++;
  shocks.grouplist[i]=32;i++;
  shocks.id[32].group_id=3;
  shocks.id[32].x=7;
  shocks.id[32].y=7;

  shocks.grouplist[i]=ENDOFSHGROUP;i++;
  shocks.grouplist[i]=12;i++;
  shocks.id[12].group_id=4;
  shocks.id[12].x=8;
  shocks.id[12].y=8;
  shocks.grouplist[i]=17;i++;
  shocks.id[17].group_id=4;
  shocks.id[17].x=9;
  shocks.id[17].y=9;

  shocks.grouplist[i]=ENDOFSHGROUP;i++;
  shocks.grouplist[i]=ENDOFSHOCKS;i++;
    
  geno_approx_of_shocks(&shocks,boundary, back_shock_ptr, height,width);

  kk=0;
  while(shocks.grouplist[kk] != ENDOFSHOCKS) {
    while(shocks.grouplist[kk] != ENDOFSHGROUP) {
      printf("ID %d BackPtr=%d\n",shocks.grouplist[kk],
         back_shock_ptr[shocks.grouplist[kk]]);
      kk++;
    }
    kk++;
  }

  init_draw_ps_original("coupled-shocks.ps", 20, height,width);
  draw_gridlines(0,0,height,width, YELLOW, data);
  
  kk=0;
  while(boundary[kk].id != ENDOFBOUNDARY){
    draw_one_boundary_segment_zoom(boundary,kk,0,0,RED);
    print_cedt_boundary(boundary,kk,kk+1);
    kk++;
  }

  close_draw_ps();
}

void main (int argc, char *argv[])
{
  /*void *data;
  Point a,b;
  Boundary bd[2];
  
  init_draw_ps_original("coupled-shocks.ps", 20, 20,20);
  draw_gridlines(0,0,20,20, YELLOW, data);
  bd[0].type=ARC;
  bd[0].id=1;
  bd[0].arc.y1=11.0;
  bd[0].arc.x1=10.5;
  bd[0].arc.y2=12.0;
  bd[0].arc.x2=10.5;
  bd[0].arc.center_x=11.5;
  bd[0].arc.center_y=11.5;
  bd[0].arc.radius=sqrt(pow(bd[0].arc.x1-bd[0].arc.center_x,2)+
            pow(bd[0].arc.y1-bd[0].arc.center_y,2));
  
  bd[1].id=ENDOFBOUNDARY;
  
  draw_one_boundary_segment_zoom(bd,0,0,0,BLACK);
  point_draw_ps(bd[0].arc.x1,bd[0].arc.y1,RED,data);
  point_draw_ps(bd[0].arc.x2,bd[0].arc.y2,RED,data);
  point_draw_ps(bd[0].arc.center_x,bd[0].arc.center_y,RED,data);
  
  close_draw_ps();*/
  
  /*a.x=5.7;
    a.y=6;
    b.x=7.7;
    b.y=5;
    point_draw_ps(a.x,a.y,BLUE,data);
    point_draw_ps(b.x,b.y,LIGHT_BLUE,data);
    force_draw_ps(a,b,0.2,RED);
    
    a.x=7.7;
    a.y=16;
    b.x=9.7;
    b.y=15;
    point_draw_ps(a.x,a.y,GREEN,data);
    point_draw_ps(b.x,b.y,LIGHT_BLUE,data);
    force_draw_ps(a,b,-0.2,RED);
    
    
    close_draw_ps();*/
 /*  test_geno_shocks(argc,argv);  */ 
 /*  test_curvature_normal(argc, argv); */
  /* test_curvatures(argc, argv);  */
  run_bubbles(argc,argv);
   /*test_shocks(argc,argv);*/  
  /* test_scdm_raph(argc,argv); */

}






