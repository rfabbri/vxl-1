//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_node.h
//:
// \file
// \brief Basic 3d point sample on a mesh
//
//
// \author
//  MingChing Chang  Oct 29, 2007
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <dbmsh3d/dbmsh3d_node.h>
#include <dbmsh3d/dbmsh3d_curve.h>
#include <dbmsh3d/dbmsh3d_mesh.h>


int dbmsh3d_node::count_incident_Cs (int& r, int& a, int& nDege, int& nVirtual) const
{
  r = 0;
  a = 0;
  nDege = 0;
  nVirtual = 0;
  int nLoop = 0;

  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_curve* C = (dbmsh3d_curve*) cur->ptr();
    if (C->c_type() == C_TYPE_RIB) {
      if (C->is_self_loop()) {
        r += 2;
        nLoop++;
      }
      else
        r++;
    }
    else if (C->c_type() == C_TYPE_AXIAL) {
      if (C->is_self_loop()) {
        a += 2;
        nLoop++;
      }
      else
        a++;
    }
    else if (C->c_type() == C_TYPE_DEGE_AXIAL) {
      if (C->is_self_loop()) {
        nDege += 2;
        nLoop++;
      }
      else
        nDege++;
    }
    else {
      assert (C->c_type() == C_TYPE_VIRTUAL);
      nVirtual++;
    }
  }
  return nLoop;
}

bool dbmsh3d_node::has_rib_C () const
{
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_curve* C = (dbmsh3d_curve*) cur->ptr();
    if (C->c_type() == C_TYPE_RIB)
      return true;
  }
  return false;
}

bool dbmsh3d_node::is_dummy_node (dbmsh3d_curve* C1, dbmsh3d_curve* C2) const
{
  if (n_incident_Es() != 2)
    return false;

  C1 = (dbmsh3d_curve*) E_list_->ptr();
  C2 = (dbmsh3d_curve*) E_list_->next()->ptr();

  if (C1->c_type() == C_TYPE_RIB && C2->c_type() == C_TYPE_RIB)
    return true;
  else if (C1->c_type() == C_TYPE_AXIAL && C2->c_type() == C_TYPE_AXIAL)
    return true;
  else if (C1->c_type() == C_TYPE_DEGE_AXIAL && C2->c_type() == C_TYPE_DEGE_AXIAL)
    return true;
  else
    return false;
}


dbmsh3d_curve* dbmsh3d_node::find_C_containing_E (const dbmsh3d_edge* E) const
{  
  //Check the integrity of the coarse-scale and fine-scale elements.
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_curve* C = (dbmsh3d_curve*) cur->ptr();
    if (C->contain_E (E))
      return C;
  }
  return NULL;
}

void dbmsh3d_node::compute_n_type ()
{
  //A1A3: 1 axial, 1 rib.
  //A14: 4 axial.
  //Dege: other cases: >= 2 axials (e.g.: 1 rib + 1 dege_axial, ...).
  int r = 0;
  int a = 0;
  int nDege = 0;
  int nVirtual = 0;
  int nLoop = count_incident_Cs (r, a, nDege, nVirtual);

  if ((r==2 && a==0 && nDege==0) ||
      (r==0 && a==2 && nDege==0) ||
      (r==0 && a==0 && nDege==2)) {
    if (nLoop == 1)
      n_type_ = N_TYPE_LOOP_END;
    else {
      if (r==2)
        n_type_ = N_TYPE_RIB_END;
      else if (a==2)
        n_type_ = N_TYPE_AXIAL_END;
      else
        n_type_ = N_TYPE_DEGE_AXIAL_END;
    }
  }
  else if (nDege==0) {  
    if (r == 1 && a == 1)
      n_type_ = N_TYPE_RIB_END;
    else if (r == 0 && a == 4)
      n_type_ = N_TYPE_AXIAL_END;
    else {
      if (r != 0)
        n_type_ = N_TYPE_DEGE_RIB_END;
      else
        n_type_ = N_TYPE_DEGE_AXIAL_END;
    }
  }
  else {
    if (r != 0)
      n_type_ = N_TYPE_DEGE_RIB_END;
    else
      n_type_ = N_TYPE_DEGE_AXIAL_END;
  }  
}


