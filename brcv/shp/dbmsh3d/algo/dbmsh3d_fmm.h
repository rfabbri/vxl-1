//: 050805 MingChing Chang
//  Kimmel & Seithan's Fast Marching Method on mesh
//  to computing geodesic shortest path on the mesh.
//  Code modified from Gaberial Peyre's GeoWave.
//  http://www.cmap.polytechnique.fr/~peyre/geodesic_computations/

#ifndef _dbmsh3d_fmm_h_
#define _dbmsh3d_fmm_h_

#include <vcl_algorithm.h>

#include <dbmsh3d/dbmsh3d_mesh.h>

#define GW_INFINITE   1e9
#define GW_EPSILON    1e-9

//: Derived FMM vertex from a mesh vertex
class dbmsh3d_fmm_vertex_3d : public dbmsh3d_vertex
{
public:
  typedef enum {
    STATE_FAR,
    STATE_ALIVE,
    STATE_DEAD
  } STATE;

protected:
  //: state of the FMM vertex : can be far/alive/dead
  STATE state_;
  
  //: the computed distance of this vertex from the closest source vertex.
  float dist_;

  //: the source vertex who produces this front.
  dbmsh3d_vertex* source_of_front_;

public:
  //: ====== Constructor/Destructor ======
  dbmsh3d_fmm_vertex_3d (int id) : dbmsh3d_vertex (id) {
    state_ = STATE_FAR;
    dist_ = -1;

    pFront1_ = NULL;
    pFront2_ = NULL;
    rDist1_  = GW_INFINITE;
    rDist2_  = GW_INFINITE;
  }

  //: ====== Data access functions ======
  STATE state() const {
    return state_;
  }
  void set_state (STATE s) {
    state_ = s;
  }
  float dist () const {
    return dist_;
  }
  void set_dist (float d) {
    dist_ = d;
  }
  dbmsh3d_vertex* source_of_front () const {
    return source_of_front_;
  }
  void set_source_of_front (dbmsh3d_vertex* source) {
    source_of_front_ = source;
  }

  //: ====== Other functions ======
  dbmsh3d_vertex* pFront1_;
  float rDist1_;
  dbmsh3d_vertex* pFront2_;
  float rDist2_;
  void RecordOverlap (dbmsh3d_vertex* front, float dist)
  {
    ///assert (0);
    if( pFront1_==NULL )
    {
      pFront1_ = front;
      rDist1_ = dist;
    }
    else if( pFront1_== front )
      rDist1_ = vcl_min( dist, rDist1_ );
    else if( pFront2_== front )
      rDist2_ = vcl_min( dist, rDist2_ );
    else
    {
      pFront2_ = front;
      rDist2_ = vcl_min( dist, rDist2_ );
    }
  }
};

// return True if distance of the 1st mesh is < to the one of the 2nd.
///static bool _compare_vertex_distance (dbmsh3d_fmm_vertex_3d* pVert1, dbmsh3d_fmm_vertex_3d* pVert2);


//: fmm_face provide interpolation of the distance value on a triangle.
//  Here we use bi-linear interpolation.

class dbmsh3d_fmm_face : public dbmsh3d_face
{
public:
  dbmsh3d_fmm_face (int id) : dbmsh3d_face (id) {
  }
  virtual ~dbmsh3d_fmm_face() {
  }

  //: return in dx and dy
  void compute_gradient (dbmsh3d_fmm_vertex_3d* v0, 
                         dbmsh3d_fmm_vertex_3d* v1, 
                         dbmsh3d_fmm_vertex_3d* v2, 
                         float x, float y, 
                         float& dx, float& dy);

  float compute_value (dbmsh3d_fmm_vertex_3d* v0, 
                       dbmsh3d_fmm_vertex_3d* v1, 
                       dbmsh3d_fmm_vertex_3d* v2, 
                       float x, float y);
};

#define STEP_SIZE 0.01f

//: Use const wave speed of 1.0
#define WAVESPEEDF 1.0f

class dbmsh3d_fmm_mesh : public dbmsh3d_mesh
{
protected:
  //: the list of all source vertices
  vcl_vector<dbmsh3d_vertex*> source_vertices_;

  //: the active_vertex heap for FMM
  //  should be filled with the starting point of the marching
  //  in the initialization stage.
  vcl_vector<dbmsh3d_fmm_vertex_3d*> active_vertex_heap_;

public:
  //: ====== Constructor/Destructor ======
  dbmsh3d_fmm_mesh () {
  }

