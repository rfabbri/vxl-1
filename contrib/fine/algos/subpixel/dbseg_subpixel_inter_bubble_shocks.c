#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include "dbseg_subpixel_subpixel_bubbles.h"

/***************************************************************************
  Description: Convert the GENO Boundary structure to the LINEAR approx.
               This is necessary as Tek's shock detection uses LINEAR
           approximation.
  Input : boundary - The GENO Boundary.
  Output: linear_boundary - LINEAR Boundary.
  Return: Length of boundary.

**************************************************************************/
int convert_boundary_to_linear(Boundary *boundary,Boundary *linear_boundary)
{
  int ii,kk;

  ii=0;kk=0;
  while (boundary[ii].id != ENDOFBOUNDARY) {
    if (boundary[ii].type == ARC) {
      linear_boundary[kk].line.x1=boundary[ii].arc.x1;
      linear_boundary[kk].line.x2=boundary[ii].arc.x2;
      linear_boundary[kk].line.y1=boundary[ii].arc.y1;
      linear_boundary[kk].line.y2=boundary[ii].arc.y2;
      linear_boundary[kk].type=LINE;
      linear_boundary[kk].left=boundary[ii].left;
      linear_boundary[kk].right=boundary[ii].right;
      linear_boundary[kk].id=boundary[ii].id;
      linear_boundary[kk].segment=boundary[ii].segment;
      kk++;
    }
    else if (boundary[ii].type == LINE){
      linear_boundary[kk].line.x1=boundary[ii].line.x1;
      linear_boundary[kk].line.x2=boundary[ii].line.x2;
      linear_boundary[kk].line.y1=boundary[ii].line.y1;
      linear_boundary[kk].line.y2=boundary[ii].line.y2;
      linear_boundary[kk].type=LINE;
      linear_boundary[kk].left=boundary[ii].left;
      linear_boundary[kk].right=boundary[ii].right;
      linear_boundary[kk].id=boundary[ii].id;
      linear_boundary[kk].segment=boundary[ii].segment;
      kk++;
    }
    else if (boundary[ii].type == POINT){
      /* Skip points for the time being */
      /*linear_boundary[kk].line.x1=boundary[ii].point.x;
    linear_boundary[kk].line.x2=boundary[ii].point.x;
    linear_boundary[kk].line.y1=boundary[ii].point.y;
    linear_boundary[kk].line.y2=boundary[ii].point.y;
    kk++;*/
    }
    ii++;
  }
  linear_boundary[kk].id=ENDOFBOUNDARY;
  linear_boundary[kk+1].id=ENDOFBOUNDARY;
  /* printf("LINEAR kk=%d ii=%d \n",kk,ii); */
  return kk;
}
/***************************************************************************
  Description: Convert the GENO Boundary structure to the LINEAR approx.
               This is necessary as Tek's shock detection uses LINEAR
           approximation.
  Input : boundary - The GENO Boundary.
          segment_list - The list of boundary segments, that are to be 
      copied.
  Output: linear_boundary - LINEAR Boundary.
  Return: Length of boundary.

**************************************************************************/
int convert_selected_boundary_to_linear(Boundary *boundary,
                     Boundary *linear_boundary,
                     int *segment_list)
{
  int ii,kk,jj;
  int num_segments=0;

  ii=0;num_segments=0;
  while(segment_list[ii] != NOLABEL){
    num_segments++;
    ii++;
  }

  ii=0;kk=0;
  while (boundary[ii].id != ENDOFBOUNDARY) {
    for (jj=0;jj<num_segments;jj++){
      if (boundary[ii].segment == segment_list[jj]){
    if (boundary[ii].type == ARC) {
      linear_boundary[kk].line.x1=boundary[ii].arc.x1;
      linear_boundary[kk].line.x2=boundary[ii].arc.x2;
      linear_boundary[kk].line.y1=boundary[ii].arc.y1;
      linear_boundary[kk].line.y2=boundary[ii].arc.y2;
      linear_boundary[kk].type=LINE;
      linear_boundary[kk].left=boundary[ii].left;
      linear_boundary[kk].right=boundary[ii].right;
      linear_boundary[kk].id=boundary[ii].id;
      linear_boundary[kk].segment=boundary[ii].segment;
      kk++;
    }
    else if (boundary[ii].type == LINE){
      linear_boundary[kk].line.x1=boundary[ii].line.x1;
      linear_boundary[kk].line.x2=boundary[ii].line.x2;
      linear_boundary[kk].line.y1=boundary[ii].line.y1;
      linear_boundary[kk].line.y2=boundary[ii].line.y2;
      linear_boundary[kk].type=LINE;
      linear_boundary[kk].left=boundary[ii].left;
      linear_boundary[kk].right=boundary[ii].right;
      linear_boundary[kk].id=boundary[ii].id;
      linear_boundary[kk].segment=boundary[ii].segment;
      kk++;
    }
    else if (boundary[ii].type == POINT){
      /* Skip points for the time being */
      /*linear_boundary[kk].line.x1=boundary[ii].point.x;
        linear_boundary[kk].line.x2=boundary[ii].point.x;
        linear_boundary[kk].line.y1=boundary[ii].point.y;
        linear_boundary[kk].line.y2=boundary[ii].point.y;
        kk++;*/
    }
    break;
      }
    }
    ii++;
  }
  linear_boundary[kk].id=ENDOFBOUNDARY;
  linear_boundary[kk+1].id=ENDOFBOUNDARY;
  /* printf("LINEAR kk=%d ii=%d \n",kk,ii); */
  return kk;
}


