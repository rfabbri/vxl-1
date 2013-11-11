/*--------------------------------------------------------------------
   sp.cpp

   Implementation of Chen & Han's algorithm for computing the
   shortest paths on a polyhedral surface from one point to
   all vertices.

Based on a senior honor's thesis by Biliana Kaneva,
Smith College, April 2000.  Supervised by Joseph O'Rourke.
See our paper, "An Implementation of Chen & Han's Shortest Paths Algorithm."
CCCG August 2000.  Available via http://cs.smith.edu/~orourke/.

Input: 2D triangles + source vertex s.
Output: Shortest paths from s to every other vertex on surface.
See http://cs.smith.edu/~orourke/ShortestPaths/ for a detailed description.

Compile: make (uses gcc)
Run (e.g.):
  sp < s10.in > s10.out

Written by Biliana Kaneva and Joseph O'Rourke.
Last modified: May 2000
Questions to orourke@cs.smith.edu.
---------------------------------------------------------------------*/

/*--------------------------------------------------------------------
This code is Copyright 2000 by Biliana Kaneva & Joseph O'Rourke.  
It may be freely redistributed in its entirety provided that this 
copyright notice is not removed.  Question to orourke@cs.smith.edu.
--------------------------------------------------------------------*/


/*--------------------------------------------------------------------
        sp.cpp - main file
        cone.cpp - cone structure
        cone.h 
        llist.cpp - list class
        llist.h    
        ctree.cpp - cone tree structure 
        ctree.h
        mathops.cpp - math operations
        mathops.h
        structs.h - structures such as Point, Face, Vertex

---------------------------------------------------------------------*/


//------------------------- LIBRARIES -----------------------------------

#include <vcl_iostream.h>
#include <vcl_cctype.h>
#include <vcl_cstdlib.h>
#include <vcl_cmath.h>
///#include <values.h>

#include <vcl_iomanip.h>
#include <vcl_fstream.h>

#include "mathops.h"
#include "llist.h"
#include "ctree.h"
#include <vcl_cstdio.h>
///#include <sys/time.h>
///#include <sys/resource.h>
///#include <unistd.h> //linux

#include <vcl_cfloat.h>
#define MAXFLOAT FLT_MAX

#ifndef M_PI
#define M_PI            3.14159265358979323846f
#endif

// ------------------------- DEFINITIONS --------------------------------

// Uncomment DOUTPUT for debugging output.  Will then produce two
// files: info and count.
#define DOUTPUT
#define CLIP
//#define MAXFLOAT HUGE_VAL
//#define M_PI 3.14159265359
#define Epsilon 0.0000000001

// structure for storing the information about a shortest path
struct SPath {
  double dist;
  NList *path;
  int num;
};


struct Info {
  int sindex;                   // sindex - index of the start vertex in 
                                // the startface - 0,1,2
  int startface;                // index of the source face
  CTree *ctree;                 // pointer to the cone tree
  int num_faces;                
  int num_vertices;
  Face **faces;                 // coordinates of the faces in with edge 
                                // 0,1, or 2 on the x-axis
  double **angles;              // angles of the faces (orientation - 
                                // edge 0 on the x-axis)
  int **adjf;                   // adjacent face with respect to each edge
  int **glarr;                  // gluing arrangements with respect to 
                                // each edge (finds the edge of the adjacent 
                                // face to which we are glued)  
  NList **voccupy;          // the cones currently occupying a vertex
  NList *source;                // list of source vertices (1 if there are 
                                // no reflex vertices)
  NList **foccupy;
  Cone **clevel;                // pointers to each the head of each level 
                                // in the tree
  int level;                    // counts the number of levels
  SPath paths;                  // structure containing a shortest path to 
                                // be outputed
};

MathOps m;
Info d;

#ifdef DOUTPUT
double clips = 0;
double colin = 0;
#endif

#ifdef MEM_COUNT
size_t maxmemory = 0;
size_t maxm = 0;
vcl_ofstream cntout("count", vcl_ios::out | vcl_ios::app); 
#endif

// ---------------------- PROTOTYPES ------------------------------------
void   GetInput();
void   CheckIfCorrect();
void   ReadComments();
void   ThreeCoordinateS();
void   ComputeAngles();
void   RollOut();
void   SetRoot();
void   RollOutOneLevel( Cone *c);
bool   RollROutOne( Cone *c);
bool   RollVOutOne( Cone *c);
bool   RollOutOne( Cone *c);
void   FaceRotate( Cone *c);
void   DisplayPaths();
void   FindShortest();
void   PrintPath();
void   ClipCones( Cone *c, double r1, double r2, double l1, double l2);
double ComputeLength( Cone *c);
double ComputeLengthToSource( Cone *c);
void   DealWithCollinearity(Cone *c, Cone *p, bool both);
void   RemoveMiddleChildren( Cone *c);
void   FreeMemory();


// -------------------------- MAIN --------------------------------------
int main()
{
  


  GetInput();
  //  CheckIfCorrect();            // this works only for convex examples
  ComputeAngles();
  ThreeCoordinateS();

  SetRoot();
  RollOut();
  FindShortest();
  FreeMemory();

#ifdef MEM_COUNT
  struct rusage r;
  getrusage(RUSAGE_SELF, &r);
  cntout << "User time: " << (r.ru_utime.tv_sec) <<vcl_endl;
  cntout << "System time: " << (r.ru_stime.tv_sec) <<vcl_endl;
#endif
  return 0;  
}


// ----------------------------------------------------------------------
// ComputePath - creates a list with the indexes of the faces and the 
// fractions along the edge the path crosses.
// ----------------------------------------------------------------------
void ComputePath( Cone *c)
{
  double temp=0;
  Cone *p, *s;
  Point v;
  p=c;  
  s=c->source;
  
  v=c->f.v[2].p;
  d.paths.num=0;

  // if we are at the source
  if (p->level == 0)
    {
      d.paths.path->insert(p,0, NCONEF);
      d.paths.num++;
    }
  
  // up to level 0 
  while (p->level != 0) 
    {
  
      m.SegSegInt(s->s.p, v, p->f.v[0].p,p->f.v[1].p,temp);     
  
      d.paths.path->insert(p,temp, NCONEF);
      d.paths.num++;
    
      p=p->p;

      // if there was a change in the source (applies to non-convex case)
      if (p->source != s)
        {
          v=s->s.p;
          s=p->source;
        }
    }
  
  
}

