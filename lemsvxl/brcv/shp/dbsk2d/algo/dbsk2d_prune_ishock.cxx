// This is brcv/shp/dbsk2d/algo/dbsk2d_prune_ishock.cxx

//:
// \file

#include <dbsk2d/algo/dbsk2d_prune_ishock.h>

#include <dbsk2d/dbsk2d_ishock_node.h>
#include <dbsk2d/dbsk2d_ishock_edge.h>
#include <dbsk2d/dbsk2d_ishock_contact.h>
#include <dbsk2d/dbsk2d_ishock_pointpoint.h>
#include <dbsk2d/dbsk2d_ishock_pointline.h>
#include <dbsk2d/dbsk2d_ishock_lineline.h>
#include <dbsk2d/dbsk2d_ishock_lineline_thirdorder.h>
#include <dbsk2d/dbsk2d_ishock_pointarc.h>
#include <dbsk2d/dbsk2d_ishock_linearc.h>
#include <dbsk2d/dbsk2d_ishock_arcarc.h>
#include <dbsk2d/dbsk2d_ishock_arcarc_thirdorder.h>

#include <dbsk2d/dbsk2d_ishock_graph.h>

#include <dbsk2d/dbsk2d_shock_node.h>
#include <dbsk2d/dbsk2d_shock_edge.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/dbsk2d_shock_ishock_node.h>
#include <dbsk2d/dbsk2d_shock_grouped_ishock_edge.h>

#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vsol/vsol_polyline_2d.h>

