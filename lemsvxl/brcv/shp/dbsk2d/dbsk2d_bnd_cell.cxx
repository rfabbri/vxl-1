// This is brcv/shp/dbsk2d/dbsk2d_bnd_cell.cxx

//:
// \file

#include "dbsk2d_bnd_cell.h"

#include <vcl_algorithm.h>
#include <vtol/vtol_list_functions.h>

#include <dbsk2d/dbsk2d_bnd_edge.h>
#include <dbsk2d/dbsk2d_bnd_utils.h>




//*************************************************
// Constructors/Destructors
//*************************************************

//-------------------------------------------------
//: Constructor
dbsk2d_bnd_cell::
dbsk2d_bnd_cell() : index_(-1, -1)
{
  this->clear_all();
}

//-------------------------------------------------
//: Constructor - type 2
dbsk2d_bnd_cell::
dbsk2d_bnd_cell(const dbsk2d_bnd_cell_index& index, 
                const vgl_box_2d<double >& box ):
index_(index), box_(box)
{
  this->clear_all();
}


//: Clear everything
void dbsk2d_bnd_cell::
clear_all()
{
  this->belms_.clear();
  this->belms_.touch();
  while (this->edges_.size() > 0)
  {
    this->remove_bnd_edge(this->edges_.front());
  }
}


//*************************************************
// Access member variables
//*************************************************

//-------------------------------------------------
//: add an edge to the cell
void dbsk2d_bnd_cell::
add_bnd_edge(const dbsk2d_bnd_edge_sptr& edge)
{
  if (vcl_find(this->edges_.begin(), this->edges_.end(), edge)==
    this->edges_.end())
  {
    this->edges_.push_back(edge);
    edge->add_cell(this);
    this->touch();
  }
  return;
}

//-------------------------------------------------
//: remove an edge fromt the cell
// Do nothing if the edge is not in the cell
void dbsk2d_bnd_cell::
remove_bnd_edge(const dbsk2d_bnd_edge_sptr& edge )
{
  vcl_list<dbsk2d_bnd_edge_sptr >::iterator eit =
    vcl_find(this->edges_.begin(), this->edges_.end(), edge);
  if ( eit != this->edges_.end() )
  {
    edge->remove_cell(this);
    this->edges_.erase(eit);
    this->touch();
  }
  return;
}


//-------------------------------------------------
//: Return reference to the belm list
const vcl_vector<dbsk2d_ishock_belm* >& dbsk2d_bnd_cell::
belms() const
{
  if (this->belms_.older(this))
  {
    this->belms_.touch();
    this->update_belms();
  }
  return this->belms_;
}


//-------------------------------------------------
//: Update the belm list in the cell, by collecting them in the `edges_'
void dbsk2d_bnd_cell::
update_belms() const
{
  // this->belms_.clear();
  
  //// create a list of vertices of the edges
  //// and insert the dbsk2d_ishock_bcurves to the `belms' list
  //vcl_vector<dbsk2d_bnd_vertex_sptr > all_vertices;
  //for (vcl_list<dbsk2d_bnd_edge_sptr >::const_iterator eit = 
  //  this->edges_.begin(); eit != this->edges_.end(); ++eit)
  //{
  //  all_vertices.push_back((*eit)->bnd_v1());

  //  // only consider the second vertex and the internal bcurve
  //  // for real edges
  //  if (! (*eit)->is_a_point())
  //  {
  //    all_vertices.push_back((*eit)->bnd_v2());
  //    this->belms_.push_back((*eit)->left_bcurve());
  //    this->belms_.push_back((*eit)->right_bcurve());
  //  }
  //}  
  //tagged_union<dbsk2d_bnd_vertex_sptr >(&all_vertices);
  //
  //// add the dbsk2d_ishock_bpoints to the `belms' list
  //for (unsigned int i = 0; i < all_vertices.size(); ++i)
  //{
  //  this->belms_.push_back(all_vertices[i]->bpoint());
  //}

  dbsk2d_bnd_utils::extract_belm_list(this->edges_, this->belms_);
  return;
}

