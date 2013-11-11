#ifdef __cplusplus
extern "C" {
#endif

#include "dbseg_subpixel_subpixel_bubbles.h"

#define MULT_LABEL_DIST_TOLERANCE 5E-2

/***************************************************************************
  Description: Scan the shock list and draw  the shocks connected to the
               appropriate boundary segments.
  Input : shocks - The Shocks detected for the boundary
          boundary - The GENO Boundary.

**************************************************************************/
void draw_shock_and_boundary(Shocks *shocks, Boundary *boundary, int draw_line)
{

  int kk,group_id;
  void *data;
  int shock_id,shock_id1,shock_id2;
  int boundary_id1,boundary_id2;
  int xstart=0,ystart=0,color;
  int verbose=0,draw_tangents=0;
  int OFFSET=150;
  Point start_pt,end_pt;

  kk=0; group_id = 1;
  while(shocks->grouplist[kk] != ENDOFSHOCKS) {
    printf("NEW BRANCH\n");
    while(shocks->grouplist[kk] != ENDOFSHGROUP) {
      if (shocks->grouplist[kk+1] != ENDOFSHGROUP) {
    shock_id1 = shocks->grouplist[kk];
    shock_id2 = shocks->grouplist[kk+1];
    color = mod(group_id,9.0);
    start_pt.x=shocks->id[shock_id1].x;
    start_pt.y=shocks->id[shock_id1].y;
    end_pt.x=shocks->id[shock_id2].x;
    end_pt.y=shocks->id[shock_id2].y;
    
    if (start_pt.x >= xstart &&start_pt.x <= xstart+OFFSET &&
        start_pt.y >= ystart && start_pt.y <= ystart+OFFSET &&
        end_pt.x >= xstart && end_pt.x <= xstart+OFFSET &&
        end_pt.y >= ystart && end_pt.y <= ystart+OFFSET) {
      line_draw_ps(shocks->id[shock_id1].x-xstart,shocks->id[shock_id1].y-ystart,
               shocks->id[shock_id2].x-xstart,shocks->id[shock_id2].y-ystart,
               color, data);
      point_draw_ps(shocks->id[shock_id1].x-xstart,shocks->id[shock_id1].y-ystart,
            color,data);
    }
      }
      if (verbose){
    printf("kk=%i, (yf,xf)=(%1.16f, %1.16f) shock_id =%i group_id =%i\n", kk,
           shocks->id[shock_id1].y,shocks->id[shock_id1].x,shock_id1,group_id);
      }
      if (shocks->id[shock_id1].x < 0 || shocks->id[shock_id1].y < 0){
    printf("kk=%i, (yf,xf)=(%1.16f, %1.16f) shock_id =%i group_id =%i\n", kk,
           shocks->id[shock_id1].y,shocks->id[shock_id1].x,shock_id1,group_id);
      }
    
      kk++;
    }
    group_id++;
    kk++; 
  }

  kk=0;
  while(boundary[kk].id != ENDOFBOUNDARY){
    start_pt=get_start_pt(&boundary[kk]);
    end_pt=get_end_pt(&boundary[kk]);
    if (start_pt.x >= xstart &&start_pt.x <= xstart+OFFSET &&
    start_pt.y >= ystart &&start_pt.y <= ystart+OFFSET &&
        end_pt.x >= xstart && end_pt.x <= xstart+OFFSET &&
        end_pt.y >= ystart && end_pt.y <= ystart+OFFSET) 
      draw_one_boundary_segment_zoom(boundary,kk,xstart,ystart,RED);
    if (draw_tangents){
      draw_boundary_tangents_zoom(boundary,kk,xstart,ystart,BLUE);
    }
    kk++;
  }

  kk=0; group_id = 1;
  while(shocks->grouplist[kk] != ENDOFSHOCKS) {
    while(shocks->grouplist[kk] != ENDOFSHGROUP) {
      shock_id = shocks->grouplist[kk];
      boundary_id1=shocks->id[shock_id].boundary_id[0];
      if (draw_line)
    line_draw_ps(shocks->id[shock_id].x-xstart,shocks->id[shock_id].y-ystart,
             shocks->id[shock_id].boundary[0].x-xstart,
             shocks->id[shock_id].boundary[0].y-ystart,color, data);
      start_pt=get_start_pt(&boundary[boundary_id1]);
      if (start_pt.x >xstart &&start_pt.x <xstart+OFFSET &&
      start_pt.y >ystart &&start_pt.y <ystart+OFFSET) 
      draw_one_boundary_segment_zoom(boundary,boundary_id1,xstart,ystart,color+1);
      boundary_id2=shocks->id[shock_id].boundary_id[1];
      if (draw_line)
    line_draw_ps(shocks->id[shock_id].x-xstart,shocks->id[shock_id].y-ystart,
             shocks->id[shock_id].boundary[1].x-xstart,
             shocks->id[shock_id].boundary[1].y-ystart,BLACK, data);
      if (verbose){
    printf("Sh (%1.16f,%1.16f) ",shocks->id[shock_id].x,shocks->id[shock_id].y);
    printf("B1=(%1.16f,%1.16f) ",shocks->id[shock_id].boundary[0].x,
           shocks->id[shock_id].boundary[0].y);
    printf("B2=(%1.16f,%1.16f) \n",shocks->id[shock_id].boundary[1].x,
           shocks->id[shock_id].boundary[1].y);
      }
      start_pt=get_start_pt(&boundary[boundary_id2]);
      end_pt=get_end_pt(&boundary[boundary_id2]);
      if (start_pt.x >=xstart &&start_pt.x <=xstart+OFFSET &&
      start_pt.y >=ystart &&start_pt.y <=ystart+OFFSET &&
        end_pt.x >= xstart && end_pt.x <= xstart+OFFSET &&
        end_pt.y >= ystart && end_pt.y <= ystart+OFFSET) 
    draw_one_boundary_segment_zoom(boundary,boundary_id2,xstart,ystart,color+1);
      kk++;
    }
    kk++;
  }
}

/***************************************************************************
  Description: Scan the shock list and draw  the shocks connected to the
               appropriate boundary segments.
  Input : shocks - The Shocks detected for the boundary
          boundary - The GENO Boundary.

**************************************************************************/
void draw_shock_and_boundary_crop(Shocks *shocks, Boundary *boundary, int draw_line,
                  int xstart, int ystart, int offset)
{

  int kk,group_id;
  void *data;
  int shock_id,shock_id1,shock_id2;
  int boundary_id1,boundary_id2;
  int color;
  int verbose=0,draw_tangents=0;
  int OFFSET=offset;
  Point start_pt,end_pt;

  kk=0; group_id = 1;
  while(shocks->grouplist[kk] != ENDOFSHOCKS) {
    printf("NEW BRANCH\n");
    while(shocks->grouplist[kk] != ENDOFSHGROUP) {
      if (shocks->grouplist[kk+1] != ENDOFSHGROUP) {
    shock_id1 = shocks->grouplist[kk];
    shock_id2 = shocks->grouplist[kk+1];
    color = mod(group_id,9.0);
    start_pt.x=shocks->id[shock_id1].x;
    start_pt.y=shocks->id[shock_id1].y;
    end_pt.x=shocks->id[shock_id2].x;
    end_pt.y=shocks->id[shock_id2].y;
    
    if (start_pt.x >= xstart &&start_pt.x <= xstart+OFFSET &&
        start_pt.y >= ystart && start_pt.y <= ystart+OFFSET &&
        end_pt.x >= xstart && end_pt.x <= xstart+OFFSET &&
        end_pt.y >= ystart && end_pt.y <= ystart+OFFSET) {
      line_draw_ps(shocks->id[shock_id1].x-xstart,shocks->id[shock_id1].y-ystart,
               shocks->id[shock_id2].x-xstart,shocks->id[shock_id2].y-ystart,
               color, data);
      point_draw_ps(shocks->id[shock_id1].x-xstart,shocks->id[shock_id1].y-ystart,
            color,data);
    }
      }
      if (verbose){
    printf("kk=%i, (yf,xf)=(%1.16f, %1.16f) shock_id =%i group_id =%i\n", kk,
           shocks->id[shock_id1].y,shocks->id[shock_id1].x,shock_id1,group_id);
      }
      if (shocks->id[shock_id1].x < 0 || shocks->id[shock_id1].y < 0){
    printf("kk=%i, (yf,xf)=(%1.16f, %1.16f) shock_id =%i group_id =%i\n", kk,
           shocks->id[shock_id1].y,shocks->id[shock_id1].x,shock_id1,group_id);
      }
    
      kk++;
    }
    group_id++;
    kk++; 
  }

  kk=0;
  while(boundary[kk].id != ENDOFBOUNDARY){
    start_pt=get_start_pt(&boundary[kk]);
    end_pt=get_end_pt(&boundary[kk]);
    if (start_pt.x >= xstart &&start_pt.x <= xstart+OFFSET &&
    start_pt.y >= ystart &&start_pt.y <= ystart+OFFSET &&
        end_pt.x >= xstart && end_pt.x <= xstart+OFFSET &&
        end_pt.y >= ystart && end_pt.y <= ystart+OFFSET) 
      draw_one_boundary_segment_zoom(boundary,kk,xstart,ystart,RED);
    if (draw_tangents){
      draw_boundary_tangents_zoom(boundary,kk,xstart,ystart,BLUE);
    }
    kk++;
  }

  kk=0; group_id = 1;
  while(shocks->grouplist[kk] != ENDOFSHOCKS) {
    while(shocks->grouplist[kk] != ENDOFSHGROUP) {
      shock_id = shocks->grouplist[kk];
      boundary_id1=shocks->id[shock_id].boundary_id[0];
      if (draw_line)
    line_draw_ps(shocks->id[shock_id].x-xstart,shocks->id[shock_id].y-ystart,
             shocks->id[shock_id].boundary[0].x-xstart,
             shocks->id[shock_id].boundary[0].y-ystart,color, data);
      start_pt=get_start_pt(&boundary[boundary_id1]);
      if (start_pt.x >xstart &&start_pt.x <xstart+OFFSET &&
      start_pt.y >ystart &&start_pt.y <ystart+OFFSET) 
      draw_one_boundary_segment_zoom(boundary,boundary_id1,xstart,ystart,color+1);
      boundary_id2=shocks->id[shock_id].boundary_id[1];
      if (draw_line)
    line_draw_ps(shocks->id[shock_id].x-xstart,shocks->id[shock_id].y-ystart,
             shocks->id[shock_id].boundary[1].x-xstart,
             shocks->id[shock_id].boundary[1].y-ystart,BLACK, data);
      if (verbose){
    printf("Sh (%1.16f,%1.16f) ",shocks->id[shock_id].x,shocks->id[shock_id].y);
    printf("B1=(%1.16f,%1.16f) ",shocks->id[shock_id].boundary[0].x,
           shocks->id[shock_id].boundary[0].y);
    printf("B2=(%1.16f,%1.16f) \n",shocks->id[shock_id].boundary[1].x,
           shocks->id[shock_id].boundary[1].y);
      }
      start_pt=get_start_pt(&boundary[boundary_id2]);
      end_pt=get_end_pt(&boundary[boundary_id2]);
      if (start_pt.x >=xstart &&start_pt.x <=xstart+OFFSET &&
      start_pt.y >=ystart &&start_pt.y <=ystart+OFFSET &&
        end_pt.x >= xstart && end_pt.x <= xstart+OFFSET &&
        end_pt.y >= ystart && end_pt.y <= ystart+OFFSET) 
    draw_one_boundary_segment_zoom(boundary,boundary_id2,xstart,ystart,color+1);
      kk++;
    }
    kk++;
  }
}


/***************************************************************************
  Description: Scan the boundary and print the shock generating points.
  Input : boundary - The GENO Boundary.

**************************************************************************/
void print_shock_labels(Boundary *boundary)
{
  ShockLabel *shl;
  
  if (boundary->num_shock_gen_points>0)
    printf("%d \n",boundary->id);
  for (shl=boundary->shock->next;shl;shl=shl->next){
    printf("Loc (%f,%f) Label=%d Dist=%f \n",shl->loc.x,shl->loc.y,shl->label,shl->dist);
  }
}

/***************************************************************************
  Description: Scan the boundary and print the shock generating points.
  Input : boundary - The GENO Boundary.

**************************************************************************/
void print_shock_labels_all(Boundary *boundary)
{
  int ii=0;
  ShockLabel *shl;

  while (boundary[ii].id != ENDOFBOUNDARY){
    print_shock_labels(&boundary[ii]);
    ii++;
  }
}


/***************************************************************************
  Description: Find  the boundary id that corresponds to the left and 
               right labels specified by lsh_label and rsh_label. 
           The order might not be consistent, so left and right
           might be flipped.
  Input : shock_boundary - The GENO Boundary approx of shocks.
          boundary - The GENO Boundary (zero-level set).
      lsh_label,rsh_label - the left and right shock labels.
      back_shock_ptr - The array of shock->boundary pointers.
  Return: The shock boundary_id corresponding the shock labels.
**************************************************************************/

int find_correct_shock_boundary_id(Boundary *shock_boundary, Boundary *boundary,
                   int *back_shock_ptr,int lsh_label, int rsh_label,
                   int lb_label,int rb_label)
{
  int verbose=0;
  int boundary_id;
  int *right_shock_labels,*left_shock_labels;
  int num_left_labels,num_right_labels,n,i,j;
  double ldist=-1,rdist=-1;
  ShockLabel *cur;
  Point lsh_gen_point,rsh_gen_point;


  boundary_id=back_shock_ptr[lsh_label];
  if (verbose){
    printf("LshL=%d RshL=%d\n", lsh_label,rsh_label);
    printf("Shock Segment using Left Label as Anchor\n");
    print_cedt_boundary(shock_boundary,boundary_id,boundary_id+1);
  }
  if (shock_boundary[boundary_id].right == rsh_label){
    return boundary_id;
  }
  boundary_id=back_shock_ptr[rsh_label];
  if (verbose){
    printf("LshL=%d RshL=%d\n", lsh_label,rsh_label);
    printf("Shock Segment using Right Label as Anchor\n");
    print_cedt_boundary(shock_boundary,boundary_id,boundary_id+1);
  }
  if (shock_boundary[boundary_id].right == lsh_label){
    return boundary_id;
  }

  /*If it came here, then this is not the straightforward case. While
    updating the shock label lists on the boundary, the shocks coming
    from the same point were added in the wrong order, so now we have
    to find all the shocks that came from the same boundary points and 
    see if we can pick the right shock boundary_id.*/

  if (lb_label == NOLABEL || rb_label == NOLABEL)
    return NOLABEL;

  /*Find the adjacent shock generating points in the two boundary
    segments.*/
  lsh_gen_point=get_shock_gen_point(boundary,lb_label,lsh_label);
  rsh_gen_point=get_shock_gen_point(boundary,rb_label,rsh_label);
  num_left_labels=get_multiple_shock_labels(boundary,lb_label,lsh_gen_point,
                        &left_shock_labels);
  num_right_labels=get_multiple_shock_labels(boundary,rb_label,rsh_gen_point,
                         &right_shock_labels);  

  if (verbose){
    printf("Num_rt_labels=%d Num_left_labels=%d \n",num_right_labels,num_left_labels);
  }
  
  /* Now combine each pair of shock labels and see if we can get the
     right boundary. Need to do both ways as we don't know if the 
     order on the shock and the boundary match. They might be in
     opposite directions.*/
  i=0;j=0;
  if (num_left_labels > 0 && num_right_labels > 0){
    while(left_shock_labels[i] != NOLABEL){
      boundary_id=back_shock_ptr[left_shock_labels[i]];
      j=0;
      while(right_shock_labels[j] != NOLABEL){
    if (shock_boundary[boundary_id].right == right_shock_labels[j]){
      free(left_shock_labels);free(right_shock_labels);
      return boundary_id;
    }
    j++;
      }
      i++;
    }
    
    i=0;j=0;
    while(right_shock_labels[i] != NOLABEL){
      boundary_id=back_shock_ptr[right_shock_labels[i]];
      j=0;
      while(left_shock_labels[j] != NOLABEL){
    if (shock_boundary[boundary_id].right == left_shock_labels[j]){
      free(left_shock_labels);free(right_shock_labels);
      return boundary_id;
    }
    j++;
      }
      i++;
    }
  }
  /*printf("This is TROUBLE\n");
    print_cedt_boundary(shock_boundary,boundary_id,boundary_id+1);*/
  if (num_left_labels > 0)
    free(left_shock_labels);
  if (num_right_labels > 0)
    free(right_shock_labels);
  return NOLABEL;
  
}
/***************************************************************************
  Description: Find multiple shock labels at a given boundary point. Some
               boundary points give rise to more than one shock, and this
           functions finds them. Looks only in the current interval
           for finding the shock labels.
  Inputs: cedt_boundary - Boundary structure.
          boundary_point - boundary_point we are interested in.
      boundary_id - identifies the boundary segment in which the
      boundary_point is.
  Output: shock_labels - array of shock labels.
  Return: the no. of shock labels in the array that is returned.
***************************************************************************/

int get_multiple_shock_labels_intv(Boundary *boundary,int boundary_id,Point boundary_point,
                   int **shock_labels)
{
  Point start_pt,end_pt;
  ShockLabel *shl;
  double dist;
  int  num_labels=0,i;
  int *tmp,verbose=0;

  if (boundary[boundary_id].num_shock_gen_points == 0)
    return 0;
  start_pt = get_start_pt(&boundary[boundary_id]);
  end_pt = get_end_pt(&boundary[boundary_id]);
 
  dist=euclidean_distance(start_pt,boundary_point);
  for (shl=boundary[boundary_id].shock->next;shl;shl=shl->next){
/*     if (shl->dist == dist) */
    if (almost_equal_with_tolerance(shl->dist,dist,MULT_LABEL_DIST_TOLERANCE)){
      num_labels++;
    }
  }
  if (num_labels==0)
    return 0;
  else{
    *shock_labels=(int *)calloc(num_labels+1,sizeof(int));
    tmp=*shock_labels;
    i=0;
    for (shl=boundary[boundary_id].shock->next;shl;shl=shl->next){
      /* if (shl->dist == dist){ */
      if (verbose){
    printf("Shl-Dist=%f Dist=%f\n",shl->dist,dist);
      }
      if (almost_equal_with_tolerance(shl->dist,dist,MULT_LABEL_DIST_TOLERANCE)){
    tmp[i]=shl->label;
    i++;
      }
    }
    tmp[i]=NOLABEL;
    return num_labels;
  }
}
    
/***************************************************************************
  Description: Find multiple shock labels at a given boundary point. Some
               boundary points give rise to more than one shock, and this
           functions finds them. Goes across boundary segments if the
           boundary_point happens to be a start/end point. 
  Inputs: cedt_boundary - Boundary structure.
          boundary_point - boundary_point we are interested in.
      boundary_id - identifies the boundary segment in which the
      boundary_point is.
  Output: shock_labels - array of shock labels.
  Return: the no. of shock labels in the array that is returned.
***************************************************************************/

int get_multiple_shock_labels(Boundary *boundary,int boundary_id,Point boundary_point,
                  int **shock_labels)
{
  Point start_pt,end_pt;
  int next_boundary_id,i;
  int *next_shock_labels,num_next_labels;
  int *curr_shock_labels,num_curr_labels;
  int *tmp,verbose=0;

  /* See if the boundary_point is a start of end point. If that is the case,
     we have to look in neighboring intervals too*/
  start_pt = get_start_pt(&boundary[boundary_id]);
  end_pt = get_end_pt(&boundary[boundary_id]);
  num_next_labels=0;
  if (start_pt.x == boundary_point.x && start_pt.y == boundary_point.y){
    if (boundary[boundary_id].left != NOLABEL){
      next_boundary_id=boundary[boundary_id].left;
      num_next_labels=get_multiple_shock_labels_intv(boundary,next_boundary_id,
                             boundary_point,&next_shock_labels);
    }
  }
  else if (end_pt.x == boundary_point.x && end_pt.y == boundary_point.y){
    if (boundary[boundary_id].right != NOLABEL){
      next_boundary_id=boundary[boundary_id].right;
      num_next_labels=get_multiple_shock_labels_intv(boundary,next_boundary_id,
                             boundary_point,&next_shock_labels);
    }
  }
  num_curr_labels=0;
  num_curr_labels=get_multiple_shock_labels_intv(boundary,boundary_id,boundary_point,
                         &curr_shock_labels);
  if (verbose){
    printf("Num Labels curr=%d next=%d\n",num_curr_labels,num_next_labels);
    for (i=0;i<num_next_labels;i++)
      printf("next =%d\n",next_shock_labels[i]);
    for (i=0;i<num_curr_labels;i++)
      printf("curr=%d\n",curr_shock_labels[i]);
  }
  if (num_next_labels+num_curr_labels > 0){
    *shock_labels=(int *)calloc(num_next_labels+num_curr_labels+1,sizeof(int));
    tmp = *shock_labels;
    for (i=0;i<num_next_labels;i++)
      tmp[i]=next_shock_labels[i];
    for (i=0+num_next_labels;i<num_next_labels+num_curr_labels;i++)
      tmp[i]=curr_shock_labels[i-num_next_labels];
    tmp[num_next_labels+num_curr_labels] = NOLABEL;
  }
  if (num_next_labels > 0)
    free(next_shock_labels);
  if (num_curr_labels > 0)
    free(curr_shock_labels);
  return num_next_labels+num_curr_labels;
}  
/***************************************************************************
  Description: Find the intersection between the shock boundary segment and
               the normal to the boundary.
  Input : boundary - The GENO Boundary approx of shocks.
          boundary_id - The boundary id that is to intersect.
      grid_point,boundary_point  - specify line. 
  Output: Sol - intersecting point (INVALID_POINT,INVALID_POINT) is no valid
          intersection is found.
***************************************************************************/
void find_line_geno_boundary_intersection(Boundary *boundary, int boundary_id,
                      Point grid_point, Point boundary_point,
                      Point *Sol)
{
  int verbose=0,draw=0,line_test=1;
  int inters_flag;
  double slope,ct;
  void *data;
  Point start_pt,end_pt;
  Point Sol2;
  EliArc *Ac;

  start_pt = get_start_pt(&boundary[boundary_id]);
  end_pt = get_end_pt(&boundary[boundary_id]);
  
  if (boundary[boundary_id].type == LINE){
    inters_flag=two_line_intersection(grid_point,boundary_point,start_pt,end_pt,Sol);
    if (inters_flag==0){
      Sol->x=INVALID_POINT;
      Sol->y=INVALID_POINT;
    }
    if (verbose)
      printf("Line: Inters Flag=%d Sol=(%f,%f)\n",inters_flag,Sol->x,Sol->y);
    if (draw){
      point_draw_ps(Sol->x,Sol->y,LIGHT_BLUE,data);
    }
  }
  else if (boundary[boundary_id].type == ARC){
    find_line_equation(grid_point,boundary_point,&slope,&ct);
    Ac = &(boundary[boundary_id].arc);
    inters_flag=find_intersection_arc_line(grid_point,slope,ct,Ac->center_x,Ac->center_y,
                       Ac->radius,start_pt,end_pt,Sol,&Sol2);
    if (inters_flag==0){
      Sol->x=INVALID_POINT;
      Sol->y=INVALID_POINT;
    }
    if (verbose)
      printf("Arc :Inters Flag=%d Sol=(%f,%f) Sol2=(%f,%f)\n",
         inters_flag,Sol->x,Sol->y,Sol2.x,Sol2.y);
    if (draw){
      point_draw_ps(boundary_point.x,boundary_point.y,BLACK,data);
            point_draw_ps(Sol->x,Sol->y,LIGHT_BLUE,data);
      point_draw_ps(Sol2.x,Sol2.y,PURPLE,data);
    }
  }
}

/****************************************************************************
  Description: Initialize the shocks structure of Huseyin.
  Input : Shocks - the shocks structure of Huseyin.
          height,width - image dimensions
***************************************************************************/
void initialize_inter_bubble_shocks(Shocks *shocks,int height,int width)
{
  int x,y,pos;
  
  for(y=0; y<height; y++) {
    for(x=0; x<width; x++) {
      pos = y*width+x;
      shocks->grid[pos].vert = NULL;
      shocks->grid[pos].horiz = NULL;
      shocks->id[pos].group_id = 0;
      shocks->id[pos].del = 0;
      shocks->id[pos].boundary_id[0] = 0;
      shocks->id[pos].boundary_id[1] = 0;
    }
  }

  for (pos=0;pos<height*width;pos++)
    shocks->grouplist[pos]=0;
  
}
/****************************************************************************
  Description: Free the memory allocated to the shock grid pointers.
  Input : Shocks - the shocks structure of Huseyin.
          height,width - image dimensions
***************************************************************************/
void free_shock_grids_ptrs(Shocks *shocks, int height, int width)
{
  int x,y,pos;

  for (y=0;y<height;y++){
    for (x=0;x<width;x++){
      pos=y*width+x;
      if (shocks->grid[pos].vert != NULL){
    free(shocks->grid[pos].vert);
    shocks->grid[pos].vert=NULL;
      }
      if (shocks->grid[pos].horiz != NULL){
    free(shocks->grid[pos].horiz);
    shocks->grid[pos].horiz=NULL;
      }
    }
  }
}

/****************************************************************************
  Description: Free the memory allocated to the higher order shocks.
  Input : Shocks - the shocks structure of Huseyin.
          size - amount of memory allocated to HOSH
***************************************************************************/
void free_memory_for_hosh(Shocks *shocks, int size)
{
  int shock_id;
  int hosh_id;
  Hosh *hosh;

  for (shock_id=0;shock_id<size;shock_id++){
    if (shocks->id[shock_id].order == 4){ 
      hosh_id=shocks->id[shock_id].hosh_id;
      hosh=&shocks->hosh[hosh_id];
      if (hosh->boundary_id != NULL)
    free(hosh->boundary_id);
      if (hosh->speed != NULL)
    free(hosh->speed);
      if (hosh->direction != NULL)
    free(hosh->direction);
      if (hosh->boundary != NULL)
    free(hosh->boundary);
    }
  }
  free(shocks->hosh);
}

#ifdef __cplusplus
}
#endif





