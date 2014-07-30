// This is brcv/shp/dbsk2d/dbsk2d_ishock_edge.cxx

//:
// \file

#include <dbsk2d/dbsk2d_ishock_edge.h>
#include <dbsk2d/dbsk2d_ishock_node.h>

dbsk2d_ishock_edge::dbsk2d_ishock_edge (int newid, double stime,
   dbsk2d_ishock_node* pse, dbsk2d_ishock_belm* lbe, dbsk2d_ishock_belm* rbe) :
   dbsk2d_ishock_elm (newid, stime)
{
  _edgeID = newid;
  _lBElement = lbe;
  _rBElement = rbe;

  _lNeighbor = NULL; 
  _rNeighbor = NULL;
  _lShock = NULL;
  _rShock = NULL;
  _lShock_id = -1;
  _rShock_id = -1;
  _pSNode = pse;
  _cSNode = NULL;

  _H = 0;

  _LsTau = -1;
  _LeTau = -1;
  _RsTau = -1;
  _ReTau = -1;

  _bnd_intersect_pos = ISHOCK_DIST_HUGE;
  _cell_bnd = 0;
}

void dbsk2d_ishock_edge::setSimTime (double stime) 
{
  _simTime = stime;
  _endTime = stime;
}

//: reset all the parameters of this shock (called during reactivation of a shock)
void dbsk2d_ishock_edge::reset_shock()
{
  _bActive = true;
  _bPropagated = false;
  _lNeighbor = 0;
  _rNeighbor = 0;
  _simTime = _startTime;
  _endTime = ISHOCK_DIST_HUGE;
  _cSNode = NULL;

  //reset the end taus too
  set_end_taus_at_init();
}

//these tests can be fuzzy around min and max taus but still have to
//satisfy the strict limits of these parameters
bool dbsk2d_ishock_edge::isLTauValid_MinMax (double ltau)
{
  return AisGEq(ltau, _minLTau) && AisLEq(ltau,_maxLTau);
  //return ltau>=minLTau && ltau<=_maxLTau;
  //return _isGEq(ltau, _minLTau, DOUBLE_PRECISION) && _isLEq(ltau, _maxLTau, DOUBLE_PRECISION);
}

bool dbsk2d_ishock_edge::isRTauValid_MinMax (double rtau)
{
  return AisGEq(rtau, _minRTau) && AisLEq(rtau,_maxRTau);
  //return rtau>=_minRTau && rtau<=_maxRTau;
  //return _isGEq(rtau,_minRTau, DOUBLE_PRECISION) && _isLEq(rtau,_maxRTau,DOUBLE_PRECISION);
}

bool dbsk2d_ishock_edge::isTauValid_MinMax (double ltau, double rtau)
{
  return isLTauValid_MinMax(ltau) &&
         isRTauValid_MinMax(rtau);
}

//: get the start eta corresponding to the dir
double dbsk2d_ishock_edge::sEta(dbsk2d_ishock_bnd_key::shock_type Stype)
{
  if (Stype==dbsk2d_ishock_bnd_key::LEFT ||
      Stype==dbsk2d_ishock_bnd_key::LEFTCONTACT)
    return LsEta();
  else
    return RsEta();
}

//: get the end eta corresponding to the dir
double dbsk2d_ishock_edge::eEta(dbsk2d_ishock_bnd_key::shock_type Stype)
{
  if (Stype==dbsk2d_ishock_bnd_key::LEFT ||
      Stype==dbsk2d_ishock_bnd_key::LEFTCONTACT)
    return LeEta();
  else
    return ReEta();
}

//: convert tau to eta
double dbsk2d_ishock_edge::TauToEta(double tau, DIRECTION dir, bool start)
{
  if (dir==LEFT)
    return LTauToLEta(tau, start);
  else
    return RTauToREta(tau, start);
}

//: return the child edge if the child node is a pruned junction
//A Pruned Junction is a junction that contains only a parent shock and a child shock.
//It's a 'Link Junction' that connects two shocks after pruning.
dbsk2d_ishock_edge* dbsk2d_ishock_edge::get_next_edge ()
{
  //1)Some edges might go to infinity
  if (!cSNode()) 
    return NULL;

  //2)Child is a Sink
  if (!cSNode()->is_a_junct()) 
    return NULL;

  //3)Return child edge only if it is a PrunedJunction
  if (cSNode()->indeg(true)==1 && !(cSNode()->cShock())->isHidden())  // ozge: added the second condition, after pruning hidden children are not valid to follow
    return cSNode()->cShock();

  return NULL;
}

//: return the parent edge if the parent node is a pruned junction
dbsk2d_ishock_edge* dbsk2d_ishock_edge::get_previous_edge ()
{
  //only junctions have valid parents
  if (!pSNode()->is_a_junct())
    return NULL;

  // Return parent link only if it is a PrunedJunction
  if (pSNode()->indeg(true)==1)
    return pSNode()->get_parent_edge();

  return NULL;
}
