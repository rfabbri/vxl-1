//: Aug 19, 2005 MingChing Chang
//  

#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <gdt/gdt_manager.h>

// Description: 
//   In a shock-edge intersection, advance the shock S to its prjE.
//
// Return: void.
//
void gdt_ws_manager::Advance_shock_to_edge (gdt_shock* S)
{
  assert (S->simT() == S->edgeT());
  assert (S->endT() == S->edgeT());

  gdt_welm* pWa = S->Wa();
  gdt_welm* pWb = S->Wb();
  gdt_welm* nWa = NULL;
  gdt_welm* nWb = NULL;
  bool nWa_detect_NE = false;
  bool nWb_detect_NE = false;

  //1. Setup the local orientation and the next nWa and nWb.
  SOT_TYPE SOtype = S->_detect_SO_type();

  dbmsh3d_face* curF;
  dbmsh3d_halfedge *heC, *heL, *heR;
  dbmsh3d_gdt_edge *eC, *eL, *eR;  
  S->_get_local_orientation2 (SOtype, &heC, &curF, &heL, &heR, &eC, &eL, &eR);

  double alphaCL, alphaCR, alphaLR;
  m2t_compute_tri_angles (eC->len(), eL->len(), eR->len(), alphaCL, alphaCR, alphaLR);

  SE_setup_nWa_nWb (S, SOtype, heC, heL, heR, eC, eL, eR, alphaCL, alphaCR, alphaLR, 
                    &nWa, &nWb, nWa_detect_NE, nWb_detect_NE);

  //2. Try to remove pWa, Wb, nWa, nWb from Qw, if they are in Qw.
  try_remove_from_Qw (pWa);
  try_remove_from_Qw (pWb);
  if (nWa)
    try_remove_from_Qw (nWa);
  if (nWb)
    try_remove_from_Qw (nWb);
  
  //3. Update tEL, tER, tVO of pWa and pWb
  //   Propagate/Finalize pWa and pWb
  SE_update_pWa_pWb (S);

  //4. Update S->Wa and S->Wb to be nWa and nWb on prjE. Finish all other works by cases.
  //   Eight cases:
  //   1) nWb==NULL, degenerate S to V-: 12 cases:
  //      SRF2 to vO-, SRF-L to vA-, SRF-L2 to vA-, SRF-L2 to vB-, 
  //      SRF-R to vB-, SRF-R2 to vA-,
  //      SE to v-, SV to vb-, SV to va-, SVE to v-
  //   2) nWa==NULL, degenerate S to V+: 12 cases:
  //      SRF2 to vO+, SRF-L to vA+, SRF-L2 to vB+,
  //      SRF-R to vA+, SRF-R to vB+, SRF-R2 to vA+,
  //      SE to v+, SV to vA+, SV to vB+, SVE to v+
  //   3) Both Wa.RF==true and Wa.RF==true
  //        3.1) prjE==eL: SRF2 to eL
  //        3.2) prjE==eR: SRF2 to eR
  //   4) Wa.RF==true. Three cases.
  //        4.1) prjE==eL: SRF-L to eL
  //        4.2) prjE==eR: 
  //               4.2.1) SRF-L to eR
  //               4.2.2) SRF-L2 to eR
  //        4.3) prjE==eC: 
  //               4.3.1) SRF-L2 to eC
  //   5) Wb.RF==true. Three cases.
  //        5.1) prjE==eR: SRF-R to eR
  //        5.2) prjE==eL: 
  //               5.2.1) SRF-R to eL
  //               5.2.2) SRF-R2 to eL
  //        5.3) prjE==eC: 
  //               5.3.1) SRF-R2 to eC
  //   6) nWa==pWa: 
  //        6.1) SV to eL
  //        6.2) SVE to eL
  //   7) nWb==pWb:
  //        7.1) SV to eR
  //        7.2) SVE to eR
  //   8) regular S-E:
  //        8.1) SE to eL
  //        8.2) SE to eR
  //        8.3) SV to eO
  //
  if (nWb == NULL) { //1) degenerate S to V-: 10 cases:
    // SRF2 to vO-, SRF-L to vA-, SRF-L2 to vA-, 
    // SRF-L2 to vB-, SRF-R to vB-, SRF-R2 to vA-, 
    // SE to v-, SV to vb-, SV to va-, SVE to v-
    SE_s_to_vL (S, nWa, nWa_detect_NE);
    return;
  }
  else if (nWa == NULL) { //2) degenerate S to V+: 10 cases:
    // SRF2 to vO+, SRF-L to vA+, SRF-L2 to vB+, 
    // SRF-R to vA+, SRF-R to vB+, SRF-R2 to vA+, 
    // SE to v+, SV to vA+, SV to vB+, SVE to v+
    SE_s_to_vR (S, nWb, nWb_detect_NE);
    return;
  } 
  else if (pWa->_is_RF() && pWb->_is_RF()) { //3) Both Wa.RF==true and Wb.RF==true: SRF2
    if (S->prjE() == eL) { //SRF2 to eL: similar to SRF-L to eL
      SE_srf_to_degeWa (S, nWa, nWb, eC, true, nWb_detect_NE);
    }
    else { //SRF2 to eR: similar to SRF-R to eR
      assert (S->prjE() == eR);
      SE_srf_to_degeWb (S, nWa, nWb, eC, true, nWa_detect_NE);
    }
  }
  else if (pWa->_is_RF()) { //4) pWa.RF==true. Three cases.
    if (S->prjE() == eL) { // 4.1) SRF-L to eL
      SE_srf_to_degeWa (S, nWa, nWb, eC, true, nWb_detect_NE);
    }
    else if (S->prjE() == eR) { //SRF-L to eR or SRF-L2 to eR
      //Test if pWa RF center = v = eC.s
      if (pWa->psrc() == eC->sV()) // 4.2.1) SRF-L to eR
        SE_se_to_e (S, nWa, nWb, nWa_detect_NE, nWb_detect_NE);
      else // 4.2.2) SRF-L2 to eR: prjE is not eL
        SE_srf_to_degeWa (S, nWa, nWb, eC, false, nWb_detect_NE);
    }
    else { 
      assert (S->prjE() == eC);
      //4.3.1) SRF-L2 to eC: Ib is always outward from v, b_SVE always false.
      SE_sv_to_Ib (S, nWa, nWb, false, false, nWa_detect_NE);
    }
  }
  else if (pWb->_is_RF()) { //5)  pWb.RF==true. Three cases.
    if (S->prjE() == eR) { //5.1) SRF-R to eR
      SE_srf_to_degeWb (S, nWa, nWb, eC, true, nWa_detect_NE);
    }
    else if (S->prjE() == eL) { //SRF-R to eL or SRF-R2 to eL
      //Test if pWb RF center = v = eC.e
      if (pWb->psrc() == eC->eV()) // 5.2.1) SRF-R to eL
        SE_se_to_e (S, nWa, nWb, nWa_detect_NE, nWb_detect_NE);
      else //5.2.2) SRF-R2 to eL: prjE is not eR
        SE_srf_to_degeWb (S, nWa, nWb, eC, false, nWa_detect_NE);
    }
    else { 
      assert (S->prjE() == eC);
      // 5.3.1) SRF-R2 to eC, Ia is always toward v, b_SVE always false.
      SE_sv_to_Ia (S, nWa, nWb, true, false, nWb_detect_NE);
    }
  }
  else if (nWa == pWa) { //6) nWa==pWa: 6.1) SV to eL and 6.2) SVE to eL
    SE_sv_to_Ia (S, nWa, nWb, !S->_is_Ia_outward(), S->bSVE(), nWb_detect_NE);
  }
  else if (nWb == pWb) { //7) nWb==pWb: 7.1) SV to eR and 7.2) SVE to eR
    SE_sv_to_Ib (S, nWa, nWb, !S->_is_Ib_outward(), S->bSVE(), nWa_detect_NE);
  }
  else { //8) regular S-E: 
    // 8.1) SE to eL, 8.2) SE to eR, 8.3) SV to eO.
    SE_se_to_e (S, nWa, nWb, nWa_detect_NE, nWb_detect_NE);
  }
  
  //5. If (nWa, nWb) switch sides from (pWa, pWb), change sign of a.
  S->_compute_a ();

  //6. Reset the toSink flag.
  S->set_bSVE (false);
}

// ############################################################################

