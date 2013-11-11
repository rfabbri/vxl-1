// This is file shp/dbsk2d/dbsk2d_bnd_contour.cxx

//:
// \file

#include "dbsk2d_bnd_contour.h"

#include <vcl_algorithm.h>

#include <dbsk2d/dbsk2d_distance.h>
#include <dbsk2d/dbsk2d_bnd_utils.h>


//************************************************************
// Constructor/Destructor/Initialization
//*************************************************************

//--------------------------------------------------------------------
//: Constructor - from a list of edges
// require: the edges are connected
dbsk2d_bnd_contour::
dbsk2d_bnd_contour( const vcl_vector<dbsk2d_bnd_edge_sptr >& bnd_edges, 
                   int id) :
vtol_one_chain()
{
  this->set_id(id);
  if (bnd_edges.empty()) return;

  // check connectedness and determine edge directions
  vcl_vector<signed char > directions;
  if (!dbsk2d_bnd_utils::determine_edge_directions(bnd_edges, directions))
  {
    vcl_cerr << "Error in dbsk2d_bnd_contour constructor: " << 
        "The edges are not connected" << vcl_endl;
    return;
  }

  // test for connectedness is passed. now put them into inferior list.
  for ( vcl_vector<dbsk2d_bnd_edge_sptr >::const_iterator edge_it =
    bnd_edges.begin(); edge_it != bnd_edges.end(); ++edge_it )
  {
    this->link_inferior(*edge_it);
  }
  this->directions_ = directions;
  this->touch();
  //this->update_len_cache();
  return;
}


//--------------------------------------------------------------------
//: Constructor - from a list of edges with known directions
// no connectivity check
dbsk2d_bnd_contour::
dbsk2d_bnd_contour(const vcl_vector<dbsk2d_bnd_edge_sptr >& edges, 
                   const vcl_vector<signed char >& directions,
                   int id) : vtol_one_chain()
{
  dbsk2d_assert(edges.size() == directions.size());
  this->set_id(id);
  
  if (edges.empty()) return;
  
  // now put edges into inferior list.
  for ( vcl_vector<dbsk2d_bnd_edge_sptr >::const_iterator eit =
    edges.begin(); eit != edges.end(); ++eit )
  {
    this->link_inferior(*eit);
  }

  this->directions_ = directions;
  this->touch();
  //this->update_len_cache();
  return;
}
  

// -----------------------------------------------------------------------
//: Constructor - from an edge
dbsk2d_bnd_contour::
dbsk2d_bnd_contour(const dbsk2d_bnd_edge_sptr& edge, int id) :
vtol_one_chain()
{
  dbsk2d_assert(edge);
  this->set_id(id);
  this->link_inferior(edge);
  this->directions_.push_back(1);
  this->touch();
  //this->update_len_cache();
  return;
}



//************************************************************
// Accessors
//************************************************************


// accessing geometry properties

//: point at arclength s
// need rewrite
vgl_point_2d< double > dbsk2d_bnd_contour::
point_at(double s) const
{
  return vgl_point_2d<double>();
}

//: tangent at arclength s
// need rewrite
vgl_vector_2d< double > dbsk2d_bnd_contour::
tangent_at (double s) const
{
  return vgl_vector_2d< double >();
}

//: tangent angle at arclength s
// need rewrite
double dbsk2d_bnd_contour::
tangent_angle_at( double s) const
{
  return 0;
}

// --------------------------------------------------------
//: return index of bnd_edge at arclength s
int dbsk2d_bnd_contour::
edge_index_at(double s ) const
{
  // do a binary search to find the correct interval
  // ignore fuzzy boolean for now
  // \TODO perform fuzzy boolean
  
  if (s <0) return -1;

  // binary search for s in vector of arclens
  const vcl_vector<double >::const_iterator p = 
    vcl_lower_bound(this->len_cache().begin(), this->len_cache().end(), s);

  // if out of range, this will return len_cache_.size()
  return p - this->len_cache().begin();
}

// --------------------------------------------------------
//: return dbsk2d_bnd_edge at index i (first edge: i = 0)
dbsk2d_bnd_edge_sptr dbsk2d_bnd_contour::
bnd_edge(int i) const
{ 
  //if (i < 0 || i >= this->numinf()) return 0;
  dbsk2d_assert (i>=0);
  dbsk2d_assert (i<this->numinf());
  return static_cast<dbsk2d_bnd_edge* >(this->inferiors_[i].ptr());
};

