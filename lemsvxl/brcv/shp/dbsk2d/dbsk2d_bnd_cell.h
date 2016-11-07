// This is brcv/shp/dbsk2d/dbsk2d_bnd_cell.h
#ifndef dbsk2d_bnd_cell_h_
#define dbsk2d_bnd_cell_h_
//:
// \file
// \brief A retangular grid cell to spatially group shock boundary
// elements together.
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date 08/04/2005
//
// \verbatim
//  Modifications
//   Nhon Trinh 08/04/2005  Initial version
// \endverbatim

#include <vgl/vgl_box_2d.h>
#include <vul/vul_timestamp.h>
#include <vbl/vbl_ref_count.h>

#include <dbsk2d/dbsk2d_ishock_belm.h>
#include <dbsk2d/dbsk2d_bnd_edge.h>
#include <dbsk2d/dbsk2d_bnd_edge_sptr.h>


//: Index of a dbsk2d_bnd_cell
class dbsk2d_bnd_cell_index
{
public:
  int row;
  int col;
  dbsk2d_bnd_cell_index():row(-1), col(-1){};
  dbsk2d_bnd_cell_index(int new_row, int new_col): row(new_row), col(new_col){};
};

struct timed_bnd_edge_vector: public vul_timestamp,
                              public vcl_vector<dbsk2d_ishock_belm* >
{};

class dbsk2d_bnd_preprocess;

//: A cell of boundary and shock elements 
class dbsk2d_bnd_cell: public vul_timestamp, 
                       public vbl_ref_count
{
  
protected:
  dbsk2d_bnd_cell_index index_;
  vgl_box_2d<double > box_;
  vcl_list<dbsk2d_bnd_edge_sptr > edges_;
  mutable timed_bnd_edge_vector belms_;

public:
  typedef vcl_vector<dbsk2d_ishock_belm* >::const_iterator belm_iterator;

  //*************************************************
  // Constructors/Destructors
  //*************************************************
  //: Constructor
  dbsk2d_bnd_cell();

  //: Constructor - type 2
  dbsk2d_bnd_cell(const dbsk2d_bnd_cell_index& index, const vgl_box_2d<double >& box );

  //: Destructor
  ~dbsk2d_bnd_cell(){};

  //: Clear edge list and belm list
  void clear_all();

  //*************************************************
  // Access member variables
  //*************************************************
  
  //: Return index of this grid cell
  dbsk2d_bnd_cell_index index() const { return this->index_; };
  
  //: Set the index of this grid cell
  void set_index( const dbsk2d_bnd_cell_index& new_index )
  { this->index_ = new_index; this->touch(); }

  //: Return the geometric retangular box of the cell
  vgl_box_2d<double > box() const {return this->box_; }
  
  //: Set the geometric retangular box of the cell
  void set_box(const vgl_box_2d<double >& newbox)
  { this->box_ = newbox; this->touch(); }

  //: Return reference to the edge list inside the box
  const vcl_list<dbsk2d_bnd_edge_sptr >& edges() const {return this->edges_; }
  
  //: Return true when the cell contains no edges
  bool empty() const {return this->edges_.empty(); }

  //: Return number of edges inside the cell
  unsigned int num_bnd_edges() const {return this->edges_.size(); }

  //: add an edge to the cell
  // Do nothing if the edges is already in the cell
  void add_bnd_edge(const dbsk2d_bnd_edge_sptr& edge);

  //: remove an edge fromt the cell
  // Do nothing if the edge is not in the cell
  void remove_bnd_edge(const dbsk2d_bnd_edge_sptr& edge );

  //: Return reference to the belm list
  const vcl_vector<dbsk2d_ishock_belm* >& belms() const;

  //: Update the belm list in the cell, by collecting them in the `edges_'
  void update_belms() const;

  //: Return number of belm in the cell
  unsigned int num_belms() const {return this->belms().size(); }

};

#endif //dbsk2d_bnd_cell_h_