// Initialize the S-E intersection: 
//   Let pWa = S->Wa, pWb = S->Wb, determine the local orientation eC, eL, eR and the next nWa and nWb.
//   Case Analysis:
//   1. Both pWa and pWb are rarefaction: eC=edge of the two centers. eL=pWa.dege_edge, eR=pWb.dege_edge.
//      vA = center of pWa, vB = center of pWb, vO = the other vertex --- SRF2
//        1.1) S.prjE=eL: SRF2 to eL or vO-
//        1.2) S.prjE=eR: SRF2 to eR or vO+
//   2. pWa is rarefaction: eC=pWb.edge, eL and eR oriented by eC. Setup v, vA, vB accordingly.
//        2.1) pWa RF center = v = eC.s --- SRF-L
//               2.1.1) S.prjE=eR: SRF-L to eR or vA+ or vB-
//               2.1.2) S.prjE=eL: SRF-L to eL or vA-
//        Otherwise, pWa RF center = va = eL.other_vertex_of_eC.s 
//          2.2) S.toSink false --- SRF-L2
//                 2.2.1) S.prjE=eR: SRF-L2 to eR or vB-
//                 2.2.2) S.prjE=eC: SRF-L2 to eC or vB+
//
//   3. pWb is rarefaction: eC=pWa.edge, eL and eR oriented by eC. Setup v, vA, vB accordingly.
//        3.1) pWb RF center = v = eC.e --- SRF-R
//               3.1.1) S.prjE=eL: SRF-R to eL or vB- or vA+
//               3.1.2) S.prjE=eR: SRF-R to eR or vB+
//        Otherwise, pWb RF center = vb = eR.other_vertex_of_eC.e
//          3.2) S.toSink false --- SRF-R2
//                 3.2.1) S.prjE=eL: SRF-R2 to eL or vA+
//                 3.2.2) S.prjE=eC: SRF-R2 to eC or vA-
//
//   4. pWa.edge = pWb.edge, S from E: eC=pWa.edge=pWb.edge, eL and eR oriented by eC --- SE
//        4.1) S.prjE=eL: SE to eL or vO-
//               4.1.1) SE to vO-
//               4.1.2) SE to eL
//        4.2) S.prjE=eR: SE to eR or vO+
//               4.2.1) SE to vO+
//               4.2.2) SE to eR
//   5. pWa.edge != pWb.edge, S from V: eL=pWa.edge, eR=pWb.edge, eC=the opposite edge. Setup v, vA, vB accordingly. 
//        5.1) S.toSink false --- SV
//               5.1.1) S.prjE=eO: SV to eO or vA+ or vB-
//               5.1.2) S.prjE=eL: SV to eL or vA-
//               5.1.3) S.prjE=eR: SV to eR or vB+
//        5.2) S.toSink true --- SVE
//               5.2.1) S.prjE=eL: SVE to eL or v- (similar to 5.1.2)
//               5.2.2) S.prjE=eR: SVE to eR or v+ (similar to 5.1.3)
//
void gdt_ws_manager::SE_setup_nWa_nWb (const gdt_shock* S, const SOT_TYPE SOtype,
                       const dbmsh3d_halfedge* heC, const dbmsh3d_halfedge* heL, const dbmsh3d_halfedge* heR,
                       const dbmsh3d_gdt_edge* eC, const dbmsh3d_gdt_edge* eL, const dbmsh3d_gdt_edge* eR,
                       const double& alphaCL, const double& alphaCR, const double& alphaLR, 
                       gdt_welm** newWa, gdt_welm** newWb,
                       bool& nWa_detect_NE, bool& nWb_detect_NE)
{ 
  gdt_welm* pWa = S->Wa();
  gdt_welm* pWb = S->Wb();
  gdt_welm* nWa = NULL;
  gdt_welm* nWb = NULL;

  //Need to consider the extreme case of S-V, i.e., S->tauE is 0 or len(prjE).
  bool b_S_Vs = S->tauE() == 0;
  bool b_S_Ve = S->tauE() == S->prjE()->len();
  bool b_S_V = b_S_Vs || b_S_Ve;

  switch (SOtype) {
  case SOT_SRFRF:
  {
    // 1. Both pWa and pWb are rarefaction: eC=edge of the two centers. 
    //    eL=pWa.dege_edge=pWb.edge, eR=pWb.dege_edge=pWa.edge.
    //    vA = center of pWa, vB = center of pWb, vO = the other vertex --- SRF2
    //      1.1) S.prjE=eL: SRF2 to eL or vO-
    //      1.2) S.prjE=eR: SRF2 to eR or vO+
    dbmsh3d_gdt_vertex_3d* vA = pWa->psrc();
    dbmsh3d_gdt_vertex_3d* vB = pWb->psrc();

    if (S->prjE() == eL) {
      // 1.1) S.prjE=eL: SRF2 to eL or vO-
      //      - SRF2 to eL: if nWb exists.
      //      - SRF2 to vO-: if S-V or nWb=NULL.

      //pWa can not have any child. nWa is the degeW from vA on eL. nWa can not be NULL.
      assert (pWa->num_nextI() == 0);
      nWa = (gdt_welm*) vA->_find_childI_on_edge (eL);
      assert (nWa);

      if (b_S_V) { //Handle S-V:
        nWb = NULL;
      }
      else {
        //pWb strikes prjE to be nWb.
        nWb = S->_get_nWb_on_prjE();
        if (nWb == NULL) {
          nWb = create_W_from_RF (pWb);
          nWb_detect_NE = true;
        }
      }
    }
    else {
      assert (S->prjE() == eR);
      // 1.2) S.prjE=eR: SRF2 to eR or vO+
      //      - SRF2 to eR: if nWa exists.
      //      - SRF2 to vO+: if S-V or nWa=NULL.

      if (b_S_V) {
        nWa = NULL;
      }
      else {
        //pWa strikes prjE to be nWa.
        nWa = S->_get_nWa_on_prjE();
        if (nWa == NULL) {
          nWa = create_W_from_RF (pWa);
          nWa_detect_NE = true;
        }
      }

      //pWb can not have any child. nWb is the degeW from vB on eR. nWb can not be NULL.
      assert (pWb->num_nextI() == 0);
      nWb = (gdt_welm*) vB->_find_childI_on_edge (eR);
      assert (nWb);
    }
  }
  break;
  case SOT_SRF_L:
  {
    // 2. pWa is rarefaction: eC=pWb.edge, eL and eR oriented by eC. Setup v, vA, vB accordingly.
    //      2.1) pWa RF center = v = eC.s --- SRF-L
    //             2.1.1) S.prjE=eR: SRF-L to eR or vA+ or vB-
    //             2.1.2) S.prjE=eL: SRF-L to eL or vA-
    //      2.2) pWa RF center = va = eL.other_vertex_of_eC.s --- SRF-L2
    //             2.2.1) S.prjE=eR: SRF-L2 to eR or vB-
    //             2.2.2) S.prjE=eC: SRF-L2 to eC or vB+

    if (pWa->psrc() == eC->sV()) {
      // 2.1) pWa RF center = v = eC.s --- SRF-L
      //        2.1.1) S.prjE=eR: SRF-L to eR or vA+ or vB-
      //        2.1.2) S.prjE=eL: SRF-L to eL or vA-

      if (S->prjE() == eR) { 
        // 2.1.1) S.prjE=eR: SRF-L to eR or vA+ or vB-
        //        - SRF-L to eR: if nWa and nWb both exist.
        //        - SRF-L to vA+: if S-V or nWa=NULL.
        //        - SRF-L to vB-: if S-V or nWb=NULL.

        if ( (b_S_Vs && eC->eV() == eR->eV()) ||
             (b_S_Ve && eC->eV() == eR->sV()) ) { //Handle S-Va
          nWa = NULL;
        }
        else {
          //pWa strikes prjE to be nWa.
          nWa = S->_get_nWa_on_prjE();
          if (nWa == NULL) {
            nWa = create_W_from_RF (pWa);
            nWa_detect_NE = true;
          }
        }

        if ( (b_S_Ve && eC->eV() == eR->eV()) ||
             (b_S_Vs && eC->eV() == eR->sV()) ) { //Handle S-Vb
          nWb = NULL;
        }
        else {
          //pWb strikes prjE to be nWb
          nWb = S->_get_nWb_on_prjE();
          if (nWb == NULL) {
            nWb = SE_se_advance_Wb_to_prjE (S, heL, alphaCL, heR, alphaCR);
            nWb_detect_NE = true;
          }
        }
      }
      else { 
        assert (S->prjE() == eL);
        // 2.1.2) S.prjE=eL: SRF-L to eL or vA-
        //        - SRF-L to eL: if nWb exists.
        //        - SRF-L to vA-: if S-V or nWb=NULL.
        
        //pWa can not have any child. nWa is the degeW from (*eC)->sV on eL. nWa can not be NULL.
        assert (pWa->num_nextI() == 0);
        nWa = (gdt_welm*) eC->sV()->_find_childI_on_edge (eL);
        assert (nWa);

        if (b_S_V) { //Handle S-V
          nWb = NULL;
        }
        else { //pWb strikes prjE to be nWb
          nWb = S->_get_nWb_on_prjE();
          if (nWb == NULL) {
            nWb = SE_se_advance_Wb_to_prjE (S, heL, alphaCL, heR, alphaCR);
            nWb_detect_NE = true;
          }
        }
      }
    }
    else { //pWa RF center = va = eL.other_vertex_of_eC.s
      // 2.2) S.toSink false --- SRF-L2
      //        2.2.1) S.prjE=eR: SRF-L2 to eR or vB-
      //        2.2.2) S.prjE=eC: SRF-L2 to eC or vB+
      //
      if (S->prjE() == eR) {
        // 2.2.1) S.prjE=eR: SRF-L2 to eR or vB-
        //        - SRF-L2 to eR: if nWb exists.
        //        - SRF-L2 to vB-: if S-V or nWb=NULL.

        //nWa is the degeW from va on eR. nWa can not be NULL.
        dbmsh3d_gdt_vertex_3d* va = (dbmsh3d_gdt_vertex_3d*) eL->other_V (eC->sV());
        assert (pWa->num_nextI() == 0);
        nWa = (gdt_welm*) va->_find_childI_on_edge (eR);
        assert (nWa);

        if (b_S_V) { //Handle S-V
          nWb = NULL;
        }
        else { //pWb strikes prjE to be nWb
          nWb = S->_get_nWb_on_prjE();
          if (nWb == NULL) {
            nWb = _SE_advance_W_to_eR (pWb, heR, alphaCR, S->edgeT());
            nWb_detect_NE = true;
          }
        }
      }
      else {
        assert (S->prjE() == eC);
        // 2.2.2) S.prjE=eC: SRF-L2 to eC or vB+
        //        - SRF-L2 to eC: if nWa exists.
        //        - SRF-L2 to vB+: if S-V or nWa=NULL.

        if (b_S_V) {
          nWa = NULL;
        }
        else { //pWa strikes prjE to be nWa.
          nWa = S->_get_nWa_on_prjE();
          if (nWa == NULL) {
            nWa = create_W_from_RF (pWa);
            nWa_detect_NE = true;
          }
        }

        //pWb can not have any child.
        assert (pWb->num_nextI() == 0);
        nWb = pWb;
        assert (nWb);
      }
    }
  }
  break;
  case SOT_SRF_R:
  {
    // 3. pWb is rarefaction: eC=pWa.edge, eL and eR oriented by eC. Setup v, vA, vB accordingly.
    //      3.1) pWb RF center = v = eC.e --- SRF-R
    //             3.1.1) S.prjE=eL: SRF-R to eL or vB- or vA+
    //             3.1.2) S.prjE=eR: SRF-R to eR or vB+
    //      3.2) pWb RF center = vb = eR.other_vertex_of_eC.e --- SRF-R2
    //             3.2.1) S.prjE=eL: SRF-R2 to eL or vA+
    //             3.2.2) S.prjE=eC: SRF-R2 to eC or vA-

    if (pWb->psrc() == eC->eV()) {
      // 3.1) pWb RF center = v = eC.e --- SRF-R
      //        3.1.1) S.prjE=eL: SRF-R to eL or vB- or vA+
      //        3.1.2) S.prjE=eR: SRF-R to eR or vB+

      if (S->prjE() == eL) {
        // 3.1.1) S.prjE=eL: SRF-R to eL or vB- or vA+
        //        - SRF-R to eL: if nWa and nWb both exist.
        //        - SRF-R to vB-: if S-V or nWb=NULL.
        //        - SRF-R to vA+: if S-V or nWa=NULL.

        if ( (b_S_Vs && eC->sV() == eL->sV()) ||
             (b_S_Ve && eC->sV() == eL->eV()) ) { //Handle S-Va+
          nWa = NULL;
        }
        else { //pWa strikes prjE to be nWa
          nWa = S->_get_nWa_on_prjE();
          if (nWa == NULL) {
            nWa = SE_se_advance_Wa_to_prjE (S, heL, alphaCL, heR, alphaCR);
            nWa_detect_NE = true;
          }
        }

        if ( (b_S_Ve && eC->sV() == eL->sV()) ||
             (b_S_Vs && eC->sV() == eL->eV()) ) { //Handle S-Vb-
          nWb = NULL;
        }
        else { //pWb strikes prjE to be nWb.
          nWb = S->_get_nWb_on_prjE();
          if (nWb == NULL) {
            nWb = create_W_from_RF (pWb);
            nWb_detect_NE = true;
          }
        }
      }
      else {
        assert (S->prjE() == eR);
        // 3.1.2) S.prjE=eR: SRF-R to eR or vB+
        //        - SRF-R to eR: if nWa exists.
        //        - SRF-R to vB+: if S-V or nWa=NULL.

        if (b_S_V) { //Handle S-V
          nWa = NULL;
        }
        else { //pWa strikes prjE to be nWa
          nWa = S->_get_nWa_on_prjE();
          if (nWa == NULL) {
            nWa = SE_se_advance_Wa_to_prjE (S, heL, alphaCL, heR, alphaCR);
            nWa_detect_NE = true;
          }
        }

        //pWb can not have any child. nWb is the degeW from eC->eV on eR. nWa can not be NULL.
        assert (pWb->num_nextI() == 0);
        nWb = (gdt_welm*) eC->eV()->_find_childI_on_edge (eR);
        assert (nWb);
      }
    }
    else { //pWb RF center = vb = eR.other_vertex_of_eC.e
      // 3.2) S.toSink false ---SRF-R2
      //        3.2.1) S.prjE=eL: SRF-R2 to eL or vA+
      //        3.2.2) S.prjE=eC: SRF-R2 to eC or vA-
      //
      if (S->prjE() == eL) {
        // 3.2.1) S.prjE=eL: SRF-R2 to eL or vA+
        //        - SRF-R2 to eL: if nWa exists.
        //        - SRF-R2 to vA+: if S-V or nWa=NULL.

        if (b_S_V) { //Handle S-V
          nWa = NULL;
        }
        else { //pWa strikes prjE to be nWa.
          nWa = S->_get_nWa_on_prjE();
          if (nWa == NULL) {
            nWa = _SE_advance_W_to_eL (pWa, heL, alphaCL, S->edgeT());
            nWa_detect_NE = true;
            pWa->set_tEL (GDT_HUGE);
          }
        }

        //nWb is the degeW from vb on eL. nWb can not be NULL.
        dbmsh3d_gdt_vertex_3d* vb = (dbmsh3d_gdt_vertex_3d*) eR->other_V (eC->eV());
        assert (pWb->num_nextI() == 0);
        nWb = (gdt_welm*) vb->_find_childI_on_edge (eL);
        assert (nWb);
      }
      else {
        assert (S->prjE() == eC);
        // 3.2.2) S.prjE=eR: SRF-R2 to eC or vA-
        //        - SRF-R2 to eC: if nWb exists.
        //        - SRF-R2 to vA-: if S-V or nWb=NULL.

        //pWa can not have any child. nWa = pWa.
        assert (pWa->num_nextI() == 0);
        nWa = pWa;
        assert (nWa);

        if (b_S_V) {
          nWb = NULL;
        }
        else { //pWb strikes prjE to be nWb.
          nWb = S->_get_nWb_on_prjE();
          if (nWb == NULL) {
            nWb = create_W_from_RF (pWb);
            nWb_detect_NE = true;
          }
        }
      }
    }
  }
  break;
  case SOT_SE:
    // 4. pWa.edge = pWb.edge, S from E: eC=pWa.edge=pWb.edge, eL and eR oriented by eC --- SE
    //      4.1) S.prjE=eL: SE to eL or vO-
    //      4.2) S.prjE=eR: SE to eR or vO+

    if (S->prjE() == eL) {
      // 4.1) S.prjE=eL: SE to eL or vO-
      //      - SE to eL: if tauE not close to boundary
      //      - SE to vO-: if tauE close to boundary

      if (( eC->sV() == eL->sV() && _eqT (S->tauE(), S->prjE()->len()) ) ||
          ( eC->sV() == eL->eV() && _eqT (S->tauE(), 0) )) { 
        // 4.1.1) SE to vO-

        //If Wa has child on eR, delete it!
        _del_childW_on_edge (pWa, eR);

        //pWa strikes prjE to be nWa
        nWa = S->_get_nWa_on_prjE();
        if (nWa == NULL) {
          nWa = SE_se_advance_Wa_to_prjE (S, heL, alphaCL, heR, alphaCR);
          nWa_detect_NE = true;
        }
     
        //If Wb has child on eL, delete it!
        _del_childW_on_edge (pWb, eL);
        nWb = NULL;
      }
      else { //4-1.2) SE to eL        
        //pWa strikes prjE to be nWa
        nWa = S->_get_nWa_on_prjE();
        if (nWa == NULL) {
          nWa = SE_se_advance_Wa_to_prjE (S, heL, alphaCL, heR, alphaCR);
          nWa_detect_NE = true;
        }
        
        //pWb strikes prjE to be nWb
        nWb = S->_get_nWb_on_prjE();
        if (nWb == NULL) {
          nWb = SE_se_advance_Wb_to_prjE (S, heL, alphaCL, heR, alphaCR);
          nWb_detect_NE = true;
        }
      }
    }
    else {
      assert (S->prjE() == eR);
      if (( eC->eV() == eR->eV() && _eqT (S->tauE(), 0) ) ||
          ( eC->eV() == eR->sV() && _eqT (S->tauE(), S->prjE()->len()) )) {
        //4.2.1) SE to vO+

        //If Wa has child on eR, delete it!
        _del_childW_on_edge (pWa, eR);
        nWa = NULL;

        //If Wb has child on eL, delete it!
        _del_childW_on_edge (pWb, eL);

        //pWb strikes prjE to be nWb
        nWb = S->_get_nWb_on_prjE();
        if (nWb == NULL) {
          nWb = SE_se_advance_Wb_to_prjE (S, heL, alphaCL, heR, alphaCR);
          nWb_detect_NE = true;
        }
      }
      else { //4.2.2) SE to eR        
        //pWa strikes prjE to be nWa
        nWa = S->_get_nWa_on_prjE();
        if (nWa == NULL) {
          nWa = SE_se_advance_Wa_to_prjE (S, heL, alphaCL, heR, alphaCR);
          nWa_detect_NE = true;
        }
        
        //pWb strikes prjE to be nWb
        nWb = S->_get_nWb_on_prjE();
        if (nWb == NULL) {
          nWb = SE_se_advance_Wb_to_prjE (S, heL, alphaCL, heR, alphaCR);
          nWb_detect_NE = true;
        }
      }
    }
  break;
  case SOT_SV_SVE:
  {
    // 5. pWa.edge != pWb.edge, S from V: eL=pWa.edge, eR=pWb.edge, eC=the opposite edge. 
    //    Setup v, vA, vB accordingly. ---- SV
    //      5.1) S.prjE=eO: SV to eO or vA+ or vB-
    //      5.2) S.prjE=eL: SV to eL or vA-
    //      5.3) S.prjE=eR: SV to eR or vB+
    bool b_eC_from_vA = eL->is_V_incident (eC->sV());

    if (S->prjE() == eC) {
      // 5.1.1) S.prjE=eO: SV to eO or vA+ or vB-
      //        - SV to eO: if both nWa and nWb exist.
      //        - SV to vA+: if S-V or nWa=NULL.
      //        - SV to vB-: if S-V or nWb=NULL.

      if ((b_S_Vs && b_eC_from_vA) || (b_S_Ve && !b_eC_from_vA)) { //Handle S-vA.
        nWa = NULL;
      }
      else { //pWa strikes prjE to be nWa
        nWa = S->_get_nWa_on_prjE();
        if (nWa == NULL) {
          nWa = SE_sv_advance_Wa_to_prjE (S, heC, alphaCL, heR, alphaLR);
          nWa_detect_NE = true;
        }
      }

      if ((b_S_Ve && b_eC_from_vA) || (b_S_Vs && !b_eC_from_vA)) { //Handle S-vB.
        nWb = NULL;
      }
      else { //pWb strikes prjE to be nWb
        nWb = S->_get_nWb_on_prjE();
        if (nWb == NULL) {
          nWb = SE_sv_advance_Wb_to_prjE (S, heC, alphaCR, heL, alphaLR);
          nWb_detect_NE = true;
        }
      }
    }
    else if (S->prjE() == eL) { 
      // 5.1.2) S.prjE=eL: SV to eL or vA-
      // 5.2.1) S.prjE=eL: SVE to eL or v-
      //        - SV to eL: if nWb exists.
      //        - SV to vA-: if S-V or nWb=NULL.

      //nIa is pIa. In Some rare case, pWa can have one nextI.
      nWa = (gdt_welm*) pWa;

      if (b_S_V) { //Handle S-V.
        nWb = NULL;
      }
      else { //pWb strikes prjE to be nWb
        nWb = S->_get_nWb_on_prjE();
        if (nWb == NULL) {
          nWb = SE_sv_advance_Wb_to_prjE (S, heC, alphaCR, heL, alphaLR);
          nWb_detect_NE = true;
        }
      }
    }
    else {
      assert (S->prjE() == eR);
      // 5.1.3) S.prjE=eR: SV to eR or vB+
      // 5.2.2) S.prjE=eR: SVE to eR or v+
      //        - SV to eR: if nWa exists.
      //        - SV to vB+: if S-V or nWa=NULL.

      //nIb is pIb. In Some rare case, pWa can have one nextI.
      assert (pWb->nextI(1)==NULL);
      nWb = (gdt_welm*) pWb;

      if (b_S_V) { //Handle S-V.
        nWa = NULL;
      }
      else { //pWa strikes prjE to be nWa
        nWa = S->_get_nWa_on_prjE();
        if (nWa == NULL) {
          nWa = SE_sv_advance_Wa_to_prjE (S, heC, alphaCL, heR, alphaLR);
          nWa_detect_NE = true;
        }
      }
    }
  }
  break;
  default:
    assert (0);
  break;
  }

  *newWa = nWa;
  *newWb = nWb;
  
#if GDT_DEBUG_MSG
  if (nWa)
    assert (nWa->edge() == S->prjE());
  if (nWb)
    assert (nWb->edge() == S->prjE());

  if (n_verbose_>3) {
    vul_printf (vcl_cerr, " Advance S %d (v %d) to prjE %d at tauE %f, endtime %f.\n", 
                 S->id(), S->Snode()->id(), S->prjE()->id(), S->tauE(), S->edgeT()); 
  
    char astring[64], bstring[64];
    vcl_sprintf (astring, "%s", pWa->_is_RF() ? "irf " : "");
    vcl_sprintf (bstring, "%s", pWb->_is_RF() ? "irf " : "");
    vul_printf (vcl_cerr, "  pWa %s%d (%.3f, %.3f), pWb %s%d (%.3f, %.3f),\n", 
                 astring, pWa->edge()->id(), pWa->stau(), pWa->etau(),
                 bstring, pWb->edge()->id(), pWb->stau(), pWb->etau());

    if (nWa)
      vcl_sprintf (astring, "%s%d (%.3f, %.3f)", nWa->is_dege() ? "dege " : "",
                   nWa->edge()->id(), nWa->stau(), nWa->etau());
    else
      vcl_sprintf (astring, "NULL");
    if (nWb)
      vcl_sprintf (bstring, "%s%d (%.3f, %.3f)", nWb->is_dege() ? "dege " : "",
                   nWb->edge()->id(), nWb->stau(), nWb->etau());
    else
      vcl_sprintf (bstring, "NULL");
    vul_printf (vcl_cerr, "  nWa %s, nWb %s.\n", astring, bstring);
  }
#endif
}

