//: 050805 MingChing Chang

#include <vcl_algorithm.h>
#include <vgl/vgl_vector_2d.h>

#include <mesh_tri/mesh_tri.h>

#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/algo/dbmsh3d_fmm.h>


//: Compute the gradient at given point (x, y) in local frame.
//  return value in (dx, dy)
//  \param  v0 [dbmsh3d_fmm_vertex_3d&] 1st vertex of local frame.
//  \param  v1 [dbmsh3d_fmm_vertex_3d&] 2nd vertex.
//  \param  v2 [dbmsh3d_fmm_vertex_3d&] 3rd vertex.
//  \param  x [float] x local coord.
//  \param  y [float] y local coord.
//  \param  dx [float&] x coord of the gradient in local coord.
//  \param  dy [float&] y coord of the gradient in local coord.
void dbmsh3d_fmm_face::compute_gradient (dbmsh3d_fmm_vertex_3d* v0, 
                                         dbmsh3d_fmm_vertex_3d* v1, 
                                         dbmsh3d_fmm_vertex_3d* v2, 
                                         float x, float y, 
                                         float& dx, float& dy)
{
  float d0 = v0->dist();
  float d1 = v1->dist();
  float d2 = v2->dist();

  // compute gradient
  // The gradient in direction (e1,e2) is:  
  //        |<grad(d),e0>|   |(d0-d2)/l0|   |gu|
  //    D = |<grad(d),e1>| = |(d1-d2)/l1| = |gv|
  //  We are searching for grad(d) = dx e0 + dy e1 wich gives rise to the system :
  //    | 1  dot|   |dx|
  //    |dot  1 | * |dy| = D      where dot=<e0,e2>
  //  i.e. it is:  
  //    1/det  *  |  1 -dot|*|gu|
  //              |-dot  1 | |gv|
  vgl_vector_3d<double> e0 = v0->pt() - v2->pt();
  vgl_vector_3d<double> e1 = v1->pt() - v2->pt();
  float l0 = (float) e0.length();
  float l1 = (float) e1.length();
  e0 /= l0;
  e1 /= l1;
  float dot = (float) dot_product (e0,e1);
  float det = 1-dot*dot;
  assert( det!=0 );
  float gu = (d0-d2)/l0;
  float gv = (d1-d2)/l1; 
  dx = 1/det * (     gu - dot*gv  );
  dy = 1/det * (-dot*gu +     gv  );
}

//: Compute the distance value at a given location (x,y) in the triangle
//  using the barycentric coordinate (bilinear interpolation).
float dbmsh3d_fmm_face::compute_value (dbmsh3d_fmm_vertex_3d* v0, 
                                       dbmsh3d_fmm_vertex_3d* v1, 
                                       dbmsh3d_fmm_vertex_3d* v2, 
                                       float x, float y)
{
  return v0->dist()*x + v1->dist()*y + v2->dist()*(1-x-y);
}

// ###################################################################

void dbmsh3d_fmm_mesh::run_fmm ()
{
  // main loop
  while ( !perform_fmm_one_step() ) { 
  }
}

// return True if distance of the 1st mesh is < to the one of the 2nd.
static bool _compare_vertex_distance (dbmsh3d_fmm_vertex_3d* pVert1, dbmsh3d_fmm_vertex_3d* pVert2)
{
  return pVert1->dist() > pVert2->dist();
}

void dbmsh3d_fmm_mesh::setup_fast_marching ()
{
  vcl_make_heap (active_vertex_heap_.begin(), active_vertex_heap_.end(), _compare_vertex_distance);
}

void dbmsh3d_fmm_mesh::add_source_vertex (dbmsh3d_fmm_vertex_3d* input_vertex)
{
  source_vertices_.push_back (input_vertex);

  input_vertex->set_source_of_front (input_vertex);
  input_vertex->set_dist (0);
  input_vertex->set_state (dbmsh3d_fmm_vertex_3d::STATE_ALIVE);
  active_vertex_heap_.push_back (input_vertex);
}

