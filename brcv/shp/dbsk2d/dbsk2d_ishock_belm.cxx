// This is brcv/shp/dbsk2d/dbsk2d_ishock_belm.cxx

//:
// \file

#include <dbsk2d/dbsk2d_ishock_belm.h>
#include <dbsk2d/dbsk2d_ishock_bpoint.h>
#include <dbsk2d/dbsk2d_ishock_bline.h>
#include <dbsk2d/dbsk2d_ishock_barc.h>

#include <dbsk2d/dbsk2d_ishock_node.h>
#include <dbsk2d/dbsk2d_ishock_edge.h>

bool dbsk2d_ishock_belm::throw_exception=true;

//: Constructor - type 1
dbsk2d_ishock_belm::dbsk2d_ishock_belm(int id) : dbsk2d_base_gui_geometry(),
  _id(id), _bGUIElm(false), _type(BPOINT)
{
  shock_map_.clear(); //make sure the shocklist is empty 
}

//: Constructor - type 2
dbsk2d_ishock_belm::
dbsk2d_ishock_belm(int id, bool bGUI, belm_type type): 
dbsk2d_base_gui_geometry(), _id(id), _bGUIElm(bGUI), _type(type)
{
  shock_map_.clear(); //make sure the shocklist is empty 
}

//: add the shock into the shock map at the slot defined by its boundary params
// Note:
//  This function is also responsible for maintaining the shock neighborhood
//  relations (in a way that explicit wavefronts would have done)
//
//  [[shock neighbors are shocks that share the same wavefront on the 
//  common side and thus also function to bound the wavefront.]]
void dbsk2d_ishock_belm::add_shock (dbsk2d_ishock_edge* shock) 
{
  dbsk2d_ishock_bnd_key::shock_type Stype;
  double start_eta;
  
  if (this==shock->lBElement()) {
    start_eta = shock->LsEta();
    if (shock->is_a_contact())
      Stype=dbsk2d_ishock_bnd_key::LEFTCONTACT;
    else
      Stype=dbsk2d_ishock_bnd_key::LEFT;
  }
  else {
    start_eta = shock->RsEta();
    if (shock->is_a_contact())
      Stype=dbsk2d_ishock_bnd_key::RIGHTCONTACT;
    else
      Stype=dbsk2d_ishock_bnd_key::RIGHT;
  }
  
  //make sure this is legal
  //dbsk2d_assert(AisBetween(start_eta, min_eta(), max_eta()));
  THROW_TOPOLOGY_EXCEPTION(AisBetween(start_eta, min_eta(), max_eta()), "Add shock: invalid eta");

  //insert it into the map
  bnd_ishock_map_iter it = shock_map_.insert(vcl_pair<dbsk2d_ishock_bnd_key, dbsk2d_ishock_edge*>(
    dbsk2d_ishock_bnd_key(start_eta, Stype, dbsk2d_ishock_bnd_key::ANGLE), shock)); 

  //if this shock is left-type the element to the left is the neighbor
  if (it->first.is_left_type()){
    if (it!=shock_map_.begin()){ //there are some elements to the left
      it--;
      //we need to check if this iterator is pointing to a valid type shock
      //dbsk2d_assert(it->first.is_right_type());
      if ( throw_exception )
      {
          THROW_TOPOLOGY_EXCEPTION(it->first.is_right_type(), 
                                   "Add shock: invalid topology");
      }

      shock->set_lShock(it->second); //set the neighbors
      it->second->set_rShock(shock);
    }
  }
  else { //the shock is right-type: the element to the right is the neighbor
    it++;
    if (it!=shock_map_.end()){
      //we need to check if this iterator is pointing to a valid type shock
      //dbsk2d_assert(it->first.is_left_type());
        if ( throw_exception)
        {
            THROW_TOPOLOGY_EXCEPTION(it->first.is_left_type(), 
                                     "Add shock: invalid topology");
        }
      shock->set_rShock(it->second);//set the neighbors
      it->second->set_lShock(shock);
    }
  }
}

