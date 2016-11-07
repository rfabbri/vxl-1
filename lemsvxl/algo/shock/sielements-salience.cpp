#include "sielements.h"
#include "sym-transforms.h"

void SIA3Source::computeSalience (void) 
{
  _dPnCost = 0;
  RADIUS_TYPE R = _startTime;

  VECTOR_TYPE start_angle = ((SIContact*)PSElementList.front())->n();
  VECTOR_TYPE end_angle = ((SIContact*)PSElementList.back())->n();
  
  _dOC = R*CCW(start_angle, end_angle);
  _dNC = _dOC;
}

void SISource::computeSalience ()
{
  _dOC = 0;
  _dNC = 0;
  _dPnCost = 0;
}

void SISink::computeSalience (void)
{
  _dOC = 0;
  _dNC = ISHOCK_DIST_HUGE;
  _dPnCost = ISHOCK_DIST_HUGE; //not sure yet!
}

void SIJunct::computeSalience (void)
{
  VECTOR_TYPE start_angle=-1;
  VECTOR_TYPE end_angle = -1;
  RADIUS_TYPE R = _startTime;

  _dOC = 0;
  _dNC = 0;
  _dPnCost = 0;
  
  //Accumulate all the information from the parent links
  //we know we are going counter clockwise over the shocks
  //coming into the junction
  SILinksList::iterator curS = PSElementList.begin();
  for(; curS!=PSElementList.end(); ++curS){
    if ((*curS)->type()==SIElement::POINTARCCONTACT){
      if ((*curS)->rBElement()->type()==BARC && start_angle<0)
        start_angle = ((SIContact*)(*curS))->n();
    
      if ((*curS)->lBElement()->type()==BARC && !(start_angle<0)){
        end_angle = ((SIContact*)(*curS))->n();
        _dOC += R*CCW(start_angle, end_angle);
        _dNC += R*CCW(start_angle, end_angle);
        start_angle = -1;
      }
    }
    else {
      _dOC += (*curS)->dOC();
      _dNC += (*curS)->dNC();
    }
    _dPnCost += (*curS)->dPnCost();
  }
}

void SIPointPoint::computeSalience (void)
{
  //regular Point-Point
  DIST_TYPE R  = _endTime;
  ANGLE_TYPE thetaE=0;

  //formed by two points, therefore
  _dOC = 0;

  if (R > MAX_RADIUS) {
      _dNC = _H;
   }
  else {
    thetaE = M_PI - 2*_LeTau;
    _dNC = R*thetaE;
  }

  _dPnCost = vcl_fabs(_dOC + _pSNode->dNC() - _dNC) + _pSNode->dPnCost();

  //some fudging to remove zero length hassels
  //if (RisEq(_dPnCost,0)) _dPnCost = ISHOCK_DIST_HUGE;

  assert (_dOC>=0 && _dNC>=0 && thetaE>=0 );//&& _dPnCost
}

void SIPointLine::computeSalience ()
{
  //regular point-line
  RADIUS_TYPE R  = _endTime;
  ANGLE_TYPE thetaE;

  if (_nu==1) {
    _dOC = _ReTau - _RsTau;
    thetaE = CCW(_u+_LeTau+M_PI, _u);
  }
  else {
    _dOC = _LeTau - _LsTau;
    thetaE = CCW(_u, _u+_ReTau+M_PI);
  }

  _dNC = R*thetaE;

  _dPnCost = vcl_fabs(_dOC + _pSNode->dNC() - _dNC) + _pSNode->dPnCost();

  //some fudging to remove zero length hassels
  //if (RisEq(_dPnCost,0)) _dPnCost = ISHOCK_DIST_HUGE;

  assert (AisGEq(_dOC,0) && _dNC>=0 && thetaE>=0 && _dPnCost);
  //assert (_dOC>=0 && _dNC>=0 && thetaE>=0 && _dPnCost);
}