// return a bool, if the marching process finishes.
bool dbmsh3d_fmm_mesh::perform_fmm_one_step()
{
  if (active_vertex_heap_.empty())
    return true;
  
  dbmsh3d_fmm_vertex_3d* pCurVert = active_vertex_heap_.front();
  assert (pCurVert!=NULL);
  vcl_pop_heap (active_vertex_heap_.begin(), active_vertex_heap_.end(), _compare_vertex_distance);
  active_vertex_heap_.pop_back();
  pCurVert->set_state (dbmsh3d_fmm_vertex_3d::STATE_DEAD);

  //: iterate through each one-ring neighborhood vertex of this pCurVert.
  //  go through each dbmsh3d_vertex::edges_[] and look for the other vertex
  for (dbmsh3d_ptr_node* cur = pCurVert->E_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();

    dbmsh3d_fmm_vertex_3d* pNewVert = (dbmsh3d_fmm_vertex_3d*) E->other_V (pCurVert);  
    assert (pNewVert!=NULL);

    //: Compute it's new distance using neighborhood information 
    float rNewDistance = GW_INFINITE;

    //: iterate through pNewVert's incident faces
    vcl_set<dbmsh3d_face*> incident_faces;
    pNewVert->get_incident_Fs (incident_faces);

    vcl_set<dbmsh3d_face*>::iterator fit =incident_faces.begin();
    for (; fit != incident_faces.end(); fit++) {
      dbmsh3d_face* pFace = (*fit);

      //: get the 3 vertices of the neighboring triangle: pNewVert, pVert1, pVert2
      assert (pFace!=NULL);
      dbmsh3d_fmm_vertex_3d* pVert1 = (dbmsh3d_fmm_vertex_3d*) pFace->next_bnd_V (pNewVert);
      assert (pVert1!=NULL);
      dbmsh3d_fmm_vertex_3d* pVert2 = (dbmsh3d_fmm_vertex_3d*) pFace->next_bnd_V (pVert1);
      assert (pVert2!=NULL);

      if (pVert1->dist() > pVert2->dist()) { //swap
        dbmsh3d_fmm_vertex_3d* temp_vertex = pVert1;
        pVert1 = pVert2;
        pVert2 = temp_vertex;
      }

      float cur_distance = compute_vertex_distance (pFace, pNewVert, pVert1, pVert2, pCurVert->source_of_front());
      rNewDistance = vcl_min (rNewDistance, cur_distance);
    }

    switch (pNewVert->state()) {
    case dbmsh3d_fmm_vertex_3d::STATE_FAR:
      pNewVert->set_dist (rNewDistance);
      /* add the vertex to the heap */
      active_vertex_heap_.push_back (pNewVert);
      vcl_push_heap (active_vertex_heap_.begin(), active_vertex_heap_.end(), _compare_vertex_distance);
      /* this one can be added to the heap */
      pNewVert->set_state (dbmsh3d_fmm_vertex_3d::STATE_ALIVE);
      pNewVert->set_source_of_front (pCurVert->source_of_front());
    break;
    case dbmsh3d_fmm_vertex_3d::STATE_ALIVE:
      /* just update it's value */
      if (rNewDistance <= pNewVert->dist()) {
        /* possible overlap with old value */
        if (pCurVert->source_of_front() != pNewVert->source_of_front())
          pNewVert->RecordOverlap (pNewVert->source_of_front(), pNewVert->dist() );
        pNewVert->set_dist (rNewDistance);
        pNewVert->set_source_of_front (pCurVert->source_of_front());
        // hum, check if we can correct this (avoid recomputing the whole heap).
        vcl_make_heap (active_vertex_heap_.begin(), active_vertex_heap_.end(), _compare_vertex_distance);
      }
      else {
        /* possible overlap with new value */
        if (pCurVert->source_of_front() != pNewVert->source_of_front())
          pNewVert->RecordOverlap (pCurVert->source_of_front(), rNewDistance );
      }
      break;
    case dbmsh3d_fmm_vertex_3d::STATE_DEAD:
      /* inform the user if there is an overlap */
      if (pCurVert->source_of_front() != pNewVert->source_of_front())
        pNewVert->RecordOverlap (pCurVert->source_of_front(), rNewDistance );
      break;
    default:
      assert (false);
    }
  }

  //: is the fast marching end?
  return active_vertex_heap_.empty();
}

/*------------------------------------------------------------------------------*/
// Name : GW_GeodesicMesh::ComputeVertexDistance
/**
*  \param  CurrentVertex [dbmsh3d_fmm_vertex_3d&] The vertex to update.
*  \param  Vert1 [dbmsh3d_fmm_vertex_3d&] It's 1st neighbor.
*  \param  Vert2 [dbmsh3d_fmm_vertex_3d&] 2nd vertex.
*  \return The distance according to this triangle contribution.
* 
*  Compute the update of a vertex from inside of a triangle.
*/
/*------------------------------------------------------------------------------*/

