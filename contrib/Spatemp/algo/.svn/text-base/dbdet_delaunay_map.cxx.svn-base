#include "dbdet_delaunay_map.h"
//#include <vnl/vnl_random.h>

dbdet_delaunay_edge::dbdet_delaunay_edge(unsigned node1_id,unsigned node2_id)
{
    node1_id_=node1_id;
    node2_id_=node2_id;
    weight_=0;
}
dbdet_delaunay_triangle::dbdet_delaunay_triangle(unsigned node1_id,unsigned node2_id,unsigned node3_id)
{
    node1_id_=node1_id;
    node2_id_=node2_id;
    node3_id_=node3_id;
    weight_=0;
    is_grouped_=false;
}
bool dbdet_delaunay_edge::operator==(dbdet_delaunay_edge e2)
{
if(this->node1_id_==e2.node1_id_ && this->node2_id_==e2.node2_id_)
        return true;
    return false;
}
bool dbdet_delaunay_triangle::operator==(dbdet_delaunay_triangle t2)
{
if(this->node1_id_==t2.node1_id_ && this->node2_id_==t2.node2_id_&& this->node3_id_==t2.node3_id_)
        return true;
    return false;
}
struct XYZ{
  double x, y;
  int id;
};




struct ITRIANGLE{
  int p1, p2, p3;
  //double f12n,f23n,f31n;

  
};

struct IEDGE{
  int p1, p2 ;
};

////////////////////////////////////////////////////////////////////////
// CircumCircle() :
//   Return true if a point (xp,yp) is inside the circumcircle made up
//   of the points (x1,y1), (x2,y2), (x3,y3)
//   The circumcircle centre is returned in (xc,yc) and the radius r
//   Note : A point on the edge is inside the circumcircle
////////////////////////////////////////////////////////////////////////

int CircumCircle(double xp, double yp, double x1, double y1, double x2, 
double y2, double x3, double y3, double &xc, double &yc, double &r){
  double m1, m2, mx1, mx2, my1, my2;
  double dx, dy, rsqr, drsqr;

/* Check for coincident points */
if(abs(y1 - y2) < EPSILON && abs(y2 - y3) < EPSILON)
  return(false);
if(abs(y2-y1) < EPSILON){ 
  m2 = - (x3 - x2) / (y3 - y2);
  mx2 = (x2 + x3) / 2.0;
  my2 = (y2 + y3) / 2.0;
  xc = (x2 + x1) / 2.0;
  yc = m2 * (xc - mx2) + my2;
}else if(abs(y3 - y2) < EPSILON){ 
        m1 = - (x2 - x1) / (y2 - y1);
        mx1 = (x1 + x2) / 2.0;
        my1 = (y1 + y2) / 2.0;
        xc = (x3 + x2) / 2.0;
        yc = m1 * (xc - mx1) + my1;
      }else{
         m1 = - (x2 - x1) / (y2 - y1); 
         m2 = - (x3 - x2) / (y3 - y2); 
         mx1 = (x1 + x2) / 2.0; 
         mx2 = (x2 + x3) / 2.0;
         my1 = (y1 + y2) / 2.0;
         my2 = (y2 + y3) / 2.0;
         xc = (m1 * mx1 - m2 * mx2 + my2 - my1) / (m1 - m2); 
         yc = m1 * (xc - mx1) + my1; 
       }
  dx = x2 - xc;
  dy = y2 - yc;
  rsqr = dx * dx + dy * dy;
  r = sqrt(rsqr); 
  dx = xp - xc;
  dy = yp - yc;
  drsqr = dx * dx + dy * dy;
  return((drsqr <= rsqr) ? true : false);
}
int XYZCompare(const void *v1, const void *v2)
{
  XYZ *p1, *p2;
    
  p1 = (XYZ*)v1;
  p2 = (XYZ*)v2;
  if(p1->x < p2->x)
    return(-1);
  else if(p1->x > p2->x)
         return(1);
       else
         return(0);
}

