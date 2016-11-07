//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/dbmsh3d_vis_fmm.cxx
//  MingChing Chang
//  Aug. 09, 2005.

#include <dbmsh3d/gdt/gdt_manager.h>
#include <dbmsh3d/gdt/gdt_mesh.h>
#include <dbmsh3d/gdt/gdt_path.h>

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_edge.h>
#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_face.h>

#include <dbmsh3d/gdtvis/gdtvis_path_iso_contour.h>

#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoDrawStyle.h>

SoSeparator* gdt_draw_result (gdt_manager* gdt_manager, 
                              int n_query_contours, 
                              int n_verbose,
                              float lineWidth)
{  
  SoSeparator* group = new SoSeparator;

  //: for each psource_[i], get the farthest vertex of it with a shortest path
  double farthest_tau;
  gdt_interval* I = gdt_manager->find_farthest_vertex (NULL, farthest_tau);
  assert (I);
  assert (I->is_dege() == false);

  gdt_interval* farthest_I = I;

  gdt_path path_farthest_to_source;
  farthest_I->I_get_gdt_path (farthest_tau, &path_farthest_to_source);
  if (n_verbose>1)
    path_farthest_to_source.print_path ();

  //: draw the path from the farthest_vertex to its closest source.
  gdt_draw_gdt_path (group, &path_farthest_to_source, SbColor (1.0f, 0.2f, 1.0f), lineWidth+2);
  ///group->addChild (vis);

  //: draw a iso-contour line for each query
  float total_length = (float) path_farthest_to_source.get_length();
  float step_dist = (float) (total_length / n_query_contours);

  gdt_draw_iso_contours (group, gdt_manager->gdt_mesh(), n_query_contours, 
                         total_length, step_dist, lineWidth);

  return group;
}

void gdt_draw_iso_contours (SoSeparator* group, dbmsh3d_mesh* gdt_mesh, 
                            int n_query_contours, float total_length,
                            float step_dist, float lineWidth)
{
  vcl_map<int, dbmsh3d_face*>::iterator it = gdt_mesh->facemap().begin();
  for (; it != gdt_mesh->facemap().end(); it++) {
    dbmsh3d_face* cur_face = (*it).second;

    //: loop through each I-face
    //  that's, for each edge, for each interval on the face
    dbmsh3d_halfedge* cur_he = cur_face->halfedge();
    do {
      dbmsh3d_gdt_edge* cur_edge = (dbmsh3d_gdt_edge*) cur_he->edge();

      //: loop through each interval
      vcl_map<double, gdt_ibase*>::iterator iit = cur_edge->interval_section()->I_map()->begin();
      for (; iit != cur_edge->interval_section()->I_map()->end(); iit++) {
        gdt_interval* I = (gdt_interval*) (*iit).second;
        if (I->is_dege() || !I->is_on_face(cur_face))
          continue;

        //: now consider the query_dist of interval I on cur_face
        double min_dist, max_dist;
        I->get_min_max_dist_I_face (min_dist, max_dist);

        //: for each valid query_dist on this face
        double query_dist = 0;
        for (int i=1; i<n_query_contours; i++) {
          query_dist += step_dist;
          double gdt_dist = total_length - query_dist;

          //: need to improve efficiency here!!
          if (min_dist < gdt_dist && gdt_dist < max_dist)
            gdt_draw_isocontour_I_face (group, I, cur_face, gdt_dist, lineWidth);
        }

      } //end for each I
      cur_he = cur_he->next();
    } //end for each edge
    while (cur_he != cur_face->halfedge());  
  } //end for each face
}