float dbmsh3d_fmm_mesh::compute_vertex_distance (dbmsh3d_face* CurrentFace, 
                                                 dbmsh3d_fmm_vertex_3d* CurrentVertex, 
                                                 dbmsh3d_fmm_vertex_3d* Vert1, 
                                                 dbmsh3d_fmm_vertex_3d* Vert2, 
                                                 dbmsh3d_vertex* CurrentFront)
{  
  if (Vert1->state()!=dbmsh3d_fmm_vertex_3d::STATE_FAR ||
      Vert2->state()!=dbmsh3d_fmm_vertex_3d::STATE_FAR)  {
    vgl_vector_3d<double> Edge1 = Vert1->pt() - CurrentVertex->pt();
    float b = (float) Edge1.length();
    Edge1 /= b;
    vgl_vector_3d<double> Edge2 = Vert2->pt() - CurrentVertex->pt();
    float a = (float) Edge2.length();
    Edge2 /= a;

    float d1 = Vert1->dist();
    float d2 = Vert2->dist();

    bool bVert1Usable = Vert1->state()==dbmsh3d_fmm_vertex_3d::STATE_DEAD && 
                        Vert1->source_of_front()==CurrentFront;
    bool bVert2Usable = Vert2->state()==dbmsh3d_fmm_vertex_3d::STATE_DEAD && 
                        Vert2->source_of_front()==CurrentFront;
    if (!bVert1Usable && bVert2Usable) {
      /* only one point is a contributor */
      return d2 + a * WAVESPEEDF;
    }
    if (bVert1Usable && !bVert2Usable)
    {
      /* only one point is a contributor */
      return d1 + b * WAVESPEEDF;
    }
    if (bVert1Usable && bVert2Usable)  {
      float dot = (float) dot_product (Edge1, Edge2); ///Edge1*Edge2;

      /*  you can choose wether to use Sethian or my own derivation of the equation. */

      /* first special case for obtuse angles */
      if (dot<0) {
        float c, dot1, dot2;
        dbmsh3d_fmm_vertex_3d* pVert = unfold_triangle (CurrentFace, CurrentVertex, Vert1, Vert2, c, dot1, dot2);
        if (pVert!=NULL && pVert->state()!=dbmsh3d_fmm_vertex_3d::STATE_FAR) {
          float d3 = (float) pVert->dist();
          float t;    // newly computed value
          /* use the unfolded value */
          t = compute_update_sethian_method (d1, d3, c, b, dot1);
          t = vcl_min( t, compute_update_sethian_method (d3, d2, a, c, dot2) );

          return t;
        }
      }

      return compute_update_sethian_method (d1, d2, a, b, dot);
    }
  }

  return GW_INFINITE;
}


/*------------------------------------------------------------------------------*/
// compute_update_sethian_method
/*
 *  \param  d1 [float] Distance value at 1st vertex.
 *  \param  d2 [float] Distance value at 2nd vertex.
 *  \param  a [float] Length of the 1st edge.
 *  \param  b [float] Length of the 2nd edge.
 *  \param  dot [float] Value of the dot product between the 2 edges.
 *  \return [float] The update value.
 * 
 *  Compute the update value using Sethian's method.
 */
/*------------------------------------------------------------------------------*/
float dbmsh3d_fmm_mesh::compute_update_sethian_method (float d1, float d2, float a, float b, float dot)
{
  float t = GW_INFINITE;

  float rCosAngle = dot;
  float rSinAngle = vcl_sqrt( 1-dot*dot );

  // Sethian method
  float u = d2-d1;    // T(B)-T(A)
  float f2 = a*a+b*b-2*a*b*rCosAngle;
  float f1 = b*u*(a*rCosAngle-b);
  float f0 = b*b*(u*u-WAVESPEEDF*WAVESPEEDF*a*a*rSinAngle*rSinAngle);

  // discriminant of the quartic equation
  float delta = f1*f1 - f0*f2;

  if (delta>=0) {
    if (vcl_fabs(f2)>GW_EPSILON) {
      // there is a solution
      t = (-f1 - vcl_sqrt(delta) )/f2;
      // test if we must must choose the other solution
      if (t<u || 
          b*(t-u)/t < a*rCosAngle ||
          a/rCosAngle < b*(t-u)/t) {
        t = (-f1 + vcl_sqrt(delta) ) / f2;
      }
    }
    else {
      // this is a 1st degree polynom
      if (f1!=0)
        t = - f0/f1;
      else
        t = -GW_INFINITE;
    }
  }
  else {
    t = -GW_INFINITE;
  }

  // choose the update from the 2 vertex only if upwind criterion is met
  if (u<t &&
      a*rCosAngle < b*(t-u)/t &&
      b*(t-u)/t < a/rCosAngle) {
    return t+d1;
  }
  else {
    return vcl_min(b*WAVESPEEDF+d1,a*WAVESPEEDF+d2);
  }
}


/*------------------------------------------------------------------------------*/
// Name : GW_GeodesicMesh::UnfoldTriangle
/**
 *  \param  CurFace [dbmsh3d_face&] Vertex to update.
 *  \param  vert [dbmsh3d_fmm_vertex_3d&] Current F.
 *  \param  vert1 [dbmsh3d_fmm_vertex_3d&] 1st neighbor.
 *  \param  vert2 [dbmsh3d_fmm_vertex_3d&] 2nd neighbor.
 *  \return [dbmsh3d_fmm_vertex_3d*] The vertex.
 * 
 *  Find a correct vertex to update \c v.
 */
/*------------------------------------------------------------------------------*/

#define MAX_TRI_UNFOLD 50