//: compute the salency of this shock element (edge/node)
void dbsk2d_prune_ishock::compute_shock_saliency(dbsk2d_ishock_elm* selm)
{
  double  dOC = 0;      // change in the original contour length by the removal of the current shock
  double  dNC = 0;      // length of the new contour added
  double  dPnCost = 0;  // cost of pruning this shock
                        //dPnCost = |_dOC + _dNC(parents) - _dNC| + dPnCost(parents)

  double R, thetaE;

  switch (selm->type())
  {
    case dbsk2d_ishock_elm::SNODE:
    {
      dbsk2d_ishock_node* snode = (dbsk2d_ishock_node*)selm;

      //second order nodes (don't know how to handle them yet)
      if (snode->is_a_source() || snode->is_a_sink()){
        dPnCost = ISHOCK_DIST_HUGE;
      }
      else {
        //Accumulate all the information from the parent edges.
        VECTOR_TYPE start_angle=-1;
        VECTOR_TYPE end_angle = -1;
        R = snode->startTime();
        dPnCost = 0;

        //we know we are going counter clockwise over the shocks
        //coming into the junction
        ishock_edge_list::iterator curS = snode->pShocks().begin();
        for(; curS!=snode->pShocks().end(); ++curS)
        {
          if ((*curS)->is_a_contact())
          {
            if ((*curS)->rBElement()->is_an_arc() && start_angle<0)
              start_angle = ((dbsk2d_ishock_contact*)(*curS))->n();
          
            if ((*curS)->lBElement()->is_an_arc() && !(start_angle<0))
            {
              end_angle = ((dbsk2d_ishock_contact*)(*curS))->n();

              dOC += R*CCW(start_angle, end_angle);
              dNC += R*CCW(start_angle, end_angle);
              start_angle = -1;
            }
          }
          else {
            dOC += shock_saliency_map[(*curS)->id()].dOC;
            dNC += shock_saliency_map[(*curS)->id()].dNC;
          }
          dPnCost += shock_saliency_map[(*curS)->id()].dPnCost;
        }
      }
      shock_saliency_map[selm->id()].dOC = dOC;
      shock_saliency_map[selm->id()].dNC = dNC;
      shock_saliency_map[selm->id()].dPnCost = dPnCost;

      return;
    }
    case dbsk2d_ishock_elm::CONTACTSHOCK:
    {
      shock_saliency_map[selm->id()].dOC = 0;
      shock_saliency_map[selm->id()].dNC = 0;
      shock_saliency_map[selm->id()].dPnCost = 0;

      return;
    }
    case dbsk2d_ishock_elm::POINTPOINT:
    {  
      dbsk2d_ishock_pointpoint* spp = (dbsk2d_ishock_pointpoint*)selm;

      R  = spp->endTime();
      dOC = 0; //because formed by two points

      if (R > MAX_RADIUS) {
        dNC = spp->H();
      }
      else {
        double thetaE = vnl_math::pi - 2*spp->LeTau();
        dNC = R*thetaE;
      }
      break;
    }
    case dbsk2d_ishock_elm::POINTLINE:
    {  
      dbsk2d_ishock_pointline* spl = (dbsk2d_ishock_pointline*)selm;

      R  = spl->endTime();
      
      if (spl->nu()==1) {
        dOC = spl->ReTau() - spl->RsTau();
        thetaE = CCW(spl->u()+spl->LeTau()+vnl_math::pi, spl->u());
      }
      else {
        dOC = spl->LeTau() - spl->LsTau();
        thetaE = CCW(spl->u(), spl->u()+spl->ReTau()+vnl_math::pi);
      }

      dNC = R*thetaE;
      break;
    }
    case dbsk2d_ishock_elm::LINELINE:
    {  
      dbsk2d_ishock_lineline* sll = (dbsk2d_ishock_lineline*)selm;

      R  = sll->endTime();
      dOC = 2*(sll->ReTau() - sll->RsTau());
      thetaE = CCW (sll->ul(), sll->ur());
      dNC = R*thetaE;

      break;
    }
    case dbsk2d_ishock_elm::POINTARC:
    {  
      dbsk2d_ishock_pointarc* spa = (dbsk2d_ishock_pointarc*)selm;

      R  = spa->endTime();
      thetaE=0;

      if (spa->nu()==1)
        dOC = spa->Rr()*vcl_fabs(spa->ReTau() - spa->RsTau());
      else 
        dOC = spa->Rl()*vcl_fabs(spa->LeTau() - spa->LsTau());

      if (R > MAX_RADIUS) {
        vgl_point_2d<double> start  = spa->getLFootPt(spa->LeTau());
        vgl_point_2d<double>  end  = spa->getRFootPt(spa->ReTau());
        dNC = _distPointPoint(start, end); 
      }
      else {
        if (spa->s()==1)
          thetaE = CCW(spa->u()+spa->LeTau()+vnl_math::pi, spa->u()+vnl_math::pi+spa->ReTau()+vnl_math::pi);
        else
          thetaE = CCW(spa->u()+spa->LeTau()+vnl_math::pi, spa->u()+vnl_math::pi+spa->ReTau());

        dNC = R*thetaE;
      }
      break;
    }
    case dbsk2d_ishock_elm::LINEARC:
    {  
      dbsk2d_ishock_linearc* sla = (dbsk2d_ishock_linearc*)selm;

      R  = sla->endTime();
      thetaE=0;

      if (sla->nu()==1)
        dOC = sla->R()*vcl_fabs(sla->LeTau() - sla->LsTau()) + (sla->ReTau() - sla->RsTau());
      else 
        dOC = sla->R()*vcl_fabs(sla->ReTau() - sla->RsTau()) + (sla->LeTau() - sla->LsTau());

      vgl_point_2d<double> start, end;
      if (R > MAX_RADIUS) {
        if (sla->nu()==1){ //arc on the left
          start  = sla->getLFootPt(sla->LeTau());
          //end  = getRFootPt(_LeTau);
          if (sla->nud()==1)
            end = sla->rBLine()->end();
          else
            end = sla->foot();
        }
        else {
          //start  = getLFootPt(_ReTau);
          if (sla->nud()==1)
            start = sla->lBLine()->start();
          else
            start = sla->foot();
          end  = sla->getRFootPt(sla->ReTau());
        }
        dNC = _distPointPoint(start, end); 
      }
      else {
        if (sla->nu()==1){
          if (sla->s()==1)
            thetaE = CCW(sla->u()+sla->LeTau()+vnl_math::pi, sla->u());
          else
            thetaE = CCW(sla->u()+sla->LeTau()+vnl_math::pi, sla->u()+vnl_math::pi);
        }
        else {
          if (sla->s()==1)
            thetaE = CCW(sla->u(), sla->u()+sla->ReTau()+vnl_math::pi);
          else
            thetaE = CCW(sla->u()+vnl_math::pi, sla->u()+sla->ReTau()+vnl_math::pi);
        }
        dNC = R*thetaE;
      }
      break;
    }
    case dbsk2d_ishock_elm::ARCARC:
    {  
      dbsk2d_ishock_arcarc* saa = (dbsk2d_ishock_arcarc*)selm;

      R  = saa->endTime();
      thetaE=0;

      dOC = saa->Rr()*vcl_fabs(saa->ReTau() - saa->RsTau()) + saa->Rl()*vcl_fabs(saa->LeTau() - saa->LsTau());

      if (R > MAX_RADIUS) {
        vgl_point_2d<double> start  = saa->getLFootPt(saa->LeTau());
        vgl_point_2d<double>  end  = saa->getRFootPt(saa->ReTau());
        dNC = _distPointPoint(start, end); 
      }
      else {
        if (saa->s()==1)
          thetaE = CCW(saa->u()+saa->LeTau()+vnl_math::pi, saa->u()+vnl_math::pi+saa->ReTau()+vnl_math::pi);
        else
          thetaE = CCW(saa->u()+saa->LeTau()+vnl_math::pi, saa->u()+vnl_math::pi+saa->ReTau());
        dNC = R*thetaE;
      }
      break;
    }
    case dbsk2d_ishock_elm::LINELINE_THIRDORDER:
    {
      dbsk2d_ishock_lineline_thirdorder* sto = (dbsk2d_ishock_lineline_thirdorder*)selm;

      R  = sto->endTime();
      dOC = 2*(sto->ReTau() - sto->RsTau());
      thetaE = vnl_math::pi;
      dNC = R*thetaE;

      break;
    }
    case dbsk2d_ishock_elm::ARCARC_THIRDORDER:
    {
      dNC = 0;
      dOC = ISHOCK_DIST_HUGE;
      break;
    }
  }

  //finally compute the pruning cost of shock edges
  dbsk2d_ishock_edge* sedge = (dbsk2d_ishock_edge*) selm;

  double ps_dNC = shock_saliency_map[sedge->pSNode()->id()].dNC;
  double ps_dPnCost = shock_saliency_map[sedge->pSNode()->id()].dPnCost;

  //prune cost is the difference of lengths + the parents prune cost
  dPnCost = vcl_fabs(dOC + ps_dNC - dNC) + ps_dPnCost;

  dbsk2d_assert (dOC>=0 && dNC>=0 && dPnCost>=0);

  shock_saliency_map[selm->id()].dOC = dOC;
  shock_saliency_map[selm->id()].dNC = dNC;
  shock_saliency_map[sedge->id()].dPnCost = dPnCost;
}


