// This is brcv/shp/dbsk2d/algo/dbsk2d_bnd_preprocess.cxx

//:
// \file



#include "dbsk2d_bnd_preprocess.h"

#include <vgl/vgl_distance.h>

#include <dbgl/algo/dbgl_distance.h>
#include <dbgl/algo/dbgl_closest_point.h>
#include <dbsk2d/dbsk2d_bnd_utils.h>


//***********************************************
// Constants
//***********************************************

double dbsk2d_bnd_preprocess::distance_tol = B_EPSILON;


//***********************************************
// Interface functions
//***********************************************




//----------------------------------------------
//: Pre-process a shock boundary
// Return false if preprocessing fails
bool dbsk2d_bnd_preprocess::
preprocess(dbsk2d_boundary_sptr boundary, bool talkative)
{
  dbsk2d_assert (boundary);
  this->set_boundary(boundary);

  if (talkative)
  {
    vcl_cout << "\nAnalyzing boundary...\n";
    vcl_cout << "Number of contours= " << boundary->preproc_contours().size() + boundary->scratch_contours().size() << vcl_endl;
    vcl_cout << "Cell partition params:\n";
    vcl_cout << "Number of rows =" << boundary->num_rows() << vcl_endl;
    vcl_cout << "Number of cols =" << boundary->num_cols() << vcl_endl;
  }

  //preprocess the whole boundary if no partitioning has been made
  if (boundary->num_cells()==0)
  {
    vcl_cout << "Preprocess the whole boundary in one shot: \n";
    bnd_edge_list edges;
    dbsk2d_bnd_utils::extract_edge_list(boundary, edges);
    return this->preprocess(edges);
  }
  // Preprocess edges in each cell separately
  else
  {
    if (talkative)
      vcl_cout << "Preprocess the boundary using partitioned cells: \n";
    for (int i=0; i< boundary->num_rows(); ++i)
      for (int j=0; j< boundary->num_cols(); ++j)
      {
        dbsk2d_bnd_cell_sptr cell = boundary->cell(i, j);
        if (talkative)
        {
          vcl_cout << "Cell row=" << i << " column= " << j << 
            "  number of edges= " << cell->num_bnd_edges() << vcl_endl;
          vcl_cout << "Preprocessing... ";
        }

        ///////////////////////////////////////////////////////////////
        bnd_edge_list edges = cell->edges();
        this->preprocess(edges);
        ///////////////////////////////////////////////////////////////
        if (talkative)
        {
          vcl_cout << "done\n";
          vcl_cout << "Number of edges after preprocessing=" << cell->edges().size() << vcl_endl;
        }
      }
  }
  boundary->remove_empty_contours();
  return true;
}


//----------------------------------------------
//: Return true if the boundary needs preprocessing
bool dbsk2d_bnd_preprocess::
need_preprocessing(dbsk2d_boundary_sptr boundary)
{
  dbsk2d_assert (boundary);
  this->set_boundary(boundary);

  bnd_edge_list edges;
  dbsk2d_bnd_utils::extract_edge_list(boundary, edges);
  return this->need_preprocessing(edges);
}