bool dbmsh3d_node::check_integrity ()
{
  if (dbmsh3d_vertex::check_integrity() == false) {
    assert (0);
    return false;
  }
  if (V_->check_integrity() == false) {
    assert (0);
    return false;
  }
  
  //Check the integrity of the coarse-scale and fine-scale elements.
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_curve* C = (dbmsh3d_curve*) cur->ptr();
    if (C->data_type() == C_DATA_TYPE_EDGE) {
      if (C->s_N() == this) {
        if (C->E_vec(0)->is_V_incident (V_) == false) {
          assert (0);
          return false;
        }
      }
      else {
        assert (C->e_N() == this);
        if (C->E_vec(C->E_vec().size()-1)->is_V_incident (V_) == false) {
          assert (0);
          return false;
        }
      }
    }
    else if (C->data_type() == C_DATA_TYPE_VERTEX) {
      if (C->s_N() == this) {
        if (C->V_vec(0) != V_) {
          assert (0);
          return false;
        }
      }
      else { //MC->eV() == this
        assert (C->e_N() == this);
        if (C->V_vec(C->V_vec().size()-1) != V_) {
          assert (0);
          return false;
        }
      }
    }
    else { //no possible
      assert (0);
      return false;
    }
  }
  
  //Check integrity of node types:
  //A1A3: 1 axial, 1 rib.
  //A14: 4 axial.
  //Dege: other cases: >= 2 axials (e.g.: 1 rib + 1 dege_axial, ...).
  int r = 0;
  int a = 0;
  int nDege = 0;
  int nVirtual = 0;
  int nLoop = count_incident_Cs (r, a, nDege, nVirtual);

  if ((r==2 && a==0 && nDege==0) ||
      (r==0 && a==2 && nDege==0) ||
      (r==0 && a==0 && nDege==2)) {
    if (nLoop == 1) {
      if (n_type_ != N_TYPE_LOOP_END) {
        assert (0);
        return false;
      }
    }
    else {
      if (r==2) {
        if (n_type_ != N_TYPE_RIB_END) {
          assert (0);
          return false;
        }
      }
      else if (a==2) {
        if (n_type_ != N_TYPE_AXIAL_END) {
          assert (0);
          return false;
        }
      }
      else {
        if (n_type_ != N_TYPE_DEGE_AXIAL_END) {
          assert (0);
          return false;
        }
      }
    }
  }
  else if (nDege==0) {
    if (r == 1 && a == 1) {
      if (n_type_ != N_TYPE_RIB_END) {
        assert (0);
        return false;
      }
    }
    else if (r == 0 && a == 4) {
      if (n_type_ != N_TYPE_AXIAL_END) {
        assert (0);
        return false;
      }
    }
    else {
      if (r != 0) {
        if (n_type_ != N_TYPE_DEGE_RIB_END) {
          assert (0);
          return false;
        }
      }
      else {
        if (n_type_ != N_TYPE_DEGE_AXIAL_END) {
          assert (0);
          return false;
        }
      }
    }
  }
  else {    
    if (r != 0) {
      if (n_type_ != N_TYPE_DEGE_RIB_END) {
        assert (0);
        return false;
      }
    }
    else {
      if (n_type_ != N_TYPE_DEGE_AXIAL_END) {
        assert (0);
        return false;
      }
    }
  }

  return true;
}

//: The clone function clones everything except the connectivity
//  when cloning a graph, need to build the connectivity separetely.
dbmsh3d_vertex* dbmsh3d_node::clone () const
{
  dbmsh3d_node* N2 = new dbmsh3d_node (id_);
  return N2;
}

dbmsh3d_node* dbmsh3d_node::clone (dbmsh3d_mesh* M) const
{
  dbmsh3d_node* N2 = (dbmsh3d_node*) clone ();
  dbmsh3d_vertex* V2 = M->vertexmap (V_->id());
  N2->set_V (V2);
  return N2;
}

void dbmsh3d_node::getInfo (vcl_ostringstream& ostrm)
{
  //not yet implemented.
  assert (0);
}

//##########################################################
//  Handle Incident Virtual Curves
//##########################################################

unsigned int dbmsh3d_node::n_incident_Cs_nv () const
{
  unsigned int count = 0;
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_curve* C = (dbmsh3d_curve*) cur->ptr();
    if (C->c_type() != C_TYPE_VIRTUAL)
      count++;
  }
  return count;
}

unsigned int dbmsh3d_node::n_E_incidence_nv () const
{
  unsigned int count = 0;
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_curve* C = (dbmsh3d_curve*) cur->ptr();
    if (C->c_type() == C_TYPE_VIRTUAL)
      continue;
    if (C->vertices(0) == this)
      count++;
    if (C->vertices(1) == this)
      count++;
  }
  return count;
}

dbmsh3d_curve* dbmsh3d_node::find_C_containing_E_nv (const dbmsh3d_edge* E) const
{  
  //Check the integrity of the coarse-scale and fine-scale elements.
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_curve* C = (dbmsh3d_curve*) cur->ptr();
    if (C->c_type() == C_TYPE_VIRTUAL)
      continue;
    if (C->contain_E (E))
      return C;
  }
  return NULL;
}

void dbmsh3d_node::get_incident_Es_nv (vcl_set<void*>& incident_Es) const
{
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_curve* C = (dbmsh3d_curve*) cur->ptr();
    if (C->c_type() != C_TYPE_VIRTUAL)
      incident_Es.insert (C);
  }
}

//: return false if any incident_E is found not in E_set 
//  (skipping virtual curves).
bool dbmsh3d_node::all_incident_Es_in_set_nv (vcl_set<dbmsh3d_edge*>& E_set) const
{
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_curve* C = (dbmsh3d_curve*) cur->ptr();
    if (C->c_type() == C_TYPE_VIRTUAL)
      continue;
    if (E_set.find (C) == E_set.end())
      return false;
  }
  return true;
}


bool dbmsh3d_get_canonical_type (const int nA3, const int nA13, const int nDege, 
                                 int& m, int& n, int& f)
{
  //assume all dege. curves are A14 curves.
  //so equivalent to 2*nDege A13 curves.
  int a = nA13 + 2*nDege;
  m = 0;
  n = 0;
  f = 0;

  if (nA3 == 0) { //1) A1n
    assert (a % 2 == 0);
    n = a / 2 + 2;
    return true;
  }
  else if (nA3 == 1) { //2) A1mA3
    assert (a % 2);
    m = (a+1) / 2;
    return true;
  }
  else {
    if (nA3 == nA13) { //3) multiple loop end of A1A3-f.
      f = nA3;
      m = 1;
      return true;
    }
    else if (nA13 == 1) { //4) The corner type dege. A5 (two A3)
      //treated as A1A3-f for now.
      f = nA3;
      m = 1;
      return true;
    }
  }

  assert (0);
  return false;
}

dbmsh3d_curve* N_find_other_C_nv (const dbmsh3d_node* N, const dbmsh3d_curve* inputC)
{
  for (dbmsh3d_ptr_node* cur = N->E_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_curve* C = (dbmsh3d_curve*) cur->ptr();
    if (C == inputC)
      continue;
    if (C->c_type() == C_TYPE_VIRTUAL)
      continue;
    return C;
  }
  return NULL;
}