//: compute the salency of this shock element (edge/node)
double dbsk2d_prune_ishock::splice_cost(dbsk2d_ishock_elm* selm)
{
  double  dOC = 0;      // change in the original contour length by the removal of the current shock
  double  dNC = 0;      // length of the new contour added
  double  dPnCost = 0;  // cost of pruning this shock
                        //dPnCost = |_dOC + _dNC(parents) - _dNC| + dPnCost(parents)

  double R, thetaE;

  switch (selm->type())
  {
    case dbsk2d_ishock_elm::SNODE:
    {
      dbsk2d_ishock_node* snode = (dbsk2d_ishock_node*)selm;

      //second order nodes (don't know how to handle them yet)
      if (snode->is_a_source() || snode->is_a_sink()){
        dPnCost = ISHOCK_DIST_HUGE;
      }
      else {
        //Accumulate all the information from the parent edges.
        VECTOR_TYPE start_angle=-1;
        VECTOR_TYPE end_angle = -1;
        R = snode->startTime();
        dPnCost = 0;

        //we know we are going counter clockwise over the shocks
        //coming into the junction
        ishock_edge_list::iterator curS = snode->pShocks().begin();
        for(; curS!=snode->pShocks().end(); ++curS)
        {
          if ((*curS)->is_a_contact())
          {
            if ((*curS)->rBElement()->is_an_arc() && start_angle<0)
              start_angle = ((dbsk2d_ishock_contact*)(*curS))->n();
          
            if ((*curS)->lBElement()->is_an_arc() && !(start_angle<0))
            {
              end_angle = ((dbsk2d_ishock_contact*)(*curS))->n();

              dOC += R*CCW(start_angle, end_angle);
              dNC += R*CCW(start_angle, end_angle);
              start_angle = -1;
            }
          }
          else {
            dOC += shock_saliency_map[(*curS)->id()].dOC;
            dNC += shock_saliency_map[(*curS)->id()].dNC;
          }
          dPnCost += shock_saliency_map[(*curS)->id()].dPnCost;
        }
      }
      shock_saliency_map[selm->id()].dOC = dOC;
      shock_saliency_map[selm->id()].dNC = dNC;
      shock_saliency_map[selm->id()].dPnCost = dPnCost;

      return dPnCost;
    }
    case dbsk2d_ishock_elm::CONTACTSHOCK:
    {
      shock_saliency_map[selm->id()].dOC = 0;
      shock_saliency_map[selm->id()].dNC = 0;
      shock_saliency_map[selm->id()].dPnCost = 0;

      return 0;
    }
    case dbsk2d_ishock_elm::POINTPOINT:
    {  
      dbsk2d_ishock_pointpoint* spp = (dbsk2d_ishock_pointpoint*)selm;

      R  = spp->endTime();
      dOC = 0; //because formed by two points

      if (R > MAX_RADIUS) {
        dNC = spp->H();
      }
      else {
        double thetaE = vnl_math::pi - 2*spp->LeTau();
        dNC = R*thetaE;
      }
      break;
    }
    case dbsk2d_ishock_elm::POINTLINE:
    {  
      dbsk2d_ishock_pointline* spl = (dbsk2d_ishock_pointline*)selm;

      R  = spl->endTime();
      
      if (spl->nu()==1) {
        dOC = spl->ReTau() - spl->RsTau();
        thetaE = CCW(spl->u()+spl->LeTau()+vnl_math::pi, spl->u());
      }
      else {
        dOC = spl->LeTau() - spl->LsTau();
        thetaE = CCW(spl->u(), spl->u()+spl->ReTau()+vnl_math::pi);
      }

      dNC = R*thetaE;
      break;
    }
    case dbsk2d_ishock_elm::LINELINE:
    {  
      dbsk2d_ishock_lineline* sll = (dbsk2d_ishock_lineline*)selm;

      R  = sll->endTime();
      dOC = 2*(sll->ReTau() - sll->RsTau());
      thetaE = CCW (sll->ul(), sll->ur());
      dNC = R*thetaE;

      break;
    }
    case dbsk2d_ishock_elm::POINTARC:
    {  
      dbsk2d_ishock_pointarc* spa = (dbsk2d_ishock_pointarc*)selm;

      R  = spa->endTime();
      thetaE=0;

      if (spa->nu()==1)
        dOC = spa->Rr()*vcl_fabs(spa->ReTau() - spa->RsTau());
      else 
        dOC = spa->Rl()*vcl_fabs(spa->LeTau() - spa->LsTau());

      if (R > MAX_RADIUS) {
        vgl_point_2d<double> start  = spa->getLFootPt(spa->LeTau());
        vgl_point_2d<double>  end  = spa->getRFootPt(spa->ReTau());
        dNC = _distPointPoint(start, end); 
      }
      else {
        if (spa->s()==1)
          thetaE = CCW(spa->u()+spa->LeTau()+vnl_math::pi, spa->u()+vnl_math::pi+spa->ReTau()+vnl_math::pi);
        else
          thetaE = CCW(spa->u()+spa->LeTau()+vnl_math::pi, spa->u()+vnl_math::pi+spa->ReTau());

        dNC = R*thetaE;
      }
      break;
    }
    case dbsk2d_ishock_elm::LINEARC:
    {  
      dbsk2d_ishock_linearc* sla = (dbsk2d_ishock_linearc*)selm;

      R  = sla->endTime();
      thetaE=0;

      if (sla->nu()==1)
        dOC = sla->R()*vcl_fabs(sla->LeTau() - sla->LsTau()) + (sla->ReTau() - sla->RsTau());
      else 
        dOC = sla->R()*vcl_fabs(sla->ReTau() - sla->RsTau()) + (sla->LeTau() - sla->LsTau());

      vgl_point_2d<double> start, end;
      if (R > MAX_RADIUS) {
        if (sla->nu()==1){ //arc on the left
          start  = sla->getLFootPt(sla->LeTau());
          //end  = getRFootPt(_LeTau);
          if (sla->nud()==1)
            end = sla->rBLine()->end();
          else
            end = sla->foot();
        }
        else {
          //start  = getLFootPt(_ReTau);
          if (sla->nud()==1)
            start = sla->lBLine()->start();
          else
            start = sla->foot();
          end  = sla->getRFootPt(sla->ReTau());
        }
        dNC = _distPointPoint(start, end); 
      }
      else {
        if (sla->nu()==1){
          if (sla->s()==1)
            thetaE = CCW(sla->u()+sla->LeTau()+vnl_math::pi, sla->u());
          else
            thetaE = CCW(sla->u()+sla->LeTau()+vnl_math::pi, sla->u()+vnl_math::pi);
        }
        else {
          if (sla->s()==1)
            thetaE = CCW(sla->u(), sla->u()+sla->ReTau()+vnl_math::pi);
          else
            thetaE = CCW(sla->u()+vnl_math::pi, sla->u()+sla->ReTau()+vnl_math::pi);
        }
        dNC = R*thetaE;
      }
      break;
    }
    case dbsk2d_ishock_elm::ARCARC:
    {  
      dbsk2d_ishock_arcarc* saa = (dbsk2d_ishock_arcarc*)selm;

      R  = saa->endTime();
      thetaE=0;

      dOC = saa->Rr()*vcl_fabs(saa->ReTau() - saa->RsTau()) + saa->Rl()*vcl_fabs(saa->LeTau() - saa->LsTau());

      if (R > MAX_RADIUS) {
        vgl_point_2d<double> start  = saa->getLFootPt(saa->LeTau());
        vgl_point_2d<double>  end  = saa->getRFootPt(saa->ReTau());
        dNC = _distPointPoint(start, end); 
      }
      else {
        if (saa->s()==1)
          thetaE = CCW(saa->u()+saa->LeTau()+vnl_math::pi, saa->u()+vnl_math::pi+saa->ReTau()+vnl_math::pi);
        else
          thetaE = CCW(saa->u()+saa->LeTau()+vnl_math::pi, saa->u()+vnl_math::pi+saa->ReTau());
        dNC = R*thetaE;
      }
      break;
    }
    case dbsk2d_ishock_elm::LINELINE_THIRDORDER:
    {
      dbsk2d_ishock_lineline_thirdorder* sto = (dbsk2d_ishock_lineline_thirdorder*)selm;

      R  = sto->endTime();
      dOC = 2*(sto->ReTau() - sto->RsTau());
      thetaE = vnl_math::pi;
      dNC = R*thetaE;

      break;
    }
    case dbsk2d_ishock_elm::ARCARC_THIRDORDER:
    {
      dNC = 0;
      dOC = ISHOCK_DIST_HUGE;
      break;
    }
  }

  //finally compute the pruning cost of shock edges
  dbsk2d_ishock_edge* sedge = (dbsk2d_ishock_edge*) selm;

  double ps_dNC = shock_saliency_map[sedge->pSNode()->id()].dNC;
  double ps_dPnCost = shock_saliency_map[sedge->pSNode()->id()].dPnCost;

  //prune cost is the difference of lengths + the parents prune cost
  dPnCost = vcl_fabs(dOC + ps_dNC - dNC) + ps_dPnCost;

  dbsk2d_assert (dOC>=0 && dNC>=0 && dPnCost>=0);

  return dPnCost;
}