void gdt_ws_manager::_del_childW_on_edge (gdt_welm* W, const dbmsh3d_gdt_edge* edge)
{
  vcl_vector<gdt_interval*>::iterator it = W->nextIs().begin();
  while (it != W->nextIs().end()) {
    gdt_welm* nW = (gdt_welm*) (*it);

    if (nW->edge() == edge) { 
      //delete nW. Be careful on the next iterator.
      if (it == W->nextIs().begin()) {
        delete_W_from_structure (nW);
        it = W->nextIs().begin();
      }
      else {
        vcl_vector<gdt_interval*>::iterator nit = it;
        nit--;
        delete_W_from_structure (nW);
        nit++;
        it = nit;
      }
    }
    else //go to the next
      it++;
  }
}

//: For S-from-Edge, propagate S->Wa to S->prjE
gdt_welm* gdt_ws_manager::SE_se_advance_Wa_to_prjE (const gdt_shock* S, 
                                                    const dbmsh3d_halfedge* heL, const double& alphaCL,
                                                    const dbmsh3d_halfedge* heR, const double& alphaCR)
{
  gdt_welm* nWa;

  if (S->prjE()->is_V_incident (S->Wa()->sV())) {
    nWa = _SE_advance_W_to_eL (S->Wa(), heL, alphaCL, S->edgeT());
    S->Wa()->set_tEL (GDT_HUGE);
  }
  else {
    assert (S->prjE()->is_V_incident (S->Wa()->eV()));

    nWa = _SE_advance_W_to_eR (S->Wa(), heR, alphaCR, S->edgeT());
    S->Wa()->set_tER (GDT_HUGE);
  }

  assert (nWa->edge() == S->prjE());
  return nWa;
}