/***************************************************************************
  Description: Scan the shock list convert the higher order shocks into 
               individual shocks.
  Input : shocks - The shocks detected on the boundary
***************************************************************************/
void convert_higher_shocks(Shocks *shocks, Boundary *boundary)
{

  int ii,kk,jj,pts_ina_group,group_id,shock_id;
  int boundary_id,segment,shock_end;
  int verbose=0;
  int segment1[2000], segment2[2000],group[2000];
  int boundary_id1,boundary_id2;
  int b1,b2;
  int hosh_id;
  Hosh *hosh;
  
  if (verbose){
    printf("Converting Higher Order Shocks\n");
    fflush(stdout);
  }

  kk=0;ii=0;jj=0;
  while(shocks->grouplist[kk] != ENDOFSHOCKS) {
    group_id=-100;jj++;
    while(shocks->grouplist[kk] != ENDOFSHGROUP){
      shock_id = shocks->grouplist[kk];
      if (shocks->id[shock_id].order != 4 && 
      group_id != shocks->id[shock_id].group_id){
    group_id = shocks->id[shock_id].group_id;
    group[ii]=group_id;
    boundary_id1=shocks->id[shock_id].boundary_id[0];
    segment1[ii] = boundary[boundary_id1].segment;
    boundary_id2=shocks->id[shock_id].boundary_id[1];
    segment2[ii] = boundary[boundary_id2].segment;
    ii++;
      }
      if (verbose==2){
      printf("ShkID=%d GrpID=%d BDRY=(%d,%d) SEGS=(%d,%d)\n",shock_id,
         shocks->id[shock_id].group_id,
         shocks->id[shock_id].boundary_id[0],shocks->id[shock_id].boundary_id[1],
         boundary[shocks->id[shock_id].boundary_id[0]].segment,
         boundary[shocks->id[shock_id].boundary_id[1]].segment);
      }
      kk++;
    }
    kk++;
  }
  if (verbose){
    for (kk=0;kk<ii;kk++){
      printf ("%d SEGS=(%d,%d) Grp=%d\n",kk,segment1[kk],segment2[kk],group[kk]);
    }
  }
  
  shock_end=shocks->ptr;
  printf("SHOCK END =%d\n",shocks->ptr);
  kk=0; ii=0;group_id = 1;
  while(shocks->grouplist[kk] != ENDOFSHOCKS) {
    pts_ina_group=0;
    if (shock_id > 0 && verbose==2){
      printf("New Shock Branch\n");
      printf("ShkID=%d SEGS=(%d,%d) ORDER=%d BDRY=(%d,%d)\n",shock_id,
         segment1[ii],segment2[ii],
         shocks->id[shock_id].order,
         boundary[shocks->id[shock_id].boundary_id[0]].segment,
         boundary[shocks->id[shock_id].boundary_id[1]].segment);
    }
    while(shocks->grouplist[kk] != ENDOFSHGROUP){
      shock_id = shocks->grouplist[kk];
      group_id = shocks->id[shock_id].group_id;
      /*Check if we have a higher order shock*/
      if (shocks->id[shock_id].order == 4){
    if (verbose) { printf("HOSH %d\n",ii); }
    hosh_id=shocks->id[shock_id].hosh_id;
    hosh=&shocks->hosh[hosh_id];
    shocks->id[shock_end+1].group_id=group[ii];
    shocks->id[shock_end+1].y=shocks->id[shock_id].y;
    shocks->id[shock_end+1].x=shocks->id[shock_id].x;
    shocks->id[shock_end+1].dist=shocks->id[shock_id].dist;
    b2=0;
    for (b1=0;b1<hosh->boundary_no;b1++){
      boundary_id=hosh->boundary_id[b1];
      segment=boundary[boundary_id].segment;
      /* printf("Segment=%d\n",segment); */
      if (segment == segment1[ii]){
        /*printf("%d Selected Segment1=%d\n",ii,segment1[ii]); */
        shocks->id[shock_end+1].boundary_id[0]=hosh->boundary_id[b1];
        shocks->id[shock_end+1].boundary[0].x=hosh->boundary[b1].x;
        shocks->id[shock_end+1].boundary[0].y=hosh->boundary[b1].y;
        b2++;
      }
      if (segment == segment2[ii]){
        /* printf("%d Selected Segment2=%d\n",ii, segment2[ii]); */
        shocks->id[shock_end+1].boundary_id[1]=hosh->boundary_id[b1];
        shocks->id[shock_end+1].boundary[1].x=hosh->boundary[b1].x;
        shocks->id[shock_end+1].boundary[1].y=hosh->boundary[b1].y;
        b2++;
      }
    }
    shocks->grouplist[kk]=shock_end+1;
    if (verbose){
      printf("HOSH\n"); 
      printf("ShkID=%d GrpID=%d Bdry=%d,%d \n",shock_end+1,
         shocks->id[shock_end+1].group_id,
         shocks->id[shock_end+1].boundary_id[0],
         shocks->id[shock_end+1].boundary_id[1]);
    }
    shock_end++;
      }
      /*printf("SHK=%d GRP=%d SG1=%d SG2=%d \n",shock_id,group_id,
    segment1[ii],segment2[ii]);*/
      kk++;
    }
    kk++;ii++;
  }
}
/***************************************************************************
  Description: Scan the shock list and add the shock label lists to the
               appropriate boundary segments.
  Input : shocks - The Shocks detected for the boundary
  Output: boundary - The SPCedt structure.
  Return: no. of shocks
**************************************************************************/
int update_shock_labels_on_boundary(Shocks *shocks, SPCedt *cedt)
{

  int ii,kk,pts_ina_group,group_id,shock_id;
  int boundary_id;
  int num_shocks=0;
  int verbose=0;
  Point shock_gen_loc;

  if (verbose){
    printf("shocks->grouplist 1 & 2 = %d %d \n", shocks->grouplist[0], shocks->grouplist[1]);
  }

  kk=0; group_id = 1;
  while(shocks->grouplist[kk] != ENDOFSHOCKS) {
    pts_ina_group=0;
    while(shocks->grouplist[kk] != ENDOFSHGROUP) {
      if (pts_ina_group == 0 && shocks->grouplist[kk+1] == ENDOFSHGROUP){
    /* Don't consider shocks that have just one point*/
      }
      else{
    shock_id = shocks->grouplist[kk];
    for (ii=0;ii<2;ii++){
      boundary_id=shocks->id[shock_id].boundary_id[ii];
      shock_gen_loc.x=shocks->id[shock_id].boundary[ii].x;
      shock_gen_loc.y=shocks->id[shock_id].boundary[ii].y;
      insert_shock_label_list(cedt,shock_gen_loc,boundary_id,shock_id);
      num_shocks++;
    }
      }
      kk++;pts_ina_group++;
    }
    group_id++;
    kk++; 
  }
  return num_shocks;
}