//: prune the shock graph
void dbsk2d_prune_ishock::prune(double thresh)
{
  //1) make an ordered shock list
  
  //go through the edge_list and insert it into the list
  dbsk2d_ishock_graph::edge_iterator curE = ishock_graph->all_edges().begin();
  for (; curE != ishock_graph->all_edges().end(); curE++){
    dbsk2d_ishock_edge* curShock = (*curE);
    ordered_shock_list.insert(
      vcl_pair<vcl_pair<double, int>, dbsk2d_ishock_elm*>(
        vcl_pair<double, int>(curShock->endTime(),curShock->id()), 
        curShock)
    );
  }

  //go through the vertex_list and insert it into the list
  dbsk2d_ishock_graph::vertex_iterator curN = ishock_graph->all_nodes().begin();
  for (; curN != ishock_graph->all_nodes().end(); curN++){
    dbsk2d_ishock_node* curShock = (*curN);
    ordered_shock_list.insert(
      vcl_pair<vcl_pair<double, int>, dbsk2d_ishock_elm*>(
        vcl_pair<double, int>(curShock->endTime(),curShock->id()), 
        curShock)
    );
  }

  //1) compute saliences
  vcl_multimap<vcl_pair<double, int>, dbsk2d_ishock_elm*>::iterator curS = ordered_shock_list.begin();
  for (; curS != ordered_shock_list.end(); ++curS)
  {
    dbsk2d_ishock_elm* selm = curS->second;
    compute_shock_saliency(selm);
  }

  //2) prune all shocks below threshold

  curS = ordered_shock_list.begin();
  for (; curS != ordered_shock_list.end(); ++curS)
  {
    dbsk2d_ishock_elm* current = curS->second;

    //special case: unhide all the shocks if the threshold is 0.0
    if (thresh==0.0){
      current->unhide();
      continue;
    }

    if (current->is_a_node())
    {
      dbsk2d_ishock_node* cur_node = (dbsk2d_ishock_node*)current;
      
      //hide it if its child is below threshold
      if (cur_node->is_an_A3source()){
        if (shock_saliency_map[cur_node->cShock()->id()].dPnCost < thresh) cur_node->hide();
        else cur_node->unhide();
        continue;
      }

      if (cur_node->is_a_junct()){
        if (shock_saliency_map[cur_node->cShock()->id()].dPnCost < thresh) cur_node->hide();
        else cur_node->unhide();
        continue;
      }

      if (cur_node->is_a_source()){
        if (shock_saliency_map[cur_node->cShock()->id()].dPnCost < thresh ||
            shock_saliency_map[cur_node->cShock2()->id()].dPnCost < thresh) 
        {
          //hide source
          cur_node->hide();

          //hide both its children
          cur_node->cShock()->hide();
          cur_node->cShock2()->hide();
        }
        else {
          cur_node->unhide();
          //hide both its children
          cur_node->cShock()->unhide();
          cur_node->cShock2()->unhide();
        }
        continue;
      }

      //hide it if all its parents are gone
      if (cur_node->is_a_sink()){
        bool remove = true;
        ishock_edge_list::iterator curS = cur_node->pShocks().begin();
        for(; curS!=cur_node->pShocks().end(); ++curS){
          if (shock_saliency_map[(*curS)->id()].dPnCost >= thresh){
            remove = false;
            break;
          }
        }

        if (remove) cur_node->hide();
        else cur_node->unhide();
        continue;
      }
    }
    
    //the rest must be links
    dbsk2d_ishock_edge* curLink = (dbsk2d_ishock_edge*) current;

    // ignore contacts
    if (curLink->is_a_contact()){
      //hide all contacts
      curLink->hide();
      continue;
    }

    if (!curLink->pSNode()->is_a_source()){
      if (shock_saliency_map[curLink->id()].dPnCost < thresh) curLink->hide();
      else curLink->unhide();
    }
  }
}