//: delete this shock from the shock map
bool dbsk2d_ishock_belm::delete_shock (dbsk2d_ishock_edge* shock) 
{ 
  //just exhaustively search the map to delete this shock
  //I don't remember why?
  bnd_ishock_map_iter curS = shock_map_.begin();
  for (; curS!=shock_map_.end(); ++curS)
  {
    //look for the shock
    if (curS->second==shock)
    {
      bnd_ishock_map_iter it = curS;

      //First, update wavefront relations between remaining shocks

      //1) If this is a left-type shock, and if the element to its right
      //   is also a left-type shock, then the latter is the new neighbor 
      //   of this shock's neighbor. However, this should only be updated
      //   if the neighbor is currently sharing the wavefront with this shock.
      if (this==shock->lBElement())
      {
        if (shock->lShock()){
          if (shock->lShock()->rShock()==shock){//only update if it is correctly paired
            it++;
            if (it!=shock_map_.end()){ //has some elements to the right
              if (it->first.is_left_type()){
                shock->lShock()->set_rShock(it->second); //update both neighbors
                it->second->set_lShock(shock->lShock());
              }
              else
                shock->lShock()->set_rShock(0); //update the left neighbor
            }
            else //has no elements to the right
              shock->lShock()->set_rShock(0); //update its left neighbor
          }
        }
      }
      else { //this is a right-type shock
        if (shock->rShock()){
          if (shock->rShock()->lShock()==shock){//only update if it is correctly paired
            if (it!=shock_map_.begin()){ //has some elements to the left
              it--;
              if (it->first.is_right_type()){
                shock->rShock()->set_lShock(it->second); //update both its neighbors
                it->second->set_rShock(shock->rShock());
              }
              else
                shock->rShock()->set_lShock(0); //update its right neighbor
            }
            else //has no elements to the left
              shock->rShock()->set_lShock(0); //update its right neighbor
          }
        }
      }

      //now this shock can be removed from the map
      shock_map_.erase(curS);
      return true;
    }
  }
  return false; //delete unsuccessful
}


//: delete this shock from the shock map
void dbsk2d_ishock_belm::delete_lr_refs (dbsk2d_ishock_edge* shock) 
{ 
  bnd_ishock_map_iter curS = shock_map_.begin();
  for (; curS!=shock_map_.end(); ++curS)
  {
      if ( curS->second->lShock() == shock )
      {
          curS->second->set_lShock(0);
      }
      else if(curS->second->rShock()==shock )
      {
          curS->second->set_rShock(0);
      }   
  }
}

//: Test if a particular point on the wavefront is still valid
// Note:
//  The time parameter defines the particular wavefront and the eta
//  parameter defines this point on the wavefront wrt the belm
//
// \todo We need separate radius functions for left and right tau.
// Conversion is turning out to be very noisy and unreliable!
bool dbsk2d_ishock_belm::is_wavefront_alive(double eta, double time)
{
  if ( _isL(eta,min_eta(), DOUBLE_PRECISION) || _isG(eta, max_eta(), DOUBLE_PRECISION))
    return false; //point does not lie on this wavefront

  bool degen_eta = false;

  //now get the shock at this eta
  dbsk2d_ishock_edge* shock = get_shock_at(eta, degen_eta);

  if (degen_eta)  //if this is a degenerate point, ignore
    return false;

  if (!shock) //this part of the wavefront is not quenched yet!
    return true; 

  //a shortcut (avoids computation of radius)
  //if this shock has a child node, and its endtime is earlier than the 
  //query time, we know that the wavefront has to have been quenched
  if (shock->cSNode() && RisLEq(shock->endTime(), time))
    return false;

  ////if only a contact exists at the specified point, wavefront is still alive
  ////because contacts don't quench any wavefronts!
  //if (shock->is_a_contact())
  //  return true;

  //else we need to compare the radius of this shock at this eta
  //to determine if the wavefront is alive
  double r;
  if (this==shock->lBElement()){
    double ltau = shock->LEtaToLTau(eta, false);

    //make sure the shock is valid at this eta
    if (!shock->isLTauValid_MinMax(ltau))
      return false; 

    r = shock->rFromLTau(ltau);
  }
  else {
    double rtau = shock->REtaToRTau(eta, false);

    //make sure the shock is valid at this eta
    if (!shock->isRTauValid_MinMax(rtau))
      return false; 

    r = shock->rFromRTau(rtau);
  }

  //Exceptions to the general rule below
  //
  //a) Degenerate shocks: LineLineThirdorder and ArcArcThirdorder
  //  - The radius stays constant throughout its length
  //    so relax the condition and only invalidate if the query time
  //    is definitely less than the radius of the shock
  //b) Rule (a) might allow multiple sources to be validated at the 
  //   starting point so only apply rule (a) if we're away from the
  //   starting point of this degenerate shock

  if (shock->is_third_order())
  {
    if (RisL(r, time)) //rule (a)
      return false;

    double seta;
    if (this==shock->lBElement())
      seta = shock->LsEta();
    else
      seta = shock->RsEta();

    if (RisEq(r, time) && LisEq(eta, seta)) //rule (b)
      return false;
    else
      return true;
  }

  //General rule (applies for all regular shocks)
  //if the shock is already there at the query time,
  //the wavefront has been quenched
  if (RisLEq(r, time))
    return false;
  else
    return true;
}