// ----------------------------------------------------------------------
// FindShortest - runs though the vertex lists and finds the shortest
// path to each vertex.
// ----------------------------------------------------------------------
void FindShortest()
{

  vcl_cout << d.num_vertices << vcl_endl;
  for (int i=0; i < d.num_vertices; i++)
    {
      double spath = MAXFLOAT, temp = MAXFLOAT;
      //int first=1; 
      Cone *c;
      NCone *p;
      c=NULL;
      
      for (p=d.voccupy[i]->getHead()->next; p!=NULL; p=p->next)
        {
          temp=ComputeLength(p->p);
          if (spath > temp)
            {
              spath=temp;
              c=p->p;
            }
        }
      d.paths.dist = spath;
      
      if (c == NULL)
        {
          vcl_cout << "BOOM compute path" << vcl_endl;
          vcl_cout << "Error in FindShortest()" << vcl_endl;
          vcl_cout << "i=" << i << ":c is null, spath=" << spath << vcl_endl;
          //exit(1);  Keep going to gain more information on problem
        }
      else
        {
          ComputePath(c);
          PrintPath();
        }
      delete d.paths.path;
      d.paths.path = new NList();
    }

  vcl_cout << d.faces[0][d.startface].v[d.sindex].label << vcl_endl;
  
}

// ----------------------------------------------------------------------
// PrintPath - outputs a path list - faces, vertices, and fractions
// ----------------------------------------------------------------------
void PrintPath()
{
  NConeF *p;
  
  vcl_cout << d.paths.num << " ";
      
  for (p=(NConeF *)d.paths.path->getHead()->next; p!=NULL; 
       p=(NConeF *)p->next)
    {
      vcl_cout << p->p->currface << " " << p->p->f.v[0].label 
           << " "<< p->fraction << " ";
      
    }
  vcl_cout << vcl_endl;

}


// ----------------------------------------------------------------------
// RollOut - rolls out each level of the cone tree
// ----------------------------------------------------------------------
void RollOut()
{
  Cone *p, *q;
  int i;
  
  long count;

#ifdef DOUTPUT
  vcl_ofstream ciout("info", vcl_ios::out); 
#endif
  
  for ( i=0; i < d.num_faces-1; i++)
    {
#ifdef DOUTPUT
      clips = 0;
      ciout << i << vcl_endl;
#endif
      for (p=d.clevel[d.level++]->next; p!=NULL; p=p->next)
        {
          // roll out a face
          RollOutOne(p);
          
          // adjust the pointers in the cone tree
          if (p->l != NULL) {
            if (p->r!=NULL)
              {
                p->r->next =d.clevel[d.level]->next;
                if (d.clevel[d.level]->next != NULL)
                  d.clevel[d.level]->next->prev = p->r;
              }
            else if (p->m!=NULL)
              {
                for (q=p->m; q->next!=NULL && q->next->p == p ; q=q->next);
                q->next=d.clevel[d.level]->next;
                if (d.clevel[d.level]->next != NULL)
                  d.clevel[d.level]->next->prev = q;
              }
            else
              {
                p->l->next =d.clevel[d.level]->next;
                if (d.clevel[d.level]->next != NULL)
                  d.clevel[d.level]->next->prev = p->l;
              }
            d.clevel[d.level]->next = p->l;
            p->l->prev = d.clevel[d.level];      
            
          }
          else if (p->m!=NULL )
            {
              if (p->r!=NULL)
                {
                  p->r->next = d.clevel[d.level]->next;
                  if (d.clevel[d.level]->next != NULL)
                    d.clevel[d.level]->next->prev = p->r;
                }
              else
                {
                  for (q=p->m; q->next!=NULL && q->next->p == p ; q=q->next);
                  q->next=d.clevel[d.level]->next;
                  if (d.clevel[d.level]->next != NULL)
                    d.clevel[d.level]->next->prev = q;
                }
              d.clevel[d.level]->next = p->m;
              p->m->prev = d.clevel[d.level];            
            }
          else if (p->r != NULL){
            p->r->next = d.clevel[d.level]->next;
            if (d.clevel[d.level]->next != NULL)
              d.clevel[d.level]->next->prev = p->r;
            d.clevel[d.level]->next = p->r;
            p->r->prev = d.clevel[d.level];      
          }
        }

      if (d.clevel[d.level]->next == NULL)
        break;
#ifdef DOUTPUT
      count = 0;
      for (p=d.clevel[d.level]->next; p!=NULL; p=p->next)
        {
          count++;
        }

      ciout << "Cones :" << count << " Clipped Cones: " << clips << vcl_endl;
      ciout << "Collinear points: " << colin << vcl_endl;
#endif      

    }
#ifdef MEM_COUNT
  count = 0;
  for ( i=0; i < d.num_faces; i++)
    {
      for (p=d.clevel[i]->next; p!=NULL; p=p->next) 
        {
          count++;
        } 
    }

  cntout << "Total number of nodes (with deletion) " << count << vcl_endl;
  cntout << "Maximal memory " << maxm << " bytes " << (maxm*1.0/(1024*1024)) 
         << "Mb" << vcl_endl;
  cntout << "Total memoty before completion " << maxmemory 
         << " bytes " << (maxmemory*1.0/(1024*1024)) << "Mb" << vcl_endl;
#endif
}



// ----------------------------------------------------------------------
// FaceRotate - rotates the face to be rolled out in the plane using
//              angle sums 
// ----------------------------------------------------------------------
void FaceRotate( Cone *c)
{
  int prevf, currf, prevgl_edge;
  Face newf;
  prevf=d.adjf[c->currface][c->gl_edge];
        prevgl_edge = d.glarr[c->currface][c->gl_edge];
  currf = c->currface;
        
  switch (c->rotated) {
  case 0:
    //right rotate
    c->angle_sum+=d.angles[prevf][prevgl_edge];break;
  case 1:
    //left rotate
    c->angle_sum-=d.angles[prevf][(prevgl_edge+1)%3];break;
  }

  
  //rotate face[coord system][face number]
  newf=m.Rotate2(d.faces[c->gl_edge][currf],c->angle_sum);
  
  //translate
  for (int k=0;  k < 3; k++)
          switch (c->rotated){
          case 0:
          case 3:
            newf.v[k].p+=c->f.v[2].p;break;
          case 1:
          case 4:
            newf.v[k].p+=c->f.v[0].p;break;
          case 2:
            newf.v[k].p+=c->f.v[1].p;break;
          }
  
  c->f=newf;
  c->currface=currf;
  
}