void SIPointArc::computeSalience ()
{
  RADIUS_TYPE R  = _endTime;
  ANGLE_TYPE thetaE=0;

  if (_nu==1)
    _dOC = Rr()*vcl_fabs(_ReTau - _RsTau);
  else 
    _dOC = Rl()*vcl_fabs(_LeTau - _LsTau);

  if (R > MAX_RADIUS) {
    Point start  = getLFootPt(_LeTau);
    Point  end  = getRFootPt(_ReTau);
      _dNC = _distPointPoint(start, end); 
   }
  else {
    if (_s==1)
      thetaE = CCW(_u+_LeTau+M_PI, _u+M_PI+_ReTau+M_PI);
    else
      thetaE = CCW(_u+_LeTau+M_PI, _u+M_PI+_ReTau);

    _dNC = R*thetaE;
  }

  _dPnCost = vcl_fabs(_dOC + _pSNode->dNC() - _dNC) + _pSNode->dPnCost();

  //some fudging to remove zero length hassels
  //if (RisEq(_dPnCost,0)) _dPnCost = ISHOCK_DIST_HUGE;

  assert (_dOC>=0 && _dNC>=0 && thetaE>=0 && _dPnCost);;
}

void SILineLine::computeSalience (void)
{
  RADIUS_TYPE R  = _endTime;

  //EPSILONISSUE 11, Correction!!
  if (_ReTau<_RsTau)
    _ReTau = _RsTau;
  _dOC = 2*(_ReTau - _RsTau);

  ANGLE_TYPE thetaE = CCW (_ul, _ur);
  _dNC = R*thetaE;

  _dPnCost = vcl_fabs(_dOC + _pSNode->dNC() - _dNC) + _pSNode->dPnCost();

  //some fudging to remove zero length hassels
  //if (RisEq(_dPnCost,0)) _dPnCost = ISHOCK_DIST_HUGE;

  assert (_dOC>=0 && _dNC>=0 && thetaE>=0 && _dPnCost);
}

void SILineArc::computeSalience ()
{
  RADIUS_TYPE R  = _endTime;
  ANGLE_TYPE thetaE=0;

  if (_nu==1)
    _dOC = _R*vcl_fabs(_LeTau - _LsTau) + (_ReTau - _RsTau);
  else 
    _dOC = _R*vcl_fabs(_ReTau - _RsTau) + (_LeTau - _LsTau);

  Point start, end;
  if (R > MAX_RADIUS) {
    if (_nu==1){ //arc on the left
      start  = getLFootPt(_LeTau);
      //end  = getRFootPt(_LeTau);
      if (_nud==1)
        end = rBLine()->end();
      else
        end = _foot;
    }
    else {
      //start  = getLFootPt(_ReTau);
      if (_nud==1)
        start = lBLine()->start();
      else
        start = _foot;
      end  = getRFootPt(_ReTau);
    }
      _dNC = _distPointPoint(start, end); 
   }
  else {
    if (_nu==1){
      if (_s==1)
        thetaE = CCW(_u+_LeTau+M_PI, _u);
      else
        thetaE = CCW(_u+_LeTau+M_PI, _u+M_PI);
    }
    else {
      if (_s==1)
        thetaE = CCW(_u, _u+_ReTau+M_PI);
      else
        thetaE = CCW(_u+M_PI, _u+_ReTau+M_PI);
    }
    _dNC = R*thetaE;
  }

  _dPnCost = vcl_fabs(_dOC + _pSNode->dNC() - _dNC) + _pSNode->dPnCost();

  //some fudging to remove zero length hassels
  //if (RisEq(_dPnCost,0)) _dPnCost = ISHOCK_DIST_HUGE;

  //assert (_dOC>=0 && _dNC>=0 && thetaE>=0 && _dPnCost);
}

void SIArcArc::computeSalience ()
{
  RADIUS_TYPE R  = _endTime;
  ANGLE_TYPE  thetaE=0;

  _dOC = Rr()*vcl_fabs(_ReTau - _RsTau) + Rl()*vcl_fabs(_LeTau - _LsTau);

  if (R > MAX_RADIUS) {
    Point start  = getLFootPt(_LeTau);
    Point  end  = getRFootPt(_ReTau);
      _dNC = _distPointPoint(start, end); 
   }
  else {
    if (_s==1)
      thetaE = CCW(_u+_LeTau+M_PI, _u+M_PI+_ReTau+M_PI);
    else
      thetaE = CCW(_u+_LeTau+M_PI, _u+M_PI+_ReTau);
    _dNC = R*thetaE;
  }

  _dPnCost = vcl_fabs(_dOC + _pSNode->dNC() - _dNC) + _pSNode->dPnCost();

  //some fudging to remove zero length hassels
  //if (RisEq(_dPnCost,0)) _dPnCost = ISHOCK_DIST_HUGE;

  assert (_dOC>=0 && _dNC>=0 && thetaE>=0 && _dPnCost);
}