/***************************************************************************
  Description: Create a GENO approx. for the shock points. 
  Input : shocks - The Shock structure.
          height,width - image dimensions.
  Output: boundary - The GENO approx. of the shock boundary.
          back_shock_ptr - The array of shock->boundary pointers.
**************************************************************************/
void geno_approx_of_shocks(Shocks *shocks, Boundary *boundary, int *back_shock_ptr, 
               int height, int width)
{
  Tracer Tr;
  Point start_pt,end_pt;
  int shock_id,boundary_idx;
  int contour_idx,kk,jj,start_idx,pts_ina_contour;
  int pts_ina_group;
  int verbose=0,i,draw=0,old_method=0;
  

  printf("H=%d W=%d\n",height,width);
  fflush(stdout);
  Tr.vert = (double *)calloc(2*height*width,sizeof(double));
  Tr.horiz = (double *)calloc(2*height*width,sizeof(double));
  Tr.label = (int *)calloc(2*height*width,sizeof(int));
  Tr.id = (int *)calloc(2*height*width,sizeof(int));
  
  
  /*Initialize the back shock pointer*/
  for (i=0;i<height*width;i++)
    back_shock_ptr[i]=NOLABEL;
  
  /*Convert the Shock Group List to Tracer format*/
  contour_idx=0;kk=0;jj=0;
  while(shocks->grouplist[kk] != ENDOFSHOCKS) {
    /*if (shocks->grouplist[kk] != ENDOFSHGROUP){
      shock_id=shocks->grouplist[kk];
      Tr.label[contour_idx]=shocks->id[shock_id].group_id;
      }
      else
      Tr.label[contour_idx]=0;*/
    pts_ina_group=0;
    while(shocks->grouplist[kk] != ENDOFSHGROUP) {
      if (pts_ina_group == 0 && shocks->grouplist[kk+1] == ENDOFSHGROUP){
    /* Don't consider shocks that have just one point*/
      }
      else{
    shock_id=shocks->grouplist[kk];
    Tr.vert[jj]=shocks->id[shock_id].y;
    Tr.horiz[jj]=shocks->id[shock_id].x;
    Tr.label[jj]=shocks->id[shock_id].group_id;
    Tr.id[jj]=shock_id;    
    if (verbose){
      printf("%d %d\n",kk,shock_id);
      printf("%02d Tr(%f %f) L=%d ShID=%d\n",jj,Tr.horiz[jj],Tr.vert[jj],
         Tr.label[jj],shock_id);
    }
    jj++;
    pts_ina_group++;
      }
      kk++;
    }
    Tr.vert[jj]=ENDOFCONTOUR;
    Tr.horiz[jj]=ENDOFCONTOUR;
    kk++;jj++;contour_idx++;
  }
  Tr.vert[jj]=ENDOFLIST;
  Tr.horiz[jj]=ENDOFLIST;
  Tr.size=2*jj+2;
  

  if (verbose){
    for (i=0;i<=kk;i++)
      printf("%02d (%f %f) L=%d\n",i,Tr.horiz[i],Tr.vert[i],Tr.label[i]);
    printf("SHOCK-GENO: Tracer index=%d Contour index=%d\n",jj,contour_idx);
    fflush(stdout);
  }
  
  /* Do a GENO and fill the boundary structure */
  cedt_boundary_with_geno_labelled(&Tr,boundary,height,width);

  if (verbose){
    printf("Done doing GENO for shocks\n");
    fflush(stdout);
  }

  if (draw){
    draw_tracer(&Tr,BLACK);
    draw_cedt_boundary_segment_zoom(boundary,0,0,GREY);
  }

  /*kk=0;
  while(boundary[kk].id != ENDOFBOUNDARY){
    print_cedt_boundary(boundary,kk,kk+1);
    kk++;
    }*/

  if (old_method){
    /* Fill the appopriate shock labels. Previous function just abitrarily assigns
       them. Also there are shock labels for each shock point, and now (after GENO
       interpolation) we have intervals. So shock label at the start point is 
       assigned to the interval and the right one is its neighbor. Back shock ptr is
       an array that lets you go from the shock label to its location on the 
       boundary (GENO approx.).*/
    contour_idx=0;kk=0;boundary_idx=0;
    while(shocks->grouplist[kk] != ENDOFSHOCKS){
      pts_ina_contour = 0; start_idx = kk; 
      if (verbose == 2){ 
    printf("New Branch\n");
      }
      while(shocks->grouplist[kk] != ENDOFSHGROUP && 
        shocks->grouplist[kk+1] != ENDOFSHGROUP){
    shock_id=shocks->grouplist[kk];
    back_shock_ptr[shock_id]=boundary_idx;
    boundary[boundary_idx].id=shock_id;
    if (kk != start_idx)
      boundary[boundary_idx].left = shocks->grouplist[kk-1];
    else
      boundary[boundary_idx].left = NOLABEL;
    boundary[boundary_idx].right = shocks->grouplist[kk+1];
    if (verbose == 2){
      start_pt=get_start_pt(&boundary[boundary_idx]);
      end_pt=get_end_pt(&boundary[boundary_idx]);
      printf("Shock Loc=(%5.3f,%5.3f) Boundary: St=(%5.3f,%5.3f) En=(%5.3f,%5.3f)\n",
         shocks->id[shock_id].x,shocks->id[shock_id].y,
         start_pt.x,start_pt.y,end_pt.x,end_pt.y);
    }
    boundary_idx++;
    kk++;
    pts_ina_contour++;
      }
      /* If the last but one point is a valid shock label, set the back
     shock ptr array */
      if (shocks->grouplist[kk] != ENDOFSHGROUP){
    pts_ina_contour++;
    shock_id=shocks->grouplist[kk];
    back_shock_ptr[shock_id]=boundary_idx-1;
      }
      /* If the no. of points is a contour is one then special consideration is
     needed.*/
      if (pts_ina_contour == 1){
    /*Currently nothing needs to be done if we have only one shock*/
    /*boundary[boundary_idx].left = NOLABEL;
      boundary[boundary_idx].right = NOLABEL;
      shock_id=shocks->grouplist[kk];
      back_shock_ptr[shock_id]=boundary_idx;
      boundary[boundary_idx].id=shock_id;
      boundary_idx++;*/
      }
      kk++;
      kk++;
    }/* while(shocks->grouplist[kk] != ENDOFSHOCKS)*/
  }/* if (old_method)*/
  
  contour_idx=0;kk=0;boundary_idx=0;
  while(boundary[kk].id != ENDOFBOUNDARY){
    if (kk == 0)
      start_idx = kk; 
    else if (boundary[kk].segment != boundary[kk-1].segment)
      start_idx = kk;
    
    shock_id=boundary[kk].id;
    back_shock_ptr[shock_id]=kk;
    /* For the last point there is some special handling.
       Need to see if the contour is closed. If not the 
       right label's back-shock-ptr needs to be set */
    if (boundary[kk].segment != boundary[kk+1].segment){
      if (boundary[kk].right != boundary[start_idx].id){
    /*printf("RT=%d IN=%d\n",boundary[kk].right,boundary[start_idx].id);*/
    shock_id=boundary[kk].right;
    back_shock_ptr[shock_id]=kk;
      }
    }
    kk++;
  }/* while(boundary[kk].id != ENDOFBOUNDARY)*/ 
  
  if (verbose){
    kk=0;
    while(shocks->grouplist[kk] != ENDOFSHOCKS) {
      while(shocks->grouplist[kk] != ENDOFSHGROUP) {
    shock_id=shocks->grouplist[kk];
    printf("%d %d %d \n",kk,shock_id,back_shock_ptr[shock_id]);
    kk++;
      }
      kk++;
    }
  }
  free(Tr.vert);
  free(Tr.horiz);
  free(Tr.label);
  free(Tr.id);
}