//: For S-from-Edge, propagate S->Wb to S->prjE
gdt_welm* gdt_ws_manager::SE_se_advance_Wb_to_prjE (const gdt_shock* S,
                                                    const dbmsh3d_halfedge* heL, const double& alphaCL,
                                                    const dbmsh3d_halfedge* heR, const double& alphaCR)
{
  gdt_welm* nWb;

  if (S->prjE()->is_V_incident (S->Wb()->sV())) {
    nWb = _SE_advance_W_to_eL (S->Wb(), heL, alphaCL, S->edgeT());
    S->Wb()->set_tEL (GDT_HUGE);
  }
  else {
    assert (S->prjE()->is_V_incident (S->Wb()->eV()));
    nWb = _SE_advance_W_to_eR (S->Wb(), heR, alphaCR, S->edgeT());
    S->Wb()->set_tER (GDT_HUGE);
  }

  assert (nWb->edge() == S->prjE());
  return nWb;
}

//: For S-from-Vertex, propagate S->Wa to S->prjE
gdt_welm* gdt_ws_manager::SE_sv_advance_Wa_to_prjE (const gdt_shock* S, 
                                                    const dbmsh3d_halfedge* heC, const double& alphaCL,
                                                    const dbmsh3d_halfedge* heR, const double& alphaLR)
{
  gdt_welm* nWa;

  if (S->prjE() == heC->edge()) {
    if (S->prjE()->is_V_incident (S->Wa()->sV())) {
      nWa = _SE_advance_W_to_eL (S->Wa(), heC, alphaCL, S->edgeT());
      S->Wa()->set_tEL (GDT_HUGE);
    }
    else {
      assert (S->prjE()->is_V_incident (S->Wa()->eV()));
      nWa = _SE_advance_W_to_eR (S->Wa(), heC, alphaCL, S->edgeT());
      S->Wa()->set_tER (GDT_HUGE);
    }
  }
  else {
    assert (S->prjE() == heR->edge());
    if (S->prjE()->is_V_incident (S->Wa()->sV())) {
      nWa = _SE_advance_W_to_eL (S->Wa(), heR, alphaLR, S->edgeT());
      S->Wa()->set_tEL (GDT_HUGE);
    }
    else {
      assert (S->prjE()->is_V_incident (S->Wa()->eV()));
      nWa = _SE_advance_W_to_eR (S->Wa(), heR, alphaLR, S->edgeT());
      S->Wa()->set_tER (GDT_HUGE);
    }
  }

  assert (nWa->edge() == S->prjE());
  return nWa;
}

//: For S-from-Vertex, propagate S->Wb to S->prjE
//  eR = S->Wb->edge
gdt_welm* gdt_ws_manager::SE_sv_advance_Wb_to_prjE (const gdt_shock* S,
                                                    const dbmsh3d_halfedge* heC, const double& alphaCR,
                                                    const dbmsh3d_halfedge* heL, const double& alphaLR)
{
  gdt_welm* nWb;

  if (S->prjE() == heC->edge()) {
    if (S->prjE()->is_V_incident (S->Wb()->sV())) {
      nWb = _SE_advance_W_to_eL (S->Wb(), heC, alphaCR, S->edgeT());
      S->Wb()->set_tEL (GDT_HUGE);
    }
    else {
      assert (S->prjE()->is_V_incident (S->Wb()->eV()));
      nWb = _SE_advance_W_to_eR (S->Wb(), heC, alphaCR, S->edgeT());
      S->Wb()->set_tER (GDT_HUGE);
    }
  }
  else {
    assert (S->prjE() == heL->edge());
    if (S->prjE()->is_V_incident (S->Wb()->sV())) {
      nWb = _SE_advance_W_to_eL (S->Wb(), heL, alphaLR, S->edgeT());
      S->Wb()->set_tEL (GDT_HUGE);
    }
    else {
      assert (S->prjE()->is_V_incident (S->Wb()->eV()));
      nWb = _SE_advance_W_to_eR (S->Wb(), heL, alphaLR, S->edgeT());
      S->Wb()->set_tER (GDT_HUGE);
    }
  }

  assert (nWb->edge() == S->prjE());
  return nWb;
}