// ----------------------------------------------------------------------
// VertexAnglet2PI - check if a vertex is reflex (non-convex input)
// ----------------------------------------------------------------------
int VertexAnglegt2PI(Cone *c, int j)
{
  double angle_sum=0;
  int v_index=0,i, face, oldface=0, gl_edge;
  
  for (i=0; i < 3; i++)
    {
      if (d.faces[0][c->currface].v[i].label == c->f.v[j].label)
        {
          v_index=i;
          break;
        }
    }
  
  face=c->currface;
  gl_edge=(v_index+2)%3;
  do{
    
    if (v_index== -1)
      {  
        break;
      }
    
    if (d.faces[0][face].v[v_index].label != c->f.v[j].label)
      {
        vcl_cout << "BOOM! Vertex " << d.faces[0][face].v[v_index].label 
             << " " << c->f.v[j].label<< vcl_endl;
        
      }  
    
    angle_sum+=(d.angles[face][v_index]);
    oldface=face;
    face=d.adjf[oldface][gl_edge];
    v_index=d.glarr[oldface][gl_edge];
    gl_edge=(v_index + 2) % 3;
    
  }while (face!=c->currface);  
  
  if (v_index == -1)
    {
      face = oldface;
      for (int k = 0; k < 3; k++)
        {
          if (d.glarr[face][k] == -1)
                return (angle_sum > 2*M_PI+ Epsilon);
        }
      
      angle_sum = (3*M_PI)/2;
      // start in the opposite direction
      gl_edge = v_index = (i + 1) % 3;    
      do{    
        
        if (d.faces[0][face].v[v_index].label != c->f.v[j].label)
          {
            vcl_cout << "BOOM! Vertex " << d.faces[0][face].v[v_index].label 
                 << " " << c->f.v[j].label<< vcl_endl;      
          }  
        
        angle_sum+=(d.angles[face][v_index]);
        oldface=face;
        face=d.adjf[oldface][gl_edge];
        v_index= gl_edge = (d.glarr[oldface][gl_edge] + 1) % 3;    
      }while (face!= -1);  
    }
  
  return (angle_sum > 2*M_PI + Epsilon);
    
}

// ----------------------------------------------------------------------
// SetPointers - set the pointers when creating middle children
// ----------------------------------------------------------------------
void SetPointers(Cone *cone, Cone *&conep, Cone *&last, Cone *&lastr, Cone *c, 
                 bool createchild, bool nc)
{
  if (createchild)
    {
      
      if (cone!=NULL) 
        {
          if (nc && (lastr == c))
            lastr = cone;
          if (c->m == NULL)
            {
              conep = last = c->m=cone;
            }
          else
            {
              last->next=cone;
              cone->prev=last;
              conep = last=cone;    
            }
        }
    }
  else
    {
      if (conep != lastr)        
        {
          delete conep;              
#ifdef MEM_COUNT
          maxmemory -= sizeof(Cone);
#endif
        }
      conep = cone;
    }
}


// ----------------------------------------------------------------------
// CreateChildCones - create child cones of a source vertex
// j is the local index of the vertex around which you lay 
// out the faces
// sface - is the index of the start face (the first face to be rolled out)
// eface - is the index of the last face to be rolled out
// ----------------------------------------------------------------------
void CreateChildCones( Cone *c,int j, int sface, int eface)
{
  int currf;
  double angle_sum=c->angle_sum;
  int v_index,  gl_edge, rotate;
  bool createchild = false;  

  d.source->insert(c, NCONE);

  v_index=j;
  currf=c->currface;

  // find the angle by which to rotate the first face to be rolled out
  Cone  *last,  *cone, *lastr, *conep;
  if (c->m)
    {
      for (last = c->m; last->next!=NULL; last= last->next); 
      angle_sum-=(d.angles[currf][v_index]);
      rotate = 4;
    }
  else
  {
    last = c;
    angle_sum+=(d.angles[currf][(v_index+2)%3]);
    rotate = 3;
  }
  gl_edge=(v_index+2)%3;      
  v_index=d.glarr[currf][gl_edge];
  currf=d.adjf[currf][gl_edge];      
  if (c->currface == sface)
    createchild = true;
  
  conep = lastr = last;

  do
    { 
      // if we reached a boundary stop        
      if (currf == -1)
        break;
      cone = new Cone(c,c,d.faces[0][currf].v[(v_index+1)%3], 
                      d.faces[0][currf].v[(v_index+2)%3],
                      currf, v_index,
                      angle_sum,c->level+1, NULL, NULL,conep->f,rotate,1);
      
      FaceRotate(cone);
      // set the correct bounding vertices
      cone->left = cone->f.v[(v_index+1)%3];
      cone->right = cone->f.v[(v_index+2)%3];      
      cone->rotated = 4;    
     
      if (sface == currf)      
        {  
          createchild = true;
          if (conep != lastr)
            {
              delete conep;
#ifdef MEM_COUNT
              maxmemory -= sizeof(Cone);
#endif
            }
        }
     
     
      SetPointers(cone, conep, last, lastr, c, createchild, true);
     
      if (currf == eface)
        break;
      angle_sum-=(d.angles[currf][v_index]);
      gl_edge=(v_index+2)%3;      
      v_index=d.glarr[currf][gl_edge];
      currf=d.adjf[currf][gl_edge];      
      rotate = 4;  // only translate when you call FaceRotate
   }  while (c->currface != currf); // stop if we made a complete loop or
                                    // reached the boundary 

   // if we hit the boundary, perhaps more work is necessary
   if (currf == -1)
     {
       // change the orientation of the start face, so that you can occupy
       // the third vertex there and rollout in the other direction
       createchild = false;
       v_index = j;
       currf = c->currface;
       angle_sum=-M_PI;
       gl_edge = v_index;
       v_index = d.glarr[currf][gl_edge];
       currf = d.adjf[currf][gl_edge];
       bool first = true;
       conep = lastr;
       if (currf != -1)
         {                  
           
           do {
             cone = new Cone(c,c,d.faces[0][currf].v[(v_index+1)%3], 
                             d.faces[0][currf].v[(v_index+2)%3],
                             currf, v_index,
                             angle_sum,c->level+1, NULL, NULL,conep->f,
                             (first)?2 :3,1);

             FaceRotate(cone);
             cone->left = cone->f.v[(v_index+2)%3];
             cone->right = cone->f.v[(v_index+1)%3];
             cone->rotated = 2;      
             if (first)
               first = false;
             
             if (eface == currf)      
               {  
                 createchild = true;
                 if (conep != lastr)
                   {
                     delete conep;
#ifdef MEM_COUNT
                     maxmemory -= sizeof(Cone);
#endif
                   }
                 if (lastr != c->m)
                   lastr = cone;
               }
     
             SetPointers(cone, conep, last, lastr, c, createchild, false);     
             
             if (currf == sface)
               break;
             angle_sum+=(d.angles[currf][(v_index+1)%3]);      
             gl_edge = (v_index+1)%3;
             v_index = d.glarr[currf][gl_edge];
             currf = d.adjf[currf][gl_edge];
           } while (currf != -1);
         }
     
       // roll out the last face
       currf = lastr->currface;
       v_index = (lastr->gl_edge+2)%3;
       angle_sum = (M_PI - d.angles[currf][(v_index+1)%3]);
       cone = new Cone(c,c,d.faces[0][currf].v[(v_index+1)%3], 
                       d.faces[0][currf].v[(v_index+2)%3],
                       currf, v_index,
                       angle_sum,c->level+1, NULL, NULL,lastr->f,2,1);
       
       cone->left = cone->f.v[2];
       cone->right = cone->f.v[0];
       Vertex temp = cone->f.v[2];
       cone->f.v[2] = cone->f.v[1];
       cone->f.v[1] = cone->f.v[0];
       cone->f.v[0] = temp;
       cone->rotated = 0;

     
       if (cone!=NULL) 
         {
           if (c->m == NULL)
             last=c->m=cone;
           else
             {
                last->next=cone;
                cone->prev=last;     
             }
         }
     }  
     
}

