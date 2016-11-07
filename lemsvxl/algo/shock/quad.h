/* Public type and operator definitions for the quad-edge data structure. */

/* See 
**
**   "Primitives for the Manipulation of General Subdivisions 
**   and the Computation of Voronoi Diagrams"
**
**   L. Guibas, J. Stolfi, ACM TOG, April 1985
**
** Originally written by Jim Roth (DEC CADM Advanced Group) on May 1986.
** Modified by J. Stolfi on April 1993.
** See the copyright notice at the end of this file.
*/

#ifndef QUAD_H
#define QUAD_H

/* Edge records: */

typedef int edge_ref;
typedef int site_ref;

struct edge{
    edge_ref next[4];
    site_ref data[4];
    unsigned mark;
};

typedef struct edge edge_struct;
//typedef edge_struct* edge_ref;

/* Edge orientation operators: */

#define ROT(e) (((e)&0xfffffffcu)+(((e)+1)&3u))
#define SYM(e) (((e)&0xfffffffcu)+(((e)+2)&3u))
#define TOR(e) (((e)&0xfffffffcu)+(((e)+3)&3u))

/* Vertex/face walking operators: */

#define ONEXT(e) ((edge_struct *)((e)&0xfffffffcu))->next[(e)&3]
#define ROTRNEXT(e) ((edge_struct *)((e)&0xfffffffcu))->next[((e)+1)&3]
#define SYMDNEXT(e) ((edge_struct *)((e)&0xfffffffcu))->next[((e)+2)&3]
#define TORLNEXT(e) ((edge_struct *)((e)&0xfffffffcu))->next[((e)+3)&3]

#define RNEXT(e) (TOR(ROTRNEXT(e)))
#define DNEXT(e) (SYM(SYMDNEXT(e)))
#define LNEXT(e) (ROT(TORLNEXT(e)))

#define OPREV(e) (ROT(ROTRNEXT(e)))
#define DPREV(e) (TOR(TORLNEXT(e)))
#define RPREV(e) (SYMDNEXT(e))
#define LPREV(e) (SYM(ONEXT(e)))

/* Data pointers: */

#define ODATA(e) ((edge_struct *)((e)&0xfffffffcu))->data[(e)&3]
#define RDATA(e) ((edge_struct *)((e)&0xfffffffcu))->data[((e)+1)&3]
#define DDATA(e) ((edge_struct *)((e)&0xfffffffcu))->data[((e)+2)&3]
#define LDATA(e) ((edge_struct *)((e)&0xfffffffcu))->data[((e)+3)&3]

#define MARK(e)  ((edge_struct *)((e)&0xfffffffcu))->mark

edge_ref make_edge(void);

void destroy_edge(edge_ref e);

void splice(edge_ref a, edge_ref b);

void quad_enum(
    edge_ref a, 
    void visit_proc(edge_ref e, void *closure), 
    void *closure
  );
  /* 
    Enumerates undirected primal edges reachable from $a$.
    
    Calls visit_proc(e, closure) for every edge $e$ that can be reached from
    edge $a$ by a chain of SYM and ONEXT calls; except that exactly one
    of $e$ and SYM(e) is visited. */

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