dbmsh3d_fmm_vertex_3d* dbmsh3d_fmm_mesh::unfold_triangle (dbmsh3d_face* CurFace, 
                                                          dbmsh3d_fmm_vertex_3d* vert, 
                                                          dbmsh3d_fmm_vertex_3d* vert1, 
                                                          dbmsh3d_fmm_vertex_3d* vert2, 
                                                          float& dist, 
                                                          float& dot1, 
                                                          float& dot2)
{
  vgl_point_3d<double> v  = vert->pt();
  vgl_point_3d<double> v1 = vert1->pt();
  vgl_point_3d<double> v2 = vert2->pt();

  vgl_vector_3d<double> e1 = v1-v;
  float rNorm1 = (float) e1.length();
  e1 /= rNorm1;

  vgl_vector_3d<double> e2 = v2-v;
  float rNorm2 = (float) e2.length();
  e2 /= rNorm2;

  float dot = (float) dot_product (e1, e2);
  assert (dot<0);

  // the equation of the lines defining the unfolding region [e.g. line 1 : {x ; <x,eq1>=0} ]
  vgl_vector_2d<double> eq1 = vgl_vector_2d<double>( dot, vcl_sqrt(1-dot*dot) );
  vgl_vector_2d<double> eq2 = vgl_vector_2d<double>(1,0);

  // position of the 2 points on the unfolding plane
  vgl_vector_2d<double> x1 (rNorm1, 0);
  vgl_vector_2d<double> x2 = eq1*rNorm2;

  // keep track of the starting point
  vgl_vector_2d<double> xstart1 = x1;
  vgl_vector_2d<double> xstart2 = x2;

  dbmsh3d_fmm_vertex_3d* pV1 = vert1;
  dbmsh3d_fmm_vertex_3d* pV2 = vert2;
  dbmsh3d_face* pCurFace = CurFace->m2t_nbr_face_against_vertex (vert);
  

  unsigned int nNum = 0;
  while (nNum<MAX_TRI_UNFOLD && pCurFace!=NULL) {
    dbmsh3d_fmm_vertex_3d* pV = (dbmsh3d_fmm_vertex_3d*) pCurFace->t_3rd_vertex (pV1, pV2);
    assert (pV!=NULL);

    e1 = pV2->pt() - pV1->pt();
    float rNorm1 = (float) e1.length();
    e1 /= rNorm1;
    e2 = pV->pt() - pV1->pt();
    float rNorm2 = (float) e2.length();
    e2 /= rNorm2;
    // compute the position of the new point x on the unfolding plane (via a rotation of -alpha on (x2-x1)/rNorm1 )
    //      | cos(alpha) sin(alpha)|
    //  x = |-sin(alpha) cos(alpha)| * [x2-x1]*rNorm2/rNorm1 + x1   where cos(alpha)=dot
    //
    vgl_vector_2d<double> vv = (x2 - x1)*rNorm2/rNorm1;
    dot = (float) dot_product (e1, e2);

    ///vgl_vector_2d<double> x = vv.Rotate( -acos(dot) ) + x1;
    float a = -acos(dot);
    vgl_vector_2d<double> x (cos(a)*vv.x() - sin(a)*vv.y(), sin(a)*vv.x() + cos(a)*vv.y());
    x = x + x1;

    // compute the intersection points.
    // We look for x=x1+lambda*(x-x1) or x=x2+lambda*(x-x2) with <x,eqi>=0
    float lambda11 = - dot_product(x1,eq1) / ( dot_product(x-x1,eq1) );  // left most 
    float lambda12 = - dot_product(x1,eq2) / ( dot_product(x-x1,eq2) );  // right most
    float lambda21 = - dot_product(x2,eq1) / ( dot_product(x-x2,eq1) );  // left most 
    float lambda22 = - dot_product(x2,eq2) / ( dot_product(x-x2,eq2) );  // right most
    bool bIntersect11 = (lambda11>=0) && (lambda11<=1);
    bool bIntersect12 = (lambda12>=0) && (lambda12<=1);
    bool bIntersect21 = (lambda21>=0) && (lambda21<=1);
    bool bIntersect22 = (lambda22>=0) && (lambda22<=1);
    if (bIntersect11 && bIntersect12) {
      /* we should unfold on edge [x x1] */
      pCurFace = pCurFace->m2t_nbr_face_against_vertex (pV2);
      pV2 = pV; 
      x2 = x;
    }
    else if (bIntersect21 && bIntersect22) {
      /* we should unfold on edge [x x2] */
      pCurFace = pCurFace->m2t_nbr_face_against_vertex (pV1);
      pV1 = pV; 
      x1 = x;
    }
    else {
      assert (bIntersect11 && !bIntersect12 && !bIntersect21 && bIntersect22);
      /* that's it, we have found the point */
      dist = x.length();
      dot1 = dot_product(x,xstart1) / (dist * xstart1.length());
      dot2 = dot_product(x,xstart2) / (dist * xstart2.length());
      return pV;
    }
    nNum++;
  }

  return NULL;
}

// ################################################################################

#define MAX_GEODESIC_TRACK 5000