/*****************************************************************************
  Description: Find the shock point corresponding to the point specifed on the
               boundary (GENO approx.). 
  Inputs: shock_boundary - GENO approx. of the shock points.
          boundary - GENO approx. of the boundary.
      back_shock_ptr - The array of shock->boundary pointers.
      boundary_point - Point on the boundary.
      grid_point - The grid location that gives rise to the boundary_point, 
      (boundary_point is the closest point on the GENO boundary to the grid
      point).
      boundary_id - the id of the boundary piece that send CEDT waves to 
      grid point.
      height,width - image dimensions
  Output: shock_point - The shock point that correponds to boundary_point.
          If a shock_point could not be found return 
      (INVALID_POINT,INVALID_POINT).
  Return: shock_boundary_id - The Boundary_Id (of the GENO approx.) of the 
          shock point.
*****************************************************************************/

int find_shock_point_for_boundary(Boundary *shock_boundary, Boundary *boundary,
                  Shocks *shocks, int *back_shock_ptr,Point grid_point,
                  Point *boundary_point, int boundary_id, 
                  Point *shock_point,int height,int width)
{
  int rsh_label,lsh_label;
  int rb_label,lb_label;
  int shock_boundary_id,shock_id;
  int verbose=0,linear=1,draw=0;
  int *shock_labels,num_labels,i,j;
  int *left_shock_labels=0,num_left_labels;
  int *right_shock_labels=0,num_right_labels;
  int stupid_idea=0,force_intersection=0;
  void *data;
  Point start_pt,end_pt;
  Point lsh_gen_point,rsh_gen_point;
  Line Ln;

  shock_point->x=INVALID_POINT;
  shock_point->y=INVALID_POINT;
  
  

  /* Huseyin's shock detection uses a LINEAR approx. and so I need to use
     a linear approx. also to find the closest point on the boundary */
  if (linear){
    start_pt = get_start_pt(&boundary[boundary_id]);
    end_pt = get_end_pt(&boundary[boundary_id]);
    Ln.x1=start_pt.x;
    Ln.y1=start_pt.y;
    Ln.x2=end_pt.x;
    Ln.y2=end_pt.y;
    intersection_of_a_point_and_line_tbs(&Ln,&grid_point,boundary_point);
  }
  
  /* Try to find the shock labels on the immediate right and left of the boundary
     point. This should point us to the shock boundary segment which  we should
     consider.*/
  rsh_label=get_next_label_on_right(boundary,boundary_id,*boundary_point,&rb_label);
  lsh_label=get_next_label_on_left(boundary,boundary_id,*boundary_point,&lb_label);
  if (verbose){
    printf("Shock label on left =%d Shock label on right =%d \n",
       lsh_label,rsh_label);
  }
  
  /* If none of the neighboring shock labels are invalid, the shock segment
     is in the middle and so find the correct shock boundary id */
  if (rsh_label != NOLABEL && lsh_label != NOLABEL){
    /* Find the relevant shock boundary segment */
    shock_boundary_id=find_correct_shock_boundary_id(shock_boundary,boundary,
                             back_shock_ptr,lsh_label,
                             rsh_label,lb_label,rb_label);
  }
  else if(rsh_label != NOLABEL){
    shock_boundary_id=back_shock_ptr[rsh_label];
  }
  else if(lsh_label != NOLABEL){
    shock_boundary_id=back_shock_ptr[lsh_label];
  }
  else {
    return NOLABEL;
  }

  if (verbose){
    printf("RELEVANT Bid=%d ShBid=%d\n",boundary_id,shock_boundary_id);
    /*line_draw_ps(boundary_point->x,boundary_point->y,
         shock_boundary[shock_boundary_id].arc.x1,
         shock_boundary[shock_boundary_id].arc.y1,GREEN,data);
    line_draw_ps(boundary_point->x,boundary_point->y,
         shock_boundary[shock_boundary_id].arc.x2,
         shock_boundary[shock_boundary_id].arc.y2,GREEN,data);*/
    if (lsh_label != NOLABEL){
      printf("Shock labels on the left\n");
      print_shock_labels(&boundary[lb_label]);
    }
    if (rsh_label != NOLABEL){
      printf("Shock labels on the right\n");
      print_shock_labels(&boundary[rb_label]);
    }
  }
  if (draw){
    point_draw_ps(boundary_point->x,boundary_point->y,BLACK,data);
  }

  if (shock_boundary_id != NOLABEL){
    /* Now find the intersection*/
    find_line_geno_boundary_intersection(shock_boundary,shock_boundary_id,
                     grid_point,*boundary_point,shock_point);
    /* Check if intersecting point is in the image. */
    if (shock_point->x < 0.0 || shock_point->y < 0.0 ||
    shock_point->x >= width || shock_point->y >= height){
      shock_point->x=INVALID_POINT;
      shock_point->y=INVALID_POINT;
    }
  }
  
  /*Now we need to handle a special case. If the "closest boundary point"
    is itself a "shock generating point" and it generates multiple labels, the
    right labels might not be picked up. Hence, find multiple labels at the point
    and then try all combinations. 
    
    This is a stupid thing to do. If the  "closest boundary point"
    is itself a "shock generating point" we know the corresponding shock point 
    and hence the second boundary point immediately.*/
  if (stupid_idea){
    if (shock_point->x==INVALID_POINT && shock_point->y==INVALID_POINT){
      num_labels=get_multiple_shock_labels(boundary,boundary_id,*boundary_point,
                       &shock_labels);
      if (verbose){
    printf("Num Labels =%d\n",num_labels);
    for (i=0;i<num_labels;i++)
    printf("%d \n",shock_labels[i]);
      }
      for (i=0;i<num_labels;i++){
    if (lsh_label != shock_labels[i] && 
        shock_point->x==INVALID_POINT && shock_point->y==INVALID_POINT){
      shock_boundary_id=
        find_correct_shock_boundary_id(shock_boundary,boundary,back_shock_ptr,
                       lsh_label,shock_labels[i],NOLABEL,NOLABEL);
      if (shock_boundary_id != NOLABEL)
        find_line_geno_boundary_intersection(shock_boundary,shock_boundary_id,
                         grid_point,*boundary_point,shock_point);
      /* Check if intersecting point is in the image. */
      if (shock_point->x < 0.0 || shock_point->y < 0.0 ||
          shock_point->x >= width || shock_point->y >= height){
        shock_point->x=INVALID_POINT;
        shock_point->y=INVALID_POINT;
      }
    }
      }
      for (i=0;i<num_labels;i++){
    if (rsh_label != shock_labels[i] && 
        shock_point->x==INVALID_POINT && shock_point->y==INVALID_POINT){
      shock_boundary_id=
        find_correct_shock_boundary_id(shock_boundary,boundary,back_shock_ptr,
                       rsh_label,shock_labels[i],NOLABEL,NOLABEL);
      if (shock_boundary_id != NOLABEL)
        find_line_geno_boundary_intersection(shock_boundary,shock_boundary_id,
                         grid_point,*boundary_point,shock_point);
      /* Check if intersecting point is in the image. */
      if (shock_point->x < 0.0 || shock_point->y < 0.0 ||
          shock_point->x >= width || shock_point->y >= height){
        shock_point->x=INVALID_POINT;
        shock_point->y=INVALID_POINT;
      }
    }
      }
      for (i=0;i<num_labels;i++){
    for (j=i+1;j<num_labels;j++){
      if (shock_point->x==INVALID_POINT && shock_point->y==INVALID_POINT){
        shock_boundary_id=
          find_correct_shock_boundary_id(shock_boundary,boundary,back_shock_ptr,
                         shock_labels[j],shock_labels[i],NOLABEL,NOLABEL);
        if (shock_boundary_id != NOLABEL)
          find_line_geno_boundary_intersection(shock_boundary,shock_boundary_id,
                           grid_point,*boundary_point,shock_point);
        /* Check if intersecting point is in the image. */
        if (shock_point->x < 0.0 || shock_point->y < 0.0 ||
        shock_point->x >= width || shock_point->y >= height){
          shock_point->x=INVALID_POINT;
          shock_point->y=INVALID_POINT;
        }
      }
    }
      }
      
      if (num_labels > 0){
    free(shock_labels);
      }
    }
  }/*End if (stupid_idea)*/
  
  /*Now the simple way. */
  if (shock_point->x==INVALID_POINT && shock_point->y==INVALID_POINT){
    num_labels=get_multiple_shock_labels(boundary,boundary_id,*boundary_point,
                     &shock_labels);
    if (num_labels == 1){
      shock_id=shock_labels[0];
      shock_boundary_id=back_shock_ptr[shock_id];
      shock_point->x=shocks->id[shock_id].x;
      shock_point->y=shocks->id[shock_id].y;
    }
    else if (num_labels > 1){
      /*Pick the middle label */
      num_labels=(int)floor(num_labels/2);
      shock_id=shock_labels[num_labels];
      shock_boundary_id=back_shock_ptr[shock_id];
      shock_point->x=shocks->id[shock_id].x;
      shock_point->y=shocks->id[shock_id].y;
    }
    if (num_labels > 0){
      free(shock_labels);
    }
  }/*if(shock_point->x==INVALID_POINT ...)*/

  /*Now going to handle another special case. If both left and right "shock generating
    points" generate multiple shocks, then there is a need to consider all those
    shock intervals. 
    This is not intuitive and doesn't make very much sense. So I am not going
    to consider this for the time being.*/
  if (force_intersection){
    if (shock_point->x==INVALID_POINT && shock_point->y==INVALID_POINT){
      lsh_gen_point=get_shock_gen_point(boundary,lb_label,lsh_label);
      rsh_gen_point=get_shock_gen_point(boundary,rb_label,rsh_label);
      num_left_labels=get_multiple_shock_labels(boundary,lb_label,lsh_gen_point,
                        &left_shock_labels);
      num_right_labels=get_multiple_shock_labels(boundary,rb_label,rsh_gen_point,
                         &right_shock_labels);
      for (i=0;i<num_left_labels;i++){
    for (j=i+1;j<num_left_labels;j++){
      if (shock_point->x==INVALID_POINT && shock_point->y==INVALID_POINT){
        shock_boundary_id=
          find_correct_shock_boundary_id(shock_boundary,boundary,back_shock_ptr,
                         left_shock_labels[j],left_shock_labels[i],
                         NOLABEL,NOLABEL);
        if (shock_boundary_id != NOLABEL)
          find_line_geno_boundary_intersection(shock_boundary,shock_boundary_id,
                           grid_point,*boundary_point,shock_point);
        /* Check if intersecting point is in the image. */
        if (shock_point->x < 0.0 || shock_point->y < 0.0 ||
        shock_point->x >= width || shock_point->y >= height){
          shock_point->x=INVALID_POINT;
          shock_point->y=INVALID_POINT;
        }
      }
    }
      }
      for (i=0;i<num_right_labels;i++){
    for (j=i+1;j<num_right_labels;j++){
      if (shock_point->x==INVALID_POINT && shock_point->y==INVALID_POINT){
        shock_boundary_id=
          find_correct_shock_boundary_id(shock_boundary,boundary,back_shock_ptr,
                         right_shock_labels[j],right_shock_labels[i],
                         NOLABEL,NOLABEL);
        if (shock_boundary_id != NOLABEL)
          find_line_geno_boundary_intersection(shock_boundary,shock_boundary_id,
                           grid_point,*boundary_point,shock_point);
        /* Check if intersecting point is in the image. */
        if (shock_point->x < 0.0 || shock_point->y < 0.0 ||
        shock_point->x >= width || shock_point->y >= height){
          shock_point->x=INVALID_POINT;
          shock_point->y=INVALID_POINT;
        }
      }
    }
      }
      for (i=0;i<num_right_labels;i++){
    for (j=0;j<num_left_labels;j++){
      if (shock_point->x==INVALID_POINT && shock_point->y==INVALID_POINT){
        shock_boundary_id=
          find_correct_shock_boundary_id(shock_boundary,boundary,back_shock_ptr,
                         left_shock_labels[j],right_shock_labels[i],
                         NOLABEL,NOLABEL);
        if (shock_boundary_id != NOLABEL)
          find_line_geno_boundary_intersection(shock_boundary,shock_boundary_id,
                           grid_point,*boundary_point,shock_point);
        /* Check if intersecting point is in the image. */
        if (shock_point->x < 0.0 || shock_point->y < 0.0 ||
        shock_point->x >= width || shock_point->y >= height){
          shock_point->x=INVALID_POINT;
          shock_point->y=INVALID_POINT;
        }
      }
    }
      }
    }
    
    if (num_left_labels > 0){
      free(left_shock_labels);
    }
    if (num_right_labels > 0){
      free(right_shock_labels);
    }
  }/*End if (force_intersection)*/

  if (verbose){
    print_cedt_boundary(shock_boundary,shock_boundary_id,shock_boundary_id+1);
    print_cedt_boundary(boundary,boundary_id,boundary_id+1);
    print_shock_labels(&boundary[boundary_id]);
    printf("Grid=(%f,%f) Bdry=%f,%f) LslL=%d RshL=%d ShB_Id=%d\n",grid_point.x,grid_point.y,
       boundary_point->x,boundary_point->y,lsh_label,rsh_label,shock_boundary_id);
  }

  if (draw)
    draw_one_boundary_segment_zoom(shock_boundary,shock_boundary_id,0,0,BLACK);

  return shock_boundary_id;
}