///////////////////////////////////////////////////////////////////////////////
// Triangulate() :
//   Triangulation subroutine
//   Takes as input NV vertices in array pxyz
//   Returned is a list of ntri triangular faces in the array v
//   These triangles are arranged in a consistent clockwise order.
//   The triangle array 'v' should be malloced to 3 * nv
//   The vertex array pxyz must be big enough to hold 3 more points
//   The vertex array must be sorted in increasing x values say
//
//   qsort(p,nv,sizeof(XYZ),XYZCompare);
///////////////////////////////////////////////////////////////////////////////

int Triangulate(int nv, XYZ pxyz[], ITRIANGLE v[], int &ntri){
  int *complete = NULL;
  IEDGE *edges = NULL; 
  IEDGE *p_EdgeTemp;
  int nedge = 0;
  int trimax, emax = 200;
  int status = 0;
  int inside;
  int i, j, k;
  double xp, yp, x1, y1, x2, y2, x3, y3, xc, yc, r;
  double xmin, xmax, ymin, ymax, xmid, ymid;
  double dx, dy, dmax; 

/* Allocate memory for the completeness list, flag for each triangle */
  trimax = 4 * nv;
  emax=trimax;
  complete = new int[trimax];
/* Allocate memory for the edge list */
  edges = new IEDGE[emax];
/*
      Find the maximum and minimum vertex bounds.
      This is to allow calculation of the bounding triangle
*/
  xmin = pxyz[0].x;
  ymin = pxyz[0].y;
  xmax = xmin;
  ymax = ymin;
  for(i = 1; i < nv; i++){
    if (pxyz[i].x < xmin) xmin = pxyz[i].x;
    if (pxyz[i].x > xmax) xmax = pxyz[i].x;
    if (pxyz[i].y < ymin) ymin = pxyz[i].y;
    if (pxyz[i].y > ymax) ymax = pxyz[i].y;
  }
  dx = xmax - xmin;
  dy = ymax - ymin;
  dmax = (dx > dy) ? dx : dy;
  xmid = (xmax + xmin) / 2.0;
  ymid = (ymax + ymin) / 2.0;
/*
   Set up the supertriangle
   his is a triangle which encompasses all the sample points.
   The supertriangle coordinates are added to the end of the
   vertex list. The supertriangle is the first triangle in
   the triangle list.
*/
  pxyz[nv+0].x = xmid - 20 * dmax;
  pxyz[nv+0].y = ymid - dmax;
  pxyz[nv+1].x = xmid;
  pxyz[nv+1].y = ymid + 20 * dmax;
  pxyz[nv+2].x = xmid + 20 * dmax;
  pxyz[nv+2].y = ymid - dmax;
  v[0].p1 = nv;
  v[0].p2 = nv+1;
  v[0].p3 = nv+2;
  complete[0] = false;
  ntri = 1;
/*
   Include each point one at a time into the existing mesh
*/
  for(i = 0; i < nv; i++){
    xp = pxyz[i].x;
    yp = pxyz[i].y;
    nedge = 0;
/*
     Set up the edge buffer.
     If the point (xp,yp) lies inside the circumcircle then the
     three edges of that triangle are added to the edge buffer
     and that triangle is removed.
*/
  for(j = 0; j < ntri; j++){
    if(complete[j])
      continue;
    x1 = pxyz[v[j].p1].x;
    y1 = pxyz[v[j].p1].y;
    x2 = pxyz[v[j].p2].x;
    y2 = pxyz[v[j].p2].y;
    x3 = pxyz[v[j].p3].x;
    y3 = pxyz[v[j].p3].y;
    inside = CircumCircle(xp, yp, x1, y1, x2, y2, x3, y3, xc, yc, r);
    if (xc + r < xp)
// Suggested
// if (xc + r + EPSILON < xp)
      complete[j] = true;
    if(inside){
/* Check that we haven't exceeded the edge list size */
      if(nedge + 3 >= emax){
        emax += 100;
        p_EdgeTemp = new IEDGE[emax];
        for (int i = 0; i < nv; i++){
          p_EdgeTemp[i] = edges[i];   
        }
        delete []edges;
        edges = p_EdgeTemp;
      }
      edges[nedge+0].p1 = v[j].p1;
      edges[nedge+0].p2 = v[j].p2;
      edges[nedge+1].p1 = v[j].p2;
      edges[nedge+1].p2 = v[j].p3;
      edges[nedge+2].p1 = v[j].p3;
      edges[nedge+2].p2 = v[j].p1;
      nedge += 3;
      v[j] = v[ntri-1];
      complete[j] = complete[ntri-1];
      ntri--;
      j--;
    }
  }
/*
  Tag multiple edges
  Note: if all triangles are specified anticlockwise then all
  interior edges are opposite pointing in direction.
*/
  for(j = 0; j < nedge - 1; j++){
    for(k = j + 1; k < nedge; k++){
      if((edges[j].p1 == edges[k].p2) && (edges[j].p2 == edges[k].p1)){
        edges[j].p1 = -1;
        edges[j].p2 = -1;
        edges[k].p1 = -1;
        edges[k].p2 = -1;
      }
       /* Shouldn't need the following, see note above */
      if((edges[j].p1 == edges[k].p1) && (edges[j].p2 == edges[k].p2)){
        edges[j].p1 = -1;
        edges[j].p2 = -1;
        edges[k].p1 = -1;
        edges[k].p2 = -1;
      }
    }
  }
/*
     Form new triangles for the current point
     Skipping over any tagged edges.
     All edges are arranged in clockwise order.
*/
  for(j = 0; j < nedge; j++) {
    if(edges[j].p1 < 0 || edges[j].p2 < 0)
      continue;
    v[ntri].p1 = edges[j].p1;
    v[ntri].p2 = edges[j].p2;
    v[ntri].p3 = i;
    complete[ntri] = false;
    ntri++;
  }
}
/*
      Remove triangles with supertriangle vertices
      These are triangles which have a vertex number greater than nv
*/
  for(i = 0; i < ntri; i++) {
    if(v[i].p1 >= nv || v[i].p2 >= nv || v[i].p3 >= nv) {
      v[i] = v[ntri-1];
      ntri--;
      i--;
    }
  }
  //delete[] edges;
  delete[] complete;
  return 0;
} 












