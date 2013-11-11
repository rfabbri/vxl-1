// This is brcv/shp/dbsk2d/dbsk2d_ishock_node.cxx

//:
// \file

#include <vcl_cstdio.h>
#include <dbsk2d/dbsk2d_ishock_node.h>
#include <dbsk2d/dbsk2d_ishock_edge.h>
#include <dbsk2d/dbsk2d_ishock_contact.h>
#include <dbsk2d/dbsk2d_ishock_bpoint.h>

dbsk2d_ishock_node::dbsk2d_ishock_node (int newid, double stime, vgl_point_2d<double> Or) :
  dbsk2d_ishock_elm (newid, stime)
{
  _type = SNODE;
  _endTime = stime;
  _origin = Or;

  _cShock = NULL;
  _cShock2 = NULL;

  _bActive = false; //nodes are propagated as soon as they form
}

dbsk2d_ishock_node::~dbsk2d_ishock_node ()
{
  //Remove link from the boundary elements to this shock
  //no need since links to nodes have not been added to the boundary elements
  /*ishock_node_belm_list_iter curB = _bndList.begin();
   for(; curB!=_bndList.end(); ++curB) {
      (curB->second)->delete_shock (this);
  }*/
  _bndList.clear();
  _pShockList.clear();

  _cShock = NULL;
  _cShock2 = NULL;

}

void dbsk2d_ishock_node::add_pShock(dbsk2d_ishock_edge* pshock, 
                                    bool in_front)
{
  if (in_front)
    _pShockList.push_front(pshock);
  else
    _pShockList.push_back(pshock);
}

void dbsk2d_ishock_node::remove_pShock(dbsk2d_ishock_edge* pshock)
{
  //remove this edge from the nodes' parent list
  ishock_edge_list::iterator curS = _pShockList.begin();
  for(; curS!=_pShockList.end(); ++curS){
    dbsk2d_ishock_edge* shock = (*curS);
    if (shock == pshock){
      _pShockList.remove(shock);
      return;
    }
  }
}

//: return the unpruned parent of this node
// assumes that this is only called when this node is a
//pruned junction
dbsk2d_ishock_edge* dbsk2d_ishock_node::get_parent_edge (void)
{
  if (indeg(true) != 1) //make sure this is a pruned junction
    return 0;

  ishock_edge_list::iterator curS = _pShockList.begin();
  for(; curS!=_pShockList.end(); ++curS)
    if (!(*curS)->is_a_contact() && !(*curS)->isHidden())
      return (*curS);
  
  return 0;
}

int dbsk2d_ishock_node::indeg(bool exclude_hidden)
{
  if (!exclude_hidden)
    return _pShockList.size();

  int nNotHidden = 0;
  ishock_edge_list::iterator curS = _pShockList.begin();
  for(; curS!=_pShockList.end(); ++curS){
    if (!(*curS)->is_a_contact() &&  !(*curS)->isHidden())
      nNotHidden++;
  }

  return nNotHidden;
} 

int dbsk2d_ishock_node::outdeg(bool exclude_hidden)
{
  int deg = 0;
  if (_cShock){
    if (!exclude_hidden || _cShock->isNotHidden())
      deg++;
  }
  if (_cShock2){
    if (!exclude_hidden || _cShock2->isNotHidden())
      deg++;
  }
  return deg;
} 

//: return the number of edges adjacent to this node
int dbsk2d_ishock_node::degree(bool exclude_hidden) 
{ 
  return outdeg(exclude_hidden) + indeg(exclude_hidden);
}

//: return the edges connected to this node;
ishock_edge_list 
dbsk2d_ishock_node::adj_edges(bool exclude_hidden)
{
  ishock_edge_list adj_edge_list;
  adj_edge_list.clear();

  if (_cShock){
    if (!exclude_hidden || _cShock->isNotHidden())
      adj_edge_list.push_back(_cShock);
  }
  if (_cShock2){
    if (!exclude_hidden || _cShock2->isNotHidden())
      adj_edge_list.push_back(_cShock2);
  }

  for (ishock_edge_list::iterator it = _pShockList.begin();
    it != _pShockList.end(); it ++)
  {
    if (!exclude_hidden || (*it)->isNotHidden())
      adj_edge_list.push_back((*it));
  }
  return adj_edge_list;
}

void dbsk2d_ishock_node::compute_extrinsic_locus()
{  
  //clear existing points and recompute in case it was modified
  ex_pts_.clear();
  ex_pts_.push_back(_origin);
}

void dbsk2d_ishock_node::getInfo (vcl_ostream& ostrm)
{
  char s[1024];

  ostrm << "\n==============================\n";

  //node type (based on connectivity)
  if (this->is_an_A3source())
    ostrm << "A3: ";
  if (this->is_a_source())
    ostrm << "SRC: ";
  if (this->is_a_junct())
    ostrm << "JUNCT: ";
  if (this->is_a_sink())
    ostrm << "SINK: ";

  ostrm << "[" << _id << "] " << vcl_endl; 
  vcl_sprintf(s, "Origin : (%.3f, %.3f)\n", _origin.x(), _origin.y()); ostrm << s;
  vcl_sprintf(s, "{ ts=%.7f}\n", _startTime); ostrm << s <<vcl_endl;

  vcl_cout << "PS: [";
  for(ishock_edge_list::iterator curS = _pShockList.begin();
      curS != _pShockList.end(); ++curS)
  {
    vcl_cout << (*curS)->id() << " ";
  }
  ostrm << "]" << vcl_endl;
  if (_cShock)
    ostrm << "cS1: " << _cShock->id();
  if (_cShock2)
    ostrm << ", cS2: " << _cShock2->id(); 
  vcl_cout << vcl_endl;

/*
   s.Printf ("nBElement: %d\n", nBElement()); buf+=s;
   s.Printf ("BElements Linked to this Shock: "); buf+=s;
   belm_list::iterator curB = bndList.begin();
   for(; curB!=bndList.end(); ++curB) {
      s.sprintf ("%d, ", (*curB)->id()); buf+=s;
  }
   s.Printf ("\n \n"); buf+=s;

   s.Printf ("PSElements Linked to this Shock: "); buf+=s;
   ishock_edge_list::iterator curS = _pShockList.begin();
  for(; curS!=_pShockList.end(); ++curS){
      s.Printf ("%d, ", (*curS)->id()); buf+=s;
  }
   s.Printf ("\n \n"); buf+=s;

  if (MessageOption >= MSG_TERSE) {
    s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
    s.Printf ("bIOVisited: %s\n", bIOVisited ? "yes" : "no"); buf+=s;
    s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
    s.Printf ("bHidden: %s\n \n", _bHidden ? "yes" : "no"); buf+=s;

    s.Printf ("PruneCost: %.3f\n", _dPnCost);buf+=s;
    s.Printf ("dOC: %.3f\n", _dOC);buf+=s;
    s.Printf ("dNC: %.3f\n", _dNC);buf+=s;
  }

  ostrm << buf;
*/
}


