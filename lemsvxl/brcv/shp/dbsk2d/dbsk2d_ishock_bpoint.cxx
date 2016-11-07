// This is brcv/shp/dbsk2d/dbsk2d_ishock_bpoint.cxx

//:
// \file
#include <vcl_cstdio.h>
#include <dbsk2d/dbsk2d_ishock_bpoint.h>

#include <dbsk2d/dbsk2d_ishock_bline.h>
#include <dbsk2d/dbsk2d_ishock_barc.h>
#include <dbsk2d/dbsk2d_ishock_contact.h>
#include <dbsk2d/dbsk2d_bnd_vertex.h>

dbsk2d_ishock_bpoint::
dbsk2d_ishock_bpoint(double x, double y, int id, bool bGUI, double tangent, double conf) :
  dbsk2d_ishock_belm(id), _pt(x,y), _dir(tangent), _conf(conf)
{
  _type = BPOINT;
  _bGUIElm = bGUI;
  _vref = -1.0; //not set yet
  _max_eta = 2*vnl_math::pi; //default (this will be reduced if contacts are involved)
  _is_visible = true;

  compute_extrinsic_locus();
}
  
  
//: Constructor
dbsk2d_ishock_bpoint::    
dbsk2d_ishock_bpoint(dbsk2d_ishock_bpoint* old_bp, int new_id):
  dbsk2d_ishock_belm(new_id)
{
  this->_pt = old_bp->_pt;
  this->_dir = old_bp->_dir;
  this->_conf = old_bp->_conf;
  this->_type = old_bp->_type;
  this->_bGUIElm = old_bp->_bGUIElm;
  this->_vref = old_bp->_vref;
  this->_max_eta = old_bp->_max_eta;
  this->_is_visible = old_bp->_is_visible;
  
  //
  this->compute_extrinsic_locus();
}





// -----------------------------------------------------------------------
//: Set the extrinsic point's coordinate
void dbsk2d_ishock_bpoint::
set_pt(double new_x, double new_y)
{ 
  _pt.set(new_x, new_y); 
  this->compute_extrinsic_locus();
  // touch the topology objects
  this->bnd_vertex()->touch_and_propagate();

  // update shock map and belm connected to this point
  this->shock_map().clear();
  for (belm_list::iterator bit = this->LinkedBElmList.begin();
    bit != this->LinkedBElmList.end(); ++bit)
  {
    if ((*bit)->is_a_curve())
    {
      dbsk2d_ishock_bcurve* bcurve = static_cast<dbsk2d_ishock_bcurve* >(*bit);
      bcurve->compute_cached_params();
      bcurve->compute_extrinsic_locus();
    }
  }
}

bool dbsk2d_ishock_bpoint::has_a_tangent() const
{
  if (is_a_free_point())
    return _dir!=TANGENT_UNDEFINED;
  else if (is_an_end_point())
    return true;
  else
    return false;
}

//: left contact shock
dbsk2d_ishock_contact* dbsk2d_ishock_bpoint::lContact()
{
  if (shock_map_.size()==0)
    return 0;

  if (is_a_free_point()){
    return 0;
  }
  else {
    //make sure this is legal
    if (shock_map_.begin()->first.type==dbsk2d_ishock_bnd_key::RIGHTCONTACT) 
      return (dbsk2d_ishock_contact*) shock_map_.begin()->second;
    else
      return 0;
  }
}

//: right contact shock
dbsk2d_ishock_contact* dbsk2d_ishock_bpoint::rContact()
{ 
  if (shock_map_.size()==0)
    return 0;

  if (is_a_free_point()){
    return 0;
  }
  else {
    //make sure this is legal
    if (shock_map_.rbegin()->first.type==dbsk2d_ishock_bnd_key::LEFTCONTACT)
      return (dbsk2d_ishock_contact*) shock_map_.rbegin()->second;
    else 
      return 0;
  }
}

