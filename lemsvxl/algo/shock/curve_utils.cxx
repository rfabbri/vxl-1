// This is curve_utils.cxx

//:
// \file

#include "curve_utils.h"
#include "ishock.h"
#include "ishock_sptr.h"
#include "boundary-bucketing.h"

#include <vcl_map.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polyline_2d.h>

potential_curve_completions_list 
compute_potential_curve_completions(vcl_vector<vcl_vector<vgl_point_2d<double> > > curve_set)
{
    
  //form a boundary out of the curves passed to this function
  Boundary_Bucketing* bound = new Boundary_Bucketing();

  //also keep a map of the endpoints back to the curves
  vcl_map<BPoint*, vcl_pair<bool, int> > ept_to_curve_map;

  for (unsigned int b = 0 ; b < curve_set.size() ; b++ ) 
  {
    vcl_vector<vgl_point_2d<double> > this_curve = curve_set[b];

    BPoint* first_bpoint = bound->addNonGUIPoint(this_curve.front().x(), this_curve.front().y());

    //add starting point to the endpoint map
    ept_to_curve_map.insert(vcl_pair<BPoint*, vcl_pair<bool, int> >(first_bpoint, vcl_pair<bool, int>(true, b)));

    BPoint* last_bpoint = bound->addNonGUIPoint(this_curve.back().x(), this_curve.back().y());

    //add starting point to the endpoint map
    ept_to_curve_map.insert(vcl_pair<BPoint*, vcl_pair<bool, int> >(last_bpoint, vcl_pair<bool, int>(false, b)));

    // compile the rest of the points into a vector of Points 
    vcl_vector<Point> pts;
    for (unsigned int i=1; i<this_curve.size()-1;i++)
    {
      pts.push_back(Point(this_curve[i].x(), this_curve[i].y()));
    }
    //and add them as a polyline
    bound->addPolyLineBetween(first_bpoint, last_bpoint, pts, b);
  }

  //now compute shocks from this boundary
  IShock_sptr shock = new IShock_Bucketing(bound);
  shock->DetectShocks();

  // Now traverse the shock list to find potential completion pairs
  potential_curve_completions_list potential_list;
  potential_list.clear();

  SIElmListIterator sh_it = shock->SIElmList.begin();
  for (; sh_it != shock->SIElmList.end(); sh_it++)
  {
    SIElement* this_shock = sh_it->second;

    //only look at point-point shocks
    if (this_shock->type()==SIElement::POINTPOINT)
    {
      SIPointPoint* pp_shock = (SIPointPoint*)this_shock;

      //this shock signals potential completion if it comes from endpoints
      if (pp_shock->lBPoint()->isEndPoint() && pp_shock->rBPoint()->isEndPoint())
      {
        //add as a potential completion pair
        vcl_pair<bool, int> c1 = ept_to_curve_map.find(pp_shock->lBPoint())->second;
        vcl_pair<bool, int> c2 = ept_to_curve_map.find(pp_shock->rBPoint())->second;

        potential_list.push_back(curve_pair(c1, c2));
      }
    }
  }

  //return the potential_curve_completions_list
  return potential_list;
}

potential_junctions_list 
compute_potential_junctions(vcl_vector<vcl_vector<vgl_point_2d<double> > > curve_set)
{
  //form a boundary out of the curves passed to this function
  Boundary_Bucketing* bound = new Boundary_Bucketing();

  //also keep a map of the endpoints back to the curves
  vcl_map<BPoint*, vcl_pair<bool, int> > ept_to_curve_map;

  for (unsigned int b = 0 ; b < curve_set.size() ; b++ ) 
  {
    vcl_vector<vgl_point_2d<double> > this_curve = curve_set[b];

    BPoint* first_bpoint = bound->addNonGUIPoint(this_curve.front().x(), this_curve.front().y());

    //add starting point to the endpoint map
    ept_to_curve_map.insert(vcl_pair<BPoint*, vcl_pair<bool, int> >(first_bpoint, vcl_pair<bool, int>(true, b)));

    BPoint* last_bpoint = bound->addNonGUIPoint(this_curve.back().x(), this_curve.back().y());

    //add starting point to the endpoint map
    ept_to_curve_map.insert(vcl_pair<BPoint*, vcl_pair<bool, int> >(last_bpoint, vcl_pair<bool, int>(false, b)));

    // compile the rest of the points into a vector of Points 
    vcl_vector<Point> pts;
    for (unsigned int i=1; i<this_curve.size()-1;i++)
    {
      pts.push_back(Point(this_curve[i].x(), this_curve[i].y()));
    }
    //and add them as a polyline
    bound->addPolyLineBetween(first_bpoint, last_bpoint, pts, b);

  }

  //now compute shocks from this boundary
  IShock_sptr shock = new IShock_Bucketing(bound);
  shock->DetectShocks();

  // Now traverse the shock list to find potential completion pairs
  potential_junctions_list potential_list;
  potential_list.clear();

  SIElmListIterator sh_it = shock->SIElmList.begin();
  for (; sh_it != shock->SIElmList.end(); sh_it++)
  {
    SIElement* this_shock = sh_it->second;

    //only look at second order shocks only
    if (this_shock->type()==SIElement::SOURCE)
    {
      SISource* so_shock = (SISource*)this_shock;

      //this shock signals potential junction if it comes from only one endpoint
      BElement* b1 = so_shock->getBElement1();
      BElement* b2 = so_shock->getBElement2();

      if (b1->type()==BPOINT && b2->type()==BPOINT)
      {
        BPoint* bp1 = (BPoint*) b1;
        BPoint* bp2 = (BPoint*) b2;

        if (bp1->isEndPoint() && !bp2->isEndPoint())
        {
          //qualifies
          int curve_id = bp2->LinkedBElmList.front()->edge_id();

          //add as a potential junction pair
          vcl_pair<bool, int> c1 = ept_to_curve_map.find(bp1)->second;
          potential_list.push_back(endpt_curve_pair(c1, curve_id));
        }
        if (!bp1->isEndPoint() && bp2->isEndPoint())
        {
          //qualifies
          int curve_id = bp1->LinkedBElmList.front()->edge_id();

          //add as a potential junction pair
          vcl_pair<bool, int> c1 = ept_to_curve_map.find(bp2)->second;
          potential_list.push_back(endpt_curve_pair(c1, curve_id));
        }
      }
      else if (b1->type()==BPOINT)
      {
        BPoint* bp1 = (BPoint*) b1;
        
        if (bp1->isEndPoint())
        {
          //qualifies
          int curve_id = b2->edge_id();

          //add as a potential junction pair
          vcl_pair<bool, int> c1 = ept_to_curve_map.find(bp1)->second;
          potential_list.push_back(endpt_curve_pair(c1, curve_id));
        }
      }
      else if (b2->type()==BPOINT)
      {
        BPoint* bp2 = (BPoint*) b2;
        
        if (bp2->isEndPoint())
        {
          //qualifies
          int curve_id = b1->edge_id();

          //add as a potential junction pair
          vcl_pair<bool, int> c1 = ept_to_curve_map.find(bp2)->second;
          potential_list.push_back(endpt_curve_pair(c1, curve_id));
        }
      }
    }
  }

  //return the potential_curve_completions_list
  return potential_list;
}