  //: ====== Data access functions ======
  vcl_vector<dbmsh3d_vertex*>& source_vertices() {
    return source_vertices_;
  }
  dbmsh3d_vertex* source_vertices (const int i) {
    return source_vertices_[i];
  }

  //: ====== Connectivity Modification Functions ======
  //: new/delete function of the class hierarchy
  virtual dbmsh3d_vertex* _new_vertex () {
    return new dbmsh3d_fmm_vertex_3d (vertex_id_counter_++);
  }
  virtual dbmsh3d_face* _new_face () {
    return new dbmsh3d_fmm_face (face_id_counter_++);
  }

  //: ====== Fast Marching functions ======
  //: the entrance function of FMM.
  void run_fmm ();

  void setup_fast_marching ();
    void add_source_vertex (dbmsh3d_fmm_vertex_3d* source_vertex);

  bool perform_fmm_one_step ();
    float compute_vertex_distance (dbmsh3d_face* CurrentFace, 
                                   dbmsh3d_fmm_vertex_3d* CurrentVertex,
                                   dbmsh3d_fmm_vertex_3d* Vert1,
                                   dbmsh3d_fmm_vertex_3d* Vert2,
                                   dbmsh3d_vertex* CurrentFront);
      float compute_update_sethian_method (float d1, float d2, 
                                           float a, float b, 
                                           float dot);
      dbmsh3d_fmm_vertex_3d* unfold_triangle (dbmsh3d_face* CurFace, 
                                              dbmsh3d_fmm_vertex_3d* v, 
                                              dbmsh3d_fmm_vertex_3d* v1, 
                                              dbmsh3d_fmm_vertex_3d* v2, 
                                              float& dist, 
                                              float& dot1, 
                                              float& dot2);    
};

// ########################################################################

//: the geodesic point on the geodesic path (polyline)
//  It has to be on an edge of a mesh.
class dbmsh3d_fmm_geodesic_point
{
protected:
  //: The variable parameterize (s_v to e_v) as (0 to 1)
  //  The extrinsic coordinate of the point is (s)*(s_v) + (1-s)*(e_v)
  //  (old) coordinate of point is s*V1+(1-s)*V2
  float s_;

  // the current face for computing the barycentric coord.
  dbmsh3d_face* cur_face_;

public:
  //: instead of keeping two vertices, should keep only one pointer to the edge.
  dbmsh3d_fmm_vertex_3d* pVert1_;
  dbmsh3d_fmm_vertex_3d* pVert2_;


  //: ====== Constructor/Destructor ======
  dbmsh3d_fmm_geodesic_point () {
    s_ = 0;
    pVert1_  = NULL;
    pVert2_  = NULL;
    cur_face_  = NULL;
  }

  //: ====== Data access functions ======
  float s() const {
    return s_;
  }
  void set_s (float s) {
    s_ = s;
  }
  dbmsh3d_face* cur_face () const {
    return cur_face_;
  }
  void set_cur_face (dbmsh3d_face* curface) {
    cur_face_ = curface;
  }

  //: compute the extrinsic coord. of this geodesic point from interpolation
  vgl_point_3d<double> get_pt() {

    double x = pVert1_->pt().x() * s_ + pVert2_->pt().x()* (1-s_);
    double y = pVert1_->pt().y() * s_ + pVert2_->pt().y()* (1-s_);
    double z = pVert1_->pt().z() * s_ + pVert2_->pt().z()* (1-s_);

    vgl_point_3d<double> pt (x, y, z);

    return pt;
  }
};

//: Back-Tracking the Geodesic Shortest Path from a given vertex.
class fmm_track_geodesic
{
protected:
  dbmsh3d_fmm_face* track_cur_face_;
  dbmsh3d_fmm_face* track_prev_face_;

  vcl_vector<dbmsh3d_fmm_geodesic_point*> geodesic_path_;

public:
  //: ====== Constructor/Destructor ======
  fmm_track_geodesic () {
    track_prev_face_ = NULL;
    track_cur_face_ = NULL;
  }
  ~fmm_track_geodesic () {
    geodesic_path_.clear();
  }

  void back_track_geodesic (dbmsh3d_vertex* ending_vertex);
    void add_vertex_compute_next_face (dbmsh3d_vertex* vertex);
    int add_new_geodesic_point ();

  void get_geodesic_polyline (vcl_vector<vgl_point_3d<double> >& geodesic_points);
};

dbmsh3d_fmm_mesh* generate_fmm_tri_mesh (dbmsh3d_mesh* mesh);

#endif