inline
bool has_support(vgl_point_2d<double>& pt, vil_image_view<bool>& rbs_mask, int& off_x, int& off_y) {
 int x = (int)vcl_floor(pt.x()+0.5)-off_x;
 int y = (int)vcl_floor(pt.y()+0.5)-off_y;
 if (rbs_mask(x, y))
  return true;
 else
  return false; 
}

//: prune all the intrinsic shock edges which has samples not supported by the given set of boundaries
//  pixel_range_in_mask_image: the neighborhood that is masked true in the image around each boundary pixel, e.g. 2 pixels
void dbsk2d_prune_ishock::prune_based_on_support(vcl_vector<vsol_polyline_2d_sptr>& rbs, vsol_box_2d_sptr bbox, int pixel_range_in_mask_image)
{
  //: prepare the mask image for real boundaries for fast access
  int ni = int(bbox->width()+4*pixel_range_in_mask_image);
  int nj = int(bbox->height()+4*pixel_range_in_mask_image);
  vil_image_view<bool> rbs_mask(ni, nj, 1); 
  int off_x = int(vcl_floor(bbox->get_min_x()-2*pixel_range_in_mask_image+0.5));
  int off_y = int(vcl_floor(bbox->get_min_y()-2*pixel_range_in_mask_image+0.5));
  rbs_mask.fill(false);

  for (unsigned i = 0; i < rbs.size(); i++) 
    for (unsigned j = 0; j < rbs[i]->size(); j++) {
      int x = (int)vcl_floor(rbs[i]->vertex(j)->x()+0.5)-off_x;
      int y = (int)vcl_floor(rbs[i]->vertex(j)->y()+0.5)-off_y;
      rbs_mask(x, y) = true;
      for (int k = 1; k < pixel_range_in_mask_image; k++) {
        rbs_mask(x+k, y) = true;
        rbs_mask(x-k, y) = true;
        rbs_mask(x, y+k) = true;
        rbs_mask(x, y-k) = true;
        rbs_mask(x+k, y+k) = true;
        rbs_mask(x-k, y+k) = true;
        rbs_mask(x+k, y-k) = true;
        rbs_mask(x-k, y-k) = true;
      }
    }

#if 0
   vil_image_view<unsigned char> temp1(ni, nj, 1);
  for (unsigned i=0;i<rbs_mask.ni();i++)
      for(unsigned j=0;j<rbs_mask.nj();j++)
          {
          if(rbs_mask(i,j))
              temp1(i,j)=255;

          else 
              temp1(i,j)=0;
          }
  vcl_string name("d:\\projects\\temp\\temp-mask.tiff");
   vil_save(temp1,name.c_str());
#endif

  //go through the edge_list and hide the ones if their boundary elements are not in rbs
  dbsk2d_ishock_graph::edge_iterator curE = ishock_graph->all_edges().begin();
  for (; curE != ishock_graph->all_edges().end(); curE++){
    dbsk2d_ishock_edge* curShock = (*curE);
    if (curShock->isHidden())
      continue;

    dbsk2d_ishock_belm* b = curShock->lBElement();
    if (b->is_a_point()) {
      vgl_point_2d<double> p = ((dbsk2d_ishock_bpoint*)b)->pt();
      if (!has_support(p , rbs_mask, off_x, off_y ))
        curShock->hide();
    } else if (b->is_a_curve()) {
      dbsk2d_ishock_bcurve* bl = (dbsk2d_ishock_bcurve*)b;
      vgl_point_2d<double> ps = bl->s_pt()->pt();
      vgl_point_2d<double> pe = bl->e_pt()->pt();
      if (!has_support(ps, rbs_mask, off_x, off_y) || !has_support(pe, rbs_mask, off_x, off_y) )
        curShock->hide();
    }
    b = curShock->rBElement();
    if (b->is_a_point()) {
      vgl_point_2d<double> p = ((dbsk2d_ishock_bpoint*)b)->pt();
      if (!has_support(p, rbs_mask, off_x, off_y ))
        curShock->hide();
    } else if (b->is_a_curve()) {
      dbsk2d_ishock_bcurve* bl = (dbsk2d_ishock_bcurve*)b;
      vgl_point_2d<double> ps = bl->s_pt()->pt();
      vgl_point_2d<double> pe = bl->e_pt()->pt();
      if (!has_support(ps, rbs_mask, off_x, off_y ) || !has_support(pe, rbs_mask, off_x, off_y ) )
        curShock->hide();
    }
  }
}