//-----------------------------------------------------------------------
// CreateChildren - finds the indices of the first and the last face 
// to be rolled out as middle children and then calls CreateChildCones 
// to create them
//-----------------------------------------------------------------------
void CreateChildren( Cone *c,int j)
{
  int startf, endf, currf;
  double angle_sum=0;
  int v_index,i,  gl_edge;
  
  c->s=c->f.v[j]; // set the source vertex...
  d.source->insert(c, NCONE);
  
  for (i=0; i < 3; i++)
    if (d.faces[0][c->currface].v[i].label == c->f.v[j].label)
      break;
  
  // find the last child
  v_index=i;
  endf=currf=c->currface;
  do{
    gl_edge=v_index%3;
    if (d.faces[0][currf].v[v_index].label != c->f.v[j].label)
      {
        vcl_cout << d.faces[0][currf].v[v_index].label 
             << " " << c->f.v[j].label;
        vcl_cout << currf << vcl_endl;
        vcl_cout << " BOOM! Child end" << vcl_endl;
      }
    
    angle_sum+=(d.angles[currf][v_index]);
    endf=currf;
    currf=d.adjf[endf][gl_edge];
    v_index=(d.glarr[endf][gl_edge]+1)%3;
  }while ((angle_sum <  M_PI -  Epsilon) && currf != -1) ;  

  if ((currf == -1) && (angle_sum < M_PI - Epsilon))
    endf = -1;
  
  // find the first child
  v_index=i;
  angle_sum=0;
  startf=currf=c->currface;
  
  do{
    gl_edge=(v_index+2)%3;
    if (d.faces[0][currf].v[v_index].label != c->f.v[j].label)
      {
        vcl_cout << d.faces[0][currf].v[v_index].label 
             << " " << c->f.v[j].label;
        vcl_cout << currf << vcl_endl;
        vcl_cout << "BOOM! Children start" << vcl_endl;
      }
    
    
    angle_sum+=(d.angles[currf][v_index]);
    startf=currf;
    currf=d.adjf[startf][gl_edge];
    v_index=d.glarr[startf][gl_edge];
  }while ((angle_sum < M_PI - Epsilon) && currf != -1);    


  if (currf == -1)
    {
      // find the last child
      v_index=i;
      startf =currf=c->currface;
      do{
        gl_edge=v_index%3;
        if (d.faces[0][currf].v[v_index].label != c->f.v[j].label)
          {
            vcl_cout << d.faces[0][currf].v[v_index].label 
                 << " " << c->f.v[j].label;
            vcl_cout << currf << vcl_endl;
            vcl_cout << " BOOM! Child end" << vcl_endl;
          }
        
        angle_sum+=(d.angles[currf][v_index]);
        startf=currf;
        currf=d.adjf[startf][gl_edge];
        v_index=(d.glarr[startf][gl_edge]+1)%3;
      }while (currf != -1) ;       
    }
  
  CreateChildCones(c,i,startf,endf);
    
}


//---------------------------------------------------------------------------
// ComputeLength - find the length to the path from vertex occupied by cone 
//                 to the source vertex
//---------------------------------------------------------------------------
double ComputeLength( Cone *c)
{
  double length=0;
  Point end;
  ///Cone *p, *q;

  if (c == d.ctree->root)
     return 0;                              
  end=c->f.v[2].p; 
  
  length+=m.Length(c->source->s.p,end); 
  length+=ComputeLengthToSource(c);
  return length;
}

//------------------------------------------------------------------------
//ComputeLengthToSource - computes the distance from source to source
//------------------------------------------------------------------------
double ComputeLengthToSource( Cone *c)
{
  double length = 0;
  Cone *p, *q;
  
  for (p=c->source->source, q=c->source; q->level!=0; p=p->source, q=q->source)
    {
      length+=m.Length(p->s.p, q->s.p); 
    }
  return length;
}

//------------------------------------------------------------------------
//RemoveMiddleChildren - deletes the middle children of cone
//------------------------------------------------------------------------
void   RemoveMiddleChildren( Cone *c)
{
  Cone *prev, *q, *temp;
  if (c->m !=NULL)
    {
      prev = c->m->prev;
      for (q=c->m; q!=NULL && q->p == c && q != c->r; )
        {
          temp = q->next;
          d.ctree->remove(q, d.voccupy, d.num_vertices,d.source,d.foccupy); 
          q = temp;
        }
      
      c->m=NULL;
      prev->next = q;
      if (q!=NULL)
        q->prev = prev;
    }

}