// --------------------------------------------------------
//: return edge at arclength s (first edge: 0<= s < s1)
dbsk2d_bnd_edge_sptr dbsk2d_bnd_contour::
edge_at(double s) const
{
  return this->bnd_edge(this->edge_index_at(s));
}


// --------------------------------------------------------
//: return arclength to the beginning of a bnd_edge
double dbsk2d_bnd_contour::
arclength_at(int index) const
{
  if (index == 0) return 0;
  dbsk2d_assert(index > 0 && index <= this->numinf());
  return this->len_cache()[index-1];
}

// --------------------------------------------------------
//: return arclength to the beginning of a bnd_edge
double dbsk2d_bnd_contour::
arclength_at(dbsk2d_bnd_edge_sptr edge) const
{
  // determine location of `edge' in the contour
  edge_list edges;
  this->edges(edges);
  edge_list::iterator edge_pos = 
    vcl_find(edges.begin(), edges.end(), edge.ptr());
  
  // if edge is not part of the contour, return -1
  if (edge_pos == edges.end())
  {
    return -1;
  }

  // order of `edge' in contour
  int edge_order = edge_pos - edges.begin();

  // determine corresponding arclength
  if (edge_order == 0) return 0;
  return this->len_cache()[edge_order-1];
}



//: Return vertex at index i of the chain of (num_edges()+1) vertices.
dbsk2d_bnd_vertex_sptr dbsk2d_bnd_contour::
bnd_vertex(int i) const
{
  dbsk2d_assert(i >= 0);
  dbsk2d_assert(this->numinf() > 0);
  dbsk2d_assert(i <= this->numinf());

  // special treatment for the last vertex
  if (i==this->numinf())
    return (this->dir(i-1)==1) ? 
    this->bnd_edge(i-1)->bnd_v2() : this->bnd_edge(i-1)->bnd_v1(); 

  return (this->dir(i)==1) ? 
    this->bnd_edge(i)->bnd_v1() : this->bnd_edge(i)->bnd_v2(); 
  return 0;
}




//---------------------------------------------
// Cache-related functions
//---------------------------------------------

// -------------------------------------------------------------
//: Return pointer to the cached cumulated-length vector 
// if `recompute' = true, the cache vector will be cleared and then recomputed
const vcl_vector< double >& dbsk2d_bnd_contour::
len_cache() const
{
  if (this->len_cache_.older(this))
  {
    this->len_cache_.touch();
    this->update_len_cache();
  }
  return (this->len_cache_);
}

//: Recompute the cached cumulated-length vector
void dbsk2d_bnd_contour::
update_len_cache() const
{
  this->len_cache_.clear();
  
  double accum_length = 0;
  for (int i = 0; i <this->num_edges(); i++)
  {
    accum_length += this->bnd_edge(i)->len();
    this->len_cache_.push_back(accum_length);
  }
}


//***********************************************
// OPERATORS
//***********************************************

//: Equality operator
// inherited from vtol_one_chain. Need rewrite.
bool dbsk2d_bnd_contour::
operator==(vtol_one_chain const& other) const
{
  return false;
}

//: Equality operator
// Need rewrite.
bool dbsk2d_bnd_contour::
operator==(dbsk2d_bnd_contour const& other) const
{
  return false;
}


//*********************************************
// Basic functions
//*********************************************

//: Link with an dbsk2d_bnd_edge
void dbsk2d_bnd_contour::
link_inferior(dbsk2d_bnd_edge_sptr inf )
{
  vtol_topology_object::link_inferior(inf->cast_to_topology_object());
}

//: Unlink an dbsk2d_bnd_edge
void dbsk2d_bnd_contour::
unlink_inferior(dbsk2d_bnd_edge_sptr inf)
{
  vtol_topology_object::unlink_inferior(inf->cast_to_topology_object());
}



//: add a vtol_edge
// `new_dir' may be overridden
// inherited.
void dbsk2d_bnd_contour::
add_edge(vtol_edge_sptr & new_edge, bool new_dir)
{
  if (new_edge->is_a() != "dbsk2d_bnd_edge") return;
  this->add_edge(static_cast<dbsk2d_bnd_edge* >(new_edge.ptr()));
}