//: compile the coarse shock graph from the pruned shock graph
void dbsk2d_prune_ishock::compile_coarse_shock_graph()
{
  //trace through the remaining graph to define the coarse graph
  compile_nodes();
  compile_edges();

  update_intrinsic_information_at_the_nodes();

  //remove the vertices that are not connected to any edges 
  //because the edges go to infinity
  shock_graph->purge_isolated_vertices();
}

//: compile all the valid leftover nodes into the coarse graph nodes
void dbsk2d_prune_ishock::compile_nodes()
{
  //first make a list of all valid nodes
  dbsk2d_ishock_graph::vertex_iterator curN = ishock_graph->all_nodes().begin();
  for (; curN != ishock_graph->all_nodes().end(); curN++)
  {
    dbsk2d_ishock_node* cur_node = (*curN);

    //In order to accommodate for the system where pruning without symmetry tranforms 
    //is allowed and also for future versions where shock nodes are not subclassed
    //but differ only by their connectivity, we shall use the connectivity information
    //to decide on the node properties

    if ( (cur_node->indeg(true)==0 && cur_node->outdeg(true)>0 ) ||       //A3 or source
         (cur_node->indeg(true)>1  && cur_node->outdeg(true)==1) ||       //junction 
         (cur_node->indeg(true)>=1  && cur_node->outdeg(true)==0) )       //sink (including degenerate)
    {  
      dbsk2d_shock_node_sptr new_shock_node = new dbsk2d_shock_ishock_node(cur_node);
      //use dbsk2d_ishock_node->id() instead of new_node_id() for debug
      new_shock_node->set_id(cur_node->id());
      shock_graph->add_vertex(new_shock_node->cast_to_shock_node());
      ishock_to_shock_node_map.insert(
        vcl_pair<int, dbsk2d_shock_node_sptr>(cur_node->id(), new_shock_node)
      );
    }

  }
}

//: now trace from these nodes to linked nodes to form edges
void dbsk2d_prune_ishock::compile_edges()
{
  // go through all the nodes compiled in the last step 
  // and trace to the opposite node
  dbsk2d_shock_graph::vertex_iterator curN = shock_graph->vertices_begin();
  for (; curN != shock_graph->vertices_end(); curN++)
  {
    compile_edges_of_node(*curN);
  } //end of for loop
}