//------------------------------------------------------------------------
// DealWithCollinearity - if two cones have paths with the same length and 
// one or both of the bounding vertices are collinear with the vertex that
// they are occupying, take one of the following actions:
// a) if both left and right bounding vertices of cone c are collinear
// remove its children, else
// b) if p and c are tied in path length, and p has collinear left and
// right bounding vertices, then remove its children
// c) if p and c are essentially the same cone, i.e. same source and same
// bounding vertices, remove c's children
// 
//------------------------------------------------------------------------
void DealWithCollinearity(Cone *c, Cone *p, bool both)
{
  if (m.Cost(c->source->s.p, c->right.p, c->left.p) > (1-Epsilon))
    {
      d.ctree->remove(c->r, d.voccupy, d.num_vertices,d.source,d.foccupy);
      d.ctree->remove(c->l, d.voccupy, d.num_vertices,d.source,d.foccupy);
    }
  else if (m.Cost(p->source->s.p, p->right.p, p->left.p) > (1 - Epsilon) && 
        both)
    {
      RemoveMiddleChildren(p);
      d.ctree->remove(p->r, d.voccupy, d.num_vertices, d.source,d.foccupy);
      d.ctree->remove(p->l, d.voccupy, d.num_vertices, d.source,d.foccupy);
      if (VertexAnglegt2PI(c,2))
        CreateChildren(c,2);
      d.voccupy[c->f.v[2].label]->insert(c, NCONE);
      
    }
  
  else if ((m.abs(m.Cost(c->source->s.p, c->right.p, c->left.p) -
                  m.Cost(p->source->s.p, p->right.p, p->left.p)) 
            < Epsilon) &&
           (m.abs(c->source->s.p.x - p->source->s.p.x) < Epsilon)&&
           (m.abs(c->source->s.p.y - p->source->s.p.y) < Epsilon)&&
           (m.Area2(c->source->s.p, c->right.p, c->f.v[2].p) == 
            m.Area2(p->source->s.p, p->right.p, p->f.v[2].p))) 
    {
      d.ctree->remove(c->r, d.voccupy, d.num_vertices,d.source,d.foccupy);
      d.ctree->remove(c->l, d.voccupy, d.num_vertices,d.source,d.foccupy);
    }

}

//------------------------------------------------------------------------
//CreateLeftRightChild - create left and right children of a cone, unless
//the edge is a boundary edge
//------------------------------------------------------------------------
void CreateLeftRightChild(Cone *c)
{
  // create the two new cones
  if (d.adjf[c->currface][(c->gl_edge+1)%3] != -1)
    {
      Cone *r=new Cone(c,c->source,c->left, c->right,
                       d.adjf[c->currface][(c->gl_edge+1)%3], 
                       d.glarr[c->currface][(c->gl_edge+1)%3], 
                       c->angle_sum,c->level+1, NULL,NULL,c->f,0,0);
      c->r=r;
      FaceRotate(r);
    }
  else 
    c->r=NULL;
  
  if (d.adjf[c->currface][(c->gl_edge+2)%3] != -1)
    {
    
      Cone *l=new Cone(c,c->source,c->left, c->right,
                       d.adjf[c->currface][(c->gl_edge+2)%3],
                       d.glarr[c->currface][(c->gl_edge+2)%3],
                       c->angle_sum,c->level+1, NULL,NULL,c->f,1,0);
      c->l=l;
      FaceRotate(l);
      l->next=c->r;
    }
  else
    c->l=NULL;
  
  if (c->r !=NULL)
    c->r->prev = c->l;
}

//-------------------------------------------------------------------------
// RollOutROne - rolls out a cone... if it occupies a reflex vertex, creates 
// the list of middle children
//-------------------------------------------------------------------------
bool RollOutROne( Cone *c)
{
  
  //unused double left=0;
  int lchild, rchild;
  NCone *p;
  
  double lnew,lold,znd,zod;
  char znew,zold;
  bool occupied = false;
  
  // create the two new cones
  CreateLeftRightChild(c);
    
  rchild=m.Area2(c->source->s.p,c->right.p,c->f.v[2].p);
  lchild=m.Area2(c->source->s.p,c->f.v[2].p,c->left.p);
  
  // if the cone is an occupier
  if (rchild >= 0 && lchild >= 0)
    {

      occupied = true;
      // check if there is another occupier
      p=d.voccupy[c->f.v[2].label]->find(c->currface);
  
      //set the boundary vertices  
      if (c->l!=NULL)
        c->l->right=c->f.v[2];      
      if (c->r!=NULL)
        c->r->left=c->f.v[2];      
      
      // if there is another occupier, see who is a winner  
      if (p!=NULL)
        {
          
          lnew= ComputeLength(c);
          lold= ComputeLength(p->p);
      
          znew=m.SegSegInt(c->source->s.p,c->f.v[2].p,
                           c->f.v[0].p,c->f.v[1].p,znd);
          
          zold=m.SegSegInt(p->p->source->s.p,p->p->f.v[2].p,
                           p->p->f.v[0].p,p->p->f.v[1].p,zod);
          
          if ((lnew - lold) < (0 - Epsilon)) {  
            
            RemoveMiddleChildren(p->p);
            
            if ((znd - zod) > Epsilon)          
              //delete right child of old occupier
              d.ctree->remove(p->p->r, d.voccupy, d.num_vertices, 
                              d.source,d.foccupy);
            else  if ((zod - znd) > Epsilon)     
              //delete left child of old occupier
              d.ctree->remove(p->p->l, d.voccupy, d.num_vertices,
                              d.source,d.foccupy);

            else
              DealWithCollinearity(p->p, c, false);                 

            if (VertexAnglegt2PI(c,2))
              CreateChildren(c,2);
            d.voccupy[c->f.v[2].label]->insert(c,NCONE);
        
          } else if ((lnew -lold) > Epsilon){    
            if ((znd - zod) > Epsilon)      
              //delete left child of new cone
              d.ctree->remove(c->l, d.voccupy, d.num_vertices,
                              d.source,d.foccupy);   
            else if ((zod - znd) > Epsilon)
              //delete right child of the new 
              d.ctree->remove(c->r, d.voccupy, d.num_vertices,
                              d.source,d.foccupy);

            else
              DealWithCollinearity(c, p->p, false);                 

          } else 
            {
              if ((znd - zod) > Epsilon)
                {
                  d.ctree->remove(p->p->r, d.voccupy, d.num_vertices,
                                  d.source,d.foccupy);    
                  d.ctree->remove(c->l, d.voccupy, d.num_vertices,
                                  d.source,d.foccupy);
                }
              else if ((zod - znd) > Epsilon)
                {
                  d.ctree->remove(p->p->l, d.voccupy, d.num_vertices, 
                                  d.source,d.foccupy);
                  d.ctree->remove(c->r, d.voccupy, d.num_vertices,
                                  d.source,d.foccupy);
                }
              else
                DealWithCollinearity(c, p->p, true);                 
            }          
        }
      else {      
        if (VertexAnglegt2PI(c,2))
          CreateChildren(c,2);
        d.voccupy[c->f.v[2].label]->insert(c, NCONE);        
      }
    }
  else 
    if (rchild >= 0 )
      {
        d.ctree->remove(c->l, d.voccupy, d.num_vertices,d.source,d.foccupy);

#ifdef CLIP
        double r1,r2,l1,l2;
        m.SegSegInt(c->source->s.p, c->right.p, c->f.v[0].p,c->f.v[1].p,r1);
        m.SegSegInt(c->source->s.p, c->right.p, c->f.v[1].p,c->f.v[2].p,r2);   
        m.SegSegInt(c->source->s.p, c->left.p, c->f.v[0].p,c->f.v[1].p,l1);
        m.SegSegInt(c->source->s.p, c->left.p, c->f.v[1].p,c->f.v[2].p,l2);
        d.foccupy[c->currface]->insert(c, r1,r2,l1,l2);
        ClipCones(c,r1,r2,l1,l2);
#endif        
    
      }
    else if (lchild >= 0) 
      {
        d.ctree->remove(c->r, d.voccupy, d.num_vertices,d.source,d.foccupy);
#ifdef CLIP
        double r1,r2,l1,l2;
        m.SegSegInt(c->source->s.p, c->right.p, c->f.v[0].p,c->f.v[1].p,r1);   
        m.SegSegInt(c->source->s.p, c->right.p, c->f.v[2].p,c->f.v[0].p,r2);   
        m.SegSegInt(c->source->s.p, c->left.p, c->f.v[0].p,c->f.v[1].p,l1);
        m.SegSegInt(c->source->s.p, c->left.p, c->f.v[2].p,c->f.v[0].p,l2);
        d.foccupy[c->currface]->insert(c, r1,r2,l1,l2);
        ClipCones(c,r1,r2,l1,l2);
#endif
      }
  
  if (c->m != NULL)
    {
      if (c->l!=NULL)
        c->l->next=c->m;
      c->m->prev=c->l;
      Cone *q;
      for (q=c->m; q->next!=NULL; q=q->next);
      q->next=c->r;
      if (c->r!=NULL)
        c->r->prev=q;
    }

  return occupied;
}