gdt_welm* gdt_ws_manager::_SE_advance_W_to_eL (gdt_welm* W, const dbmsh3d_halfedge* heL, 
                                               const double& alphaCL, const double& simtime)
{
  assert (heL);
  dbmsh3d_gdt_edge* eL = (dbmsh3d_gdt_edge*) heL->edge();

  //Project W to eL.
  double nH, nL, nStau, nEtau;
  IP_RESULT result = W->L_proj_tauS_tauE (alphaCL, eL, nL, nH, nStau, nEtau);
  if (result == IP_INVALID)
    return NULL;
  else if (result == IP_DEGE) {
    //Get the degeW of W.sV on eL
    return (gdt_welm*) W->sV()->_find_childI_on_edge (eL);
  }
  else {
    if (eL->fix_interval (nStau, nEtau) == false)
      return NULL;

    gdt_welm* nW = new gdt_welm (ITYPE_PSRC, nStau, nEtau, heL, W->psrc(), nL, nH, W, simtime);  
    tie_prevI_nextI (W, nW);

    detect_psrcW_Qs2_event (nW);

    return nW;
  }
}

//: Advance W to eR and create a new W on eR, given a known simtime.
//  Used in S-E events.
//  
gdt_welm* gdt_ws_manager::_SE_advance_W_to_eR (gdt_welm* W, const dbmsh3d_halfedge* heR, 
                                               const double& alphaCR, const double& simtime)
{
  assert (heR);
  dbmsh3d_gdt_edge* eR = (dbmsh3d_gdt_edge*) heR->edge();

  //Project W to eR.
  double nH, nL, nStau, nEtau;
  IP_RESULT result = W->R_proj_tauS_tauE (alphaCR, eR, nL, nH, nStau, nEtau);
  if (result == IP_INVALID)
    return NULL;
  else if (result == IP_DEGE) {
    //Get the degeW of W.eV on eR
    return (gdt_welm*) W->eV()->_find_childI_on_edge (eR);
  }
  else { //IP_VALID
    if (eR->fix_interval (nStau, nEtau) == false)
      return NULL;

    gdt_welm* nW = new gdt_welm (ITYPE_PSRC, nStau, nEtau, heR, W->psrc(), nL, nH, W, simtime);
    tie_prevI_nextI (W, nW);

    detect_psrcW_Qs2_event (nW);

    return nW;
  }
}

// ###########################################################################

// 1. Update tEL, tER, tVO of S->Wa and S->Wb
// 2. Propagate/Finalize Wa and Wb
void gdt_ws_manager::SE_update_pWa_pWb (const gdt_shock* S)
{
  gdt_welm* pWa = S->Wa();
  gdt_welm* pWb = S->Wb();

  if (S->Wa()->_is_RF()) { //L-RFS:
    //Do nothing to Wa, since all 3 has to be inf.
    assert (pWa->tEL() == GDT_HUGE);
    assert (pWa->tER() == GDT_HUGE);
    assert (pWa->tVO() == GDT_HUGE);

    //Two cases for Wb.
    if (S->prjE()->is_V_incident (pWb->eV())) {
      //For L-RFS to eR: Wb.tEL = Wb.tER = Wb.tVO = inf.
      pWb->set_tEL (GDT_HUGE);
      pWb->set_tER (GDT_HUGE);
      pWb->set_tVO (GDT_HUGE);
    }
    else { //For L-RFS to eL: Wb.tEL = inf.
      assert (S->prjE()->is_V_incident (pWb->sV()));
      pWb->set_tEL (GDT_HUGE);
    }
  }
  else if (pWb->_is_RF()) { //R-RFS:
    //Do nothing to Wb, since all 3 has to be inf.
    assert (pWb->tEL() == GDT_HUGE);
    assert (pWb->tER() == GDT_HUGE);
    assert (pWb->tVO() == GDT_HUGE);

    //Two cases for Wa.
    if (S->prjE()->is_V_incident (pWa->sV())) {
      //For /R-RFS to eL: Wa.tEL = Wa.tER = Wa.tVO = inf.
      pWa->set_tEL (GDT_HUGE);
      pWa->set_tER (GDT_HUGE);
      pWa->set_tVO (GDT_HUGE);
    }
    else { //For /R-RFS to eR: Wa.tER = inf.
      assert (S->prjE()->is_V_incident (pWa->eV()));
      pWa->set_tER (GDT_HUGE);
    }
  }
  else if (S->_is_from_edge()) { //SE:
    if (S->prjE()->is_V_incident (pWa->sV())) { 
      //For SE to eL: Wa.tEL = Wa.tER = Wa.tVO = inf. Wb.tEL = inf.
      pWa->set_tEL (GDT_HUGE);
      pWa->set_tER (GDT_HUGE);
      pWa->set_tVO (GDT_HUGE);
      pWb->set_tEL (GDT_HUGE);
    }
    else { 
      //For SE to eR: Wb.tEL = Wb.tER = Wb.tVO = inf. Wa.tER = inf.
      assert (S->prjE()->is_V_incident (pWb->eV())); 
      pWb->set_tEL (GDT_HUGE);
      pWb->set_tER (GDT_HUGE);
      pWb->set_tVO (GDT_HUGE);
      pWa->set_tER (GDT_HUGE);
    }
  }
  else { //SV:
    if (S->prjE() == pWa->edge()) {
      //For SV to eL: Wa.tEL = Wa.tER = Wa.tVO = inf.
      pWa->set_tEL (GDT_HUGE);
      pWa->set_tER (GDT_HUGE);
      pWa->set_tVO (GDT_HUGE);
      
      //For SV to eL: if eL is on Wb's eV, Wb.tER = inf.
      //           else, eL is on Wb's sV, Wb.tEL = inf.
      if (pWa->edge()->is_V_incident (pWb->eV()))
        pWb->set_tER (GDT_HUGE);
      else {
        assert (pWa->edge()->is_V_incident (pWb->sV()));
        pWb->set_tEL (GDT_HUGE);
      }
    }
    else if (S->prjE() == pWb->edge()) {
      //For SV to eR: Wb.tEL = Wb.tER = Wb.tVO = inf.
      pWb->set_tEL (GDT_HUGE);
      pWb->set_tER (GDT_HUGE);
      pWb->set_tVO (GDT_HUGE);

      //For SV to eR: if eR is on Wa's eV, Wa.tER = inf.
      //           else, eR is on Wa's SV, Wa.tEL = inf.
      if (pWb->edge()->is_V_incident (pWa->eV()))
        pWa->set_tER (GDT_HUGE);
      else {
        assert (pWb->edge()->is_V_incident (pWa->sV()));
        pWa->set_tEL (GDT_HUGE);
      }
    }
    else { //For SV to eO: set both Wa's and Wb's tEL, tER, tVO = inf.
      pWa->set_tEL (GDT_HUGE);
      pWa->set_tER (GDT_HUGE);
      pWa->set_tVO (GDT_HUGE);
      pWb->set_tEL (GDT_HUGE);
      pWb->set_tER (GDT_HUGE);
      pWb->set_tVO (GDT_HUGE);
    }
  }

  //If Wa is RF, finalize Wa, since all events will be handled by nWa.
  //Otherwise, try to propagate/finalize Wa for all other cases.
  if (pWa->_is_RF()) {
    pWa->set_final();
  }
  else if (pWa->is_final() == false) {
    if (pWa->try_finalize()) {
      pWa->edge()->_attach_interval (pWa);

      #if GDT_DEBUG_MSG
      if (n_verbose_>3)
        vul_printf (vcl_cerr, "  Finalize_I psrcW %d (%.3f, %.3f).\n", 
                     pWa->edge()->id(), pWa->stau(), pWa->etau());
      #endif
    }
    else {
      pWa->set_next_event (WENE_NA);
      pWa->set_simT (S->edgeT());
      add_to_Qw (pWa);
    }
  }

  //If Wb is RF, finalize Wb, since all events will be handled by nWb.
  //Otherwise, try to propagate/finalize Wb for all other cases.
  if (pWb->_is_RF()) {
    pWb->set_final();
  }
  else if (pWb->is_final() == false) {
    if (pWb->try_finalize()) {
      pWb->edge()->_attach_interval (pWb);

      #if GDT_DEBUG_MSG
      if (n_verbose_>3)
        vul_printf (vcl_cerr, "  Finalize_I psrcW %d (%.3f, %.3f).\n", 
                     pWb->edge()->id(), pWb->stau(), pWb->etau());
      #endif
    }
    else {
      pWb->set_next_event (WENE_NA);
      pWb->set_simT (S->edgeT());
      add_to_Qw (pWb);
    }
  }
}

//: return true if flip is required.
//
bool gdt_ws_manager::SE_associate_new_W_S_W (gdt_shock* S, gdt_welm* nWa, gdt_welm* nWb)
{
  if (S->Wa()->_is_RF()) { //SO_ILCONTACT
    if (S->Wb()->edge()->eV() == S->prjE()->eV()) {
      associate_ep_W_S_W (nWa, S, nWb);
      return false;
    }
    else {
      associate_ep_W_S_W (nWb, S, nWa);
      return true;
    }
  }
  else if (S->Wb()->_is_RF()) { //SO_IRCONTACT
    if (S->Wa()->edge()->sV() == S->prjE()->sV()) {
      associate_ep_W_S_W (nWa, S, nWb);
      return false;
    }
    else {
      associate_ep_W_S_W (nWb, S, nWa);
      return true;
    }
  }
  else if (S->_is_from_edge()) { //SO_EDGE
    if (S->Wa()->edge()->sV() == S->prjE()->sV() ||
        S->Wa()->edge()->eV() == S->prjE()->eV()) {
      associate_ep_W_S_W (nWa, S, nWb);
      return false;
    }
    else {
      associate_ep_W_S_W (nWb, S, nWa);
      return true;
    }
  }
  else { //S from vertex
    if (S->_is_Ia_outward()) {
      if (S->Wa()->edge()->eV() == S->prjE()->eV()) {
        associate_ep_W_S_W (nWb, S, nWa);
        return true;
      }
      else {
        associate_ep_W_S_W (nWa, S, nWb);
        return false;
      }
    }
    else { //SO_R_VERTEX
      if (S->Wa()->edge()->sV() == S->prjE()->sV()) {
        associate_ep_W_S_W (nWa, S, nWb);
        return false;
      }
      else {
        associate_ep_W_S_W (nWb, S, nWa);
        return true;
      }
    }
  }
}