//: for the given interval I on the given face f,
//  draw the iso-contour line for the given query_dist
void gdt_draw_isocontour_I_face (SoSeparator* group, 
                                 gdt_interval* I, dbmsh3d_face* cur_face, 
                                 double query_dist, float lineWidth)
{
  //: the two endpoints of the arc to be drawn.
  vgl_point_3d<double> spt, ept;  
  bool b_spt_valid = false;
  bool b_ept_valid = false;

  //: the edge that endpoints are on
  dbmsh3d_gdt_edge* spt_on_edge = NULL;
  
  //: two endpoinits of the interval I
  vgl_point_3d<double> stau_pt = I->_point_from_tau (I->stau());
  vgl_point_3d<double> etau_pt = I->_point_from_tau (I->etau());

  //: local distance to the pseudo-source !!
  double stau_local_d = I->_get_d_at_tau (I->stau());
  double etau_local_d = I->_get_d_at_tau (I->etau());
  double query_local_d = query_dist - I->psrc()->dist ();

  //: the spt has to be on the intersection of stau's gdt_path and the face.
  if (stau_local_d >= query_local_d) {    
    //: if prevI exists, query_local_d has to be greater than prevI's local_d
    if (I->prevI()) {
      double prev_stau = I->get_prev_tau (I->stau());
      double prev_stau_local_d = I->prevI()->_get_d_at_tau (prev_stau);
      if (query_local_d > prev_stau_local_d) {
        //: try to locate spt between stau_pt and prev_stau_pt using linear interpolation
        vgl_point_3d<double> prev_stau_pt = I->prevI()->_point_from_tau (prev_stau);
        //the two distances between I and prev_I
        double stau_prev_d = stau_local_d - prev_stau_local_d;
        double query_prev_d = query_local_d - prev_stau_local_d;

        if (_eqT(stau_prev_d, 0))
          spt = stau_pt;
        else
          spt = prev_stau_pt + (stau_pt - prev_stau_pt) / stau_prev_d * query_prev_d;
        b_spt_valid = true;
      }
    }
    else {
      //: try to locate spt on the path of stau_
      //  linear interpolation of stau_pt and psrc with the query_dist
      assert (!_eqT(stau_local_d, 0));
      spt = I->psrc()->pt() + (stau_pt - I->psrc()->pt()) / stau_local_d * query_local_d;
      b_spt_valid = true;
    }
  }
  
  //: the ept has to be on the intersection of etau's gdt_path and the face.
  if (etau_local_d >= query_local_d) {    
    //: if prevI exists, query_local_d has to be greater than prevI's local_d
    //unused bool prev_ok = true;
    if (I->prevI()) {
      double prev_etau = I->get_prev_tau (I->etau());
      double prev_etau_local_d = I->prevI()->_get_d_at_tau (prev_etau);
      if (query_local_d > prev_etau_local_d) {
        //: try to locate ept between etau_pt and prev_etau_pt using linear interpolation
        vgl_point_3d<double> prev_etau_pt = I->prevI()->_point_from_tau (prev_etau);
        //the two distances between I and prev_I
        double etau_prev_d = etau_local_d - prev_etau_local_d;
        double query_prev_d = query_local_d - prev_etau_local_d;

        if (_eqT(etau_prev_d, 0))
          ept = etau_pt;
        else
          ept = prev_etau_pt + (etau_pt - prev_etau_pt) / etau_prev_d * query_prev_d;
        b_ept_valid = true;
      }
    }
    else {
      //: try to locate ept on the path of etau_
      //  linear interpolation of etau_pt and psrc with the query_dist
      assert (!_eqT(etau_local_d, 0));
      ept = I->psrc()->pt() + (etau_pt - I->psrc()->pt()) / etau_local_d * query_local_d;
      b_ept_valid = true;
    }
  }

  //:A) If spt not valid, try to locate it on I
  double tau[2];
  if (b_spt_valid == false) {
    I->query_taus_from_dist (query_dist, tau[0], tau[1]);
    for (unsigned int i=0; i<2; i++) {
      if (tau[i] != -1) {
        spt = I->_point_from_tau (tau[i]);
        b_spt_valid = true;
        spt_on_edge = I->edge();
        break;
      }
    }
  }

  //:B) If spt not valid, try to locate it on prev_I
  if (b_spt_valid == false && I->prevI()) {
    I->prevI()->query_taus_from_dist (query_dist, tau[0], tau[1]);

    //1)If both solutions are valid on prev_I, select the one closer to stau_pt
    if (tau[0]!=-1 && tau[1]!=-1) {
      vgl_point_3d<double> spt0 = I->prevI()->_point_from_tau (tau[0]);
      vgl_point_3d<double> spt1 = I->prevI()->_point_from_tau (tau[1]);
      if (vgl_distance (spt0, stau_pt) < vgl_distance (spt1, stau_pt))
        spt = spt0;
      else
        spt = spt1;
      b_spt_valid = true;
      spt_on_edge = I->prevI()->edge();
    }
    else { //2)Select the valid one
      for (unsigned int i=0; i<2; i++) {
        if (tau[i] != -1) {
          spt = I->prevI()->_point_from_tau (tau[i]);
          b_spt_valid = true;
          spt_on_edge = I->prevI()->edge();
          break;
        }
      }
    }
  }

  //:C) If ept not valid, try to locate it on I
  if (b_ept_valid == false) {
    //Now that spt is picked, pick the ept != spt
    if (spt_on_edge != I->edge()) {
      I->query_taus_from_dist (query_dist, tau[0], tau[1]);
      for (unsigned int i=0; i<2; i++) {
        if (tau[i] != -1) {
          ept = I->_point_from_tau (tau[i]);
          b_ept_valid = true;
          break;
        }
      }
    }
  }

  //:D) If ept not valid, try to locate it on prev_I
  //    Now that spt is picked, pick the ept != spt
  if (b_ept_valid == false && I->prevI() && spt_on_edge != I->prevI()->edge()) {
    I->prevI()->query_taus_from_dist (query_dist, tau[0], tau[1]);

    //1)If both solutions are valid on prev_I, select the one closer to etau_pt
    if (tau[0]!=-1 && tau[1]!=-1) {
      vgl_point_3d<double> ept0 = I->prevI()->_point_from_tau (tau[0]);
      vgl_point_3d<double> ept1 = I->prevI()->_point_from_tau (tau[1]);
      if (vgl_distance (ept0, etau_pt) < vgl_distance (ept1, etau_pt))
        ept = ept0;
      else
        ept = ept1;
      b_ept_valid = true;
    }
    else { //2)Select the valid one
      for (unsigned int i=0; i<2; i++) {
        if (tau[i] != -1) {
          ept = I->prevI()->_point_from_tau (tau[i]);
          b_ept_valid = true;
          break;
        }
      }
    }
  }
  
  //: draw line from spt to ept
  if (b_spt_valid && b_ept_valid) {
    //: draw
    SbColor color = SbColor (0.5f, 1.0f, 0.3f);

    SoDrawStyle* ds = new SoDrawStyle;
    ds->setName (SbName("iso_contour_style"));
    ds->lineWidth.setValue (lineWidth);
    group->addChild (ds);
  
    SoBaseColor *basecolor = new SoBaseColor;
    basecolor->rgb = color;
    group->addChild (basecolor);

    draw_line_geom (group, spt, ept);
  }
}

void gdt_draw_gdt_path (SoSeparator* group, gdt_path* gdt_path,
                        SbColor color, float lineWidth)
{
  vcl_vector<vgl_point_3d<double> > vertices;
  for (unsigned int i=0; i<gdt_path->I_tau_pairs()->size(); i++) {
    vgl_point_3d<double> pt = gdt_path->get_point (i);
    vertices.push_back (pt);
  }

  group->addChild (draw_polyline (vertices, lineWidth, color));
}