//: Return the shock at a given point (eta) on the wavefront
// It will return the shock that quenched the wavefront at the
// specified eta. If the wavefront at this eta hasn't been quenched yet,
// it should return NULL
//
// Note: it might return contact shocks for lines and arcs
dbsk2d_ishock_edge* dbsk2d_ishock_belm::get_shock_at(double eta, bool & degen_eta)
{
  degen_eta = false; //make sure this is the default case

  if ( _isL(eta,min_eta(), DOUBLE_PRECISION) || _isG(eta, max_eta(), DOUBLE_PRECISION))
    return 0; //point does not lie on this wavefront

  //query the shock_map for a shock with eta >= query eta
  bnd_ishock_map_iter kit = shock_map_.lower_bound(
    dbsk2d_ishock_bnd_key(eta, dbsk2d_ishock_bnd_key::QUERY, dbsk2d_ishock_bnd_key::ANGLE));

  if (kit == shock_map_.end()){ //there are no shocks higher than this
    degen_eta = true; //only possible in singular points (all others should have contacts)
    return 0; 
  }

  //lower bound shock
  dbsk2d_ishock_edge* shock = kit->second;
 
  if (kit->first.type==dbsk2d_ishock_bnd_key::RIGHT){
    //return this shock if the query eta is exactly equal to its start eta
    if (AisEq(eta, shock->RsEta()))
      return shock; 
    else { //else we have a hole due to degeneracy
      degen_eta = true;
      return 0;
    }
  }

  if (kit->first.type==dbsk2d_ishock_bnd_key::LEFT){
    //return this shock if eta is within its end eta
    if (AisGEq(eta, shock->LeEta()))
      return shock;
  }

  //If none of the tests passed, we need to check the shock before this one: 
  if (kit==shock_map_.begin()){     //there are no more shocks before this
    degen_eta = true; //only possible in singular points (all others should have contacts)
    return 0; 
  }
  else { //has some shocks to the left
    //decrement the iterator
    kit--;

    //this shock is typically of type RIGHT,
    //but if there is a hole here due to the presence of degenerate node, 
    //it is going to be of type LEFT. In this case set the degen_eta flag.
    if (kit->first.is_left_type() && shock->is_a_contact()){ //does the shock have to be a contact???
      degen_eta = true;
      return 0;
    }

    //get the shock on the left
    shock = kit->second;

    //return this one if eta is within its end eta
    if (AisLEq(eta, shock->eEta(kit->first.type)))
      return shock;
    else
      return 0; //there are no shocks at this eta
  }
}