dbdet_delaunay_map::dbdet_delaunay_map(vcl_vector<dbdet_edgel*> edges)
{
    edges_=edges;

    is_included_.resize(edges.size(),true);
    compute_delaunay(edges, is_included_);



}
dbdet_delaunay_map::dbdet_delaunay_map(vcl_vector<dbdet_edgel*> edges,vcl_vector<bool> is_included)
{
    edges_=edges;

    compute_delaunay(edges, is_included);


}

void dbdet_delaunay_map::compute_delaunay(vcl_vector<dbdet_edgel*> edges, vcl_vector<bool> ons)
{
    neighbor_map_.clear();
    neighbor_map_.resize(edges.size(),vcl_vector<int>());
    delaunay_edges_.clear();

    int n=0;
    for(unsigned i=0;i<ons.size();i++)
    {
        if(ons[i])
            n++;
    }
    XYZ * points=new XYZ[n+3];
    for(unsigned i=0,j=0;i<edges.size();i++)
    {
        if(ons[i])
        {
            points[j].x=edges[i]->pt.x();
            points[j].y=edges[i]->pt.y();
            points[j].id=edges[i]->id;
            j++;

        }
    }

    int ntri=0;
    ITRIANGLE * v=new ITRIANGLE[n*3];
    IEDGE * edges1=new IEDGE[n*6];
    //: sort the points by x -value. But this changes the index of edges.

    qsort(points, n, sizeof(XYZ), XYZCompare);
    int nedge=0;

    Triangulate(n, points, v, ntri);

    triangle_map_.resize(edges.size(),vcl_vector<int>());

    //: fill in the edges of the triangle ....
    for(int i=0;i<ntri;i++)
    {
        dbdet_delaunay_triangle t(points[v[i].p1].id,points[v[i].p2].id,points[v[i].p3].id);
        t.id_=triangles_.size();
        triangle_map_[points[v[i].p1].id].push_back(t.id_);
        triangle_map_[points[v[i].p2].id].push_back(t.id_);
        triangle_map_[points[v[i].p3].id].push_back(t.id_);

        triangles_.push_back(t);
    }

    //for(int i=0;i<nedge;i++)
    //{
    //    if(edges1[i].p1==-1 || edges1[i].p2==-1)
    //        continue;
    //    dbdet_delaunay_edge e(points[edges1[i].p1].id,points[edges1[i].p2].id);
    //    delaunay_edges_.push_back(e);

    //}

        //dbdet_delaunay_edge e1(points[v[i].p1].id,points[v[i].p2].id);
        //
        //vcl_vector<dbdet_delaunay_edge>::iterator iter=
        //    vcl_find(delaunay_edges_.begin(),delaunay_edges_.end(),e1);
        //if(iter==delaunay_edges_.end())
        //{
        //    e1.edge_id_=delaunay_edges_.size();
        //    delaunay_edges_.push_back(e1);
        //    neighbor_map_[points[v[i].p1].id].push_back(points[v[i].p2].id);
        //    neighbor_map_[points[v[i].p2].id].push_back(points[v[i].p1].id);

        //}

        //dbdet_delaunay_edge e2(points[v[i].p2].id,points[v[i].p3].id);
        //iter=vcl_find(delaunay_edges_.begin(),delaunay_edges_.end(),e2);
        //if(iter==delaunay_edges_.end())
        //{
        //    e2.edge_id_=delaunay_edges_.size();
        //    delaunay_edges_.push_back(e2);
        //    neighbor_map_[points[v[i].p2].id].push_back(points[v[i].p3].id);
        //    neighbor_map_[points[v[i].p3].id].push_back(points[v[i].p2].id);

        //}
        //dbdet_delaunay_edge e3(points[v[i].p3].id,points[v[i].p1].id);

        //iter=vcl_find(delaunay_edges_.begin(),delaunay_edges_.end(),e3);
        //if(iter==delaunay_edges_.end())
        //{
        //    e3.edge_id_=delaunay_edges_.size();
        //    delaunay_edges_.push_back(e3);
        //    neighbor_map_[points[v[i].p3].id].push_back(points[v[i].p1].id);
        //    neighbor_map_[points[v[i].p1].id].push_back(points[v[i].p3].id);

        //}

    //}

}


