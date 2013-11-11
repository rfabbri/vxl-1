/* Delaunay triangulation. */

/* See 
**
**   "Primitives for the Manipulation of General Subdivisions 
**   and the Computation of Voronoi Diagrams"
**
**   L. Guibas, J. Stolfi, ACM TOG, April 1985
**
** Implemented by Jim Roth (DEC CADM Advanced Group) on May 1986.
** Adapted by J. Stolfi on April 1993.
** See the copyright notice at the end of this file.
*/

#ifndef DELAUNAY_NEW_H
#define DELAUNAY_NEW_H

#include "quad.h"

struct site{
    float x, y;
   int id;
};

typedef struct site site_struct;

//typedef int site_ref;

/* Quad-edge data pointers: */

#define ORG(e) ((site_ref) ODATA(e))
#define DEST(e) ((site_ref) DDATA(e))

edge_ref delaunay_build(site_struct sites[], int nsites);

float ccw (site_ref a, site_ref b, site_ref c);

int rightof (site_ref s, edge_ref e);
int leftof (site_ref s, edge_ref e);
int incircle (site_ref a, site_ref b, site_ref c, site_ref d);

edge_ref connect(edge_ref a, edge_ref b);

void sort_sites(site_struct sites[], int nsites);

void rec_delaunay(
    site_struct sites[],  /* The sites */
    int sl, int sh,        /* Consider only sites[sl..sh-1] */
    edge_ref *le,          /* Output: leftmost and */
    edge_ref *re           /*   rightmost edges of traingulation. */
  );


#endif

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
** DISCLAIMER: This software is provided "as is" with no explicit or
** implicit warranty of any kind.  Neither the authors nor their
** employers can be held responsible for any losses or damages
** that might be attributed to its use.
**
** End of copyright notice.
*/