//: back-tracking of the FMM for the geodesic shortest path from a given vertex.
void fmm_track_geodesic::back_track_geodesic (dbmsh3d_vertex* ending_vertex)
{
  //: reset the tracing structure.
  track_cur_face_ = NULL;
  geodesic_path_.clear();

  //: add the ending vertex to the geodesic_path
  add_vertex_compute_next_face (ending_vertex);

  unsigned n_link = 0;
  while (add_new_geodesic_point()==0) { 
    n_link++;

    if (n_link > MAX_GEODESIC_TRACK) {
      assert (0);
      break;
    }
  }
}

void fmm_track_geodesic::get_geodesic_polyline (vcl_vector<vgl_point_3d<double> >& geodesic_points)
{
  //: put the geodesic points to return
  for (unsigned int i=0; i<geodesic_path_.size(); i++) {
    dbmsh3d_fmm_geodesic_point* gp = geodesic_path_[i];
    geodesic_points.push_back (gp->get_pt());
  }

  //: clean up the geodesic_path_
  geodesic_path_.clear();
}

//: add a vertex to path and compute next F.
void fmm_track_geodesic::add_vertex_compute_next_face (dbmsh3d_vertex* Vert)
{
  track_prev_face_ = track_cur_face_;
  float rBestDistance = GW_INFINITE;
  track_cur_face_ = NULL;
  dbmsh3d_fmm_vertex_3d* pSelectedVert = NULL;
  dbmsh3d_edge* selected_edge = NULL;

  //: iterate through each one-ring neighborhood vertex of this Vert.
  //  go through each dbmsh3d_vertex::edges_[] and look for the other vertex
  for (dbmsh3d_ptr_node* cur = Vert->E_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    dbmsh3d_fmm_vertex_3d* pVert = (dbmsh3d_fmm_vertex_3d*) E->other_V (Vert);

    //: setup pSelectedVert
    if (pVert->dist() < rBestDistance) {
      rBestDistance = pVert->dist();
      pSelectedVert = pVert;
      selected_edge = E;

      //: if the # of faces of E is only one, then the F is the track_cur_face_
      //  else, there are 2 faces, select the F with closer other vertex
      if (E->halfedge()->pair() == NULL) {
        track_cur_face_ = (dbmsh3d_fmm_face*) E->halfedge()->face();
      }
      else {
        //: for the two faces f1 and f2, find the other edges e1 and e2, respectively.
        dbmsh3d_fmm_face* f1 = (dbmsh3d_fmm_face*) E->halfedge()->face();
        dbmsh3d_edge* e1 = f1->find_next_bnd_E (Vert, E);
        dbmsh3d_fmm_vertex_3d* v1 = (dbmsh3d_fmm_vertex_3d*) e1->other_V (Vert);

        dbmsh3d_halfedge* he = E->halfedge()->pair();
        dbmsh3d_fmm_face* f2 = (dbmsh3d_fmm_face*) he->face();
        dbmsh3d_edge* e2 = f2->find_next_bnd_E (Vert, E);
        dbmsh3d_fmm_vertex_3d* v2 = (dbmsh3d_fmm_vertex_3d*) e2->other_V (Vert);

        //: choose the shorter E
        if (v1->dist() < v2->dist())
          track_cur_face_ = f1;
        else
          track_cur_face_ = f2;
      }
    }
  }
  assert (track_cur_face_!=NULL);
  assert (pSelectedVert!=NULL);
  assert (selected_edge != NULL);

  //: this point is in the middle of (Vert and pSelectedVert)
  dbmsh3d_fmm_geodesic_point* pPoint = new dbmsh3d_fmm_geodesic_point;
  geodesic_path_.push_back (pPoint);

  if (selected_edge->sV() == Vert) {
    pPoint->pVert1_ = (dbmsh3d_fmm_vertex_3d*) Vert;
    pPoint->pVert2_ = (dbmsh3d_fmm_vertex_3d*) pSelectedVert;
    pPoint->set_s (1.0f); //the point is actually on the pSelectedVert
  }
  else {
    pPoint->pVert1_ = (dbmsh3d_fmm_vertex_3d*) pSelectedVert;
    pPoint->pVert2_ = (dbmsh3d_fmm_vertex_3d*) Vert;
    pPoint->set_s (0.0f); //the point is actually on the pSelectedVert
  }

  pPoint->set_cur_face (track_cur_face_);
}

/*------------------------------------------------------------------------------*/
// Name : GW_GeodesicPath::add_new_geodesic_point
/**
 *  \return [int] >0 if the path is ended.
 * 
 *  Compute a new point and add it to the path.
 */
/*------------------------------------------------------------------------------*/

#define NUM_TRACK_STEPS 10000