void dbdet_delaunay_map::threshold_delaunay_edges(double t)
{
    vcl_vector<dbdet_delaunay_edge>::iterator edge_iter;
    for(edge_iter=delaunay_edges_.begin();edge_iter!=delaunay_edges_.end();)
    {
        if(edge_iter->weight_<t)
        {
            int id1=edge_iter->node1_id_;
            int id2=edge_iter->node2_id_;    
            vcl_vector<int>::iterator iter=vcl_find(neighbor_map_[id1].begin(),neighbor_map_[id1].end(),id2);
            if(iter!=neighbor_map_[id1].end())
                neighbor_map_[id1].erase(iter);
            iter=vcl_find(neighbor_map_[id2].begin(),neighbor_map_[id2].end(),id1);
            if(iter!=neighbor_map_[id2].end())
                neighbor_map_[id2].erase(iter);
            edge_iter=delaunay_edges_.erase(edge_iter); 
        }
        else
        {
            edge_iter++;
        }
        
    }
}

void dbdet_delaunay_map::recompute_delaunay()
{
    
    for(unsigned i=0;i<neighbor_map_.size();i++)
    {
        if(neighbor_map_[i].size()>0)
            is_included_[i]=true;
        else
            is_included_[i]=false;
    }
    compute_delaunay(edges_,is_included_);
}



int dbdet_delaunay_triangle::return_third_id(int id1,int id2)
{
    if(node1_id_!=id1 && node1_id_!=id2)
        return node1_id_;
    if(node2_id_!=id1 && node2_id_!=id2)
        return node2_id_;
    if(node3_id_!=id1 && node3_id_!=id2)
        return node3_id_;
    else
        return -1;

}