void dbsk2d_prune_ishock::compile_edges_of_node(dbsk2d_shock_node_sptr node) 
{
  dbsk2d_shock_node_sptr end_node;
  dbsk2d_shock_edge_sptr new_edge;
  dbsk2d_ishock_edge* first_ishock_edge;
  vcl_list<dbsk2d_ishock_edge*> shock_edges;

  dbsk2d_shock_ishock_node* cur_node = (dbsk2d_shock_ishock_node*)node.ptr();
  dbsk2d_ishock_node* cur_inode = cur_node->ishock_node();  
  
  if (cur_inode->id() == 49542 || cur_inode->id() == 44300 || cur_inode->id() == 49724 )
    vcl_cout << "here!!\n";

   // ozge added the following line, is this a problem?? TODO: ask Amir
   if (cur_inode->isHidden())
    return;

  if ( (cur_inode->indeg(true)==0 && cur_inode->outdeg(true)>0 ) ||       //A3 or source
       (cur_inode->indeg(true)>1  && cur_inode->outdeg(true)==1))         //junction
  {                                                               //sinks have no children
    //initialize child edge
    shock_edges.clear();
    first_ishock_edge = cur_inode->cShock();

    // ozge added the following line, is this a problem?? TODO: ask Amir
    if (first_ishock_edge->isHidden())
      if (cur_inode->cShock2() && !(cur_inode->cShock2()->isHidden()) )
        first_ishock_edge = cur_inode->cShock2();
      else
        return;

    //ozge: added this check not to recreate an existing edge
    //      this check was necessary when this function is used in gap transform
    //      normally during pruning it is not necessary
    vcl_map <int, dbsk2d_shock_edge_sptr>::iterator iter = ishock_to_shock_edge_map.find(first_ishock_edge->id());
    if (iter == ishock_to_shock_edge_map.end()) {
    
      //end_node = trace_to_target_node(cur_inode->cShock(), shock_edges);
      //: ozge changed to 
      end_node = trace_to_target_node(first_ishock_edge, shock_edges);
    
      new_edge = new dbsk2d_shock_grouped_ishock_edge(cur_node, end_node, shock_edges);
      shock_graph->add_edge(new_edge);
      new_edge->set_id(first_ishock_edge->id());

      //Amir: Postponed this step until all edges have been instantiated
      //      This can be done while updating the intrinsic information at the nodes
      //
      //cur_node->add_outgoing_edge(new_edge);
      //if (end_node)
      //  end_node->add_incoming_edge(new_edge);

      //add it to the map
      ishock_to_shock_edge_map.insert(
        vcl_pair<int, dbsk2d_shock_edge_sptr>(new_edge->id(), new_edge)
      );
    }
  }

  //initialize a second child edge for a source
  if (cur_inode->indeg(true)==0 && cur_inode->outdeg(true)==2)
  {
    //second child
    shock_edges.clear();
    first_ishock_edge = cur_inode->cShock2();

    //ozge: added this check not to recreate an existing edge
    //      this check was necessary when this function is used in gap transform
    //      normally during pruning it is not necessary
    vcl_map <int, dbsk2d_shock_edge_sptr>::iterator iter = ishock_to_shock_edge_map.find(first_ishock_edge->id());
    if (iter == ishock_to_shock_edge_map.end()) {

      end_node = trace_to_target_node(cur_inode->cShock2(), shock_edges);
    
      new_edge = new dbsk2d_shock_grouped_ishock_edge(cur_node, end_node, shock_edges);
      shock_graph->add_edge(new_edge);
      new_edge->set_id(first_ishock_edge->id());
      //Amir: Postponed this step until all edges have been instantiated
      //      This can be done while updating the intrinsic information at the nodes
      //
      //cur_node->add_outgoing_edge(new_edge);
      //if (end_node)
      //  end_node->add_incoming_edge(new_edge);

      //add it to the map
      ishock_to_shock_edge_map.insert(
        vcl_pair<int, dbsk2d_shock_edge_sptr>(new_edge->id(), new_edge)
      );
    }
  }

}

//: trace from a given ishock edge to the next coarse level node while
// compiling a list of edges that it traversed through
dbsk2d_shock_node_sptr 
dbsk2d_prune_ishock::trace_to_target_node(dbsk2d_ishock_edge* shock_edge, 
                                          vcl_list<dbsk2d_ishock_edge*>& shock_edges)
{
  int edge_id = shock_edge->id();

  dbsk2d_ishock_edge* cur_edge = shock_edge;
  dbsk2d_ishock_edge* last_edge = 0;
  dbsk2d_ishock_node* target_node = 0;

  while (cur_edge){
    //set the first edge id as the edge id
    cur_edge->setEdgeID(edge_id);

    //add the current shock edge to list
    shock_edges.push_back(cur_edge);
    last_edge = cur_edge;

    //get the next edge if the child node is a pruned junction
    cur_edge = cur_edge->get_next_edge();
    
    //check to see if the child node of the cur_edge 
    //is a degenerate pruned junction
    if (cur_edge && 
        last_edge->cSNode()->indeg(false) != last_edge->cSNode()->indeg(true))
    {
      //experimenting with just leaving a blank marker
      shock_edges.push_back(0); //signals a A1-Ainf edge
    }
  }
  target_node= last_edge->cSNode();

  if (!target_node)   //some edges go to infinity
    return 0;

  //now find dbsk2d_shock_node corresponding to this dbsk2d_ishock_node
  vcl_map<int, dbsk2d_shock_node_sptr>::iterator nmap_iter = 
    ishock_to_shock_node_map.find(target_node->id());

  if (nmap_iter != ishock_to_shock_node_map.end())
    return nmap_iter->second;
  else {
    dbsk2d_assert(0); //can't find this node, something is wrong
    return 0;
  }
}

//: add descriptors to all the shock nodes corresponding to the incident
// edges and degeneracies (also form the visual fragments for the degenerate
// descriptors (A-inf) at this time
void dbsk2d_prune_ishock::update_intrinsic_information_at_the_nodes()
{
  //go over all the nodes in the graph and add descritpors 
  dbsk2d_shock_graph::vertex_iterator curN = shock_graph->vertices_begin();
  for (; curN != shock_graph->vertices_end(); curN++)
  {
    dbsk2d_shock_node_sptr cur_node = (*curN);
    update_intrinsic_information_at_the_node(cur_node);
  }   
}