void gdt_ws_manager::SE_propagate_nSWa (gdt_welm* nSWa, double timeS, const bool detect_NE)
{
  if (detect_NE) {
    nSWa->compute_tVS ();
    nSWa->compute_tEL_tER_tOV ();
    assert (_leqD (cur_simT_, nSWa->tEL()));
    assert (_leqD (cur_simT_, nSWa->tER()));
    assert (_leqD (cur_simT_, nSWa->tVO()));
  }
  nSWa->set_tVE (GDT_HUGE);
    
  if (_try_finalize (nSWa) == false) {
    //Need to reset the event to N/A since the shock has changed the configuration.
    nSWa->set_next_event (WENE_NA);
    nSWa->set_simT (timeS);
    add_to_Qw (nSWa);
  }
}

void gdt_ws_manager::SE_propagate_nSWb (gdt_welm* nSWb, double timeS, const bool detect_NE)
{
  if (detect_NE) {
    nSWb->compute_tVE ();
    nSWb->compute_tEL_tER_tOV ();
    assert (_leqD (cur_simT_, nSWb->tEL()));
    assert (_leqD (cur_simT_, nSWb->tER()));
    assert (_leqD (cur_simT_, nSWb->tVO()));
  }
  nSWb->set_tVS (GDT_HUGE);

  if (_try_finalize (nSWb) == false) {
    //Need to reset the event to N/A since the shock has changed the configuration.
    nSWb->set_next_event (WENE_NA);
    nSWb->set_simT (timeS);
    add_to_Qw (nSWb);
  }
}

//: return true if W is finalized_I.
//  !! Whether W is finalized_I or not, it should be added to Qw.
bool gdt_ws_manager::_try_finalize (gdt_welm* W)
{
  ///assert (!W->is_final());
  if (W->is_final())
    return true;
  
  if (W->try_finalize()) {
    W->edge()->_attach_interval (W);

    #if GDT_DEBUG_MSG
    if (n_verbose_>3)
      vul_printf (vcl_cerr, "  Finalize_I psrcW %d (%.3f, %.3f).\n", 
                   W->edge()->id(), W->stau(), W->etau());
    #endif
    return true;
  }

  return false;
}

// ##########################################################################

// nWb == NULL
void gdt_ws_manager::SE_s_to_vL (gdt_shock* S, gdt_welm* nWa, const bool nWa_detect_NE)
{
  gdt_welm* pWa = S->Wa();
  gdt_welm* pWb = S->Wb();

  #if GDT_DEBUG_MSG
  n_S_term_at_V_++;
  #endif
  assert (!pWa->_is_RF() || !pWb->_is_RF());

  //Set nWa's ending tau. Propagate/Finalize nWa
  dbmsh3d_gdt_vertex_3d* v;
  if (S->tauE() == 0) {
    S->_set_Wa (NULL);
    associate_S_Wb (S, nWa);

    if (pWa != nWa)
      SE_propagate_nSWb (nWa, S->edgeT(), nWa_detect_NE);

    v = (dbmsh3d_gdt_vertex_3d*) S->prjE()->sV();
  }
  else {
    associate_Wa_S (nWa, S);
    S->_set_Wb (NULL);

    if (pWa != nWa)
      SE_propagate_nSWa (nWa, S->edgeT(), nWa_detect_NE);

    v = (dbmsh3d_gdt_vertex_3d*) S->prjE()->eV();
  }

  //Handle the S-V sink case:
  //If there exists an active vertex (v, pWa, pWb) in Qv, remove it.
  vcl_multimap<double, gdt_active_vertex*>::iterator vit = _find_in_Qv (v);
  if (vit != Qv_.end()) {
    gdt_active_vertex* av = (*vit).second;
    if ((av->Wa_ == pWa && av->Wb_ == pWb) || (av->Wa_ == pWb && av->Wb_ == pWa))
      Qv_.erase (vit);
  }

  //Try to remove nWa from Qw
  try_remove_from_Qw (nWa);

  //Finalize S and associate set (pWa, pWb) to S
  S->set_Enode (v);
  S->_set_Wa (pWa);
  S->_set_Wb (pWb);
}

// nWa == NULL
void gdt_ws_manager::SE_s_to_vR (gdt_shock* S, gdt_welm* nWb, const bool nWb_detect_NE)
{
  gdt_welm* pWa = S->Wa();
  gdt_welm* pWb = S->Wb();

  #if GDT_DEBUG_MSG
  n_S_term_at_V_++;
  #endif
  assert (!pWa->_is_RF() && !pWb->_is_RF()); //both pWa, pWb non-rarefaction

  //Set nWb's ending tau. Propagate/Finalize nWb
  dbmsh3d_gdt_vertex_3d* v;
  if (S->tauE() == 0) {
    S->_set_Wa (NULL);
    associate_S_Wb (S, nWb);

    if (pWb != nWb)
      SE_propagate_nSWb (nWb, S->edgeT(), nWb_detect_NE);

    v = (dbmsh3d_gdt_vertex_3d*) S->prjE()->sV();
  }
  else {      
    associate_Wa_S (nWb, S);
    S->_set_Wb (NULL);

    if (pWb != S->Wa())
      SE_propagate_nSWa (nWb, S->edgeT(), nWb_detect_NE);

    v = (dbmsh3d_gdt_vertex_3d*) S->prjE()->eV();
  }

  //Handle the S-V sink case:
  //If there exists an active vertex (v, pWa, pWb) in Qv, remove it.
  vcl_multimap<double, gdt_active_vertex*>::iterator vit = _find_in_Qv (v);
  if (vit != Qv_.end()) {    
    gdt_active_vertex* av = (*vit).second;
    if ((av->Wa_ == pWa && av->Wb_ == pWb) || (av->Wa_ == pWb && av->Wb_ == pWa))
      Qv_.erase (vit);
  }

  //Try to remove nWb from Qw
  try_remove_from_Qw (nWb);

  //Finalize S and associate set (pWa, pWb) to S
  S->set_Enode (v);
  S->_set_Wa (pWa);
  S->_set_Wb (pWb);
}

//: eC = pWb->edge
//  eL = S->prjE
void gdt_ws_manager::SE_srf_to_degeWa (gdt_shock* S, gdt_welm* nWa, gdt_welm* nWb, 
                                       const dbmsh3d_gdt_edge* eC, const bool prjE_is_eL,
                                       const bool nWb_detect_NE)
{
  gdt_welm* pWa = S->Wa();
  gdt_welm* pWb = S->Wb();
  dbmsh3d_gdt_edge* prjE = S->prjE();

  #if GDT_DEBUG_MSG
  n_S_E_++;

  //Assert the S-E point is equal-dist from both intervals. 
  assert (nWa->edge() == nWb->edge());
  double da = nWa->get_dist_at_tau_fuzzy (S->tauE());
  double db = nWb->get_dist_at_tau_fuzzy (S->tauE());
  assert (_eqD (da, db));
  #endif

  //We are in the middle of transformation: nWa = degeWa, nWb = pWb->child.
  //Associate wavefronts for shock after the S-E strike. Two cases.
  if ( (prjE_is_eL && eC->sV() == prjE->sV()) ||
       (!prjE_is_eL && eC->eV() == prjE->eV()) ) { //No flip between pW and nW.

    //Fix the ending tau of nWa->etau and nWb->stau.
    nWa->_set_etau (S->tauE());
    nWb->_set_stau (S->tauE());

    gdt_welm* degeWa = nWa;
    //Try to locate the nWa on the next face.
    dbmsh3d_gdt_edge* nextF_eR = nWb->get_nextF_eR ();

    //If the next face does not exist, finalize degeWa.
    associate_S_Wb (S, nWb);
    if (nextF_eR == NULL)
      associate_Wa_S (degeWa, S);
    else { //nWa is the RF on eR of the next face.
      nWa = (gdt_welm*) prjE->sV()->_find_childI_on_edge (nextF_eR);
      RFStoE_associate_Wa_S (degeWa, nWa, S);
    }

    if (pWb != nWb)
      SE_propagate_nSWb (nWb, S->edgeT(), nWb_detect_NE);

    //Finalize degeWa.
    bool final = _try_finalize (degeWa);
    assert (final);

    //Finalize pWa.    
    final = _try_finalize (pWa);
    assert (final);
    try_remove_from_Qw (pWa); //A hack here!!
  }
  else { //There is a flip between pW and nW.
    if (prjE_is_eL)
      assert (eC->sV() == prjE->eV());
    else
      assert (eC->eV() == prjE->sV());

    //Fix the ending tau of nWb->etau and nWa->stau.
    nWb->_set_etau (S->tauE());
    nWa->_set_stau (S->tauE());

    gdt_welm* degeWa = nWa;
    //Try to locate the nWa on the next face.
    dbmsh3d_gdt_edge* nextF_eL = nWb->get_nextF_eL ();
    
    //If the next face does not exist, finalize degeWa.
    associate_Wa_S (nWb, S);
    if (nextF_eL == NULL)
      associate_S_Wb (S, degeWa);
    else { //nWa is the RF on eL of the next face.
      nWa = (gdt_welm*) prjE->eV()->_find_childI_on_edge (nextF_eL);
      RFStoE_associate_S_Wb (S, degeWa, nWa);
    }

    if (pWb != nWb)
      SE_propagate_nSWa (nWb, S->edgeT(), nWb_detect_NE);
    
    //Finalize degeWa.
    bool final = _try_finalize (degeWa);
    assert (final);

    //Finalize pWa.
    final = _try_finalize (pWa);
    assert (final);
    try_remove_from_Qw (pWa); //A hack here!!
  }

  dbmsh3d_halfedge* nextF_he = nWb->he()->pair();

  //If S intersects mesh boundary, finalize S and return.
  if (nextF_he == NULL) { 
    vgl_point_3d<double> P3 = nWb->_point_from_tau (S->tauE());
    dbmsh3d_gdt_vertex_3d* N = new_snode (P3, S->edgeT());
    add_snode (N);
    S->set_Enode (N);
    S->_set_Wa (pWa);
    S->_set_Wb (pWb);
    return;
  }
  
  S->_add_cur_selm ();

  //Propagate S to the nextF and intersect it to its next prjE.
  dbmsh3d_face* nextF = nextF_he->face();  
  SE_propagate_S_to_face (S, nextF);

  S->set_propagated (false);
  add_to_Qs (S);
}