//--------------------------------------------------
//: add a dbsk2d_bnd_edge
// Require: `new_edge' shares a vertex with contour's last edge
bool dbsk2d_bnd_contour::
add_edge(const dbsk2d_bnd_edge_sptr & new_edge)
{
  // if the contour is empty, no need to check for connectivity
  if (this->numinf() == 0)
  {
    vtol_one_chain::add_edge(new_edge.ptr(), true);
    return true;
  }

  // determine direction of the new edge
  bool new_dir = true;
  vtol_edge_sptr last_edge = this->edge(this->num_edges()-1);
  vtol_vertex_sptr last_vertex = 
    (this->dir(this->num_edges()-1)==1) ? last_edge->v2() : last_edge->v1();
  if (new_edge->v1() == last_vertex)
    new_dir = true;
  else if (new_edge->v2() == last_vertex)
    new_dir = false;
  else
    return false;

  // use the parent class function to add the new edge.
  vtol_one_chain::add_edge(new_edge.ptr(), new_dir);
  return true;
}


//--------------------------------------------------
// replace a group of edges with a group of new edges
// Require: `new_edges' need to be connected together and to
// edges before and after the replaced edges
// if `end_edge' == 0, it is assumed = `start_edge'
// No connectivity check in this function
bool dbsk2d_bnd_contour::
replace_edges(const vcl_vector<dbsk2d_bnd_edge_sptr >& new_edges,
              const vcl_vector<signed char >& directions,
              const dbsk2d_bnd_edge_sptr& start_edge,
              const dbsk2d_bnd_edge_sptr& end_edge)
{
  //require
  dbsk2d_assert(start_edge);
  dbsk2d_assert(new_edges.size() == directions.size());

  // 0. Locate position of `start_edge' and `end_edge' in contour
  // start_index
  topology_list::const_iterator eit_start = 
    vcl_find(this->inferiors()->begin(), this->inferiors()->end(), 
    start_edge->cast_to_topology_object());
  if (eit_start == this->inferiors()->end()) return false;
  int start_index= eit_start-this->inferiors()->begin();

  // end_index
  int end_index = (end_edge) ? 
    vcl_find(this->inferiors()->begin(), this->inferiors()->end(), 
    end_edge->cast_to_topology_object()) - this->inferiors()->begin()
  : start_index;

  if (end_index == this->numinf()) return false;

  // 1. Connectivity check (simple)
  dbsk2d_bnd_vertex_sptr sv = (directions[0]==1) ?
    new_edges[0]->bnd_v1() : new_edges[0]->bnd_v2();

  dbsk2d_bnd_vertex_sptr ev = (directions.back()==1) ?
    new_edges.back()->bnd_v2() : new_edges.back()->bnd_v1();

  if (sv != this->bnd_vertex(start_index)) return false;
  if (ev != this->bnd_vertex(end_index+1)) return false;
  
  // 2. Replace the edges
  // 2.1 Create links from `new_edges' to `this' contour
  for (unsigned int i = 0; i < new_edges.size(); ++i)
  {
    this->link_inferior(new_edges[i]);
  }

  // 2.2 the added edges are not in right order, now fix it.

  // list of topology objects justed added
  //unused unsigned int new_size = this->numinf();
  topology_list tlist (this->inferiors()->end()-new_edges.size(), this->inferiors()->end());

  // erase the newly added ones
  this->inferiors()->erase(this->inferiors()->end()-new_edges.size(), this->inferiors()->end());
  
  // put the `new_edges' back in the proper position
  this->inferiors()->insert(this->inferiors()->begin()+start_index,
    tlist.begin(), tlist.end());
  
  // 2.3 Unlink the old edges
  for (unsigned int i = start_index+new_edges.size(); 
    i <= end_index+new_edges.size(); ++i)
  {
    vtol_topology_object::unlink_inferior(
      this->edge(start_index+new_edges.size())->cast_to_topology_object());
  }

  // 3. Replace directions
  // 3.1 Add new directions
  this->directions()->insert(this->directions()->begin()+ start_index,
    directions.begin(), directions.end());

  // 3.2 Remove old directions
  start_index += new_edges.size();
  end_index += new_edges.size();

  this->directions()->erase(this->directions()->begin()+start_index,
    this->directions()->begin()+end_index+1);
  
  // 4. wrapping up
  this->touch();

  return true;
}

//*********************************************
// Binary I/O
//*********************************************



//: very brief description of the class
void dbsk2d_bnd_contour::
print(vcl_ostream &os) const
{
  os << "<" << this->is_a()<< "  " << 
    inferiors()->size() << " edges  " << 
    (void const *) this << 
    "   with id " << this->get_id() << ">\n";

}

//virtual void describe_directions(vcl_ostream &strm=vcl_cout, int blanking=0) const;
//virtual void describe(vcl_ostream &strm=vcl_cout, int blanking=0) const;