// -----------------------------------------------------------------------
//: Pre-process a group of edges
// Return false if preprocessing fails
bool dbsk2d_bnd_preprocess::
preprocess(vcl_list<dbsk2d_bnd_edge_sptr >& edges)
{

  // 0. Remove too short lines and arcs
  // 0.1 Find the short lines and arcs and convert them into points.
  // 0.2 Remove points that are part of contours
  this->remove_short_curves(edges);
  
  // 1. Separate the edges into three groups: points, edges, lines
  vcl_list<dbsk2d_bnd_edge_sptr > bnd_pts; 
  vcl_list<dbsk2d_bnd_edge_sptr > bnd_lines; 
  vcl_list<dbsk2d_bnd_edge_sptr > bnd_arcs;
  this->classify_edges(edges, &bnd_pts, &bnd_lines, &bnd_arcs);


  // 2. Process the points - merge close points
  this->merge_close_points(bnd_pts);

  // 3. Process the lines - intersection, shortening, removing short lines

  // 3.0 Merge close end-points
  vcl_list<dbsk2d_bnd_vertex_sptr > line_vertices;
  dbsk2d_bnd_utils::extract_vertex_list(bnd_lines, line_vertices);
  
  bnd_vertex_list affected_vertices;
  this->merge_close_vertices(&affected_vertices, &line_vertices);
  
  bnd_edge_list affected_lines;
  dbsk2d_bnd_utils::extract_edge_list(affected_vertices, affected_lines);
  affected_vertices.clear();
  this->remove_duplicate_lines(affected_lines);
  affected_lines.clear();
  this->remove_unlinked_edges(bnd_lines);


  // 3.1. For every pair of lines, check closest distance
  // Intersect, form X junction when necessary. 
  this->intersect_bnd_lines(&affected_lines, &bnd_lines);

  // all the new vertices are connected to at least two lines
  // so the following step does not introduce new stand-alone points
  this->remove_short_curves(affected_lines);
  this->remove_duplicate_lines(affected_lines);
  affected_lines.clear();
  this->remove_unlinked_edges(bnd_lines);
  

  // 3.2 Merge end-vertices into lines close to it
  dbsk2d_bnd_utils::extract_vertex_list(bnd_lines, line_vertices);
  bnd_edge_list all_affected;
  this->dissolve_vertices_into_curves(all_affected, bnd_lines, line_vertices);
  line_vertices.clear();
  
  this->classify_edges(all_affected, 0, &affected_lines, 0);
  this->remove_short_curves(affected_lines);
  this->remove_duplicate_lines(affected_lines);
  affected_lines.clear();
  this->remove_unlinked_edges(bnd_lines);



  // 4. Process the arcs
  
  // 4.0 Merge close end-points
  vcl_list<dbsk2d_bnd_vertex_sptr > arc_vertices;
  dbsk2d_bnd_utils::extract_vertex_list(bnd_arcs, arc_vertices);

  this->merge_close_vertices(&affected_vertices, &arc_vertices);

  bnd_edge_list affected_arcs;
  dbsk2d_bnd_utils::extract_edge_list(affected_vertices, affected_arcs);
  affected_vertices.clear();

  this->remove_duplicate_arcs(affected_arcs);
  affected_arcs.clear();
  this->remove_unlinked_edges(bnd_arcs);


  // 4.1. For every pair of arcs, check intersection
  // Form X junction when necessary. 
  this->intersect_bnd_arcs(&affected_arcs, &bnd_arcs);

  // all the new vertices are connected to at least two lines
  // so the following step does not introduce new stand-alone points
  this->remove_short_curves(affected_arcs);
  this->remove_duplicate_arcs(affected_arcs);
  affected_arcs.clear();
  this->remove_unlinked_edges(bnd_arcs);

  // 4.2 Merge end-vertices into arcs close to it
  dbsk2d_bnd_utils::extract_vertex_list(bnd_arcs, arc_vertices);
  this->dissolve_vertices_into_curves(affected_arcs, bnd_arcs, arc_vertices);
  arc_vertices.clear();

  this->remove_short_curves(affected_arcs);
  this->remove_duplicate_arcs(affected_arcs);
  affected_arcs.clear();
  this->remove_unlinked_edges(bnd_arcs);
  
    
  // 5. Process lines against arcs

  // 5.0 Merge close end-points
  dbsk2d_bnd_utils::extract_vertex_list(bnd_arcs, arc_vertices);
  dbsk2d_bnd_utils::extract_vertex_list(bnd_lines, line_vertices);

  this->merge_close_vertices(&affected_vertices, &arc_vertices, &line_vertices);

  // clean up the mess after merging the vertices
  bnd_edge_list affected_edges;
  dbsk2d_bnd_utils::extract_edge_list(affected_vertices, affected_edges);
  affected_vertices.clear();
  
  this->classify_edges(affected_edges, 0, &affected_lines, &affected_arcs);
  
  
  // removing duplicated arcs caused by merging vertices
  this->remove_duplicate_arcs(affected_arcs);
  affected_arcs.clear();
  this->remove_unlinked_edges(bnd_arcs);

  // removing duplicated lines caused by merging vertices
  this->remove_duplicate_lines(affected_lines);
  affected_lines.clear();
  this->remove_unlinked_edges(bnd_lines);


  // 5.1. For every pair of (line,arc)'s, check intersection
  // Form X junction when necessary. 
  bnd_edge_list tainted_edges;
  this->intersect_lines_against_arcs(&tainted_edges, &bnd_arcs, &bnd_lines);

  // all the new vertices are connected to at least two lines
  // so the following step does not introduce new stand-alone points
  this->remove_short_curves(tainted_edges);

  // classify the tainted edges to detect duplication
  this->classify_edges(tainted_edges, 0, &affected_lines, &affected_arcs);
  
  this->remove_duplicate_arcs(affected_arcs);
  this->remove_duplicate_lines(affected_lines);

  this->remove_arcs_duplicating_lines(affected_arcs, affected_lines);

  affected_arcs.clear();
  this->remove_unlinked_edges(bnd_arcs);

  affected_lines.clear();
  this->remove_unlinked_edges(bnd_lines);


  // 5.2 Merge end-vertices into arcs and lines close to it
  
  // arcs' vertices against lines
  dbsk2d_bnd_utils::extract_vertex_list(bnd_arcs, arc_vertices);
  this->dissolve_vertices_into_curves(affected_lines, bnd_lines, arc_vertices);
  arc_vertices.clear();
  this->remove_short_curves(affected_lines);
  this->remove_duplicate_lines(affected_lines);
  
  // lines' vertices against arcs
  dbsk2d_bnd_utils::extract_vertex_list(bnd_lines, line_vertices);
  this->dissolve_vertices_into_curves(affected_arcs, bnd_arcs, line_vertices);
  line_vertices.clear();
  this->remove_short_curves(affected_arcs);
  this->remove_duplicate_arcs(affected_arcs);

  // remove arcs that are very close to some lines
  this->remove_arcs_duplicating_lines(affected_arcs, affected_lines);

  affected_lines.clear();
  affected_arcs.clear();
  this->remove_unlinked_edges(bnd_lines);
  this->remove_unlinked_edges(bnd_lines);



  // 6. Process point-line
  // Check distance of all points against all lines.
  // If too close, remove the points
  this->remove_points_close_to_lines(bnd_pts, bnd_lines);


  // 7. Process point-arcs
  // Check distance of all points against all arcs.
  // If too close, remove the points
  this->remove_points_close_to_arcs(bnd_pts, bnd_arcs);

  // put the edges back to their original container
  edges.splice(edges.end(), bnd_pts);
  edges.splice(edges.end(), bnd_lines);
  edges.splice(edges.end(), bnd_arcs);
  return true;
}