//: Add the shock into the shock map at the slot defined by its boundary params
//Note:
//  This function is also responsible for maintaining the shock neighborhood
//  relations (in a way that explicit wavefronts would have done)
//
//  [[shock neighbors are shocks that share the same wavefront on the 
//  common side and thus also function to bound the wavefront.]]
// 
//  Non-singular points have the same wavefront structure as the other curves
//  So these can be handled in the conventional manner
//  Singular points need to be handled carefully, respecting the circular map
void dbsk2d_ishock_bpoint::add_shock (dbsk2d_ishock_edge* shock) 
{
  if (!this->is_a_free_point())
    return dbsk2d_ishock_belm::add_shock(shock);

  dbsk2d_ishock_bnd_key::shock_type Stype;
  double start_eta;
  
  if (this==shock->lBElement()) 
  {
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
  
  //insert it into the map
  bnd_ishock_map_iter it = shock_map_.insert(vcl_pair<dbsk2d_ishock_bnd_key, dbsk2d_ishock_edge*>(
    dbsk2d_ishock_bnd_key(start_eta, Stype, dbsk2d_ishock_bnd_key::ANGLE), shock)); 

  //if this shock is left-type the element to the left is the neighbor
  if (it->first.is_left_type()){
    if (it!=shock_map_.begin()){ //there are some elements to the left
      it--;
      //we need to check if this iterator is pointing to a valid type shock
      //dbsk2d_assert(it->first.is_right_type());
      THROW_TOPOLOGY_EXCEPTION(it->first.is_right_type(), "Add shock");

      shock->set_lShock(it->second); //set the neighbors
      it->second->set_rShock(shock);
    }
    else { 
      //treat points as having a circular list 
      if (shock_map_.size()>1){ //it can't be it's own neighbor
        //the last element in the map is the new neighbor
        bnd_ishock_map_riter r_it = shock_map_.rbegin();

        //we need to check if this iterator is pointing to a valid type shock
        //dbsk2d_assert(r_it->first.type == dbsk2d_ishock_bnd_key::RIGHT);
        THROW_TOPOLOGY_EXCEPTION(r_it->first.type == dbsk2d_ishock_bnd_key::RIGHT, "Add shock");

        shock->set_lShock(r_it->second);//set the neighbors
        r_it->second->set_rShock(shock);
      }
    }
  }
  else { //the shock is right-type: the element to the right is the neighbor
    it++;
    if (it!=shock_map_.end()){
      //we need to check if this iterator is pointing to a valid type shock
      //dbsk2d_assert(it->first.is_left_type());
      THROW_TOPOLOGY_EXCEPTION(it->first.is_left_type(), "Add shock");

      shock->set_rShock(it->second);//set the neighbors
      it->second->set_lShock(shock);
    }
    else { 
      //treat points as having a circular list
      if (shock_map_.size()>1){ //it can't be it's own neighbor
        //the first element in the map is the new neighbor
        bnd_ishock_map_iter it2 = shock_map_.begin();

        //we need to check if this iterator is pointing to a valid type shock
        //dbsk2d_assert(it2->first.type == dbsk2d_ishock_bnd_key::LEFT);
        THROW_TOPOLOGY_EXCEPTION(it2->first.type == dbsk2d_ishock_bnd_key::LEFT, "Add shock");

        shock->set_rShock(it2->second);//set the neighbors
        it2->second->set_lShock(shock);
      }
    }
  }
}

//: delete this shock from the shock map
//  Non-singular points have the same wavefront structure as the other curves
//  So these can be handled in the conventional manner
//  Singular points need to be handled carefully, respecting the circular map
//
// \todo fix the neighbor updates after deletes
bool dbsk2d_ishock_bpoint::delete_shock (dbsk2d_ishock_edge* shock) 
{  
  if (!this->is_a_free_point())
    return dbsk2d_ishock_belm::delete_shock(shock);

  //just exhaustively search the map to delete this shock
  //I don't remember why?
  bnd_ishock_map_iter curS = shock_map_.begin();
  for (; curS!=shock_map_.end(); ++curS)
  {
    //look for the shock
    if (curS->second==shock)
    {
      bnd_ishock_map_iter it = curS;

      //A) First, update wavefront relations between remaining shocks

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

      //B) now this shock can be removed from the map
      shock_map_.erase(curS);
      
      //C) reset vref when there are no more shocks left (only for points)
      if (shock_map_.size()==0)
        _vref = -1.0;

      return true;
    }
  }

  return false; //delete unsuccessful
}

//: convert from vector to the eta parameter for this point
double dbsk2d_ishock_bpoint::vec_to_eta(VECTOR_TYPE vec)
{
  //vref is set by the first shock source that is created by this boundary
  if (_vref == -1.0){
    _vref = vec;
    return 0;
  }
  else
    return CCW(vec, _vref); 
}

//: convert from eta to vector for this point
VECTOR_TYPE dbsk2d_ishock_bpoint::eta_to_vec(double eta)
{
  //if vref has not been set this eta is still the raw vector
  if (_vref == -1.0)
    return eta;
  else
    return angle0To2Pi(_vref - eta); //because eta is a CW parameter 
}

bool dbsk2d_ishock_bpoint::is_wavefront_alive(double vec, double time)
{
  //for degenerate points between two co-linear lines (which are just not visible)
  if (!_is_visible)
    return false;

  //if the eta is not defined yet, the wavefront is certainly alive
  if (_vref == -1.0)
    return true;

  double cur_eta = CCW(vec, _vref);

  //else use the standard belm test
  return dbsk2d_ishock_belm::is_wavefront_alive(cur_eta, time);
}

//: Return the shock at a given point (eta) on the wavefront
// It will return the shock that quenched the wavefront at the
// specified eta. If the wavefront at this eta hasn't been quenched yet,
// it should return NULL
//
// Non-singular points behave in the same way as bcurves. 
// Their wavefront data can be represented by a flat list and they
// have contact shocks to bound them.  
//For singular points, we need to handle this method with the circular map in mind.
dbsk2d_ishock_edge* dbsk2d_ishock_bpoint::get_shock_at(double eta, bool & degen_eta)
{
  if (!this->is_a_free_point())
    return dbsk2d_ishock_belm::get_shock_at(eta, degen_eta);

  degen_eta = false; //make sure this is the default case

  if ( _isL(eta,min_eta(), DOUBLE_PRECISION) || _isG(eta, max_eta(), DOUBLE_PRECISION))
    return 0; //point does not lie on this wavefront

  //if there are no other shocks in the shock_map_, there's no point looking for one
  if (shock_map_.size()==0)
    return 0;

  //query the shock_map for a shock with eta >= query eta
  bnd_ishock_map_iter kit = shock_map_.lower_bound(
    dbsk2d_ishock_bnd_key(eta, dbsk2d_ishock_bnd_key::QUERY, dbsk2d_ishock_bnd_key::ANGLE));

  //this is a circular list so go around if necessary
  if (kit == shock_map_.end())
    kit = shock_map_.begin();

  //lower bound shock
  dbsk2d_ishock_edge* shock = kit->second;
 
  if (kit->first.type==dbsk2d_ishock_bnd_key::RIGHT){
    //return this shock if the query eta is exactly equal to its start eta
    if (AisEq02Pi(eta, shock->sEta(dbsk2d_ishock_bnd_key::RIGHT)))
      return shock; 
    else { //else we have a hole due to degeneracy
      degen_eta = true;
      return 0;
    }
  }

  if (kit->first.type==dbsk2d_ishock_bnd_key::LEFT){
    //return this shock if eta is within its end eta
    //this test needs to be updated for a circular list
    if (AisGEq(eta, shock->eEta(dbsk2d_ishock_bnd_key::LEFT)))
      return shock;
  }

  //If none of the tests passed, we need to check the shock before this one: 
  //this is a circular list so go around if necessary
  if (kit == shock_map_.begin())
    kit = shock_map_.end();

  //now get the earlier shock
  kit--;

  //this shock should be of type RIGHT, else there is a hole
  //due to degeneracy
  if (!kit->first.type==dbsk2d_ishock_bnd_key::RIGHT){
    degen_eta = true;
    return 0;
  }

  //get the shock
  shock = kit->second;

  //return this shock if eta is within its end eta
  //this test needs to be updated for a circular list
  if (AisLEq(eta, shock->eEta(kit->first.type)))
    return shock;
  else
    return 0; //there are no shocks at this eta
}

//: Connect this to a line or arc type by putting
// the element into the appropriate slot of its LinkedBElmList
//------------------------------------------------------------
//
// The LinkedBElmList is CCW ordered.  
// And the ordering rules are:
//      BLINE: if this is the endpoint it goes first
//      BARC:  if this is the startpt, the CCW arc goes first
//             if this is the endpt, the CW arc goes first
//------------------------------------------------------------
// \todo 
// However, we would not want the bpoint class to have to know 
// about all the other belm classes so should just send it the
// information it requires for ordering from the belm classes
// that are connecting to it.
//
void dbsk2d_ishock_bpoint::connectTo(dbsk2d_ishock_belm* elm)
{
  if (!(elm->is_a_line() || elm->is_an_arc()))
    return;

  double CCWAngle=0;
  bool  bStartPt = (this==elm->s_pt());

  //get the tangent angle of the element to be added
  if (elm->is_a_line()){
    if (bStartPt) CCWAngle = ((dbsk2d_ishock_bline*)elm)->u();
    else CCWAngle = ((dbsk2d_ishock_bline*)elm)->u() + vnl_math::pi;
  }
  else if (elm->is_an_arc()){
    if (bStartPt)
      CCWAngle = ((dbsk2d_ishock_barc*)elm)->InTangentAtStartPt();
    else 
      CCWAngle = ((dbsk2d_ishock_barc*)elm)->InTangentAtEndPt();
  }
  CCWAngle = angle0To2Pi(CCWAngle);

  // this is just an experiment: 
  // put this angle in as the tangent dir of the point
  // (For end points that need to be linked to other end
  // points by completion curves this will provide the 
  // necessary point-tangent information. However, this 
  // information cannot be trusted)
  _dir = CCWAngle;

  //if there are no elements yet 
  if (LinkedBElmList.size()==0){
    LinkedBElmList.push_back(elm);
    //No ContactShock issue here.
    return;
  }

  // Now traverse the existing list of connected elements to insert
  // into the appropriate slot
  // \todo  Tangential elements: If the new element being inserted is tangent
  // to an existing line there is a problem but if the element being inserted is
  // an arc, it's fine but the order depends on the curvature of the arc 

  double curCCWAngle=0;
  belm_list::iterator curB = LinkedBElmList.begin();
  for(; curB!=LinkedBElmList.end(); ++curB) {
    dbsk2d_ishock_belm* curbElm = (*curB);

    bool  bCurStartPt = (this==curbElm->s_pt());
    
    if (curbElm->is_a_line()){
      if (bCurStartPt)
        curCCWAngle = ((dbsk2d_ishock_bline*)curbElm)->u();
      else
        curCCWAngle = ((dbsk2d_ishock_bline*)curbElm)->u() + vnl_math::pi;
    }
    else if (curbElm->is_an_arc()){
      if (bCurStartPt)
        curCCWAngle = ((dbsk2d_ishock_barc*)curbElm)->InTangentAtStartPt();
      else
        curCCWAngle = ((dbsk2d_ishock_barc*)curbElm)->InTangentAtEndPt();
    }
    curCCWAngle = angle0To2Pi(curCCWAngle);

    //EPSILONISSUE: HORIZONTAL LINE WILL HAVE 0, 2*pi PROBLEM!!
    if (AisEq02Pi(curCCWAngle,CCWAngle)){
      //this portion deals with the twin elements
      if (bStartPt){
        curB++;
        LinkedBElmList.insert(curB, elm);
      }
      else {
        LinkedBElmList.insert(curB, elm);
      }
      return;
    }    
    else if (AisG(curCCWAngle,CCWAngle)){
      //here you have to add it before the current element
      LinkedBElmList.insert(curB, elm);
      return;
    }
  }

  //if it gets here it means the element has to be added to the end of the list
  LinkedBElmList.push_back(elm);
}

void dbsk2d_ishock_bpoint::disconnectFrom(dbsk2d_ishock_belm* elm)
{
  belm_list::iterator curB = LinkedBElmList.begin();
   for(; curB!=LinkedBElmList.end(); ++curB) {
    if ((*curB) == elm){
      LinkedBElmList.erase(curB);
      return;
    }
  }
}

dbsk2d_ishock_belm* dbsk2d_ishock_bpoint::getElmToTheRightOf(dbsk2d_ishock_belm* elm)
{
  if (LinkedBElmList.size()==0)
    return NULL;

  if (LinkedBElmList.front() == elm)
    return LinkedBElmList.back();

  belm_list::iterator curB = LinkedBElmList.begin();
   for(; curB!=LinkedBElmList.end(); ++curB) {
    if ((*curB) == elm){
      curB--;
      return (*curB);
    }
  }

  return NULL;
}

dbsk2d_ishock_belm* dbsk2d_ishock_bpoint::getElmToTheLeftOf(dbsk2d_ishock_belm* elm)
{
  if (LinkedBElmList.size()==0) return NULL;

  if (LinkedBElmList.back() == elm)
    return LinkedBElmList.front();

  belm_list::iterator curB = LinkedBElmList.begin();
   for(; curB!=LinkedBElmList.end(); ++curB) {
    if ((*curB) == elm){
      curB++;
      return (*curB);
    }
  }

  return NULL;
}

void dbsk2d_ishock_bpoint::mergeWith (dbsk2d_ishock_bpoint* bpt)
{
  dbsk2d_ishock_belm* connectedElm;
  //We need to take the connectivity data from bpt and put it
  //into the linked element list of the current element
  //at the same time we need to update the other elements of
  //their change in connectivity
  belm_list::iterator curB = bpt->LinkedBElmList.begin();
   for(; curB!=bpt->LinkedBElmList.end(); ++curB) {
    connectedElm = (*curB);
    connectedElm->reconnect(bpt, this);
    connectTo(connectedElm);
  }

  //choose the one with higher confidence
  if (this->has_a_tangent() && bpt->has_a_tangent()){
    if (bpt->conf() > this->conf()){
      this->set_tangent(bpt->tangent());
    }
  }
}

void dbsk2d_ishock_bpoint::getInfo (vcl_ostream& ostrm)
{
  char s[1024];

  ostrm << "\n==============================\n";
  ostrm << "BP: [" << _id << "]\n";
  vcl_sprintf(s, "Position : (%.3f, %.3f)\n", _pt.x(), _pt.y()); ostrm << s;

  vcl_sprintf (s, "Tangent: %.5f (= %.5f) \n", _dir, _dir*180/vnl_math::pi); ostrm<<s;
  vcl_sprintf (s, "Confidence: %.5f \n", _conf); ostrm<<s;
  vcl_sprintf (s, "Ref Vector: %.5f \n", _vref); ostrm<<s;
  vcl_sprintf (s, "max Eta: %.5f \n", _max_eta); ostrm<<s;
  ostrm << "Visible: ";
  if (_is_visible) ostrm << "Yes\n";
  else ostrm <<  "No\n";
  
  if (is_a_free_point())
    ostrm << "Singular Point" << vcl_endl;
  else {
    ostrm << "Linked Elms: [" << nLinkedElms() << "] (";
    belm_list::iterator curB = LinkedBElmList.begin();
    for(; curB!=LinkedBElmList.end(); ++curB) {
      ostrm << (*curB)->id() << " ";
    }
    ostrm << ")\n";
  }

  //bnd_ishock_map
  bnd_ishock_map_iter curS = shock_map_.begin();
  ostrm << "\nShockMap: [" << id() << "]\n";
  for (; curS!=shock_map_.end(); ++curS){
    vcl_sprintf (s, "%.5f -> %d (%s)\n", 
      curS->first.s_eta, curS->second->id(), 
      (curS->first).type_string().c_str()); 
    ostrm<<s;
  }

  /* sprintf (s, "Neighboring BElements::\n"); ostrm<<s;
  belm_list neighboringBElms = getAllNeighboringBElements();
  curB = neighboringBElms.begin();
   for (; curB!=neighboringBElms.end(); ++curB) {
      sprintf (s, "%d, ", (*curB)->id()); ostrm<<s;
  }
  sprintf (s, "\n\n"); ostrm<<s;*/
}

void dbsk2d_ishock_bpoint::compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ex_pts_.clear();
  ex_pts_.push_back(_pt);
}