//-------------------------------------------------------------------------
// ComputeIntersections - coumpute the four intersection points between
// cone c and the face it is crossing
//-------------------------------------------------------------------------
void ComputeIntersections( Cone *c, NConeP *p, 
                           Point &pr1,Point &pr2, Point &pl1,Point &pl2)
{
  int i;
  
  for (i = 0; i < 3; i++)    
    if (c->f.v[2].label == d.faces[0][c->currface].v[i].label)
      break;
  
  if (c->l == NULL)
    {
      pr1 = d.faces[0][c->currface].v[(i+1)%3].p + 
        (p->r1*(d.faces[0][c->currface].v[(i+2)%3].p -
                d.faces[0][c->currface].v[(i+1)%3].p));
      pl1 = d.faces[0][c->currface].v[(i+1)%3].p + 
        (p->l1*(d.faces[0][c->currface].v[(i+2)%3].p -
                d.faces[0][c->currface].v[(i+1)%3].p));
      pr2 = d.faces[0][c->currface].v[(i+2)%3].p + 
        (p->r2*(d.faces[0][c->currface].v[(i)%3].p -
                d.faces[0][c->currface].v[(i+2)%3].p));
      pl2 = d.faces[0][c->currface].v[(i+2)%3].p + 
        (p->l2*(d.faces[0][c->currface].v[(i)%3].p -
               d.faces[0][c->currface].v[(i+2)%3].p));
    }
  else if (c->r == NULL)
    {  
      pr1 = d.faces[0][c->currface].v[(i+1)%3].p + 
        (p->r1*(d.faces[0][c->currface].v[(i+2)%3].p -
               d.faces[0][c->currface].v[(i+1)%3].p));
      pl1 = d.faces[0][c->currface].v[(i+1)%3].p + 
        (p->l1*(d.faces[0][c->currface].v[(i+2)%3].p -
                d.faces[0][c->currface].v[(i+1)%3].p));
      pr2 = d.faces[0][c->currface].v[i%3].p + 
        (p->r2*(d.faces[0][c->currface].v[(i+1)%3].p -
                d.faces[0][c->currface].v[i%3].p));
      pl2 = d.faces[0][c->currface].v[(i)%3].p + 
        (p->l2*(d.faces[0][c->currface].v[(i+1)%3].p -
                d.faces[0][c->currface].v[(i)%3].p));
    }    

}