//***********************************************
// For Debugging purpose
//***********************************************


//--------------------------------------------------------
//: Return true if the set of edges needs preprocessing
bool dbsk2d_bnd_preprocess::
need_preprocessing(vcl_list<dbsk2d_bnd_edge_sptr >& edges)
{
  // Separate the edges into three groups: points, edges, lines
  vcl_list<dbsk2d_bnd_edge_sptr > bnd_pts; 
  vcl_list<dbsk2d_bnd_edge_sptr > bnd_lines; 
  vcl_list<dbsk2d_bnd_edge_sptr > bnd_arcs;
  this->classify_edges(edges, &bnd_pts, &bnd_lines, &bnd_arcs);

  bool to_return = this->lines_need_preprocessing(bnd_lines) ||
    this->points_need_preprocessing(bnd_pts) ||
    this->point_lines_need_preprocessing(bnd_lines, bnd_pts);

  return to_return;

}



//------------------------------------------------------------------------
//: Return true if the set of points need preprocessing
bool dbsk2d_bnd_preprocess::
points_need_preprocessing(const vcl_list<dbsk2d_bnd_edge_sptr >& bnd_pts)
{
  for (bnd_edge_list::const_iterator eit1 = bnd_pts.begin();
    eit1 != bnd_pts.end(); ++eit1)
  {
    vgl_point_2d<double > p1 = (*eit1)->point1();
    bnd_edge_list::const_iterator eit2 = eit1;
    ++eit2;
    for (; eit2 != bnd_pts.end(); ++eit2)
    {
      vgl_point_2d<double > p2 = (*eit2)->point1();

      double d = vnl_math_hypot(p2.x()-p1.x(), p2.y()-p1.y());
      if (d <= dbsk2d_bnd_preprocess ::distance_tol)
        return true;
    }
  }
  return false;
}



