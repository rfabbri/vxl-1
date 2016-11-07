// This is dbmsh3d/algo/dbmsh3d_pt_set_correspondence.h

#ifndef dbmsh3d_pt_set_correspondence_h_
#define dbmsh3d_pt_set_correspondence_h_

//--------------------------------------------------------------------------------
//:
// \file
// \brief Algorithms to compute correspondence between two point sets
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date August 14, 2006
//
// \verbatim
//  Modifications:
// \endverbatim
//--------------------------------------------------------------------------------


#include <dbmsh3d/dbmsh3d_mesh.h>
#include <vcl_map.h>

//: a class to match between two meshes
class dbmsh3d_match_mesh
{
public:
  dbmsh3d_match_mesh(): data_mesh_(0), reference_mesh_(0){}
  virtual ~dbmsh3d_match_mesh(){}

  // ========== Data access =================
  // Source mesh
  dbmsh3d_mesh* data_mesh(){ return this->data_mesh_; }
  void set_data_mesh(dbmsh3d_mesh* mesh){ this->data_mesh_ = mesh; }
  

  // Target mesh
  dbmsh3d_mesh* reference_mesh(){ return this->reference_mesh_; }
  void set_reference_mesh(dbmsh3d_mesh* mesh){ this->reference_mesh_ = mesh; }

  virtual void match() = 0;


  // ========== Data member =================

  // Source mesh
  dbmsh3d_mesh* data_mesh_;

  // Target mesh
  dbmsh3d_mesh* reference_mesh_;
};


// ============================================================================
//: Match 2 meshes using euclidean distance between its point sets
class dbmsh3d_match_mesh_using_euclidean_distance : public dbmsh3d_match_mesh
{
public:
  dbmsh3d_match_mesh_using_euclidean_distance(): dbmsh3d_match_mesh() 
  {
    this->correspondence_map_.clear();
    this->distance_map_.clear();
  }

  dbmsh3d_match_mesh_using_euclidean_distance( dbmsh3d_mesh* reference_mesh,
    dbmsh3d_mesh* data_mesh,
    double max_distance);
  virtual ~dbmsh3d_match_mesh_using_euclidean_distance(){};

public:
  double max_distance() const { return this->max_distance_; }
  void set_max_distance(double max_distance) 
  { this->max_distance_ = max_distance; }


  //: Match the two meshes
  virtual void match();

  // ============ Data access ===================================
  //: Return matching results
  const vcl_map<int, int >& correspondence_map()
  { return this->correspondence_map_; }

  //: Return deviation map
  const vcl_map<int, double >& distance_map()
  { return this->distance_map_; };

protected:
  // max distance between two corresponding points
  double max_distance_;

  // map between id's of data mesh and id's of reference map
  vcl_map<int, int > correspondence_map_;

  // distance between matched pairs of vertices, key is the id's of 
  // vertices on data mesh
  vcl_map<int, double > distance_map_;
};


#endif //dbmsh3d_pt_set_correspondence_h_