//: adding the intrinsic information at the given node
void dbsk2d_prune_ishock::update_intrinsic_information_at_the_node(dbsk2d_shock_node_sptr cur_node) {
  
  dbsk2d_ishock_node* cur_inode = ((dbsk2d_shock_ishock_node*)cur_node.ptr())->ishock_node();

  //traverse through the edges adjacent to intrinsic node 
  //and add descriptors corresponding to the edges that survived.
  //Also add A-infinity descriptors for degenerate nodes, where they
  //were two or more consecutive contacts or edges that were pruned

  //first the incoming edges list
  bool A_infinity = false;
  double tan_start_vec=0;
  double tan_end_vec=0;

  for (ishock_edge_list::iterator curIE = cur_inode->pShocks().begin(); 
       curIE != cur_inode->pShocks().end(); ++curIE)
  {
    dbsk2d_ishock_edge* cur_iedge = (*curIE);

    //if an unpruned shock is hit, we know we are at the end of the pruned group
    if (!cur_iedge->isHidden() && A_infinity)
    {   
      //create the A-infinity descriptor
      dbsk2d_shock_node_descriptor new_param(NULL, tan_start_vec, 0, tan_end_vec);
      //insert it into the ordered list
      cur_node->descriptor_list().push_back(new_param);

      //reset the flag
      A_infinity = false;
    }

    if (!cur_iedge->isHidden())  //unpruned edge
    {
      //find dbsk2d_shock_edge corresponding to this dbsk2d_ishock_edge
      dbsk2d_shock_edge_sptr cur_edge = ishock_to_shock_edge_map.find(cur_iedge->edgeID())->second;

      // Add this edge to the incoming edge list of this node
      cur_node->add_incoming_edge(cur_edge);

      double tan_vec = angle0To2Pi(cur_iedge->tangent(cur_iedge->eTau()) + vnl_math::pi); //invert for incoming
      double phi = angle0To2Pi(vnl_math::pi - cur_iedge->phi(cur_iedge->eTau())); //invert for incoming
      //create new intrinsic parameter for this edge
      dbsk2d_shock_node_descriptor new_param(cur_edge.ptr(), tan_vec, phi);
      //insert it into the ordered list
      cur_node->descriptor_list().push_back(new_param);
    }
    else {
      //The goal is to collect all the pruned edges to make an A-infinity descriptor
      //
      //If the A_infinity flag is not set, we know that this is the first pruned shock
      //so record the tan_start_vec from this shock
      if (!A_infinity)
        tan_start_vec = angle0To2Pi(cur_iedge->tangent(cur_iedge->eTau()) + 
                                    cur_iedge->phi(cur_iedge->eTau()));

      //update the tan_end_vec for subsequent pruned edges
      tan_end_vec = angle0To2Pi(cur_iedge->tangent(cur_iedge->eTau()) - 
                                  cur_iedge->phi(cur_iedge->eTau()));
      A_infinity = true;
    }
  }

  //if the A_infinity flag is still up then we know that 
  //all of the incoming shocks were pruned
  if (A_infinity){ 
    //create the A-infinity descriptor
    dbsk2d_shock_node_descriptor new_param(NULL, tan_start_vec, 0, tan_end_vec);
    //insert it into the ordered list
    cur_node->descriptor_list().push_back(new_param);

    //reset the flag
    A_infinity = false;
  }

  //then the outgoing edges
  //NOTE: assuming that outgoing edges cannot be pruned

  // ozge's note on March 05, 2007. After gap transforms the outgoing edges CAN BE pruned (hidden)

  //if the first child shock exists
  if (cur_inode->cShock() && !(cur_inode->cShock()->isHidden()) ) { // ozge added the second condition
    dbsk2d_ishock_edge* cur_iedge = cur_inode->cShock();

    //find dbsk2d_shock_edge corresponding to this dbsk2d_ishock_edge
    dbsk2d_shock_edge_sptr cur_edge = ishock_to_shock_edge_map.find(cur_iedge->edgeID())->second;
    
    // Add this edge to the outgoing edge list of this node
    cur_node->add_outgoing_edge(cur_edge);

    double tan_vec = cur_iedge->tangent(cur_iedge->sTau());
    double phi = cur_iedge->phi(cur_iedge->sTau());
    //create new intrinsic parameter for this edge
    dbsk2d_shock_node_descriptor new_param(cur_edge.ptr(), tan_vec, phi);
    //insert it into the ordered list
    cur_node->descriptor_list().push_back(new_param);
  }

  //then the second child if it exists
  if (cur_inode->cShock2() && !(cur_inode->cShock2()->isHidden()) ) {  // ozge added the second condition
    dbsk2d_ishock_edge* cur_iedge = cur_inode->cShock2();

    //find dbsk2d_shock_edge corresponding to this dbsk2d_ishock_edge
    dbsk2d_shock_edge_sptr cur_edge = ishock_to_shock_edge_map.find(cur_iedge->edgeID())->second;
    
    // Add this edge to the outgoing edge list of this node
    cur_node->add_outgoing_edge(cur_edge);

    double tan_vec = cur_iedge->tangent(cur_iedge->sTau());
    double phi = cur_iedge->phi(cur_iedge->sTau());
    //create new intrinsic parameter for this edge
    dbsk2d_shock_node_descriptor new_param(cur_edge.ptr(), tan_vec, phi);
    //insert it into the ordered list
    cur_node->descriptor_list().push_back(new_param);
  }

  //now create the visual fragments for the degenerate descriptors on this node
  cur_node->form_shock_fragments();
}