//: return the shock starting to the left of this eta
// This is similar to what the add_shock function does while determining shock 
// neighbors, except this needs to do it with just the eta value. 
// It is to be used to detect degeneracies while propagating from a node.
dbsk2d_ishock_edge* dbsk2d_ishock_belm::get_left_neighboring_shock_at(double eta)
{
  if ( _isL(eta,min_eta(), DOUBLE_PRECISION) || _isG(eta, max_eta(), DOUBLE_PRECISION))
    return 0; //point does not lie on this wavefront

  //query the shock_map for a shock with eta >= query eta
  bnd_ishock_map_iter kit = shock_map_.lower_bound(
    dbsk2d_ishock_bnd_key(eta, dbsk2d_ishock_bnd_key::LEFT, dbsk2d_ishock_bnd_key::ANGLE));

  // the element to the left is the neighbor
  if (kit!=shock_map_.begin()){ //there are some elements to the left
    kit--;
    //we need to check if this iterator is pointing to a valid type shock
    //dbsk2d_assert(kit->first.is_right_type());
    if ( throw_exception )
    {
        THROW_TOPOLOGY_EXCEPTION(kit->first.is_right_type(), "get left-neighbor");
    }

    return kit->second;
  }
  else
    return 0;
}

//: return the shock starting to the right of this eta
dbsk2d_ishock_edge* dbsk2d_ishock_belm::get_right_neighboring_shock_at(double eta)
{
  if ( _isL(eta,min_eta(), DOUBLE_PRECISION) || _isG(eta, max_eta(), DOUBLE_PRECISION))
    return 0; //point does not lie on this wavefront

  //query the shock_map for a shock with eta >= query eta
  bnd_ishock_map_iter kit = shock_map_.lower_bound(
    dbsk2d_ishock_bnd_key(eta, dbsk2d_ishock_bnd_key::RIGHT, dbsk2d_ishock_bnd_key::ANGLE));

  //the element to the right (which it should be pointing to) is the neighbor
  if (kit!=shock_map_.end()){
    //we need to check if this iterator is pointing to a valid type shock
    //dbsk2d_assert(kit->first.is_left_type());
      if ( throw_exception )
      {
          THROW_TOPOLOGY_EXCEPTION(kit->first.is_left_type(), 
                                   "get right-neighbor");
      }
    return kit->second;
  }
  else
    return 0;
}

//: check to see if the current shock invalidates its neighbor
bool dbsk2d_ishock_belm::
shock_invalidates_neighbor(dbsk2d_ishock_edge* shock, 
                           dbsk2d_ishock_edge* neighbor)
{
  //if they came from the same source, they don't invalidate each other 
  //even though the numbers say so
  if (shock->pSNode()!=0 && shock->pSNode()==neighbor->pSNode())
    return false;

  //Special treatment fo thirdorder shocks
  if (shock->is_third_order() && neighbor->is_third_order())
    return false;

  //first convert the eta of the neighbor to the tau of the current shock
  //then compute its radius at that tau
  double R;
  if (this==shock->lBElement()){
    double ltau = shock->LEtaToLTau(neighbor->RsEta(),UNCONSTRAINED);

    //now check to see if this tau is valid,
    //if it's not valid, we cannot say for sure 
    if (!shock->isLTauValid_MinMax(ltau))
      return false;

    //compute the radius at this tau
    R = shock->rFromLTau(ltau);
  }
  else {
    double rtau = shock->REtaToRTau(neighbor->LsEta(),UNCONSTRAINED);

    //now check to see if this tau is valid,
    //if it's not valid, we cannot say for sure 
    if (!shock->isRTauValid_MinMax(rtau))
      return false;

    //compute the radius at this tau
    R = shock->rFromRTau(rtau);
  }
  
  //we can only say with certainty that it invalidates its neighbor
  //if it satisfies this condition
  if (R <= neighbor->startTime())  //Note: fuzzy test causes some problems
    return true;

  return false; //default (can't say for sure)
}

//: return a list of belements it interacts with
void dbsk2d_ishock_belm::get_interacting_belements(belm_list & belmList)
{
  dbsk2d_ishock_belm* last_BElm=0;

  //traverse the bnd_ishock_map and record all the elements 
  bnd_ishock_map_iter curS = shock_map_.begin();
  for (; curS!=shock_map_.end(); ++curS)
  {
    dbsk2d_ishock_belm* cur_BElm=0;
    if (curS->first.is_right_type())
      cur_BElm = curS->second->lBElement();
    else
      cur_BElm = curS->second->rBElement();

    //if belm not already on the list, add it
    if (cur_BElm != last_BElm){
      belmList.push_back(cur_BElm);
      last_BElm = cur_BElm;
    }
  }
}




