/* ******************************************************
  File: /algo/shock/ishock_boundary_linking.cxx
  Author: Nhon Trinh
  Date: Jul 21, 2004
<<<<<<< ishock_boundary_linking.cxx
  Last modified Aug 7, 2004
  Status: working for line and point belm's.
          need to take care of some special cases
=======
  Last modified Jul 29, 2004
  Status: working for line and point belm's.
          need to take care of some special cases
>>>>>>> 1.2

  This file contain functions to connect the ishock structure
  and the boundary that it is created from. 

********************************************************/

#include "ishock.h"

// Name: vcl_vector < SINode_BCoor_Pair > IShock::find_BCoor_of_cvt_xtrema_nodes()
// Return: a list of SINode_BCoor_Pair which contains all the A3 nodes and 
// its projection on the boundary (coresponding belements, tangent, distance).
//
// By Nhon Trinh
// Date: Jul 26 2004
// Last modified : Aug 7 2004
// status: working, tested with polygon boundary
vcl_list < A3node_bnd_link* > 

IShock::find_BCoor_of_cvt_xtrema_nodes()
{
  // The algorithm go through all SINodes and check if they corespond to a 
  // curvature extrema points on the boundary
  // If yes, project the SINode to the boundary.

  vcl_list < A3node_bnd_link* > A3node_bnd_link_list;
  A3node_bnd_link_list.clear();


  for ( SIElmListIterator sielm_it = SIElmList.begin(); sielm_it != SIElmList.end(); sielm_it++)
  {
    SINode * cur_node;
    SILink * cur_link = 0;
    if (!(sielm_it->second->isANode())) continue;
    cur_node = (SINode *) sielm_it->second;
      
    // only work with Nodes at curvature extrema
    if (!cur_node->is_curvature_xtrema_node()){
      continue;
    }
  
    if (cur_node->neighbor_links().size() != 1){
      vcl_cout << "ERROR in find_BCoor_of_cvt_xtrema_nodes()" << vcl_endl;
      vcl_cout << "A3 node id = " << cur_node->id() << " size(neighbor_links) != 1" << vcl_endl;
      continue;
    }
    cur_link = cur_node->neighbor_links().front();

    // right now cur_node is an A3Source node and 
    // cur_link is the only notHidden link connected to it

    Point node_origin = cur_node->origin();
    
    // create a A3node_bnd_link record to save to the list A3node_bnd_link_list
    A3node_bnd_link* nblink = new A3node_bnd_link;
    
    // A3 node
    nblink->A3_node = cur_node;

    // belm_list. find the belements that form this A3Source
    SINode* node_temp = opposite(cur_node, cur_link);
    nblink->belm_list = CCWTraceContourBetween(node_temp, cur_link, cur_node, cur_link);
    if ((nblink->belm_list).size() < 2){
      vcl_cout << "ERROR: bcoor.belm_list.size() < 2 " << vcl_endl;
      continue;
    }

    // bcoors. find the boundary element that contains the projection (right, center, left) of cur_node onto boundary
    // Strategy:
    // As we traverse CCW from along the belm_list, the angles drawing from the node_orgin
    // of those BElements increase. We will use them to determine which belement contains the projection
    // We chose tangent of cur_link as the mid proj axis and compute the angle phi between side axis and the mid-axis
    // To avoid the transition at 0=2Pi, we define the start_angle of the most CW belm as 0 
    // and compute others accordingly.

    double angle_offset = _vPointPoint(node_origin, ((nblink->belm_list).front())->start());
    
    // the projection axes from the A3 node
    if (cur_node == source(cur_link)){
      nblink->node_tangent = angle0To2Pi(cur_link->tangent(cur_link->sTau()) + M_PI);
      double v = cur_link->v(cur_link->sTau());
      
      double rp;
      if (v >= 100000) rp = 0;
      else rp = 1/v;
      
      if (vcl_abs(rp) <= 1)
        nblink->arc_angle = 2*(-vcl_acos(-rp) + M_PI);
      else{
        vcl_cout << "abs(rp) > 1";
        continue;
      }
    }
    else{
      nblink->node_tangent = angle0To2Pi(cur_link->tangent(cur_link->eTau()));
      double v = cur_link->v(cur_link->eTau());
      double rp;
      if (v >= 100000) rp = 0;
      else rp = 1/v;
      
      // double rp = cur_link->rp(cur_link->eTau());
      if (vcl_abs(rp) <= 1)
        nblink->arc_angle = 2*vcl_acos(-rp);
      else{
        vcl_cout << "abs(rp) > 1";
        continue;
      }
    }
    
    // the tangent angles along the boundary
    (nblink->bcoors[CENTER_BELEMENT]).btangent = angle0To2Pi(nblink->node_tangent + M_PI_2);
    (nblink->bcoors[RIGHT_BELEMENT]).btangent = (nblink->bcoors[CENTER_BELEMENT]).btangent - nblink->arc_angle/2;
    (nblink->bcoors[LEFT_BELEMENT]).btangent = (nblink->bcoors[CENTER_BELEMENT]).btangent + nblink->arc_angle/2;
    
    // angles of the axes after offset
    double left_axis, center_axis, right_axis;
    center_axis = angle0To2Pi(nblink->node_tangent - angle_offset);
    left_axis = center_axis + nblink->arc_angle / 2;
    if ((0 > left_axis)|| (left_axis > 2 * M_PI))
      vcl_cout << "left axis is out of range" << vcl_endl;
    
    right_axis = center_axis - nblink->arc_angle / 2;
    if ((0 > right_axis)|| (right_axis > 2 * M_PI))
      vcl_cout << "right axis is out of range" << vcl_endl;

  //  vcl_cout << " center = " << center_axis << " , right = " << right_axis << " , left = " << left_axis << vcl_endl;
    
    // now loop through all belement in belm_list and compute their start_angle
    // and end_angle and compare with angles of the axes
    // the loop terminates when all projected belements are found
    bool is_right_found = false;
    bool is_left_found = false;
    bool is_center_found = false;

    for (BElementList::iterator it = (nblink->belm_list).begin(); 
      it != (nblink->belm_list).end(); it++ )
    {
      Point start_point = (*it)->start();
      Point end_point = (*it)->end();
      double start_angle = angle0To2Pi(_vPointPoint(node_origin, start_point) - angle_offset);
      double end_angle = angle0To2Pi(_vPointPoint(node_origin, end_point) - angle_offset);
      // since we are traversing CCW, the angle should be increasing

      // right projection point
      if ( !is_right_found && (start_angle-A_EPSILON < right_axis) && (end_angle > right_axis) && (start_angle + EP_EPSILON < end_angle)){
        (nblink->bcoors[RIGHT_BELEMENT]).belm = (*it);
        (nblink->bcoors[RIGHT_BELEMENT]).distance = (*it)->intersect_distance(&node_origin, right_axis + angle_offset);
        is_right_found = true;
      }
      // center
      if ( !is_center_found && (start_angle <= center_axis) && (end_angle > center_axis)){
        (nblink->bcoors[CENTER_BELEMENT]).belm = (*it);
        (nblink->bcoors[CENTER_BELEMENT]).distance = (*it)->intersect_distance(&node_origin, center_axis + angle_offset);
        double intersect_distance (vgl_homg_line_2d<double>& line, BElement* belm);
        is_center_found = true;
      }
      // left
      if ( !is_left_found && (start_angle < left_axis) && (end_angle+A_EPSILON > left_axis) && (start_angle + EP_EPSILON < end_angle) ){
        (nblink->bcoors[LEFT_BELEMENT]).belm = (*it);
        (nblink->bcoors[LEFT_BELEMENT]).distance = (*it)->intersect_distance(&node_origin, left_axis + angle_offset);
        is_left_found = true;
      }
      // exit loop if all three points are found
      if (is_right_found && is_center_found && is_left_found) 
        break;     
    }
    
    if (!(is_right_found && is_center_found && is_left_found)){
      vcl_cout << "could not find all three proj point" << vcl_endl;
      continue;
    }
    A3node_bnd_link_list.push_back(nblink);
  }
  return A3node_bnd_link_list;
}

// update boudary of the shock with projection 
// information of A3 nodes
// Date Jul 29 2004
// last modified: Aug 7 2004
// status: coding
void 
IShock::update_boundary_with_A3_node_info(vcl_list < A3node_bnd_link* >& nblink_list)
{
  for ( vcl_list < A3node_bnd_link* >::iterator nblink_it = nblink_list.begin();
    nblink_it != nblink_list.end(); nblink_it ++)
  {
    for (int i = 0; i <3; i++){
      BElement* belm = (*nblink_it)->bcoors[i].belm;
      A3_node_proj* to_save = new A3_node_proj;
      to_save->A3_node = (*nblink_it)->A3_node;
      to_save->btangent = (*nblink_it)->bcoors[i].btangent;
      to_save->distance = (*nblink_it)->bcoors[i].distance;
      to_save->type = (PROJ_BELEMENT_TYPE)i ;
      
      (belm->A3_node_list).push_back(to_save);
    }
  }
  return;
}
