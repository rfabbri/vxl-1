#include "graphics.h"


/*********************************************************************
  Description : Find the slope and constant (y-intercept) of a line
                segment with endpoint a and b.

  Inputs : a,b Endpoints of line segment.
  
  Outputs: slope - slope of line segment joining a,b
           constant - constant of line segment joining a,b

*********************************************************************/

void tr_find_line_equation(Tr_Point a, Tr_Point b,double *slope, double *constant)
{
  double sl,c;

  sl=tr_find_slope(a,b);
  if (fabs(sl) != TR_VERY_LARGE_VALUE)
    c = a.y - sl*a.x;
  else 
    c = a.x;
  *slope = sl;
  *constant= c;
}

void tr_compute_directions(Tr_Point a, Tr_Point b, int *xdir, int *ydir)
{

  double xdiff,ydiff;

  xdiff=a.x-b.x;
  ydiff=a.y-b.y;
  
  if (xdiff > 0.0)
    *xdir = 1;
  else if(xdiff < 0.0)
    *xdir = -1;
  else
    *xdir = 0;
  
  if (ydiff > 0.0)
    *ydir = 1;
  else if(ydiff < 0.0)
    *ydir = -1;
  else
    *ydir = 0;
}
/*********************************************************************
  Description : Finds the type of the given point, whether it lies
                on a horizontal grid line (HORIZ)
                on a vertical grid line (VERT)
                on a grid line intersection (GRID)
                not on a grid line (CELL)

  Inputs : a - Point
  
  Return: pt_type

*********************************************************************/

int tr_xing_type(Tr_Point *Pt) 

{
  double rem_x,rem_y;
  double small_number = 1E-2;

  rem_x = Pt->x - (int) Pt->x; 
  rem_y = Pt->y - (int) Pt->y; 
  
  if (rem_x < small_number && rem_y < small_number)
    return (TR_GRID);
  else if (rem_x > small_number && rem_y > small_number)
    return(TR_CELL);
  else if  (rem_x < small_number) 
    return (TR_VERT);
  else if  (rem_y < small_number)
    return (TR_HORIZ);
    
}

/*********************************************************************
  Description : Find the slope  of a line segment with endpoint a and b.

  Inputs : a,b Endpoints of line segment.
  
  Return: slope - slope of line segment joining a,b

*********************************************************************/
double tr_find_slope(Tr_Point a, Tr_Point b)
{
  double slope;

  if (a.y == b.y)
    slope = 0.0;
  else if (tr_almost_equal(a.x,b.x))
    if (a.y >= b.y)
      slope = -TR_VERY_LARGE_VALUE;
    else
      slope = TR_VERY_LARGE_VALUE;
  else
    slope = (a.y-b.y)/(a.x-b.x);
  
  return slope;
}

int tr_almost_equal(double a, double b)
{
  if (a > b+TR_VERY_SMALL_VALUE)
    return 0;
  else if (a < b-TR_VERY_SMALL_VALUE)
    return 0;
  else 
    return 1;
}

/**********************************************************************
  Description: Free data allocated for tracer.
  Input : Tr - Tracer structure.
**********************************************************************/
void free_data_tracer(Tracer *Tr)
{
  if (Tr->vert != NULL)
    free(Tr->vert);
  if (Tr->horiz != NULL)
    free(Tr->horiz);
  if (Tr->label != NULL)
    free(Tr->label);
  if (Tr->id != NULL)
    free(Tr->id);
  if (Tr->length != NULL)
    free(Tr->length);
}

/**********************************************************************
  Description: Allocate memory for the tracer.
  Input : Tr - Tracer structure.
          size - size of memory to be allocated.
**********************************************************************/
void allocate_data_tracer(Tracer *Tr, int size)
{
  Tr->vert=(double *)calloc(size,sizeof(double));
  Tr->horiz=(double *)calloc(size,sizeof(double));
  Tr->label=(int *)calloc(size,sizeof(int));
  Tr->id=(int *)calloc(size,sizeof(int));
  Tr->length = (int *) calloc(1000, sizeof(int));
}