int fmm_track_geodesic::add_new_geodesic_point ()
{
  dbmsh3d_fmm_geodesic_point* pPoint = geodesic_path_.back();

  dbmsh3d_fmm_vertex_3d* pVert1 = pPoint->pVert1_;
  assert( pVert1!=NULL );
  dbmsh3d_fmm_vertex_3d* pVert2 = pPoint->pVert2_;
  assert( pVert2!=NULL );

  assert (track_cur_face_!=NULL);
  dbmsh3d_fmm_vertex_3d* pVert3 = (dbmsh3d_fmm_vertex_3d*) track_cur_face_->t_3rd_vertex (pVert1, pVert2);
  assert( pVert3!=NULL );

  dbmsh3d_edge* edge23 = track_cur_face_->m2t_edge_against_vertex (pVert1);
  dbmsh3d_edge* edge13 = track_cur_face_->m2t_edge_against_vertex (pVert2);
  dbmsh3d_edge* edge12 = track_cur_face_->m2t_edge_against_vertex (pVert3);

  // barycentric coords of the point (on an E of the triangle)
  float x,y,z;  
  x = pPoint->s();
  y = 1-x;
  z = 0;

  vgl_vector_3d<double> vector1 = pVert1->pt() - pVert3->pt();
  vgl_vector_3d<double> vector2 = pVert2->pt() - pVert3->pt();
  float l1 = (float) vector1.length();
  float l2 = (float) vector2.length();

  // should be an infinity loop, this is just to avoid bug.
  unsigned int nNum = 0;
  while (nNum<NUM_TRACK_STEPS)  { 
    nNum++;

    float dx, dy;
    track_cur_face_->compute_gradient (pVert1, pVert2, pVert3, x, y, dx, dy);

    float l, a;
    // Try each kind of possible crossing. 
    // The barycentric coords of the point is (x-l*dx/l1,y-l*dy/l2,z+l*(dx/l1+dy/l2)) 
    if (vcl_fabs(dx) > GW_EPSILON) {
      l = l1*x/dx;    // position along the line
      a = y-l*dy/l2;    // coordonate with respect to v2
      if (l>0 && l<=STEP_SIZE && 0<=a && a<=1)  {
        /* the crossing occurs on [v2,v3] */
        dbmsh3d_fmm_geodesic_point* pNewPoint = new dbmsh3d_fmm_geodesic_point;
        geodesic_path_.push_back (pNewPoint);

        assert (edge23->is_V_incident (pVert2));
        assert (edge23->is_V_incident (pVert3));
        pNewPoint->pVert1_ = pVert2;
        pNewPoint->pVert2_ = pVert3;

        assert (a <= 1);
        pNewPoint->set_s (a);
        track_prev_face_ = track_cur_face_;

        dbmsh3d_face* nextface = track_cur_face_->m2t_nbr_face_sharing_edge (pVert2, pVert3);
        if (nextface == NULL) {
          // we should stay on the same face
          pNewPoint->set_cur_face (track_cur_face_);
          return 0;
        }
        else {
          track_cur_face_ = (dbmsh3d_fmm_face*) nextface;
          pNewPoint->set_cur_face (track_cur_face_);

          // test for ending
          if (a<0.01 && pVert2->dist()<GW_EPSILON)
            return -1;
          if (a>0.99 && pVert3->dist()<GW_EPSILON)
            return -1;
          return 0;
        }        
      }
    }
    if (vcl_fabs(dy)>GW_EPSILON!=0)  {
      l = l2*y/dy;    // position along the line
      a = x-l*dx/l1;    // coordonate with respect to v1
      if (l>0 && l<=STEP_SIZE && 0<=a && a<=1)  {
        /* the crossing occurs on [v1,v3] */
        dbmsh3d_fmm_geodesic_point* pNewPoint = new dbmsh3d_fmm_geodesic_point;
        geodesic_path_.push_back( pNewPoint );
        
        assert (edge13->is_V_incident (pVert1));
        assert (edge13->is_V_incident (pVert3));
        pNewPoint->pVert1_ = pVert1;
        pNewPoint->pVert2_ = pVert3;
        pNewPoint->set_s (a);
        assert (a < 1);
        track_prev_face_ = track_cur_face_;

        dbmsh3d_face* nextface = track_cur_face_->m2t_nbr_face_sharing_edge (pVert1, pVert3);
        if (nextface == NULL) {
          // we should stay on the same face, 
          // the fact that track_prev_face_==pCurFace_ will force to go on an E
          pNewPoint->set_cur_face (track_cur_face_);
          return 0;
        }
        else {          track_cur_face_ = (dbmsh3d_fmm_face*) nextface;
          pNewPoint->set_cur_face (track_cur_face_);

          // test for ending
          if (a<0.01 && pVert1->dist()<GW_EPSILON)
            return -1;
          if (a>0.99 && pVert3->dist()<GW_EPSILON)
            return -1;
          return 0;
        }
      }
    }
    if (vcl_fabs(dx/l1+dy/l2)>GW_EPSILON)  {
      l = -z/(dx/l1+dy/l2);    // position along the line
      a = x-l*dx/l1;    // coordonate with respect to v1
      if (l>0 && l<=STEP_SIZE && 0<=a && a<=1)  {
        /* the crossing occurs on [v1,v2] */
        dbmsh3d_fmm_geodesic_point* pNewPoint = new dbmsh3d_fmm_geodesic_point;
        geodesic_path_.push_back (pNewPoint);
        
        assert (edge12->is_V_incident (pVert1));
        assert (edge12->is_V_incident (pVert2));
        pNewPoint->pVert1_ = pVert1;
        pNewPoint->pVert2_ = pVert2;
        pNewPoint->set_s (a);
        assert (a < 1);
        track_prev_face_ = track_cur_face_;

        dbmsh3d_face* nextface = track_cur_face_->m2t_nbr_face_sharing_edge (pVert1, pVert2);
        if (nextface == NULL) {
          // we should stay on the same face
          pNewPoint->set_cur_face (track_cur_face_);          
          return 0;
        }
        else {
          track_cur_face_ = (dbmsh3d_fmm_face*) nextface;
          pNewPoint->set_cur_face (track_cur_face_);

          // test for ending
          if (a<0.01 && pVert1->dist()<GW_EPSILON)
            return -1;
          if (a>0.99 && pVert2->dist()<GW_EPSILON)
            return -1;
          return 0;
        }
      }
    }

    if (vcl_fabs(dx)<GW_EPSILON && vcl_fabs(dx)<GW_EPSILON)  {
      /* special case : we must follow the E. */
      dbmsh3d_fmm_vertex_3d* pSelectedVert = pVert1;
      if (pVert2->dist() < pSelectedVert->dist())
        pSelectedVert = pVert2;
      if (pVert3->dist() < pSelectedVert->dist())
        pSelectedVert = pVert3;

      // just a check
      add_vertex_compute_next_face (pSelectedVert);
      assert (track_cur_face_!=NULL);
      if (pSelectedVert->dist() < GW_EPSILON)
        return -1;
      if (track_cur_face_==track_prev_face_ && pPoint->s() > 1-GW_EPSILON)  {
        /* hum, problem, we are in a local minimum */
        return -1;
      }
      return 0;
    }

    /* no intersection: we can advance */
    float xprev = x; //unused, yprev = y;
    x = x - STEP_SIZE*dx/l1;
    y = y - STEP_SIZE*dy/l2;

    if (x<0 || x>1 || y<0 || y>1)  {
      assert (z==0);
      
      dbmsh3d_face* pNextFace = track_cur_face_->m2t_nbr_face_against_vertex (pVert3);

      //: \todo remove the duplicate call.
      if (pNextFace==track_prev_face_ || track_cur_face_->m2t_nbr_face_against_vertex (pVert3)==NULL)  {
        /* special case : we must follow the E. */
        dbmsh3d_fmm_vertex_3d* pSelectedVert = pVert1;
        if (pVert2->dist() < pSelectedVert->dist())
          pSelectedVert = pVert2;
        if (pVert3->dist() < pSelectedVert->dist())
          pSelectedVert = pVert3;

        // just a check
        add_vertex_compute_next_face (pSelectedVert);
        if (pSelectedVert->dist() < GW_EPSILON)
          return -1;
      }
      else {
        /* we should go on another face */
        track_prev_face_ = track_cur_face_;
        track_cur_face_ = (dbmsh3d_fmm_face*) track_cur_face_->m2t_nbr_face_against_vertex (pVert3);
        assert( track_cur_face_!=NULL );
        dbmsh3d_fmm_geodesic_point* pNewPoint = new dbmsh3d_fmm_geodesic_point;
        geodesic_path_.push_back( pNewPoint );
        
        assert (edge12->is_V_incident (pVert1));
        assert (edge12->is_V_incident (pVert2));
        pNewPoint->pVert1_ = pVert1;
        pNewPoint->pVert2_ = pVert2;
        pNewPoint->set_cur_face (track_cur_face_);
        pNewPoint->set_s (xprev);
      }
      return 0;
    }
  }
  assert (false);
  dbmsh3d_fmm_vertex_3d* pSelectedVert = pVert1;
  if (pVert2->dist() < pSelectedVert->dist())
    pSelectedVert = pVert2;
  if (pVert3->dist() < pSelectedVert->dist())
    pSelectedVert = pVert3;

  // just a check
  add_vertex_compute_next_face (pSelectedVert);
  assert (track_cur_face_!=NULL);
  if (pSelectedVert->dist() < GW_EPSILON)
    return -1;
  if (track_cur_face_==track_prev_face_ && pPoint->s() > 1-GW_EPSILON)  {
    /* hum, problem, we are in a local minimum */
    return -1;
  }
  return 0;
}

