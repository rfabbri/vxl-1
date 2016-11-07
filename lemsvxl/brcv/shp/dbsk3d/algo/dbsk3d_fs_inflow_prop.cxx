//: dbsk3d/dbsk3d_fs_inflow_prop.cxx
//  Surface In-Flow Generator Propagation on the Shocks.
//  MingChing Chang
//  June 26, 2007        Creation.

#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <dbsk3d/algo/dbsk3d_fs_xform.h>
#include <dbsk3d/algo/dbsk3d_fs_inflow_prop.h>

void dbsk3d_fs_inflow_prop::compute_surface_inflow ()
{
  vul_printf (vcl_cout, "compute_surface_inflow(): \n");

  surface_inflow_gene_init ();

  surface_inflow_gene_prop ();

}

void dbsk3d_fs_inflow_prop::surface_inflow_gene_init ()
{
  //The priority queue of shock curve or shock sheet elements.  
  PQ_.clear();

  //Detect flow_type of all fs_edges.
  vcl_map<int, dbmsh3d_edge*>::iterator lit = fs_mesh_->edgemap().begin();
  for (; lit != fs_mesh_->edgemap().end(); lit++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*lit).second;
    FE->detect_flow_type();
  }

  //Detect flow_type of all fs_faces.
  //Put all A12 shock sheets containing valid A12-2 shock source points 
  //to a priority queue Q ordered by their A12-2 radius.
  vcl_map<int, dbmsh3d_face*>::iterator pit = fs_mesh_->facemap().begin();
  for (; pit != fs_mesh_->facemap().end(); pit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*pit).second;
    FF->detect_flow_type();
    if (FF->flow_type() == FF_FT_I_A12_2) {
      double A122_radius = FF->mid_pt_time();
      add_FF_to_PQ (A122_radius, FF);
    }
  }

}

