/* Delaunay triangulation by straightline divide-and-conquer. */

/* 
** Written by J. Stolfi on april 1993, based on an original
** implementation by Jim Roth (DEC CADM Advanced Group, May 1986).  
** See the copyright notice at the end of this file.
*/ 

#include "delaunay.h"
#include "quad.h"

edge_ref delaunay_build(site_struct sites[], int nsites)
{
  edge_ref le, re;
  sort_sites(sites, nsites);
  rec_delaunay(sites, 0, nsites, &le, &re);
  return (le);
}

/* Shell-sort the sites into x order, breaking ties by y: */

void sort_sites(site_struct sites[], int nsites)
{
  int gap, i, j;
  site_struct tmp;

  for (gap = nsites/2; gap > 0; gap /= 2)
    for (i = gap; i < nsites; i++)
      for ( j = i-gap; 
          j >= 0 && 
          ( sites[j].x != sites[j+gap].x ? 
          (sites[j].x > sites[j+gap].x) : 
          (sites[j].y > sites[j+gap].y)
          );
          j -= gap
          ) 
      {
        tmp =  sites[j]; sites[j] = sites[j+gap]; sites[j+gap] = tmp;
      }
}

/* Connect two vertices with a new edge: */

edge_ref connect(edge_ref a, edge_ref b)
{
  edge_ref e;

  e = make_edge();
  ODATA(e) = DEST(a);
  DDATA(e) = ORG(b);
  splice(e, LNEXT(a));
  splice(SYM(e), b);
  return e;
}

/* Recursively create the Delaunay triangulation of a sorted set of sites. */

void rec_delaunay(
    site_struct sites[],
    int sl, int sh,
    edge_ref *le, edge_ref *re
  )
{
  if (sh == sl+2) 
  {
    edge_ref a = make_edge();
    ODATA(a) = (site_ref)&sites[sl]; 
    DDATA(a) = (site_ref)&sites[sl+1];

    *le = a; *re = SYM(a);
  }
  else if (sh == sl+3) 
  {
    edge_ref a = make_edge();
    edge_ref b = make_edge();
    float ct = ccw((site_ref)&sites[sl], (site_ref)&sites[sl+1], (site_ref)&sites[sl+2]);
    splice(SYM(a), b);

    ODATA(a) = (site_ref)&sites[sl]; 
    DDATA(a) = (site_ref)&sites[sl+1];

    ODATA(b) = (site_ref)&sites[sl+1];  
    DDATA(b) = (site_ref)&sites[sl+2];

    if (ct == 0.0)
    { 
      *le = a; *re = SYM(b); 
    }
    else 
    { 
      edge_ref c = connect(b, a);

      if (ct > 0.0)  { *le = a; *re = SYM(b); }
      else        { *le = SYM(c); *re = c; }
    }
  }
  else
  {
    edge_ref ldo, ldi, rdi, rdo;
    edge_ref basel, lcand, rcand;

    int sm = (sl+sh)/2;

    rec_delaunay(sites, sl, sm, &ldo, &ldi);
    rec_delaunay(sites, sm, sh, &rdi, &rdo);

    while (1) 
    {
      if (leftof(ORG(rdi), ldi)) ldi = LNEXT(ldi);
      else if (rightof(ORG(ldi), rdi)) rdi = ONEXT(SYM(rdi));
      else break;
    }

    basel = connect(SYM(rdi), ldi);
    if (ORG(ldi) == ORG(ldo)) ldo = SYM(basel);
    if (ORG(rdi) == ORG(rdo)) rdo = basel;

    while (1) 
    {
      lcand = ONEXT(SYM(basel));
      if (rightof(DEST(lcand), basel))
        while (incircle(DEST(basel), ORG(basel), DEST(lcand), DEST(ONEXT(lcand)))) 
        { edge_ref t = ONEXT(lcand); destroy_edge(lcand); lcand = t; }

      rcand = OPREV(basel);
      if (rightof(DEST(rcand), basel))
        while (incircle(DEST(basel), ORG(basel), DEST(rcand), DEST(OPREV(rcand)))) 
        { edge_ref t = OPREV(rcand); destroy_edge(rcand); rcand = t; }

      if (!rightof(DEST(lcand), basel) && !rightof(DEST(rcand), basel)) break;

      if ( !rightof(DEST(lcand), basel) ||
         ( rightof(DEST(rcand), basel) && 
         incircle(DEST(lcand), ORG(lcand), ORG(rcand), DEST(rcand))
         )
        )
        basel = connect(rcand, SYM(basel));
      else
        basel = connect(SYM(basel), SYM(lcand));
    }
    *le = ldo; *re = rdo;
  }
}

/* Test if point to right of given edge: */

int rightof(site_ref s, edge_ref e)
{
  return ccw(s, DEST(e), ORG(e)) > 0.0;
}

/* Test if point to left of given edge: */

int leftof(site_ref s, edge_ref e)
{
  return ccw(s, ORG(e), DEST(e)) > 0.0;
}

/* Counterclockwise triangle predicate: */

float ccw(site_ref a, site_ref b, site_ref c)
{
  double x1 = ((site*)a)->x, y1 = ((site*)a)->y;
  double x2 = ((site*)b)->x, y2 = ((site*)b)->y;
  double x3 = ((site*)c)->x, y3 = ((site*)c)->y;

  return (float) ((x2*y3-y2*x3) - (x1*y3-y1*x3) + (x1*y2-y1*x2));
}

/* InCircle predicate: */

int incircle(site_ref a, site_ref b, site_ref c, site_ref d)
{
  double x1 = ((site*)a)->x, y1 = ((site*)a)->y;
  double x2 = ((site*)b)->x, y2 = ((site*)b)->y;
  double x3 = ((site*)c)->x, y3 = ((site*)c)->y;
  double x4 = ((site*)d)->x, y4 = ((site*)d)->y;

  return ((y4-y1)*(x2-x3)+(x4-x1)*(y2-y3))*((x4-x3)*(x2-x1)-(y4-y3)*(y2-y1)) >
  ((y4-y3)*(x2-x1)+(x4-x3)*(y2-y1))*((x4-x1)*(x2-x3)-(y4-y1)*(y2-y3));
}

/*
** Copyright notice:
**
** Copyright 1996 Institute of Computing, Unicamp.
**
** Permission to use this software for any purpose is hereby granted,
** provided that any subsvcl_tantial copy or mechanically derived version
** of this file that is made available to other parties is accompanied
** by this copyright notice in full, and is distributed under these same
** terms. 
**
** NOTE: this copyright notice does not claim to supersede any copyrights
** that may apply to the original DEC implementation of the quad-edge
** data structure.
**
** DISCLAIMER: This software is provided "as is" with no explicit or
** implicit warranty of any kind.  Neither the authors nor their
** employers can be held responsible for any losses or damages
** that might be attributed to its use.
**
** End of copyright notice.
*/
 