/*****************************************************************************
  Description: Find the boundary point corresponding to the shock point. The
               shock point itself was found from the boundary point and now the
           other contributing boundary point is being found.
  Inputs: shock_boundary - GENO approx. of the shock points.
          boundary - GENO approx. of the boundary.
      back_shock_ptr - The array of shock->boundary pointers.
      boundary_point - Point on the boundary.
      shock_boundary_id-The Boundary_Id (of the GENO approx.) of the 
          shock point.
      grid_point - The grid location that gives rise to the boundary_point, 
      (boundary_point is the closest point on the GENO boundary to the grid
      point).
      boundary_id - the id of the boundary piece that send CEDT waves to 
      grid point.
  Output: shock_point - The shock point that correponds to boundary_point.
  Return: shock_boundary_id - The Boundary_Id (of the GENO approx.) of the 
          shock point.
*****************************************************************************/
int find_boundary_point_for_shock(Boundary *shock_boundary, SPCedt *cedt, Shocks *shocks,
                  Point shock_point, int shock_boundary_id,
                  Point first_boundary_point,int first_boundary_id, 
                  Point *second_boundary_point)
{
  int verbose=0,draw=0;
  int true_left_shock_id,left_shock_id,right_shock_id;
  int lsh_gen_boundary_id,rsh_gen_boundary_id;
  int next_lsh_id,next_rsh_id;
  int look_left=0,look_right=0;
  int second_boundary_id;
  int first_boundary_flag,second_boundary_flag;
  int min_dist_boundary_id;
  int tmp,first_time;
  double first_distance,second_distance,min_distance;
  double tolerance=0.02;
  void *data;
  Point lsh_gen_point,rsh_gen_point,min_dist_boundary_point;

  second_boundary_point->x=INVALID_POINT;
  second_boundary_point->y=INVALID_POINT;

  /* Find the contour id of the first boundary point and the shock ids at the
   endpoints of this shock interval (GENO approx)*/
  first_boundary_flag=cedt->boundary[first_boundary_id].segment;
  left_shock_id=shock_boundary[shock_boundary_id].id;
  right_shock_id=shock_boundary[shock_boundary_id].right;
  
  if (verbose){
    printf("LShId=%d RShId=%d \n",left_shock_id,right_shock_id);
    print_cedt_boundary(shock_boundary,shock_boundary_id,shock_boundary_id+1);
  }
  if (draw)
    draw_one_boundary_segment_zoom(shock_boundary,shock_boundary_id,0,0,BLUE);

  /* Find the boundary segments on the second contour that gives rise to 
     the shocks at each end point. Here it is assumed that the all shocks
     are formed by the interaction of two different boundary segments
     from two different contour segments*/
  lsh_gen_boundary_id=shocks->id[left_shock_id].boundary_id[0];
  second_boundary_flag=cedt->boundary[lsh_gen_boundary_id].segment;
  if (second_boundary_flag == first_boundary_flag){
    lsh_gen_boundary_id=shocks->id[left_shock_id].boundary_id[1];
    second_boundary_flag=cedt->boundary[lsh_gen_boundary_id].segment;
    lsh_gen_point.x=shocks->id[left_shock_id].boundary[1].x;
    lsh_gen_point.y=shocks->id[left_shock_id].boundary[1].y;
  }
  else{
    lsh_gen_point.x=shocks->id[left_shock_id].boundary[0].x;
    lsh_gen_point.y=shocks->id[left_shock_id].boundary[0].y;
  }
  rsh_gen_boundary_id=shocks->id[right_shock_id].boundary_id[0];
  if (cedt->boundary[rsh_gen_boundary_id].segment == first_boundary_flag){
    rsh_gen_boundary_id=shocks->id[right_shock_id].boundary_id[1];
    rsh_gen_point.x=shocks->id[right_shock_id].boundary[1].x;
    rsh_gen_point.y=shocks->id[right_shock_id].boundary[1].y;
  }
  else{
    rsh_gen_point.x=shocks->id[right_shock_id].boundary[0].x;
    rsh_gen_point.y=shocks->id[right_shock_id].boundary[0].y;
  }
  /* lsh_gen_boundary_id & rsh_gen_boundary_id are the boundary 
     segments (on 2nd contour) that gives rise to the shock points
     at either end points. */

  /* If the shock point corresponds to an ENO shock point, we are in luck,
     as we know the corresponding "shock generating points" on the 
     boundary*/

  if (almost_equal_with_tolerance(shock_point.x,shocks->id[left_shock_id].x,1E-5) &&
      almost_equal_with_tolerance(shock_point.y,shocks->id[left_shock_id].y,1E-5) ){
    second_boundary_point->x=lsh_gen_point.x;
    second_boundary_point->y=lsh_gen_point.y;
    return lsh_gen_boundary_id;
  }
  else if (almost_equal_with_tolerance(shock_point.x,shocks->id[right_shock_id].x,1E-5) &&
       almost_equal_with_tolerance(shock_point.y,shocks->id[right_shock_id].y,1E-5) ){
    second_boundary_point->x=rsh_gen_point.x;
    second_boundary_point->y=rsh_gen_point.y;
    return rsh_gen_boundary_id;
  }

  /* Now handle the case when the shock is not an ENO shock point */
  first_distance=euclidean_distance(shock_point,first_boundary_point);
  find_distance_linear_approx(cedt,shock_point.x,shock_point.y,
        lsh_gen_boundary_id,second_boundary_point);
  second_distance=euclidean_distance(shock_point,*second_boundary_point);

  if (draw){
    point_draw_ps(lsh_gen_point.x,lsh_gen_point.y,RED,data);
    draw_one_boundary_segment_zoom(cedt->boundary,lsh_gen_boundary_id,0,0,BLUE);
    point_draw_ps(second_boundary_point->x,second_boundary_point->y,VIOLET,data);
  }
  if (verbose){
    print_cedt_boundary(cedt->boundary,lsh_gen_boundary_id,lsh_gen_boundary_id+1);
    printf("LBId=%d RBId=%d \n",lsh_gen_boundary_id,rsh_gen_boundary_id);
    printf("Shk=(%f,%f) 1st=(%f,%f) 2nd=(%f,%f)\n",shock_point.x,shock_point.y,
       first_boundary_point.x,first_boundary_point.y,
       second_boundary_point->x,second_boundary_point->y);
    printf("1st=%f 2nd=%f Approx=%f\n",first_distance,second_distance,
       shocks->id[left_shock_id].dist);
    printf("Nbr Labels: L=%d,R=%d\n",
       get_next_label_on_left(cedt->boundary,
                  lsh_gen_boundary_id,lsh_gen_point,&tmp),
       get_next_label_on_right(cedt->boundary,
                   lsh_gen_boundary_id,lsh_gen_point,&tmp));
       
  }
  
  
  /* If the shocks at both endpoints of the shock (GENO) interval came from the
     same boundary segment, we need to look in only one interval for the 
     "shock generating point" of the "shock_point" */
  if (lsh_gen_boundary_id== rsh_gen_boundary_id){
    find_distance_linear_approx(cedt,shock_point.x,shock_point.y,
                lsh_gen_boundary_id,second_boundary_point);
    return lsh_gen_boundary_id;
  }

  /* If the shocks at both endpoints of the shock (GENO) interval came from the
     different boundary segments, scan all the intervening intervals (incl. both
     at the end) and choose the one that gives min. distance */ 
  find_distance_linear_approx(cedt,shock_point.x,shock_point.y,
                  lsh_gen_boundary_id,second_boundary_point);
  second_distance=euclidean_distance(shock_point,*second_boundary_point);
  min_distance=second_distance;
  min_dist_boundary_point.x=second_boundary_point->x;
  min_dist_boundary_point.y=second_boundary_point->y;
  min_dist_boundary_id=lsh_gen_boundary_id;
  
  /*Sometimes there are some BAD intervals (POINTS mostly), that screws up. So need
    to check for that.*/
  if ((cedt->boundary[lsh_gen_boundary_id].right == NOLABEL && 
       cedt->boundary[lsh_gen_boundary_id].left == NOLABEL) ||
      (cedt->boundary[rsh_gen_boundary_id].right == NOLABEL && 
       cedt->boundary[rsh_gen_boundary_id].left == NOLABEL)){
    find_distance_linear_approx(cedt,shock_point.x,shock_point.y,
                lsh_gen_boundary_id,second_boundary_point);
    min_distance=euclidean_distance(shock_point,*second_boundary_point);
    min_dist_boundary_point.x=second_boundary_point->x;
    min_dist_boundary_point.y=second_boundary_point->y;
    find_distance_linear_approx(cedt,shock_point.x,shock_point.y,
                rsh_gen_boundary_id,second_boundary_point);
    second_distance=euclidean_distance(shock_point,*second_boundary_point);
    if (verbose){
      printf("Ldist=%f Rdist=%f\n",min_distance,second_distance);
    }
    if (min_distance < second_distance){
      second_boundary_point->x=min_dist_boundary_point.x;
      second_boundary_point->y=min_dist_boundary_point.y;
      return lsh_gen_boundary_id;
    }
    else{
      return rsh_gen_boundary_id;
    }
  }


  /* Find which way to go along the boundary (cedt). Note: Shocks and boundary need
     not be in the same orientation (and in most cases they are not). 
     To find the orientation on the boundary, use the shock ids on the left and 
     right of the "shock generating point" of left endpoint of the shock interval.*/
  next_rsh_id=get_next_label_on_right(cedt->boundary,lsh_gen_boundary_id,lsh_gen_point,&tmp);
  next_lsh_id=get_next_label_on_left(cedt->boundary,lsh_gen_boundary_id,lsh_gen_point,&tmp);

  if (shock_boundary[left_shock_id].left != NOLABEL)
    true_left_shock_id=shock_boundary[shock_boundary_id].left;
  else
    true_left_shock_id=left_shock_id;
  
  if (next_rsh_id == left_shock_id && next_lsh_id == left_shock_id) 
    return NOLABEL;
  if (next_rsh_id == right_shock_id)
    look_right = 1;
  else if (next_lsh_id == right_shock_id)
    look_left = 1;
  else if (next_rsh_id == true_left_shock_id)
    look_left = 1;
  else if (next_lsh_id == true_left_shock_id)
    look_right = 1;
  else
    return NOLABEL;
  
  if (verbose)
    printf("LOOK-L=%d R=%d \n",look_left,look_right);

  /* Scan the intervals till you hit the boundary id that generated the right 
     endpoint of the shock interval. Choose the interval that gives you minimum
     distance.*/
  second_boundary_id=lsh_gen_boundary_id;
  first_time=1;
  if (look_right){
    while (second_boundary_id != NOLABEL && 
       (first_time || second_boundary_id != lsh_gen_boundary_id) && 
       second_boundary_id != rsh_gen_boundary_id){
      first_time=0;
      second_boundary_id=cedt->boundary[second_boundary_id].right;
      find_distance_linear_approx(cedt,shock_point.x,shock_point.y,
                  second_boundary_id,second_boundary_point);
      second_distance=euclidean_distance(shock_point,*second_boundary_point);
      if (verbose)
    printf("RIGHT:id=%d  1st=%f 2nd=%f \n",second_boundary_id,
           first_distance,second_distance);
      if (min_distance < second_distance){
    /* Do nothing*/
      }
      else{
    min_distance=second_distance;
    min_dist_boundary_point.x=second_boundary_point->x;
    min_dist_boundary_point.y=second_boundary_point->y;
    min_dist_boundary_id=second_boundary_id;
      }
    }
  }
  else if (look_left){
    while (second_boundary_id != NOLABEL && 
       (first_time || second_boundary_id != lsh_gen_boundary_id) && 
       second_boundary_id != rsh_gen_boundary_id){
      first_time=0;
      second_boundary_id=cedt->boundary[second_boundary_id].left;
      find_distance_linear_approx(cedt,shock_point.x,shock_point.y,
                  second_boundary_id,second_boundary_point);
      second_distance=euclidean_distance(shock_point,*second_boundary_point);
      if (verbose)
    printf("LEFT: id=%d 1st=%f 2nd=%f \n",second_boundary_id,
           first_distance,second_distance);
      if (min_distance < second_distance){
    /* Do nothing*/
      }
      else{
    min_distance=second_distance;
    min_dist_boundary_point.x=second_boundary_point->x;
    min_dist_boundary_point.y=second_boundary_point->y;
    min_dist_boundary_id=second_boundary_id;
      }
    }
  }
  second_boundary_point->x=min_dist_boundary_point.x;
  second_boundary_point->y=min_dist_boundary_point.y;
  return min_dist_boundary_id;
}