void dbsk3d_fs_inflow_prop::surface_inflow_gene_prop ()
{
  //Loop through elements in PQ_ until finish.
  vcl_multimap<double, vispt_elm*>::iterator it = PQ_.begin();
  while (it != PQ_.end()) {
    vispt_elm* E = (*it).second;
    PQ_.erase (it); //remove E from queue.

    //Detect type of element in (*it).
    if (dbsk3d_fs_face* FF = dynamic_cast<dbsk3d_fs_face*>(E)) {
      //Case I: E is a fs_face FF
      assert (FF->b_visited() == false);
      if (FF->flow_type() == FF_FT_I_A12_2) {
        //FF: Type I A12 shock sheet containing its A12-2 source.
        //Send FF's 2 generators to all incident bi-directional A13 shock curves,
        //and put them into PQ.        
        dbmsh3d_halfedge* HE = FF->halfedge();
        do {
          dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();      
          if (FE->flow_type() == '1' || FE->flow_type() == '2') {
            //FE bidirect, including the fuzzy case
            FE->check_add_asgn_G (FF->genes(0));
            FE->check_add_asgn_G (FF->genes(1));
            double r = FE->circum_cen_time ();
            add_FE_to_PQ (r, FE);
          }
          HE = HE->next();
        }
        while (HE != FF->halfedge());

        //Set FF to be visited.
        FF->set_visited (true);
      }
      else {
        //FF: Type II A12 shock sheet starting from its A13-3 relay.
        assert (FF->flow_type() == FF_FT_II_A13_3);
        
        //Send FF's all inflow generators to all incident bi-directional A13 shock curves
        //(skipping the fs_edge containing A13-3 relay) and put them into PQ. 
        dbmsh3d_halfedge* HE = FF->halfedge();
        do {
          dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();    
          //Skip shock curve containing the incoming A13-3 relay, which is already visited.
          if (FE->b_visited() == false) {
            if (FE->flow_type() == '1' || FE->flow_type() == '2') {
              //FE bidirect, including the fuzzy case
              FE->check_add_asgn_G (FF->genes(0));
              FE->check_add_asgn_G (FF->genes(1));
              double r = FE->circum_cen_time ();
              add_FE_to_PQ (r, FE);
            }
          }
          HE = HE->next();
        }
        while (HE != FF->halfedge());
        
        //Set FF to be visited.
        FF->set_visited (true); 
      }
    }
    else if (dbsk3d_fs_edge* FE = dynamic_cast<dbsk3d_fs_edge*>(E)) {
      //Case II: E is a fs_edge FE
      //There can be multiple identical FE (with different r's) in PQ.
      //We only process FE if it's visited and all of its incoming FF's are visited.
      if (FE->b_visited() == true) {
        it = PQ_.begin(); //Skip FE. Continue on next element in PQ_
        continue;
      }

      if (FE->flow_type() == FE_FT_I_A13_2_ACUTE) {
        //Type I acute A13 shock curve containing its A13-2 saddle.
        //Propagate FE if all incident FF's are visited.
        if (FE->all_incident_Fs_visited()) {
          //Send FE's all in-flow generators from all incident A12 shock sheets 
          //(assert all visited) to FE. 
          dbmsh3d_halfedge* HE = FE->halfedge();
          do {
            dbsk3d_fs_face* FF = (dbsk3d_fs_face*) HE->face();
            assert (FF->b_visited());
            //Get all incoming genes of FF and add to FE
            vcl_set<dbmsh3d_vertex*> incomingG;
            FF->get_incoming_Gs (incomingG);
            vcl_set<dbmsh3d_vertex*>::iterator it = incomingG.begin();
            for (; it != incomingG.end(); it++) {
              dbmsh3d_vertex* G = (*it);
              FE->check_add_asgn_G (G);
            }
            HE = HE->pair();
          }
          while (HE != FE->halfedge());
          
          //Set FE to be visited.
          FE->set_visited (true); 
        }
      }
      else {
        //Type II obtuse A13 shock curve with outside A13-3 relay.
        assert (FE->flow_type() == FE_FT_II_A13_3_OBT);
        dbsk3d_fs_face* newP = FE->type_II_get_outgoing_P ();

        //Propagate FE if all incoming FF's are visited.        
        if (FE->all_incident_Fs_visited_except (newP)) {
          //Send all in-flow generators from all incoming A12 shock sheets 
          //(assert all visited) to FE and to the outgoing (new) A12 shock sheet newP. 
          dbmsh3d_halfedge* HE = FE->halfedge();
          do {
            dbsk3d_fs_face* FF = (dbsk3d_fs_face*) HE->face();
            if (FF != newP) {
              assert (FF->b_visited());
              //Get all incoming genes of FF and add to FE
              vcl_set<dbmsh3d_vertex*> incomingG;
              FF->get_incoming_Gs (incomingG);
              vcl_set<dbmsh3d_vertex*>::iterator it = incomingG.begin();
              for (; it != incomingG.end(); it++) {
                dbmsh3d_vertex* G = (*it);
                FE->check_add_asgn_G (G);
              }
            }
            HE = HE->pair();
          }
          while (HE != FE->halfedge());

          //No need to send all in-flow generators from FE to newP (they are implicit).
          //Put newP into Q. 
          double r = FE->circum_cen_time ();
          add_FF_to_PQ (r, newP);
          
          //Set FE to be visited.
          FE->set_visited (true); 
        }
      }
    }
    else 
      assert (0);
   
    it = PQ_.begin(); //Next element in PQ_
  }
}

//Save the reconstructed surface mesh.
void dbsk3d_fs_inflow_prop::build_surface_mesh ()
{
  //Loop through all fs_edges and build trianlges if asgn_gene > 3.
  vcl_map<int, dbmsh3d_edge*>::iterator lit = fs_mesh_->edgemap().begin();
  for (; lit != fs_mesh_->edgemap().end(); lit++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*lit).second;
    if (FE->n_asgn_Gs() >= 3) {
      //create FE's Delaunay triangle as a surface mesh face.
      dbmsh3d_face* F = L_add_Delaunay_facet (bnd_mesh(), FE);
    }
  }
}