//-------------------------------------------------------------------------
//ClipCones - perform the cone clipping if two very thin cones cross each 
//other on a face and one of them is undoubtedly closer to the source, i.e.
//all of the four intersection points of the two cones are closer to the 
//its source
//-------------------------------------------------------------------------
void ClipCones( Cone *c, double r1, double r2, double l1, double l2)
{
  Point p1[2], p2[2], c1[2], c2[2];
  NConeP *curr =(NConeP *) d.foccupy[c->currface]->getHead()->next;
  ComputeIntersections(c,curr,p1[0],p2[0],p1[1],p2[1]);
  NConeP *tempP;
  int i;
  for (NConeP *p = (NConeP *)curr->next; p!=NULL; )
    {

      ComputeIntersections(p->p,p,c1[0],c2[0],c1[1],c2[1]);
      double t[4], s[4];
      if (m.SegSegInt(p1[0],p2[0],c1[0],c2[0],s[0],t[0]) == '1' &&
          m.SegSegInt(p1[0],p2[0],c1[1],c2[1],s[1],t[1]) == '1' &&
          m.SegSegInt(p1[1],p2[1],c1[0],c2[0],s[2],t[2]) == '1' &&
          m.SegSegInt(p1[1],p2[1],c1[1],c2[1],s[3],t[3]) == '1')
        {
          double plen[4], clen[4];
          
          for (i = 0; i < 4; i++)
            {
              Point temp;
              temp = curr->p1[((i < 2)?0:1)] + 
                (s[i]*(curr->p2[((i < 2)?0:1)] - curr->p1[((i < 2)?0:1)]));
              plen[i] = ComputeLengthToSource( c) + 
                m.Length(c->source->s.p, curr->p1[((i < 2)?0:1)]) +
                m.Length(curr->p1[((i < 2)?0:1)], temp);
              
              temp = p->p1[((i < 2)?0:1)] + 
                (t[i]*(p->p2[((i < 2)?0:1)] - p->p1[((i < 2)?0:1)]));
              clen[i] = ComputeLengthToSource( p->p) + 
                m.Length(p->p->source->s.p, p->p1[((i < 2)?0:1)]) +
                m.Length(p->p1[((i < 2)?0:1)], temp);
            }
          for (i = 0; i < 4; i++)
            if (plen[i] > clen[i])
              break;

          if (i >= 4)
            {
              RemoveMiddleChildren(p->p);
              d.ctree->remove(p->p->l, d.voccupy, d.num_vertices,
                              d.source,d.foccupy);
              d.ctree->remove(p->p->r, d.voccupy, d.num_vertices,
                              d.source,d.foccupy);           
              tempP =(NConeP *) p->next;
              d.foccupy[p->p->currface]->remove(p->p, NCONEP);
#ifdef DOUTPUT 
              clips++;
#endif              
              p=tempP;
            }
          else
            {
              for (i = 0; i < 4; i++)
                {
                  if (plen[i] < clen[i])
                    break;
                }
              
              if (i >= 4)
                {
                  RemoveMiddleChildren(c);
                  d.ctree->remove(c->l, d.voccupy, d.num_vertices,
                                  d.source,d.foccupy);
                  d.ctree->remove(c->r, d.voccupy, d.num_vertices,
                                  d.source,d.foccupy);         
                  d.foccupy[c->currface]->remove(c, NCONEP);
#ifdef DOUTPUT 
              clips++;
#endif
                }
              p=(NConeP *)p->next;

            }
        }
      else
        p = (NConeP *)p->next;
    }  
}

//-------------------------------------------------------------------------
// RollOutVOne - rolls out a cone whose parent occupies a reflex vertex
//               creates only one child (left or right). if the cone occupies
//               a reflex vertex then the function creates the appropriate
//               list of middle children
//-------------------------------------------------------------------------
bool RollOutVOne( Cone *c)
{  
  Cone *child, *q; ///, *s, *prev;  
  NCone *p;
  double lnew, lold;

  
  // depending on what direction the c was rotated, we will create either a 
  // right or a left child
  switch (c->rotated)
    {
    case 4:
      {
        //left child
        if (d.adjf[c->currface][(c->gl_edge+1)%3]!= -1)
          {   
            child = new Cone(c,c->source,c->f.v[2], c->f.v[1],
                             d.adjf[c->currface][(c->gl_edge+1)%3], 
                             d.glarr[c->currface][(c->gl_edge+1)%3], 
                             c->angle_sum,c->level+1, NULL,NULL,c->f,0,0);
            FaceRotate(child);
          }   
        
        else child = NULL;
  
        c->l=child;
        c->r=NULL;  
    }
      break;
    case 2:    
      {
        //rigth child
        if (d.adjf[c->currface][(c->gl_edge+2)%3]!= -1)
          {   
            child = new Cone(c,c->source,c->f.v[0], c->f.v[2],
                             d.adjf[c->currface][(c->gl_edge+2)%3], 
                             d.glarr[c->currface][(c->gl_edge+2)%3], 
                             c->angle_sum,c->level+1, NULL,NULL,c->f,1,0);
            FaceRotate(child);
          }   
        
        else child = NULL;
  
        c->r=child;
        c->l=NULL;  
      }
      break;
    default:
      c->r = NULL;
      c->l = NULL;
    }


  p=d.voccupy[c->f.v[2].label]->find(c->currface);  
  if (p!=NULL)
    {
      lnew = ComputeLength(c);    
      
      lold = ComputeLength(p->p);
      if (lnew < lold)
        {
          RemoveMiddleChildren(p->p);
          if (VertexAnglegt2PI(c,2))
            {
              CreateChildren(c,2);
            }
          
          d.voccupy[c->f.v[2].label]->insert(c, NCONE);  
        }
      
    }
  else
    {
      if (VertexAnglegt2PI(c,2))
        {
          CreateChildren(c,2);
        }
      d.voccupy[c->f.v[2].label]->insert(c, NCONE);    
    }

  if (c->m!=NULL )
    {
      if (c->l!= NULL)
        {
          c->l->next = c->m;
          c->m->prev = c->l;
        }
      else
        c->m->prev = NULL;
      for (q=c->m; q->next!=NULL; q=q->next);
      q->next=c->r;
      if (c->r!=NULL)
        c->r->prev=q;
    }  
  
  return true;
}

// ----------------------------------------------------------------------
// RollOutOne - rolls out one face
// ----------------------------------------------------------------------
bool RollOutOne( Cone *c)
{
  if (c->type == 0)
    // if the parent occupies a convex vertex
    return RollOutROne(c);
  else
    // if the parent occupies reflex vertex
    return RollOutVOne(c);
}


// ----------------------------------------------------------------------
// SetRoot - create the root of the tree and its children
// ----------------------------------------------------------------------
void SetRoot()

{
  d.ctree = new CTree();
  d.ctree->root = new Cone(NULL,NULL,d.faces[d.sindex][d.startface].v[1],
                           d.faces[d.sindex][d.startface].v[2],
                           d.startface,0,0,0, NULL,NULL, 
                           d.faces[d.sindex][d.startface],0,0);
  
  d.ctree->root->f.v[2] = d.ctree->root->f.v[0];
  
  d.ctree->root->s = d.ctree->root->f.v[2];
  d.ctree->root->source = d.ctree->root;
  d.voccupy[d.ctree->root->f.v[2].label]->insert(d.ctree->root, NCONE);
  
  d.ctree->root->m = new Cone(d.ctree->root,
                             d.ctree->root,d.faces[d.sindex][d.startface].v[1],
                              d.faces[d.sindex][d.startface].v[2],
                              d.startface, d.sindex,
                              0,d.ctree->root->level+1, NULL, NULL,
                              d.faces[d.sindex][d.startface],4,1);
  
  d.ctree->root->m->currface = d.startface;
  CreateChildCones( d.ctree->root, d.sindex, 
                    d.adjf[d.startface][(d.sindex+2)%3],
                    d.adjf[d.startface][d.sindex]);
  d.clevel[d.level]->next = d.ctree->root->m;
  d.ctree->root->m->prev = d.clevel[d.level];
  
}