void gdt_ws_manager::SE_srf_to_degeWb (gdt_shock* S, gdt_welm* nWa, gdt_welm* nWb, 
                                       const dbmsh3d_gdt_edge* eC, const bool prjE_is_eR,
                                       const bool nWa_detect_NE)
{
  gdt_welm* pWa = S->Wa();
  gdt_welm* pWb = S->Wb();
  dbmsh3d_gdt_edge *prjE = S->prjE();

  #if GDT_DEBUG_MSG
  n_S_E_++;

  //Assert the S-E point is equal-dist from both intervals. 
  assert (nWa->edge() == nWb->edge());
  double da = nWa->get_dist_at_tau_fuzzy (S->tauE());
  double db = nWb->get_dist_at_tau_fuzzy (S->tauE());
  assert (_eqD (da, db));
  #endif

  //We are in the middle of transformation: nWa = pWa->child, nWb = degeWb.
  //Associate wavefronts for shock after the S-E strike. Two cases.
  if ( (prjE_is_eR && eC->eV() == prjE->eV()) ||
       (!prjE_is_eR && eC->sV() == prjE->sV()) ) { //No flip between pW and nW.

    //Fix the ending tau of nWa->etau and nWb->stau.
    nWa->_set_etau (S->tauE());
    nWb->_set_stau (S->tauE());
    
    gdt_welm* degeWb = nWb;
    //Try to locate the nWb on the next face.
    dbmsh3d_gdt_edge* nextF_eL = nWa->get_nextF_eL ();

    //If the next face does not exist, return.
    associate_Wa_S (nWa, S);
    if (nextF_eL == NULL) { 
      associate_S_Wb (S, degeWb);
    }
    else {
      //nWb is the RF on neL of the next face.
      nWb = (gdt_welm*) prjE->eV()->_find_childI_on_edge (nextF_eL);
      RFStoE_associate_S_Wb (S, degeWb, nWb);   
    }

    if (pWa != nWa)
      SE_propagate_nSWa (nWa, S->edgeT(), nWa_detect_NE);

    //Finalize degeWb.
    bool final = _try_finalize (degeWb);
    assert (final);

    //Finalize pWb.
    final = _try_finalize (pWb);
    assert (final);
    try_remove_from_Qw (pWb); //A hack here!!   
  }
  else { //There is a flip between pW and nW.
    if (prjE_is_eR)
      assert (eC->eV() == prjE->sV());
    else
      assert (eC->sV() == prjE->eV());

    //Fix the ending tau of nWb->etau and nWa->stau.
    nWb->_set_etau (S->tauE());
    nWa->_set_stau (S->tauE());

    gdt_welm* degeWb = nWb;
    //Try to locate the nWb on the next face.
    dbmsh3d_gdt_edge* nextF_eR = nWa->get_nextF_eR ();

    //If the next face does not exist, finalize degeWb.
    associate_S_Wb (S, nWa);
    if (nextF_eR == NULL)
      associate_Wa_S (degeWb, S);
    else { //nWb is the RF on eR of the next face.
      nWb = (gdt_welm*) prjE->sV()->_find_childI_on_edge (nextF_eR);
      RFStoE_associate_Wa_S (degeWb, nWb, S);
    }

    if (pWa != nWa)
      SE_propagate_nSWb (nWa, S->edgeT(), nWa_detect_NE); 

    //Finalize degeWb.
    bool final = _try_finalize (degeWb);
    assert (final);

    //Finalize pWb.
    final = _try_finalize (pWb);
    assert (final);
    try_remove_from_Qw (pWb); //A hack here!!   
  }

  dbmsh3d_halfedge* nextF_he = nWa->he()->pair();

  //If S intersects mesh boundary, finalize S and return.
  if (nextF_he == NULL) { 
    vgl_point_3d<double> P3 = nWa->_point_from_tau (S->tauE());
    dbmsh3d_gdt_vertex_3d* N = new_snode (P3, S->edgeT());
    add_snode (N);
    S->set_Enode (N);
    S->_set_Wa (pWa);
    S->_set_Wb (pWb);
    return;
  }

  S->_add_cur_selm ();

  //Propagate S to the nextF and intersect it to its next prjE.
  dbmsh3d_face* nextF = nextF_he->face();
  SE_propagate_S_to_face (S, nextF);

  S->set_propagated (false);
  add_to_Qs (S);
}

void gdt_ws_manager::SE_sv_to_Ia (gdt_shock* S, gdt_welm* nWa, gdt_welm* nWb, 
                                  const bool Ia_toward_v, const bool b_SVE,
                                  const bool nWb_detect_NE)
{
  gdt_welm* pWa = S->Wa();
  gdt_welm* pWb = S->Wb();

  #if GDT_DEBUG_MSG
  n_S_E_++; 

  //Assert the S-E point is equal-dist from both intervals. 
  assert (nWa->edge() == nWb->edge());
  double da = nWa->get_dist_at_tau_fuzzy (S->tauE());
  double db = nWb->get_dist_at_tau_fuzzy (S->tauE());
  assert (_eqD (da, db));
  #endif

  //We are in the middle of transformation: nWa = pWa , nWb = pWb->child.
  //Associate wavefronts for shock (Wa, S, Wb) after the S-eL strike, Two cases.
  assert (pWa->prevI());

  if (Ia_toward_v) { //For pWa inward:  S.Wa = nWb, S.Wb = pWa->prev.
    if (b_SVE) { //For SVE: fix nWa.etau and nWb.stau
      nWa->_set_etau (S->tauE());
      nWb->_set_stau (S->tauE());

      SVtoE_associate_Wa_S (pWa, S);
      associate_S_Wb (S, nWb);

      if (pWb != nWb)
        SE_propagate_nSWb (nWb, S->edgeT(), nWb_detect_NE);
    }
    else { //For SV: fix nWb.etau and nWa.stau
      nWb->_set_etau (S->tauE());
      nWa->_set_stau (S->tauE());

      associate_Wa_S (nWb, S);
      SVtoE_associate_S_Wb (S, pWa);   

      if (pWb != nWb)
        SE_propagate_nSWa (nWb, S->edgeT(), nWb_detect_NE);   
    } 
  }
  else { //For pWa outward: S.Wa = pWa->prev, S.Wb = nWb.
    if (b_SVE) { //For SVE: fix nWb.etau and nWa.stau
      nWb->_set_etau (S->tauE());
      nWa->_set_stau (S->tauE());

      associate_Wa_S (nWb, S);
      SVtoE_associate_S_Wb (S, pWa);

      if (pWb != nWb)
        SE_propagate_nSWa (nWb, S->edgeT(), nWb_detect_NE);
    }
    else { //For SV: fix nWa.etau and nWb.stau
      nWa->_set_etau (S->tauE());
      nWb->_set_stau (S->tauE());

      SVtoE_associate_Wa_S (pWa, S);
      associate_S_Wb (S, nWb);    

      if (pWb != nWb)
        SE_propagate_nSWb (nWb, S->edgeT(), nWb_detect_NE);  
    }    
  }

  //Finalize pWa.
  bool final = _try_finalize (pWa);
  assert (final);
  try_remove_from_Qw (pWa); //A hack here!!

  dbmsh3d_halfedge* nextF_he = pWa->he();

  //If S intersects mesh boundary, finalize S and return.
  if (nextF_he == NULL) { 
    vgl_point_3d<double> P3 = pWa->_point_from_tau (S->tauE());
    dbmsh3d_gdt_vertex_3d* N = new_snode (P3, S->edgeT());
    add_snode (N);
    S->set_Enode (N);    
    S->_set_Wa (pWa);
    S->_set_Wb (pWb);
    return;
  }

  S->_add_cur_selm ();

  //Propagate S to the nextF and intersect it to its next prjE.
  dbmsh3d_face* nextF = nextF_he->face();  
  SE_propagate_S_to_face (S, nextF);

  S->set_propagated (false);
  add_to_Qs (S);
}