//------------------------------------------------------------------------
//: Return true if this set of line edges need preprocessor
bool dbsk2d_bnd_preprocess::
lines_need_preprocessing(const vcl_list<dbsk2d_bnd_edge_sptr >& bnd_lines)
{

  for (bnd_edge_list::const_iterator eit1 = bnd_lines.begin();
    eit1 != bnd_lines.end(); ++eit1)
  {
    dbsk2d_bnd_edge_sptr e1 = *eit1;
    vgl_point_2d<double > p11 = e1->point1();
    vgl_point_2d<double > p12 = e1->point2();

    
    bnd_edge_list::const_iterator eit2 = eit1;
    ++eit2;
    for (; eit2 != bnd_lines.end(); ++eit2)
    {
      dbsk2d_bnd_edge_sptr e2 = *eit2;
      if (e1->share_vertex_with(e2.ptr()))
        continue;
      vgl_point_2d<double > p21 = e2->point1();
      vgl_point_2d<double > p22 = e2->point2();

      double d = dbgl_distance::lineseg_lineseg(p11, p12, p21, p22);
      if (d <= dbsk2d_bnd_preprocess ::distance_tol)
      {
        return true;
      }
    }
  }

  bnd_vertex_list line_vertices;
  dbsk2d_bnd_utils::extract_vertex_list(bnd_lines, line_vertices);

  for (bnd_edge_list::const_iterator eit = bnd_lines.begin();
    eit != bnd_lines.end(); ++eit)
  {
    dbsk2d_bnd_edge_sptr e = *eit;
    vgl_point_2d<double > p1 = e->point1();
    vgl_point_2d<double > p2 = e->point2();
    for (bnd_vertex_list::iterator vit = line_vertices.begin();
      vit != line_vertices.end(); ++vit)
    {
      dbsk2d_bnd_vertex_sptr v = *vit;
      if (e->is_endpoint(v.ptr())) continue;
      vgl_point_2d<double > pt = v->point();
      double d = vgl_distance_to_linesegment(p1.x(), p1.y(), 
        p2.x(), p2.y(), pt.x(), pt.y());

      if (d <= dbsk2d_bnd_preprocess ::distance_tol)
        return true;
    }
  }

  return false;
}





//----------------------------------------------------------------------
//: Return true if this set of line edges need preprocessor
bool dbsk2d_bnd_preprocess::
point_lines_need_preprocessing(const vcl_list<dbsk2d_bnd_edge_sptr >& bnd_lines,                               const vcl_list<dbsk2d_bnd_edge_sptr >& bnd_pts)
{
  for (bnd_edge_list::const_iterator line_it = bnd_lines.begin();
    line_it != bnd_lines.end(); ++line_it)
  {
    vgl_point_2d<double > p1 = (*line_it)->point1();
    vgl_point_2d<double > p2 = (*line_it)->point2();

    for (bnd_edge_list::const_iterator pt_it = bnd_pts.begin();
      pt_it != bnd_pts.end(); ++pt_it)
    {
      vgl_point_2d<double > pt = (*pt_it)->point1();
      double d = vgl_distance_to_linesegment(p1.x(), p1.y(), 
        p2.x(), p2.y(), pt.x(), pt.y());

      if (d <= dbsk2d_bnd_preprocess ::distance_tol)
        return true;
    }
  }
  return false;
}



//--------------- OLD CODE ----------------------------------------


////**************************************************************//
////             PREPROCESSING FUNCTIONS
////**************************************************************//