//-------------------------------------------------------------------
//CheckIfCorrect: Checks for correctness of the input
//                1. Are two adjacent faces shown twice?
//                2. Is each vertex glued to its correspondent vertex?
// Note: this function works only for terrains without boundary
//--------------------------------------------------------------------
void CheckIfCorrect()
{
  bool correct=false;
  int i,j,k;
  for ( i=0; i < d.num_faces; i++)
    for (j=0; j < 3; j++)
      {
        correct=false;
        for (k=0; k < 3; k++)
          if (d.adjf[d.adjf[i][j]][k]==i)
            correct=true;
        if (!correct)
          {
            vcl_cout << "Incorrect input Face="<< i<< " Edge="<<j<< vcl_endl;
            exit(0);
          }
      }
    
  for ( i=0; i < d.num_faces; i++)
    for (j=0; j < 3; j++)
      if (d.glarr[d.adjf[i][j]][d.glarr[i][j]]!=j)
        {
          vcl_cout << "Incorrect input Face="<< i << vcl_endl;
          exit(0);
        }
}

//-------------------------------------------------------------------
//GetInput: Gets the input from the stdin.
//          number of faces
//          v0 v1 v2 for each face
//          adjacent face gluing edge for each face
//          source face stource index...
//          Line starting with # sign are ignored
//-------------------------------------------------------------------
void GetInput()
{
  int i,j;
  
  ReadComments();
  vcl_cin >> d.num_faces;
  ReadComments();
  vcl_cin >> d.num_vertices;
  ReadComments();
  d.voccupy= new NList*[d.num_vertices];
  for (i=0; i < d.num_vertices; i++)
    {
      d.voccupy[i]=new NList();   
    }
  d.paths.num=0;
  d.paths.path = new NList();
  // allocate memory for the arrays in the structure
  d.faces=new Face*[3];
  d.glarr=new int*[d.num_faces];
  d.adjf=new int*[d.num_faces];
  d.angles=new double*[d.num_faces];
  d.clevel= new Cone*[d.num_faces+1];
  d.level=0;
  d.source=new NList();
  d.foccupy = new NList*[d.num_faces];
  for (i=0; i < 3; i++)
    d.faces[i]=new Face[d.num_faces];
  
  for ( i=0; i < d.num_faces; i++)
    {
      d.glarr[i]=new int[3];
      d.adjf[i]=new int[3];
      d.angles[i]=new double[3];
      d.clevel[i]=new Cone();
      d.foccupy[i] = new NList();
    }
  
  d.clevel[i]=new Cone();
  // get coordinates of the vertices
  for (i=0; i < d.num_faces; i++)
    vcl_cin >> d.faces[0][i];
  
  ReadComments();
  
  // get gluing instructions
  for ( i=0; i < d.num_faces; i++)
    for ( j=0; j < 3; j++)
      vcl_cin >> d.adjf[i][j] >> d.glarr[i][j];
  
  ReadComments();
  
  //faces to be crossed
  vcl_cin >> d.startface;
  vcl_cin >> d.sindex;
  
}


//-------------------------------------------------------------------
//FreeMemory - free all the dynamically allocated memory
//-------------------------------------------------------------------
void FreeMemory()
{
  int i;

  delete d.ctree;
  for (i=0; i < d.num_vertices; i++)
    {
      delete d.voccupy[i];      

    }

  delete d.paths.path;
  delete [] d.voccupy;

  delete d.source;

  for (i=0; i < 3; i++)
    delete [] d.faces[i];
  
  delete [] d.faces;
  for ( i=0; i < d.num_faces; i++)
    {
      delete [] d.glarr[i];
      delete [] d.adjf[i];
      delete [] d.angles[i];
      delete d.clevel[i];
      delete d.foccupy[i];
    }

  delete d.clevel[i];
  delete [] d.glarr;
  delete [] d.adjf;
  delete [] d.angles;
  delete [] d.clevel;
  delete [] d.foccupy;

}

//-------------------------------------------------------------------
//readcomments: reads until it finds a line starting with something 
//              different than a #-sign
//-------------------------------------------------------------------
void ReadComments()
{
  char line[80];
  
  while (true) {
    vcl_cin >> vcl_ws;
    if (vcl_cin.peek()=='#')
      {
        vcl_cin.get(line, 79, '\n');
        continue;
      }
    return;
    
  }
}



//--------------------------------------------------------------------
//COMPUTEANGLES: computes the angles of all faces, using the 
//               coordinates of each vertex
//--------------------------------------------------------------------
void ComputeAngles()
{
  
  for (int i=0; i < d.num_faces; i++)
    for (int j=0; j < 3; j++)
      d.angles[i][j]=acos(m.Cost(d.faces[0][i].v[j].p, 
                                 d.faces[0][i].v[(j+1)%3].p,
                                 d.faces[0][i].v[(j+2)%3].p));
}

//-------------------------------------------------------------------
//ThreeCoordinateS: computes the coordinates of the vertices of
//                  each face in three coordinate systems, such 
//                  that each of the two vertices are placed on the
//                  x-axis, one of them being at the origin
//-------------------------------------------------------------------
void ThreeCoordinateS()
{
  Face ft;
  int i,j;
  for ( i=1; i < 3; i++)
    for ( j=0; j < d.num_faces; j++)
      {
        ft=d.faces[i-1][j];  
        
        for (int k=0; k < 3; k++)
          ft.v[k].p-=d.faces[i-1][j].v[1].p;
        
        ft=m.Rotate(ft, 
                    m.Sint(d.faces[i-1][j].v[1].p,d.faces[i-1][j].v[0].p,
                           d.faces[i-1][j].v[2].p),
                    0-m.Cost(d.faces[i-1][j].v[1].p,d.faces[i-1][j].v[0].p,
                             d.faces[i-1][j].v[2].p));
        for (int m=0; m< 3; m++)
          d.faces[i][j].v[(m+2)%3]=ft.v[m];
      }
}