void gdt_ws_manager::SE_sv_to_Ib (gdt_shock* S, gdt_welm* nWa, gdt_welm* nWb, 
                                  const bool Ib_toward_v, const bool b_SVE,
                                  const bool nWa_detect_NE)
{
  gdt_welm* pWa = S->Wa();
  gdt_welm* pWb = S->Wb();

  #if GDT_DEBUG_MSG
  n_S_E_++;

  //Assert the S-E point is equal-dist from both intervals.
  assert (nWa->edge() == nWb->edge());
  double da = nWa->get_dist_at_tau_fuzzy (S->tauE());
  double db = nWb->get_dist_at_tau_fuzzy (S->tauE());
  assert (_eqD (da, db));
  #endif

  //We are in the middle of transformation: nWa = pWa->child, nWb = pWb.
  //Associate wavefronts for shock (Wa, S, Wb) after the S-eR strike, Two cases.
  assert (pWb->prevI());

  if (Ib_toward_v) { //For pWb inward:  S.Wa = nWa, S.Wb = pWb->prev.
    if (b_SVE) { //For SVE: nWb.etau and nWa.stau
      nWb->_set_etau (S->tauE());
      nWa->_set_stau (S->tauE());

      SVtoE_associate_Wa_S (pWb, S);
      associate_S_Wb (S, nWa);

      if (pWa != nWa)
        SE_propagate_nSWb (nWa, S->edgeT(), nWa_detect_NE);
    }
    else { //For SV: fix nWa.etau and nWb.stau
      nWa->_set_etau (S->tauE());
      nWb->_set_stau (S->tauE());

      associate_Wa_S (nWa, S);
      SVtoE_associate_S_Wb (S, pWb);

      if (pWa != nWa)
        SE_propagate_nSWa (nWa, S->edgeT(), nWa_detect_NE);
    }    
  }
  else { //For pWb outward: S.Wa = pWb->prev, S.Wb = nWa.
    if (b_SVE) { //For SVE: fix nWa.etau and nWb.stau
      nWa->_set_etau (S->tauE());
      nWb->_set_stau (S->tauE());

      associate_Wa_S (nWa, S);
      SVtoE_associate_S_Wb (S, pWb);

      if (pWa != nWa)
        SE_propagate_nSWa (nWa, S->edgeT(), nWa_detect_NE);
    }
    else { //For SV: fix nWb.etau and nWa.stau
      nWb->_set_etau (S->tauE());
      nWa->_set_stau (S->tauE());

      SVtoE_associate_Wa_S (pWb, S);
      associate_S_Wb (S, nWa);
      
      if (pWa != nWa) //Propagate/Finalize S's new Wb
        SE_propagate_nSWb (nWa, S->edgeT(), nWa_detect_NE);
    }    
  }

  //Finalize pWb.
  bool final = _try_finalize (pWb);
  assert (final);
  try_remove_from_Qw (pWb); //A hack here!!

  dbmsh3d_halfedge* nextF_he = pWb->he();

  //If S intersects mesh boundary, finalize S and return.
  if (nextF_he == NULL) { 
    vgl_point_3d<double> P3 = pWb->_point_from_tau (S->tauE());
    dbmsh3d_gdt_vertex_3d* N = new_snode (P3, S->edgeT());
    add_snode (N);
    S->set_Enode (N);    
    S->_set_Wa (pWa);
    S->_set_Wb (pWb);
    return;
  }

  S->_add_cur_selm ();

  //Propagate S to the nextF and intersect it to its next prjE.
  dbmsh3d_face* nextF = nextF_he->face();
  SE_propagate_S_to_face (S, nextF);

  S->set_propagated (false);
  add_to_Qs (S);
}

void gdt_ws_manager::SE_se_to_e (gdt_shock* S, gdt_welm* nWa, gdt_welm* nWb,
                                 const bool nWa_detect_NE, const bool nWb_detect_NE)
{
  gdt_welm* pWa = S->Wa();
  gdt_welm* pWb = S->Wb();

  #if GDT_DEBUG_MSG
  n_S_E_++;

  //Assert the S-E point is equal-dist from both intervals.
  assert (nWa->edge() == nWb->edge());
  double da = nWa->get_dist_at_tau_fuzzy (S->tauE());
  double db = nWb->get_dist_at_tau_fuzzy (S->tauE());
  assert (_eqD (da, db));
  #endif

  bool bFlip = SE_associate_new_W_S_W (S, nWa, nWb);

  //Now S->Wa() and S->Wb() are the new wavefront elements.
  //Fix the ending tau of S->Wa and S->Wb.
  S->Wa()->_set_etau (S->tauE());
  S->Wb()->_set_stau (S->tauE());

  //Propagate/Finalize S->Wa and S->Wb.
  if (bFlip) {
    SE_propagate_nSWa (nWb, S->edgeT(), nWb_detect_NE);
    SE_propagate_nSWb (nWa, S->edgeT(), nWa_detect_NE);  
  }
  else {
    SE_propagate_nSWa (nWa, S->edgeT(), nWa_detect_NE);
    SE_propagate_nSWb (nWb, S->edgeT(), nWb_detect_NE);  
  }

  //Determine the nextF. 
  dbmsh3d_halfedge* nextF_he;
  if (!S->Wa()->is_dege())
    nextF_he = S->Wa()->he()->pair();
  else {
    //Note that the degeW.he is arbitrary. Compare with the oldF.
    nextF_he = S->Wa()->he();
    dbmsh3d_face* oldF = pWa->curF();
    if (nextF_he->face() == oldF)
      nextF_he = nextF_he->pair();
  }

  //If S intersects mesh boundary, finalize S and return.
  if (nextF_he == NULL) { 
    vgl_point_3d<double> P3 = S->Wa()->_point_from_tau (S->tauE());
    dbmsh3d_gdt_vertex_3d* N = new_snode (P3, S->edgeT());
    add_snode (N);
    S->set_Enode (N);
    S->_set_Wa (pWa);
    S->_set_Wb (pWb);
    return;
  }

  S->_add_cur_selm ();

  //Propagate S to the nextF and intersect it to its next prjE.
  dbmsh3d_face* nextF = nextF_he->face();
  SE_propagate_S_to_face (S, nextF);

  S->set_propagated (false);
  add_to_Qs (S);
}

// ##########################################################################

void gdt_ws_manager::SE_propagate_S_to_face (gdt_shock* S, const dbmsh3d_face* nextF)
{  
  S->set_startT (S->edgeT());
  assert (S->simT() == S->startT());

  // Compute the local orientation angles.
  const dbmsh3d_gdt_edge* neC = S->prjE();
  const dbmsh3d_gdt_edge* neL = (dbmsh3d_gdt_edge*) nextF->find_next_bnd_E (neC->sV(), neC);
  const dbmsh3d_gdt_edge* neR = (dbmsh3d_gdt_edge*) nextF->find_next_bnd_E (neC->eV(), neC);
  double alphaCL, alphaCR, alphaLR;
  m2t_compute_tri_angles (neC->len(), neL->len(), neR->len(), alphaCL, alphaCR, alphaLR);

  // Find the next prjE for S: either neL or neR.
  // Compute the upper bound S-E intersection tauE and time.
  double tauL, tauR, tauC;
  double tL = GDT_HUGE;
  double tR = GDT_HUGE;
  double tC = GDT_HUGE;

  gdt_welm* Wa = S->Wa();
  gdt_welm* Wb = S->Wb();
  
  //Handle the case that nWa/nWb is degeW.
  //Note that after the change, S can be SRF-L or SRF-R. May need to swtich Wa/Wb
  if (Wa->is_dege()) {
    Wa = (gdt_welm*) Wa->sV()->_find_childI_on_edge (neR);
    associate_Wa_S (Wa, S);
    S->_compute_c();
    S->_compute_a ();
    S->_compute_b2 ();
  }
  if (Wb->is_dege()) {
    Wb = (gdt_welm*) Wb->eV()->_find_childI_on_edge (neL);
    associate_S_Wb (S, Wb);
    S->_compute_c();
    S->_compute_a ();
    S->_compute_b2 ();
  }

  //: The nextS after a S-E can only be:
  //  SRF-L, SRF-R, SE, or SV.
  //
  if (Wa->_is_RF()) { //SRF-L to eL or eR
    next_SRF_L_to_eL (Wa, Wb, neL, alphaCL, tauL, tL);
    next_SRF_L_to_eR (Wa, Wb, neR, alphaCR, tauR, tR);
  }
  else if (Wb->_is_RF()) { //SRF-R to eL or eR
    next_SRF_R_to_eL (Wa, Wb, neL, alphaCL, tauL, tL);
    next_SRF_R_to_eR (Wa, Wb, neR, alphaCR, tauR, tR);
  }
  else if (Wa->edge() == Wb->edge()) { //next_SE_to_eL, next_SE_to_eR
    next_SE_to_eL (Wa, Wb, neL, alphaCL, tauL, tL);
    next_SE_to_eR (Wa, Wb, neR, alphaCR, tauR, tR);
  }
  else if (Wa->edge() == neL) { //next_SV_SVE_to_eL, next_SV_to_eC
    assert (Wb->edge() == neC);
    //Wa.e is neL, Wb.e is neC, the other is neR
    next_SV_SVE_to_eL (Wa, Wb, neL, alphaCL, tauL, tL);
    next_SV_to_eC (Wa, Wb, neR, alphaLR, alphaCR, tauR, tR);

    //It is possible for SV/SVE to intersect back to neC.
    next_SV_SVE_to_eR (Wa, Wb, neC, alphaCL, tauC, tC);
  }
  else if (Wb->edge() == neR) { //next_SV_SVE_to_eR, next_SV_to_eC
    assert (Wa->edge() == neC);    
    //Wa.e is neC, Wb.e is neR, the other is neL
    next_SV_SVE_to_eR (Wa, Wb, neR, alphaCR, tauR, tR);
    next_SV_to_eC (Wa, Wb, neL, alphaCL, alphaLR, tauL, tL);

    //It is possible for SV/SVE to intersect back to neC.
    next_SV_SVE_to_eR (Wa, Wb, neC, alphaCR, tauC, tC);
  }
  else 
    assert (0);

  //For the case of S from neC, the existing S-E is not a solustion!
  if (_leqF (tC, S->startT()))
    tC = GDT_HUGE;

  S->_set_next_prjE (neL, tauL, tL, neR, tauR, tR, neC, tauC, tC);
}

