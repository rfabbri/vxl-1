#include <extrautils/msgout.h>
#include "ishock.h"

void IShock::checkAndIntersectColinearContacts (SISource* newSource, BElement* belm1, BElement* belm2)
{
  SIContact *cc1, *cc2;

  if (belm2->type()==BPOINT) { //P-P

    //First test belm1...
    if (!((BPoint*)belm1)->isFormingColinearContact(newSource->origin(), &cc1, &cc2)) {
      //Test belm2...
      if (!((BPoint*)belm2)->isFormingColinearContact(newSource->origin(), &cc1, &cc2))
        return;

      //Case I. belm2 has ColinearContact
      SIPointPoint *ppChild1; //Left belm1, right belm2
      SIPointPoint *ppChild2; //Left belm2, right belm1
      if (newSource->cSLink()->rBElement()==belm1) {
        ppChild1 = (SIPointPoint*) newSource->cSLink();
        ppChild2 = (SIPointPoint*) newSource->cSLink2();
      }
      else {
        ppChild2 = (SIPointPoint*) newSource->cSLink();
        ppChild1 = (SIPointPoint*) newSource->cSLink2();
      }
    
      //intersect cc1 with ppChild1
      intersectSIContactWithPP (cc1, ppChild1);    
      //intersect ppChild2 with cc2
      intersectPPWithSIContact (ppChild2, cc2);

      return;
    }
    else {//Case II. belm1 has ColinearContact, cc1 and cc2
      SIPointPoint *ppChild1; //Left belm1, right belm2
      SIPointPoint *ppChild2; //Left belm2, right belm1
      if (newSource->cSLink()->lBElement()==belm1) {
        ppChild1 = (SIPointPoint*) newSource->cSLink();
        ppChild2 = (SIPointPoint*) newSource->cSLink2();
      }
      else {
        ppChild2 = (SIPointPoint*) newSource->cSLink();
        ppChild1 = (SIPointPoint*) newSource->cSLink2();
      }
    
      //intersect cc1 with ppChild1
      intersectSIContactWithPP (cc1, ppChild1);    
      //intersect ppChild2 with cc2
      intersectPPWithSIContact (ppChild2, cc2);

      return;
    }

    //Case III. both belm1 and belm2 has ColinearContact
    SIContact *cc3, *cc4;
    if (((BPoint*)belm2)->isFormingColinearContact(newSource->origin(), &cc3, &cc4)) { //cases II.
      SIPointPoint *ppChild1; //Left belm1, right belm2
      SIPointPoint *ppChild2; //Left belm2, right belm1
      if (newSource->cSLink()->lBElement()==belm1) {
        ppChild1 = (SIPointPoint*) newSource->cSLink();
        ppChild2 = (SIPointPoint*) newSource->cSLink2();
      }
      else {
        ppChild2 = (SIPointPoint*) newSource->cSLink();
        ppChild1 = (SIPointPoint*) newSource->cSLink2();
      }

      IntrinsicIntersection intersection;
      //1)intersect cc1 with ppChild1
      intersectSIContactWithPP (cc1, ppChild1);
      //2)intersect ppChild1 with cc4
      intersectPPWithSIContact (ppChild1, cc4);
      //3)intersect cc3 with ppChild2
      intersectSIContactWithPP (cc3, ppChild2);
      //4)intersect ppChild2 with cc2
      intersectPPWithSIContact (ppChild2, cc2);
    
    }//endif belm2->FormingColinearContact
  }
  else if (belm2->type()==BLINE) { //P-L
    if (!((BPoint*)belm1)->isFormingColinearContact(newSource->origin(), &cc1, &cc2))
      return;

    SIPointLine *plChild1, *plChild2;
    if (((SIPointLine*)newSource->cSLink())->nu()==1) {
      plChild1 = (SIPointLine*) newSource->cSLink();
      plChild2 = (SIPointLine*) newSource->cSLink2();
    }
    else {
      plChild1 = (SIPointLine*) newSource->cSLink2();
      plChild2 = (SIPointLine*) newSource->cSLink();
    }

    //intersect cc1 with plChild1
    intersectSIContactWithPL (cc1, plChild1);
    //intersect plChild2 with cc2
    intersectPLWithSIContact (plChild2, cc2);
  }
  else if (belm2->type()==BARC) { //P-A
    if (!((BPoint*)belm1)->isFormingColinearContact(newSource->origin(), &cc1, &cc2))
      return;

    SIPointArc *paChild1, *paChild2;
    if (((SIPointLine*)newSource->cSLink())->nu()==1) {
      paChild1 = (SIPointArc*) newSource->cSLink();
      paChild2 = (SIPointArc*) newSource->cSLink2();
    }
    else {
      paChild1 = (SIPointArc*) newSource->cSLink2();
      paChild2 = (SIPointArc*) newSource->cSLink();
    }

    //intersect cc1 with paChild1
    intersectSIContactWithPA (cc1, paChild1);
    //intersect paChild2 with cc2
    intersectPAWithSIContact (paChild2, cc2);
  }

  return;
}