//void dbsk2d_boundary::
//PreProcessBoundary (void)
//{
//  vcl_cout<< "Boundary Preprocessing\n";
//
//  dbsk2d_ishock_bpoint *bp1, *bp2;
//  belm_map_iter i, j, temp;
//  belm_vector BElmToBeDeleted;
//
//  //1)C(N,2): test if e_pt() in the same position
//  //  The iterator temp is required for immediate deletion!
//   for (i=BElmList.begin(); i!=BElmList.end(); i++) {
//    j=i; j++;
//    switch ((i->second)->type()) {
//    case BPOINT:
//      bp1 = (dbsk2d_ishock_bpoint*)(i->second);
//      for (;j!=BElmList.end(); j++) {
//        switch ((j->second)->type()) {
//        case BPOINT:
//          bp2 = (dbsk2d_ishock_bpoint*)(j->second);
//          if (_BisEqPoint(bp1->pt(), bp2->pt())) {
//            //merge bp2 into bp1 and delete bp2
//            mergeDuplicatepoints(bp1, bp2);
//            temp=j; temp++;
//            //add this point to the deleted point list
//            BElmToBeDeleted.push_back (bp2);
//            delBElement(bp2);
//            temp--; j=temp;
//          }
//        default: break;
//        }//end switch
//      }//end for j
//      default: break;
//      }//end switch
//   }//end for i
//
//  //2)Go through the list again to remove them from BElmList
//  belm_vector::iterator it = BElmToBeDeleted.begin();
//  for (; it!=BElmToBeDeleted.end(); ++it) {
//    BElmList.erase ((*it)->id());
//  }
//
//  //3)
//  _bIsPreprocessingNeeded = false;
//}
//
//void dbsk2d_boundary::
//PreProcessBPoint (dbsk2d_ishock_bpoint* bp)
//{
//  dbsk2d_ishock_bpoint *bp1;
//  belm_map_iter i;
//
//   //1)C(N,2): test if e_pt() in the same position
//   for (i=BElmList.begin(); i!=BElmList.end(); i++) {
//      switch ((i->second)->type()) {
//    case BPOINT:
//      bp1 = (dbsk2d_ishock_bpoint*)(i->second);
//      if (bp1->id()==bp->id())
//        continue;
//      //======================================
//      //Merge the EndPts of a Corner
//      if (_BisEqPoint(bp->pt(), bp1->pt())) {
//        //merge bp2 into bp1 and delete bp2
//        mergeDuplicatepoints (bp, bp1);
//        i++;
//        delBElement(bp1);
//        i--;
//      }
//      default: break;
//    }//end switch
//   }//end for i
//}
//
//void dbsk2d_boundary::
//PreProcessGUIElement (dbsk2d_ishock_belm* GUIElm)
//{
//  switch (GUIElm->type()) {
//  case BPOINT:
//    PreProcessBPoint ((dbsk2d_ishock_bpoint*)GUIElm);
//  break;
//  case BLINE:
//    PreProcessBPoint (((dbsk2d_ishock_bline*)GUIElm)->s_pt());
//    PreProcessBPoint (((dbsk2d_ishock_bline*)GUIElm)->e_pt());
//  break;
//  case BARC:
//    PreProcessBPoint (((dbsk2d_ishock_barc*)GUIElm)->s_pt());
//    PreProcessBPoint (((dbsk2d_ishock_barc*)GUIElm)->e_pt());
//  break;
//  default: break;
//  }
//}
//
//void dbsk2d_boundary::
//PreProcessBoundaryForEdgeInput(double position_accuracy, double angle_accuracy,
//    double operator_width, double operator_length)
//{
//  //this function is trying to weed out redundant edges that subpixel edge detectors like
//  //L/L often output. These edges unnecessarily increase the complexity of the grouping
//  //process and often times crashes the DT code
//
//  //0. Remove edge of the image responses.
//  //1. if two edges are really close, then replace them with an edge that is in between them
//  //2. Definition of really close:
//  //      a. if tangents are within angle_accuracy, and
//  //        i. if they are seperated in the normal direction by less than the
//  //          position accuracy
//  //        ii. if they are seperated in the tangential direction by less than
//  //          the position accuracy
//  //3. If an edge is close to two edges that are not close to one another then remove it.
//  //4. Definition of close:
//  //      a. If the tangents agree within the angle inaccuracy and the edge in the
//  //        middle is less than half the operator length away from both the edges.
//
//  //To make it a less than O(N^2) operation, let's bin the points first
//
//  vcl_cout << "**********************************************" <<vcl_endl;
//  vcl_cout << "Edge Results Preprocessing for DT" << vcl_endl;
//  vcl_cout << "**********************************************" <<vcl_endl;
//
//  //long sec1 = clock();
//
//  vcl_list<int> ** ImgBins;
//  ImgBins = new vcl_list<int> *[1001];
//  for (int j=0; j<1001;j++)
//    ImgBins[j] = new vcl_list<int>[1001];
//
//  //long sec2 = clock();
//
//  //vcl_cout << "init Bins Time: "<<sec2-sec1<<" msec."<<vcl_endl;
//  vcl_cout << "Binning the Edge points" << vcl_endl;
//
//  //keep the elements to delete in this vcl_set
//  vcl_set<int> elmsToDel;
//
//  belm_map_iter i=BElmList.begin();
//   for (; i!=BElmList.end(); i++) {
//    dbsk2d_ishock_bpoint* bp = (dbsk2d_ishock_bpoint*)(i->second);
//    int x =  (int)vcl_floor (bp->pt().x());
//    int y =  (int)vcl_floor (bp->pt().y());
//
//    if (x>=0 && y>=0){
//      ImgBins[x][y].push_back(bp->id());
//
//      //if the point is close to another grid location by
//      //less than the position accuracy, include it in the
//      //neighboring grids too
//
//      if ((bp->pt().x() - x)< position_accuracy && (x-1)>=0){
//        ImgBins[x-1][y].push_back(bp->id());
//      }
//
//      if (x+1 - (bp->pt().x() )< position_accuracy && (x+1)<1001){
//        ImgBins[x+1][y].push_back(bp->id());
//      }
//
//      if ((bp->pt().y() - y)< position_accuracy && (y-1)>=0){
//        ImgBins[x][y-1].push_back(bp->id());
//      }
//
//      if ((y+1 - bp->pt().y())< position_accuracy && (y+1)<1001){
//        ImgBins[x][y+1].push_back(bp->id());
//      }
//    }
//    else
//      elmsToDel.insert(bp->id());
//  }
//  //long sec3 = clock();
//
//  vcl_cout << "Done Binning." << vcl_endl;
//  //vcl_cout << "Fill Bins Time: "<<sec3-sec2<<" msec."<<vcl_endl;
//
//  //Now the search space should be limited to a neighborhood of 9 bins only
//  //let's do each bin seperately first
//
//  for (int x=0; x<1000; x++){
//    for (int y=0; y<1000; y++){
//      if (ImgBins[x][y].size()>1){
//        vcl_list<int>::iterator m, n;
//        for (m = ImgBins[x][y].begin(); m!=ImgBins[x][y].end(); m++){
//          n=m; n++;
//          for (;n!=ImgBins[x][y].end(); n++){
//            dbsk2d_ishock_bpoint* bp1 = (dbsk2d_ishock_bpoint*)BElmList[(*m)];
//            dbsk2d_ishock_bpoint* bp2 = (dbsk2d_ishock_bpoint*)BElmList[(*n)];
//
//            double a1 = bp1->tangent();
//            double a2 = bp2->tangent();
//
//            //do the angles agree?
//            if (_dot(a1,a2) > vcl_cos(2*angle_accuracy)){
//              //are they really close?
//              double dist = _distPointPoint(bp1->pt(), bp2->pt());
//              if (dist <= position_accuracy){//dist <operator_length/2
//                //hide the second point
//                //they should both be replaced by another point really
//                elmsToDel.insert(bp2->id());
//              }
//            }
//          }
//        }
//      }
//    }
//  }
//
//  //long sec4 = clock();
//  //vcl_cout << "Deciding duplicates Time: "<<sec4-sec3<<" msec."<<vcl_endl;
//
//  //delete all the marked elements
//  vcl_set<int>::iterator k=elmsToDel.begin();
//   for (; k!=elmsToDel.end(); k++) {
//    int ID = *k;
//    //dbsk2d_ishock_bpoint* curBElm = (dbsk2d_ishock_bpoint*)BElmList[ID];
//
//    //curBElm->_bSomething = false; //for visualizing
//    //update_list.insert(id_belm_pair(curBElm->id(), curBElm));
//
//    delBElement(BElmList[ID]);
//  }
//
//  //long sec5 = clock();
//  //vcl_cout << "Deleting duplicates Time: "<<sec5-sec4<<" msec."<<vcl_endl;
//
//  //delete the bins
//  for (int x=0; x<1001;x++){
//    for (int y=0;y<1001;y++){
//      ImgBins[x][y].clear();
//    }
//    delete []ImgBins[x];
//  }
//  delete []ImgBins;
//
//  //long sec6 = clock();
//  vcl_cout << "Done Cleaning." << vcl_endl;
//
//}
//