// #############################################################

dbmsh3d_fmm_mesh* generate_fmm_tri_mesh (dbmsh3d_mesh* mesh)
{
  dbmsh3d_fmm_mesh* fmm_tri_mesh = new dbmsh3d_fmm_mesh;

  //: put all existing vertices into the new tri_mesh
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = mesh->vertexmap().begin();
  for (; vit != mesh->vertexmap().end(); vit++) {
    dbmsh3d_vertex* vertex = (*vit).second;

    dbmsh3d_fmm_vertex_3d* new_vertex = (dbmsh3d_fmm_vertex_3d*) fmm_tri_mesh->_new_vertex ();
    new_vertex->set_id (vertex->id());
    new_vertex->get_pt().set (vertex->pt().x(), vertex->pt().y(), vertex->pt().z());
    fmm_tri_mesh->_add_vertex (new_vertex);
  }

  //: triangulate each face of the mesh
  int count = 0;
  vcl_map<int, dbmsh3d_face*>::iterator it = mesh->facemap().begin();
  for (; it != mesh->facemap().end(); it++) {
    dbmsh3d_face* F = (*it).second;
    
    //: Note that if the input F is already a triangle, 
    //  need to deal with it separately.
    if (F->vertices().size() < 4) {
      assert (F->vertices().size() == 3);

      //: just make a new F
      dbmsh3d_face* tri_face = fmm_tri_mesh->_new_face ();

      for (unsigned int i=0; i<F->vertices().size(); i++) {
        dbmsh3d_vertex* vertex = F->vertices (i);
        int id = vertex->id();
        dbmsh3d_fmm_vertex_3d* new_vertex = (dbmsh3d_fmm_vertex_3d*) fmm_tri_mesh->vertexmap (id);
        tri_face->_ifs_add_bnd_V (new_vertex);
      }

      fmm_tri_mesh->_add_face (tri_face);
    }
    else {

      //triangulate the F
      Vector2dVector a;
      ///vul_printf (vcl_cout, "Input Polygon %d => \n", count);

      //: need to project the 3d coord of each vertex onto a reference plane
      //  to get its 2d coord.
      vcl_vector<dbmsh3d_vertex*> vertices;
      vgl_vector_3d<double> normal = compute_normal_ifs (F->vertices());

      //: use V[0] as origin O, use V[1]-V[0] as e1, e2 = N * e1
      vgl_point_3d<double> o = ((dbmsh3d_vertex*)F->vertices(0))->pt();
      vgl_vector_3d<double> e1 = ((dbmsh3d_vertex*)F->vertices(1))->pt() - o;
      vgl_vector_3d<double> e2 = cross_product (normal, e1);

      //: normalize e1 and e2
      e1 = normalize (e1);
      e2 = normalize (e2);

      //: for any given point P on the 3D polygon, can project to the plane
      //  OP = u e1 + v e2

      for (unsigned int j=0; j<F->vertices().size(); j++) {
        dbmsh3d_vertex* vertex = F->vertices (j);

        //: for any given point P on the 3D polygon, can project to the plane
        //  OP = u e1 + v e2
        vgl_vector_3d<double> op = vertex->pt() - o;
        float u = (float) dot_product (op, e1);
        float v = (float) dot_product (op, e2);

        //  for now, just use the (x, y), ignore z!!! 
        //  Should compute normal and project to the polygon plane!
        a.push_back (Vector2d(u, v, vertex->id()));

        ///vul_printf (vcl_cout, "\t (%lf, %lf) \n", x, y);
      }
    
      // allocate an STL vector to hold the answer.
      Vector2dVector result;

      // triangulate this polygon.
      Triangulate::Process (a, result);

      unsigned int tcount = (unsigned int) (result.size()/3);
      //: if any numerical arises, this is not true!
      ///assert (F->vertices().size() == tcount+2);

      for (unsigned int i=0; i<tcount; i++) {
        const Vector2d &p1 = result[i*3+0];
        const Vector2d &p2 = result[i*3+1];
        const Vector2d &p3 = result[i*3+2];

        ///vul_printf (vcl_cout, "Triangle %d => (%lf, %lf) (%lf, %lf) (%lf, %lf)\n",
        //              i+1,p1.GetX(),p1.GetY(),p2.GetX(),p2.GetY(),p3.GetX(),p3.GetY());
        ///vul_printf (vcl_cout, "Triangle %d => v[%d], v[%d], v[%d]\n", i+1, p1.id_, p2.id_, p3.id_);

        //: for each triangle, add as a new F
        dbmsh3d_face* tri_face = fmm_tri_mesh->_new_face ();

        dbmsh3d_fmm_vertex_3d* v1 = (dbmsh3d_fmm_vertex_3d*) fmm_tri_mesh->vertexmap (p1.id_);
        tri_face->_ifs_add_bnd_V (v1);
        dbmsh3d_fmm_vertex_3d* v2 = (dbmsh3d_fmm_vertex_3d*) fmm_tri_mesh->vertexmap (p2.id_);
        tri_face->_ifs_add_bnd_V (v2);
        dbmsh3d_fmm_vertex_3d* v3 = (dbmsh3d_fmm_vertex_3d*) fmm_tri_mesh->vertexmap (p3.id_);
        tri_face->_ifs_add_bnd_V (v3);

        fmm_tri_mesh->_add_face (tri_face);
      }

      count++;
    }
  }

  return fmm_tri_mesh;
}