void IShock::intersectSIContactWithPP (SIContact* cc, SIPointPoint* spp)
{
  IntrinsicIntersection intersection;
  intersection.R      = spp->startTime();
  intersection.LSLtau = cc->LsTau();
  intersection.LSRtau = cc->RsTau();
  intersection.RSLtau = 0;
  intersection.RSRtau = spp->RTau(intersection.RSLtau);
  intersection.newLtau = cc->LsTau();
  intersection.newRtau = angle0To2Pi (spp->u()+M_PI + intersection.RSRtau);
  spp->set_lNeighbor (cc);
  moveASILinkToAJunction (intersection, spp, RIGHT);
  cc->set_rNeighbor (spp);
  moveASILinkToAJunction (intersection, cc, LEFT);
}

void IShock::intersectPPWithSIContact (SIPointPoint* spp, SIContact* cc)
{
  IntrinsicIntersection intersection;
  intersection.R      = spp->startTime();
  intersection.LSRtau = 2*M_PI;
  intersection.LSLtau = spp->LTau(intersection.LSRtau);
  intersection.RSLtau = cc->LsTau();
  intersection.RSRtau = cc->RsTau();
  intersection.newLtau = angle0To2Pi (spp->u() + intersection.LSLtau);
  intersection.newRtau = cc->RsTau();
  spp->set_rNeighbor (cc);
  moveASILinkToAJunction (intersection, spp, LEFT);
  cc->set_lNeighbor (spp);
  moveASILinkToAJunction (intersection, cc, RIGHT);
}

void IShock::intersectSIContactWithPL (SIContact* cc, SIPointLine* spl)
{
  IntrinsicIntersection intersection;
  intersection.R      = spl->startTime();
  intersection.LSLtau = cc->LsTau();
  intersection.LSRtau = cc->RsTau();
  intersection.RSLtau = 0;
  intersection.RSRtau = spl->RTau(intersection.RSLtau);
  intersection.newLtau = cc->LsTau();
  intersection.newRtau = spl->delta() + intersection.RSRtau;
  spl->set_lNeighbor (cc);
  moveASILinkToAJunction (intersection, spl, RIGHT);
  cc->set_rNeighbor (spl);
  moveASILinkToAJunction (intersection, cc, LEFT);
}

void IShock::intersectPLWithSIContact (SIPointLine* spl, SIContact* cc)
{
  IntrinsicIntersection intersection;
  intersection.R      = spl->startTime();
  intersection.LSRtau = 2*M_PI;
  intersection.LSLtau = spl->LTau(intersection.LSRtau);
  intersection.RSLtau = cc->LsTau();
  intersection.RSRtau = cc->RsTau();
  intersection.newLtau = spl->delta() - intersection.LSLtau;
  intersection.newRtau = cc->RsTau();
  spl->set_rNeighbor (cc);
  moveASILinkToAJunction (intersection, spl, LEFT);
  cc->set_lNeighbor (spl);
  moveASILinkToAJunction (intersection, cc, RIGHT);
}

void IShock::intersectSIContactWithPA (SIContact* cc, SIPointArc* spa)
{
  IntrinsicIntersection intersection;
  intersection.R      = spa->startTime();
  intersection.LSLtau = cc->LsTau();
  intersection.LSRtau = cc->RsTau();
  intersection.RSLtau = 0;
  intersection.RSRtau = spa->RTau(intersection.RSLtau);
  intersection.newLtau = cc->LsTau();
///  intersection.newRtau = spa->delta() + intersection.RSRtau;
  spa->set_lNeighbor (cc);
  moveASILinkToAJunction (intersection, spa, RIGHT);
  cc->set_rNeighbor (spa);
  moveASILinkToAJunction (intersection, cc, LEFT);
}

void IShock::intersectPAWithSIContact (SIPointArc* spa, SIContact* cc)
{
  IntrinsicIntersection intersection;
   intersection.R      = spa->startTime();
   intersection.LSRtau = 2*M_PI;
  intersection.LSLtau = spa->LTau(intersection.LSRtau);
   intersection.RSLtau = cc->LsTau();
   intersection.RSRtau = cc->RsTau();
///  intersection.newLtau = spa->delta() - intersection.LSLtau;
   intersection.newRtau = cc->RsTau();
  spa->set_rNeighbor (cc);
  moveASILinkToAJunction (intersection, spa, LEFT);
  cc->set_lNeighbor (spa);
  moveASILinkToAJunction (intersection, cc, RIGHT);
}