/*****************************************************************************
  Description: Function that returns the shock generating corresponding to the
               shock label in a given boundary segment
  Input : shock_label - shock label
          Boundary - Boundary structure.
      boundary_id - identifies the boundary segment
  Return: The shock generating point.
*****************************************************************************/
Point get_shock_gen_point(Boundary *boundary, int boundary_id, int shock_label)
{
  ShockLabel *shl;
  Point sh_gen_point;

  sh_gen_point.x=INVALID_POINT;
  sh_gen_point.y=INVALID_POINT;
  
  for (shl=boundary[boundary_id].shock->next;shl;shl=shl->next){
    if (shl->label == shock_label){
      sh_gen_point.x=shl->loc.x;
      sh_gen_point.y=shl->loc.y;
    }
  }
  return sh_gen_point;
}

/***************************************************************************************
   Description : Find the inter-bubble shocks.
   Inputs : spbub_deform - SP_Bubble_Deform structure.

***************************************************************************************/
void find_inter_bubble_shocks(SP_Bubble_Deform *spbub_deform)
{
  
  int height,width;
  int verbose=0;

  /* For Finding Times */
  int time=1;
  FILE *time_fp;
  clock_t iter_clock,start_clock,clock_fnst,clock_fnend;
  double start_time,time_fnst,time_fnend,time_total=0;
  double shock_time=0,pre_proc_time=0,post_proc_time=0;


  height=spbub_deform->params.height;
  width=spbub_deform->params.width;
  convert_boundary_to_linear(spbub_deform->cedt.boundary,
                 spbub_deform->cedt.boundary);

  if (verbose){
    printf("In inter_bubble_shocks\n");
    fflush(stdout);
  }
  printf("CEDT End =%d CEDT Ray End =%d\n",spbub_deform->cedt.boundary_end,
     spbub_deform->cedt.ray_ptr);
 

  free_shock_label_list_all(spbub_deform->cedt.boundary,
                MAX_BOUNDARY_LENGTH);
  free_shock_grids_ptrs(&spbub_deform->shocks,height,width);
 



  inter_boundary_shocks(&spbub_deform->cedt,&spbub_deform->waves,
            &spbub_deform->shocks,1,height,width);
  printf("No of shocks %d\n",spbub_deform->shocks.ptr);



  convert_higher_shocks(&spbub_deform->shocks,spbub_deform->cedt.boundary);
  free_memory_for_hosh(&spbub_deform->shocks,MAX_SHOCK_LENGTH);
  spbub_deform->num_shocks=
    update_shock_labels_on_boundary(&spbub_deform->shocks,&spbub_deform->cedt);
  if (spbub_deform->num_shocks > 0){
    geno_approx_of_shocks(&spbub_deform->shocks,spbub_deform->shock_boundary, 
              spbub_deform->back_shock_ptr,height,width);
  }

}

#ifdef __cplusplus
}
#endif





